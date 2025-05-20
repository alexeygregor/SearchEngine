
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
        return -1;
    }

    cvr.start();
    cvr.setResponsesLimit();
    idx.updateDocumentBase( cvr.getDocPath() );

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
            if ( cvr.getDBUpdate() ) idx.updateDocumentBase( cvr.getDocPath() );
        }
        else if ( command == "read" )
        {
            idx.readDocument( cvr );
        }
        else if ( command == "quit" )
        {
            cvr.finish();
            break;
        }
        else
            cerr << "Incorrect command" << endl;
    }
}