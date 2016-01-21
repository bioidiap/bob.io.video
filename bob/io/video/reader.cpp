/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Wed  6 Nov 21:44:34 2013
 *
 * @brief Bindings to bob::io::video::Reader
 */

#include "bobskin.h"

#include <boost/make_shared.hpp>
#include <numpy/arrayobject.h>
#include <bob.blitz/capi.h>
#include <bob.blitz/cleanup.h>
#include <bob.io.base/api.h>
#include <stdexcept>

#include "cpp/reader.h"

#define VIDEOREADER_NAME "reader"
PyDoc_STRVAR(s_videoreader_str, BOB_EXT_MODULE_PREFIX "." VIDEOREADER_NAME);

PyDoc_STRVAR(s_videoreader_doc,
"reader(filename, [check=True]) -> new reader\n\
\n\
Use this object to read frames from video files.\n\
\n\
Constructor parameters:\n\
\n\
filename\n\
  [str] The file path to the file you want to read data from\n\
\n\
check\n\
  [bool] Format and codec will be extracted from the video metadata.\n\
  By default, if the format and/or the codec are not\n\
  supported by this version of Bob, an exception will be raised.\n\
  You can (at your own risk) set this flag to ``False`` to\n\
  avoid this check.\n\
\n\
Video reader objects can read data from video files. The current\n\
implementation uses `FFmpeg <http://ffmpeg.org>`_ (or\n\
`libav <http://libav.org>`_ if FFmpeg is not available) which is\n\
a stable freely available video encoding and decoding library,\n\
designed specifically for these tasks. You can read an entire\n\
video in memory by using the :py:meth:`bob.io.video.reader.load`\n\
method or use iterators to read it frame by frame and avoid\n\
overloading your machine\'s memory. The maximum precision data\n\
`FFmpeg`_ will yield is a 24-bit (8-bit per band) representation\n\
of each pixel (32-bit depths are also supported by `FFmpeg`_, but\n\
not by this extension presently). So, the output of data is done\n\
with ``uint8`` as data type. Output will be colored using the RGB\n\
standard, with each band varying between 0 and 255, with zero\n\
meaning pure black and 255, pure white (color).\n\
\n\
");

typedef struct {
  PyObject_HEAD
  boost::shared_ptr<bob::io::video::Reader> v;
} PyBobIoVideoReaderObject;

extern PyTypeObject PyBobIoVideoReader_Type;

/* How to create a new PyBobIoVideoReaderObject */
static PyObject* PyBobIoVideoReader_New(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobIoVideoReaderObject* self = (PyBobIoVideoReaderObject*)type->tp_alloc(type, 0);

  self->v.reset();

  return reinterpret_cast<PyObject*>(self);
}

static void PyBobIoVideoReader_Delete (PyBobIoVideoReaderObject* o) {

  o->v.reset();
  Py_TYPE(o)->tp_free((PyObject*)o);

}

/* The __init__(self) method */
static int PyBobIoVideoReader_Init(PyBobIoVideoReaderObject* self,
    PyObject *args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"filename", "check", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* filename = 0;

  PyObject* pycheck = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&|O", kwlist,
        &PyBobIo_FilenameConverter, &filename, &pycheck)) return -1;

  auto filename_ = make_safe(filename);

  bool check = false;
  if (pycheck && PyObject_IsTrue(pycheck)) check = true;

#if PY_VERSION_HEX >= 0x03000000
  const char* c_filename = PyBytes_AS_STRING(filename);
#else
  const char* c_filename = PyString_AS_STRING(filename);
#endif

  try {
    self->v.reset(new bob::io::video::Reader(c_filename, check));
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return -1;
  }
  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "cannot open video file `%s' for reading: unknown exception caught", c_filename);
    return -1;
  }

  return 0; ///< SUCCESS
}

PyObject* PyBobIoVideoReader_Filename(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->filename().c_str());
}

