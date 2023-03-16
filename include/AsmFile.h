///////////////////////////////////////////////////////////////////////////////
//
//	AsmFile.h
//
//	$Id: AsmFile.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
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

#ifndef _ASM_H
#define _ASM_H

void af_open(char *,/*@null@*/FILE *);
int af_close(void);
int af_number(void);
char *af_name(void);
unsigned int af_line(void);
void af_reset(void);

int af_linehaschanged(void);
void af_printforerror(void);
void af_cleanup(void);
unsigned int af_pos(void);
int af_parseline(void);

#endif
