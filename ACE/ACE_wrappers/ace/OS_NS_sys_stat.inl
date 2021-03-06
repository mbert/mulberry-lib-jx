// -*- C++ -*-
// OS_NS_sys_stat.inl,v 1.22 2005/01/04 04:30:02 bala Exp

#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_macros.h"

namespace ACE_OS {

  ACE_INLINE ACE_HANDLE
  creat (const ACE_TCHAR *filename, mode_t mode)
  {
    ACE_OS_TRACE ("ACE_OS::creat");
#if defined (ACE_WIN32)
    return ACE_OS::open (filename, O_CREAT|O_TRUNC|O_WRONLY, mode);
#elif defined(ACE_PSOS)
    ACE_OSCALL_RETURN(::create_f((char *)filename, 1024,
                                 S_IRUSR | S_IWUSR | S_IXUSR),
                      ACE_HANDLE, ACE_INVALID_HANDLE);
#elif defined(ACE_PSOS_TM)
    ACE_UNUSED_ARG (filename);
    ACE_UNUSED_ARG (mode);
    ACE_NOTSUP_RETURN (-1);
#elif defined(ACE_PSOS)
    ACE_UNUSED_ARG (filename);
    ACE_UNUSED_ARG (mode);
    ACE_NOTSUP_RETURN (-1);
#else
    ACE_OSCALL_RETURN (::creat (ACE_TEXT_ALWAYS_CHAR (filename), mode),
                       ACE_HANDLE, ACE_INVALID_HANDLE);
#endif /* ACE_WIN32 */
  }

#if !defined (ACE_WIN32)

  ACE_INLINE int
  fstat (ACE_HANDLE handle, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::fstat");
#if defined (ACE_PSOS_LACKS_PHILE)
    ACE_UNUSED_ARG (handle);
    ACE_UNUSED_ARG (stp);
    ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_PSOS)
    ACE_OSCALL_RETURN (::fstat_f (handle, stp), int, -1);
#else
# if defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for fstat(), this is a wrapper
    // for _fxstat() use of the macro.
    // causes compile and runtime problems.
    ACE_OSCALL_RETURN (::_fxstat (_STAT_VER, handle, stp), int, -1);
# elif defined (ACE_WIN32)
    ACE_OSCALL_RETURN (::_fstat (handle, stp), int, -1);
# else
#  if defined (ACE_OPENVMS)
    ::fsync(handle);
#  endif
    ACE_OSCALL_RETURN (::fstat (handle, stp), int, -1);
# endif /* !ACE_HAS_X86_STAT_MACROS */
#endif /* ACE_PSOS_LACKS_PHILE */
  }

#else /* ACE_WIN32 */

  ACE_INLINE int
  fstat (ACE_HANDLE handle, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::fstat");
# if 1
    BY_HANDLE_FILE_INFORMATION fdata;

    if (::GetFileInformationByHandle (handle, &fdata) == FALSE)
      {
        ACE_OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = ACE_Time_Value (fdata.ftLastAccessTime).sec ();
        stp->st_mtime = ACE_Time_Value (fdata.ftLastWriteTime).sec ();
        stp->st_ctime = ACE_Time_Value (fdata.ftCreationTime).sec ();
        stp->st_nlink = static_cast<short> (fdata.nNumberOfLinks);
        stp->st_dev = stp->st_rdev = 0; // No equivalent conversion.
        stp->st_mode = S_IXOTH | S_IROTH |
          (fdata.dwFileAttributes & FILE_ATTRIBUTE_READONLY ? 0 : S_IWOTH) |
          (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? S_IFDIR : S_IFREG);
      }
    return 0;
# else /* 1 */
    // This implementation close the handle.
    int retval = -1;
    int fd = ::_open_osfhandle ((long) handle, 0);
    if (fd != -1)
      retval = ::_fstat (fd, stp);

    ::_close (fd);
    // Remember to close the file handle.
    return retval;
# endif /* 1 */
  }