PyDoc_STRVAR(s_filename_str, "filename");
PyDoc_STRVAR(s_filename_doc,
"[str] The full path to the file that will be decoded by this object");

PyObject* PyBobIoVideoReader_Height(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("n", self->v->height());
}

PyDoc_STRVAR(s_height_str, "height");
PyDoc_STRVAR(s_height_doc,
"[int] The height of each frame in the video (a multiple of 2)");

PyObject* PyBobIoVideoReader_Width(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("n", self->v->width());
}

PyDoc_STRVAR(s_width_str, "width");
PyDoc_STRVAR(s_width_doc,
"[int] The width of each frame in the video (a multiple of 2)");

PyObject* PyBobIoVideoReader_NumberOfFrames(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("n", self->v->numberOfFrames());
}

PyDoc_STRVAR(s_number_of_frames_str, "number_of_frames");
PyDoc_STRVAR(s_number_of_frames_doc,
"[int] The number of frames in this video file");

PyObject* PyBobIoVideoReader_Duration(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("n", self->v->duration());
}

PyDoc_STRVAR(s_duration_str, "duration");
PyDoc_STRVAR(s_duration_doc,
"[int] Total duration of this video file in microseconds (long)");

PyObject* PyBobIoVideoReader_FormatName(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->formatName().c_str());
}

PyDoc_STRVAR(s_format_name_str, "format_name");
PyDoc_STRVAR(s_format_name_doc,
"[str] Short name of the format in which this video file was recorded in");

PyObject* PyBobIoVideoReader_FormatLongName(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->formatLongName().c_str());
}

PyDoc_STRVAR(s_format_long_name_str, "format_long_name");
PyDoc_STRVAR(s_format_long_name_doc,
"[str] Full name of the format in which this video file was recorded in");

PyObject* PyBobIoVideoReader_CodecName(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->codecName().c_str());
}

PyDoc_STRVAR(s_codec_name_str, "codec_name");
PyDoc_STRVAR(s_codec_name_doc,
"[str] Short name of the codec in which this video file was recorded in");

PyObject* PyBobIoVideoReader_CodecLongName(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->codecLongName().c_str());
}

PyDoc_STRVAR(s_codec_long_name_str, "codec_long_name");
PyDoc_STRVAR(s_codec_long_name_doc,
"[str] Full name of the codec in which this video file was recorded in");

PyObject* PyBobIoVideoReader_FrameRate(PyBobIoVideoReaderObject* self) {
  return PyFloat_FromDouble(self->v->frameRate());
}

PyDoc_STRVAR(s_frame_rate_str, "frame_rate");
PyDoc_STRVAR(s_frame_rate_doc,
"[float] Video's announced frame rate (note there are video formats with variable frame rates)");

PyObject* PyBobIoVideoReader_VideoType(PyBobIoVideoReaderObject* self) {
  return PyBobIo_TypeInfoAsTuple(self->v->video_type());
}

PyDoc_STRVAR(s_video_type_str, "video_type");
PyDoc_STRVAR(s_video_type_doc,
"[tuple] Typing information to load all of the file at once");

PyObject* PyBobIoVideoReader_FrameType(PyBobIoVideoReaderObject* self) {
  return PyBobIo_TypeInfoAsTuple(self->v->frame_type());
}

PyDoc_STRVAR(s_frame_type_str, "frame_type");
PyDoc_STRVAR(s_frame_type_doc,
"[tuple] Typing information to load each frame separatedly");

static PyObject* PyBobIoVideoReader_Print(PyBobIoVideoReaderObject* self) {
  return Py_BuildValue("s", self->v->info().c_str());
}

PyDoc_STRVAR(s_info_str, "info");
PyDoc_STRVAR(s_info_doc,
"[str] A string with lots of video information (same as ``str(x)``)");

