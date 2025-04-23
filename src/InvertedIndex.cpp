
#include "InvertedIndex.h"

mutex dict_access;

void InvertedIndex::indexDocument(const size_t& doc_id, vector<string> texts_input)
{
    string word;
    while(!texts_input.empty())
    {
        size_t count = 0;
        vector<Entry> entry;

        word = texts_input[0];
        for(auto i = 0; i < texts_input.size(); ++i)
        {
            if(word == texts_input[i])
            {
                texts_input.erase(texts_input.begin() + i);
                ++count;
                --i;
            }
        }
        entry.push_back({doc_id, count});

        dict_access.lock();
        bool check = true;
        for(it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it)
        {
            if(it->first == word)
            {
                it->second.push_back({doc_id, count});
                check = false;
            }
        }
        if(check || freq_dictionary.empty())
            freq_dictionary.insert(pair(word, entry));
        dict_access.unlock();
    }
    ++thread_count;
}

void InvertedIndex::updateDocumentBase(vector<string> input_docs)
{
    freq_dictionary.clear();

    auto thread_Dict = [this](const string& doc_page, const size_t& doc_id)
    {
        string word;
        vector<string> texts_input;

        #ifdef TESTING
        int simbol = 0;
        while(simbol < doc_page.length())
        {
            if(doc_page[simbol] != ' ') word += doc_page[simbol];
            if(!word.empty())
            {
                if(doc_page[simbol] == ' ' || simbol == doc_page.length() - 1)
                {
                    texts_input.push_back(word);
                    word.clear();
                }
            }
            ++simbol;
        }
        #endif

        #ifndef TESTING
        ifstream Check(doc_page.c_str(), ios::in);
        Check >> word;
        if(word.empty())
            cerr << "file_" << to_string(doc_id) << " not found" << endl;
        Check.close();

        ifstream Document(doc_page.c_str(), ios::in);
        while (Document >> word)
        {
            string buffer;
            for(auto i = 0; i < word.size(); ++i)
            {
                word[i] = letterCase(word[i]);
                if(word[i] == '.' || word[i] == ',' || word[i] == '!'
                || word[i] == '-' || word[i] == '"' || (word[i] == '\'' && word[i + 1] == 's'))
                {
                    if(!buffer.empty()) texts_input.push_back(buffer);
                    buffer.clear();
                }
                else  buffer += word[i];
            }
            if(!buffer.empty()) texts_input.push_back(buffer);

            if(texts_input.size() > 1000 || buffer.length() > 10)
            {
                cerr << "file_" << to_string(doc_id) << " incorrect" << endl;
                texts_input.clear();
                break;
            }
        }
        Document.close();
        #endif

        indexDocument(doc_id, texts_input);
    };

    bool check = true;
    for(auto doc_id = 0; doc_id < input_docs.size(); ++doc_id)
    {
        if(doc_id < 1000)
        {
            jthread ThreadDict(thread_Dict, input_docs[doc_id], doc_id);
            ThreadDict.detach();
        }
        else check = false;
    }
    if(!check) cerr << "1000+ files" << endl;

    input_documents.clear();
    input_documents.assign(input_docs.begin(), input_docs.end());
    while(thread_count < input_docs.size()){}
}

char InvertedIndex::letterCase(char value)
{
    if(value == 'A') value = 'a'; if(value == 'B') value = 'b'; if(value == 'C') value = 'c';
    if(value == 'D') value = 'd'; if(value == 'E') value = 'e'; if(value == 'F') value = 'f';
    if(value == 'G') value = 'g'; if(value == 'H') value = 'h'; if(value == 'I') value = 'i';
    if(value == 'J') value = 'j'; if(value == 'K') value = 'k'; if(value == 'L') value = 'l';
    if(value == 'M') value = 'm'; if(value == 'N') value = 'n'; if(value == 'O') value = 'o';
    if(value == 'P') value = 'p'; if(value == 'Q') value = 'q'; if(value == 'R') value = 'r';
    if(value == 'S') value = 's'; if(value == 'T') value = 't'; if(value == 'U') value = 'u';
    if(value == 'V') value = 'v'; if(value == 'W') value = 'w'; if(value == 'X') value = 'x';
    if(value == 'Y') value = 'y'; if(value == 'Z') value = 'z';
    return value;
};

vector<Entry> InvertedIndex::getWordCount(const string& word)
{
    vector<Entry> entry;
    for(it = freq_dictionary.begin(); it != freq_dictionary.end(); ++it)
    {
        if(word == it->first)
        {
            for(auto & i : it->second)
                entry.push_back({i.doc_id, i.count});
        }
    }
    return entry;
}

void InvertedIndex::getDocuments()
{
    bool check = true;
    string request, word;
    int doc_id = 0, word_count = 0;

    char document[15000];
    for(auto & i : document) i = 0;

    cout << "Input file number:" << endl;
    getline(cin, request);

    for(auto i = 0; i < request.length(); ++i)
    {
        if(static_cast<int>(request[i]) - 48 < 0 || static_cast<int>(request[i]) - 48 > 9)
        {
            doc_id = 0;
            break;
        }
        if(i > 0) doc_id *= 10;
        doc_id += static_cast<int>(request[i]) - 48;
    }

    if(doc_id > 0 && doc_id <= input_documents.size())
    {
        ifstream Check(input_documents[doc_id - 1].c_str(), ios::in);
        Check >> word;
        if(word.empty())
        {
            cerr << "File not found" << endl;
            check = false;
        }
        Check.close();

        ifstream Document(input_documents[doc_id - 1].c_str(), ios::binary);
        Document.read(document, sizeof(document));
        while(Document >> word)
        {
            ++word_count;
            if(word.length() > 10 || word_count > 1000)
            {
                cerr << "File incorrect" << endl;
                check = false;
                break;
            }
        }
        Document.close();
    }
    else
    {
        cerr << "File not found" << endl;
        check = false;
    }
    if(check) cout << document << endl;
}