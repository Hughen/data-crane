#ifndef _MLDB_PYTHON_H_     /* NOLINT */
#define _MLDB_PYTHON_H_

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <string>
#include "sdk.h"

namespace py = pybind11;

using std::string;

class PyIODispatcher {
 public:
    explicit PyIODispatcher(const string& raw_uri) : base_io(raw_uri) {}
    virtual ~PyIODispatcher() {}

    py::bytes read() {
        int rsize = 0;
        char* raw_buf = this->base_io.read(&rsize);
        py::bytes rb(raw_buf, rsize);
        delete[] raw_buf;
        return rb;
    }
    int write(const char* content) {
        return this->base_io.write(content);
    }

    // DEPRECATED
    // temporary API
    string _filePath() {
        return this->base_io._filePath();
    }

 private:
    IODispatcher base_io;
};

#endif  // _MLDB_PYTHON_H_      /* NOLINT */
