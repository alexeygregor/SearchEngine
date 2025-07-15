
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

SearchServer::SearchServer( const InvertedIndex& idx ) : _index( idx ) {};

vector<vector<RelativeIndex>>& SearchServer::search( vector<string>& queries_input )
{
    relative_index.clear();

    for ( auto& request : queries_input )
    {
        string word;
        vector<string> input_words;

        for( auto i = 0; i < request.length(); ++i )
        {
            if ( request[ i ] != ' ' )
            {
                word += request[ i ];
                if ( i != request.length() - 1 )
                    continue;
            }
            input_words.push_back( word );
            word.clear();
        }

        if ( input_words.size() > 10 )
        {
            input_words.clear();
            cerr << "Overlong request" << endl;
        }
        else
            requestParsing( input_words );
    }
    return relative_index;
}

void SearchServer::requestParsing(const vector<string>& input_words )
{
    vector<Entry> entry;
    vector<vector<size_t>> infrequent, document;

    for ( size_t i = 0; i < input_words.size(); ++i )
    {
        size_t count = 0;
        entry = _index.getWordCount( input_words[ i ] );

        for ( auto& [ doc_id, count ] : entry )
            count += count;
        infrequent.push_back( { count, i } );
    }
    sort( infrequent.begin(), infrequent.end() );

    for ( auto& i : infrequent )
    {
        entry = _index.getWordCount( input_words[ i[ 1 ] ] );
        for ( auto& [ doc_id, count ] : entry )
        {
            bool check = true;
            for ( auto& j : document )
                if ( j[ 1 ] ==  doc_id )
                {
                    j[ 0 ] += count;
                    check = false;
                    break;
                }
            if ( check ) document.push_back( { count,  doc_id } );
        }
    }
    distribution( document );
}

void SearchServer::distribution( vector<vector<size_t>>& document )
{
    if ( ! document.empty() )
    {
        bool check = true;
        while ( check )
        {
            check = false;
            for ( auto i = 0; i < document.size() - 1; ++i )
                if ( document[ i ][ 0 ] < document[ i + 1 ][ 0 ] )
                {
                    swap( document[ i ], document[ i + 1 ] );
                    check = true;
                }
        }
    }

    vector<RelativeIndex> input_relative;
    for ( auto& i : document )
    {
        if ( ConverterJSON cvr; input_relative.size() == cvr.getResponsesLimit() )
            break;
        RelativeIndex rlv;
        rlv.doc_id = i[ 1 ];
        rlv.rank = static_cast<float>( i[ 0 ] )
                 / static_cast<float>( document [ 0 ][ 0 ] );
        input_relative.push_back( { rlv.doc_id, rlv.rank } );
    }
    relative_index.push_back( input_relative );
}

vector<vector<pair<int, float>>>& SearchServer::getAnswers( ConverterJSON& cvr )
{
    for ( auto& i : search( cvr.getRequests() ) )
    {
        vector<pair<int, float>> answer;
        for ( auto&[ doc_id, rank ] : i )
            answer.emplace_back(  doc_id, rank );
        answers.push_back( answer );
    }
    return answers;
}