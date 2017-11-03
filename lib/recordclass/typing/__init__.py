from recordclass.record import recordclass
from typing import _type_check

import sys as _sys

def _make_recordclass(name, types):
    msg = "RecordClass('Name', [(f0, t0), (f1, t1), ...]); each t must be a type"
    types = [(n, _type_check(t, msg)) for n, t in types]
    rec_cls = recordclass(name, [n for n, t in types])
    rec_cls._field_types = dict(types)
    try:
        rec_cls.__module__ = _sys._getframe(2).f_globals.get('__name__', '__main__')
    except (AttributeError, ValueError):
        pass
    return rec_cls

class RecordClassMeta(type):
    def __new__(cls, typename, bases, ns):
        if ns.get('_root', False):
            return super().__new__(cls, typename, bases, ns)
        # if not _PY36:
        #     raise TypeError("Class syntax for RecordClass is only supported"
        #                     " in Python 3.6+")
        types = ns.get('__annotations__', {})
        return _make_recordclass(typename, types.items())


class RecordClass(metaclass=RecordClassMeta):
    _root = True

    def __new__(self, typename, fields=None, **kwargs):
        # if kwargs and not _PY36:
        #     raise TypeError("Keyword syntax for RecordClass is only supported"
        #                     " in Python 3.6+")
        if fields is None:
            fields = kwargs.items()
        elif kwargs:
            raise TypeError("Either list of fields or keywords"
                            " can be provided to RecordClass, not both")
        return _make_recordclass(typename, fields)
