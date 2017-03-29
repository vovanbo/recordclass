=============
trafaretclass
=============

This is a fork of **recordclass** (`bitbucket <https://bitbucket.org/intellimath/recordclass>`_) by Zaur Shibzukhov.

**trafaretclass** is `MIT Licensed <http://opensource.org/licenses/MIT>`_ python library.
It implements the type ``memoryslots`` and factory function ``trafaretclass`` 
in order to create record-like classes. 

* ``memoryslots`` is ``tuple``-like type, which supports assignment operations. 
* ``trafaretclass`` is a factory function that create a "mutable" analog of 
  ``collection.namedtuple``.

This library actually is a "proof of concept" for the problem of fast "mutable" 
alternative of ``namedtuple``.
