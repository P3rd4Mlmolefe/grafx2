/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2020 Thomas Bernard
    Copyright 2011 Pawel Góralski
    Copyright 2008 Yves Rizoud
    Copyright 2008 Franck Charlet
    Copyright 2007 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
//////////////////////////////////////////////////////////////////////////////
///@file osdep.h
/// OS Dependend code
//////////////////////////////////////////////////////////////////////////////

#ifndef OSDEP_H_DEFINED
#define OSDEP_H_DEFINED

/// Return a number of milliseconds
dword GFX2_GetTicks(void);

/**
 * Open an URL in the system default browser
 * @param url URL (ascii)
 * @param len URL length in bytes
 */
void GFX2_OpenURL(const char * url, unsigned int len);

/**
 * Disk free space
 */
qword GFX2_DiskFreeSpace(const char * path);

#if defined (__MINT__)
void Atari_Memory_free(unsigned long *stRam, unsigned long *ttRam);
#else
/**
 * @return the number of RAM bytes available
 */
unsigned long Memory_free(void);
#endif

#if defined(WIN32)
char * Unicode_to_utf8(const word * str, size_t * utf8len);
#endif

#endif
