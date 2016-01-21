/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Wed  6 Nov 21:44:34 2013
 *
 * @brief Bindings to bob::io::video::Writer
 */

#include "bobskin.h"

#include <boost/make_shared.hpp>
#include <numpy/arrayobject.h>
#include <bob.blitz/cppapi.h>
#include <bob.blitz/cleanup.h>
#include <bob.io.base/api.h>
#include <stdexcept>

#include "cpp/writer.h"

#define VIDEOWRITER_NAME "writer"
PyDoc_STRVAR(s_videowriter_str, BOB_EXT_MODULE_PREFIX "." VIDEOWRITER_NAME);

PyDoc_STRVAR(s_videowriter_doc,
"writer(filename, height, width, [framerate=25., [bitrate=1500000., [gop=12, [codec='', [format='', [check=True]]]]]]) -> new writer\n\
\n\
Use this object to write frames to video files.\n\
\n\
Constructor parameters:\n\
\n\
filename\n\
  [str] The file path to the file you want to read data from\n\
\n\
height\n\
  [int] The height of the video (must be a multiple of 2)\n\
\n\
width\n\
  [int] The width of the video (must be a multiple of 2)\n\
\n\
framerate\n\
  [float, optional] The number of frames per second\n\
\n\
bitrate\n\
  [float, optional] The estimated bitrate of the output video\n\
\n\
gop\n\
  [int, optional] Group-of-Pictures (emit one intra frame\n\
  every ``gop`` frames at most).\n\
\n\
codec\n\
  [str, optional] If you must, specify a valid FFmpeg codec\n\
  name here and that will be used to encode the video stream\n\
  on the output file.\n\
\n\
format\n\
  [str, optional] If you must, specify a valid FFmpeg output\n\
  format name and that will be used to encode the video on the\n\
  output file. Leave it empty to guess from the filename extension.\n\
\n\
check\n\
  [bool, optional] The video will be created if the combination\n\
  of format and codec are known to work and have been tested,\n\
  otherwise an exception is raised. If you set this parameter to\n\
  ``False``, though, we will ignore this check.\n\
\n\
Video writer objects can write data to video files. The current\n\
implementation uses `FFmpeg <http://ffmpeg.org>`_ (or\n\
`libav <http://libav.org>`_ if FFmpeg is not available) which is\n\
a stable freely available video encoding and decoding library,\n\
designed specifically for these tasks. Videos are objects composed\n\
of RGB colored frames. Each frame inserted should be a 3D\n\
:py:class:`numpy.ndarray` composed of unsigned integers of 8 bits.\n\
Each frame should have a shape equivalent to\n\
``(plane, height, width)``.\n\
");

typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::io::video::Writer> v;
} PyBobIoVideoWriterObject;

extern PyTypeObject PyBobIoVideoWriter_Type;

/* How to create a new PyBobIoVideoWriterObject */
static PyObject* PyBobIoVideoWriter_New(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobIoVideoWriterObject* self = (PyBobIoVideoWriterObject*)type->tp_alloc(type, 0);

  self->v.reset();

  return reinterpret_cast<PyObject*>(self);
}

static void PyBobIoVideoWriter_Delete (PyBobIoVideoWriterObject* o) {

  o->v.reset();
  Py_TYPE(o)->tp_free((PyObject*)o);

}

/* The __init__(self) method */
static int PyBobIoVideoWriter_Init(PyBobIoVideoWriterObject* self,
    PyObject *args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {
    "filename", "height", "width", //mandatory
    "framerate", "bitrate", "gop", "codec", "format", "check",  //optional
    0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* filename = 0;

  Py_ssize_t height = 0;
  Py_ssize_t width = 0;

  double framerate = 25.;
  double bitrate = 1500000.;
  Py_ssize_t gop = 12;
  char* codec = 0;
  char* format = 0;
  PyObject* pycheck = Py_True;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&nn|ddnssO", kwlist,
        &PyBobIo_FilenameConverter, &filename,
        &height, &width, &framerate, &bitrate, &gop, &codec,
        &format, &pycheck)) return -1;

  auto filename_ = make_safe(filename);

  std::string codec_str = codec?codec:"";
  std::string format_str = format?format:"";
  bool check = PyObject_IsTrue(pycheck);

#if PY_VERSION_HEX >= 0x03000000
  const char* c_filename = PyBytes_AS_STRING(filename);
#else
  const char* c_filename = PyString_AS_STRING(filename);
