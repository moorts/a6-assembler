///////////////////////////////////////////////////////////////////////////////
//
//	OutputFactory.h
//
//	$Id: OutputFactory.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Copyright (C) Simon Collis 2000-2012
//
//-----------------------------------------------------------------------------
//
//	This file is part of a6.
//
//	a6 is free software: you can redistribute it and/or modify it under the
//	terms of the GNU General Public License as published by the Free Software
//	Foundation, either version 3 of the License, or (at your option) any later
//	version.
//
//	a6 is distributed in the hope that it will be useful, but WITHOUT ANY
//	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//	FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
//	details.
//
//	You should have received a copy of the GNU General Public License
//	along with a6.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OUTF_H
#define _OUTF_H

#define OUTF_BIN (0)
#define OUTF_PRG (1)
#define OUTF_P00 (2)
#define OUTF_MHX (3)
#define OUTF_IHX (4)

#define ENDIAN_LITTLE (0)
#define ENDIAN_BIG (1)
#define ENDIAN_PDP (2)

extern int outf_format;

void outf_open(char *);
void outf_close(void);
void outf_wbyte(unsigned int);
void outf_wword(unsigned int);
void outf_wdata(unsigned long, int, int);

unsigned int outf_getpc(void);
void outf_setpc(unsigned int);

#endif
