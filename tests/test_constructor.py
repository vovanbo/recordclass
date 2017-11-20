"""Unit tests for recordclass.py."""

import copy
import keyword
import pickle
import re

import pytest

from trafaretrecord import trafaretrecord

try:
    from test import support
except:
    from test import test_support as support

SomeClass = trafaretrecord('SomeClass', 'x y z')  # type used for pickle tests


def test_factory():
    Point = trafaretrecord('Point', 'x y')
    assert Point.__name__ == 'Point'
    assert Point.__slots__ == ()
    assert Point.__module__ == __name__
    assert Point._fields == ('x', 'y')
    assert 'class Point(memoryslots)' in Point._source

    trafaretrecord('Point0', 'x1 y2')  # Verify that numbers are allowed in names
    trafaretrecord('_', 'a b c')  # Test leading underscores in a typename

    nt = trafaretrecord('nt', 'the quick brown fox')  # check unicode input
    assert "u'" not in repr(nt._fields)
    nt = trafaretrecord('nt', ('the', 'quick'))  # check unicode input
    assert "u'" not in repr(nt._fields)


@pytest.mark.parametrize(['name', 'fields'], [
    ('abc%', 'efg ghi'),  # type has non-alpha char
    ('class', 'efg ghi'),  # type has keyword
    ('9abc', 'efg ghi'),  # type starts with digit
    ('abc', 'efg g%hi'),  # field with non-alpha char
    ('abc', 'abc class'),  # field has keyword
    ('abc', '8efg 9ghi'),  # field starts with digit
    ('abc', '_efg ghi'),  # field with leading underscore
    ('abc', 'efg efg ghi')  # duplicate field
])
def test_factory_fail(name, fields):
    with pytest.raises(ValueError):
        trafaretrecord(name, fields)


@pytest.mark.parametrize('iterable', [
    [11],  # catch too few args
    [11, 22, 33]  # catch too many args
])
def test_factory_make_fail(iterable):
    Point = trafaretrecord('Point', 'x y')

    with pytest.raises(TypeError):
        Point._make(iterable)


def test_factory_doc_attr():
    Point = trafaretrecord('Point', 'x y')
    assert Point.__doc__ == 'Point(x, y)'


@pytest.mark.parametrize(['spec', 'renamed'], [
    [('efg', 'g%hi'), ('efg', '_1')],  # field with non-alpha char
    [('abc', 'class'), ('abc', '_1')],  # field has keyword
    [('8efg', '9ghi'), ('_0', '_1')],  # field starts with digit
    [('abc', '_efg'), ('abc', '_1')],  # field with leading underscore
    # duplicate field
    [('abc', 'efg', 'efg', 'ghi'), ('abc', 'efg', '_2', 'ghi')],
    [('abc', '', 'x'), ('abc', '_1', 'x')],  # fieldname is a space
])
def test_name_fixer(spec, renamed):
    assert trafaretrecord('NT', spec, rename=True)._fields == renamed


def test_instance():
    Point = trafaretrecord('Point', 'x y')
    p = Point(11, 22)
    assert p == Point(x=11, y=22)
    assert p == Point(11, y=22)
    assert p == Point(y=22, x=11)
    assert p == Point(*(11, 22))
    assert p == Point(**dict(x=11, y=22))

    with pytest.raises(TypeError):
        eval('Point(XXX=1, y=2)', locals())  # wrong keyword argument
    with pytest.raises(TypeError):
        eval('Point(x=1)', locals())  # missing keyword argument

    assert repr(p) == 'Point(x=11, y=22)'
    # self.assertNotIn('__weakref__', dir(p))
    # print(p)
    assert p == Point._make([11, 22])  # test _make classmethod
    assert p._fields == ('x', 'y')  # test _fields attribute
    assert p._replace(x=1) == (1, 22)  # test _replace method
    assert p._asdict() == dict(x=1, y=22)  # test _asdict method
    assert vars(p) == p._asdict()  # verify that vars() works

    p.x = 1
    assert p.x == 1

    p.x = 2
    assert p.x == 2

    p._replace(x=3)
    assert p.x == 3

    # verify that field string can have commas
    Point = trafaretrecord('Point', 'x, y')
    p = Point(x=11, y=22)
    assert repr(p) == 'Point(x=11, y=22)'

    # verify that fieldspec can be a non-string sequence
    Point = trafaretrecord('Point', ('x', 'y'))
    p = Point(x=11, y=22)
    assert repr(p) == 'Point(x=11, y=22)'


def test_tupleness():
    Point = trafaretrecord('Point', 'x y')
    p = Point(11, 22)

    assert tuple(p) == (11, 22)  # coercable to a real tuple
    assert list(p) == [11, 22]  # coercable to a list
    assert max(p) == 22  # iterable
    assert max(*p) == 22  # star-able
    x, y = p
    assert tuple(p) == (x, y)  # unpacks like a tuple
    assert (p[0], p[1]) == (11, 22)  # indexable like a tuple

    with pytest.raises(IndexError):
        p.__getitem__(3)

    assert p.x == x
    assert p.y == y

    with pytest.raises(AttributeError):
        eval('p.z', locals())


