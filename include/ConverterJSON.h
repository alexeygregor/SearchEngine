#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

class ConverterJSON
{
public:

    ConverterJSON() = default;

    void start();

    void finish();

    void setConfig();

    void checkConfig();

    void setResponsesLimit();

    void setDocuments();

    vector<string> getDocuments();

    void setRequest();

    vector<string> getRequests();

    int getResponsesLimit();

    bool getDBUpdate();

    void putAnswers( vector<vector<pair<int, float>>> answers );

private:
    json dict;
    float rank = 0;
    size_t doc_id = 0;
    int max_responses = 5;
    vector<string> requests;
    vector<string> documents;
};
