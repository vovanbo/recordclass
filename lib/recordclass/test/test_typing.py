"""Unit tests for recordclass.py."""
import unittest, doctest, operator
from recordclass import recordclass

try:
    from test import support
except:
    from test import test_support as support

import sys as _sys
_PY36 = _sys.version_info[:2] >= (3, 6)

if _PY36:
    import typing
    from recordclass import RecordClass

    class RecordClassTypingTest(unittest.TestCase):
        def test_typing(self):
            class A(RecordClass):
                __annotations__ = {'a': int, 'b': str, 'c': typing.List[int]}

            tmp = A(a=1, b='1', c=[1, 2, 3])
            self.assertEqual(repr(tmp), "A(a=1, b='1', c=[1, 2, 3])")
            self.assertEqual(tmp._field_types, {'a': int, 'b': str, 'c': typing.List[int]})

    def main():
        suite = unittest.TestSuite()
        suite.addTest(unittest.makeSuite(RecordClassTypingTest))
        return suite
else:
    def main():
        pass