#endif /* WIN32 */

  // This function returns the number of bytes in the file referenced by
  // FD.

  ACE_INLINE long
  filesize (ACE_HANDLE handle)
  {
    ACE_OS_TRACE ("ACE_OS::filesize");
#if defined (ACE_WIN32)
    ACE_WIN32CALL_RETURN (::GetFileSize (handle, 0), long, -1);
#else /* !ACE_WIN32 */
    ACE_stat sb;
    return ACE_OS::fstat (handle, &sb) == -1 ? -1 : (long) sb.st_size;
#endif /* ACE_WIN32 */
  }

  ACE_INLINE long
  filesize (const ACE_TCHAR *filename)
  {
    ACE_OS_TRACE ("ACE_OS::filesize");

    ACE_HANDLE h = ACE_OS::open (filename, O_RDONLY);
    if (h != ACE_INVALID_HANDLE)
      {
        long size = ACE_OS::filesize (h);
        ACE_OS::close (h);
        return size;
      }
    else
      return -1;
  }

  ACE_INLINE int
  lstat (const char *file, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::lstat");
# if defined (ACE_LACKS_LSTAT)
    return ACE_OS::stat (file, stp);
# elif defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for lstat(), this macro is a
    // wrapper for _lxstat().
    ACE_OSCALL_RETURN (::_lxstat (_STAT_VER, file, stp), int, -1);
# else /* !ACE_HAS_X86_STAT_MACROS */
    ACE_OSCALL_RETURN (::lstat (file, stp), int, -1);
# endif /* ACE_LACKS_LSTAT */
  }

#if defined (ACE_HAS_WCHAR)
  ACE_INLINE int
  lstat (const wchar_t *file, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::lstat");
# if defined (ACE_LACKS_LSTAT)
    return ACE_OS::stat (file, stp);
# else
    return ACE_OS::lstat (ACE_Wide_To_Ascii (file).char_rep (), stp);
# endif /* ACE_LACKS_LSTAT */
  }
#endif /* ACE_HAS_WCHAR */

  ACE_INLINE int
  mkdir (const char *path, mode_t mode)
  {
#if defined (ACE_PSOS_LACKS_PHILE)
    ACE_UNUSED_ARG (path);
    ACE_UNUSED_ARG (mode);
    ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_PSOS)
    //The pSOS make_dir fails if the last character is a '/'
    int location;
    char *phile_path;

    phile_path = (char *)ACE_OS::malloc (strlen (path+1));
    if (phile_path == 0)
      {
        ACE_OS::printf ("malloc in make_dir failed: [%X]\n",
                        errno);
        return -1;
      }
    else
      ACE_OS::strcpy (phile_path, path);

    location = ACE_OS::strlen(phile_path);
    if(phile_path[location-1] == '/')
      {
        phile_path[location-1] = 0;
      }

    u_long result;
    result = ::make_dir ((char *) phile_path, mode);
    if (result == 0x2011)  // Directory already exists
      {
        result = 0;
      }
    else if (result != 0)
      {
        result = -1;
      }

    ACE_OS::free(phile_path);
    return result;

#elif defined (ACE_WIN32) && defined (__IBMCPP__) && (__IBMCPP__ >= 400)
    ACE_UNUSED_ARG (mode);
    ACE_OSCALL_RETURN (::_mkdir ((char *) path), int, -1);
#elif defined (ACE_HAS_WINCE)
    ACE_UNUSED_ARG (mode);
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CreateDirectory (ACE_TEXT_CHAR_TO_TCHAR (path), 0),
                                            ace_result_),
                          int, -1);
#elif defined (ACE_MKDIR_LACKS_MODE)
    ACE_UNUSED_ARG (mode);
    ACE_OSCALL_RETURN (::mkdir (path), int, -1);
#else
    ACE_OSCALL_RETURN (::mkdir (path, mode), int, -1);
#endif /* ACE_PSOS_LACKS_PHILE */
  }

#if defined (ACE_HAS_WCHAR)

  ACE_INLINE int
  mkdir (const wchar_t *path, mode_t mode)
  {
#if defined (ACE_HAS_WINCE)
    ACE_UNUSED_ARG (mode);
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (CreateDirectory (path, 0),
                                            ace_result_),
                          int, -1);
#elif defined (ACE_WIN32) && defined (ACE_USES_WCHAR)
    ACE_UNUSED_ARG (mode);
    ACE_OSCALL_RETURN (::_wmkdir (path), int, -1);
#else
    return ACE_OS::mkdir (ACE_Wide_To_Ascii (path).char_rep (), mode);
#endif /* ACE_HAS_WINCE */
  }

