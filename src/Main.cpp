///////////////////////////////////////////////////////////////////////////////
//
//	Main.cpp
//
//	$Id: Main.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Main program, options, etc etc etc
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AsmFile.h"
#include "Conditional.h"
#include "Config.h"
#include "Cpu.h"
#include "Error.h"
#include "Global.h"
#include "Label.h"
#include "OutputFactory.h"
#include "A6String.h"
#include "Text.h"

/* OUTF_EXTENSION */
/* Must be here because GCC chokes on extern char** */
char *outf_extension[] =
{
	"obj", "prg", "p00"
};

/* Other module-level variables */
char *inname = NULL;
int symflag = 0;
int verbose = 0;

/*===========================================================================*
 * Useful routines                                                           *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 * extractfilename: extract file name from string                            *
 *   arg 0 : name of file.  *modified*                                       *
 *---------------------------------------------------------------------------*/
char *extractfilename(char *s)
{
	char *t;

	/* Strip leading spaces */
	while(isspace(*s)) s++;

	/* Strip quotes */
	t = NULL;
	switch(*s)
	{
	case '\'':
		t = strchr(s + 1, '\'');
		break;

	case '\"':
		t = strchr(s + 1, '\"');
		break;

	case '<':
		t = strchr(s + 1, '>');
		break;
	}

	if((*s == '\'' || *s == '\"' || *s == '<'))
	{
		if(t == NULL)
		{
			A6Err(Warning, UntermString, s);
		}
		else
		{
			*t = '\0';
			return (s + 1);
		}
	}

	return(s);
}

/*---------------------------------------------------------------------------*
 * licence : show preamble about the GPL                                     *
 *---------------------------------------------------------------------------*/
void licence(void)
{
	printf(
	    "a6 is free software: you can redistribute it and/or modify it under the\n"
	    "terms of the GNU General Public License as published by the Free Software\n"
	    "Foundation, either version 3 of the License, or (at your option) any later\n"
	    "version.\n\n"
	    "a6 is distributed in the hope that it will be useful, but WITHOUT ANY\n"
	    "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
	    "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n"
	    "details.\n"
	    "You should have received a copy of the GNU General Public License\n"
	    "along with a6.  If not, see <http://www.gnu.org/licenses/>.\n");
}

/*---------------------------------------------------------------------------*
 * usage : show program usage                                                *
 *---------------------------------------------------------------------------*/
void usage(void)
{
	printf("a6 " A6_VER_TEXT " (" A6_VER_NAME ") on " SYSNAME "\n\n"
	       "Usage:\n"
	       "    a6 [-dltv?] [-o output-file] [-fb|-fP|-f0] filename\n"
	       /*
	       		"      -d  --dots-optional : pseudo-ops can be invoked without leading dots\n"
	       */
	       "      -fb --format-bin    : output file in raw binary format\n"
	       "      -fp --format-prg    : output file in CBM loadable format\n"
	       "      -f0 --format-p00    : output file in PC64 P00 format\n"
	       "          --licence       : show information about the GPL\n"
	       "      -l  --list          : show program listing on pass 2\n"
	       "      -t  --list-symbols  : list symbol table\n"
	       "      -o  --output        : set output filename\n"
	       "      -v  --verbose       : print pass 1, pass 2 messages\n"
	       "      -?  --help          : show this message\n"
	      );
}

/*---------------------------------------------------------------------------*
 * getopts : get options                                                     *
 *---------------------------------------------------------------------------*/
void getopts(int argc, char **argv)
{
	int ignoreargs = 0;

	/* TODO: add in code for -D --a6-homedir */

	while(argc > 1)
	{
		if(argv[1][0] == '-')
		{
			if(ignoreargs == 0)
			{
				/*
								if(strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--dots-optional") == 0) {
									cpu_dotflag = 1;
								} else
				*/
				if(strcmp(argv[1], "-fb") == 0 || strcmp(argv[1], "--format-bin") == 0)
				{
					outf_format = OUTF_BIN;

				}
				else if(strcmp(argv[1], "-fp") == 0 || strcmp(argv[1], "--format-prg") == 0)
				{
					outf_format = OUTF_PRG;

				}
				else if(strcmp(argv[1], "-f0") == 0 || strcmp(argv[1], "--format-p00") == 0)
				{
					outf_format = OUTF_P00;

				}
				else if(strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "--list") == 0)
				{
					g_listflag = 1;

				}
				else if(strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--list-symbols") == 0)
				{
					symflag = 1;

				}
				else if(strcmp(argv[1], "-o") == 0 || strcmp(argv[1], "--output") == 0)
				{
					argv++;
					argc--;
					g_outname = argv[1];

				}
				else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--verbose") == 0)
				{
					verbose = 1;

				}
				else if(strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "--help") == 0)
				{
					usage();
					exit(EXIT_SUCCESS);

				}
				else if(strcmp(argv[1], "--licence") == 0 || strcmp(argv[1], "--license") == 0)
				{
					licence();
					exit(EXIT_SUCCESS);

				}
				else if(strcmp(argv[1], "--version") == 0)
				{
					printf("a6 " A6_VER_TEXT);
					exit(EXIT_SUCCESS);

				}
				else if(strcmp(argv[1], "-s0") == 0
				        || strcmp(argv[1], "-s1") == 0
				        || strcmp(argv[1], "-s2") == 0)
				{
					/* DO NOTHING!! */

				}
				else if(strcmp(argv[1], "--") == 0)
				{
					ignoreargs = 1;

				}
				else
				{
					printf("unknown option: %s\n", argv[1]);
					usage();
					exit(EXIT_FAILURE);

				}
			}
		}
		else
		{
			if(inname == NULL)
			{
				inname = argv[1];
			}
			else
			{
				printf("unknown option: %s\n", argv[1]);
				usage();
				exit(EXIT_FAILURE);
			}
		}

		argc--;
		argv++;
	}
}

