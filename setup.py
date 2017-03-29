#!/usr/bin/env python
# -*- coding: utf-8 -*-

from setuptools import setup
from setuptools.command.build_ext import build_ext
from setuptools.extension import Extension

ext_modules = [
    Extension(
        "trafaretrecord.memoryslots",
        ["trafaretrecord/memoryslots.c"]
    ),
]

with open('README.rst') as readme_file:
    readme = readme_file.read()

with open('HISTORY.rst') as history_file:
    history = history_file.read()

requirements = [
    # TODO: put package requirements here
]

test_requirements = [
    'pytest>=3.0.7,<3.1'
]


setup(
    name='trafaretrecord',
    version='0.1.0',
    description='Fork of recordclass (mutable variant of collections.namedtuple, which supports assignments)',
    long_description=readme + '\n\n' + history,
    author='Vladimir Bolshakov',
    author_email='vovanbo@gmail.com',
    url='https://github.com/vovanbo/trafaretrecord',
    packages=[
        'trafaretrecord',
    ],
    package_dir={'trafaretrecord': 'trafaretrecord'},
    include_package_data=True,
    install_requires=requirements,
    license="MIT License",
    zip_safe=False,
    cmdclass={'build_ext': build_ext},
    ext_modules=ext_modules,
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
    tests_require=test_requirements
)
