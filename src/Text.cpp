///////////////////////////////////////////////////////////////////////////////
//
//	Text.cpp
//
//	$Id: Text.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Character set parser
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

#include "Error.h"
#include "OutputFactory.h"
#include "Text.h"

int cset;

unsigned char petscii[] =
{
	/*0x00*/ 0x20, 0x20, 0x20, 0xd3, 0xda, 0xd8, 0xc1, 0xd1,
	/*0x08*/ 0xd1, 0x77, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2a,
	/*0x10*/ 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0xaf, 0x20,
	/*0x18*/ 0x5e, 0x20, 0x20, 0x5f, 0x20, 0x20, 0x20, 0x20,
	/*0x20*/ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	/*0x28*/ 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	/*0x30*/ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	/*0x38*/ 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	/*0x40*/ 0x40, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	/*0x48*/ 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	/*0x50*/ 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	/*0x58*/ 0xd8, 0xd9, 0xda, 0x5b, 0x20, 0x5d, 0x5e, 0x5f,
	/*0x60*/ 0x20, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	/*0x68*/ 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	/*0x70*/ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	/*0x78*/ 0x58, 0x59, 0x5a, 0x27, 0xdd, 0x28, 0x20, 0x20,
	/*0x80*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x88*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x90*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x98*/ 0x20, 0x20, 0x20, 0x20, 0x5c, 0x20, 0x20, 0x20,
	/*0xa0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xa8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xb0*/ 0xa6, 0xa6, 0xa6, 0xdd, 0xb3, 0x20, 0x20, 0x20,
	/*0xb8*/ 0x20, 0xb3, 0xdd, 0xae, 0xbd, 0x20, 0x20, 0xae,
	/*0xc0*/ 0xad, 0xb1, 0xb2, 0xab, 0xc0, 0xdb, 0x20, 0x20,
	/*0xc8*/ 0xad, 0xb0, 0xb1, 0xb2, 0xab, 0x60, 0x7b, 0x20,
	/*0xd0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xd8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

unsigned char scrl[] =
{
	/*0x00*/ 0x00, 0x00, 0x00, 0x53, 0x5a, 0x58, 0x41, 0x51,
	/*0x08*/ 0xd1, 0x57, 0xd7, 0x20, 0x20, 0x20, 0x20, 0x2a,
	/*0x10*/ 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x79, 0x20,
	/*0x18*/ 0x1e, 0x20, 0x20, 0x1f, 0x20, 0x20, 0x20, 0x20,
	/*0x20*/ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	/*0x28*/ 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	/*0x30*/ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	/*0x38*/ 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	/*0x40*/ 0x00, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	/*0x48*/ 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	/*0x50*/ 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	/*0x58*/ 0x58, 0x59, 0x5a, 0x1b, 0x20, 0x1d, 0x1e, 0x64,
	/*0x60*/ 0x20, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	/*0x68*/ 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	/*0x70*/ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	/*0x78*/ 0x18, 0x19, 0x1a, 0x28, 0x42, 0x29, 0x20, 0x20,
	/*0x80*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x88*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x90*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x98*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xa0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xa8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xb0*/ 0x66, 0x66, 0x66, 0x5d, 0x73, 0x20, 0x20, 0x20,
	/*0xb8*/ 0x20, 0x73, 0x5d, 0x6e, 0x7d, 0x20, 0x20, 0x6e,
	/*0xc0*/ 0x6d, 0x71, 0x72, 0x6b, 0x40, 0x5b, 0x20, 0x20,
	/*0xc8*/ 0x6d, 0x70, 0x71, 0x72, 0x6b, 0x40, 0x5b, 0x20,
	/*0xd0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xd8*/ 0x20, 0x7d, 0x70, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

unsigned char scru[] =
{
	/*0x00*/ 0x00, 0x00, 0x00, 0x53, 0x5a, 0x58, 0x41, 0x51,
	/*0x08*/ 0xd1, 0x57, 0xd7, 0x20, 0x20, 0x20, 0x20, 0x2a,
	/*0x10*/ 0x20, 0x20, 0x20, 0x21, 0x20, 0x20, 0x79, 0x20,
	/*0x18*/ 0x1e, 0x20, 0x20, 0x1f, 0x20, 0x20, 0x20, 0x20,
	/*0x20*/ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	/*0x28*/ 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	/*0x30*/ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	/*0x38*/ 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	/*0x40*/ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	/*0x48*/ 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	/*0x50*/ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	/*0x58*/ 0x18, 0x19, 0x1a, 0x1b, 0x20, 0x1d, 0x1e, 0x64,
	/*0x60*/ 0x20, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	/*0x68*/ 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	/*0x70*/ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	/*0x78*/ 0x18, 0x19, 0x1a, 0x28, 0x42, 0x29, 0x20, 0x20,
	/*0x80*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x88*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x90*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0x98*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xa0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xa8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xb0*/ 0x66, 0x66, 0x66, 0x5d, 0x73, 0x20, 0x20, 0x20,
	/*0xb8*/ 0x20, 0x73, 0x5d, 0x6e, 0x7d, 0x20, 0x20, 0x6e,
	/*0xc0*/ 0x6d, 0x71, 0x72, 0x6b, 0x40, 0x5b, 0x20, 0x20,
	/*0xc8*/ 0x6d, 0x70, 0x71, 0x72, 0x6b, 0x40, 0x5b, 0x20,
	/*0xd0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xd8*/ 0x20, 0x7d, 0x70, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xe8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf0*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	/*0xf8*/ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

/* --------------------------------------------------------------- */
unsigned char pstext_convchar(unsigned char c)
{
	switch(cset)
	{
	case CSET_ASCII: return c;
	case CSET_PETSCII: return petscii[c & 0x7f];
	case CSET_SCRL: return scrl[c & 0x7f];
	case CSET_SCRU: return scru[c & 0x7f];
	}

	/* In case of unknown set */
	return(0x20);
}

char *parsetext(char *s, int mode)
{
	char quote = *s++;

	while(*s != '\0')
	{
#ifdef DEBUG
		printf("parsetext: *s=%c, s=%p\n", *s, s);
#endif

		if(*s == '\0')
		{
			A6Err(Pass1, UntermString);
			return(s);
		}

		if(*s == quote)
		{
			if(*(s + 1) == quote)
			{
				outf_wbyte(quote);
				s += 2;
			}
			else
				return(s + 1);
		}
		else
			if(mode == 2 && (s[1] == '\0' || (s[1] == quote && s[2] != quote)))
				outf_wbyte(pstext_convchar(*s++) | 0x80);
			else
				outf_wbyte(pstext_convchar(*s++));
#ifdef DEBUG
		printf("(endloop) parsetext: *s=%c, s=%p\n", *s, s);
#endif

	}

	if(mode == 1)
		outf_wbyte(0);

#ifdef DEBUG
	printf("(exit) parsetext: *s=%c, s=%p\n", *s, s);
#endif

	return(s);
}

int pstext_getcset(void)
{
	return(cset);
}

void pstext_setcset(int z)
{
	cset = z;
}
