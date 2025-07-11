
#include "ConverterJSON.h"

void ConverterJSON::checkConfigJSON()
{
    json dict1, dict2;

    string valid;
    ifstream Check1( "config.json", ios::in );
    Check1 >> valid;
    if ( ! valid.empty() )
    {
        ifstream Check1_1( "config.json", ios::in );
        Check1_1 >> dict1;

        if ( dict1[ "config" ].empty() )
            throw invalid_argument( "Config file is empty" );
    }

    ifstream Check2( "resourses/config.json", ios::in );
    Check2 >> dict2;

    if ( dict2[ "config" ].empty() )
        throw invalid_argument( "Сonfig file is missing" );

    if ( dict1[ "config" ][ "version" ] != dict2[ "config" ][ "version" ] )
        throw invalid_argument( "config.json has incorrect file version" );
}

void ConverterJSON::setConfigJSON()
{
    string valid;
    ifstream Check( "config.json", ios::in );
    Check >> valid;
    if ( valid.empty() )
    {
        ifstream getFile( "resourses/config.json" );
        getFile >> dict;
        ofstream putFile( "config.json" );
        putFile << dict;
        cout << "Create config.json" << endl;
    }
}

void ConverterJSON::setRequestsJSON()
{
    string valid;
    ifstream Check( "requests.json" );
    Check >> valid;
    if ( valid.empty() )
    {
        ifstream getFile( "resourses/requests.json" );
        getFile >> dict;
        ofstream putFile( "requests.json" );
        putFile << dict;
        cout << "Create requests.json" << endl;
    }
}

void ConverterJSON::setAnswersJSON()
{
    string valid;
    ifstream Check( "answers.json" );
    Check >> valid;
    if ( valid.empty() )
    {
        ifstream getFile( "resourses/answers.json" );
        getFile >> dict;
        ofstream putFile( "answers.json" );
        putFile << dict;
        cout << "Create answers.json" << endl;
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
    documents.clear();
    for ( auto& i : dict[ "files" ] )
        documents.push_back( i );
}

vector<string> ConverterJSON::getDocuments()
{
    return documents;
}

void ConverterJSON::setRequest()
{
    setRequestsJSON();
    ifstream getFile( "requests.json" );
    getFile >> dict;

    if ( dict[ "requests" ].empty() )
        cerr << "Nothing a search for" << endl;

    requests.clear();
    if ( dict[ "requests" ].size() <= 1000 )
        for ( auto& i : dict[ "requests" ] )
            requests.push_back( i );
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

    if ( requests.size() == 0 ||
         requests.size() % db_update != 0 )
        return false;

    cout << "DocumentBase update" << endl;
    return true;
}

    void ConverterJSON::putAnswers( const vector<vector<pair<int, float>>>& answers )
{
    if ( ! requests.empty() )
    {
        dict.clear();
        setAnswersJSON();
        ofstream putFile( "answers.json" );

        for ( auto i = 0; i < answers.size(); ++i )
        {
            json temp;
            string request_count = to_string( i + 1 );
            if ( answers[ i ].empty() )
                temp[ "result" ] = "false";
            else
            {
                temp[ "result" ] = "true";
                for ( auto j = 0; j < answers[ i ].size(); ++j )
                {
                    if ( j == getResponsesLimit() ) break;
                    if ( answers[ i ].size() == 1 )
                    {
                        temp[ "doc_id" ].push_back( answers[ i ][ j ].first );
                        temp[ "rank" ].push_back( answers[ i ][ j ].second );
                    }
                    else {
                        map<string, double> answer( {
                            { "doc_id", answers[ i ][ j ].first },
                            { "rank", answers[ i ][ j ].second }
                        } );
                        temp[ "relevance" ].push_back( answer );
                    }
                }
            }
            dict[ "answers" ][ "request_" + request_count ] = temp;
        }
        putFile << dict.dump(4);
        cout << "Search complete" << endl;
    }
}