#endif

  try {
    self->v = boost::make_shared<bob::io::video::Writer>(c_filename,
        height, width, framerate, bitrate, gop, codec_str, format_str, check);
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot open video file `%s' for writing: unknown exception caught", c_filename);
    return -1;
  }

  return 0; ///< SUCCESS
}

PyObject* PyBobIoVideoWriter_Filename(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("s", self->v->filename().c_str());
}

PyDoc_STRVAR(s_filename_str, "filename");
PyDoc_STRVAR(s_filename_doc,
"[str] The full path to the file that will be decoded by this object");

PyObject* PyBobIoVideoWriter_Height(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("n", self->v->height());
}

PyDoc_STRVAR(s_height_str, "height");
PyDoc_STRVAR(s_height_doc,
"[int] The height of each frame in the video (a multiple of 2)");

PyObject* PyBobIoVideoWriter_Width(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("n", self->v->width());
}

PyDoc_STRVAR(s_width_str, "width");
PyDoc_STRVAR(s_width_doc,
"[int] The width of each frame in the video (a multiple of 2)");

PyObject* PyBobIoVideoWriter_NumberOfFrames(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("n", self->v->numberOfFrames());
}

PyDoc_STRVAR(s_number_of_frames_str, "number_of_frames");
PyDoc_STRVAR(s_number_of_frames_doc,
"[int] The number of frames in this video file");

PyObject* PyBobIoVideoWriter_Duration(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("n", self->v->duration());
}

PyDoc_STRVAR(s_duration_str, "duration");
PyDoc_STRVAR(s_duration_doc,
"[int] Total duration of this video file in microseconds (long)");

PyObject* PyBobIoVideoWriter_FormatName(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }
  return Py_BuildValue("s", self->v->formatName().c_str());
}

PyDoc_STRVAR(s_format_name_str, "format_name");
PyDoc_STRVAR(s_format_name_doc,
"[str] Short name of the format in which this video file was recorded in");

PyObject* PyBobIoVideoWriter_FormatLongName(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }
  return Py_BuildValue("s", self->v->formatLongName().c_str());
}

PyDoc_STRVAR(s_format_long_name_str, "format_long_name");
PyDoc_STRVAR(s_format_long_name_doc,
"[str] Full name of the format in which this video file was recorded in");

PyObject* PyBobIoVideoWriter_CodecName(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }
  return Py_BuildValue("s", self->v->codecName().c_str());
}

PyDoc_STRVAR(s_codec_name_str, "codec_name");
PyDoc_STRVAR(s_codec_name_doc,
"[str] Short name of the codec in which this video file was recorded in");

PyObject* PyBobIoVideoWriter_CodecLongName(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }
  return Py_BuildValue("s", self->v->codecLongName().c_str());
}

PyDoc_STRVAR(s_codec_long_name_str, "codec_long_name");
PyDoc_STRVAR(s_codec_long_name_doc,
"[str] Full name of the codec in which this video file was recorded in");

PyObject* PyBobIoVideoWriter_FrameRate(PyBobIoVideoWriterObject* self) {
  return PyFloat_FromDouble(self->v->frameRate());
}

PyDoc_STRVAR(s_frame_rate_str, "frame_rate");
PyDoc_STRVAR(s_frame_rate_doc,
"[float] Video's announced frame rate (note there are video formats\n\
with variable frame rates)");

PyObject* PyBobIoVideoWriter_BitRate(PyBobIoVideoWriterObject* self) {
  return PyFloat_FromDouble(self->v->bitRate());
}

PyDoc_STRVAR(s_bit_rate_str, "bit_rate");
PyDoc_STRVAR(s_bit_rate_doc,
"[float] The indicative bit rate for this video file, given as a\n\
hint to `FFmpeg` (compression levels are subject to the picture\n\
textures)");

PyObject* PyBobIoVideoWriter_GOP(PyBobIoVideoWriterObject* self) {
  return Py_BuildValue("n", self->v->gop());
}

PyDoc_STRVAR(s_gop_str, "gop");
PyDoc_STRVAR(s_gop_doc,
"[int] Group of pictures setting (see the `Wikipedia entry\n\
<http://en.wikipedia.org/wiki/Group_of_pictures>`_ for details\n\
on this setting)");

PyObject* PyBobIoVideoWriter_VideoType(PyBobIoVideoWriterObject* self) {
  return PyBobIo_TypeInfoAsTuple(self->v->video_type());
}

PyDoc_STRVAR(s_video_type_str, "video_type");
PyDoc_STRVAR(s_video_type_doc,
"[tuple] Typing information to load all of the file at once");

PyObject* PyBobIoVideoWriter_FrameType(PyBobIoVideoWriterObject* self) {
  return PyBobIo_TypeInfoAsTuple(self->v->frame_type());
}

