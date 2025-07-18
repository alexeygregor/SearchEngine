#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
using namespace std;

struct RelativeIndex
{
    size_t doc_id;
    float rank;

    bool operator ==( const RelativeIndex& other ) const
    {
        return ( doc_id == other.doc_id && rank == other.rank );
    }
};

class SearchServer
{
public:

    SearchServer() = default;

    SearchServer( const InvertedIndex& idx );

    vector<vector<RelativeIndex>>& search( vector<string>& queries_input );

    void requestParsing( const vector<string>& input_words );

    void distribution( vector<vector<size_t>>& document );

    vector<vector<pair<int, float>>>& getAnswers( ConverterJSON& cvr );

private:
    InvertedIndex _index;
    vector<vector<RelativeIndex>> relative_index;
    vector<vector<pair<int, float>>> answers;
};