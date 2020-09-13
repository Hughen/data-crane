#include <pybind11/pybind11.h>
#include "python.h"

using py::return_value_policy;

PYBIND11_MODULE(mldb_sdk, m) {
    m.doc() = "Machine Learning Database SDK";

    m.def("init", &sdk_init, "Initialize the SDK module");

    py::class_<PyIODispatcher>(m, "IODispatcher")
        .def(py::init<const string&>())
        .def("read", &PyIODispatcher::read, return_value_policy::take_ownership)
        .def("write", &PyIODispatcher::write,
            py::arg_v("content", py::bytes(""), "bytes of the file")
                .none(false))
        .def("_getFilePath", &PyIODispatcher::_filePath);

    py::class_<Prefetcher>(m, "Prefetcher")
        .def(py::init<>())
        .def(py::init<const set<string>&>())
        .def("append", &Prefetcher::Append, py::arg("lis"))
        .def("start", &Prefetcher::Start);

    m.def("open", &open, py::arg("uri"), "Open a file in MLDB");
    m.def("list_batch", &list_batch, "List batch resources from MLDB");

    #ifdef VERSION
    m.attr("__version__") = VERSION;
    #endif
}
