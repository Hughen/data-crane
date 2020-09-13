#include <fstream>
#include <streambuf>
#include <cstring>
#include "file.h"
#include "log/log.h"

using std::ifstream;
using std::ios;
using std::istreambuf_iterator;

char* readAllBytes(const string& fname, int* const& read_size) {
    ifstream fd(fname, ios::binary | ios::ate);
    if (!fd) {
        *read_size = -1;
        return nullptr;
    }

    fd.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    ifstream::pos_type pos = fd.tellg();
    *read_size = pos;
    if (pos == -1) {
        fd.close();
        return nullptr;
    }

    char* buf = nullptr;
    // secure for closing and destructe buffer
    try {
        buf = new char[pos];
        fd.seekg(0, ios::beg);
        fd.read(buf, pos);
        fd.close();
    } catch (const std::ifstream::failure& e) {
        dlog(Error) << "exception from read file: " << e.what() << lendl;
        if (!buf) {
            delete[] buf;
            buf = nullptr;
        }
    }

    return buf;
}

const string readAllStrvalBytes(const string& fname, int* const& read_size) {
    ifstream fd(fname, ios::binary | ios::ate);
    if (!fd) {
        *read_size = -1;
        return string();
    }

    fd.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
    string buf;
    ifstream::pos_type pos = fd.tellg();
    *read_size = pos;
    if (pos == -1) {
        fd.close();
        return buf;
    }

    buf.reserve(pos);

    try {
        fd.seekg(0, ios::beg);
        buf.assign((istreambuf_iterator<char>(fd)),
            istreambuf_iterator<char>());
        fd.close();
    } catch (const std::ifstream::failure& e) {
        dlog(Error) << "exception from read file: " << e.what() << lendl;
    }

    return buf;
}
