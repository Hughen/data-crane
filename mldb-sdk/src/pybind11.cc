#include "sdk.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

using py::return_value_policy;

PYBIND11_MODULE(mldb, m) {
    m.doc() = "Machine Learning Database SDK";

    m.def("init", &sdk_init, "Initialize the SDK package");

    py::class_<IODispatcher>(m, "IODispatcher")
        .def(py::init<const string&>())
        .def("read", &IODispatcher::read, return_value_policy::take_ownership)
        .def("write", &IODispatcher::write);

    m.def("open", &open, py::arg("uri"), "Open a file in MLDB");
    m.def("list_batch", &list_batch, "List batch resources from MLDB");

    #ifdef VERSION
    m.attr("__version__") = VERSION;
    #endif
}
