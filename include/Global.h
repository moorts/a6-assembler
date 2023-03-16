///////////////////////////////////////////////////////////////////////////////
//
//	Global.h
//
//	$Id: Global.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
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

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------
 * Global defines
 *
 * This section for settings that affect the whole of A6
 *---------------------------------------------------------------------------*/

/* Maximum number of (open) assembly files
 * This is the number of persistent names that it stores; i.e. the number
 * of file IDs that can be in a single assembly pass.
 * TODO: replace with a linked list and dynamic allocation.
 */
#define	MAX_ASMFILES	64

/*---------------------------------------------------------------------------
 * Global variables
 *
 * Deprecated; the idea is to shrink this section, not grow it  :-)
 *---------------------------------------------------------------------------*/
extern int	g_listflag;
extern FILE	*g_listout;

extern int	g_pass;
extern int	g_endofpass;

extern int	g_undocopsflag;

extern int	g_outf_add;
extern int	g_outf_eor;

extern int	g_outf_format;

extern char	*g_outname;

/*---------------------------------------------------------------------------
 * Global functions
 *
 * Stuff that gets used over and over again...
 *---------------------------------------------------------------------------*/

/* Malloc or die */
void	*a6malloc(size_t size);

#endif
