///////////////////////////////////////////////////////////////////////////////
//
//	Expression.cpp
//
//	$Id: Expression.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Expression parser for A6
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
//  - proper recursive parser, BODMAS rules
//  - option to use l-to-r parsing instead (LG compatibility)
//  - add {} as synonym for []
//  - warning on first use of [] (remove at 0.6.0)


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "Error.h"
#include "Global.h"
#include "Label.h"
#include "OutputFactory.h"
#include "Text.h"

#define RETURNLO (1)
#define RETURNHI (2)

/*---------------------------------------------------------------------------*
 * psexpr : parse expression                                                 *
 *---------------------------------------------------------------------------*/
unsigned long psexpr(char *exp)
{
	unsigned long accum = 0, next = 0;
	int sign, operatr = '+';
	static char label[33];
	int label_size = 0, return_type = 0;
	char *errpos, *t, temp;
	struct label *templbl;

	/* Skip whitespace */
	while(isspace(*exp)) exp++;

	/* Check for low / high */
	if(*exp == '<')
	{
		return_type = RETURNLO;
		exp++;
	}
	else if(*exp == '>')
	{
		return_type = RETURNHI;
		exp++;
	}

	/* Skip whitespace */
	while(isspace(*exp)) exp++;

	/* Loop through expression */
	while(*exp != '\0')
	{
		sign = '+';
		next = 0;

		/* Skip whitespace */
		while(isspace(*exp)) exp++;
		errpos = exp;

		/* Get signs */
		if(*exp == '-' || *exp == '~' || *exp == '!')
			sign = *exp++;
		else
			sign = '\0';

		/* Get expression */
		switch(*exp)
		{
			/* TERNARY, OCTAL or HEX */
		case '@':
		case '0':
		case '$':
			/* HEX */
			if((*exp == '0' && exp[1] == 'x') || *exp == '$')
			{
				if(*exp == '0') exp++;
				exp++;
				while((*exp >= '0' && *exp <= '9')
				        || (*exp >= 'a' && *exp <= 'f')
				        || (*exp >= 'A' && *exp <= 'F'))
				{
					if(*exp >= '0' && *exp <= '9')
						next = (next << 4) | (*exp & 0x0f);
					else
						next = (next << 4) | (tolower(*exp) - 'a') + 10;
					exp++;
				}

			}
			else if(*exp == '0' && exp[1] == 't')
			{
				exp += 2;
				while((*exp >= '0' && *exp <= '9')
				        || (*exp == 't' || *exp == 'T' || *exp == 'e' || *exp == 'E'))
				{
					switch(*exp)
					{
					case 't':
					case 'T':
						next = (next * 12) + 10;
						break;
					case 'e':
					case 'E':
						next = (next * 12) + 11;
						break;
					default:
						next = (next * 12) + *exp - '0';
						break;
					}
					exp++;
				}

				/* OCTAL */
			}
			else
			{
				exp++;
				while(*exp >= '0' && *exp <= '7')
				{
					next = (next << 3) | (*exp & 7);
					exp++;
				}
			}

			break;

			/* BINARY */
		case '%':
			exp++;
			while(*exp == '0' || *exp == '1')
				next = (next << 1) | (*exp++ & 0x01);
			break;

			/* ISDEF? */
		case '[':
			t = ++exp;
			while(*exp != '\0' && *exp != ']')
				exp++;
			if(*exp == '\0')
				A6Err(Pass1, UntermString, errpos);
			else
			{
				*exp = '\0';
				next = (lbl_getptr(t, lbl_getscope()) != NULL);
				exp++;
			}
			break;

			/* BASE 10 */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			while(*exp >= '0' && *exp <= '9')
				next = (next * 10) + (*exp++ - '0');
			break;

			/* CHARACTER */
		case '\'':
		case '\"':
			temp = *exp++;
			next = pstext_convchar(*exp);
			if(*exp == temp)
			{
				exp++;
				if(*exp != temp)
				{
					A6Err(Pass1, UntermString, errpos);
					break;
				}
			}
			exp++;
			if(*exp != temp)
			{
				A6Err(Pass1, UntermString, errpos);
				break;
			}

			/* PC COUNTER */
		case '*':
			next = outf_getpc();
			exp++;
			break;

			/* LABEL */
		default:
			if(isalpha(*exp) || (*exp == '_'))
			{
				while(isalnum(*exp) || (*exp == '_') || (*exp == '.') || (*exp == '$'))
				{
					label[label_size++] = *exp++;
					if(label_size == 33)
					{
						A6Err(Pass1, LabelTooLong, errpos);
					}
				}

				if(label_size != 33)
				{
					label[label_size] = '\0';
					templbl = lbl_getptr(label, lbl_getscope());
					if(templbl == NULL)
					{
						if(g_pass)
						{
							A6Err(Pass2, UndefinedLabel, label);
						}
						next = 0xffff;
					}
					else
					{
						next = templbl -> value;
					}
				}
				else
				{
					next = 0xffff;
				}
				label_size = 0;

			}
			else
			{
				A6Err(Pass1, IllegalCharInExpression, exp);
				return(-1);
			}
		} /* Get expression */

		/* Perform sign */
		switch(sign)
		{
		case '-':
			next = -next;
			break;
		case '~':
			next = ~next;
			break;
		case '!':
			next = !next;
			break;
		}

		/* Perform operation */
		switch(operatr)
		{
		case '+':
			accum += next;
			break;
		case '-':
			accum -= next;
			break;
		case '*':
			accum *= next;
			break;
		case '?':
			accum ^= next;
			break;
		case '&':
			accum &= next;
			break;
		case -('&'):
			accum = accum && next;
			break;
		case -('?'):
			accum = accum ^ next ? 1 : 0;
			break;
		case '^':
		case '|':
			accum |= next;
			break;
		case -('^'):
		case -('|'):
			accum = accum || next;
			break;

		case '%':
			if(next == 0)
				A6Err(Warning, DivisionByZero, errpos);
			else
				accum %= next;
			break;

		case '/':
			if(next == 0)
				A6Err(Warning, DivisionByZero, errpos);
			else
				accum /= next;
			break;
		}

		/* Skip whitespace */
		while(isspace(*exp)) exp++;

		/* Get sign for next operation */
		switch(*exp)
		{
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
			operatr = *exp++;
			break;

		case '?':
		case '&':
		case '^':
		case '|':
			operatr = *exp++;
			if(operatr == *exp)
			{
				exp++;
				operatr = -operatr;
			}
			break;

		case '\0':
			break;

		default:
			A6Err(Pass1, IllegalCharInExpression, exp);
		}
	}

	switch(return_type)
	{
	case RETURNLO:
		return(accum & 0xff);
	case RETURNHI:
		return((accum >> 8) & 0xff);
	default:
		return(accum);
	}
}
