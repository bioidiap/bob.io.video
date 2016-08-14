#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 16 Apr 08:18:08 2012 CEST

bob_packages = ['bob.core', 'bob.io.base']

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.extension', 'bob.blitz'] + bob_packages))
from bob.blitz.extension import Extension, build_ext
from bob.extension import pkgconfig, find_library

from bob.extension.utils import load_requirements
build_requires = load_requirements()

# Define package version
version = open("version.txt").read().rstrip()

import os
package_dir = os.path.dirname(os.path.realpath(__file__))
target_dir = os.path.join(package_dir, 'bob', 'io', 'video')

packages = [
  'boost',
  'libavformat >= 52.31.0',
  'libavcodec >= 52.20.0',
  'libavutil >= 49.15.0',
  'libswscale >= 0.7.1'
  ]

define_macros = [('__STDC_CONSTANT_MACROS', None)]

# Checks if we have avformat_alloc_output_context2 defined in libavformat
libavformat_pkg = pkgconfig('libavformat >= 52.31.0')
import ctypes
lib = find_library(
    name=libavformat_pkg.libraries()[0],
    version=libavformat_pkg.version,
    prefixes=libavformat_pkg.library_directories()
    )
if lib:
  try:
    dll = ctypes.CDLL(lib[0])
    if hasattr(dll, 'avformat_alloc_output_context2'):
      define_macros.append(('HAVE_AVFORMAT_ALLOC_OUTPUT_CONTEXT2', None))
  except OSError:
    pass #ignore it

setup(

    name='bob.io.video',
    version=version,
    description='Video I/O support for Bob',
    url='http://gitlab.idiap.ch/bob/bob.io.video',
    license='BSD',
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,

    setup_requires = build_requires,
    install_requires = build_requires,



    ext_modules = [
      Extension("bob.io.video.version",
        [
          "bob/io/video/version.cpp",
        ],
        packages = packages,
        boost_modules = ['system'],
        bob_packages = bob_packages,
        version = version,
        define_macros = define_macros,
      ),

      Extension("bob.io.video._library",
        [
          "bob/io/video/cpp/utils.cpp",
          "bob/io/video/cpp/reader.cpp",
          "bob/io/video/cpp/writer.cpp",
          "bob/io/video/bobskin.cpp",
          "bob/io/video/reader.cpp",
          "bob/io/video/writer.cpp",
          "bob/io/video/file.cpp",
          "bob/io/video/main.cpp",
        ],
        packages = packages,
        boost_modules = ['system'],
        bob_packages = bob_packages,
        version = version,
        define_macros = define_macros,
      ),
    ],

    cmdclass = {
      'build_ext': build_ext
    },

    entry_points={
      'console_scripts': [
        'bob_video_test.py = bob.io.video.script.video_test:main',
      ],
    },

    classifiers = [
      'Framework :: Bob',
      'Development Status :: 4 - Beta',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Programming Language :: Python :: 3',
      'Topic :: Software Development :: Libraries :: Python Modules',
      'Environment :: Plugins',
    ],

  )