PyDoc_STRVAR(s_frame_type_str, "frame_type");
PyDoc_STRVAR(s_frame_type_doc,
"[tuple] Typing information to load each frame separatedly");

static PyObject* PyBobIoVideoWriter_Print(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }

  return Py_BuildValue("s", self->v->info().c_str());
}

PyDoc_STRVAR(s_info_str, "info");
PyDoc_STRVAR(s_info_doc,
"[str] A string with lots of video information (same as ``str(x)``)");

static PyObject* PyBobIoVideoWriter_IsOpened(PyBobIoVideoWriterObject* self) {
  if (self->v->is_opened()) Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

PyDoc_STRVAR(s_is_opened_str, "is_opened");
PyDoc_STRVAR(s_is_opened_doc,
"[bool] A flag, indicating if the video is still opened for writing\n\
(or has already been closed by the user using ``close()``)");

static PyGetSetDef PyBobIoVideoWriter_getseters[] = {
    {
      s_filename_str,
      (getter)PyBobIoVideoWriter_Filename,
      0,
      s_filename_doc,
      0,
    },
    {
      s_height_str,
      (getter)PyBobIoVideoWriter_Height,
      0,
      s_height_doc,
      0,
    },
    {
      s_width_str,
      (getter)PyBobIoVideoWriter_Width,
      0,
      s_width_doc,
      0,
    },
    {
      s_number_of_frames_str,
      (getter)PyBobIoVideoWriter_NumberOfFrames,
      0,
      s_number_of_frames_doc,
      0,
    },
    {
      s_duration_str,
      (getter)PyBobIoVideoWriter_Duration,
      0,
      s_duration_doc,
      0,
    },
    {
      s_format_name_str,
      (getter)PyBobIoVideoWriter_FormatName,
      0,
      s_format_name_doc,
      0,
    },
    {
      s_format_long_name_str,
      (getter)PyBobIoVideoWriter_FormatLongName,
      0,
      s_format_long_name_doc,
      0,
    },
    {
      s_codec_name_str,
      (getter)PyBobIoVideoWriter_CodecName,
      0,
      s_codec_name_doc,
      0,
    },
    {
      s_codec_long_name_str,
      (getter)PyBobIoVideoWriter_CodecLongName,
      0,
      s_codec_long_name_doc,
      0,
    },
    {
      s_frame_rate_str,
      (getter)PyBobIoVideoWriter_FrameRate,
      0,
      s_frame_rate_doc,
      0,
    },
    {
      s_bit_rate_str,
      (getter)PyBobIoVideoWriter_BitRate,
      0,
      s_bit_rate_doc,
      0,
    },
    {
      s_gop_str,
      (getter)PyBobIoVideoWriter_GOP,
      0,
      s_gop_doc,
      0,
    },
    {
      s_video_type_str,
      (getter)PyBobIoVideoWriter_VideoType,
      0,
      s_video_type_doc,
      0,
    },
    {
      s_frame_type_str,
      (getter)PyBobIoVideoWriter_FrameType,
      0,
      s_frame_type_doc,
      0,
    },
    {
      s_info_str,
      (getter)PyBobIoVideoWriter_Print,
      0,
      s_info_doc,
      0,
    },
    {
      s_is_opened_str,
      (getter)PyBobIoVideoWriter_IsOpened,
      0,
      s_is_opened_doc,
      0,
    },
    {0}  /* Sentinel */
};

static PyObject* PyBobIoVideoWriter_Repr(PyBobIoVideoWriterObject* self) {
  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }

  return
# if PY_VERSION_HEX >= 0x03000000
  PyUnicode_FromFormat
# else
  PyString_FromFormat
# endif
  ("%s(filename='%s', height=%" PY_FORMAT_SIZE_T "d, width=%" PY_FORMAT_SIZE_T "d, framerate=%g, bitrate=%g, gop=%" PY_FORMAT_SIZE_T "d, codec='%s', format='%s')", Py_TYPE(self)->tp_name, self->v->filename().c_str(), self->v->height(), self->v->width(), self->v->frameRate(), self->v->bitRate(), self->v->gop(), self->v->codecName().c_str(), self->v->formatName().c_str());
}

