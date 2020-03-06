// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _COMMON_LOGGERS_H_
#define _COMMON_LOGGERS_H_

#include <common/log.h>
#include <common/threads.h>
#include <common/nonstd_stl.h>

class LoggerBase;

class LoggerManager
{
	friend class LoggerBase;
	vector<LoggerBase*> loggers;
	void send(int position, const char *obj, const char *method, int level, const char *msg);
public:
	int find(LoggerBase *logger)
	{
		vector<LoggerBase*>::iterator it = std::find(loggers.begin(), loggers.end(), logger);
		return it == loggers.end() ? -1 : std::distance(loggers.begin(), it);
	}
	int add(LoggerBase *logger);
	void remove(int i);
	void remove(LoggerBase *logger);
	void removeAll();
	void send(const char *obj, const char *method, int level, const char *msg)
	{
		send(loggers.size() - 1, obj, method, level, msg);
	}
	~LoggerManager() { removeAll(); }
};

extern THREAD_LOCAL_DECL(LoggerManager, message_handler_manager_instance);

////////////////////////////////////////

class LoggerBase
{
	friend class LoggerManager;
protected:
	LoggerManager* manager;
	int options;

public:

	enum LoggerOptions
	{
		DontBlock = 1, CannotBeBlocked = 2, Enable = 4, Paused = 8
	};

	void logPrintf(const char *o, const char *m, int w, const char *bl, ...);
	void send(const char *obj, const char *method, int level, const char *msg);

	bool isEnabled() { return manager ? true : false; }
	void enable();
	void disable();
	bool isPaused() { return (options & Paused) != 0; }
	void pause();
	void resume();

	LoggerBase(int opts = 0) :manager(NULL), options(opts)
	{
		if (options&Enable) enable();
	}
	virtual ~LoggerBase()
	{
		disable();
	}

	virtual void handle(const char *obj, const char *method, int level, const char *msg); ///< implemented by loggers accepting multiline messages. if not implemented, the default handle() splits multiline text into single lines and calls handleSingleLine()
	virtual void handleSingleLine(const char *obj, const char *method, int level, const char *msg) {}  ///< implemented by loggers expecting single line messages
};

///////////////////////////////////////////

class LoggerToMemory : public LoggerBase
{
protected:
	int maxlevel, minleveltostore, errcount, warncount, infocount, storedcount;
	string msgs;

public:

	void reset() { maxlevel = LOG_MIN - 1; errcount = warncount = infocount = storedcount = 0; msgs = ""; }

	enum Options2
	{
		StoreFirstMessage = 16, StoreAllMessages = 32
	};

	int getErrorCount() const   { return errcount; }
	int getWarningCount() const { return warncount; }
	int getInfoCount() const    { return infocount; }
	int getStoredCount() const  { return storedcount; }
	int getErrorLevel() const   { return maxlevel; }
	string getMessages() const  { return msgs; }
	string getCountSummary() const; ///< return the standard "... error(s), ... warning(s), ... message(s)" text (or empty string if count==0)

	LoggerToMemory(int opts = 0, int minimal_level_to_store = LOG_ERROR) :LoggerBase(opts), minleveltostore(minimal_level_to_store)
	{
		reset();
	}

	void handle(const char *obj, const char *method, int level, const char *msg);
};

class RedirectingLogger : public LoggerBase
{
	LoggerManager *other_manager;
public:
	RedirectingLogger(LoggerManager *other_mgr, int opts = 0)
		:LoggerBase(opts), other_manager(other_mgr) {}

	void handle(const char *obj, const char *method, int level, const char *msg)
	{
		other_manager->send(obj, method, level, msg);
	}
};

#endif
