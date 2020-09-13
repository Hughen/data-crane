#include <errno.h>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <vector>
#include "sdk.h"
#include "common/file.h"
#include "log/log.h"

using std::vector;
using std::ifstream;

IODispatcher::IODispatcher(const string& raw_uri) {
    this->uri = new URI(raw_uri);
}

IODispatcher::~IODispatcher() {
    delete this->uri;
}

IODispatcher::IODispatcher(const IODispatcher& _another_io) {
    this->uri = _another_io.uri;
}

char* IODispatcher::read(int* const& rsize) {
    char* raw = nullptr;

    switch (this->uri->type) {
    case Raw:
        // do stuff
        break;
    case Batch:
        raw = this->_crane_open(rsize);
        break;
    default:
        break;
    }

    return raw;
}

int IODispatcher::write(const char* content) {
    // do stuff
    return 0;
}

// singleton instance
CraneClient& getCraneClient() {
    static CraneClient crclient;
    return crclient;
}

IODispatcher* open(const string& raw_uri) {
    return new IODispatcher(raw_uri);
}

char* IODispatcher::_crane_open(int* const& rsize) {
    CraneClient& crclient = getCraneClient();
    string fname;

    do {
        int ret_code = crclient.Open(this->uri, fname);
        if (ret_code == -EINVAL) {
            throw std::invalid_argument("invalid batch resource identifier");
        }

        try {
            char* content = readAllBytes(fname, rsize);
            if (*rsize != -1) {
                return content;
            }
        } catch (...) {
            dlog(Error) << "failed to read " << fname << lendl;
        }
    } while (true);
}

string IODispatcher::_filePath() {
    CraneClient& crclient = getCraneClient();
    string fname;

    int ret_code = crclient.Open(this->uri, fname);
    if (ret_code == -EINVAL) {
        throw std::invalid_argument("invalid batch resource identifier");
    }

    return fname;
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
    for (int i = 0; i < 256; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "coco_det-train.tfrecord-%05d-of-00256", i);
        lis.insert("mldb://batch/coco2017/object/" + string(buf));
        lis.insert("mldb://batch/coco2017/object/" + string(buf) + ".idx");
    }

    return lis;
}

void sdk_init() {
    std::call_once(init_flag, [](){
        srand(time(nullptr));
    });
}
