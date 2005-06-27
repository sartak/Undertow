#include "undertow.h"

#ifndef strlcpy
/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t strlcat(char *dst, const char *src, size_t siz)
{
  char *d = dst;
  const char *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != '\0')
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0)
    return(dlen + strlen(s));
  while (*s != '\0') {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return(dlen + (s - src)); /* count does not include NUL */
}

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t siz)
{
  char *d = dst;
  const char *s = src;
  size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0 && --n != 0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    } while (--n != 0);
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (siz != 0)
      *d = '\0';    /* NUL-terminate dst */
    while (*s++)
      ;
  }

  return(s - src - 1);  /* count does not include NUL */
}


#endif /* strlcpy */

/*
 * Copyright (c) 2005 Shawn M. Moore <sartak@gmail.com>
 *
 * Purpose: This function attempts to prepend one string onto another while
 *          preventing buffer overflows.
 *
 * Parameters:
 *         char *dst:  The destination string.
 *         char *src:  The source string.
 *         size_t siz: The maximum number of bytes (null-terminator included) of dst.
 *
 * Return: strlpre's return value is the length of src + length of dst regardless
 *         of how many characters were actually copied into dst.
 *
 * Information: strlpre is a function designed to be analogous to strlcpy and strlcat.
 *              dst[siz] will *never* be written to. dst[siz-1] should always be the
 *              null-terminator. The exception to this is if siz is passed as 0, in
 *              which case the function exits without changing anything. To predict
 *              the behavior of the function when siz is insufficient to hold both
 *              dst and src, imagine the function simply appending dst onto src, and
 *              then chopping at the appropriate spot and null-terminating.
 */

size_t strlpre(char *dst, const char *src, size_t siz)
{
  char *d; /* a pointer to a position in dst */
  const char *s; /* a pointer to a position in src */
  size_t n = 0; /* keeps track of dst's length */
  size_t x = 0; /* keeps track of src's length */
  size_t t = 0; /* keeps track of some temporary length */

  if (siz == 0)
    return x + n;

  /* quick length calculation */
  for (s = src; *s; ++s);
  for (d = dst; *d; ++d);
  x = s - src;
  n = d - dst;

  if (siz <= x)
  {
    for (d = dst, s = src; t < siz - 1; ++t)
      *d++ = *s++;
    *d = 0;
    return x + n;
  }

  if (siz <= x + n)
    d = dst + siz - 1;
  else
    d = dst + x + n;
  for (*d-- = 0; d >= dst + x; --d)
    *d = *(d - x);
  for (--s; t < x; ++t)
    *d-- = *s--;
  return x + n;
}
