# -*- coding: utf-8 -*-
from mldb_sdk import Prefetcher as _sdk_Prefetcher
from typing import Set as _py_Set, Optional as _py_Optional

class Prefetcher(object):
    def __init__(self, list: _py_Optional[_py_Set[str]] = None) -> None:
        if list:
            self.__prefetcher = _sdk_Prefetcher(list)
        else:
            self.__prefetcher = _sdk_Prefetcher()

    def append(self, list: _py_Set[str]) -> "Prefetcher":
        self.__prefetcher.append(list)
        return self

    def start(self) -> int:
        return self.__prefetcher.start()