#endif /* ACE_HAS_WCHAR */

  ACE_INLINE int
  mkfifo (const ACE_TCHAR *file, mode_t mode)
  {
    ACE_OS_TRACE ("ACE_OS::mkfifo");
#if defined (ACE_LACKS_MKFIFO)
    ACE_UNUSED_ARG (file);
    ACE_UNUSED_ARG (mode);
    ACE_NOTSUP_RETURN (-1);
#else
    ACE_OSCALL_RETURN (::mkfifo (ACE_TEXT_ALWAYS_CHAR (file), mode), int, -1);
#endif /* ACE_LACKS_MKFIFO */
  }

  ACE_INLINE int
  stat (const char *file, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::stat");
#if defined (ACE_HAS_NONCONST_STAT)
    ACE_OSCALL_RETURN (::stat (const_cast <char *> (file), stp), int, -1);
#elif defined (ACE_PSOS_LACKS_PHILE)
    ACE_UNUSED_ARG (file);
    ACE_UNUSED_ARG (stp);
    ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_PSOS)
    ACE_OSCALL_RETURN (::stat_f (const_cast <char *> (file), stp), int, -1);
#elif defined (ACE_HAS_WINCE)
    ACE_TEXT_WIN32_FIND_DATA fdata;

    HANDLE fhandle;

    fhandle = ::FindFirstFile (ACE_TEXT_CHAR_TO_TCHAR (file), &fdata);
    if (fhandle == INVALID_HANDLE_VALUE)
      {
        ACE_OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_mode = static_cast<unsigned short>(fdata.dwFileAttributes);
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = ACE_Time_Value (fdata.ftLastAccessTime);
        stp->st_mtime = ACE_Time_Value (fdata.ftLastWriteTime);
      }
    return 0;
#elif defined (ACE_HAS_X86_STAT_MACROS)
    // Solaris for intel uses an macro for stat(), this macro is a
    // wrapper for _xstat().
    ACE_OSCALL_RETURN (::_xstat (_STAT_VER, file, stp), int, -1);
#elif defined (ACE_WIN32)
# if defined(__IBMCPP__)
    ACE_OSCALL_RETURN (::_stat (file,  stp), int, -1);
#else
    ACE_OSCALL_RETURN (::_stat (file, (struct _stat *) stp), int, -1);
#endif /* __IBMCPP__ */
#else /* ACE_HAS_NONCONST_STAT */
    ACE_OSCALL_RETURN (::stat (file, stp), int, -1);
#endif /* ACE_HAS_NONCONST_STAT */
  }

#if defined (ACE_HAS_WCHAR)
  ACE_INLINE int
  stat (const wchar_t *file, ACE_stat *stp)
  {
    ACE_OS_TRACE ("ACE_OS::stat");
#if defined (ACE_HAS_WINCE)
    ACE_TEXT_WIN32_FIND_DATA fdata;

    HANDLE fhandle;

    fhandle = ::FindFirstFile (file, &fdata);
    if (fhandle == INVALID_HANDLE_VALUE)
      {
        ACE_OS::set_errno_to_last_error ();
        return -1;
      }
    else if (fdata.nFileSizeHigh != 0)
      {
        errno = EINVAL;
        return -1;
      }
    else
      {
        stp->st_mode = static_cast<unsigned short>(fdata.dwFileAttributes);
        stp->st_size = fdata.nFileSizeLow;
        stp->st_atime = ACE_Time_Value (fdata.ftLastAccessTime);
        stp->st_mtime = ACE_Time_Value (fdata.ftLastWriteTime);
      }
    return 0;
#elif defined (__BORLANDC__)  && (__BORLANDC__ <= 0x540)
    ACE_OSCALL_RETURN (::_wstat (file, stp), int, -1);
#elif defined (ACE_WIN32)
    ACE_OSCALL_RETURN (::_wstat (file, (struct _stat *) stp), int, -1);
#else /* ACE_HAS_WINCE */
    ACE_Wide_To_Ascii nfile (file);
    return ACE_OS::stat (nfile.char_rep (), stp);
#endif /* ACE_HAS_WINCE */
  }
#endif /* ACE_HAS_WCHAR */

  ACE_INLINE mode_t
  umask (mode_t cmask)
  {
    ACE_OS_TRACE ("ACE_OS::umask");
# if defined (ACE_LACKS_UMASK)
    ACE_UNUSED_ARG (cmask);
    ACE_NOTSUP_RETURN ((mode_t)-1);
# elif defined (ACE_WIN32)
    ACE_OSCALL_RETURN (::_umask (cmask), mode_t, -1);
# else
    return ::umask (cmask); // This call shouldn't fail...
# endif /* ACE_LACKS_UMASK */
  }

} // ACE_OS namespace
