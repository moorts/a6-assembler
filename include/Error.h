///////////////////////////////////////////////////////////////////////////////
//
//	Error.h
//
//	$Id: Error.h 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
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

#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>

// Error types
typedef enum
{
	Fatal,	  // Fatal error - bail out right now
	Warning,	// Warning - always warn user
	Pass1,	  // Error that should
	Pass2
} ErrorType;

typedef enum
{
	InternalError = 0,
	FileNotFound = 1,
	UntermString = 2,
	UnknownDirective = 3,
	//DirectiveNotAvailable = 4,
	WhiteSpace = 5,
	SecondDotOrg = 6,
	ArgTooBig = 7,
	UnknownCharset = 8,
	UnknownOpcode = 9,
	InvalidAdrsMode = 10,
	IllegalLabelStart = 11,
	DivisionByZero = 12,
	PcWrapped = 13,
	LabelIsntVariable = 14,
	CantCloseFile = 15,
	ReservedLabel = 16,
	NestingTooDeep = 17,
	IllegalCharInExpression = 18,
	UndefinedLabel = 19,
	StackEmpty = 20,
	LabelTooLong = 21,
	TooManyFiles = 22,
	AdrsModeNotAvailable = 23,
	NotACpuFile = 24,
	UnclosedCond = 25,
	OutOfMemory = 26,
	BadEndian = 27,
	//AdrsModeMax = 28,
	DuplicateName = 29
} ErrorCode;

void A6Err(ErrorType, ErrorCode);
void A6Err(ErrorType, ErrorCode, char *);

//void A6Err(ErrorCode, char *, int);
int geterrcount(void);

#endif
