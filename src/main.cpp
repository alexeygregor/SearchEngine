#include "SearchServer.h"
#include <gtest/gtest.h>


#ifdef TESTING
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif


#ifndef TESTING
int main()
{
    ConverterJSON cvr;
    InvertedIndex idx;
    cvr.loadConfig();

    try
    {
        cvr.checkConfig();
    }
    catch(const invalid_argument& e)
    {
        cerr << e.what() << endl;
        return 0;
    }

    cvr.starting();
    idx.updateDocumentBase(cvr.getTextDocument());

    while(true)
    {
        string command;
        cout << "Input command:" << endl;
        getline(cin, command);

        if(command == "search")
        {
            cvr.loadRequest();
            SearchServer srv(idx, cvr);
            cvr.putAnswers(srv.getAnswers());
            cvr.getAnswers();
            if(cvr.getDBUpdate()) idx.updateDocumentBase(cvr.getTextDocument());
        }
        else if(command == "read")
        {
            idx.getDocuments();
        }
        else if(command == "quit")
            break;
        else
            cerr << "Incorrect command" << endl;
    }
}
#endif