.. vim: set fileencoding=utf-8 :
.. Sun 14 Aug 2016 18:25:23 CEST

.. image:: http://img.shields.io/badge/docs-stable-yellow.png
   :target: http://pythonhosted.org/bob.io.video/index.html
.. image:: http://img.shields.io/badge/docs-latest-orange.png
   :target: https://www.idiap.ch/software/bob/docs/latest/bob/bob.io.video/master/index.html
.. image:: https://gitlab.idiap.ch/bob/bob.io.video/badges/v2.0.12/build.svg
   :target: https://gitlab.idiap.ch/bob/bob.io.video/commits/v2.0.12
.. image:: https://img.shields.io/badge/gitlab-project-0000c0.svg
   :target: https://gitlab.idiap.ch/bob/bob.io.video
.. image:: http://img.shields.io/pypi/v/bob.io.video.png
   :target: https://pypi.python.org/pypi/bob.io.video
.. image:: http://img.shields.io/pypi/dm/bob.io.video.png
   :target: https://pypi.python.org/pypi/bob.io.video


===========================
 Video I/O Support for Bob
===========================

This package is part of the signal-processing and machine learning toolbox
Bob_. It contains support for Video I/O in Bob. Video reading and writing is
implemented using either FFmpeg or LibAV, whichever is installed. By importing
this package, you activate a transparent plugin that makes possible reading and
writing video files using ``bob.io.base`` functionalities.


Installation
------------

Follow our `installation`_ instructions. Then, using the Python interpreter
provided by the distribution, bootstrap and buildout this package::

  $ python bootstrap-buildout.py
  $ ./bin/buildout


Contact
-------

For questions or reporting issues to this software package, contact our
development `mailing list`_.


.. Place your references here:
.. _bob: https://www.idiap.ch/software/bob
.. _installation: https://gitlab.idiap.ch/bob/bob/wikis/Installation
.. _mailing list: https://groups.google.com/forum/?fromgroups#!forum/bob-devel
