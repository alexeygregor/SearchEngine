
#include "ConverterJSON.h"

void ConverterJSON::starting()
{
    ifstream File("config.json");
    File >> dict;
    string name = dict["config"]["name"];
    cout << "Starting " << name << "\n\n";
}

void ConverterJSON::loadConfig()
    {
        ifstream getConfig("resourses/config.json");
        getConfig >> dict;
        ofstream putConfig("config.json");
        putConfig << dict;
    }

void ConverterJSON::checkConfig()
{
    if(ifstream Check1("config.json", ios::in); !Check1)
        throw invalid_argument("config file is empty");
    else
    {
        json dict1;
        Check1 >> dict1;
        if(dict1["config"].empty())
            throw invalid_argument("config file is missing");

        json dict2;
        ifstream Check2("resourses/config.json");
        Check2 >> dict2;
        if(dict1["config"]["version"] != dict2["config"]["version"])
            throw invalid_argument("config.json has incorrect file version");
    }
}

void ConverterJSON::loadRequest()
{
    string  request;
    cout << "Input request:" << endl;
    getline(cin, request);

    string valid;
    ifstream Check("requests.json");
    Check >> valid;
    Check.close();
    if(!valid.empty())
    {
        ifstream getFile("requests.json");
        getFile >> dict;
        getFile.close();
    }

    if(dict["requests"].size() < 1000)
    {
        dict["requests"].push_back(request);
        ofstream putFile("requests.json");
        putFile << dict;
        putFile.close();
    }

    if(requests.empty())
    {
        for(auto& i : dict["requests"])
            requests.push_back(i);
    }
    else requests.push_back(request);
}

vector<string> ConverterJSON::getRequests()
{
    return requests;
}

vector<string> ConverterJSON::getTextDocument()
{
    ifstream File("config.json");
    File >> dict;
    File.close();
    documents.clear();
    for(auto& i : dict["files"])
        documents.push_back(i);
    return documents;
}

int ConverterJSON::getResponsesLimit()
{
    ifstream File("config.json");
    File >> dict;
    int max_responses = dict["config"]["max_responses"];
    return max_responses;
}

bool ConverterJSON::getDBUpdate()
{
    ifstream Config("config.json");
    Config >> dict;
    int db_update = dict["config"]["db_update"];
    dict.clear();
    Config.close();
    string valid;
    ifstream Check("requests.json");
    Check >> valid;
    Check.close();
    if(!valid.empty())
    {
        if(requests.size() % db_update != 0)
            return false;
    }
    cout << "db_update" << endl;
    return true;
}

void ConverterJSON::putAnswers(vector<vector<pair<int, float>>> answers)
{
    json dict;
    ofstream putFile("answers.json");

    for(auto i = 0; i < answers.size(); ++i)
    {
        string request_count = to_string(i + 1);

        if(answers[i].empty())
            dict["answers"]["request" + request_count]["result"] = "false";

        else
        {
            dict["answers"]["request" + request_count]["result"] = "true";
            for(auto j = 0; j < answers[i].size(); ++j)
            {
                if(j == getResponsesLimit()) break;
                if(answers[i].size() == 1)
                {
                    dict["answers"]["request" + request_count]["doc_id"].push_back(answers[i][j].first);
                    dict["answers"]["request" + request_count]["rank"].push_back(answers[i][j].second);
                }
                else
                {
                    dict["answers"]["request" + request_count]["relevance"]["doc_id"].push_back(answers[i][j].first);
                    dict["answers"]["request" + request_count]["relevance"]["rank"].push_back(answers[i][j].second);
                }
            }
        }
    }
    putFile << dict;
    putFile.close();
}

void ConverterJSON::getAnswers()
{
    json dict;
    ifstream getFile("answers.json");
    getFile >> dict;

    if(requests.size() <= 1000)
    {
        string request_count = to_string(dict["answers"].size());
        auto dictionary = [dict, request_count]()->const json
        {
            return dict["answers"]["request" + request_count];
        };

        cout << "Result:" << endl;
        if(dictionary()["result"] == "false")
            cout << "Not found" << endl;
        else
        {
            int answer_count = 0;
            if (dict["answers"]["request" + request_count]["doc_id"].size() == 1) answer_count = 1;
            else answer_count = dict["answers"]["request" + request_count]["relevance"]["doc_id"].size();

            for(auto i = 0; i < answer_count; ++i)
            {
                if(i == getResponsesLimit()) break;

                if(answer_count == 1)
                {
                    doc_id = (size_t)dictionary()["doc_id"][i];
                    rank = (float)dictionary()["rank"][i];
                }
                else
                {
                    doc_id = (size_t)dictionary()["relevance"]["doc_id"][i];
                    rank = (float)dictionary()["relevance"]["rank"][i];
                }

                string file_count = to_string(doc_id + 1);
                cout << "File_" << file_count  << "\t" << rank << endl;
            }
        }
    }
    else cerr << "1000+ requests" << endl;
    getFile.close();
}
