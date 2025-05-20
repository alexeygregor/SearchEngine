#pragma once

//#define TESTING

#include <fstream>
#include <iostream>
#include <locale>
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

    void start();

    void finish();

    void setConfig();

    void setRequest();

    void setResponsesLimit();

    void checkConfig();

    vector<string> getRequests();

    vector<string> getDocPath();

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
};
