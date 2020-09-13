#ifndef _MLDB_SDK_H_    /* NOLINT */
#define _MLDB_SDK_H_

#include <string>
#include <set>
#include "uri/uri.h"
#include "crane/client.h"
#include "http/httprequest.h"
#include "version/version.h"

using crn::CraneClient;
using std::string;
using std::set;

CraneClient& getCraneClient();

static std::once_flag init_flag;

class IODispatcher {
 public:
    IODispatcher() = default;
    explicit IODispatcher(const string& raw_uri);
    IODispatcher(const IODispatcher& _another_io);
    virtual ~IODispatcher();

    char* read(int* const& rsize);
    int write(const char* content);

    // temporary API
    string _filePath();

 private:
    char* _crane_open(int* const& rsize);

    URI *uri;
};

// exported
class Prefetcher {
 public:
    explicit Prefetcher(const set<string>& lis = set<string>());
    Prefetcher* Append(const set<string>& lis);
    virtual ~Prefetcher() {}
    int Start();
};

// exported
void sdk_init();
IODispatcher* open(const std::string& raw_uri);
set<string> list_batch();

#endif  // _MLDB_SDK_H_    /* NOLINT */
