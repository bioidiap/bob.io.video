.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.anjos@idiap.ch>
.. Thu 29 Aug 2013 16:07:57 CEST

.. image:: http://img.shields.io/badge/docs-stable-yellow.png
   :target: http://pythonhosted.org/bob.io.video/index.html
.. image:: http://img.shields.io/badge/docs-latest-orange.png
   :target: https://www.idiap.ch/software/bob/docs/latest/bioidiap/bob.io.video/master/index.html
.. image:: https://travis-ci.org/bioidiap/bob.io.video.svg?branch=v2.0.8
   :target: https://travis-ci.org/bioidiap/bob.io.video?branch=v2.0.8
.. image:: https://coveralls.io/repos/bioidiap/bob.io.video/badge.svg?branch=v2.0.8
   :target: https://coveralls.io/r/bioidiap/bob.io.video?branch=v2.0.8
.. image:: https://img.shields.io/badge/github-master-0000c0.png
   :target: https://github.com/bioidiap/bob.io.video/tree/master
.. image:: http://img.shields.io/pypi/v/bob.io.video.png
   :target: https://pypi.python.org/pypi/bob.io.video
.. image:: http://img.shields.io/pypi/dm/bob.io.video.png
   :target: https://pypi.python.org/pypi/bob.io.video

===========================
 Video I/O Support for Bob
===========================

This package contains support for Video I/O in Bob.
Video reading and writing is implemented using either FFmpeg or LibAV, whichever is installed.
By importing this package, you activate a transparent plugin that makes possible reading and writing video files using ``bob.io.base`` functionalities.

Installation
------------
To install this package -- alone or together with other `Packages of Bob <https://github.com/idiap/bob/wiki/Packages>`_ -- please read the `Installation Instructions <https://github.com/idiap/bob/wiki/Installation>`_.
For Bob_ to be able to work properly, some dependent packages are required to be installed.
Please make sure that you have read the `Dependencies <https://github.com/idiap/bob/wiki/Dependencies>`_ for your operating system.

Documentation
-------------
For further documentation on this package, please read the `Stable Version <http://pythonhosted.org/bob.io.video/index.html>`_ or the `Latest Version <https://www.idiap.ch/software/bob/docs/latest/bioidiap/bob.io.video/master/index.html>`_ of the documentation.
For a list of tutorials on this or the other packages ob Bob_, or information on submitting issues, asking questions and starting discussions, please visit its website.

.. _bob: https://www.idiap.ch/software/bob
