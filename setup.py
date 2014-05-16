#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Andre Anjos <andre.anjos@idiap.ch>
# Mon 16 Apr 08:18:08 2012 CEST

from setuptools import setup, find_packages, dist
dist.Distribution(dict(setup_requires=['xbob.blitz', 'xbob.io.base']))
from xbob.blitz.extension import Extension
import xbob.io.base

include_dirs = [xbob.io.base.get_include()]

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

    name='xbob.io.video',
    version=version,
    description='Video I/O support for Bob',
    url='http://github.com/bioidiap/xbob.io.video',
    license='BSD',
    author='Andre Anjos',
    author_email='andre.anjos@idiap.ch',

    long_description=open('README.rst').read(),

    packages=find_packages(),
    include_package_data=True,

    install_requires=[
      'setuptools',
      'xbob.blitz',
      'xbob.io.base',
    ] + pil_or_pillow,

    namespace_packages=[
      "xbob",
      "xbob.io",
      ],

    ext_modules = [
      Extension("xbob.io.video.version",
        [
          "xbob/io/video/version.cpp",
          ],
        packages = packages,
        boost_modules = ['system'],
        include_dirs = include_dirs,
        version = version,
        define_macros = [('__STDC_CONSTANT_MACROS', None)],
        ),
      Extension("xbob.io.video._library",
        [
          "xbob/io/video/cpp/utils.cpp",
          "xbob/io/video/cpp/reader.cpp",
          "xbob/io/video/cpp/writer.cpp",
          "xbob/io/video/bobskin.cpp",
          "xbob/io/video/reader.cpp",
          "xbob/io/video/writer.cpp",
          "xbob/io/video/file.cpp",
          "xbob/io/video/main.cpp",
          ],
        packages = packages,
        include_dirs = include_dirs,
        version = version,
        define_macros = [('__STDC_CONSTANT_MACROS', None)],
        ),
      ],

    entry_points={
      'console_scripts': [
        'xbob_video_test.py = xbob.io.video.script.video_test:main',
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
