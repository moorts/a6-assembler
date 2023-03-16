///////////////////////////////////////////////////////////////////////////////
//
//	Directive.cpp
//
//	$Id: Directive.cpp 46 2012-09-16 02:49:15Z simon.collis@gmail.com $
//
//	Directive parser.
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
//-----------------------------------------------------------------------------
//
//	0.5.0	Rewritten to use the std::map<> class rather than gperf.
//			"Flags" magic numbers replaced with more small functions,
//				usually calling handler functions with appropriate arguments.
//			Added 3 letter abbreviations in all cases where it wouldn't be
//				ambiguous.
//
//////////////////////////////////////////////////////////////////////////////

// TODO
//  - tidy the code!
//  - make .ver work
//  - finish checking and testing all directives

#include <map>
#include <string>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Main.h"
#include "AsmFile.h"
#include "Conditional.h"
#include "Config.h"
#include "Cpu.h"
#include "Error.h"
#include "Expression.h"
#include "Global.h"
#include "Label.h"
#include "OutputFactory.h"
#include "A6String.h"
#include "Text.h"

///////////////////////////////////////////////////////////////////////////////
//
//	.byte, .word, .dbyte, etc...
//	.text, .shift, .asc, .tscrl...
//
typedef enum StringTerm { None, Zero, Shift };

// stringTerm - 0 = none, 1 = OR 0x80, 2 = zero terminator
void numArgList(char *e, int size, int endian, StringTerm stringTerm)
{
	int quote, chr;
	unsigned long val;
	char *t;

	// Loop through the expressions
	while(e != NULL && *e)
	{
		/* Skip leading whitespace */
		while(isspace(*e)) e++;

		/* Text argument? */
		if(*e == '\'' || *e == '\"')
		{
			quote = *e++;

			/* Loop until end of text */
			while(quote && *e)
			{
				/* Check if current char is quote */
				if(*e == quote)
				{
					if(e[1] == quote)
					{
						e += 2;
						chr = quote;
					}
					else
					{
						quote = 0;
						chr = -1;
					}
				}
				else
				{
					chr = *e;
				}

				/* Next character */
				e++;

				/* Convert current */
				if(chr != -1)
				{
					chr = pstext_convchar(chr);

					// End of text for .shift
					if((stringTerm == Shift) && (*e == '\0' || (*e == quote && e[1] != quote)))
						chr |= 0x80;

					/* Output the character */
					outf_wbyte(chr);
				}
			}

			/* Output null char? */
			if(stringTerm == Zero)//(flags & 0x20)
			{
				// TODO -- get null character from current character set
				outf_wbyte(0);
			}

			/* Skip whitespace after string */
			while(isspace(*e)) e++;

			/* Skip comma */
			while(*e == ',') e++;

			/* Expression argument */
		}
		else
		{
			/* Find next argument */
			t = strchr(e, ',');
			if(t != NULL)
				*t++ = '\0';

			/* Get the value */
			val = psexpr(e);

			/* Warn if value too large (on second pass) */
			if(g_pass != 0)
			{
				if(*e == '-' || *e == '~')
				{
					if((size == 0 && val > 0x00007f && val < 0xffffff80) ||
					        (size == 1 && val > 0x007fff && val < 0xffff8000) ||
					        (size == 2 && val > 0x7fffff && val < 0xff800000))
					{
						//printf("%u %u %s",size,val,e);
						A6Err(Warning, ArgTooBig, e);	//, Warning);
					}
				}
				else
				{
					if((size == 0 && val > 0x0000ff) ||
					        (size == 1 && val > 0x00ffff) ||
					        (size == 2 && val > 0xffffff))
					{
						//printf("%u %u %s", size, val, e);
						A6Err(Warning, ArgTooBig, e);	//, Warning);
					}
				}
			}

			/* Write the data */
			outf_wdata(val, size, endian);

			/* Next expression */
			e = t;
		}
	}
}

// Current CPU endian (TODO when CPUs can be loaded...)
void d_byte(char *e, char *t)
{
	numArgList(e, 8, Little, None);
}
void d_long(char *e, char *t)
{
	numArgList(e, 32, Little, None);
}
void d_short(char *e, char *t)
{
	numArgList(e, 16, Little, None);
}

