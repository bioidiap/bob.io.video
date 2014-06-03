#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 16 Apr 08:18:08 2012 CEST

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['bob.blitz', 'bob.io.base']))
from bob.blitz.extension import Extension
import bob.io.base

include_dirs = [bob.io.base.get_include()]

packages = [
  'boost',
  'bob-io >= 2.0.0a2',
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
        include_dirs = include_dirs,
        version = version,
        define_macros = [('__STDC_CONSTANT_MACROS', None)],
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
        include_dirs = include_dirs,
        version = version,
        define_macros = [('__STDC_CONSTANT_MACROS', None)],
        ),
      ],

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
