#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <mutex>
#include <thread>
#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

class ConverterJSON
{
public:

    void starting();

    void setConfig();

    void setRequest();

    void setDocuments();

    void setResponsesLimit();

    void checkConfig();

    vector<string> getDocuments();

    vector<string> getRequests();

    vector<string> getTextPath();

    int getResponsesLimit();

    bool getDBUpdate();

    void putAnswers( vector<vector<pair<int, float>>> answers );

    void getAnswers();

private:
    json dict;
    float rank = 0;
    size_t doc_id = 0;
    int max_responses = 5;
    vector<string> requests;
    vector<string> documents;
    vector<string> input_docs;
};
