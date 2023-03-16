///////////////////////////////////////////////////////////////////////////////
//
//	String.cpp
//
//	$Id: String.cpp 46 2012-09-16 02:49:15Z simon.collis@gmail.com $
//
//	Various string functions.
//	Eventually this might become a proper class.
//	Don't hold your breath.
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
#include <stdlib.h>
#include <string.h>

#include "Main.h"
#include "A6String.h"

//-----------------------------------------------------------------------------
// Constructor / destructor
String::String(char *text)
{
	if(text == NULL)
	{
		this->txt = NULL;
	}
	else
	{
		this->txt = new char[strlen(text) + 1];
		strncpy(this->txt, text, strlen(text));
	}
}

String::~String()
{
	if(this->txt != NULL)
	{
		delete this->txt;
	}
}


//-----------------------------------------------------------------------------
// Old stuff
/* NEWSTRING -- return same string in a new place in memory */
char *newstring(char *s)
{
	char *ns;
	size_t i = strlen(s);

	if(i == 0) return(0);

	ns = new char [i + 1];  //a6_malloc(++i);
	strcpy(ns, s);

	return(ns);
}


/* RTRIM -- note this function messes with the original data.  However,
   it's still safe to use on things you are going to need to free... */
char *rtrim(char *s)
{
	char *t;

	t = s + strlen(s);
	t--;

	while(isspace(*t)) t--;

	t++;

	if(isspace(*t)) *t = '\0';

	return(s);
}

/* STRIPQUOTE -- strips leading and trailing quotes, if found.
   *Not safe for stuff that needs freeing* */
char *stripquote(char *s)
{
	size_t i;

	if(s[0] == '\'' || s[0] == '\"')
	{
		i = strlen(s);
		while(i && s[i] != s[0]) i--;
		if(i)
			s[i] = 0;
		s++;
	}

	return s;
}

/* TRIM -- note this function returns a slightly `moved' pointer, and
   messes with the original data.  Don't use it on things you're going
   to need to free... */
char *trim(char *s)
{
	// Safety check...
	if(s == NULL) return NULL;

	char *t;

	while(isspace(*s)) s++;

	t = s + strlen(s);
	t--;
	while(isspace(*t)) t--;

	t++;

	if(isspace(*t)) *t = '\0';

	return(s);
}
