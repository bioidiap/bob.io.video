.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.anjos@idiap.ch>
.. Wed 14 May 15:22:33 2014 CEST
..
.. Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland

.. _bob.io.video:

==========================
 Bob's Video I/O Routines
==========================

.. todolist::

This module provides two Video I/O components:

1. A plugin for :py:mod:`bob.io.base` that allows |project| to read and write
   videos using its native API (:py:func:`bob.io.base.load` and
   :py:func:`bob.io.base.save`).

2. A special interface for reading and writing video files with an enhanced set
   of options. This includes the possibility to define the recording rate,
   codec and/or format to be used for example.

Documentation
-------------

.. toctree::
   :maxdepth: 2

   guide
   video
   py_api

Indices and tables
------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

.. include:: links.rst
