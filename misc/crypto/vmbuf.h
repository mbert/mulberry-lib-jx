/*	$KAME: vmbuf.h,v 1.7 2000/10/04 17:41:05 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _VMBUF_H_
#define _VMBUF_H_

#include <sys/types.h>

typedef struct _vchar_t_
{
	size_t	l;	/* length of the value */
	caddr_t	v;	/* pointer to the data */
}
	vchar_t;

#define VREALLOC(ptr, size) ((ptr) = vrealloc((ptr), (size)))

#define VPTRINIT(p) \
do { \
	if (p) { \
		vfree(p); \
		(p) = NULL; \
	} \
} while(0);

extern vchar_t *vmalloc __P((const size_t));
extern vchar_t *vrealloc __P((vchar_t *, const size_t));
extern void vfree __P((vchar_t *));
extern vchar_t *vdup __P((const vchar_t *));
extern vchar_t *vcopy __P((const void *, const size_t));
extern char *vstr __P((const vchar_t *));
extern int vcmp __P((const vchar_t*, const vchar_t*));

#endif
