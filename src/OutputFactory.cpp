///////////////////////////////////////////////////////////////////////////////
//
//	OutputFactory.cpp
//
//	$Id: OutputFactory.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Dumping ground for the old binary output file formats.
//	Will eventually be the interface class and factory class for output
//	formats.
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

#include "Error.h"
#include "Global.h"
#include "OutputFactory.h"
#include "A6String.h"

int outf_format = OUTF_BIN;
FILE *outfile;

/* Figure out how to splice in 32-bit range check later */
unsigned long outf_pccount;
#define PC_WRAP (0x10000)

/*---------------------------------------------------------------------------*
 * outf_wdata : write any size/endian data to output file                    *
 *---------------------------------------------------------------------------*/
void outf_wdata(unsigned long data, int size, int endian)
{
	// Temporary hack
	if(size == 8) size = 0;
	if(size == 16) size = 1;
	if(size == 32) size = 3;

	switch(endian)
	{
	case ENDIAN_BIG:
		if(size == 3) outf_wbyte((data & 0xff000000) >> 24);
		if(size > 1) outf_wbyte((data & 0x00ff0000) >> 16);
		if(size > 0) outf_wbyte((data & 0x0000ff00) >> 8);
		outf_wbyte(data & 0x000000ff);
		break;

	case ENDIAN_PDP:
		/* Warning: GUESSWORK!!! */
		if(size > 1) outf_wbyte((data & 0x00ff0000) >> 16);
		if(size == 3) outf_wbyte((data & 0xff000000) >> 24);
		outf_wbyte(data & 0x000000ff);
		if(size > 0) outf_wbyte((data & 0x0000ff00) >> 8);

		/* Default is little-endian (6502, 8086, Z80...) */
	default:
		outf_wbyte(data & 0x000000ff);
		if(size > 0) outf_wbyte((data & 0x0000ff00) >> 8);
		if(size > 1) outf_wbyte((data & 0x00ff0000) >> 16);
		if(size == 3) outf_wbyte((data & 0xff000000) >> 24);
		break;
	}
}


void outf_wbyte(unsigned int b)
{
	static int haveoutput = 0;
	unsigned int c = (b + g_outf_add) ^ g_outf_eor;

#ifdef DEBUG
	printf("outf_wbyte: %u $%x\n", outf_pccount, b);
	fflush(stdout);
#endif

	if(g_pass)      /* Only actual output on pass 2!!! */
	{
		if(haveoutput && outf_pccount == PC_WRAP)
		{
			A6Err(Fatal, PcWrapped);
		}
		putc((unsigned char)(c & 0xff), outfile);
		haveoutput = 1;
	}
	++outf_pccount;

}

void outf_wword(unsigned int w)
{
	outf_wbyte(w & 0xff);
	outf_wbyte(w >> 8);
}

void outf_open(char *filename)
{
	outfile = fopen(filename, "wb");

	if(outfile == NULL)
	{
		A6Err(Fatal, FileNotFound, filename);
	}
}

void outf_close(void)
{
	if(fclose(outfile))
	{
		A6Err(Fatal, CantCloseFile);
	}
}

void outf_header(void)
{
	char buffer[16];		/* For C64 formats */
	int i = 0, mode = 0;

	if(!g_pass) return;

	/* Generate (simple) C64-style filename */
	while(i < 16)
	{
		if(mode)
			buffer[i++] = '\0';
		else
		{
			if(g_outname[i] == '.' || g_outname[i] == 0)
				mode++;
			else
			{
				buffer[i] = toupper(g_outname[i]);
				i++;
			}
		}
	}

	/* Output the header */
	switch(g_outf_format)
	{
	case OUTF_P00:
		fprintf(outfile, "C64file");	/* Header (7) */
		putc(0, outfile);	/* $0008 */
		for(i = 0; i < 16; i++)	/* Filename (16) */
			putc(buffer[i], outfile);
		putc(0, outfile);	/* $0018 */
		putc(0, outfile);	/* $0019 */
		/* Now data -- drop through for file start */

	case OUTF_PRG:
		putc((outf_pccount & 0xff), outfile);
		putc((outf_pccount >> 8), outfile);
		break;
	}
}

void outf_setpc(unsigned int newpc)
{
	if(outf_pccount)    /* Set before, no header needed */
	{
		outf_pccount = newpc;
	}
	else      /* First time set, write header */
	{
		outf_pccount = newpc;
#ifdef DEBUG
		printf("outf_setpc: newpc=%u,*=%u\n", newpc, outf_pccount);
#endif
		outf_header();
	}
}

unsigned int outf_getpc(void)
{
	return(outf_pccount);
}
