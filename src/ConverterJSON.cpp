
#include "ConverterJSON.h"

void ConverterJSON::start()
{
    ifstream File( "config.json" );
    File >> dict;
    string name = dict[ "config" ][ "name" ];
    cout << "\nStarting " << name << endl;
}

void ConverterJSON::finish()
{
    ifstream File( "config.json" );
    File >> dict;
    string name = dict[ "config" ][ "name" ];
    cout << "\nFinishing " << name << endl;
}

void ConverterJSON::setConfig()
    {
        ifstream getConfig( "resourses/config.json" );
        getConfig >> dict;
        ofstream putConfig( "config.json" );
        putConfig << dict;
    }

void ConverterJSON::checkConfig()
{
    if ( ifstream Check1( "config.json", ios::in ); ! Check1 )
        throw invalid_argument( "config file is empty" );
    else
    {
        json dict1;
        Check1 >> dict1;
        if ( dict1[ "config" ].empty() )
            throw invalid_argument( "config file is missing" );

        json dict2;
        ifstream Check2( "resourses/config.json" );
        Check2 >> dict2;
        if ( dict1[ "config" ][ "version" ] != dict2[ "config" ][ "version" ] )
            throw invalid_argument( "config.json has incorrect file version" );
    }
}

void ConverterJSON::setResponsesLimit()
{
    ifstream File( "config.json" );
    File >> dict;
    max_responses = dict[ "config" ][ "max_responses" ];
}

void ConverterJSON::setDocuments()
{
    ifstream File( "config.json" );
    File >> dict;
    File.close();
    for ( auto& i : dict[ "files" ] )
        documents.push_back( i );
}

vector<string> ConverterJSON::getDocuments()
{
    return documents;
}

void ConverterJSON::setRequest()
{
    string valid;
    ifstream Check( "requests.json" );
    Check >> valid;
    Check.close();
    if ( ! valid.empty() )
    {
        ifstream getFile( "requests.json" );
        getFile >> dict;
        getFile.close();
    }

    requests.clear();
    if ( dict[ "requests" ].size() <= 1000 )
    {
        for ( auto& i : dict[ "requests" ] )
            requests.push_back( i );
    }
    else
        cerr << "1000+ requests" << endl;
}

vector<string> ConverterJSON::getRequests()
{
    return requests;
}

int ConverterJSON::getResponsesLimit()
{
    return max_responses;
}

bool ConverterJSON::getDBUpdate()
{
    ifstream Config( "config.json" );
    Config >> dict;
    int db_update = dict[ "config" ][ "db_update" ];
    dict.clear();
    Config.close();
    string valid;
    ifstream Check( "requests.json" );
    Check >> valid;
    Check.close();
    if ( ! valid.empty() )
    {
        if ( requests.size() % db_update != 0 )
            return false;
    }

    cout << "db_update" << endl;
    return true;
}

void ConverterJSON::putAnswers( vector<vector<pair<int, float>>> answers )
{
    if ( ! requests.empty() )
    {
        json dict;
        ofstream putFile( "answers.json" );

        for ( auto i = 0; i < answers.size(); ++i )
        {
            string request_count = to_string( i + 1 );

            if ( answers[ i ].empty() )
                dict[ "answers" ][ "request" + request_count ][ "result" ] = "false";

            else
            {
                dict[ "answers" ][ "request" + request_count ][ "result" ] = "true";
                for ( auto j = 0; j < answers[ i ].size(); ++j )
                {
                    if ( j == getResponsesLimit() ) break;
                    if ( answers[ i ].size() == 1 )
                    {
                        dict[ "answers" ][ "request" + request_count ][ "doc_id" ].push_back( answers[ i ][ j ].first );
                        dict[ "answers" ][ "request" + request_count ][ "rank" ].push_back( answers[ i ][ j ].second );
                    }
                    else
                    {
                        dict[ "answers" ][ "request" + request_count ][ "relevance" ][ "doc_id" ].push_back( answers[ i ][ j ].first );
                        dict[ "answers" ][ "request" + request_count ][ "relevance" ][ "rank" ].push_back( answers[ i ][ j ].second );
                    }
                }
            }
        }
        putFile << dict;
        putFile.close();
        cout << "complete" << endl;
    }
}