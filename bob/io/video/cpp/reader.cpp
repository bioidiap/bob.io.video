/**
 * @file io/cxx/video::Reader.cc
 * @date Wed Jun 22 17:50:08 2011 +0200
 * @author Andre Anjos <andre.anjos@idiap.ch>
 *
 * @brief A class to help you read videos. This code originates from
 * http://ffmpeg.org/doxygen/1.0/, "decoding & encoding example".
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#include "reader.h"

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/preprocessor.hpp>
#include <limits>

#include <bob.io.base/blitz_array.h>

#if LIBAVUTIL_VERSION_INT < 0x371167 //55.17.103 @ ffmpeg-3.0
#ifndef AV_PIX_FMT_RGB24
#define AV_PIX_FMT_RGB24 PIX_FMT_RGB24
#endif
#endif

namespace bob { namespace io { namespace video {

  Reader::Reader(const std::string& filename, bool check) {
    open(filename, check);
  }

  Reader::Reader(const Reader& other) {
    *this = other;
  }

  Reader& Reader::operator= (const Reader& other) {
    open(other.filename(), other.m_check);
    return *this;
  }

  void Reader::open(const std::string& filename, bool check) {
    m_filepath = filename;

    boost::shared_ptr<AVFormatContext> format_ctxt =
      make_input_format_context(m_filepath);

    m_formatname = format_ctxt->iformat->name;
    m_formatname_long = format_ctxt->iformat->long_name;

    int stream_index = find_video_stream(m_filepath, format_ctxt);

    AVCodec* codec = find_decoder(m_filepath, format_ctxt, stream_index);

    m_codecname = codec->name;
    m_codecname_long = codec->long_name;

    /**
     * Runs a format/codec check on user request
     */
    if (check) {
      if (!iformat_is_supported(m_formatname)) {
        boost::format s("The detected format (`%s' = `%s') of the input video file `%s' is not currently supported by this version of Bob. Convert the video file to a supported wrapping format or disable the `check' flag on the video::Reader object (if you are sure of what you are doing).");
        s % m_formatname % m_formatname_long % m_filepath;
        throw std::runtime_error(s.str());
      }
      if (!codec_is_supported(m_codecname)) {
        boost::format s("The detected decoder (`%s' = `%s') for the video stream on the input video file `%s' is not currently supported by this version of Bob. Convert the video file to a supported codec or disable the `check' flag on the video::Reader object (if you are sure of what you are doing).");
        s % m_codecname % m_codecname_long % m_filepath;
        throw std::runtime_error(s.str());
      }
    }

    boost::shared_ptr<AVCodecContext> codec_ctxt =
      make_codec_context(m_filepath,
          format_ctxt->streams[stream_index], codec);

    /**
     * Copies some information from the context just opened
     */
    m_width = codec_ctxt->width;
    m_height = codec_ctxt->height;
    m_duration = format_ctxt->duration;
    m_nframes = format_ctxt->streams[stream_index]->nb_frames;
    if (m_nframes > 0) {
      //number of frames is known
      m_framerate = (double(m_nframes) * AV_TIME_BASE) / double(m_duration);
    }
    else {
      //number of frames is not known
      m_framerate = av_q2d(format_ctxt->streams[stream_index]->avg_frame_rate);
      // removed the use of r_frame_rate as it is depricated now and libAV advices to use avg_frame_rate instead
      //m_framerate = av_q2d(format_ctxt->streams[stream_index]->r_frame_rate);
      m_nframes = (int)(m_framerate * m_duration / AV_TIME_BASE);
    }

    /**
     * This will create a local description of the contents of the stream, in
     * printable format.
     */
    boost::format fmt("Video file: %s; FFmpeg: avformat-%s; avcodec-%s; avutil-%s; swscale-%d; Format: %s (%s); Codec: %s (%s); Time: %.2f s (%d @ %2.fHz); Size (w x h): %d x %d pixels");
    fmt % m_filepath;
    fmt % BOOST_PP_STRINGIZE(LIBAVFORMAT_VERSION);
    fmt % BOOST_PP_STRINGIZE(LIBAVCODEC_VERSION);
    fmt % BOOST_PP_STRINGIZE(LIBAVUTIL_VERSION);
    fmt % BOOST_PP_STRINGIZE(LIBSWSCALE_VERSION);
    fmt % m_formatname_long;
    fmt % m_formatname;
    fmt % m_codecname_long;
    fmt % m_codecname;
    fmt % (m_duration / 1e6);
    fmt % m_nframes;
    fmt % m_framerate;
    fmt % m_width;
    fmt % m_height;
    m_formatted_info = fmt.str();

    /**
     * This will make sure we can interface with the io subsystem
     */
    m_typeinfo_video.dtype = m_typeinfo_frame.dtype = bob::io::base::array::t_uint8;
    m_typeinfo_video.nd = 4;
    m_typeinfo_frame.nd = 3;
    m_typeinfo_video.shape[0] = m_nframes;
    m_typeinfo_video.shape[1] = m_typeinfo_frame.shape[0] = 3;
    m_typeinfo_video.shape[2] = m_typeinfo_frame.shape[1] = m_height;
    m_typeinfo_video.shape[3] = m_typeinfo_frame.shape[2] = m_width;
    m_typeinfo_frame.update_strides();
    m_typeinfo_video.update_strides();

  }

  Reader::~Reader() {
  }

  size_t Reader::load(blitz::Array<uint8_t,4>& data,
      bool throw_on_error, void (*check)(void)) const {
    bob::io::base::array::blitz_array tmp(data);
    return load(tmp, throw_on_error, check);
  }

  size_t Reader::load(bob::io::base::array::interface& b,
      bool throw_on_error, void (*check)(void)) const {

    //checks if the output array shape conforms to the video specifications,
    //otherwise, throw.
    if (!m_typeinfo_video.is_compatible(b.type())) {
      boost::format s("input buffer (%s) does not conform to the video size specifications (%s)");
      s % b.type().str() % m_typeinfo_video.str();
      throw std::runtime_error(s.str());
    }

    unsigned long int frame_size = m_typeinfo_frame.buffer_size();
    uint8_t* ptr = static_cast<uint8_t*>(b.ptr());
    size_t frames_read = 0;

    for (const_iterator it=begin(); it!=end();) {
      if (check) check(); ///< runs user check function before we start our work
      bob::io::base::array::blitz_array ref(static_cast<void*>(ptr), m_typeinfo_frame);
      if (it.read(ref, throw_on_error)) {
        ptr += frame_size;
        ++frames_read;
      }
      //otherwise we don't count!
    }

    return frames_read;
  }

  Reader::const_iterator Reader::begin() const {
    return Reader::const_iterator(this);
  }

  Reader::const_iterator Reader::end() const {
    return Reader::const_iterator();
  }

  Reader::const_iterator::const_iterator(const Reader* parent) :
    m_parent(parent),
    m_current_frame(std::numeric_limits<size_t>::max())
  {
    init();
  }

  Reader::const_iterator::const_iterator():
    m_parent(0),
    m_current_frame(std::numeric_limits<size_t>::max())
  {
  }

  Reader::const_iterator::const_iterator
    (const Reader::const_iterator& other) :
      m_parent(other.m_parent),
      m_current_frame(std::numeric_limits<size_t>::max())
  {
    init();
    (*this) += other.m_current_frame;
  }

  Reader::const_iterator::~const_iterator() {
    reset();
  }

  Reader::const_iterator& Reader::const_iterator::operator= (const Reader::const_iterator& other) {
    reset();
    m_parent = other.m_parent;
    init();
    (*this) += other.m_current_frame;
    return *this;
  }

  void Reader::const_iterator::init() {

    //ffmpeg initialization
    const std::string& filename = m_parent->filename();
    m_format_context = make_input_format_context(filename);
    m_stream_index = find_video_stream(filename, m_format_context);
    m_codec = find_decoder(filename, m_format_context, m_stream_index);
    m_codec_context = make_codec_context(filename,
        m_format_context->streams[m_stream_index], m_codec);
    m_swscaler = make_scaler(filename, m_codec_context,
        m_codec_context->pix_fmt, AV_PIX_FMT_RGB24);
    m_context_frame = make_empty_frame(filename);
    m_rgb_array.reference(blitz::Array<uint8_t,3>(m_codec_context->height,
          m_codec_context->width, 3));

    //at this point we are ready to start reading out frames.
    m_current_frame = 0;

    //the file maybe valid, but contain zero frames... We check for this here:
    if (m_current_frame >= m_parent->numberOfFrames()) {
      //transforms the current iterator in "end"
      reset();
    }

  }

  void Reader::const_iterator::reset() {
    m_context_frame.reset();
    m_swscaler.reset();
    m_codec_context.reset();
    m_codec = 0;
    m_format_context.reset();
    m_current_frame = std::numeric_limits<size_t>::max(); //that means "end"
    m_parent = 0;
  }

  bool Reader::const_iterator::read(blitz::Array<uint8_t,3>& data,
      bool throw_on_error) {
    bob::io::base::array::blitz_array tmp(data);
    return read(tmp, throw_on_error);
  }

  bool Reader::const_iterator::read(bob::io::base::array::interface& data,
      bool throw_on_error) {

    if (!m_parent) {
      //we are already past the end of the stream
      throw std::runtime_error("video iterator for file has already reached its end and was reset");
    }

    //checks if we have not passed the end of the video sequence already
    if(m_current_frame >= m_parent->numberOfFrames()) {

      if (throw_on_error) {
        boost::format m("you are trying to read past the file end (next frame no. to be read would be %d) on file %s, which contains only %d frames");
        m % m_current_frame % m_parent->m_filepath % m_parent->m_nframes;
        throw std::runtime_error(m.str());
      }

      reset();
      return false;
    }

    const bob::io::base::array::typeinfo& info = data.type();

    //checks if the output array shape conforms to the video specifications,
    //otherwise, throw
    if (!info.is_compatible(m_parent->m_typeinfo_frame)) {
      boost::format s("input buffer (%s) does not conform to the video frame size specifications (%s)");
      s % info.str() % m_parent->m_typeinfo_frame.str();
      throw std::runtime_error(s.str());
    }

    //we are going to need another copy step - use our internal array
    bool ok = read_video_frame(m_parent->m_filepath, m_current_frame,
        m_stream_index, m_format_context, m_codec_context, m_swscaler,
        m_context_frame, m_rgb_array.data(), throw_on_error);

    if (ok) {

      //now we copy from one container to the other, using our Blitz++ technique
      blitz::TinyVector<int,3> shape;
      blitz::TinyVector<int,3> stride;

      shape = info.shape[0], info.shape[1], info.shape[2];
      stride = info.stride[0], info.stride[1], info.stride[2];
      blitz::Array<uint8_t,3> dst(static_cast<uint8_t*>(data.ptr()),
          shape, stride, blitz::neverDeleteData);

      dst = m_rgb_array.transpose(2,0,1);
      ++m_current_frame;

    }

    return ok;
  }

  /**
   * This method does essentially the same as read(), except it skips a few
   * operations to get a better performance.
   */
  Reader::const_iterator& Reader::const_iterator::operator++ () {
    if (!m_parent) {
      //we are already past the end of the stream
      throw std::runtime_error("video iterator for file has already reached its end and was reset");
    }

    //checks if we have not passed the end of the video sequence already
    if(m_current_frame >= m_parent->numberOfFrames()) {
      reset();
      return *this;
    }

    //we are going to need another copy step - use our internal array
    try {
      bool ok = skip_video_frame(m_parent->m_filepath, m_current_frame,
          m_stream_index, m_format_context, m_codec_context, m_context_frame,
          true);
      if (ok) ++m_current_frame;
    }
    catch (std::runtime_error& e) {
      reset();
    }

    return *this;
  }

  Reader::const_iterator& Reader::const_iterator::operator+= (size_t frames) {
    for (size_t i=0; i<frames; ++i) ++(*this);
    return *this;
  }

  bool Reader::const_iterator::operator== (const const_iterator& other) {
    return (this->m_parent == other.m_parent) && (this->m_current_frame == other.m_current_frame);
  }

  bool Reader::const_iterator::operator!= (const const_iterator& other) {
    return !(*this == other);
  }

}}}