static PyObject* PyBobIoVideoWriter_Append(PyBobIoVideoWriterObject* self, PyObject *args, PyObject* kwds) {

  if (!self->v->is_opened()) {
    PyErr_Format(PyExc_RuntimeError, "`%s' for `%s' is closed",
        Py_TYPE(self)->tp_name, self->v->filename().c_str());
    return 0;
  }

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"frame", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyBlitzArrayObject* frame = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&", kwlist, &PyBlitzArray_BehavedConverter, &frame)) return 0;
  auto frame_ = make_safe(frame);

  if (frame->ndim != 3 && frame->ndim != 4) {
    PyErr_Format(PyExc_ValueError, "input array should have 3 or 4 dimensions, but you passed an array with %" PY_FORMAT_SIZE_T "d dimensions", frame->ndim);
    return 0;
  }

  if (frame->type_num != NPY_UINT8) {
    PyErr_Format(PyExc_TypeError, "input array should have dtype `uint8', but you passed an array with dtype == `%s'", PyBlitzArray_TypenumAsString(frame->type_num));
    return 0;
  }

  try {
    if (frame->ndim == 3) {
      self->v->append(*PyBlitzArrayCxx_AsBlitz<uint8_t,3>(frame));
    }
    else {
      self->v->append(*PyBlitzArrayCxx_AsBlitz<uint8_t,4>(frame));
    }
  }
  catch (std::exception& e) {
    if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    if (!PyErr_Occurred()) PyErr_Format(PyExc_RuntimeError, "caught unknown exception while writing frame #%" PY_FORMAT_SIZE_T "d to file `%s'", self->v->numberOfFrames(), self->v->filename().c_str());
    return 0;
  }

  Py_RETURN_NONE;

}

PyDoc_STRVAR(s_append_str, "append");
PyDoc_STRVAR(s_append_doc,
"x.append(frame) -> None\n\
\n\
Writes a new frame or set of frames to the file.\n\
\n\
The frame should be setup as a array with 3 dimensions organized\n\
in this way (RGB color-bands, height, width). Sets of frames should\n\
be setup as a 4D array in this way: (frame-number, RGB color-bands,\n\
height, width). Arrays should contain only unsigned integers of 8\n\
bits.\n\
\n\
.. note::\n\
  At present time we only support arrays that have C-style storages\n\
  (if you pass reversed arrays or arrays with Fortran-style storage,\n\
  the result is undefined).\n\
\n\
");

static PyObject* PyBobIoVideoWriter_Close(PyBobIoVideoWriterObject* self) {
  self->v->close();
  Py_RETURN_NONE;
}

PyDoc_STRVAR(s_close_str, "close");
PyDoc_STRVAR(s_close_doc,
"x.close() -> None\n\
\n\
Closes the current video stream and forces writing the trailer.\n\
After this point the video is finalized and cannot be written to\n\
anymore.\n\
");

static PyMethodDef PyBobIoVideoWriter_Methods[] = {
    {
      s_append_str,
      (PyCFunction)PyBobIoVideoWriter_Append,
      METH_VARARGS|METH_KEYWORDS,
      s_append_doc,
    },
    {
      s_close_str,
      (PyCFunction)PyBobIoVideoWriter_Close,
      METH_NOARGS,
      s_close_doc,
    },
    {0}  /* Sentinel */
};

Py_ssize_t PyBobIoVideoWriter_Len(PyBobIoVideoWriterObject* self) {
  return self->v->numberOfFrames();
}

static PyMappingMethods PyBobIoVideoWriter_Mapping = {
    (lenfunc)PyBobIoVideoWriter_Len, //mp_lenght
    0, /* (binaryfunc)PyBobIoVideoWriter_GetItem, //mp_subscript */
    0  /* (objobjargproc)PyBobIoVideoWriter_SetItem //mp_ass_subscript */
};

PyTypeObject PyBobIoVideoWriter_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_videowriter_str,                          /*tp_name*/
    sizeof(PyBobIoVideoWriterObject),           /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)PyBobIoVideoWriter_Delete,      /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    (reprfunc)PyBobIoVideoWriter_Repr,          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    &PyBobIoVideoWriter_Mapping,                /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    (reprfunc)PyBobIoVideoWriter_Print,         /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    s_videowriter_doc,                          /* tp_doc */
    0,		                                      /* tp_traverse */
    0,		                                      /* tp_clear */
    0,                                          /* tp_richcompare */
    0,		                                      /* tp_weaklistoffset */
    0,                                          /* tp_iter */
    0,		                                      /* tp_iternext */
    PyBobIoVideoWriter_Methods,                 /* tp_methods */
    0,                                          /* tp_members */
    PyBobIoVideoWriter_getseters,               /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    (initproc)PyBobIoVideoWriter_Init,          /* tp_init */
    0,                                          /* tp_alloc */
    PyBobIoVideoWriter_New,                     /* tp_new */
};
