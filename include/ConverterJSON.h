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

    void setConfigJSON();

    void setRequestsJSON();

    void setAnswersJSON();

    void checkResoursesJSON();

    void setResponsesLimit();

    void setDocuments();

    vector<string> getDocuments();

    void setRequest();

    vector<string> getRequests();

    int getResponsesLimit();

    bool getDBUpdate();

    void putAnswers( const vector<vector<pair<int, float>>>& answers );

private:
    json dict;
    int max_responses = 5;
    vector<string> requests;
    vector<string> documents;
};
