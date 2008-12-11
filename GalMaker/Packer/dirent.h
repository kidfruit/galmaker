/* /////////////////////////////////////////////////////////////////////////////
 * File:        dirent.h
 *
 * Purpose:     Declaration of the opendir() API functions and types for the
 *              Win32 platform.
 *
 * Created      19th October 2002
 * Updated:     2nd February 2003
 *
 * Author:      Matthew Wilson, Synesis Software Pty Ltd.
 *
 * License:     (Licensed under the Synesis Software Standard Source License)
 *
 *              Copyright (C) 2002, Synesis Software Pty Ltd.
 *
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/
 *                          http://www.synesis.com.au/software/
 *
 *              email:      software@synesis.com.au
 *                          software@synesis-group.com
 *
 *              Redistribution and use in source and binary forms, with or
 *              without modification, are permitted provided that the following
 *              conditions are met:
 *
 *              (i) Redistributions of source code must retain the above
 *              copyright notice and contact information, this list of
 *              conditions and the following disclaimer.
 *
 *              (ii) Any derived versions of this software (howsoever modified)
 *              remain the sole property of Synesis Software.
 *
 *              (iii) Any derived versions of this software (howsoever modified)
 *              remain subject to all these conditions.
 *
 *              (iv) Neither the name of Synesis Software nor the names of any
 *              subdivisions, employees or agents of Synesis Software, nor the
 *              names of any other contributors to this software may be used to
 *              endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              This source code is provided by Synesis Software "as is" and any
 *              warranties, whether expressed or implied, including, but not
 *              limited to, the implied warranties of merchantability and
 *              fitness for a particular purpose are disclaimed. In no event
 *              shall the Synesis Software be liable for any direct, indirect,
 *              incidental, special, exemplary, or consequential damages
 *              (including, but not limited to, procurement of substitute goods
 *              or services; loss of use, data, or profits; or business
 *              interruption) however caused and on any theory of liability,
 *              whether in contract, strict liability, or tort (including
 *              negligence or otherwise) arising in any way out of the use of
 *              this software, even if advised of the possibility of such
 *              damage.
 *
 * ////////////////////////////////////////////////////////////////////////// */


#ifndef _SYNSOFT_INCL_H_DIRENT
#define _SYNSOFT_INCL_H_DIRENT

#define _SYNSOFT_VER_H_DIRENT_MAJOR       1
#define _SYNSOFT_VER_H_DIRENT_MINOR       0
#define _SYNSOFT_VER_H_DIRENT_REVISION    1
#define _SYNSOFT_VER_H_DIRENT_EDIT        9

/* ////////////////////////////////////////////////////////////////////////// */

#ifndef WIN32
 #error This file is only currently defined for Win32 compilation units
#endif /* WIN32 */

/* /////////////////////////////////////////////////////////////////////////////
 * Constants and definitions
 */

#ifndef NAME_MAX
 #define NAME_MAX   (260)
#endif /* !NAME_MAX */

/* /////////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef struct dirent_dir   DIR; /* dirent_dir is defined internally */

struct dirent
{
    char d_name[NAME_MAX + 1];   /* file name (null-terminated) */
};

/* /////////////////////////////////////////////////////////////////////////////
 * API functions
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

DIR             *opendir(const char *name);

int             closedir(DIR *dir);

void            rewinddir(DIR *dir);

struct dirent   *readdir(DIR *dir);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* ////////////////////////////////////////////////////////////////////////// */

#endif /* _SYNSOFT_INCL_H_DIRENT */

/* ////////////////////////////////////////////////////////////////////////// */
