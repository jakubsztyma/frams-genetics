// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/vm/classes/collectionobj.h>

void printIndent(int indent)
{
for(int i=0;i<indent;i++)
	putchar(' ');
}

struct Trace
{
ExtObject object;
Trace *previous;
bool isAlreadyVisited(const ExtObject& o)
	{
	if (object==o) return true;
	if (previous) return previous->isAlreadyVisited(o);
	return false;
	}
};

void print(ExtValue* v,int indent=0,Trace *prev_trace=NULL)
{
printIndent(indent);
if (!v)
	{puts("null"); return;}

switch(v->getType())
	{
	case TUnknown: puts("null"); return;
	case TInvalid: puts("invalid"); return;
	case TInt: printf("int:"); break;
	case TDouble: printf("double:"); break;
	case TString: printf("string:"); break;
	case TObj:
		{
		Trace next_trace={v->getObject(),prev_trace};
		printf("%s object:",v->getObject().interfaceName());
		if (prev_trace && prev_trace->isAlreadyVisited(next_trace.object))
			{
			printf(" already visited - breaking recursion\n");
			return;
			}
		VectorObject *vec=VectorObject::fromObject(v->getObject(),false);
		if (vec)
			{
			printf("\n");
			for(int i=0;i<vec->data.size();i++)
				print((ExtValue*)vec->data(i),indent+3,&next_trace);
			return;
			}
		DictionaryObject *dict=DictionaryObject::fromObject(v->getObject(),false);
		if (dict)
			{
			printf("\n");
			for(HashEntryIterator it(dict->hash);it.isValid();it++)
				{
				printIndent(indent+3);
				printf("key \"%s\"\n",it->key.c_str());
				print((ExtValue*)it->value,indent+6,&next_trace);
				}
			return;
			}
		}
	}
puts(v->getString().c_str());
}

int main(int argc,char*argv[])
{
const char* in= (argc>1) ? argv[1] : "[1,2,3,null,{\"x\":3.4,\"y\":[7,77,777]},4]";

// note: this is not an example of how to read a param field, but only a special case where the input data was
// previously identified as "serialized" and the "@Serialized:" prefix was already removed

printf("DESERIALIZATION TEST\n"
       "input string = %s\n",in);
ExtValue v;
const char* ret=v.deserialize(in);
if (ret==NULL)
	{
	printf("failed!\n");
	return 1;
	}
else
	{
	printf("OK, consumed %d of %d characters\n\n",ret-in,strlen(in));
	print(&v);
	return 0;
	}
}
