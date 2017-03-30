import typing

from trafaretrecord import TrafaretRecord


def test_initialization():
    class A(TrafaretRecord):
        a: int
        b: str
        c: typing.List[int]

    tmp = A(a=1, b='1', c=[1, 2, 3])
    assert repr(tmp) == "A(a=1, b='1', c=[1, 2, 3])"
    assert tmp._field_types == {'a': int, 'b': str, 'c': typing.List[int]}


def test_class_properties():
    class A(TrafaretRecord):
        a: int
        b: str
        c: typing.List[int]

        @property
        def B(self):
            return self.b.upper()

        @B.setter
        def B(self, value):
            self._replace(b=value.lower())

        @staticmethod
        def static():
            return 'static method result'

        @classmethod
        def initialize(cls, *args, **kwargs):
            return cls(*args, **kwargs)

    tmp = A(a=1, b='b', c=[1, 2, 3])
    assert tmp.b == 'b'
    assert tmp.B == 'B'
    tmp.B = 'BB'
    assert tmp.b == 'bb'
    assert tmp.B == 'BB'
    assert tmp.static() == 'static method result'
    assert A.initialize(a=3, b='B', c=[4, 5, 6]) == A(a=3, b='B', c=[4, 5, 6])