def test_odd_sizes():
    import string, random

    Zero = trafaretrecord('Zero', '')
    assert Zero() == ()
    assert Zero._make([]) == ()
    assert repr(Zero()) == 'Zero()'
    assert Zero()._asdict() == {}
    print(Zero())
    assert Zero()._fields == ()

    Dot = trafaretrecord('Dot', 'd')
    assert Dot(1) == (1,)
    assert Dot._make([1]) == (1,)
    assert Dot(1).d == 1
    assert repr(Dot(1)) == 'Dot(d=1)'
    assert Dot(1)._asdict() == {'d': 1}
    assert Dot(1)._replace(d=999) == (999,)
    assert Dot(1)._fields == ('d',)

    # n = 5000
    n = 254  # SyntaxError: more than 255 arguments:
    names = list(set(''.join([random.choice(string.ascii_letters)
                              for j in range(10)]) for i in range(n)))
    n = len(names)
    Big = trafaretrecord('Big', names)
    b = Big(*range(n))

    assert b == tuple(range(n))
    assert Big._make(range(n)) == tuple(range(n))

    for pos, name in enumerate(names):
        assert getattr(b, name) == pos

    repr(b)  # make sure repr() doesn't blow-up
    d = b._asdict()
    d_expected = dict(zip(names, range(n)))
    assert d == d_expected

    b2 = b._replace(**dict([(names[1], 999), (names[-5], 42)]))
    b2_expected = list(range(n))
    b2_expected[1] = 999
    b2_expected[-5] = 42

    assert b2 == tuple(b2_expected)
    assert b._fields == tuple(names)


@pytest.mark.parametrize('module', [pickle, ])
def test_pickle(module):
    p = SomeClass(x=10, y=20, z=30)
    loads = getattr(module, 'loads')
    dumps = getattr(module, 'dumps')
    for protocol in range(-1, module.HIGHEST_PROTOCOL + 1):
        tmp = dumps(p, protocol)
        q = loads(tmp)
        assert p == q
        assert p._fields == q._fields
        assert b'OrderedDict' not in dumps(p, protocol)


@pytest.mark.parametrize('copier', [copy.copy, copy.deepcopy])
def test_copy(copier):
    p = SomeClass(x=10, y=20, z=30)
    q = copier(p)
    assert p == q
    assert p._fields == q._fields


def test_name_conflicts():
    # Some names like "self", "cls", "tuple", "itemgetter", and "property"
    # failed when used as field names.  Test to make sure these now work.
    T = trafaretrecord('T', 'itemgetter property self cls tuple')
    t = T(1, 2, 3, 4, 5)
    assert t == (1, 2, 3, 4, 5)
    newt = t._replace(itemgetter=10, property=20, self=30, cls=40, tuple=50)
    assert newt == (10, 20, 30, 40, 50)

    # Broader test of all interesting names in a template
    with support.captured_stdout() as template:
        T = trafaretrecord('T', 'x', verbose=True)

    words = set(re.findall('[A-Za-z]+', template.getvalue()))
    words -= set(keyword.kwlist)
    words = list(words)

    if 'None' in words:
        words.remove('None')

    T = trafaretrecord('T', words)
    # test __new__
    values = tuple(range(len(words)))
    t = T(*values)
    assert t == values
    t = T(**dict(zip(T._fields, values)))
    assert t == values
    # test _make
    t = T._make(values)
    assert t == values
    # exercise __repr__
    repr(t)
    # test _asdict
    assert t._asdict() == dict(zip(T._fields, values))
    # test _replace
    t = T._make(values)
    newvalues = tuple(v * 10 for v in values)
    newt = t._replace(**dict(zip(T._fields, newvalues)))
    assert newt == newvalues
    # test _fields
    assert T._fields == tuple(words)
    # test __getnewargs__
    assert t.__getnewargs__() == newvalues


def test_repr():
    with support.captured_stdout() as template:
        A = trafaretrecord('A', 'x', verbose=True)
    assert repr(A(1)) == 'A(x=1)'

    # repr should show the name of the subclass
    class B(A):
        pass

    assert repr(B(1)) == 'B(x=1)'


def test_source():
    # verify that _source can be run through exec()
    tmp = trafaretrecord('NTColor', 'red green blue')
    globals().pop('NTColor', None)  # remove artifacts from other tests
    exec(tmp._source, globals())
    assert 'NTColor' in globals()

    c = NTColor(10, 20, 30)
    assert (c.red, c.green, c.blue) == (10, 20, 30)
    assert NTColor._fields == ('red', 'green', 'blue')

    globals().pop('NTColor', None)  # clean-up after this test
