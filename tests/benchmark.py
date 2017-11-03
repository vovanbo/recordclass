#!/usr/bin/env python
from collections import namedtuple
from timeit import timeit

from trafaretrecord import trafaretrecord, TrafaretRecord

ITERATIONS = 100000000


STest = namedtuple("TEST", "a b c")
sa = STest(a=1, b=2, c=3)

RTest = trafaretrecord("RTEST", "a b c")
ra = RTest(a=1, b=2, c=3)


class TRAgain(TrafaretRecord):
    a: int
    b: int
    c: int


tra = TRAgain(a=1, b=2, c=3)


class Test(object):
    __slots__ = ("a", "b", "c")

    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c


b = Test(1, 2, 3)

c = {'a': 1, 'b': 2, 'c': 3}

d = (1, 2, 3)
e = [1, 2, 3]
f = (1, 2, 3)
g = [1, 2, 3]
key = 2

print('Start %s iterations:\n' % ITERATIONS)

print("Named tuple with a, b, c:")
print(timeit("z = sa.c", "from __main__ import sa", number=ITERATIONS))

print("trafaretrecord with a, b, c:")
print(timeit("z = ra.c", "from __main__ import ra", number=ITERATIONS))

print("TrafaretRecord with a, b, c:")
print(timeit("z = tra.c", "from __main__ import tra", number=ITERATIONS))

print("Named tuple, using index:")
print(timeit("z = sa[2]", "from __main__ import sa", number=ITERATIONS))

print("Class using __slots__, with a, b, c:")
print(timeit("z = b.c", "from __main__ import b", number=ITERATIONS))

print("Dictionary with keys a, b, c:")
print(timeit("z = c['c']", "from __main__ import c", number=ITERATIONS))

print("Tuple with three values, using a constant key:")
print(timeit("z = d[2]", "from __main__ import d", number=ITERATIONS))

print("List with three values, using a constant key:")
print(timeit("z = e[2]", "from __main__ import e", number=ITERATIONS))

print("Tuple with three values, using a local key:")
print(timeit("z = d[key]", "from __main__ import d, key", number=ITERATIONS))

print("List with three values, using a local key:")
print(timeit("z = e[key]", "from __main__ import e, key", number=ITERATIONS))
