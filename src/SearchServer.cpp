
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

SearchServer::SearchServer( InvertedIndex& idx ) : _index( idx ) {};

vector<vector<RelativeIndex>>& SearchServer::search( vector<string> queries_input )
{
    relative_index.clear();

    for ( auto& request : queries_input )
    {
        string word;
        vector<string> input_words;

        for( auto simbol = 0; simbol < request.length(); ++simbol )
        {
            if ( request[ simbol ] != ' ' )
            {
                word += request[ simbol ];
                if ( simbol != request.length() - 1 )
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

    for ( auto& word : input_words )
    {
        entry = _index.getWordCount( word );
        for ( auto& [ doc_id, count ] : entry )
        {
            bool check = true;
            for ( auto& index : document )
            {
                if ( index[ 1 ] ==  doc_id )
                {
                    index[ 0 ] += count;
                    check = false;
                    break;
                }
            }
            if ( check ) document.push_back( { count, doc_id } );
        }
    }
    distribution( document );
}

void SearchServer::distribution( vector<vector<size_t>>& document )
{
    float max = 0;
    if ( ! document.empty() )
    {
        for ( int count = 1; count < document.size(); ++count )
        {
            int temp = count;
            while ( temp > 0 && document[ temp - 1 ][ 0 ] < document[ temp ][ 0 ] )
            {
                swap( document[ temp - 1 ], document[ temp ] );
                --temp;
            }
        }
        max = static_cast<float>( document[ 0 ][ 0 ] );
    }
	
    RelativeIndex rlv{};
    vector<RelativeIndex> input_relative;

    int responsesLimit = ConverterJSON::getResponsesLimit();
    for ( auto& i : document )
    {
        if ( input_relative.size() == responsesLimit ) break;
        rlv.doc_id = i[ 1 ];
        rlv.rank = static_cast<float>( i[ 0 ] ) / max;
        input_relative.push_back( { rlv.doc_id, rlv.rank } );
    }
    relative_index.push_back( input_relative );
}

vector<vector<pair<int, float>>>& SearchServer::getAnswers( ConverterJSON& cvr )
{
    for ( auto& relative_index : search( cvr.getRequests() ) )
    {
        vector<pair<int, float>> answer;
        for ( auto&[ doc_id, rank ] : relative_index )
            answer.emplace_back(  doc_id, rank );
        answers.push_back( answer );
    }
    return answers;
}