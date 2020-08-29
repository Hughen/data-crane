#ifndef _URI_H_
#define _URI_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

enum Proto {
    MLDB = 1
};

enum ResourceType {
    Raw = 1,
    Batch
};

struct URI {
    Proto protocol;
    ResourceType type;
    string path;
    map<string, vector<string>> query;

    URI(const string& raw_uri) { this->_parse(raw_uri); }
    void _parse(const string& raw_uri);
};

#endif
