// -*- C++ -*-

//=============================================================================
/**
 *  @file    String_Base.h
 *
 *  String_Base.h,v 4.23 2005/06/03 17:49:36 shuston Exp
 *
 *  @author Douglas C. Schmidt (schmidt@cs.wustl.edu)
 *  @author Nanbor Wang <nanbor@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_STRING_BASE_H
#define ACE_STRING_BASE_H

#include /**/ "ace/pre.h"

#include "ace/String_Base_Const.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Global_Macros.h"

class ACE_Allocator;

/**
 * @class ACE_String_Base
 *
 * @brief This class provides a wrapper facade for C strings.
 *
 * This class uses an ACE_Allocator to allocate memory.  The
 * user can make this a persistant class by providing an
 * ACE_Allocator with a persistable memory pool.  This class is
 * optimized for efficiency, so it doesn't provide any internal
 * locking.
 * NOTE: if an instance of this class is constructed from or
 * assigned an empty string (with first element of '\0'), then it
 * is not allocated new space.  Instead, its internal
 * representation is set equal to a global empty string.
 * CAUTION: in cases when ACE_String_Base is constructed from a
 * provided buffer with the release parameter set to 0,
 * ACE_String_Base is not guaranteed to be '\0' terminated.
 */
template <class CHAR> class ACE_String_Base : public ACE_String_Base_Const
{
public:
   /**
    *  Default constructor.
    *
    *  @param the_allocator ACE_Allocator associated with string
    *  @return Default ACE_String_Base string.
    */
  ACE_String_Base (ACE_Allocator *the_allocator = 0);

  /**
   * Constructor that copies @a s into dynamically allocated memory.
   *
   * if release == 1 then a new buffer is allocated internally, and
   *   s is copied to the internal buffer.
   * if release == 0 then the s buffer is used directly. If s == 0
   *   then it will _not_ be used, and instead the internal buffer
   *   is set to NULL_String_.
   *
   * @param s Zero terminated input string
   * @param the_allocator ACE_Allocator associated with string
   * @param release Allocator responsible(1)/not reponsible(0) for
   *    freeing memory.
   * @return ACE_String_Base containing const CHAR *s
   */
  ACE_String_Base (const CHAR *s,
                   ACE_Allocator *the_allocator = 0,
                   int release = 1);

  /**
   * Constructor that copies @a len CHARs of @a s into dynamically
   * allocated memory (will zero terminate the result).
   *
   * if release == 1 then a new buffer is allocated internally.
   *   s is copied to the internal buffer.
   * if release == 0 then the s buffer is used directly. If s == 0
   *   then it will _not_ be used, and instead the internal buffer
   *   is set to NULL_String_.
   *
   * @param s Non-zero terminated input string
   * @param len Length of non-zero terminated input string
   * @param the_allocator ACE_Allocator associated with string
   * @param release Allocator responsible(1)/not reponsible(0) for
   *    freeing memory.
   * @return ACE_String_Base containing const CHAR *s
   */
  ACE_String_Base (const CHAR *s,
                   size_t len,
                   ACE_Allocator *the_allocator = 0,
                   int release = 1);

  /**
   *  Copy constructor.
   *
   *  @param s Input ACE_String_Base string to copy
   *  @return Copy of input string @a s
   */
  ACE_String_Base (const ACE_String_Base < CHAR > &s);

  /**
   *  Constructor that copies @a c into dynamically allocated memory.
   *
   *  @param c Single input character.
   *  @param the_allocator ACE_Allocator associated with string
   *  @return ACE_String_Base containing CHAR 'c'
   */
  ACE_String_Base (CHAR c, ACE_Allocator *the_allocator = 0);

  /**
   *  Constructor that allocates a len long string.
   *
   *  Warning : This constructor was incorrectly documented in the past.
   *  It simply calls resize(len, c).
   *  It is probably not advisable to use the second parameter. See
   *  resize() for more information.
   *
   *  @param len Amount of space to reserve for the string.
   *  @param c The array is filled with c's
   *  @param the_allocator ACE_Allocator associated with string
   *  @return Empty ACE_String_Base with room for len CHARs
   */
  ACE_String_Base (size_t len,
                   CHAR c = 0,
                   ACE_Allocator *the_allocator = 0);

  /**
   *  Deletes the memory...
   */
  ~ACE_String_Base (void);

  /**
   * Return the <slot'th> character in the string (doesn't perform
   * bounds checking).
   *
   * @param slot Index of the desired character
   * @return The character at index @a slot
   */
  const CHAR & operator[] (size_t slot) const;

  /**
   * Return the <slot'th> character by reference in the string
   * (doesn't perform bounds checking).
   *
   * @param slot Index of the desired character
   * @return The character at index @a slot
   */
  CHAR & operator[] (size_t slot);

  /**
   *  Assignment operator (does copy memory).
   *
   *  @param s Input ACE_String_Base string to assign to this object.
   *  @return Return a copy of the this string.
   */
  ACE_String_Base < CHAR > &operator = (const ACE_String_Base < CHAR > &s);

  /**
   *  Assignment alternative method (does not copy memory).
   *
   *  @param s Input ACE_String_Base string to assign to this object.
   *  @return Return this string.
   */
  ACE_String_Base < CHAR > &assign_nocopy (const ACE_String_Base < CHAR > &s);

  /**
   * Copy @a s into this @a ACE_String_Base.
   *
   * If release == 1 then a new buffer is allocated internally if the
   *   existing one is not big enough to hold s. If the existing
   *   buffer is big enough, then it will be used. This means that
   *   set(*, 1) can be illegal when the string is constructed with a
   *   const char*. (e.g. ACE_String_Base("test", 0, 0)).
   *
   * if release == 0 then the s buffer is used directly, and any
   *   existing buffer is destroyed. If s == 0 then it will _not_ be
   *   used, and instead the internal buffer is set to NULL_String_.
   *
   * @param s Null terminated input string
   * @param release Allocator responsible(1)/not reponsible(0) for
   *    freeing memory.
   */
  void set (const CHAR * s, int release = 1);

  /**
   *  Copy @a len bytes of @a s (will zero terminate the result).
   *
   * If release == 1 then a new buffer is allocated internally if the
   *   existing one is not big enough to hold s. If the existing
   *   buffer is big enough, then it will be used. This means that
   *   set(*, *, 1) is illegal when the string is constructed with a
   *   non-owned const char*. (e.g. ACE_String_Base("test", 0, 0))
   *
   * If release == 0 then the s buffer is used directly, and any
   *   existing buffer is destroyed. If s == 0 then it will _not_ be
   *   used, and instead the internal buffer is set to NULL_String_.
   *
   *  @param s Non-zero terminated input string
   *  @param len Length of input string 's'
   *  @param release Allocator responsible(1)/not reponsible(0) for
   *    freeing memory.
   */
  void set (const CHAR * s, size_t len, int release);

  /**
   * Clear this string. Memory is _not_ freed if <release> is 0.
   *
   * Warning: This method was incorrectly documented in the past, but
   * the current implementation has been changed to match the documented
   * behavior.
   *
   * Warning: clear(0) behaves like fast_clear() below.
   *
   * @param release Memory is freed if 1 or not if 0.
   */
  void clear (int release = 0);

  /**
   * A more specialized version of clear(): "fast clear". fast_clear()
   * resets the string to 0 length. If the string owns the buffer
   * (@arg release_== 1):
   *  - the string buffer is not freed
   *  - the first character of the buffer is set to 0.
   *
   * If @arg release_ is 0 (this object does not own the buffer):
   *  - the buffer pointer is reset to the NULL_String_ and does not
   *    maintain a pointer to the caller-supplied buffer on return
   *  - the maximum string length is reset to 0.
   *
   * Warning : Calling clear(0) or fast_clear() can have unintended
   *   side-effects if the string was constructed (or set()) with an
   *   external buffer. The string will be disassociated with the buffer
   *   and the next append() or +=() will cause a new buffer to be
   *   allocated internally.
   */
  void fast_clear (void);

  /**
   * Return a substring given an offset and length, if length == -1
   * use rest of str.  Return empty substring if offset or
   * offset/length are invalid.
   *
   * @param offset Index of first desired character of the substring.
   * @param length How many characters to return starting at the offset.
   * @return The string containing the desired substring
   */
  ACE_String_Base < CHAR > substring (size_t offset,
                                      ssize_t length = -1) const;

  /**
   *  Same as <substring>.
   *
   * @param offset Index of first desired character of the substring.
   * @param length How many characters to return starting at the offset.
   * @return The string containing the desired substring
   */
  ACE_String_Base < CHAR > substr (size_t offset, ssize_t length = -1) const;

  /**
   *  Concat operator (copies memory).
   *
   *  @param s Input ACE_String_Base string to concatenate to this string.
   *  @return The combined string (input append to the end of the old). New
   *    string is zero terminated.
   */
  ACE_String_Base < CHAR > &operator += (const ACE_String_Base < CHAR > &s);

  /**
   *  Concat operator (copies memory).
   *
   *  @param s Input C string to concatenate to this string.
   *  @return The combined string (input append to the end of the old). New
   *    string is zero terminated.
   */
  ACE_String_Base < CHAR >& operator += (const CHAR* s);

  /**
   *  Concat operator (copies memory).
   *
   *  @param c Input CHAR to concatenate to this string.
   *  @return The combined string (input append to the end of the old). New
   *    string is zero terminated.
   */
  ACE_String_Base < CHAR >& operator += (const CHAR c);

  /**
   *  Append function (copies memory).
   *
   *  @param s Input CHAR array to concatenate to this string.
   *  @param slen The length of the array.
   *  @return The combined string (input append to the end of the old). New
   *    string is zero terminated.
   */
  ACE_String_Base < CHAR >& append (const CHAR* s, size_t slen);

  /**
   *  Returns a hash value for this string.
   *
   *  @return Hash value of string
   */
  u_long hash (void) const;

  /**
   *  Return the length of the string.
   *
   *  @return Length of stored string
   */
  size_t length (void) const;

  /**
   * Get a copy of the underlying representation.
   *
   * This method allocates memory for a copy of the string and returns
   * a pointer to the new area. The caller is responsible for freeing
   * the memory when finished; use delete []
   *
   * @return Pointer reference to the string data. Returned string is
   *    zero terminated.
   */
  CHAR *rep (void) const;

  /**
   * Get at the underlying representation directly!
   * _Don't_ even think about casting the result to (char *) and modifying it,
   * if it has length 0!
   *
   * @return Pointer reference to the stored string data. No guarantee is
   *    that the string is zero terminated.
   *
   */
  const CHAR *fast_rep (void) const;

  /**
   *  Same as STL String's <c_str> and <fast_rep>.
   */
  const CHAR *c_str (void) const;

  /**
   *  Comparison operator that will match substrings.  Returns the
   *  slot of the first location that matches, else -1.
   *
   *  @param s Input ACE_String_Base string
   *  @return Integer index value of the first location of string @a s or
   *    -1 (not found).
   */
  ssize_t strstr (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Find <str> starting at pos.  Returns the slot of the first
   *  location that matches (will be >= pos), else npos.
   *
   *  @param str Input ACE_String_Base string to search for in stored string.
   *  @param pos Starting index position to start searching for string @a str.
   *  @return Index value of the first location of string @a str else npos.
   */
  ssize_t find (const ACE_String_Base<CHAR> &str, size_t pos = 0) const;

  /**
   *  Find @a s starting at pos.  Returns the slot of the first
   *  location that matches (will be >= pos), else npos.
   *
   *  @param s non-zero input string to search for in stored string.
   *  @param pos Starting index position to start searching for string @a str.
   *  @return Index value of the first location of string @a str else npos.
   */
  ssize_t find (const CHAR *s, size_t pos = 0) const;

  /**
   *  Find @a c starting at pos.  Returns the slot of the first
   *  location that matches (will be >= pos), else npos.
   *
   *  @param c Input character to search for in stored string.
   *  @param pos Starting index position to start searching for string @a str.
   *  @return Index value of the first location of string @a str else npos.
   */
  ssize_t find (CHAR c, size_t pos = 0) const;

  /**
   *  Find @a c starting at pos (counting from the end).  Returns the
   *  slot of the first location that matches, else npos.
   *
   *  @param c Input character to search for in stored string.
   *  @param pos Starting index position to start searching for string @a str.
   *  @return Index value of the first location of string @a str else npos.
   */
  ssize_t rfind (CHAR c, ssize_t pos = npos) const;

  /**
   *  Equality comparison operator (must match entire string).
   *
   * @param s Input ACE_String_Base string to compare against stored string.
   * @return Integer value of result (@c true = found, @c false = not
   * found).
   */
  bool operator == (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Less than comparison operator.
   *
   *  @param s Input ACE_String_Base string to compare against stored string.
   *  @return Integer value of result (@c true = less than, @c false =
   *  greater than or equal).
   */
  bool operator < (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Greater than comparison operator.
   *
   *  @param s Input ACE_String_Base string to compare against stored string.
   *  @return Integer value of result (@c true = greater than, @c
   *  false = less than or equal).
   */
  bool operator > (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Inequality comparison operator.
   *
   *  @param s Input ACE_String_Base string to compare against stored string.
   *  @return Integer value of result (@c true = not equal, @c false = equal).
   */
  bool operator != (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Performs a strncmp comparison.
   *
   *  @param s Input ACE_String_Base string to compare against stored string.
   *  @return Integer value of result (less than 0, 0, greater than 0)
   *    depending on how input string @a s is to the stored string.
   */
  int compare (const ACE_String_Base<CHAR> &s) const;

  /**
   *  Dump the state of an object.
   */
  void dump (void) const;

  /**
   * This method is designed for high-performance. Please use with
   * care ;-)
   *
   * Warning : This method was documented incorrectly in the past.
   * The original intention was to change the length of the string to
   * len, and to fill the whole thing with c CHARs.
   * However, what was actually done was to set the length of the
   * string to zero, and fill the buffer with c's. The buffer was
   * also not null-terminated unless c happened to be zero.
   * Rather than fix the method to work as documented, the code is
   * left as is, but the second parameter should probably not be used.
   *
   * @param len The number of CHARs to reserve
   * @param c The CHAR to use when filling the string.
   */
  void resize (size_t len, CHAR c = 0);

  /**
   *  Declare the dynamic allocation hooks.
   */
  ACE_ALLOC_HOOK_DECLARE;

protected:
  /**
   *  Pointer to a memory allocator.
   */
  ACE_Allocator *allocator_;

  /**
   *  Length of the ACE_String_Base data (not counting the trailing '\0').
   */
  size_t len_;

  /**
   *  Length of the ACE_String_Base data buffer.  Keeping track of the
   *  length allows to avoid unnecessary dynamic allocations.
   */
  size_t buf_len_;

  /**
   *  Pointer to data.
   */
  CHAR *rep_;

  /**
   *  Flag that indicates if we own the memory
   */
  int release_;

  /**
   *  Represents the "NULL" string to simplify the internal logic.
   */
  static CHAR NULL_String_;
};

template < class CHAR >
  ACE_String_Base < CHAR > operator + (const ACE_String_Base < CHAR > &,
                                       const ACE_String_Base < CHAR > &);
template < class CHAR >
  ACE_String_Base < CHAR > operator + (const ACE_String_Base < CHAR > &,
                                       const CHAR *);
template < class CHAR >
  ACE_String_Base < CHAR > operator + (const CHAR *,
                                       const ACE_String_Base < CHAR > &);

template < class CHAR >
  ACE_String_Base < CHAR > operator + (const ACE_String_Base < CHAR > &t,
                                       const CHAR c);

template < class CHAR >
  ACE_String_Base < CHAR > operator + (const CHAR c,
                                       const ACE_String_Base < CHAR > &t);

#if defined (__ACE_INLINE__)
#include "ace/String_Base.inl"
#endif /* __ACE_INLINE__ */

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "ace/String_Base.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("String_Base.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#include /**/ "ace/post.h"

#endif /* ACE_STRING_BASE_H */
