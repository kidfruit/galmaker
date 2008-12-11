/* /////////////////////////////////////////////////////////////////////////////
 * File:        dirent.c
 *
 * Purpose:     Definition of the opendir() API functions for the Win32 platform.
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


#include <windows.h>
#include <errno.h>

#include "dirent.h"

#define _SYNSOFT_VER_C_DIRENT_MAJOR       1
#define _SYNSOFT_VER_C_DIRENT_MINOR       0
#define _SYNSOFT_VER_C_DIRENT_REVISION    1
#define _SYNSOFT_VER_C_DIRENT_EDIT        15

/* /////////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

struct dirent_dir
{
    char            directory[_MAX_DIR + 1];    /* . */
    WIN32_FIND_DATA find_data;                  /* The Win32 FindFile data. */
    HANDLE          hFind;                      /* The Win32 FindFile handle. */
    struct dirent   dirent;                     /* The handle's entry. */
};

/* /////////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

static HANDLE findfile_directory(char const *name, LPWIN32_FIND_DATA data)
{
    char    search_spec[_MAX_PATH +1];

    /* Simply add the *.*, ensuring the path separator is
     * included.
     */
    lstrcpyA(search_spec, name);
    if(search_spec[lstrlenA(search_spec) - 1] != '\\')
    {
        lstrcatA(search_spec, "\\*.*");
    }
    else
    {
        lstrcatA(search_spec, "*.*");
    }

    return FindFirstFileA(search_spec, data);
}

/* /////////////////////////////////////////////////////////////////////////////
 * API functions
 */

DIR *opendir(char const *name)
{
    DIR     *result =   NULL;
    DWORD   dwAttr;

    /* Must be a valid name */
    if( !name ||
        !*name ||
        (dwAttr = GetFileAttributes(name)) == 0xFFFFFFFF)
    {
        errno = ENOENT;
    }
    /* Must be a directory */
    else if(!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
    {
        errno = ENOTDIR;
    }
    else
    {
        result = (DIR*)malloc(sizeof(DIR));

        if(result == NULL)
        {
            errno = ENOMEM;
        }
        else
        {
            result->hFind = findfile_directory(name, &result->find_data);

            if(result->hFind == INVALID_HANDLE_VALUE)
            {
                free(result);

                result = NULL;
            }
            else
            {
                /* Save the directory, in case of rewind. */
                lstrcpyA(result->directory, name);
                lstrcpyA(result->dirent.d_name, result->find_data.cFileName);
            }
        }
    }

    return result;
}

int closedir(DIR *dir)
{
    int ret;

    if(dir == NULL)
    {
        errno = EBADF;

        ret = -1;
    }
    else
    {
        /* Close the search handle, if not already done. */
        if(dir->hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(dir->hFind);
        }

        free(dir);

        ret = 0;
    }

    return ret;
}

void rewinddir(DIR *dir)
{
    /* Close the search handle, if not already done. */
    if(dir->hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(dir->hFind);
    }

    dir->hFind = findfile_directory(dir->directory, &dir->find_data);

    if(dir->hFind != INVALID_HANDLE_VALUE)
    {
        lstrcpyA(dir->dirent.d_name, dir->find_data.cFileName);
    }
}

struct dirent *readdir(DIR *dir)
{
    /* The last find exhausted the matches, so return NULL. */
    if(dir->hFind == INVALID_HANDLE_VALUE)
    {
        errno = EBADF;

        return NULL;
    }
    else
    {
        /* Copy the result of the last successful match to
         * dirent.
         */
        lstrcpyA(dir->dirent.d_name, dir->find_data.cFileName);

        /* Attempt the next match. */
        if(!FindNextFileA(dir->hFind, &dir->find_data))
        {
            /* Exhausted all matches, so close and null the
             * handle.
             */
            FindClose(dir->hFind);
            dir->hFind = INVALID_HANDLE_VALUE;
        }

        return &dir->dirent;
    }
}

/* ////////////////////////////////////////////////////////////////////////// */