static PyGetSetDef PyBobIoVideoReader_getseters[] = {
    {
      s_filename_str,
      (getter)PyBobIoVideoReader_Filename,
      0,
      s_filename_doc,
      0,
    },
    {
      s_height_str,
      (getter)PyBobIoVideoReader_Height,
      0,
      s_height_doc,
      0,
    },
    {
      s_width_str,
      (getter)PyBobIoVideoReader_Width,
      0,
      s_width_doc,
      0,
    },
    {
      s_number_of_frames_str,
      (getter)PyBobIoVideoReader_NumberOfFrames,
      0,
      s_number_of_frames_doc,
      0,
    },
    {
      s_duration_str,
      (getter)PyBobIoVideoReader_Duration,
      0,
      s_duration_doc,
      0,
    },
    {
      s_format_name_str,
      (getter)PyBobIoVideoReader_FormatName,
      0,
      s_format_name_doc,
      0,
    },
    {
      s_format_long_name_str,
      (getter)PyBobIoVideoReader_FormatLongName,
      0,
      s_format_long_name_doc,
      0,
    },
    {
      s_codec_name_str,
      (getter)PyBobIoVideoReader_CodecName,
      0,
      s_codec_name_doc,
      0,
    },
    {
      s_codec_long_name_str,
      (getter)PyBobIoVideoReader_CodecLongName,
      0,
      s_codec_long_name_doc,
      0,
    },
    {
      s_frame_rate_str,
      (getter)PyBobIoVideoReader_FrameRate,
      0,
      s_frame_rate_doc,
      0,
    },
    {
      s_video_type_str,
      (getter)PyBobIoVideoReader_VideoType,
      0,
      s_video_type_doc,
      0,
    },
    {
      s_frame_type_str,
      (getter)PyBobIoVideoReader_FrameType,
      0,
      s_frame_type_doc,
      0,
    },
    {
      s_info_str,
      (getter)PyBobIoVideoReader_Print,
      0,
      s_info_doc,
      0,
    },
    {0}  /* Sentinel */
};

static PyObject* PyBobIoVideoReader_Repr(PyBobIoVideoReaderObject* self) {
  return
# if PY_VERSION_HEX >= 0x03000000
  PyUnicode_FromFormat
# else
  PyString_FromFormat
# endif
  ("%s(filename='%s')", Py_TYPE(self)->tp_name, self->v->filename().c_str());
}

/**
 * If a keyboard interruption occurs, then it is translated into a C++
 * exception that makes the loop stops.
 */
static void Check_Interrupt() {
  if(PyErr_CheckSignals() == -1) {
    if (!PyErr_Occurred()) PyErr_SetInterrupt();
    throw std::runtime_error("error is already set");
  }
}

static PyObject* PyBobIoVideoReader_Load(PyBobIoVideoReaderObject* self, PyObject *args, PyObject* kwds) {

  /* Parses input arguments in a single shot */
  static const char* const_kwlist[] = {"raise_on_error", 0};
  static char** kwlist = const_cast<char**>(const_kwlist);

  PyObject* raise = 0;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &raise)) return 0;

  bool raise_on_error = false;
  if (raise && PyObject_IsTrue(raise)) raise_on_error = true;

  const bob::io::base::array::typeinfo& info = self->v->video_type();

  npy_intp shape[NPY_MAXDIMS];
  for (size_t k=0; k<info.nd; ++k) shape[k] = info.shape[k];

  int type_num = PyBobIo_AsTypenum(info.dtype);
  if (type_num == NPY_NOTYPE) return 0; ///< failure

  PyObject* retval = PyArray_SimpleNew(info.nd, shape, type_num);
  if (!retval) return 0;
  auto retval_ = make_safe(retval);

  Py_ssize_t frames_read = 0;

  try {
    bobskin skin((PyArrayObject*)retval, info.dtype);
    frames_read = self->v->load(skin, raise_on_error, &Check_Interrupt);
  }
  catch (std::exception& e) {
    if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    if (!PyErr_Occurred()) PyErr_Format(PyExc_RuntimeError, "caught unknown exception while reading video from file `%s'", self->v->filename().c_str());
    return 0;
  }

  if (frames_read != shape[0]) {
    //resize
    shape[0] = frames_read;
    PyArray_Dims newshape;
    newshape.ptr = shape;
    newshape.len = info.nd;
    PyArray_Resize((PyArrayObject*)retval, &newshape, 1, NPY_ANYORDER);
  }

  Py_INCREF(retval);
  return retval;

}