/*===========================================================================*
 * MAIN                                                                      *
 *===========================================================================*/
int main(int argc, char **argv)
{
	/*	char *s;*/
	size_t i;
	int freeoutname = 0;
	/*	char *outname = NULL;*/
	FILE *infile = NULL;

	/* TODO -- add code to get a6_homedir */

	/* Defaults before command line */
	g_listout = stdout;

	getopts(argc, argv);

	/* If no input name, show usage and exit */
	if(inname == 0)
	{
		usage();
		exit(EXIT_FAILURE);
	}

	/* If no output name, generate it */
	if(g_outname == NULL)
	{
		g_outname = (char *)a6malloc(strlen(inname) + 5);
		freeoutname = 1;

		strcpy(g_outname, inname);

		i = strlen(g_outname);
		while(i > 0 && g_outname[i] != '.') i--;

		if(i == 0)
			i = strlen(g_outname);

		g_outname[i++] = '.';
		g_outname[i++] = outf_extension[g_outf_format][0];
		g_outname[i++] = outf_extension[g_outf_format][1];
		g_outname[i++] = outf_extension[g_outf_format][2];
		g_outname[i] = '\0';
	}

	/* Check that main file exists */
	infile = fopen(inname, "r");
	if(infile == NULL)
	{
		printf("file not found: '%s'\n", inname);
		exit(EXIT_FAILURE);
	}

	/* CALL ALL ASSEMBLY ROUTINES HERE */

	/* ------------------------------------------------------- */
	/* Pass 1 */
	if(verbose) printf("\npass 1\n\n");
	g_pass = 0;
	g_undocopsflag = 0;		/* Undoc. ops OFF by default */
	pstext_setcset(CSET_PETSCII);	/* Default = PET ASCII */
	lbl_init();			/* Set up label indexing */

	/* Actual pass */
	af_open(inname, infile);
	while(af_parseline());

	//----------------------------------------------------------------------------
	// Pass 2
	if(verbose) printf("\npass 2\n\n");
	g_pass++;

	// Reset conditional assembly stack
	Conditional::Clear();

	// Clear up asmfiles (This has to be the last cleanup!)
	af_reset();			/* Reset af_postotal */
	while(af_close() != 0);


	/* ------------------------------------------------------- */
	/* Pass 2 */
	g_undocopsflag = 0;		/* Undoc. ops OFF by default */
	lbl_reset();			/* Reset local labels */
	pstext_setcset(CSET_PETSCII);	/* Default = PET ASCII */

	outf_setpc(0);			/* Reset PC counter */

	outf_open(g_outname);		/* Open output file */

	/* Actual pass */
	af_open(inname, 0);
	while(af_parseline());

	
	//----------------------------------------------------------------------------
	// End of pass 2

	// Warning if conditional stack not empty
	if(Conditional::OpenItems() > 0)
	{
		A6Err(Warning, UnclosedCond);
	}

	/* Cleanup input files */
	while(af_close() != 0);

	/* List symbol table if required */
	if(symflag != 0)
	{
		if(verbose)
			printf("\n\n");
		lbl_dumpsym();
	}

	/* Tidy everything up */
	af_cleanup();		/* Clean up assembly files */
	lbl_destroy();		/* Delete all allocated labels */
	outf_close();

	if(freeoutname)
		free(g_outname);

	/* ------------------------------------------------------- */

	/* Final message */
	if(geterrcount() > 0)
	{
		printf("\n %u errors; assembly failed.\n", geterrcount());
		exit(EXIT_FAILURE);
	}

	if(verbose)
		printf("\n assembly successful; no errors.\n");
	return(EXIT_SUCCESS);
}
