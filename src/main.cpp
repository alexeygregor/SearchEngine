
#include <iostream>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
using namespace std;

int main()
{
    cout << endl;
    ConverterJSON cvr;
    cvr.setConfigJSON();

    try
    {
        cvr.checkConfigJSON();
    }
    catch ( const invalid_argument& e )
    {
        cerr << e.what() << endl;
        return -1;
    }

    cvr.setDocuments();
    cvr.setRequestsJSON();
    cvr.setResponsesLimit();

    InvertedIndex idx( cvr );
    idx.updateDocumentBase( cvr.getDocuments() );

    cvr.setRequest();
    SearchServer srv( idx );
    cvr.putAnswers( srv.getAnswers( cvr ) );

    if ( cvr.getDBUpdate() )
    {
        cvr.setDocuments();
        idx.updateDocumentBase( cvr.getDocuments() );
    }
}