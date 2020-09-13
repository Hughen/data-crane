# -*- coding: utf-8 -*-
from mldb_sdk import list_batch as _sdk_list_batch
from typing import Set as _py_Set
from . import dispatcher

def open(uri: str) -> dispatcher.IODispatcher:
    return dispatcher.IODispatcher(uri)

def list_batch() -> _py_Set[str]:
    return _sdk_list_batch()
