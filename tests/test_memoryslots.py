import pickle

import pytest
from trafaretrecord.memoryslots import memoryslots


def test_constructors():
    assert memoryslots() == ()
    assert memoryslots([]) == memoryslots([])
    assert memoryslots(0, 1, 2, 3) == memoryslots(0, 1, 2, 3)
    assert memoryslots('') == memoryslots('')


def test_truth():
    assert not memoryslots()
    assert memoryslots([42])


def test_len():
    assert len(memoryslots()) == 0
    assert len(memoryslots(0)) == 1
    assert len(memoryslots(0, 1, 2)) == 3


def test_memoryslotsresizebug():
    # Check that a specific bug in _PyTuple_Resize() is squashed.
    def f():
        for i in range(1000):
            yield i
    assert list(memoryslots(*f())) == list(range(1000))


@pytest.mark.parametrize('type2test', [memoryslots, ])
def test_repr(type2test):
    l0 = memoryslots()
    l2 = memoryslots(0, 1, 2)
    a0 = type2test(*l0)
    a2 = type2test(*l2)

    assert str(a0) == repr(l0)
    assert str(a2) == repr(l2)
    assert repr(a0) == "memoryslots()"
    assert repr(a2) == "memoryslots(0, 1, 2)"


@pytest.mark.parametrize('n', [10, 1000000])
def test_repr_large(n):
    # Check the repr of large list objects
    l = (0,) * n
    s = repr(l)
    assert s == '(' + ', '.join(['0'] * n) + ')'


@pytest.mark.parametrize('type2test', [memoryslots, ])
def test_iterator_pickle(type2test):
    # Userlist iterators don't support pickling yet since
    # they are based on generators.
    data = type2test([4, 5, 6, 7])

    for proto in range(pickle.HIGHEST_PROTOCOL + 1):
        itorg = iter(data)
        d = pickle.dumps(itorg, proto)
        it = pickle.loads(d)
        assert type(itorg) == type(it)
        assert type2test(*it) == type2test(data)

    it = pickle.loads(d)
    next(it)
    d = pickle.dumps(it)
    assert type2test(*it) == type2test(data)[1:]


@pytest.mark.parametrize('type2test', [memoryslots, ])
def test_reversed_pickle(type2test):
    data = type2test([4, 5, 6, 7])
    for proto in range(pickle.HIGHEST_PROTOCOL + 1):
        itorg = reversed(data)
        d = pickle.dumps(itorg, proto)
        it = pickle.loads(d)
        assert type(itorg) == type(it)
        assert type2test(*it) == type2test(tuple(reversed(data)))

        it = pickle.loads(d)
        next(it)
        d = pickle.dumps(it, proto)
        assert type2test(*it) == type2test(tuple(reversed(data)))[1:]


def test_no_comdat_folding():
    # Issue 8847: In the PGO build, the MSVC linker's COMDAT folding
    # optimization causes failures in code that relies on distinct
    # function addresses.
    class T(memoryslots): pass

    with pytest.raises(TypeError):
        [3,] + T((1,2))
