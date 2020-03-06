// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _USERTAGS_H_
#define _USERTAGS_H_

/**
  UserTags is the fast way to associate custom data with general purpose objects.
  Objects don't have to know about their users and we can add more users later without recompilation.
  For example: MechObject is general physical object in MechaStick engine.
  In Framsticks simulator every MechObject is connected with Model object, MechParts and MechJoints are
  connected with their counterparts in the Model as well, but the MechObject doesn't even know about it.
  If the SSG visualisation is used, the same MechObject is also connected with SSGFramObject.
  UserTags class is implemented as array, therefore the access is very fast but we have to define
  maximum possible number of tags for every object:

  UserTags<anyclass,datatype,maximumtags> object;
  (all UserTags object for a given 'anyclass' will share registered id's)

  usage:
    1.allocate private id:
       int id=object->userdata.newID();
    'id' is shared across all objects of that class (in this example)
    and therefore you need to allocate it only once.
    0 is illegal here and means that we run out of id values.
       
    2.now you can use id to access your private data inside UserTags:
       object->userdata[id]=...;
       ...=object->userdata[id];
       ...
    3.you can also assign/access object->userdata (which is a shortcut for 
    object->userdata[0]). no id here, so in that case it is your responsibility
    to avoid conflicts.
       object->userdata=...;
       ...=object->userdata;
       ...

    4.free your id when finished:
       object->userdata.freeID(id);
 */

template<class ID, class T, int N> class UserTags
{
	static char reg[N];
	T data[N];
public:
	UserTags() { memset(data, 0, sizeof(data)); }

	/** allocate new id */
	static int newID()
	{
		for (int i = 1; i < N; i++) if (!reg[i]) { reg[i] = 1; return i; }
		DB(printf("Warning: UserTags run out of ids!\n"));
		return 0;
	}
	static void freeID(int id)
	{
		reg[id] = 0;
	}
	T& operator[](int id)
	{
		DB(if (!id) printf("Warning: UserTags @ %p is using id=0\n", this);)
			return data[id];
	}
	operator T() { return data[0]; }
	void operator=(T x) { data[0] = x; }
};

template<class ID, class T, int N> char UserTags<ID,T,N>::reg[N]={0};

#endif
