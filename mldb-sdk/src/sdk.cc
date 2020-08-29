#include "sdk.h"
#include "common/file.h"
#include "log/log.h"
#include <errno.h>
#include <fstream>

using std::vector;
using std::ifstream;

IODispatcher::IODispatcher(const string& raw_uri) {
    this->uri = new URI(raw_uri);
}

IODispatcher::~IODispatcher() {
    delete this->uri;
}

char* IODispatcher::read() {
    switch (this->uri->type)
    {
    case Raw:
        // do stuff
        break;
    case Batch:
        return this->_crane_open();
    default:
        break;
    }

    return NULL;
}

int IODispatcher::write() {
    // do stuff
    return 0;
}

// singleton instance
CraneClient& getCraneClient() {
    static CraneClient crclient;
    return crclient;
}

IODispatcher open(const string& raw_uri) {
    return IODispatcher(raw_uri);
}

char* IODispatcher::_crane_open() {
    CraneClient& crclient = getCraneClient();
    string fname;

    do {
        int ret_code = crclient.Open(this->uri, fname);
        if (ret_code == -EINVAL) {
            throw std::invalid_argument("invalid batch resource identifier");
        }

        try {
            int rsize = 0;
            char* content = readAllBytes(fname, rsize);
            dlog(Debug) << "read from crane file, size: " << rsize << lendl;
            if (content != NULL) {
                return content;
            }
        } catch (...) {
            dlog(Error) << "failed to read " << fname << lendl;
        }
    } while (true);
}

Prefetcher::Prefetcher(const set<string>& lis) {
    CraneClient& crclient = getCraneClient();
    if (lis.empty()) {
        crclient.Prefetch();
        return;
    }

    vector<string> vector_lis;
    for (const auto& v : lis) {
        vector_lis.push_back(v);
    }
    crclient.Prefetch(vector_lis);
}

Prefetcher* Prefetcher::Append(const set<string>& lis) {
    if (lis.empty()) {
        return this;
    }

    vector<string> vector_lis;
    for (const auto& v : lis) {
        vector_lis.push_back(v);
    }

    getCraneClient().PrefetchAppend(vector_lis);
    return this;
}

int Prefetcher::Start() {
    return getCraneClient().StartPrefetch();
}

// TODO: put in official code
set<string> list_batch() {
    set<string> lis;
    for (int i = 1; i < 100; i++) {
        lis.insert("mldb://batch/kindle-test/" + std::to_string(i) + ".txt");
    }

    return lis;
}

void sdk_init() {
    std::call_once(init_flag, [](){
        srand(time(NULL));
    });
}
