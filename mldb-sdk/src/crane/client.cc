#include "client.h"
#include "common/filepath.h"
#include "common/string.h"
#include "common/utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <openssl/sha.h>
#include <cstdlib>
#include <chrono>
#include <stdexcept>
#include <errno.h>

namespace crn {

using crane::OpenRequest;
using crane::OpenResponse;
using crane::PrefetchResponse;
using google::protobuf::Empty;
using grpc::StatusCode;

const string CraneClient::_version = "1.0";

string get_csi_volume() {
    std::ifstream cpuset_file("/proc/1/cpuset");
    string content;
    std::getline(cpuset_file, content);
    cpuset_file.close();

    std::regex reg(
        "([0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12})",
        std::regex_constants::icase | std::regex_constants::ECMAScript
    );
    std::smatch match;
    string pod_uid;
    if (std::regex_search(content, match, reg)) {
        pod_uid = match[1];
    } else {
        return "";
    }

    // echo -n lcs | md5sum
    // 9351c00f73660c0a9076d185506e355f
    string plaintext = pod_uid + "9351c00f73660c0a9076d185506e355f";

    char buf[3];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, plaintext.c_str(), plaintext.size());
    SHA256_Final(hash, &sha256);

    string hash_value;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(buf, "%02x", hash[i]);
        hash_value = hash_value + buf;
    }

    return hash_value;
}

string get_lcs_root() {
    char* v = std::getenv("LCS_ROOT");
    string lcs;
    if (v == NULL) {
        lcs = "/lcs";
    } else {
        lcs = string(v);
    }

    rtrim(lcs, "/");

    return lcs;
}

PrefetchList::PrefetchList() : fname(get_lcs_root() + "/.crane/prefetch_list") {
    string fpath = filepathJoin(2, fname, "..");
    mkdir_p(fpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

int PrefetchList::append(const vector<string>& lines) {
    std::ofstream listFile(fname, std::ios::app);

    int n = 0;
    for (const auto& v : lines) {
        listFile << v << std::endl;
        ++n;
    }
    listFile.close();

    dlog(Debug) << "prefetch list append number: " << n << lendl;

    return n;
}

int PrefetchList::reset(const vector<string>& lines) {
    std::ofstream listFile(fname, std::ios::trunc);

    int n = 0;
    for (const auto& v : lines) {
        listFile << v << std::endl;
        ++n;
    }
    listFile.close();

    dlog(Debug) << "prefetch list reset number: " << n << lendl;

    return n;
}

CraneClient::CraneClient() {
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
        "unix:///var/run/data-crane/crane.sock",
        grpc::InsecureChannelCredentials()
    );
    this->stub_ = Crane::NewStub(channel);

    this->initMembers();
}

CraneClient::CraneClient(std::shared_ptr<Channel> channel) : stub_(Crane::NewStub(channel)) {
    this->initMembers();
}

CraneClient::~CraneClient() {
    // empty
}

int CraneClient::Open(const URI* uri, string& file) {
    string path = uri->path;
    size_t pos = path.find("/");
    if (pos == string::npos || (pos + 1) >= path.size()) {
        // invalid uri
        return -EINVAL;
    }

    string batch_name = path.substr(0, pos);
    string object_name = path.substr(pos + 1);

    return this->Open(batch_name, object_name, file);
}

int CraneClient::Open(
    const string& batch,
    const string& object,
    string& file)
{
    do {
        int status_code = this->_open(batch, object, file);
        switch (status_code)
        {
        case StatusCode::OK:
            break;
        case StatusCode::NOT_FOUND:
            throw std::overflow_error(
                "the '" + batch + "/" + object + "' is not found"
            );
        }

        sleepRandom(5, 50);
    } while (true);

    return 0;
}

int CraneClient::_open(
    const string& batch,
    const string& object,
    string& file)
{
    auto _st = std::chrono::system_clock::now();

    grpc::ClientContext ctx;
    ctx.AddMetadata("version", this->_version);
    ctx.AddMetadata("csi-volume", this->csi_volume);

    OpenRequest req;
    req.set_batch(batch);
    req.set_object(object);

    OpenResponse reply;

    grpc::Status status = this->stub_->Open(&ctx, req, &reply);

    std::chrono::duration<double> cost = std::chrono::system_clock::now() - _st;

    dlog(Debug) << "crane open \"" << batch << "/" << object << "\""
        << " called cost: " << cost.count() << "s"
        << " status code: " << status.error_code()
        << " error message: " << status.error_message() << lendl;

    if (status.ok()) {
        file = reply.path();
        return status.error_code();
    }
    
    return status.error_code();
}

CraneClient* CraneClient::Prefetch(const vector<string>& lines) {
    this->prefeth_list->reset(lines);

    return this;
}

CraneClient* CraneClient::PrefetchAppend(const vector<string>& lines) {
    this->prefeth_list->append(lines);
    return this;
}

int CraneClient::StartPrefetch() {
    grpc::ClientContext ctx;
    ctx.AddMetadata("version", this->_version);
    ctx.AddMetadata("csi-volume", this->csi_volume);

    Empty empty;
    PrefetchResponse reply;

    grpc::Status status = this->stub_->Prefetch(&ctx, empty, &reply);

    if (status.ok()) {
        return reply.total();
    }

    int err_code = status.error_code();
    if (err_code == grpc::StatusCode::CANCELLED) {
        dlog(Warn) << "the prefetch feature has been cancelled" << lendl;
    } else {
        dlog(Error) << "unknown error in prefetching: errno="
            << -err_code << lendl;
    }

    return -err_code;
}

}
