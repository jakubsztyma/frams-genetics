// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _STATRICK_H_
#define _STATRICK_H_

/**
 @file
 CALLING METHODS WITH "STATIC TRICK" (="STATRICK")

 each trick-callable method has the static counterpart defined by the STATRICKDEF macro

\code
 static void Class::method_statrick(Class* instance,otherparams)
    { instance->method(otherparams); }

------ class.h -----------
 class Class
 {
 #define STATRICKCLASS Class
 STATRICKDEF(method_x)
 STATRICKDEF1(method_y,argtype1)
 STATRICKDEF2(method_z,argtype1,argtype2)
 STATRICKIMPL(method_a)
   {
   ...implementation...
   }
 STATRICKIMPL1(method_b,argtype1)
   {
   ...implementation...
   }
 STATRICKIMPL2(method_c,argtype1,argtype2)
   {
   ...implementation...
   }
 #undef STATRICKCLASS
 }
------ class.cpp ---------
 Class::method_x()
  {
  ...implementation
  }
 Class::method_y(type1 arg1)
  {
  ...implementation
  }
 Class::method_z(type1 arg1,type2 arg2)
  {
  ...implementation
  }
--------------------------
\endcode

 usage:

\code 
 void* methodptr=STATRICKPTR(Class::method);
 STATRICKCALL(method,anyobject);
\endcode

 */

#define STATRICKNAME(name) (name ## _statrick)
#define STATRICKCALL(method,anyobject) (*method(anyobject))
#define STATRICKCALL1(method,anyobject,a) (*method(anyobject,a))
#define STATRICKCALL2(method,anyobject,a,b) (*method(anyobject,a,b))

#define STATRICKSTUB(cls,method)				\
static void method ## _statrick (cls * instance)	\
{ instance -> method (); }

#define STATRICKSTUB1(cls,method,type1) \
static void method ## _statrick (cls * instance, type1 arg1)	\
{ instance -> method (arg1); }

#define STATRICKSTUB2(cls,method,type1,type2)							\
static void method ## _statrick (cls * instance, type1 arg1, type2 arg2) \
{ instance -> method (arg1,arg2); }

#define STATRICKRSTUB(cls,ret,method)				\
static ret method ## _statrick (cls * instance)	\
{ return instance -> method (); }

#define STATRICKRSTUB1(cls,ret,method,type1) \
static ret method ## _statrick (cls * instance, type1 arg1)	\
{ return instance -> method (arg1); }

#define STATRICKRSTUB2(cls,ret,method,type1,type2)							\
static ret method ## _statrick (cls * instance, type1 arg1, type2 arg2) \
{ return instance -> method (arg1,arg2); }

#define STATRICKDEF(method)			\
 STATRICKSTUB(STATRICKCLASS,method)		 \
void method ()

#define STATRICKRDEF(ret,method)	\
 STATRICKRSTUB(STATRICKCLASS,ret,method)		 \
ret method ()

#define STATRICKDEF1(method,type1)	\
 STATRICKSTUB1(STATRICKCLASS,method,type1)             \
void method (type1 arg1)

#define STATRICKRDEF1(ret,method,type1)	\
 STATRICKRSTUB1(STATRICKCLASS,ret,method,type1)             \
ret method (type1 arg1)

#define STATRICKDEF2(method,type1,type2)	\
STATRICKSTUB2(STATRICKCLASS,method,type1,type2)                \
void method (type1 arg1,type2 arg2)

#define STATRICKRDEF2(ret,method,type1,type2)	\
STATRICKRSTUB2(STATRICKCLASS,ret,method,type1,type2)                \
ret method (type1 arg1,type2 arg2)

#endif