PyDoc_STRVAR(s_load_str, "load");
PyDoc_STRVAR(s_load_doc,
"x.load([raise_on_error=False]) -> numpy.ndarray\n\
\n\
Loads all of the video stream in a numpy ndarray organized\n\
in this way: (frames, color-bands, height, width). I'll dynamically\n\
allocate the output array and return it to you.\n\
\n\
The flag ``raise_on_error``, which is set to ``False`` by default\n\
influences the error reporting in case problems are found with the\n\
video file. If you set it to ``True``, we will report problems\n\
raising exceptions. If you either don't set it or set it to\n\
``False``, we will truncate the file at the frame with problems\n\
and will not report anything. It is your task to verify if the\n\
number of frames returned matches the expected number of frames as\n\
reported by the property ``number_of_frames`` (or ``len``) of this\n\
object.\n\
");

static PyMethodDef PyBobIoVideoReader_Methods[] = {
    {
      s_load_str,
      (PyCFunction)PyBobIoVideoReader_Load,
      METH_VARARGS|METH_KEYWORDS,
      s_load_doc,
    },
    {0}  /* Sentinel */
};

static PyObject* PyBobIoVideoReader_GetIndex (PyBobIoVideoReaderObject* self, Py_ssize_t i) {

  if (i < 0) i += self->v->numberOfFrames(); ///< adjust for negative indexing

  if (i < 0 || (size_t)i >= self->v->numberOfFrames()) {
    PyErr_Format(PyExc_IndexError, "video frame index out of range - `%s' only contains %" PY_FORMAT_SIZE_T "d frame(s)", self->v->filename().c_str(), self->v->numberOfFrames());
    return 0;
  }

  const bob::io::base::array::typeinfo& info = self->v->frame_type();

  npy_intp shape[NPY_MAXDIMS];
  for (size_t k=0; k<info.nd; ++k) shape[k] = info.shape[k];

  int type_num = PyBobIo_AsTypenum(info.dtype);
  if (type_num == NPY_NOTYPE) return 0; ///< failure

  PyObject* retval = PyArray_SimpleNew(info.nd, shape, type_num);
  if (!retval) return 0;
  auto retval_ = make_safe(retval);

  try {
    auto it = self->v->begin();
    it += i;
    bobskin skin((PyArrayObject*)retval, info.dtype);
    it.read(skin);
  }
  catch (std::exception& e) {
    if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    if (!PyErr_Occurred()) PyErr_Format(PyExc_RuntimeError, "caught unknown exception while reading frame #%" PY_FORMAT_SIZE_T "d from file `%s'", i, self->v->filename().c_str());
    return 0;
  }

  Py_INCREF(retval);
  return retval;

}

