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

    void indexDocument( const size_t& doc_id, vector<string> texts_input );

    void updateDocumentBase( vector<string> input_docs );

    char letterCase( char value );

    vector<Entry> getWordCount( const string& word );

    void getDocuments( ConverterJSON& cvr );

private:
    int thread_count = 0;
    map<string, vector<Entry>>::iterator it;
    map<string, vector<Entry>> freq_dictionary;
};
