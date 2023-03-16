///////////////////////////////////////////////////////////////////////////////
//
//	Cpu.h
//
//	$Id: Cpu.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
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
//////////////////////////////////////////////////////////////////////////////

/* OPCODES */

#ifndef _PSOP_H
#define _PSOP_H

#define IMPLIED (0)
#define IMMEDIATE (1)
#define INDIRECT (2)
#define INDIRECT_Y (3)
#define INDIRECT_X (4)
#define ZEROPAGE (5)
#define ABSOLUTE (6)
#define ZEROPAGE_Y (7)
#define ABSOLUTE_Y (8)
#define ZEROPAGE_X (9)
#define ABSOLUTE_X (10)
#define RELATIVE (11)

#define ADRMODECOUNT (12)

#define NOUNDOCOPS (0)
#define UNDOCOPS (1)

// Endianisms
typedef enum { Little, Big, Pdp } Endian;

extern int psop_dotflag;

void parseopcode(char *, char *);

int cpu_getnop(void);
#endif
