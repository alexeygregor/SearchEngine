
#include "ConverterJSON.h"
#include "InvertedIndex.h"

#include <utility>

mutex dict_access;

InvertedIndex::InvertedIndex( const ConverterJSON&  cvr ) : _convert( cvr ) {};

void InvertedIndex::indexDocument( const size_t& doc_id, vector<string>& texts_input )
{
    while ( ! texts_input.empty() )
    {
        size_t count = 0;
        vector<Entry> entry;

        string word = texts_input[ 0 ];
        for ( auto i = 0; i < texts_input.size(); ++i )
        {
            if ( word == texts_input[ i ] )
            {
                texts_input.erase( texts_input.begin() + i );
                ++count;
                --i;
            }
        }
        entry.push_back( { doc_id, count } );

        dict_access.lock();
        bool check = true;
        for ( it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it )
        {
            if ( it->first == word )
            {
                it->second.push_back( { doc_id, count } );
                check = false;
            }
        }
        if ( check || freq_dictionary.empty() )
            freq_dictionary.insert( pair( word, entry ) );
        dict_access.unlock();
    }
    ++thread_count;
}

void InvertedIndex::updateDocumentBase( vector<string>& input_docs )
{
    freq_dictionary.clear();

    auto thread_Dict = [ this ]( const string& doc_page, const size_t& doc_id )
    {
        string word;
        vector<string> texts_input;

        if( ! _convert.getDocuments().empty() )
        {
            ifstream Check( doc_page.c_str(), ios::in );
            Check >> word;
            if( word.empty() )
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

                for ( auto i = 0; i < word.size(); ++i )
                {
                    letterCase( word[ i ] );

                    if ( word[ i ] == '\'' && word[ i + 1 ] == 's' )
                    {
                        ++i;
                        upload( buffer );
                    }
                    else if ( validSimbol( word[ i ] ) )
                        upload( buffer );
                    else
                        buffer += word[ i ];
                }
                if ( texts_input.size() > 1000 || buffer.length() > 10 )
                {
                    cerr << "file_" << to_string( doc_id );
                    buffer.length() > 10
                    ? cerr << " wideword: " << buffer << " \n"
                    : cerr << ": lots of words" << " \n";
                    texts_input.clear();
                    break;
                }
                upload( buffer );
            }
        }
        else
        {
            for( auto i = 0; i < doc_page.length(); ++i )
            {
                if ( doc_page[ i ] != ' ' )
                {
                    word += doc_page[ i ];
                    if ( i != doc_page.length() - 1 )
                        continue;
                }
                texts_input.push_back( word );
                word.clear();
            }
        }

        suffixS( texts_input );
        indexDocument( doc_id, texts_input );
    };

    bool check = true;
    for ( auto doc_id = 0; doc_id < input_docs.size(); ++doc_id )
    {
        if ( doc_id < 1000 )
        {
            jthread ThreadDict( thread_Dict, input_docs[ doc_id ], doc_id );
            ThreadDict.detach();
        }
        else check = false;
    }
    if ( ! check ) cerr << "1000+ files" << endl;

    while ( thread_count < input_docs.size() );
}

void InvertedIndex::letterCase( char& value )
{
    const string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for( auto& i : caps )
        if ( value == i )
        {
            value = tolower( value );
            break;
        }
}

bool InvertedIndex::validSimbol(const char& value )
{
    const string simbols = ".,!?-:;()'\"\\|/{}[]<>_+=№@#$%^&*~`";

    for( auto& i : simbols )
        return value == i;
    return false;
}

void InvertedIndex::suffixS ( vector<string>& texts_input )
{
    for( auto& i : texts_input )
    {
        if ( i.size() <= 3 ) continue;

        if ( i[ i.size() - 1 ] == 's')
        {
            const int spos = 0;
            const int pos3 = i.size() - 3;
            const string suffixes ( "oussisius" );

            if ( ! i.compare( pos3, 3, "ies" ) )
                i.replace( pos3, 3, 1, 'y' );
            else if ( ! i.compare( pos3, 3, suffixes, spos, 3 ) ||
                  ! i.compare( pos3, 3, suffixes, spos + 2, 2 ) ||
                  ! i.compare( pos3, 3, suffixes, spos + 4, 2 ) ||
                  ! i.compare( pos3, 3, suffixes, spos + 6, 3 ) ) {}
            else
                i.erase( pos3 + 2 );
        }
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