
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

SearchServer::SearchServer( InvertedIndex& idx ) : _index( idx ) {};

vector<vector<RelativeIndex>> SearchServer::search( vector<string> queries_input )
{
    relative_index.clear();

    for ( auto& request : queries_input )
    {
        string word;
        vector<string> input_words;

        int simbol = 0;
        while ( simbol < request.length() )
        {
            if ( request[ simbol ] != ' ' ) word += request[ simbol ];
            if ( ! word.empty() )
            {
                if ( request[ simbol ] == ' ' || simbol == request.length() - 1 )
                {
                    input_words.push_back( word );
                    word.clear();
                }
            }
            ++simbol;
        }
        if ( input_words.size() > 10 )
        {
            input_words.clear();
            cerr << "Overlong request" << endl;
        }
        else
            requestsParsing( input_words );
    }
    return relative_index;
}

void SearchServer::requestsParsing( vector<string>& input_words )
{
    vector<Entry> entry;
    vector<vector<size_t>> infrequent, document;

    for ( size_t i = 0; i < input_words.size(); ++i )
    {
        size_t count = 0;
        entry = _index.getWordCount( input_words[ i ] );

        for ( auto& j : entry )
            count += j.count;
        infrequent.push_back( { count, i } );
    }
    sort( infrequent.begin(), infrequent.end() );

    for ( auto& i : infrequent )
    {
        entry = _index.getWordCount( input_words[ i[ 1 ] ] );
        for ( auto& j : entry )
        {
            bool check = true;
            if ( document.empty() )
            {
                document.push_back( { j.count, j.doc_id } );
                check = false;
            }
            else
            {
                for ( auto& k : document )
                {
                    if ( k[ 1 ] == j.doc_id )
                    {
                        k[ 0 ] += j.count;
                        check = false;
                        break;
                    }
                }
            }
            if ( check ) document.push_back( { j.count, j.doc_id } );
        }
    }
    distribution( document );
}

void SearchServer::distribution( vector<vector<size_t>>& document )
{
    int max = 0;
    float rank = 0;
    RelativeIndex rlv;
    vector<RelativeIndex> input_relative;

    if ( ! document.empty() )
    {
        bool check = true;
        while ( check )
        {
            check = false;
            for ( auto i = 0; i < document.size() - 1; ++i )
            {
                if ( document[ i ][ 0 ] < document[ i + 1 ][ 0 ] )
                {
                    swap( document[ i ], document[ i + 1 ] );
                    check = true;
                }
            }
        }
        max = document[ 0 ][ 0 ];
    }

    ConverterJSON cvr;
    for ( auto & i : document )
    {
        if ( input_relative.size() == cvr.getResponsesLimit() ) break;
        rank = ( float ) i[ 0 ] / ( float )max;
        rlv.doc_id = i[ 1 ];
        rlv.rank = rank;
        input_relative.push_back( { i[ 1 ], rank } );
    }
    relative_index.push_back( input_relative );
}

const vector<vector<pair<int, float>>>& SearchServer::getAnswers( ConverterJSON& cvr )
{
    answers.clear();
    for ( auto& i : search( cvr.getRequests() ) )
    {
        vector<pair<int, float>> answer;
        for ( auto& j : i )
            answer.push_back( { j.doc_id, j.rank } );
        answers.push_back( answer );
    }
    return answers;
}