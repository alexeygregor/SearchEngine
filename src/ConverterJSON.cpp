
#include "ConverterJSON.h"

bool ConverterJSON::valid( const string& value )
{
    string valid;
    ifstream Check( value, ios::in );
    Check >> valid;
    return ! valid.empty();
}

json ConverterJSON::configJSON()
{
    json dict;
    ifstream getFile( "config.json", ios::in );
    getFile >> dict;
    return dict;
}

void ConverterJSON::checkConfigJSON()
{
    if ( valid( "config.json" ) )
    {
        dict.clear();
        ifstream Check( "resourses/config.json", ios::in );
        Check >> dict;

        if ( configJSON()[ "config" ].empty() )
            throw invalid_argument( "Config file is empty" );
        if ( dict[ "config" ].empty() )
            throw invalid_argument( "Сonfig file is missing" );
        if ( dict[ "config" ][ "version" ] != configJSON()[ "config" ][ "version" ] )
            throw invalid_argument( "config.json has incorrect file version" );
    }
}

void ConverterJSON::setConfigJSON()
{
    if ( ! valid( "config.json" ) )
    {
        dict.clear();
        ifstream getFile( "resourses/config.json", ios::in );
        getFile >> dict;
        ofstream putFile( "config.json");
        putFile << dict;
        cout << "Create config.json" << endl;
    }
}

void ConverterJSON::setRequestsJSON()
{
    if ( ! valid( "requests.json" ) )
    {
        dict.clear();
        ifstream getFile( "resourses/requests.json", ios::in );
        getFile >> dict;
        ofstream putFile( "requests.json" );
        putFile << dict;
        cout << "Create requests.json" << endl;
    }
}

void ConverterJSON::setAnswersJSON()
{
    if ( ! valid( "answers.json" ) )
    {
        dict.clear();
        ifstream getFile( "resourses/answers.json", ios::in );
        getFile >> dict;
        ofstream putFile( "answers.json" );
        putFile << dict;
        cout << "Create answers.json" << endl;
    }
}

void ConverterJSON::setResponsesLimit()
{
    max_responses = configJSON()[ "config" ][ "max_responses" ];
}

void ConverterJSON::setDocuments()
{
    dict = configJSON()[ "files" ];
    for ( auto& i : dict )
        documents.push_back( i );
}

vector<string>& ConverterJSON::getDocuments()
{
    return documents;
}

void ConverterJSON::setRequest()
{
    dict.clear();
    setRequestsJSON();
    ifstream getFile( "requests.json", ios::in );
    getFile >> dict;

    if ( dict[ "requests" ].size() > 1000 )
        cerr << "1000+ requests" << endl;
    else
        for ( auto& i : dict[ "requests" ] )
            requests.push_back( i );
}

vector<string>& ConverterJSON::getRequests()
{
    return requests;
}

int ConverterJSON::getResponsesLimit() const
{
    return max_responses;
}

bool ConverterJSON::getDBUpdate() const
{
    if ( int db_update = configJSON()["config"]["db_update"];
        requests.empty() || requests.size() % db_update != 0 )
        return false;
    cout << "DocumentBase update" << endl;
    return true;
}

void ConverterJSON::putAnswers( vector<vector<pair<int, float>>>& answers )
{
    if ( ! requests.empty() )
    {
        setAnswersJSON();
        cout << "Search complete" << endl;
    }
    else
    {
        answers.clear();
        cout << "Nothing a search for" << endl;
    }

    dict.clear();
    ofstream putFile( "answers.json" );

    for ( auto i = 0; i < answers.size(); ++i )
    {
        json temp;
        if ( answers[ i ].empty() )
            temp[ "result" ] = "false";
        else
        {
            temp[ "result" ] = "true";
            for ( auto j = 0; j < answers[ i ].size(); ++j )
            {
                if ( j == getResponsesLimit() ) break;

                int doc_id = answers[ i ][ j ].first;
                float rank = answers[ i ][ j ].second;

                if ( answers[ i ].size() == 1 )
                {
                    temp[ "doc_id" ].push_back( doc_id );
                    temp[ "rank" ].push_back( rank );
                }
                else
                {
                    map<string, float> answer( {
                        { "doc_id", doc_id },
                        { "rank", rank } } );
                    temp[ "relevance" ].push_back( answer );
                }
            }
        }
        dict[ "answers" ][ "request_" + to_string( i + 1 ) ] = temp;
    }
    putFile << dict.dump(4);
}