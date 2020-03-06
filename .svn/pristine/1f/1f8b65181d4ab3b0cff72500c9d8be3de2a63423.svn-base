// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "callbacks.h"
#include <stdio.h>

#ifdef USEMEMBERCALLBACK
int MemberCallbackNode::equals(CallbackNode*n)
{
	if (n == this) return 1;
	MemberCallbackNode *classok = dynamic_cast<MemberCallbackNode*>(n);
	if (!classok) return 0;
	return ((userdata == classok->userdata) && (object == classok->object) && (member == classok->member));
}
#endif

int FunctionCallbackNode::equals(CallbackNode*n)
{
	if (n == this) return 1;
	FunctionCallbackNode *classok = dynamic_cast<FunctionCallbackNode*>(n);
	if (!classok) return 0;
	return ((userdata == classok->userdata) && (fun == classok->fun));
}

int StatrickCallbackNode::equals(CallbackNode*n)
{
	if (n == this) return 1;
	StatrickCallbackNode *classok = dynamic_cast<StatrickCallbackNode*>(n);
	if (!classok) return 0;
	return ((object == classok->object) && (userdata == classok->userdata) && (fun == classok->fun));
}

/////////////////

CallbackNode* Callback::add(CallbackNode*n)
{
	SList::operator+=(n);
	return n;
}

void Callback::removeNode(CallbackNode*n)
{
	SList::operator-=(n);
	delete n;
}

void Callback::remove(CallbackNode*node)
{
	CallbackNode *n;
	//printf("Hint: removing callbacks (former 'DuoList') is more efficient using removeNode(). (refer to 'callbacks.h')\n");
	for (int i = 0; n = (CallbackNode *)operator()(i); i++)
		if (node->equals(n))
		{
		SList::operator-=(i);
		delete node;
		if (n != node) delete n;
		return;
		}
	delete node; // tu nie wiem czy na pewno...
}

void Callback::action(intptr_t data)
{
	if (size() == 0) return;
	SList copy = *this;
	FOREACH(CallbackNode*, n, copy)
		n->action(data);
}

Callback::~Callback()
{
	CallbackNode *n;
	for (int i = size() - 1; i >= 0; i--)
	{
		n = (CallbackNode *)operator()(i);
		delete n;
		// todo: zrobic zeby kolejnosc delete callbacknode <-> delete callback nie wplywala na poprawne dzialania
		// blad odkryty 24.01 pokazal, ze deletowanie callbacknodow w ~callback
		// moze powodowac problemy, jezeli obiekty sa usuwane w "zlej" kolejnosci
		// ale na razie tak zostanie
	}
}

