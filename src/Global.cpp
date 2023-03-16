///////////////////////////////////////////////////////////////////////////////
//
//	Global.cpp
//
//	$Id: Global.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Global variables.
//	Make them die.
//	Make them die now.
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

#include <stdio.h>
#include <stdlib.h>

#include "Error.h"

/* TODO: remove all these global variables!!!
 */

/* Listing to stdout?  0=no */
int g_listflag = 0;
FILE *g_listout;

/* Pass 2?  1=yes */
int g_pass;

/* End of current pass?  1=yes */
int g_endofpass;

/* Undocumented opcodes on/off */
int g_undocopsflag = 0;

/* Encode bytes */
int g_outf_add = 0;
int g_outf_eor = 0;

/* Output format and filename */
int g_outf_format = 0;
char *g_outname;

/*---------------------------------------------------------------------------
 * a6_malloc
 *
 * malloc that either returns the required size or crashes and burns
 *---------------------------------------------------------------------------*/
void *a6malloc(size_t size)
{
	register void *p;

	p = malloc(size);

	if(p == NULL)
		A6Err(Fatal, OutOfMemory);

	return(p);
}

