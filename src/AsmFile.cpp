///////////////////////////////////////////////////////////////////////////////
//
//	AsmFile.cpp
//
//	$Id: AsmFile.cpp 46 2012-09-16 02:49:15Z simon.collis@gmail.com $
//
//	Main assembly file handler
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


// TODO
//  - move to using the new String class
//  - don't split the original line (will be cached?)

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Main.h"
#include "Conditional.h"
#include "Config.h"
#include "Cpu.h"
#include "Directive.h"
#include "Error.h"
#include "Expression.h"
#include "Global.h"
#include "Label.h"
#include "OutputFactory.h"
#include "A6String.h"

/* The big structure */
struct asmfile
{
	char *filename;
	unsigned int line;
	struct asmfile *previous;
	long pos;
} *current = NULL;

FILE *current_file = NULL;

long lastpos;

/*---------------------------------------------------------------------------*
 * af_postotal
 *
 * total line number, returned by af_pos
 *---------------------------------------------------------------------------*/
unsigned int af_postotal = 0;

/* This is only used by af_getline and af_getcurrentline */
/* oh--and by almost every other file in the program ;-) */
static char currentline[256];


/* Persistent names -- ensure pointers are always unique (save memory) */
char *persistentnames[MAX_ASMFILES];
int filesused = 0;

int line_changed = 1;


/*---------------------------------------------------------------------------*
 * uniqueptr: allocate a persistent name for the file                        *
 *---------------------------------------------------------------------------*/
char *uniqueptr(char *s)
{
	int i = 0;
	size_t j = strlen(s);

	while(i < filesused)
	{
		if(strncmp(persistentnames[i], s, j) == 0)
		{
			return(persistentnames[i]);
		}
		i++;
	}

	return((persistentnames[filesused++] = newstring(s)));
}

/*---------------------------------------------------------------------------*
 * af_open: open a file into the assembly chain                              *
 *---------------------------------------------------------------------------*/
void af_open(char *fn, FILE *f)
{
	struct asmfile *af;

	/* If a file currently open, close it */
	if(current)
	{
		current->pos = ftell(current_file);
		fclose(current_file);
		current_file = NULL;
	}

	// Generate a new asmfile
	af = new asmfile();

	/* Set all the bits of the struct */
	af->filename = uniqueptr(fn);
	af->line = 0;
	af->previous = current;
	af->pos = 0;

	/* Check whether we need to open the file */
	if(f)
	{
		current_file = f;
	}
	else
	{
		current_file = fopen(af->filename, "r");

		if(current_file == NULL)
			A6Err(Fatal, FileNotFound, fn);
	}

	current = af;

	return;
}

/*---------------------------------------------------------------------------*
 * af_close: close current file, return 0 if no more files left              *
 *---------------------------------------------------------------------------*/
int af_close(void)
{
	struct asmfile *z;

	if(current == NULL)
	{
		return(0);
	}
	else
	{
		fclose(current_file);
		current_file = NULL;
	}

	z = current;
	current = z->previous;
	free(z);

	return(current == NULL);
}

/*---------------------------------------------------------------------------*
 * af_reset: reset everything ready for pass 2                               *
 *---------------------------------------------------------------------------*/
void af_reset(void)
{
	/* Close all open files */
	while(af_close());

	/* Reset line counter */
	af_postotal = 0;
}

/*---------------------------------------------------------------------------*
 * af_name: return the name of the current file                              *
 *---------------------------------------------------------------------------*/
char *af_name(void)
{
	if(current != NULL)
		return(current->filename);
	else
		return(0);
}

/*---------------------------------------------------------------------------*
 * af_line: return the current line number                                   *
 *---------------------------------------------------------------------------*/
unsigned int af_line(void)
{
	if(current != NULL)
		return(current->line);
	else
		return(-1);
}

/*---------------------------------------------------------------------------*
 * af_pos: return the current absolute line number                           *
 *---------------------------------------------------------------------------*/
unsigned int af_pos(void)
{
	return(af_postotal);
}

/*---------------------------------------------------------------------------*
 * af_printforerror: print current line (for error messages)                 *
 *---------------------------------------------------------------------------*/
