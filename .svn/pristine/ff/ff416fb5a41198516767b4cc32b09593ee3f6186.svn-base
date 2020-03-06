// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.
#ifndef _COMMON_RANDOM_H_
#define _COMMON_RANDOM_H_

#ifdef _MSC_VER
 #define NOMINMAX //we don't want Windows headers (minwindef.h) to define min() and max() as macros
 #undef min //for some reason, NOMINMAX did not work so we have to #undef anyway
 #undef max
#endif

#include <time.h> //time()
#include <stdint.h> //uintptr_t
#ifdef MULTITHREADED
 #include "threads.h"
#endif
#ifdef LINUX
 #include <unistd.h>
 #include <sys/stat.h>
 #include <fcntl.h>
#endif
#ifdef _WIN32
 #define _WINSOCKAPI_ //http://stackoverflow.com/questions/1372480/c-redefinition-header-files
 #include <rpc.h> //UUID
 #pragma comment(lib, "Rpcrt4.lib")
#endif

//adapted from
//http://en.wikipedia.org/wiki/Mersenne_twister#Pseudocode
//http://my.opera.com/metrallik/blog/2013/04/19/c-class-for-random-generation-with-mersenne-twister-method

class RandomGenerator
{
private:
	static const unsigned int length = 624;
	static const unsigned int bitMask_32 = 0xffffffff;
	static const unsigned int bitPow_31 = 1 << 31;
	static const unsigned int MAXVALUE = 0xffffffff;
	unsigned int counter; //only used in randomize(). uninitialized is OK
#ifdef MULTITHREADED
	pthread_mutex_t lock;
#endif
	unsigned int *mt;
	unsigned int idx;
public:

	RandomGenerator(unsigned int seed)
	{
#ifdef MULTITHREADED
		pthread_mutex_init(&lock, NULL);
#endif
		mt = new unsigned int[length];
		setSeed(seed);
	}

	inline void setSeed(unsigned int seed)
	{
#ifdef MULTITHREADED
		pthread_mutex_lock(&lock);
#endif
		idx = 0;
		mt[0] = seed;
		for (unsigned int i = 1; i < length; i++)
			mt[i] = (1812433253 * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i)&bitMask_32;
#ifdef MULTITHREADED
		pthread_mutex_unlock(&lock);
#endif
	}

	unsigned int randomize()
	{
		unsigned int seed;
		//for ms visual, could use http://msdn.microsoft.com/en-us/library/sxtz2fa8.aspx
#ifdef LINUX
		int fd=open("/dev/urandom",O_RDONLY);
		if (fd>=0)
		{
			read(fd,&seed,sizeof(seed));
			close(fd);
		}
		else
#endif
		{
			counter++;
			seed = (unsigned int)time(NULL);                         //time (seconds); could use hi-res timer but then we would depend on common/timer.h
			seed ^= counter;                           //incremented value, possibly randomly initialized
			seed ^= (unsigned int)(uintptr_t)&counter; //memory address
		}
#ifdef _WIN32 //add more randomness from uuid
		UUID uuid;
		::UuidCreate(&uuid);
		seed ^= uuid.Data1^uuid.Data2^uuid.Data3^uuid.Data4[0];
#endif
		setSeed(seed);
		return seed;
	}

	inline unsigned int getUint32()
	{
#ifdef MULTITHREADED
		pthread_mutex_lock(&lock);
#endif
		if (idx == 0) gen();
		unsigned int y = mt[idx];
		idx = (idx + 1) % length;
#ifdef MULTITHREADED
		pthread_mutex_unlock(&lock);
#endif
		y ^= y >> 11;
		y ^= (y << 7) & 2636928640U;
		y ^= (y << 15) & 4022730752U;
		y ^= y >> 18;
		return y;
	}

	//UniformRandomBitGenerator
	typedef unsigned int result_type;
	static constexpr unsigned int min() {return 0;}
	static constexpr unsigned int max() {return MAXVALUE;}
	inline unsigned int operator()() {return getUint32();}

	inline double getDouble() // [0,1)
	{
		return double(getUint32()) / ((int64_t)(MAXVALUE)+1);
	}

	inline void gen()
	{
		for (unsigned int i = 0; i < length; i++)
		{
			unsigned int y = (mt[i] & bitPow_31) + (mt[(i + 1) % length] & (bitPow_31 - 1));
			mt[i] = mt[(i + 397) % length] ^ (y >> 1);
			if (y % 2) mt[i] ^= 2567483615U;
		}
		return;
	}

	~RandomGenerator()
	{
		delete[] mt;
	}
};
#endif