// Specific big-endian
void d_belong(char *e, char *t)
{
	numArgList(e, 32, Big,	None);
}
void d_beshort(char *e, char *t)
{
	numArgList(e, 16, Big,	None);
}

// Specific little-endian
void d_lelong(char *e, char *t)
{
	numArgList(e, 32, Little, None);
}
void d_leshort(char *e, char *t)
{
	numArgList(e, 16, Little, None);
}

// Text
void d_null(char *e, char *t)
{
	numArgList(e,  8, Little, Zero);
}
void d_shift(char *e, char *t)
{
	numArgList(e,  8, Little, Shift);
}

// Text using specified charsets
void dottasc(char *e, char *t)
{
	int i = pstext_getcset();
	pstext_setcset(CSET_ASCII);
	numArgList(e, 8, Little, Zero);
	pstext_setcset(i);
}

void dottpet(char *e, char *t)
{
	int i = pstext_getcset();
	pstext_setcset(CSET_PETSCII);
	numArgList(e, 8, Little, Zero);
	pstext_setcset(i);
}

void dottscrl(char *e, char *t)
{
	int i = pstext_getcset();
	pstext_setcset(CSET_SCRL);
	numArgList(e, 8, Little, Zero);
	pstext_setcset(i);
}

void dottscru(char *e, char *t)
{
	int i = pstext_getcset();
	pstext_setcset(CSET_SCRU);
	numArgList(e, 8, Little, Zero);
	pstext_setcset(i);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Conditional assembly
//

// .if and all its friends...
void d_if(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee != 0);
}
void d_ifequ(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee == 0);
}
void d_ifeven(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter((ee & 1) == 0);
}
void d_ifmi(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee < 0);
}
void d_ifneg(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee < 0);
}
void d_ifneq(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee != 0);
}
void d_ifodd(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter((ee & 1) == 1);
}
void d_ifpl(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee > 0);
}
void d_ifpos(char *e, char *t)
{
	int ee = psexpr(e);
	Conditional::Enter(ee >= 0);
}

// .else
void d_else(char *e, char *t)
{
	Conditional::Else();
}

// .endif (aka .ifend)
void d_endif(char *e, char *t)
{
	Conditional::Exit();
}

///////////////////////////////////////////////////////////////////////////////
//
//	Binary includes
//

// Generic handler
void incbin(char *name, int skipbytes)
{
	char *s = extractfilename(name);
	if(s == NULL)
	{
		// TODO -- new error message
		A6Err(Pass2, FileNotFound);
		return;
	}

	// Open the file
	FILE *infile = fopen(s, "rb");
	if(infile == NULL)
	{
		A6Err(Pass2, FileNotFound, s); //,Pass2);
		return;
	}

	// Skip file header
	while((skipbytes > 0) && (!feof(infile)))
	{
		getc(infile);
		--skipbytes;
	}

	// Copy file to output
	if(!feof(infile))
	{
		int c = getc(infile);
		while(!feof(infile))
		{
			outf_wbyte(c);
			c = getc(infile);
		}
	}

	/* Close input file */
	fclose(infile);
}

// Handler functions for SortedList
void d_incbin(char *e, char *t)
{
	incbin(e, 0x00);
}
void d_incp00(char *e, char *t)
{
	incbin(e, 0x1a);
}
void d_incprg(char *e, char *t)
{
	incbin(e, 0x02);
}

