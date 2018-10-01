/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.7
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace cAudio {

using System;
using System.Runtime.InteropServices;

public class AudioCaptureBuffer : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal AudioCaptureBuffer(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(AudioCaptureBuffer obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~AudioCaptureBuffer() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          cAudioCSharpWrapperPINVOKE.delete_AudioCaptureBuffer(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public AudioCaptureBuffer(uint inlength) : this(cAudioCSharpWrapperPINVOKE.new_AudioCaptureBuffer__SWIG_0(inlength), true) {
  }

  public AudioCaptureBuffer(AudioCaptureBuffer p) : this(cAudioCSharpWrapperPINVOKE.new_AudioCaptureBuffer__SWIG_1(AudioCaptureBuffer.getCPtr(p)), true) {
    if (cAudioCSharpWrapperPINVOKE.SWIGPendingException.Pending) throw cAudioCSharpWrapperPINVOKE.SWIGPendingException.Retrieve();
  }

  public string getReadBuffer() {
    string ret = cAudioCSharpWrapperPINVOKE.AudioCaptureBuffer_getReadBuffer(swigCPtr);
    return ret;
  }

  public string getWriteBuffer() {
    string ret = cAudioCSharpWrapperPINVOKE.AudioCaptureBuffer_getWriteBuffer(swigCPtr);
    return ret;
  }

  public uint getLength() {
    uint ret = cAudioCSharpWrapperPINVOKE.AudioCaptureBuffer_getLength(swigCPtr);
    return ret;
  }

}

}
