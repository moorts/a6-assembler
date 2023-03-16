///////////////////////////////////////////////////////////////////////////////
//
//	Label.cpp
//
//	$Id: Label.cpp 46 2012-09-16 02:49:15Z simon.collis@gmail.com $
//
//	Label and symbol table handler.
//	Label will stay here, new SymbolTable class 
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Main.h"
#include "AsmFile.h"
#include "Config.h"
#include "Error.h"
#include "Global.h"
#include "Label.h"
#include "OutputFactory.h"
#include "A6String.h"

#define MAXLOCALENEST (32)

/*---------------------------------------------------------------------------*
 * Label table, hash table, etc                                              *
 *---------------------------------------------------------------------------*/

/* Labels */
struct label *lbl_hash[256];
unsigned int lbl_hash_count[256];

/* Locales */
unsigned int scope_next=0;
unsigned int scope_now=0;
unsigned int scope_stk[MAXLOCALENEST];
unsigned int scope_stk_pos=0;

/*---------------------------------------------------------------------------*
 * makehash : main hashing function                                          *
 *---------------------------------------------------------------------------*/
#ifdef __GNUC
inline
#endif
int makehash(char *s, int scope)
{
	register size_t len = strlen(s);
	register size_t i = s[0] + scope + s[len-1] + len;
	if(len > 1) i += s[1];
	return(i & 0xff);
}

/*---------------------------------------------------------------------------*
 * lbl_init : setup for pass 1                                               *
 *---------------------------------------------------------------------------*/
void lbl_init(void)
{
        register int i;

        for(i=0; i<256; i++) {
                lbl_hash[i]=NULL;
                lbl_hash_count[i]=0;
        }
}

/*---------------------------------------------------------------------------*
 * lbl_reset : reset for pass 2                                              *
 *---------------------------------------------------------------------------*/
void lbl_reset(void)
{
        scope_next=0;
        scope_now=0;
        scope_stk_pos=0;
}

/*---------------------------------------------------------------------------*
 * lbl_destroy : free all memory                                             *
 *---------------------------------------------------------------------------*/
void lbl_destroy(void)
{
        register int i;
        register struct label *l, *k;

        for(i=0; i<256; i++) {
                if(lbl_hash[i]!=NULL) {
                        l=lbl_hash[i]->next;
                        free(lbl_hash[i]->name);
                        free(lbl_hash[i]);
                        while(l) {
                                free(l->name);
                                k=l->next;
                                free(l);
                                l=k;
                        }
                }
        }
}

/*----------------------------------------------------------------------------*
 * labelcmp : used internally by lbl_define and lbl_getptr                    *
 *----------------------------------------------------------------------------*/
#ifdef __GNUC
inline
#endif
int labelcmp(const char *name, const int scope, const register struct label *labl)
{
        register int i;

        i=strcmp(name, labl->name);
        if(i==0)
                i=scope - labl->scope;

        return(i);
}

/*----------------------------------------------------------------------------*
 * lbl_getptr : return pointer to a label structure                           *
 *----------------------------------------------------------------------------*/
/*@null@*/struct label *lbl_getptr(char *name, unsigned int scope)
{
        register int hash=makehash(name, scope);
        register struct label *temp;
        int upper, lower, mid, pos, i;

        /* Global? */
        if(*name == ':' && name[1] == ':') {
                name += 2;
                scope = 0;
        }

        /* Binary search for label */
        lower = 0;
        upper = lbl_hash_count[hash] - 1;
        pos = 0;
        temp = lbl_hash[hash];

        /* Find label */
        while(upper >= lower) {
                mid = (lower + upper) / 2;
                if(mid < pos) {
                        pos = 0;
                        temp = lbl_hash[hash];
                }
                while(pos < mid) {
                        temp = temp->next;
                        pos++;
                }
                i = labelcmp(name, scope, temp);
                if(i > 0) {
                        /* name is > temp->name */
                        lower = mid + 1;
                } else if(i < 0) {
                        /* name is < temp->name */
                        upper = mid - 1;
                } else if(i == 0) {
                        return(temp);
                }
        }

        /* If local not found, return global */
        if(scope!=0)
                return(lbl_getptr(name,0));
        else
                return(NULL);
}

