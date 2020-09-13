#include <stdexcept>
#include <regex>        // NOLINT
#include "uri.h"
#include "common/string.h"

using std::regex;
using std::smatch;
using std::invalid_argument;

static const regex reg("^(mldb):\\/\\/([^\\/]+)(\\/[^\\?]+)?\\/?(\\?.*)?$");

URI::URI(const URI& uri) {
    this->protocol = uri.protocol;
    this->type = uri.type;
    this->path = uri.path;
    this->query = uri.query;
}

// mldb://batch/batch-name-1/objects/object-name-xxx
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
            "just only support to 'raw' or 'batch' resource type");
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

string URI::String() const {
    if (this->protocol != MLDB || (this->type != Raw && this->type != Batch)) {
        return "";
    }
    string str_uri("mldb://");
    switch (this->type) {
    case Raw:
        str_uri += "raw";
        break;
    case Batch:
        str_uri += "batch";
        break;
    }
    if (this->path.size() > 0) {
        string dup_path = this->path;
        ltrim(dup_path, "/");
        str_uri += "/" + dup_path;
    }
    if (this->query.size() > 0) {
        string param_value = "";
        for (const auto& kv : this->query) {
            for (const auto& pv : kv.second) {
                param_value += "&" + kv.first + "=" + pv;
            }
        }
        str_uri += "?" + param_value.substr(1);
    }

    return str_uri;
}

ostream& operator<<(ostream& os, const URI& uri) {
    os << uri.String();
    return os;
}
