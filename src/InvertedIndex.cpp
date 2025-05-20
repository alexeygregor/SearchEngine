
#include "ConverterJSON.h"
#include "InvertedIndex.h"

mutex dict_access;

void InvertedIndex::indexDocument( const size_t& doc_id, vector<string> texts_input )
{
    while ( !texts_input.empty() )
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

void InvertedIndex::updateDocumentBase( vector<string> input_docs )
{
    freq_dictionary.clear();

    auto thread_Dict = [ this ]( const string& doc_page, const size_t& doc_id )
    {
        string word;
        vector<string> texts_input;

        #ifdef TESTING
        int simbol = 0;
        while(simbol < doc_page.length())
        {
            if ( doc_page[ simbol ] != ' ' ) word += doc_page[ simbol ];
            if( !word.empty() )
            {
                if( doc_page[ simbol ] == ' ' || simbol == doc_page.length() - 1 )
                {
                    texts_input.push_back( word );
                    word.clear();
                }
            }
            ++simbol;
        }
        #endif

        #ifndef TESTING
        ifstream Check( doc_page.c_str(), ios::in );
        Check >> word;
        if( word.empty() )
            cerr << "file_" << to_string( doc_id ) << " not found" << endl;
        Check.close();

        ifstream Document( doc_page.c_str(), ios::in );
        while ( Document >> word )
        {
            string buffer;
            for ( auto i = 0; i < word.size(); ++i )
            {
                word[ i ] = letterCase( word[ i ] );
                if ( word[ i ] == '.' || word[ i ] == ',' || word[ i ] == '!'
                  || word[ i ] == '-' || word[ i ] == '"' ||
                  ( word[ i ] == '\'' && word[ i + 1 ] == 's' ) )
                {
                    if ( word[ i ] == '\'' ) ++i;
                    if ( !buffer.empty() ) texts_input.push_back( buffer );
                    buffer.clear();
                }
                else  buffer += word[ i ];
            }
            if ( !buffer.empty() ) texts_input.push_back( buffer );

            if ( texts_input.size() > 1000 || buffer.length() > 10 )
            {
                cerr << "file_" << to_string( doc_id );
                if ( buffer.length() > 10 ) cerr << ": " << buffer << " \n";
                else cerr << ": lots of words" << " \n";
                texts_input.clear();
                break;
            }
        }
        Document.close();
        #endif

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
    if ( !check ) cerr << "1000+ files" << endl;

    while ( thread_count < input_docs.size() );
}

char InvertedIndex::letterCase( char value )
{
    string caps = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for( auto & i : caps )
    {
        if ( value == i )
        {
            tolower( value );
            break;
        }
    }
    return value;
};

void InvertedIndex::readDocument( ConverterJSON& cvr )
{
    int doc_id = 0;
    string request, textline;

    cout << "Input file number:" << endl;
    getline( cin, request );

    for ( auto i = 0; i < request.length(); ++i )
    {
        if ( ( int )request[ i ] - 48 < 0 || ( int )request[ i ] - 48 > 9)
        {
            doc_id = 0;
            break;
        }
        if ( i > 0 ) doc_id *= 10;
        doc_id += ( int )request[ i ] - 48;
    }

    if ( doc_id <= 0 && doc_id > cvr.getDocPath().size() )
        cerr << "File not found" << endl;
    else
    {
        ifstream Check( cvr.getDocPath()[ doc_id - 1 ].c_str(), ios::in );
        Check >> textline;
        if ( textline.empty() )
            cerr << "File not found" << endl;
        else
        {
            ifstream Document( cvr.getDocPath()[ doc_id - 1 ].c_str(), ios::in );
            while ( getline( Document, textline, '\n' ) )
            {
                cout << textline << endl;
            }
            Document.close();
        }
    }
}

vector<Entry> InvertedIndex::getWordCount( const string& word )
{
    vector<Entry> entry;
    for ( it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it )
    {
        if ( word == it->first )
        {
            for ( auto & i : it->second )
                entry.push_back( { i.doc_id, i.count } );
        }
    }
    return entry;
}