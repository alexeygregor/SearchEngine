#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

class ConverterJSON
{
public:

    ConverterJSON() = default;

    static bool valid( const string& value );

    static json configJSON();

    void checkConfigJSON();

    void setConfigJSON();

    void setRequestsJSON();

    void setAnswersJSON();

    void setResponsesLimit();

    void setDocuments();

    vector<string>& getDocuments();

    void setRequest();

    vector<string>& getRequests();

    int getResponsesLimit() const;

    bool getDBUpdate() const;

    void putAnswers( vector<vector<pair<int, float>>>& answers );

private:
    json dict;
    int max_responses = 5;
    vector<string> requests;
    vector<string> documents;
};
