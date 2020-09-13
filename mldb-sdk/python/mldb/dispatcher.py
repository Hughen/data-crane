# -*- coding: utf-8 -*-
from mldb_sdk import IODispatcher as _sdk_IODispatcher

class IODispatcher(object):
    def __init__(self, uri: str) -> None:
        self.__io_dispatcher = _sdk_IODispatcher(uri)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        del self.__io_dispatcher

    def read(self) -> bytes:
        return self.__io_dispatcher.read()

    # DEPRECATED
    def _filePath(self) -> str:
        return self.__io_dispatcher._getFilePath()
