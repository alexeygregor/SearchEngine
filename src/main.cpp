
#include <iostream>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
using namespace std;

int main()
{
    ConverterJSON cvr;
    try
    {
        cvr.checkConfig();
        cvr.setRequest();
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return -1;
    }
    cout << endl;
    cvr.setDocuments();
    InvertedIndex idx( cvr );
    idx.updateDocumentBase( cvr.getDocuments() );
    SearchServer srv( idx );
    cvr.putAnswers( srv.getAnswers( cvr ) );
    return 0;
}