///////////////////////////////////////////////////////////////////////////////
//
//	Handle source file inclusions
//	.include, .file, .lib...
//
void includehandler(char *expr, char *t, int transfer)
{
	FILE *f = NULL;
	char *s = trim(expr);
	char *u, *p;
	size_t scancurrent = 1;

	t = NULL;

	/* strip single quote */
	if(*s == '\'')
	{
		t = strchr(s + 1, '\'');
		if(t == NULL)
		{
			A6Err(Pass1, UntermString, s);
			return;
		}

	}
	else if(*s == '\"')
	{
		t = strchr(s + 1, '\"');
		if(t == NULL)
		{
			A6Err(Pass1, UntermString, s);
			return;
		}

	}
	else if(*s == '<')
	{
		t = strchr(s + 1, '>');
		if(t == 0)
		{
			A6Err(Pass1, UntermString, s);
			return;
		}
		scancurrent = 0;
	}

	if(t)
	{
		*t = 0;
		s++;
	}

	if(scancurrent)
	{
		f = fopen(s, "r");
	}

	if(f != NULL)
	{
		af_open(s, f);
		return;
	}

	t = getenv("A6_INCPATH");

	p = (char *)malloc(256 * sizeof(char));

	/* Close current file if this is a .file */
	if(transfer)
		af_close();

	/* Find the file */
	while(t)
	{
		/* Find where current section of path ends */
		u = strchr(t, PATH_SEPARATOR);

		/* Delete separator */
		if(u)
			*u++ = 0;

		/* Build new filename */
		scancurrent = strlen(t);

		if(t[scancurrent - 1] == PATH_SLASH)
			sprintf(p, "%s%s", t, s);
		else
			sprintf(p, "%s%c%s", t, PATH_SLASH, s);

		/* Find it */
		if((f = fopen(p, "r")))
		{
			af_open(p, f);
			free(p);
			return;
		}

		/* Next path */
		/*		if(u)*/
		t = u;
	}

	free(p);

	A6Err(Pass2, FileNotFound, s);
}

