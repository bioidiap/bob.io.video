.. vim: set fileencoding=utf-8 :
.. Andre Anjos <andre.dos.anjos@gmail.com>
.. Tue 15 Oct 17:41:52 2013

.. testsetup:: *

   import numpy
   import xbob.io.base
   import xbob.io.video
   import tempfile
   import os

   current_directory = os.path.realpath(os.curdir)
   temp_dir = tempfile.mkdtemp(prefix='bob_doctest_')
   os.chdir(temp_dir)

============
 User Guide
============

This package provides support for dealing with videos in an equivalent way to
dealing with other data files in |project|:

.. doctest::

  >>> my_video = numpy.random.random_integers(0,255,(30,3,256,256))
  >>> xbob.io.base.save(my_video.astype('uint8'), 'testvideo.avi') # saving the video avi format with a default codec
  >>> my_video_copy = xbob.io.base.load('testvideo.avi')

Video reading and writing is performed using an `FFmpeg`_ (or `libav`_ if
`FFmpeg`_ is not available) bridge. |project|'s :py:meth:`xbob.io.base.save`
method will allow you to choose the output format with the same extension
mechanism as mentioned earlier. `FFmpeg`_ will then choose a default codec for
the format and perform encoding. The output file can be as easily loaded using
:py:meth:`xbob.io.base.load`.

For finer control over the loading, saving, format and codecs used for a
specific encoding or decoding operation, you must directly use either
:py:class:`xbob.io.video.reader` or :py:class:`xbob.io.video.writer`
classes. For example, it is possible to use
:py:class:`xbob.io.video.reader` to read videos frame by frame and avoid
overloading your machine's memory. In the following example you can see how to
create a video, save it using the class :py:class:`xbob.io.video.writer`
and load it again using the class :py:class:`xbob.io.video.reader`. The
created video will have 30 frames generated randomly.

.. note::

  Due to `FFmpeg`_ constrains, the width and height of the video need to be
  multiples of two.

.. doctest::

  >>> width = 50; height = 50;
  >>> framerate = 24
  >>> outv = xbob.io.video.writer('testvideo.avi', height, width, framerate, codec='mpeg1video') # output video
  >>> for i in range(0, 30):
  ...   newframe = (numpy.random.random_integers(0,255,(3,height,width)))
  ...   outv.append(newframe.astype('uint8'))
  >>> outv.close()
  >>> input = xbob.io.video.reader('testvideo.avi')
  >>> input.number_of_frames
  30
  >>> inv = input.load()
  >>> inv.shape
  (30, 3, 50, 50)
  >>> type(inv)
  <... 'numpy.ndarray'>

Videos in |project| are represented as sequences of colored images, i.e. 4D
arrays of type ``uint8``. All the extensions and formats for videos supported
in version of |project| installed on your machine can be listed using the
|project|'s utility ``bob_config.py``.

.. testcleanup:: *

  import shutil
  os.chdir(current_directory)
  shutil.rmtree(temp_dir)

.. warning::

  Please read :doc:`video` for details on choosing codecs and formats that are
  adequate to your application, as well as drawbacks and pitfalls with video
  encoding and decoding.

.. Place here your external references
.. include:: links.rst
