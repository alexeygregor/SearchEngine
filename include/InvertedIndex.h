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

    void indexDocument( const pair<vector<string>, const size_t>& texts_input );

    void updateDocumentBase( vector<string> input_docs );

    static pair<vector<string>, const size_t> docOutVector(const string& doc_page, const size_t& doc_id);

    static pair<vector<string>, const size_t> docOutFile(const string& doc_page, const size_t& doc_id);

    static void letterCase( char& value );

    static bool validSimbol (const char& value );

    void setDocBase();

    void getDocBase();

    vector<Entry> getWordCount( const string& word );

private:
    ConverterJSON _convert;
    map<string, vector<Entry>>::iterator it;
    map<string, vector<Entry>> freq_dictionary;
};