void d_file(char *e, char *t)
{
	includehandler(e, t, true);
}
void d_include(char *e, char *t)
{
	includehandler(e, t, false);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/*---------------------------------------------------------------------------*
 * d_cpu: load a CPU definition file                                         *
 *---------------------------------------------------------------------------*/
void d_cpu(char *expr, char *t)
{
	// Does nothing at present...
	//cpu_load(expr);
	if(g_pass > 0)
	{
		printf("Note: .cpu doesn't do anything (yet)");
	}
}

/*---------------------------------------------------------------------------*
 * d_block: handle .block, .pad                                              *
 *---------------------------------------------------------------------------*/
void d_block(char *e, char *t)
{
	unsigned long val = psexpr(e);

	val = val & 0xff;

	while(val)
	{
		outf_wbyte(cpu_getnop());
		val--;
	}
}

/*---------------------------------------------------------------------------*
 * d_echo: parrot to screen                                                  *
 *---------------------------------------------------------------------------*/
void d_echo(char *e, char *t)
{
	if(g_pass != 0)
		printf("%s\n", e);
}

/*---------------------------------------------------------------------------*
 * d_equ: synonym for = to define label                                      *
 *---------------------------------------------------------------------------*/
void d_equ(char *expr, char *lbl)
{
	lbl_define(lbl, psexpr(expr), LBL_NONE);
}


/*-----------------------------------------------------------------*/
/* Charset */
void dotcset(char *e, char *t)
{
	if(!strcmp(e, "ascii") || !strcmp(e, "host")) pstext_setcset(CSET_ASCII);
	else if(!strcmp(e, "scru")) pstext_setcset(CSET_SCRU);
	else if(!strcmp(e, "scrl")) pstext_setcset(CSET_SCRL);
	else if(!strcmp(e, "pet")) pstext_setcset(CSET_PETSCII);
	else A6Err(Pass2, UnknownCharset, e);
}

/*-----------------------------------------------------------------*/
/* Local scopes */
void dotendloc(char *expr, char *t)
{
	lbl_exitlocal();
}

void dotlocal(char *expr, char *t)
{
	lbl_enterlocal();
}

/*-----------------------------------------------------------------*/
/* Org */
void dotorg(char *e, char *t)
{
	if(outf_getpc() != 0)
		A6Err(Pass2, SecondDotOrg);
	else
	{
		outf_setpc(psexpr(e));
	}
}

/*-----------------------------------------------------------------*/
/* Align */
void dotalign(char *e, char *t)
{
	unsigned int alignto = psexpr(e) & 0xff;

	if(alignto == 0) alignto = 256;

	while(outf_getpc() & alignto)
		outf_wbyte(0xea);
}

/*-----------------------------------------------------------------*/
/* Ver */

/* This has changed significantly for 0.4.4 (Caleb) and beyond.
   Main version is taken from global.h */
void dotver(char *e, char *t)
{
	if(g_pass > 0)
	{
		printf("Note: .ver doesn't work yet, but thanks anyway!");
	}

	// TODO -- rewrite this altogether
//	int fail = 0;
//	unsigned int ver = psexpr(e);
//
//	/* Fix for documentation supplied with 0.4.3 */
//	if(ver == 0x403)
//		ver = 43;
//
//	/* Check requested version later than this one */
//	if(ver > G_VER)
//	{
//		printf("A6 version %u.%u.%u is recommended to assemble this source!\n", ver / 100, (ver / 10) % 10, ver % 10);
//
//		if(ver > G_FAILVER)
//			printf("Fatal error:  Later version of A6 requested.\n\n"
//			       "Go to http://www.asix.sourceforge.net/ to download the latest version.\n");
//	}
//
//	/* Check for <0.5.0 */
//	if(ver < 50)
//	{
//		printf("This is a pre-0.5.0 source; you'll need to change the .ver and add\n"
//		       "        .cpu 6510\n"
//		       "at the top of the file in order to compile it.\n");
//		fail++;
//	}
//
//	if(fail)
//		exit(EXIT_FAILURE);
}

/*-----------------------------------------------------------------*/
/* .ADD, .EOR */
void dotadd(char *e, char *t)
{
	if(g_pass)
		g_outf_add = psexpr(e);
}

void doteor(char *e, char *t)
{
	if(g_pass)
		g_outf_eor = psexpr(e);
}

/*-----------------------------------------------------------------*/
/* Undocumented opcodes on and off */
void dotundoc(char *e, char *t)
{
	g_undocopsflag = 1;
}

void dotnoundoc(char *e, char *t)
{
	g_undocopsflag = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//	Directive list
//
//	Build the sorted list
//

// Directive functions - expression text, defined label
typedef void (*PFC)(char *, char *);

// Structure to hold each directive
struct Directive
{
	PFC Function;
	int NoAutoDefine;
	int ConditionalMode;
}; // Directive;

// List of directives
static std::map<std::string, Directive *, std::less<std::string> > qqDirTable;
static int isDirTableBuilt = 0;

void AddDirTLA(char *text, PFC function, int noAutoDefine, int conditionalMode)
{
	Directive *dir			= new Directive;
	dir->Function			= function;
	dir->NoAutoDefine		= noAutoDefine;
	dir->ConditionalMode	= conditionalMode;
	qqDirTable[text] = dir;

	char *buf				= new char[4];
	buf[0]					= text[0];
	buf[1]					= text[1];
	buf[2]					= text[2];
	buf[3]					= '\0';
	qqDirTable[buf] = dir;

	delete buf;
}

void AddDir(char *text, PFC function, int noAutoDefine, int conditionalMode)
{
	Directive *dir			= new Directive;
	dir->Function			= function;
	dir->NoAutoDefine		= noAutoDefine;
	dir->ConditionalMode	= conditionalMode;
	qqDirTable[text] = dir;
}

// Build directory table
void BuildDirTable()
{
	// Data directives
	AddDirTLA	("belong",		d_belong,	0,	0);
	AddDirTLA	("beshort",		d_beshort,	0,	0);
	AddDir		("by",			d_byte,		0,	0);
	AddDirTLA	("byte",		d_byte,		0,	0);
	AddDirTLA	("dbyte",		d_leshort,	0,	0);
	AddDirTLA	("lelong",		d_lelong,	0,	0);
	AddDirTLA	("leshort",		d_leshort,	0,	0);
	AddDirTLA	("long",		d_long,		0,	0);
	AddDir		("wd",			d_short,	0,	0);
	AddDirTLA	("word",		d_short,	0,	0);

	// Text directives
	AddDirTLA	("null",		d_null,		0,	0);
	AddDirTLA	("shift",		d_shift,	0,	0);
	AddDirTLA	("tasc",		dottasc,	0,	0);
	AddDirTLA	("text",		d_byte,		0,	0);
	AddDirTLA	("tpet",		dottpet,	0,	0);
	AddDir		("tscrl",		dottscrl,	0,	0);
	AddDir		("tscru",		dottscru,	0,	0);
	AddDir		("txt",			d_byte,		0,	0);

	// Conditional assembly
	AddDirTLA	("else",		d_else,		0,	2);
	AddDir		("endif",		d_endif,	0,	2);
	AddDir		("if",			d_if,		0,	1);
	AddDir		("ifend",		d_endif,	0,	2);
	AddDir		("ifequ",		d_ifequ,	0,	1);
	AddDir		("ifeq",		d_ifequ,	0,	1);
	AddDir		("ifeven",		d_ifeven,	0,	1);
	AddDirTLA	("ifmi",		d_ifmi,		0,	1);
	AddDir		("ifneg",		d_ifneg,	0,	1);
	AddDir		("ifneq",		d_ifneq,	0,	1);
	AddDirTLA	("ifodd",		d_ifodd,	0,	1);
	AddDir		("ifpl",		d_ifpl,		0,	1);
	AddDir		("ifpos",		d_ifpos,	0,	1);

	// Others
	AddDir		("add",			dotadd,		0,	0);
	AddDirTLA	("align",		dotalign,	0,	0);
	AddDirTLA	("block",		d_block,	0,	0);
	AddDir		("cpu",			d_cpu,		0,	0);
	AddDirTLA	("cset",		dotcset,	0,	0);
	AddDirTLA	("echo",		d_echo,		0,	0);
	AddDirTLA	("print",		d_echo,		0,	0);
	AddDir		("endloc",		dotendloc,	0,	0);
	AddDir		("eor",			doteor,		0,	0);
	AddDir		("equ",			d_equ,		1,	0);
	AddDirTLA	("file",		d_include,	0,	0);
	AddDir		("incbin",		d_incbin,	0,	1);
	AddDirTLA	("include",		d_include,	0,	0);
	AddDir		("incp00",		d_incbin,	0,	0);
	AddDir		("incprg",		d_incbin,	0,	0);
	AddDir		("lib",			d_include,	0,	0);
	AddDirTLA	("local",		dotlocal,	0,	0);
	AddDirTLA	("noundoc",		dotnoundoc,	0,	0);
	AddDir		("org",			dotorg,		0,	0);
	AddDir		("pad",			d_block,	0,	0);
	AddDirTLA	("undoc",		dotundoc,	0,	0);
	AddDir		("ver",			dotver,		0,	0);

	// Mark table built
	isDirTableBuilt = 1;
}


/* --------------------------------------------------------------- */
/* Parse directive                                                 */
void psdir(char *popn, char *expr, char *labl)
{
	if(isDirTableBuilt == 0)
	{
		BuildDirTable();
	}

	// Convert to lower case (makes directives non-case-sensitive)
	char *buf = new char[strlen(popn) + 1];
	for(unsigned int i = 0; i < strlen(popn); i++)
	{
		buf[i] = tolower(popn[i]);
	}
	buf[strlen(popn)] = '\0';

	Directive *pop = qqDirTable[buf];	//in_word_set(popn, strlen(popn));

	// Error if not found
	if(pop == NULL)
	{
		// Define label, then error and return
		if(labl != NULL)
		{
			lbl_define(labl, outf_getpc(), LBL_SYNCPC);
		}
		A6Err(Pass2, UnknownDirective);
		return;
	}

	// Are we assembling?
	if(Conditional::IsOn())
	{
		// Yes -- auto define the label
		if((!pop->NoAutoDefine) && (labl != NULL))
		{
			lbl_define(labl, outf_getpc(), LBL_SYNCPC);
		}

		// Call function
		pop->Function(expr, labl);
	}
	else
	{
		// Not defining - only call .else or .endif
		switch(pop->ConditionalMode)
		{
			// .if* -- Enter conditional
			case 1:
				Conditional::Enter(0);
				break;

			// .else or .end -- Call function
			case 2:
				pop->Function(expr, labl);
				break;
		}
	}
}
