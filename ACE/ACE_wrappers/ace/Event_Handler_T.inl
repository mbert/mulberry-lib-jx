// -*- C++ -*-
//
// Event_Handler_T.inl,v 4.2 2005/05/31 01:11:28 ossama Exp


#include "ace/Global_Macros.h"


template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::op_handler (T *op)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::op_handler");
  this->op_handler_ = op;
}

template<class T> ACE_INLINE T *
ACE_Event_Handler_T<T>::op_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::op_handler");
  return this->op_handler_;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::GET_HANDLE
ACE_Event_Handler_T<T>::handle_get (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::handle_get");
  return this->get_handle_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::handle_get (ACE_TYPENAME ACE_Event_Handler_T<T>::GET_HANDLE h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::handle_get");
  this->get_handle_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::SET_HANDLE
ACE_Event_Handler_T<T>::handle_set (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::handle_set");
  return this->set_handle_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::handle_set (ACE_TYPENAME ACE_Event_Handler_T<T>::SET_HANDLE h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::handle_set");
  this->set_handle_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER
ACE_Event_Handler_T<T>::input_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::input_handler");
  return this->input_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::input_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::input_handler");
  this->input_handler_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER
ACE_Event_Handler_T<T>::output_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::output_handler");
  return this->output_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::output_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::output_handler");
  this->output_handler_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER
ACE_Event_Handler_T<T>::except_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::except_handler");
  return this->except_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::except_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::IO_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::except_handler");
  this->except_handler_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::TO_HANDLER
ACE_Event_Handler_T<T>::to_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::to_handler");
  return this->to_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::to_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::TO_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::to_handler");
  this->to_handler_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::CL_HANDLER
ACE_Event_Handler_T<T>::cl_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::cl_handler");
  return this->cl_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::cl_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::CL_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::cl_handler");
  this->cl_handler_ = h;
}

template<class T> ACE_INLINE ACE_TYPENAME ACE_Event_Handler_T<T>::SIG_HANDLER
ACE_Event_Handler_T<T>::sig_handler (void)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::sig_handler");
  return this->sig_handler_;
}

template<class T> ACE_INLINE void
ACE_Event_Handler_T<T>::sig_handler (ACE_TYPENAME ACE_Event_Handler_T<T>::SIG_HANDLER h)
{
  ACE_TRACE ("ACE_Event_Handler_T<T>::sig_handler");
  this->sig_handler_ = h;
}
