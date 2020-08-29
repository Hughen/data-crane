#ifndef _MLDB_SDK_H_
#define _MLDB_SDK_H_

#include "uri/uri.h"
#include "crane/client.h"
#include <string>
#include <pybind11/stl.h>

using crn::CraneClient;
using std::string;
using std::set;

CraneClient& getCraneClient();

static std::once_flag init_flag;

class IODispatcher {
public:
    IODispatcher(const string& raw_uri);
    ~IODispatcher();

    char* read();
    int write();

private:
    char* _crane_open();

    URI *uri;
};

// exported
class Prefetcher {
public:
    Prefetcher() {}
    Prefetcher(const set<string>& lis = set<string>());
    Prefetcher* Append(const set<string>& lis);
    int Start();
};

// exported
void sdk_init();
IODispatcher open(const std::string& raw_uri);
set<string> list_batch();

#endif

#ifndef VERSION
#define VERSION 1.0
#endif