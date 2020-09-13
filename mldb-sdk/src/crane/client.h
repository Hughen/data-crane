#ifndef CRANE_CLIENT_GRPC_H_        /* NOLINT */
#define CRANE_CLIENT_GRPC_H_

#include <grpcpp/grpcpp.h>
#include <vector>
#include <memory>
#include <string>
#include "crane.grpc.pb.h"
#include "crane.pb.h"
#include "log/log.h"
#include "uri/uri.h"
#include "common/filepath.h"

namespace crn {

using grpc::Channel;
using crane::Crane;
using std::vector;
using std::string;

std::string get_csi_volume();
std::string get_lcs_root();

struct PrefetchList {
    string fname;

    PrefetchList();
    int append(const vector<string>& lines);
    int reset(const vector<string>& lines);
};

class CraneClient {
 public:
    CraneClient();
    explicit CraneClient(std::shared_ptr<Channel> channel);
    ~CraneClient();
    int Open(const URI* uri, string& file);     // NOLINT
    int Open(const string& batch, const string& object, string& file);  // NOLINT
    CraneClient* Prefetch(const vector<string>& lines = vector<string>());
    CraneClient* PrefetchAppend(const vector<string>& lines);
    int StartPrefetch();

 private:
    int _open(const string& batch, const string& object, string& file); // NOLINT
    inline void initMembers() {
        this->csi_volume = get_csi_volume();
        this->lcs_root = get_lcs_root();

        dlog(Debug) << "lcs: " << this->lcs_root
            << ", csi-volume: " << this->csi_volume << lendl;

        this->prefeth_list = std::unique_ptr<PrefetchList>(new PrefetchList());
    }

    inline string getSelfFilePath(const string& self_file) {
        return filepathJoin(2, this->lcs_root.c_str(), self_file.c_str());
    }

    string csi_volume;
    string lcs_root;
    std::unique_ptr<Crane::Stub> stub_;
    std::unique_ptr<PrefetchList> prefeth_list;

    static const char _version[];
};

}   // namespace crn

#endif  // CRANE_CLIENT_GRPC_H_     /* NOLINT */
