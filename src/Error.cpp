///////////////////////////////////////////////////////////////////////////////
//
//	Error.cpp
//
//	$Id: Error.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Error messages
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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "AsmFile.h"
#include "Error.h"
#include "Global.h"

/* Cope with non-ANSI stdlib.h */
#ifndef EXIT_FAILURE
#define EXIT_FAILURE (int)errcount+1
#endif

int errcount = 0;

char *errortext[] =
{
	"internal error",
	"file not found",
	"unterminated string",
	"unknown directive",
	"(unused)",
	"junk (maybe whitespace) after string",
	"second .org encountered",
	"value out of range",
	"unknown character set",
	"unknown opcode",
	"invalid or unknown address mode",
	"illegal character to start label",
	"division by zero",
	"object code overflow",
	"label is not a variable",
	"cannot close file",
	"reserved label",
	"nesting too deep",
	"illegal character in expression",
	"label not defined",
	"stack empty",
	"label too long",
	"too many files",
	"address mode not available",
	"file is not a valid CPU file",
	"conditional assembly not exited",					// 25 - UnclosedCond
	"out of memory",
	"bad endianism (use i[ntel], m[otorola], b[ig], l[ittle] or p[dp])",
	"maximum number of address modes in use"
};

///////////////////////////////////////////////////////////////////////////////
//
// A6Err
//
// Print an error to the user, if appropriate.
// Increment the error count if it isn't a warning.
//

void A6Err(ErrorType type, ErrorCode code)
{
	A6Err(type, code, NULL);
}

void A6Err(ErrorType type, ErrorCode code, char *text)	//int errnum, char *txt, int fatal)
{
	char *w = "error";

	// Return if we are not reporting the error right now...
	switch(type)
	{
	case Pass1:
		if(g_pass > 0) return;
		break;
	case Pass2:
		if(g_pass == 0) return;
		break;
	case Fatal:
		w = "fatal";
		break;
	case Warning:
		if(g_pass == 0) return;
		w = "warning";
		break;
	}

	if(type != Warning)
		errcount++;

	af_printforerror();

	/* Print error */
	int errnum = (int)code;
	if(af_name() != NULL)
	{
		fprintf(stderr, "%s(%u):", af_name(), af_line());
	}

	fprintf(stderr, "%s %u:%s", w, errnum, errortext[errnum]);

	if(text != NULL)
	{
		fprintf(stderr, "at '%s'", text);
	}

	fprintf(stderr, "\n");

	/* Stop if fatal error */
	if(type == Fatal)
	{
		printf("assembly stopped: fatal error\n");
		exit(EXIT_FAILURE);
	}
}

/*---------------------------------------------------------------------------
 * geterrcount
 *
 * Return total number of errors
 *---------------------------------------------------------------------------*/
int geterrcount(void)
{
	return(errcount);
}