void af_printforerror(void)
{
	char buffer[256];

	if(line_changed)
		line_changed = 0;
	else
		return;

	if(current == NULL)
	{
		//0.5.1 -- disabled for Conditional::Clear between passes...
		//printf("internal fatal error: no file open, please email " AUTHOR_EMAIL "\n");
		//exit(EXIT_FAILURE);
		return;
	}

	if(fseek(current_file, lastpos, SEEK_SET))
	{
		printf("internal fatal error: fseek failed, please email " AUTHOR_EMAIL "\n");
		exit(EXIT_FAILURE);
	}

	fgets((char *)buffer, 255, current_file);

	printf("\n%s", buffer);
}

/*---------------------------------------------------------------------------*
 * af_cleanup: clean up prior to program exit                                *
 *---------------------------------------------------------------------------*/
void af_cleanup(void)
{
	while(af_close());

	while(filesused)
	{
		free(persistentnames[--filesused]);
	}
}


/*---------------------------------------------------------------------------*
 * af_parseline: get and parse a line from current assembly.  Return 0 when  *
 *               assembly finished.                                          *
 *---------------------------------------------------------------------------*/
int af_parseline(void)
{
	char *expr;
	char *label;
	char opcode[16];
	char *s;
	int opsize = 0;

	/* Open file if necessary */
	if(current_file == NULL)
	{
		if(current == NULL)
		{
			return(0);
		}
		else
		{
			current_file = fopen(current->filename, "r");
			if(fseek(current_file, current->pos, SEEK_SET))
			{
				A6Err(Fatal, InternalError, "fseek failed");
			}
		}
	}

	/* Grab the line */
	lastpos = ftell(current_file);
	if(!fgets((char *)currentline, 255, current_file))
	{
		af_close();
		return(1);
	}
	current->line++;

	/* List the line if list flag on */
	if((g_listflag != 0) && (g_pass > 0))
	{
		// TODO -- rework printing
		printf(" %4d %04x %s", af_line(), outf_getpc(), currentline);
		//fputs(currentline, stdout);
	}
	else
	{
		line_changed = 1;
	}

	/* Strip off comment */
	s = strchr(currentline, ';');
	if(s == NULL)
		s = strstr(currentline, "//");
	if(s != NULL)
		*s = '\0';

	/* Get the label */
	s = currentline;
	if(isalpha(*s) || *s == '_' || *s == '*')
	{
		label = s;
		s++;
		while(*s != '\0' && (isalnum(*s) || *s == '_' || *s == '.' || *s == '$'))
			s++;
		expr = s;
	}
	else
	{
		label = NULL;
		expr = NULL;
	}

	/* Strip white space */
	while(*s != '\0' && isspace(*s))
		s++;

	/* Grab opcode */
	if(*s == ':' || *s == '=')
	{
		while(*s != '\0' && (*s == ':' || *s == '='))
			opcode[opsize++] = *s++;
	}
	else
	{
		while(*s != '\0' && (*s == '.' || isalnum(*s)))
			opcode[opsize++] = *s++;
	}
	opcode[opsize] = '\0';

	/* Terminate label if needed */
	if(expr != NULL)
		*expr = '\0';

	/* Check for expression */
	while(*s != '\0' && isspace(*s))
		s++;
	if(*s != '\0')
	{
		expr = s;

		/* Trim whitespace from expression */
		while(isspace(*expr)) expr++;
		s = expr + strlen(expr) - 1;
		while(s > expr && isspace(*s))
			*s-- = '\0';
	}
	else
	{
		expr = NULL;
	}

#ifdef DEBUG
	printf("%32s:%8s:%s\n", label, opcode, expr);
#endif

	/* Now everything's sorted, can we assemble this? */
	if(Conditional::IsOn())
	{
		if(opcode[0] == '\0')
		{
			if(label != NULL)
				lbl_define(label, outf_getpc(), LBL_SYNCPC);
		}
		else
		{
			if(opcode[0] == '.')
			{
				psdir(opcode + 1, expr, label);
			}
			else
			{
				if(strcmp(opcode, ":=") == 0)
				{
					if(label != NULL)
						lbl_define(label, psexpr(expr), LBL_VARIABLE);
				}
				else if(strcmp(opcode, "=") == 0)
				{
					if(label != NULL)
						lbl_define(label, psexpr(expr), LBL_NONE);
				}
				else
				{
					parseopcode(opcode, expr);
				}
			}
		}
	}
	else
	{
		/* Only ever call the directive handler in non-asm situations */
		if(opcode[0] == '.')
		{
			psdir(opcode + 1, expr, label);
			/*
			                } else {
			                        if(g_dotflag != 0 && opcode[0] != '\0') {
			                                psdir(opcode + 1, expr, label);
			                        }
			*/
		}
	}

	return(1);
}
