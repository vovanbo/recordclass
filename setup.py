#!/usr/bin/env python
# -*- coding: utf-8 -*-

# The MIT License (MIT)
#
# Copyright (c) 2011-2017 Shibzukhov Zaur <szport at gmail dot com>,
#                         Vladimir Bolshakov <vovanbo at gmail dot com>
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

from setuptools import setup
from setuptools.command.build_ext import build_ext
from setuptools.extension import Extension

EXTENSIONS = [
    Extension(
        "trafaretrecord.memoryslots",
        ["trafaretrecord/memoryslots.c"]
    ),
]

with open('README.rst') as readme_file:
    README = readme_file.read()

with open('HISTORY.rst') as history_file:
    HISTORY = history_file.read()

REQUIREMENTS = [
    'typing',
]

TEST_REQUIREMENTS = [
    'pytest>=3.2'
]


setup(
    name='trafaretrecord',
    version='0.1.2',
    description='Fork of recordclass '
                '(mutable variant of collections.namedtuple, '
                'which supports assignments)',
    long_description=README + '\n\n' + HISTORY,
    author='Vladimir Bolshakov',
    author_email='vovanbo@gmail.com',
    url='https://github.com/vovanbo/trafaretrecord',
    packages=['trafaretrecord'],
    include_package_data=True,
    install_requires=REQUIREMENTS,
    license="MIT License",
    zip_safe=False,
    cmdclass={'build_ext': build_ext},
    ext_modules=EXTENSIONS,
    download_url='https://github.com/vovanbo/trafaretrecord',
    platforms='Linux, Mac OS X, Windows',
    keywords=['namedtuple', 'record', 'struct'],
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Operating System :: OS Independent',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
    test_suite='tests',
    tests_require=TEST_REQUIREMENTS
)
