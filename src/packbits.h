/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2018-2019 Thomas Bernard
    Copyright 2008 Yves Rizoud
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

///@file packbits.h
/// Packbits compression as used in IFF etc.

#ifndef PACKBITS_H_INCLUDED
#define PACKBITS_H_INCLUDED

// error codes :

#define PACKBITS_UNPACK_OK 0
#define PACKBITS_UNPACK_READ_ERROR -1
#define PACKBITS_UNPACK_OVERFLOW_ERROR -2

/**
 * @return PACKBITS_UNPACK_OK or PACKBITS_UNPACK_READ_ERROR or PACKBITS_UNPACK_OVERFLOW_ERROR
 */
int PackBits_unpack_from_file(FILE * f, byte * dest, unsigned int count);

#endif