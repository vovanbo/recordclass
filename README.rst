===========
recordclass
===========

**recordclass** is an `MIT Licensed <http://opensource.org/licenses/MIT>`_ python library.
It implements the type ``mutabletuple`` factory function ``recordclass`` 
in order to create record-like class. 

* ``mutabletuple`` is specialization of the `tuple` type 
  (actually it's currently subclass of ``tuple``). 
  It's actually an *array of objects* with fixed size.
* ``recordclass`` is a "mutable" variant of the ``collections.namedtuple``. 
  ``recordclass`` bases on the ``mutabletuple`` and has the same API as 
  ``collections.namedtuple``.

This library actually is a "proof of concept" for the problem of "mutable" alternative
of ``namedtuple``.

Main repository for ``recordclass`` is on `bitbucket <https://bitbucket.org/intellimath/recordclass>`_.


