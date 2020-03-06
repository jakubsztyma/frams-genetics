// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "loggers.h"
#include <common/util-string.h>
#include <string.h>

void logMessage(const char *obj, const char *method, int level, const char *msg)
{
	tlsGetRef(message_handler_manager_instance).send(obj, method, level, msg);
}

THREAD_LOCAL_DEF(LoggerManager, message_handler_manager_instance);

void LoggerManager::send(int position, const char *obj, const char *method, int level, const char *msg)
{
	if (position >= (int)loggers.size()) position = loggers.size() - 1;
	bool blocked = false;
	for (int i = position; i >= 0; i--)
	{
		LoggerBase *logger = loggers[i];
		if ((!(logger->options & LoggerBase::Paused)) &&
			((!blocked) || (logger->options & LoggerBase::CannotBeBlocked)))
		{
			logger->handle(obj, method, level, msg);
			if (!(logger->options & LoggerBase::DontBlock)) blocked = true;
		}
	}
}

int LoggerManager::add(LoggerBase *logger)
{
	logger->manager = this;
	loggers.push_back(logger);
	return loggers.size() - 1;
}

void LoggerManager::remove(int i)
{
	LoggerBase *h = loggers[i];
	h->manager = NULL;
	loggers.erase(loggers.begin() + i);
}

void LoggerManager::remove(LoggerBase *logger)
{
	int index = find(logger);
	if (index >= 0)
		remove(index);
}

void LoggerManager::removeAll()
{
	while (loggers.size() > 0)
		remove(loggers.size() - 1);
}

//////////////////////////////////

void LoggerBase::send(const char *obj, const char *method, int level, const char *msg)
{
	if (!isEnabled()) return;
	int position = manager->find(this);
	if (position >= 0) manager->send(position - 1, obj, method, level, msg);
}

void LoggerBase::logPrintf(const char *obj, const char *method, int level, const char *msg, ...)
{
	if (!isEnabled()) return;
	string buf;
	va_list argptr;
	va_start(argptr, msg);
	buf = ssprintf_va(msg, argptr);
	va_end(argptr);
	send(obj, method, level, buf.c_str());
}


void LoggerBase::enable()
{
	if (isEnabled()) return;
	tlsGetRef(message_handler_manager_instance).add(this);
}

void LoggerBase::disable()
{
	if (!isEnabled()) return;
	tlsGetRef(message_handler_manager_instance).remove(this);
}

void LoggerBase::pause()
{
	if (isPaused()) return;
	options |= Paused;
}

void LoggerBase::resume()
{
	if (!isPaused()) return;
	options &= ~Paused;
}

void LoggerBase::handle(const char *obj, const char *method, int level, const char *msg)
{
	int line = 0; //all lines except the first one get the "..." prefix
	const char* nextsep;
	do
	{
		nextsep = strchr(msg, '\n');
		if (nextsep == NULL) //last chunk, until the end
			nextsep = strchr(msg, '\0');
		if ((nextsep > msg) && (nextsep[-1] == '\r'))
			nextsep--;
		if (line == 0)
		{
			if (*nextsep == 0) //there was only one line! no need to modify it in any way.
				handleSingleLine(obj, method, level, msg);
			else //first line from multi-line
				handleSingleLine(obj, method, level, string(msg, nextsep - msg).c_str());
		}
		else //consecutive lines from multi-line
			handleSingleLine(obj, method, level, (LOG_MULTILINE_CONTINUATION + string(msg, nextsep - msg)).c_str()); //could also add line numbers like ...(3)... but let's keep the prefix short and simple
		line++;
		if ((options & Paused) != 0) break; //seemingly pointless test (it was already checked), but 'options' can change inside handleSingleLine().
		if ((nextsep[0] == '\r') && (nextsep[1] == '\n'))
			msg = nextsep + 2;
		else if (*nextsep)
			msg = nextsep + 1;
	} while (*nextsep);
}

/////////////////////////////////

void LoggerToMemory::handle(const char *obj, const char *method, int level, const char *msg)
{
	if (level > maxlevel) maxlevel = level;
	if (level >= LOG_INFO) infocount++;
	if (level >= LOG_WARN) warncount++;
	if (level >= LOG_ERROR) errcount++;

	if (level >= minleveltostore)
	{
		storedcount++;
		if (options & (StoreFirstMessage | StoreAllMessages))
		{
			if (!((options&StoreFirstMessage) && (msgs.length() > 0)))
			{
				if (msgs.length() > 0) msgs += '\n';
				msgs += ssprintf(LOG_FORMAT, logLevelName(level), obj, method, msg);
			}
		}
	}
}

string LoggerToMemory::getCountSummary() const
{
	if (getInfoCount())
	{
		string msg;
		if (getErrorCount())
			msg = ssprintf("%d error(s)", getErrorCount());
		int w;
		if (w = getWarningCount() - getErrorCount())
			msg += ssprintf("%s%d warning(s)", (getErrorCount() ? ", " : ""), w);
		if (w = getInfoCount() - getWarningCount())
			msg += ssprintf("%s%d message(s)", (getWarningCount() ? ", " : ""), w);
		return msg;
	}
	return string("");
}
