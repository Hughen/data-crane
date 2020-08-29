#include "file.h"
#include <fstream>

using std::ifstream;
using std::ios;

char* readAllBytes(const string& fname, int& read_size) {
    ifstream fd(fname, ios::binary | ios::ate);

    char* buf = NULL;
    // secure for closing and destructe buffer
    try {
        ifstream::pos_type pos = fd.tellg();
        buf = new char[pos];
        fd.seekg(0, ios::beg);
        fd.read(buf, pos);
        read_size = pos;
    } catch (...) {
        if (buf != NULL) {
            delete[] buf;
            buf = NULL;
        }
    }

    fd.close();
    return buf;
}
