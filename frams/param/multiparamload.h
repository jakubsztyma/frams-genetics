// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MULTIPARAMLOAD_H_
#define _MULTIPARAMLOAD_H_

#include <stdio.h>
#include "param.h"
#include <common/virtfile/virtfile.h>
#include <frams/util/extvalue.h>

/** This is the general "framsticks-format" file parser for loading multiple objects.
	http://www.framsticks.com/common/formatspec.html
	The loader can be configured to recognize multiple object types from object headers
	and automatically call ParamInterface::load for the matching class.

	Your code should repeatedly call MultiParamLoader::go() method and check the status after each call, until the end of file.
	The loader pauses before and/or after each object giving you a chance to perform your application-specific actions (see MultiParamLoader::breakOn()).
	If your application does not require any special actions, then the simple MultiParamLoader:run() can be used.
	The word "record" (and "record type") used in this description refer to the textual form of a serialized object - this is to avoid confusion with 'live' objects passed to the loader methods. "Record type" can be the same as the class name, but it does not have to be the same. For example, the most common record type for storing the Genotype object is called "org" (think: organism) instead of "Genotype".

	Typical usage scenarios:
	1. Loading a file that contains at most one object of any given class:
	- declare the object class(es) - MultiParamLoader::addClass()
	- call MultiParamLoader::run()
	- and that's all, the records from the file will be loaded into the corresponding objects

	2. Loading multiple objects and adding them to a list (see loadtest.cpp for a sample code that demonstrates this scenario)
	- declare the object class giving the empty "template" object - MultiParamLoader::addClass()
	- set breakOn(AfterObject)
	- call MultiParamLoader::go() in a loop
	- the returned status will be equal to AfterObject each time an object is loaded. One can detect this condition and create the real object from our template object
	(alternatively, one could breakOn(BeforeObject) and use MultiParamLoader::loadObjectNow(ParamInterface*) to load the incoming object into a newly created object).
	*/
class MultiParamLoader
{
	VirtFILE *file;
	SListTempl<VirtFILE*> filestack;
	char ownfile;
	PtrListTempl<ExtObject*> objects;
	int status;
	SString lasterror, lastcomment, lastunknown;
	ExtObject lastobject;
	int breakcond;
	Param emptyparam;
	bool aborting;
	int linenum;

	void init();

	int maybeBreak(int cond)
	{
		status = cond;
		return breakcond & cond;
	}

	VirtFILE* popstack();
	void clearstack();

public:
	MultiParamLoader() { init(); }
	MultiParamLoader(VirtFILE *f) { init(); load(f); }
	MultiParamLoader(const char* filename) { init(); load(filename); }

	virtual ~MultiParamLoader() { abort(); clearObjects(); }

	void reset();

	void load(); //< use previously opened file
	void load(VirtFILE *f);
	void load(const char* filename);

	/** Register the object class. Class names will be matched with object headers ("xxx:" in the input file).
		Used with breakOn(BeforeObject) and/or breakOn(AfterObject).
		Note that registered classes will only work when the record name matches the class name, otherwise breakOn(BeforeUnknown) must be used and then getClassName() to check for the expected record.
		*/
	void addObject(ParamInterface *pi) { objects += new ExtObject(pi); }
	void removeObject(ParamInterface *pi) { removeObject(ExtObject(pi)); }
	void addObject(const ExtObject &o) { objects += new ExtObject(o); }
	void removeObject(const ExtObject &o);
	int findObject(const ExtObject &o);
	void clearObjects();

	/** To be used in the main loop: while(event=loader.go()) { ... }
		loader.go() will return on specified events (@see breakOn(), noBreakOn()),
		then you can handle the event and resume loading.
		*/
	virtual int go();
	/** same value as 'go()' */
	int getStatus() { return status; }
	int finished() { return (status == Finished); }

	VirtFILE *getFile() { return file; }
	SString currentFilePathForErrorMessage();

	/** Abort immediately and close the file if needed */
	void abort();
	/** @param conditions can be combined bitwise, eg. MultiParamLoader::BeforeObject |  MultiParamLoader::OnComment
		@see BreakConfitions
		*/
	void breakOn(int conditions) { breakcond |= conditions; }
	void noBreakOn(int conditions) { breakcond &= ~conditions; }
	/**
	   These constants are used as arguments in breakOn(), and as status values from go() and getStatus().
	   The user code can define some classes for automatic recognition (using addClass()); such records can be read without performing any additional actions.

	   - BeforeObject: found an object with recognized classname (addClass()). Application code can choose to skip the incoming record (skipObject()), redirect the incoming data into a different object (loadObjectNow(ParamInterface*)), or do nothing for default behavior (loading into previously registered object).

	   - AfterObject: the object was loaded into the registered class interface (addClass()). This is to allow for additional actions after loading the object (e.g. data validation).

	   - BeforeUnknown: unknown (not registered) object header detected. Like in BeforeObject, the application can skipObject() or loadObjectNow().

	   @see getClass(), GetClassName()
	   */
	enum BreakConditions {
		Finished = 0, BeforeObject = 1, AfterObject = 2,
		BeforeUnknown = 4, OnComment = 8, OnError = 16, Loading = 32
	};

	/** Can be used BeforeObject and AfterObject */
	ExtObject &getObject() { return lastobject; }
	/** Can be used BeforeUnknown, BeforeObject, AfterObject */
	const SString& getObjectName() { return lastunknown; }
	void setObjectName(SString n) { lastunknown = n; }
	/** Unknown object will be loaded if you set its class BeforeUnknown */
	void setObject(ParamInterface *pi) { lastobject = ExtObject(pi); }
	void setObject(const ExtObject& o) { lastobject = o; }
	/** Can be used OnComment */
	const SString& getComment() { return lastcomment; }
	/** Can be used OnError */
	const SString& getError() { return lasterror; }
	/** Can be used BeforeObject and BeforeUnknown */
	int loadObjectNow(ParamInterface *pi, bool warn_unknown_fields = true) { return loadObjectNow(ExtObject(pi), warn_unknown_fields); }
	int loadObjectNow(const ExtObject &o, bool warn_unknown_fields = true);
	/** Can be used BeforeObject */
	int loadObjectNow() { return loadObjectNow(getObject()); }
	/** Can be used BeforeObject and BeforeUnknown.
		Object data will not be loaded. */
	int skipObject() { return loadObjectNow(&emptyparam, false); }
	/** @return 1 if no errors */
	int run();

	void includeFile(SString& filename);
	bool returnFromIncluded();
	bool alreadyIncluded(const char* filename);

};

#endif
