#pragma once

#include "ConverterJSON.h"

struct Entry
{
    size_t doc_id, count;

    bool operator ==( const Entry& other ) const
    {
        return ( doc_id == other.doc_id && count == other.count );
    }
};

class InvertedIndex
{
public:
    InvertedIndex() = default;

    InvertedIndex(ConverterJSON& cvr);

    void indexDocument( const size_t& doc_id, vector<string>& texts_input );

    void updateDocumentBase( vector<string> input_docs );

    char letterCase( char& value );

    void suffixS ( vector<string>& texts_input);

    vector<Entry> getWordCount( const string& word );

private:
    ConverterJSON _convert;
    int thread_count = 0;
    map<string, vector<Entry>>::iterator it;
    map<string, vector<Entry>> freq_dictionary;
};