static PyObject* PyBobIoVideoReader_GetSlice (PyBobIoVideoReaderObject* self, PySliceObject* slice) {

  Py_ssize_t start, stop, step, slicelength;
#if PY_VERSION_HEX < 0x03000000
  if (PySlice_GetIndicesEx(slice,
#else
  if (PySlice_GetIndicesEx(reinterpret_cast<PyObject*>(slice),
#endif
        self->v->numberOfFrames(), &start, &stop, &step, &slicelength) < 0) return 0;

  //creates the return array
  const bob::io::base::array::typeinfo& info = self->v->frame_type();

  int type_num = PyBobIo_AsTypenum(info.dtype);
  if (type_num == NPY_NOTYPE) return 0; ///< failure

  if (slicelength <= 0) return PyArray_SimpleNew(0, 0, type_num);

  npy_intp shape[NPY_MAXDIMS];
  shape[0] = slicelength;
  for (size_t k=0; k<info.nd; ++k) shape[k+1] = info.shape[k];

  PyObject* retval = PyArray_SimpleNew(info.nd+1, shape, type_num);
  if (!retval) return 0;
  auto retval_ = make_safe(retval);

  Py_ssize_t counter;
  Py_ssize_t lo, hi, st;
  auto it = self->v->begin();
  if (start <= stop) {
    lo = start, hi = stop, st = step;
    it += lo, counter = 0;
  }
  else {
    lo = stop, hi = start, st = -step;
    it += lo + (hi-lo)%st, counter = slicelength - 1;
  }

  for (auto i=lo; i<hi; i+=st) {

    //get slice to fill
    PyObject* islice = Py_BuildValue("n", counter);
    counter = (st == -step)? counter-1 : counter+1;
    if (!islice) return 0;
    auto islice_ = make_safe(islice);

    PyObject* item = PyObject_GetItem(retval, islice);
    if (!item) return 0;
    auto item_ = make_safe(item);

    try {
      bobskin skin((PyArrayObject*)item, info.dtype);
      it.read(skin);
      it += (st-1);
    }
    catch (std::exception& e) {
      if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, e.what());
      return 0;
    }
    catch (...) {
      if (!PyErr_Occurred()) PyErr_Format(PyExc_RuntimeError, "caught unknown exception while reading frame #%" PY_FORMAT_SIZE_T "d from file `%s'", i, self->v->filename().c_str());
      return 0;
    }

  }

  Py_INCREF(retval);
  return retval;

}

static PyObject* PyBobIoVideoReader_GetItem (PyBobIoVideoReaderObject* self, PyObject* item) {
   if (PyIndex_Check(item)) {
     Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
     if (i == -1 && PyErr_Occurred()) return 0;
     return PyBobIoVideoReader_GetIndex(self, i);
   }
   if (PySlice_Check(item)) {
     return PyBobIoVideoReader_GetSlice(self, (PySliceObject*)item);
   }
   else {
     PyErr_Format(PyExc_TypeError, "VideoReader indices must be integers, not `%s'",
         Py_TYPE(item)->tp_name);
     return 0;
   }
}

Py_ssize_t PyBobIoVideoReader_Len(PyBobIoVideoReaderObject* self) {
  return self->v->numberOfFrames();
}

static PyMappingMethods PyBobIoVideoReader_Mapping = {
    (lenfunc)PyBobIoVideoReader_Len, //mp_lenght
    (binaryfunc)PyBobIoVideoReader_GetItem, //mp_subscript
    0 /* (objobjargproc)PyBobIoVideoReader_SetItem //mp_ass_subscript */
};

/*****************************************
 * Definition of Iterator to VideoReader *
 *****************************************/

#define VIDEOITERTYPE_NAME "reader.iter"
PyDoc_STRVAR(s_videoreaderiterator_str, BOB_EXT_MODULE_PREFIX "." VIDEOITERTYPE_NAME);

typedef struct {
  PyObject_HEAD
  PyBobIoVideoReaderObject* pyreader;
  boost::shared_ptr<bob::io::video::Reader::const_iterator> iter;
} PyBobIoVideoReaderIteratorObject;

extern PyTypeObject PyBobIoVideoReaderIterator_Type;

static PyObject* PyBobIoVideoReaderIterator_New(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBobIoVideoReaderIteratorObject* self = (PyBobIoVideoReaderIteratorObject*)type->tp_alloc(type, 0);

  self->iter.reset();

  return reinterpret_cast<PyObject*>(self);
}

static PyObject* PyBobIoVideoReaderIterator_Iter (PyBobIoVideoReaderIteratorObject* self) {
  return Py_BuildValue("O", self);
}

static PyObject* PyBobIoVideoReaderIterator_Next (PyBobIoVideoReaderIteratorObject* self) {

  if ((*self->iter == self->pyreader->v->end()) ||
      (self->iter->cur() == self->pyreader->v->numberOfFrames())) {
    self->iter->reset();
    self->iter.reset();
    Py_XDECREF((PyObject*)self->pyreader);
    return 0;
  }

  const bob::io::base::array::typeinfo& info = self->pyreader->v->frame_type();

  npy_intp shape[NPY_MAXDIMS];
  for (size_t k=0; k<info.nd; ++k) shape[k] = info.shape[k];

  int type_num = PyBobIo_AsTypenum(info.dtype);
  if (type_num == NPY_NOTYPE) return 0; ///< failure

  PyObject* retval = PyArray_SimpleNew(info.nd, shape, type_num);
  if (!retval) return 0;
  auto retval_ = make_safe(retval);

  try {
    bobskin skin((PyArrayObject*)retval, info.dtype);
    self->iter->read(skin);
  }
  catch (std::exception& e) {
    if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
  catch (...) {
    if (!PyErr_Occurred()) PyErr_Format(PyExc_RuntimeError, "caught unknown exception while reading frame #%" PY_FORMAT_SIZE_T "d from file `%s'", self->iter->cur(), self->pyreader->v->filename().c_str());
    return 0;
  }

  Py_INCREF(retval);
  return retval;

}

#if PY_VERSION_HEX >= 0x03000000
#  define Py_TPFLAGS_HAVE_ITER 0
#endif

PyTypeObject PyBobIoVideoReaderIterator_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_videoreaderiterator_str,                  /* tp_name */
    sizeof(PyBobIoVideoReaderIteratorObject),   /* tp_basicsize */
    0,                                          /* tp_itemsize */
    0,                                          /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_compare */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,  /* tp_flags */
    0,                                          /* tp_doc */
    0,		                                      /* tp_traverse */
    0,		                                      /* tp_clear */
    0,                                          /* tp_richcompare */
    0,		                                      /* tp_weaklistoffset */
    (getiterfunc)PyBobIoVideoReaderIterator_Iter,      /* tp_iter */
    (iternextfunc)PyBobIoVideoReaderIterator_Next      /* tp_iternext */
};

static PyObject* PyBobIoVideoReader_Iter (PyBobIoVideoReaderObject* self) {

  /* Allocates the python object itself */
  PyBobIoVideoReaderIteratorObject* retval = (PyBobIoVideoReaderIteratorObject*)PyBobIoVideoReaderIterator_New(&PyBobIoVideoReaderIterator_Type, 0, 0);
  if (!retval) return 0;

  Py_INCREF(self);
  retval->pyreader = self;
  retval->iter.reset(new bob::io::video::Reader::const_iterator(self->v->begin()));
  return Py_BuildValue("N", retval);
}

PyTypeObject PyBobIoVideoReader_Type = {
    PyVarObject_HEAD_INIT(0, 0)
    s_videoreader_str,                          /*tp_name*/
    sizeof(PyBobIoVideoReaderObject),           /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)PyBobIoVideoReader_Delete,      /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    (reprfunc)PyBobIoVideoReader_Repr,          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    &PyBobIoVideoReader_Mapping,                /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    (reprfunc)PyBobIoVideoReader_Print,         /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    s_videoreader_doc,                          /* tp_doc */
    0,		                                      /* tp_traverse */
    0,		                                      /* tp_clear */
    0,                                          /* tp_richcompare */
    0,		                                      /* tp_weaklistoffset */
    (getiterfunc)PyBobIoVideoReader_Iter,       /* tp_iter */
    0,		                                      /* tp_iternext */
    PyBobIoVideoReader_Methods,                 /* tp_methods */
    0,                                          /* tp_members */
    PyBobIoVideoReader_getseters,               /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    (initproc)PyBobIoVideoReader_Init,          /* tp_init */
    0,                                          /* tp_alloc */
    PyBobIoVideoReader_New,                     /* tp_new */
};
