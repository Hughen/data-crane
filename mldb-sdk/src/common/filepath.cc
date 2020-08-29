#include "filepath.h"
#include "string.h"
#include <vector>
#include <climits>
#include <errno.h>
#include <cstring>

string filepathJoin(int n, ...) {
    vector<string> path_segs;
    va_list vl;
    va_start(vl, n);
    string first_arg;
    for (int i = 0; i < n; i++) {
        char* arg = va_arg(vl, char*);
        if (i == 0) {
            first_arg = string(arg);
        }
        vector<string> arg_segs = split(arg, "/");
        for (const auto& v : arg_segs) {
            if (v == ".") {
                continue;
            } else if (v == ".." && !path_segs.empty()) {
                path_segs.pop_back();
            } else {
                path_segs.push_back(v);
            }
        }
    }
    va_end(vl);

    string fpath = "";
    if (first_arg[0] == '/') {
        fpath += "/";
    }
    for (const auto& v : path_segs) {
        if (fpath.empty()) {
            fpath += v;
        } else {
            fpath += "/" + v;
        }
    }

    return fpath;
}


void mkdir_p(const string& dir, mode_t mode) {
    mkdir_p(dir.c_str(), mode);
}

void mkdir_p(const char* dir, mode_t mode) {
    const size_t len = strlen(dir);
    char _path[PATH_MAX];
    char *p;

    errno = 0;
    if (len > (sizeof(_path) - 1)) {
        errno = ENAMETOOLONG;
        return;
    }
    strcpy(_path, dir);

    for (p = _path + 1; *p; p++) {
        if (*p != '/') {
            continue;
        }
        *p = '\0';
        if (mkdir(_path, mode) != 0) {
            if (errno != EEXIST)
                return;
        }
        *p = '/';
    }

    mkdir(_path, mode);
}