/*----------------------------------------------------------------------------*
 * lbl_define : add a label to the symbol table                               *
 *----------------------------------------------------------------------------*/
void lbl_define(char *name, unsigned long val, unsigned int lbltype)
{
        register int hash;
        register struct label *temp;
        register struct label *oldtemp = NULL;
        int scope = scope_now;
        int i;

		// Set counter?
		if (name[0] == '*' && name[1] == '\0')
		{
			// There has to be a better way to do it than this!!!
			if(outf_getpc() == 0)
			{
				outf_setpc(val);
			}
			else
			{
				if(outf_getpc() > val)
				{
					A6Err(Pass2, PcWrapped);
				}
				else
				{
					while(outf_getpc() < val)
					{
						// TODO -- skip in hex files, get NOP from CPU
						outf_wbyte(0xea);
					}
				}
			}
			return;
		}

        /* Global? */
        if(*name == ':' && name[1] == ':') scope=0;

        /* Make hash and get head of table */
        hash = makehash(name, scope);
        temp = lbl_hash[hash];

        /* Insert as only entry? */
        if(temp == NULL) {
                oldtemp = (struct label *)malloc(sizeof(struct label)); //TODO: malloc(sizeof(struct label));;
                oldtemp->name = newstring(name);
                oldtemp->filename = af_name();
                oldtemp->value = val;
                oldtemp->line = af_line();
                oldtemp->scope = scope;
                oldtemp->type = lbltype;
                oldtemp->af_pos = af_pos();
                oldtemp->next = temp;
                lbl_hash[hash] = oldtemp;
                lbl_hash_count[hash]++;
                return;
        }

        /* Find the position in the list */
        while(1) {
                i = labelcmp(name, scope, temp);

                /* Label redefinition? */
                if(i == 0) {
                        /* Automatically (and silently) redefine variables */
                        if(temp->type == LBL_VARIABLE) {
                                temp->value = val;
                                return;

                        /* Not a variable */
                        } else {
                                /* Same label, same place (i.e. second pass?) */
                                if(temp->af_pos == af_pos()) {
                                        /* Redefine in case of forward refs */
                                        temp->value = val;
                                        return;

                                /* Naughty, naughty - redefined! */
                                } else {
                                        A6Err(Pass1, LabelIsntVariable, name);
                                        return;
                                }
                        }

                /* Label further down the list? */
                } else if(i > 0) {
                        /* Label is further down the list */
                        oldtemp = temp;
                        temp = temp->next;
                        if(temp == NULL) {
                                temp = (struct label *)malloc(sizeof(struct label));;        //a6_malloc(sizeof(struct label));
                                oldtemp->next = temp;
                                temp->name = newstring(name);
                                temp->filename = af_name();
                                temp->value = val;
                                temp->line = af_line();
                                temp->scope = scope;
                                temp->type = lbltype;
                                temp->af_pos = af_pos();
                                temp->next = NULL;
                                lbl_hash_count[hash]++;
                                return;
                        }

                /* Insert label here */
                } else if(i < 0) {
                        /* Insert it here matey */
                        if(g_pass) {
                                /* This shouldn't happen on pass 2*/
                                A6Err(Fatal, InternalError, "can't insert label in symbol table");
                        }

                        /* Head of list? */
                        if(oldtemp == NULL) {
                                oldtemp = (struct label *)malloc(sizeof(struct label));;             //a6_malloc(sizeof(struct label));
                                oldtemp->name = newstring(name);
                                oldtemp->filename = af_name();
                                oldtemp->value = val;
                                oldtemp->line = af_line();
                                oldtemp->scope = scope;
                                oldtemp->type = lbltype;
                                oldtemp->af_pos = af_pos();
                                oldtemp->next = temp;
                                lbl_hash[hash]=oldtemp;
                                lbl_hash_count[hash]++;
                                return;

                        /* Not head of list */
                        } else {
                                temp = (struct label *)malloc(sizeof(struct label));;                //a6_malloc(sizeof(struct label));
                                oldtemp->next = temp;
                                temp->name = newstring(name);
                                temp->filename = af_name();
                                temp->value = val;
                                temp->line = af_line();
                                temp->scope = scope;
                                temp->type = lbltype;
                                temp->af_pos = af_pos();
                                temp->next = NULL;
                                lbl_hash_count[hash]++;
                                return;
                        }
                }
        }
}

