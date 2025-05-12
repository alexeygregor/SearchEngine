#pragma once

#include "ConverterJSON.h"
#include "InvertedIndex.h"

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

    SearchServer( InvertedIndex& idx );

    vector<vector<RelativeIndex>> search( vector<string> queries_input );

    void searchDocument( vector<string> input_words );

    void searchDocuments( vector<vector<size_t>> document );

    vector<vector<pair<int, float>>> getAnswers( ConverterJSON& cvr );

private:
    InvertedIndex _index;
    vector<vector<RelativeIndex>> relative_index;
    vector<vector<pair<int, float>>> answers;
};
