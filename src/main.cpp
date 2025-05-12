
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

int main()
{
    ConverterJSON cvr;
    InvertedIndex idx;
    cvr.setConfig();

    try
    {
        cvr.checkConfig();
    }
    catch ( const invalid_argument& e )
    {
        cerr << e.what() << endl;
        return 0;
    }

    cvr.starting();
    cvr.setDocuments();
    cvr.setResponsesLimit();
    idx.updateDocumentBase( cvr.getDocuments() );

    while ( true )
    {
        string command;
        cout << "Input command:" << endl;
        getline( cin, command );

        if ( command == "find" )
        {
            cvr.setRequest();
            SearchServer srv( idx );
            cvr.putAnswers( srv.getAnswers( cvr ) );
            cvr.getAnswers();
            if ( cvr.getDBUpdate() ) idx.updateDocumentBase( cvr.getDocuments() );
        }
        else if ( command == "read" )
        {
            idx.getDocuments( cvr );
        }
        else if ( command == "quit" )
            break;
        else
            cerr << "Incorrect command" << endl;
    }
}