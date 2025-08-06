
#include "ConverterJSON.h"
#include "InvertedIndex.h"

#include <utility>

mutex dict_access;

InvertedIndex::InvertedIndex( const ConverterJSON&  cvr ) : _convert( cvr ) {};

void InvertedIndex::indexDocument( const pair<vector<string>, const size_t>& texts_input )
{
    unordered_map<string, size_t> words;
    unordered_map<string, size_t>::iterator itw;

    for ( auto& index : texts_input.first )
        words.insert( pair( index, 0 ) );

    for( itw = words.begin(); itw != words.end(); ++itw )
        itw->second = ranges::count( texts_input.first, itw->first );

    dict_access.lock();
    for ( itw = words.begin(); itw != words.end(); ++itw )
    {
        bool check = true;
        for ( it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it )
        {
            if ( it->first == itw->first )
            {
                it->second.push_back( { texts_input.second, itw->second } );
                check = false;
            }
        }
        if ( check || freq_dictionary.empty() )
        {
            vector<Entry>entry{ { texts_input.second, itw->second } };
            freq_dictionary.insert( pair( itw->first, entry ) );
        }
    }
    dict_access.unlock();
}

void InvertedIndex::updateDocumentBase( vector<string> input_docs )
{
    auto thread_Dict = [ this ]( const string& doc_page, const size_t& doc_id )
    {
        if ( _convert.getDocuments().empty() )
            indexDocument( docOutVector( doc_page, doc_id ) );
        else
            indexDocument( docOutFile( doc_page, doc_id ) );
    };

    if ( !ConverterJSON::checkDocBase() || _convert.getDBUpdate()  || _convert.getDocuments().empty() )
    {
        vector<thread> thread_vec;
        if ( input_docs.size() >= 1000 ) cerr << "1000+ files" << endl;
        for ( auto doc_id = 0; doc_id < input_docs.size() && doc_id < 1000; ++doc_id )
            thread_vec.emplace_back (thread_Dict, input_docs[ doc_id ], doc_id );
        for ( auto & thread : thread_vec ) thread.join();

        if ( ! _convert.getDocuments().empty() ) setDocBase();
    }
    else getDocBase();
}

pair<vector<string>, const size_t> InvertedIndex::docOutVector( const string& doc_page, const size_t& doc_id )
{
    string word;
    vector<string> texts_input;

    for ( auto simbol = 0; simbol < doc_page.length(); ++simbol )
    {
        if ( doc_page[ simbol ] != ' ' )
        {
            if ( ! validSimbol( doc_page[ simbol ] ) )
            {
                char temp = doc_page[ simbol ];
                letterCase( temp );
                word += temp;
            }

            if ( simbol != doc_page.length() - 1 )
                continue;
        }
        texts_input.push_back( word );
        word.clear();
    }
    return pair( texts_input, doc_id );
}

pair<vector<string>, const size_t> InvertedIndex::docOutFile(const string& doc_page, const size_t& doc_id)
{
    string word;
    vector<string> texts_input;

    if ( ifstream Check( doc_page.c_str(), ios::in ); !Check )
        cerr << "file_" << to_string( doc_id ) << " not found" << endl;

    ifstream Document( doc_page.c_str(), ios::in );
    while ( Document >> word )
    {
        string buffer;
        auto upload = [ &texts_input ]( string& buffer )
        {
            if ( ! buffer.empty() ) texts_input.push_back( buffer );
            buffer.clear();
        };

        for ( auto simbol = 0; simbol < word.size(); ++simbol )
        {
            letterCase( word[ simbol ] );

            if ( word[ simbol ] == '\'' && word[ simbol + 1 ] == 's' )
            {
                ++simbol;
                upload( buffer );
            }
            else if ( validSimbol( word[ simbol ] ) )
                upload( buffer );
            else
                buffer += word[ simbol ];
        }
        if ( texts_input.size() > 1000 || buffer.length() > 100 )
        {
            cerr << "file_" << to_string( doc_id );
            buffer.length() > 100
            ? cerr << " wideword: " << buffer << " \n"
            : cerr << ": lots of words" << " \n";
            texts_input.clear();
            break;
        }
        upload( buffer );
    }
    return pair(texts_input, doc_id);
}

void InvertedIndex::letterCase( char& value )
{
    const string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for ( auto& simbol : caps )
        if ( value == simbol )
        {
            value = tolower( value );
            break;
        }
}

bool InvertedIndex::validSimbol(const char& value )
{
    const string simbols = ".,!?-:;()'\"\\|/{}[]<>_+=№@#$%^&*~`1234567890";
    for ( auto& simbol : simbols )
        if ( value == simbol ) return true;
    return false;
}

void InvertedIndex::setDocBase()
{
    map<string, vector<pair<size_t, size_t>>> doc_base;
    for ( it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it )
    {
        vector<pair<size_t, size_t>> word_cnt;
        for ( auto&[ doc_id, count ] : it->second )
            word_cnt.emplace_back(  doc_id, count );
        doc_base.insert(pair(it->first, word_cnt));
    }
    ConverterJSON::setDocBaseJSON( doc_base );
}

void InvertedIndex::getDocBase()
{
    map doc_base( ConverterJSON::getDocBase() );
    map<string, vector<pair<size_t, size_t>>>::iterator it;
    for ( it = doc_base.begin(); it != doc_base.end(); ++it)
    {
        vector<Entry> entry;
        for ( auto& [ doc_id, count ] : it->second )
            entry.push_back( {  doc_id, count } );
        freq_dictionary.insert(pair(it->first, entry));
    }
}

vector<Entry> InvertedIndex::getWordCount( const string& word )
{
    vector<Entry> entry;
    for ( it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it )
    {
        if ( word == it->first )
            for ( auto& [ doc_id, count ] : it->second )
                entry.push_back( {  doc_id, count } );
    }
    return entry;
}