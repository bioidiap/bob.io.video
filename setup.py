#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 16 Apr 08:18:08 2012 CEST

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.blitz', 'bob.core', 'bob.io.base']))
from bob.blitz.extension import Extension, Library, build_ext
from bob.extension import pkgconfig

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

version = '2.0.0a0'

# Check if python-imaging means pil or pillow
pil_or_pillow = []
try:
  import pkg_resources
  pkg_resources.require('PIL')
  pil_or_pillow.append('pil')
except pkg_resources.DistributionNotFound as e:
  pil_or_pillow.append('pillow')

define_macros = [('__STDC_CONSTANT_MACROS', None)]

# Checks if we have avformat_alloc_output_context2 defined in libavformat
libavformat_pkg = pkgconfig('libavformat >= 52.31.0')
import ctypes
import ctypes.util
lib = ctypes.util.find_library(libavformat_pkg.libraries()[0])
if lib is not None:
  try:
    dll = ctypes.CDLL(lib)
    if hasattr(dll, 'avformat_alloc_output_context2'):
      define_macros.append(('HAVE_AVFORMAT_ALLOC_OUTPUT_CONTEXT2', None))
  except OSError:
    pass #ignore it

setup(

    name='bob.io.video',
    version=version,
    description='Video I/O support for Bob',
    url='http://github.com/bioidiap/bob.io.video',
    license='BSD',
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,

    install_requires=[
      'setuptools',
      'bob.blitz',
      'bob.io.base',
    ] + pil_or_pillow,

    namespace_packages=[
      "bob",
      "bob.io",
      ],

    ext_modules = [
      Extension("bob.io.video.version",
        [
          "bob/io/video/version.cpp",
          ],
        packages = packages,
        boost_modules = ['system'],
        bob_packages = ['bob.core', 'bob.io.base'],
        version = version,
        define_macros = define_macros,
        ),

      Library("bob_io_video",
        [
          "bob/io/video/cpp/utils.cpp",
          "bob/io/video/cpp/reader.cpp",
          "bob/io/video/cpp/writer.cpp",
        ],
        package_directory = package_dir,
        target_directory = target_dir,
        define_macros = define_macros,
        version = version,
        bob_packages = ['bob.core', 'bob.io.base'],
        packages = packages,
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
        bob_packages = ['bob.core', 'bob.io.base'],
        libraries = ['bob_io_video'],
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
      'Development Status :: 3 - Alpha',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Programming Language :: Python',
      'Programming Language :: Python :: 3',
      'Topic :: Software Development :: Libraries :: Python Modules',
      'Environment :: Plugins',
      ],

    )
