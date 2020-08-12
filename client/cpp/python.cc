#include <Python.h>
#include "client.h"

PyObject* crane_new(PyObject* self, PyObject *args)
{
    myclient::Client* client = new myclient::Client();

    PyObject* pyClient = PyCapsule_New((void*)client, "myclient_client", NULL);
    PyCapsule_SetPointer(pyClient, (void*)client);

    return Py_BuildValue("O", pyClient);
}

PyObject* hello(PyObject* self, PyObject* args)
{
    PyObject *_oclient;
    PyArg_ParseTuple(args, "O", &_oclient);
    myclient::Client* client = (myclient::Client*)PyCapsule_GetPointer(_oclient, "myclient_client");

    int n = client->hello();

    return Py_BuildValue("i", n);
}

PyObject* close(PyObject* self, PyObject* args)
{
    PyObject *_oclient;
    PyArg_ParseTuple(args, "O", &_oclient);
    myclient::Client* client = (myclient::Client*)PyCapsule_GetPointer(_oclient, "myclient_client");

    delete client;

    return Py_BuildValue("");
}

PyMethodDef clientFunctions[] =
{
    {"crane_new", crane_new, METH_VARARGS, "create a test class"},
    {"hello", hello, METH_VARARGS, "call hello function"},

    {NULL, NULL, 0, NULL}
};

struct PyModuleDef clientModule =
{
    PyModuleDef_HEAD_INIT,
    "myclient",
    NULL,
    -1,
    clientFunctions
};

PyMODINIT_FUNC PyInit_myclient(void)
{
    return PyModule_Create(&clientModule);
}
