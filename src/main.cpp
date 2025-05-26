
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

int main()
{
    ConverterJSON cvr;
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
    cvr.setDocuments();
    cvr.setResponsesLimit();

    InvertedIndex idx( cvr );
    idx.updateDocumentBase( cvr.getDocuments() );

    while ( true )
    {
        string command;
        cout << "\nInput command:" << endl;
        getline( cin, command );

        if ( command == "find" )
        {
            cvr.setRequest();
            SearchServer srv( idx );
            cvr.putAnswers( srv.getAnswers( cvr ) );
            if ( cvr.getDBUpdate() )
            {
                cvr.setDocuments();
                idx.updateDocumentBase( cvr.getDocuments() );
            }
        }
        else if ( command == "read" )
        {
            idx.readDocument();
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