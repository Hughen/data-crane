from inspect import isfunction
from typing import Any, List, Optional, Tuple, Union, Dict
from mldb import URI


class Mapper:
    callers = {}

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        raise NotImplementedError("mappper caller must be implemented")

    def __init_subclass__(cls, **kwargs) -> None:
        super().__init_subclass__(**kwargs)
        name = cls.name if hasattr(cls, "name") else cls.__class__.__name__
        if name in Mapper.callers:
            raise TypeError(f"the mapper {cls}({name}) already exists")
        Mapper.callers[name] = cls


def map_rule_register():
    pass


class Attachment:
    pass


class Field:
    def __init__(self, key: str) -> None:
        pass

    def __str__(self) -> str:
        pass


def default_no_convert(value: Any) -> Any:
    return value


class Attr:
    def __init__(self, name, converter = default_no_convert) -> None:
        super().__init__()
        self.name = name
        self.conv = converter

    def dump(self, value: Any) -> Tuple[str, Any]:
        return (self.name, self.conv(Any))


class URIAttr(Attr):
    def __init__(self) -> None:
        super().__init__("u", str)


class ItemAttr(Attr):
    def __init__(self) -> None:
        super().__init__("attr")


class RawLabelAttr(Attr):
    def __init__(self) -> None:
        super().__init__("v")


class LabelAttr(Attr):
    def __init__(self) -> None:
        super().__init__("u", str)


class FieldMeta(type):
    def __new__(cls, name, bases, classdict):
        print("Value", cls, name, bases, classdict)
        if name == "Field":
            return super().__new__(cls, name, bases, classdict)
        structs = {}
        for k, v in classdict.items():
            if not isinstance(v, Attr):
                raise ValueError("attributes of field must be a Attr")
            if k.startswith("_"):
                continue
            structs[k] = v
        for k in structs.items():
            classdict.pop(k)
        classdict["__structs__"] = structs
        return super().__new__(cls, name, bases, classdict)


class Field(dict, metaclass=FieldMeta):
    def __init__(self, *args, context = None, **kwargs):
        super().__init__(*args, **kwargs)
        self._context = context

    def __setattr__(self, key: str, value: Field) -> None:
        self[key] = value

    def __getattr__(self, key) -> Field:
        try:
            return self[key]
        except KeyError:
            raise AttributeError(f"Value object has no attribute '{key}'")

    def dump(self) -> Tuple[str, Union[str, Dict, List, int]]:
        result = {}
        for k, attr in self.__structs__.items():
            vk, vv = attr.dump(self[k])
            result[vk] = vv
        return result


class MLDBURIValue(dict, metaclass=FieldMeta):
    def __init__(self, value) -> None:
        print("hello MLDBURIValue")
        self._value = value

    def echo(self):
        print("MLDBURIValue.echo", self._value)


class Image(Field):
    image = URIAttr()
    attr = ItemAttr()


class RawLabel:
    lebel = RawLabelAttr()


class DataBatchRunner:
    def __init__(self, raw_uid: List[str], anno_uid: Optional[List[str]] = None, num_workers: int = 1) -> None:
        pass

    def set_attachment(self, name: str, map_rule: List[Mapper]) -> "DataBatchRunner":
        pass

    def set_workers(self, num_workers: int) -> "DataBatchRunner":
        pass


if __name__ == "__main__":
    v = MLDBURIValue(10)
    v.echo()
