.. vim: set fileencoding=utf-8 :
.. Sun 14 Aug 2016 18:25:23 CEST

.. image:: http://img.shields.io/badge/docs-v3.0.1-yellow.svg
   :target: https://www.idiap.ch/software/bob/docs/bob/bob.io.video/v3.0.1/index.html
.. image:: http://img.shields.io/badge/docs-latest-orange.svg
   :target: https://www.idiap.ch/software/bob/docs/bob/bob.io.video/master/index.html
.. image:: https://gitlab.idiap.ch/bob/bob.io.video/badges/v3.0.1/build.svg
   :target: https://gitlab.idiap.ch/bob/bob.io.video/commits/v3.0.1
.. image:: https://gitlab.idiap.ch/bob/bob.io.video/badges/v3.0.1/coverage.svg
   :target: https://gitlab.idiap.ch/bob/bob.io.video/commits/v3.0.1
.. image:: https://img.shields.io/badge/gitlab-project-0000c0.svg
   :target: https://gitlab.idiap.ch/bob/bob.io.video
.. image:: http://img.shields.io/pypi/v/bob.io.video.svg
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

Complete Bob's `installation`_ instructions. Then, to install this package,
run::

  $ conda install bob.io.video


Contact
-------

For questions or reporting issues to this software package, contact our
development `mailing list`_.


.. Place your references here:
.. _bob: https://www.idiap.ch/software/bob
.. _installation: https://www.idiap.ch/software/bob/install
.. _mailing list: https://www.idiap.ch/software/bob/discuss
