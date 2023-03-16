///////////////////////////////////////////////////////////////////////////////
//
//	Label.h
//
//	$Id: Label.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
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

#ifndef _LBL_H
#define _LBL_H

#define LBL_NONE		0
#define LBL_SYNCPC		1
#define LBL_VARIABLE	2

struct label
{
	struct label	*next;
	char			*name;
	char			*filename;
	unsigned long	value;
	unsigned int	line;
	unsigned int	scope;
	unsigned int	type;
	unsigned int	af_pos;
};

/* Init / reset */
void			lbl_destroy(void);
void			lbl_init(void);
void			lbl_reset(void);

/* Local scope functions */
void			lbl_enterlocal(void);
void			lbl_exitlocal(void);
unsigned int	lbl_getscope(void);

/* List functions */
void			lbl_dumpsym(void);

/* Define / find */
void			lbl_define(char *, unsigned long, unsigned int);
struct label	*lbl_getptr(char *, unsigned int);
unsigned long	lbl_getval(char *);

#endif
