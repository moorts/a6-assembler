///////////////////////////////////////////////////////////////////////////////
//
//	Conditional.cpp
//
//	$Id: Conditional.cpp 10 2012-09-03 23:48:22Z simon.collis@gmail.com $
//
//	Conditional assembly support
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

#include "Conditional.h"
#include "Error.h"

//----------------------------------------------------------------------------
// Singleton gubbins...
Conditional* Conditional::instance	=	NULL;


Conditional* Conditional::Inst()
{
	if(instance == NULL)
	{
		instance = new Conditional();
	}
	return instance;
}

Conditional::Conditional()
{
	StackTop = NULL;
}

Conditional::~Conditional()
{
	Clear();
}

//----------------------------------------------------------------------------
// ConditionalEnter
// Go into a new conditional situation
void Conditional::Enter(int isOn)
{
	// New stack item
	struct condstack *cond = new condstack;
	cond->IsOn = isOn;

	// Is this the first of the stack?
	if(Inst()->StackTop == NULL)
	{
		cond->ParentIsOn = 1;
		cond->Parent = NULL;
	}
	else
	{
		cond->ParentIsOn = instance->StackTop->IsOn;
		cond->Parent = instance->StackTop;
	}

	instance->StackTop = cond;
}

//----------------------------------------------------------------------------
// ConditionalExit
// Exit the current conditional, return to parent
void Conditional::Exit(void)
{
	struct condstack *stack = Inst()->StackTop;

	// Is this the first of the stack?
	if(stack == NULL)
	{
		A6Err(Pass2, StackEmpty);
	}
	else
	{
		// Roll back to previous
		Inst()->StackTop = stack->Parent;
	}
}

//----------------------------------------------------------------------------
// ConditionalIsOn
// Return 1 if the assembly is on
int Conditional::IsOn(void)
{
	if(Inst()->StackTop == NULL)
	{
		return 1;
	}
	else
	{
		return instance->StackTop->IsOn;
	}
}

//----------------------------------------------------------------------------
// ConditionalClear
// Clear conditional stack, ready for pass 2
void Conditional::Clear(void)
{
	struct condstack* node = Inst()->StackTop;

	while(node != NULL)
	{
		struct condstack *p = node->Parent;
		delete node;
		node = p;
	}

	instance->StackTop = NULL;
}

//----------------------------------------------------------------------------
// Else
void Conditional::Else(void)
{
	// Error if stack empty
	if(Inst()->StackTop == NULL)
	{
		A6Err(Pass2, StackEmpty);
	}
	else
	{
		struct condstack *node = instance->StackTop;

		// NB: can only .else when parent assembly is on
		if(node->ParentIsOn)
		{
			node->IsOn = !node->IsOn;
		}
	}
}

//----------------------------------------------------------------------------
// OpenItems
int Conditional::OpenItems()
{
	if(Inst()->StackTop == NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}