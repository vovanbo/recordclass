# coding: utf-8

# The MIT License (MIT)
# 
# Copyright (c) <2011-2015> <Shibzukhov Zaur, szport at gmail dot com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import sys as _sys
_PY36 = _sys.version_info[:2] >= (3, 6)

from setuptools import setup
from setuptools.command.build_ext import build_ext
from setuptools.extension import Extension

ext_modules = [
    Extension(
        "recordclass.memoryslots",
        ["lib/recordclass/memoryslots.c"]
    ),
]

long_description = open('README.rst').read()

packages=['recordclass', 'recordclass.test']
if _PY36:
    packages.append('recordclass.test.typing')
    packages.append('recordclass.typing')

setup(
    name='recordclass',
    version='0.4.3',
    description='Mutable variant of collections.namedtuple, which supports assignments',
    author='Zaur Shibzukhov',
    author_email='szport@gmail.com',
    # maintainer = 'Zaur Shibzukhov',
    # maintainer_email = 'szport@gmail.com',
    license="MIT License",
    cmdclass={'build_ext': build_ext},
    ext_modules=ext_modules,
    package_dir={'': 'lib'},
    packages=packages,
    url='http://intellimath.bitbucket.org/recordclass',
    download_url='https://bitbucket.org/intellimath/recordclass',
    long_description=long_description,
    platforms='Linux, Mac OS X, Windows',
    keywords=['namedtuple', 'record', 'struct'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Operating System :: OS Independent',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
)
