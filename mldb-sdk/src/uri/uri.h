#ifndef URI_URI_H_
#define URI_URI_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>

using std::string;
using std::vector;
using std::map;
using std::ostream;

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

    explicit URI(const string& raw_uri) { this->_parse(raw_uri); }
    URI(const URI& uri);
    string String() const;
    friend ostream& operator<<(ostream& os, const URI& uri);
    void _parse(const string& raw_uri);
};

#endif  // URI_URI_H_
