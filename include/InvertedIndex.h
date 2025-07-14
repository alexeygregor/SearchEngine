#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "ConverterJSON.h"
using namespace std;

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

    InvertedIndex( const ConverterJSON& cvr );

    void indexDocument( const size_t& doc_id, vector<string>& texts_input );

    void updateDocumentBase( vector<string>& input_docs );

    static void letterCase( char& value );

    static bool validSimbol (const char& value );

    static void suffixS ( vector<string>& texts_input );

    vector<Entry> getWordCount( const string& word );

private:
    ConverterJSON _convert;
    map<string, vector<Entry>>::iterator it;
    map<string, vector<Entry>> freq_dictionary;
    int thread_count = 0;
};