/*----------------------------------------------------------------------------*
 * lbl_getval : return value for label                                        *
 *----------------------------------------------------------------------------*/
unsigned long lbl_getval(char *name)
{
        register struct label *temp;

        temp = lbl_getptr(name, scope_now);

        if(temp==NULL)
                return(0xffff); /* Change to cpu_maxval() */
        else
                return(temp->value);
}

/*----------------------------------------------------------------------------*
 * lbl_enterlocal : define a new local scope                                  *
 *----------------------------------------------------------------------------*/
void lbl_enterlocal(void)
{
        if(scope_stk_pos==MAXLOCALENEST)
                A6Err(Pass2, NestingTooDeep);
        else {
                scope_stk[scope_stk_pos++]=scope_now;
                scope_now=++scope_next;
        }
}

/*----------------------------------------------------------------------------*
 * lbl_exitlocal : exit local scope                                           *
 *----------------------------------------------------------------------------*/
void lbl_exitlocal(void)
{
	if(scope_stk_pos == 0)
	{
		A6Err(Pass2, StackEmpty);
	}
	else
	{
		scope_now = scope_stk[--scope_stk_pos];
	}
}

/*-----------------------------------------------------------------*/
/* Return number of current scope */
unsigned int lbl_getscope(void)
{
        return(scope_now);
}

/*---------------------------------------------------------------------------*
 * lbl_dumpsym : list symbol table                                           *
 *---------------------------------------------------------------------------*/
void lbl_dumpsym(void)
{
	struct label *temp;
	struct label **tempary;
	unsigned int count=0, i, j;
	char labelname[65];

	/* Count labels used */
	for(i = 0; i < 256; i++)
		count += lbl_hash_count[i];

	/* Only print if labels defined */
	if(count) {
		/* Build a sorted list of labels */
		tempary = (struct label **)malloc(sizeof(struct label *) * count);
		count = 0;

		// This will go away when I move it to STL. Tee hee!
		/* Horrific insertion sort.  Look away now! */
		for(i = 0; i < 256; i++) {
			if(lbl_hash_count[i]) {
				temp = lbl_hash[i];
				while(temp != NULL) {
					/* Insert label into list */
					j=count++;
					tempary[j] = temp;
                                        while(j > 0) {
                                                j--;
                                                if(tempary[j]->scope > tempary[j+1]->scope
                                                  || strcmp(tempary[j]->name, tempary[j+1]->name)>0) {
                                                        temp=tempary[j];
                                                        tempary[j]=tempary[j+1];
                                                        tempary[j+1]=temp;
                                                } else
                                                        j=0;
                                        }
                                        temp = temp->next;
                                }
                        }
                }

                /* Begin the printing */
                printf("Symbols defined:\n\n"
                        "Scope Label                            Value Line  File\n"
                        "----- -----                            ----- ----  ----\n");

                scope_now = 0;
                for(i = 0; i < count; i++) {
                        if(tempary[i]->scope != scope_now) {
                                scope_now = tempary[i]->scope;
                                printf("\n%5u ", scope_now);
                        } else {
                                printf("      ");
                        }

                        sprintf(labelname,"%s                               ", tempary[i]->name);

                        labelname[32]='\0';

                        printf("%s $%4x ",labelname, tempary[i]->value); //tohex(tempary[i]->value));

                        if(tempary[i]->line==0)
                                printf("---- %s\n", tempary[i]->filename);
                        else
                                printf("%4u %s\n", tempary[i]->line, tempary[i]->filename);
                }

                printf(" (%u symbols)\n",count);
        }
}
