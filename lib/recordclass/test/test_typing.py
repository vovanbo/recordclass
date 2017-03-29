"""Unit tests for recordclass.py."""
import typing
import unittest, doctest, operator
from recordclass import recordclass
#from collections import OrderedDict
#import pickle, copy
#import keyword
#import re
#import sys

from recordclass import RecordClass

try:
    from test import support
except:
    from test import test_support as support


class RecordClassTypingTest(unittest.TestCase):
    def test_typing(self):
        class A(RecordClass):
            __annotations__ = {'a':int, 'b':str, 'c':typing.List[int]}
            #a: int
            #b: str
            #c: typing.List[int]

        tmp = A(a=1, b='1', c=[1, 2, 3])
        self.assertEqual(repr(tmp), "A(a=1, b='1', c=[1, 2, 3])")
        self.assertEqual(tmp._field_types, {'a': int, 'b': str, 'c': typing.List[int]})

def main():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(RecordClassTypingTest))
    return suite
