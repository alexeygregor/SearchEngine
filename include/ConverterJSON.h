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

    static json configJSON();

    static void checkConfig();

    void setDocuments();

    vector<string>& getDocuments();

    void setRequest();

    vector<string>& getRequests();

    static int getResponsesLimit() ;

    bool getDBUpdate() const;

    static bool checkDocBase();

    static void setDocBaseJSON(map<string, vector<pair<size_t, size_t>>>& doc_base);

    static map<string, vector<pair<size_t, size_t>>> getDocBase();

    static void putAnswers(const vector<vector<pair<int, float>>>& answers ) ;

private:
    vector<string> requests;
    vector<string> documents;
};
