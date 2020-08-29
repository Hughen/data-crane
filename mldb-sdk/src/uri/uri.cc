#include "uri.h"
#include "common/string.h"
#include <stdexcept>
#include <regex>

static const regex reg("^(mldb):\\/\\/([^\\/]+)(\\/[^\\?]+)?\\/?(\\?.*)?$");

// TODO: url decode
void URI::_parse(const string& raw_uri) {
    // protocal position
    string uri = raw_uri;
    trim(uri, " \t");

    smatch matches;
    if (!regex_match(uri, matches, reg)) {
        throw invalid_argument("invalid uri");
    }

    if (tolower(matches[1]) != "mldb") {
        throw invalid_argument("not supported protocal");
    }
    this->protocol = MLDB;

    string rtype = tolower(matches[2]);
    if (rtype == "raw") {
        this->type = Raw;
    } else if (rtype == "batch") {
        this->type = Batch;
    } else {
        throw invalid_argument(
            "just only support to 'raw' or 'batch' resource type"
        );
    }

    this->path = matches[3];
    trim(this->path, "/");

    string query_str = matches[4];
    ltrim(query_str, "?");
    vector<string> qarr = split(query_str, "&");
    for (string& v : qarr) {
        if (v.size() == 0) {
            continue;
        }
        size_t pos = v.find("=");
        if (pos == string::npos) {
            this->query[v] = vector<string>();
            continue;
        }

        string pk = v.substr(0, pos);
        string pv = v.substr(pos + 1);

        if (this->query.find(pk) == this->query.end()) {
            vector<string> pv_arr{pv};
            this->query[pk] = pv_arr;
        } else {
            this->query[pk].push_back(pv);
        }
    }
}
