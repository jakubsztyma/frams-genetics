// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "extvalue.h"
#include <frams/param/param.h>
#include "sstringutils.h"
#include <ctype.h>
#include <frams/vm/classes/collectionobj.h>
#include <frams/vm/classes/3dobject.h>
#include <frams/vm/classes/genoobj.h>
#include <common/nonstd_math.h>
#include <common/Convert.h>
#include <climits>
#include <errno.h>

#ifndef NO_BARRIER
#include <frams/simul/barrier.h>
#include <common/threads.h>
#endif

#ifdef MULTITHREADED
#include <pthread.h>
//this lock only protects against ref.counter corruption caused by concurrent reads.
//read/write conficts and nonatomicity are handled by BarrierObject (at least in theory ;-))
static pthread_mutex_t extobject_ref_lock = PTHREAD_MUTEX_INITIALIZER;
#define REF_LOCK pthread_mutex_lock(&extobject_ref_lock)
#define REF_UNLOCK pthread_mutex_unlock(&extobject_ref_lock)
#else
#define REF_LOCK 
#define REF_UNLOCK 
#endif

void ExtObject::incref() const
{
	if (subtype & 1)
	{
		REF_LOCK;
		dbobject->refcount++;
		REF_UNLOCK;
	}
}

void ExtObject::decref() const
{
	if (subtype & 1)
	{
		REF_LOCK;
		bool destroy = !--dbobject->refcount;
		REF_UNLOCK;
		//another thread can now access the object while we are deleting it
		//but this is not a bug since we only guarantee read/read safety
		if (destroy) delete dbobject;
	}
}

bool ExtObject::operator==(const ExtObject& src) const
{
	if (object != src.object) return false;
	const char* n1 = interfaceName();
	const char* n2 = src.interfaceName();
	return (n1 == n2) || (strcmp(n1, n2) == 0);
}

bool ExtObject::makeUnique()
{
	if (!(subtype & 1)) return false;
	if (dbobject->refcount == 1) return false;
	VectorObject* v = VectorObject::fromObject(*this, false);
	if (v)
	{
		VectorObject* n = new VectorObject;
		n->data.setSize(n->data.size());
		for (int i = 0; i < v->data.size(); i++)
		{
			ExtValue *x = (ExtValue*)v->data(i);
			n->data.set(i, x ? new ExtValue(*x) : NULL);
		}
		operator=(n->makeObject());
		return true;
	}
	return false;
}

void* ExtObject::getTarget(const char* classname, bool through_barrier, bool warn) const
{
	if (!strcmp(interfaceName(), classname))
		return getTarget();
#ifndef NO_BARRIER
	if (through_barrier)
	{
		BarrierObject *bo = BarrierObject::fromObject(*this);
		if (bo)
			return bo->getSourceObject().getTarget(classname, true, warn);
	}
#endif

	if (warn)
	{
		logPrintf("ExtValue", "getObjectTarget", LOG_WARN, "%s object expected, %s found", classname, interfaceName());
	}

	return NULL;
}

bool ExtObject::callDelegate(const char* delegate, ExtValue *args, ExtValue *ret)
{
	Param tmp;
	ParamInterface *pi = getParamInterface(tmp);
	if (pi)
	{
		int f = pi->findId(delegate);
		if (f >= 0)
		{
			pi->call(f, args, ret);
			return true;
		}
	}
	logPrintf("Genotype", "get", LOG_ERROR, "Could not call delegate '%s.%s'", pi ? pi->getName() : "NULL", delegate);
	return false;
}

SString ExtObject::toString() const
{
	if (isEmpty()) return SString("null");
	Param tmp_param;
	ParamInterface *p = getParamInterface(tmp_param);
	int tostr = p->findId("toString");
	if (tostr >= 0)
	{
		return SString(p->getString(tostr));
	}
	else
	{
		SString tmp("<");
		tmp += p->getName();
		tmp += SString::sprintf(" object at %p>", object ? object : paraminterface);
		return tmp;
	}
}

THREAD_LOCAL_DEF(ExtObject::Serialization, ExtObject_serialization);

void ExtObject::Serialization::begin()
{
	if (level == 0)
		refs.clear();
	level++;
}

int ExtObject::Serialization::add(const ExtObject &o)
{
	if (o.isEmpty()) return -1;
	for (int i = 0; i < (int)refs.size(); i++)
	{
		ExtObject& r = refs[i];
		if (r == o) return i;
	}
	refs.push_back(o);
	return -1;
}

void ExtObject::Serialization::replace(const ExtObject& o, const ExtObject& other)
{
	if (o.isEmpty()) return;
	for (int i = 0; i < (int)refs.size(); i++)
	{
		ExtObject& r = refs[i];
		if (r == o)
		{
			r = other;
			return;
		}
	}
}

void ExtObject::Serialization::remove(const ExtObject& o)
{
	if (o.isEmpty()) return;
	for (int i = 0; i < (int)refs.size(); i++)
	{
		ExtObject& r = refs[i];
		if (o == r) refs.erase(refs.begin() + i);
	}
}

const ExtObject* ExtObject::Serialization::get(int ref)
{
	if (ref < 0) return NULL;
	if (ref >= (int)refs.size()) return NULL;
	return &refs[ref];
}

void ExtObject::Serialization::end()
{
	level--;
	if (level == 0)
		refs.clear();
}

SString ExtObject::serialize_inner(SerializationFormat format) const
{
	int ref = tlsGetRef(ExtObject_serialization).add(*this);
	SString ret;

	if (ref >= 0)
	{
		switch (format)
		{
		case NativeSerialization: return SString::sprintf("^%d", ref);
		case JSONSerialization: return SString("null");
		}
	}

	if (isEmpty()) return SString("null");
	{
		VectorObject *vec = VectorObject::fromObject(*this, false);
		if (vec)
		{
			ret = vec->serialize(format); goto finally;
		}
	}
	{
		DictionaryObject *dic = DictionaryObject::fromObject(*this, false);
		if (dic)
		{
			ret = dic->serialize(format); goto finally;
		}
	}
	{
		Param tmp_param;
		ParamInterface *p = getParamInterface(tmp_param);
		int m = p->findId("toVector");
		if (m < 0)
			m = p->findId("toDictionary");
		if (m >= 0)
		{
			ExtObject o(p->getObject(m));
			switch (format)
			{
			case NativeSerialization: ret = SString(interfaceName()) + o.serialize(format); break;
			case JSONSerialization: ret = SString::sprintf("{\"class\":\"%s\",\"data\":%s}", interfaceName(), o.serialize(format).c_str()); break;
			}
			goto finally;
		}
		m = p->findId("toString");
		if (m >= 0)
		{
			SString str = p->getString(m);
			sstringQuote(str);
			switch (format)
			{
			case NativeSerialization: ret = SString(interfaceName()) + "\"" + str + "\""; break;
			case JSONSerialization: ret = SString::sprintf("{\"class\":\"%s\",\"data\":\"%s\"}", interfaceName(), str.c_str()); break;
			}
			goto finally;
		}
	}

	tlsGetRef(ExtObject_serialization).remove(*this);//undo nonserializable reference
	switch (format)
	{
	case NativeSerialization: return SString(interfaceName()) + SString::sprintf("<%p>", object ? object : paraminterface); break;
	case JSONSerialization: return SString::sprintf("{\"class\":\"%s\"}", interfaceName()); break;
	}

	finally: // not 100% "finally", the case of nonserializable reference (directly above) returns directly without going through finally

	switch (format)
	{
	case JSONSerialization:
		tlsGetRef(ExtObject_serialization).remove(*this);//JSON only tracks recursion, does not track reuse
		break;
	case NativeSerialization:; //nop (just to avoid compiler warning)
	}

	return ret;
}

SString ExtObject::serialize(SerializationFormat format) const
{
	tlsGetRef(ExtObject_serialization).begin();
	SString ret = serialize_inner(format);
	tlsGetRef(ExtObject_serialization).end();
	return ret;
}

///////////////////////////////////////

SString ExtValue::typeDescription() const
{
	switch (type)
	{
	case TInt: return SString("int");
	case TDouble: return SString("float");
	case TString: return SString("string");
	case TUnknown: return SString("null");
	case TInvalid: return SString("invalid");
	case TObj: return getObject().isEmpty() ? SString("null") : SString(getObject().interfaceName());
	}
	return SString::empty();
}

SString ExtValue::typeAndValue() const
{
	SString msg = typeDescription();
	SString delimit("'");
	switch (type)
	{
	case TString:
		delimit = "\"";
	case TInt: case TDouble: case TObj:
		msg += " ";
		msg += sstringDelimitAndShorten(getString(), 50, (type == TString), delimit, delimit);
	default:;
	}
	return msg;
}

void *ExtValue::getObjectTarget(const char* classname, bool warn) const
{
	if (type != TObj)
	{
		if (warn)
		{
			SString tmp = getString();
			if (tmp.len() > 30) tmp = tmp.substr(0, 30) + "...";
			if (type == TString) tmp = SString("\"") + tmp + SString("\"");
			logPrintf("ExtValue", "getObjectTarget", LOG_WARN, "%s object expected, %s found", classname, tmp.c_str());
		}
		return NULL;
	}

	return getObject().getTarget(classname, true, warn);
}

void ExtValue::set(const ExtValue& src)
{
	switch (src.type)
	{
	case TString: sets(src.sdata()); break;
	case TInt: seti(src.idata()); break;
	case TDouble: setd(src.ddata()); break;
	case TObj: seto(src.odata()); break;
	default:type = src.type; break;
	}
}

void ExtValue::setEmpty()
{
	switch (type)
	{
#ifdef EXTVALUEUNION
	case TString: sdata().~SString(); break;
	case TObj: odata().~ExtObject(); break;
#else
	case TString: delete s; break;
	case TObj: delete o; break;
#endif
	default:;
	}
	type = TUnknown;
}

void ExtValue::setError(const SString& msg)
{
	ErrorObject *err = new ErrorObject;
	err->message = msg;
	setObject(ErrorObject::makeDynamicObject(err));
}

static ExtValue::CompareResult longsign(paInt x)
{
	if (x < 0) return ExtValue::ResultLower;
	if (x > 0) return ExtValue::ResultHigher;
	return ExtValue::ResultEqual;
}

static ExtValue::CompareResult compareNull(const ExtValue& v)
{
	if (v.isNull()) return ExtValue::ResultEqualUnordered;
	if ((v.getType() == TInt) && (v.getInt() == 0)) return ExtValue::ResultUnequal_RelaxedEqual;
	return ExtValue::ResultUnequal_RelaxedUnequal; //comparing anything else with null is valid but null is neither higher nor lower than numbers or strings
}

static ExtValue::CompareResult compareInvalid(const ExtValue& v)
{
	if (v.getType() == TInvalid) return ExtValue::ResultEqualUnordered;
	if ((v.getType() == TInt) && (v.getInt() == 0)) return ExtValue::ResultUnequal_RelaxedEqual;
	return ExtValue::ResultMismatch; //comparing anything else with invalid is invalid
}

static ExtValue::CompareResult compareFloat(double a, double b)
{
	double t = a - b;
	if (t < 0) return ExtValue::ResultLower;
	else if (t > 0) return ExtValue::ResultHigher;
	return ExtValue::ResultEqual;
}

static ExtValue::CompareResult compareString(const SString &a, const SString &b)
{
	const char* s1 = a.c_str();
	const char* s2 = b.c_str();
	return longsign(strcmp(s1, s2));
}

ExtValue::CompareResult ExtValue::compare(const ExtValue& src) const
{
	if (isNull())
		return compareNull(src);
	else if (src.isNull())
		return compareNull(*this);
	if (getType() == TInvalid)
		return compareInvalid(src);
	else if (src.getType() == TInvalid)
		return compareInvalid(*this);
	switch (type)
	{

	case TInt:

		if (src.getType() == TInt)
		{
			paInt t = src.getInt();
			if (idata() > 0)
			{
				if (t > 0) return longsign(idata() - t); else return ResultHigher;
			}
			else
			{
				if (t <= 0) return longsign(idata() - t); else return ResultLower;
			}
		}
		else if (src.getType() == TDouble)
			return compareFloat((double)idata(), src.getDouble());
		else if ((getInt() == 0) && (src.getType() == TString))
			return ResultMismatch_RelaxedUnequal;
		else
			return ResultMismatch;//comparing numbers with other things is invalid
		break;

	case TDouble:
		if ((src.getType() == TDouble) || (src.getType() == TInt))
			return compareFloat(getDouble(), src.getDouble());
		else
			return ResultMismatch;
		break;

	case TString:
		if (src.getType() == TString)
			return compareString(sdata(), src.getString());
		else if ((src.type == TInt) && (src.getInt() == 0))
			return ResultMismatch_RelaxedUnequal;
		else
			return ResultMismatch;
		break;

	case TObj:
	{
		if (src.type == TObj)
			return odata() == src.odata() ? ResultEqualUnordered : ResultUnequal_RelaxedUnequal;
		if ((src.type == TInt) && (src.getInt() == 0))
			return ResultMismatch_RelaxedUnequal;
		return ResultMismatch;
	}
	default:;
	}
	return ResultMismatch;
}

const char* ExtValue::cmp_op_names[] = { "==", "!=", ">=", "<=", ">", "<", "~=", "!~", NULL };

int ExtValue::interpretCompare(CmpOperator op, CompareResult result, CmpContext *context)
{
	CompareResult error_threshold = ResultUnequal_RelaxedEqual;//error when ResultUnequal_RelaxedEqual or higher (not comparable)
	int ret = 0;
	switch (op)
	{
	case CmpEQ: ret = (result == ResultEqual) || (result == ResultEqualUnordered); error_threshold = ResultMismatch_RelaxedUnequal; break;
	case CmpNE: ret = !((result == ResultEqual) || (result == ResultEqualUnordered)); error_threshold = ResultMismatch_RelaxedUnequal; break;
	case CmpGT: ret = (result == ResultHigher); error_threshold = ResultEqualUnordered; break;
	case CmpGE: ret = (result == ResultEqual) || (result == ResultHigher); error_threshold = ResultEqualUnordered; break;
	case CmpLT: ret = (result == ResultLower); error_threshold = ResultEqualUnordered; break;
	case CmpLE: ret = (result == ResultEqual) || (result == ResultLower); error_threshold = ResultEqualUnordered; break;
	case CmpREQ: ret = (result == ResultEqual) || (result == ResultEqualUnordered) || (result == ResultUnequal_RelaxedEqual); error_threshold = ResultMismatch; break;
	case CmpRNE: ret = !((result == ResultEqual) || (result == ResultEqualUnordered) || (result == ResultUnequal_RelaxedEqual)); error_threshold = ResultMismatch; break;
	default:;
	}
	if (result >= error_threshold)
	{
		SString msg = "Type mismatch while comparing";
		if (context)
		{
			if (context->v1 && context->v2)
				msg += SString::sprintf(": %s %s %s",
					context->v1->typeAndValue().c_str(),
					cmp_op_names[op - CmpFIRST],
					context->v2->typeAndValue().c_str());
		}
		logPrintf("ExtValue", "interpretCompare", LOG_ERROR, "%s", msg.c_str());
		ret = -1;
	}
	return ret;
}

int ExtValue::operator==(const ExtValue& src) const
{
	if (type != src.type) return 0;
	switch (type)
	{
	case TInt: return idata() == src.idata();
	case TDouble: return ddata() == src.ddata();
	case TString: return sdata() == src.sdata();
	case TObj: return odata() == src.odata();
	default:;
	}
	return 1;
}

void ExtValue::operator+=(const ExtValue& src)
{
	// return = ok, break = fail
	switch (type)
	{
	case TInt:
		switch (src.getType())
		{
		case TDouble:
			setDouble(double(getInt()) + src.getDouble());
			return;
		case TString:
			break;
		default:
			idata() += src.getInt();
			return;
		}
		break;
	case TDouble:
		switch (src.getType())
		{
		case TString:
			break;
		default:
			ddata() += src.getDouble();
			return;
		}
		break;
	case TString: sdata() += src.getString(); return;
	case TObj:
	{
		VectorObject *vec = VectorObject::fromObject(getObject(), false);
		VectorObject *vec2 = VectorObject::fromObject(src.getObject(), false);
		if (vec && vec2)
		{
			for (int i = 0; i < vec2->data.size(); i++)
			{
				ExtValue *s = (ExtValue*)vec2->data(i);
				ExtValue *d = s ? new ExtValue(*s) : NULL;
				vec->data += d;
			}
			return;
		}
	}
	//NO break;
	default:;
	}
	logPrintf("ExtValue", "add", LOG_ERROR, "Can't add %s to %s", src.typeAndValue().c_str(), typeAndValue().c_str());
}

void ExtValue::operator-=(const ExtValue& src)
{
	// return = ok, break = fail
	switch (type)
	{
	case TInt:
		switch (src.getType())
		{
		case TInt:
			idata() -= src.getInt();
			return;
		case TDouble:
			setDouble(double(getInt()) - src.getDouble());
			return;
		default:;
		}
		break;
	case TDouble:
		switch (src.getType())
		{
		case TDouble:
		case TInt:
			ddata() -= src.getDouble();
			return;
		default:;
		}
		break;
	default:;
	}
	logPrintf("ExtValue", "subtract", LOG_ERROR, "Can't subtract %s from %s", src.typeAndValue().c_str(), typeAndValue().c_str());
}

void ExtValue::operator*=(const ExtValue& src)
{
	// return = ok, break = fail
	switch (type)
	{
	case TInt:
		switch (src.getType())
		{
		case TInt:
			idata() *= src.getInt();
			return;
		case TDouble:
			setDouble(double(getInt())*src.getDouble());
			return;
		default:;
		}
		break;
	case TDouble:
		switch (src.getType())
		{
		case TInt:
		case TDouble:
			ddata() *= src.getDouble();
			return;
		default:;
		}
		break;
	case TString:
		switch (src.getType())
		{
		case TInt: case TDouble:
		{
			SString t;
			for (int n = src.getInt(); n > 0; n--)
				t += getString();
			setString(t);
			return;
		}
		default:;
		}
		break;
	case TObj:
	{
		VectorObject *vec = VectorObject::fromObject(getObject(), false);
		if (vec)
		{
			int n = src.getInt();
			int orig_size = vec->data.size();
			if (n <= 0)
			{
				vec->clear(); return;
			}
			for (; n > 1; n--)
			{
				for (int i = 0; i < orig_size; i++)
				{
					ExtValue *s = (ExtValue*)vec->data(i);
					ExtValue *d = s ? new ExtValue(*s) : NULL;
					vec->data += d;
				}
			}
			return;
		}
	}
	//NO break;
	default:;
	}
	logPrintf("ExtValue", "multiply", LOG_WARN, "Can't multiply %s by %s", typeAndValue().c_str(), src.typeAndValue().c_str());
}

/*#include "fpu_control.h"
#include <signal.h>

static int fpuexception;
void mathhandler(int sig)
{
printf("fpu exception!\n");
fpuexception=1;
signal(SIGFPE,SIG_IGN);
} */

void ExtValue::divInt(paInt a)
{
	if (a)
		idata() /= a;
	else
	{
		logPrintf("ExtValue", "divide", LOG_ERROR, "Division by zero: %d/0", idata());
		setInvalid();
	}
}

void ExtValue::divDouble(double a)
{
	if (a == 0.0)
	{
		logPrintf("ExtValue", "divide", LOG_ERROR, "Division by zero: %s/0.0", getString().c_str());
		setInvalid();
	}
	else
	{
		fpExceptDisable();
		double tmp = getDouble() / a;
		if (!finite(tmp))
		{
			logPrintf("ExtValue", "divide", LOG_ERROR, "Overflow %s/%g", getString().c_str(), a); setInvalid();
		}
		else
			setDouble(tmp);
		// niby dobrze ale lepiej byloby to robic bardziej systematycznie a nie tylko w dzieleniu?
		//if (isnan(ddata())) //http://www.digitalmars.com/d/archives/c++/Traping_divide_by_zero_5728.html
		//	  { logPrintf("ExtValue","divide",LOG_ERROR,"not-a-number",(const char*)getString()); setInvalid(); }
		fpExceptEnable();
	}
}

void ExtValue::operator/=(const ExtValue& src)
{
	switch (type)
	{
	case TInt:
		switch (src.getType())
		{
		case TInt:
			divInt(src.idata());
			return;
		case TDouble:
			divDouble(src.ddata());
			return;
		default:;
		}
		break;

	case TDouble:
		switch (src.getType())
		{
		case TInt:
			divDouble(src.getDouble());
			return;
		case TDouble:
			divDouble(src.ddata());
			return;
		default:;
		}
		break;

	default:;
	}
	logPrintf("ExtValue", "divide", LOG_ERROR, "Can't divide %s by %s", typeAndValue().c_str(), src.typeAndValue().c_str());
}

SString ExtValue::formatTime(char fmt, double value)
{
	if (fmt == 'i')
	{ //could be Convert::ctime()
		int d, h, m, ti = value;
		int ms = 1000 * (value - ti);
		d = ti / 86400; ti -= d * 86400;
		h = ti / 3600; ti -= h * 3600;
		m = ti / 60; ti -= m * 60;
		SString ret;
		if (d > 0) ret += SString::sprintf("%dd ", d);
		if (h > 0) ret += SString::sprintf("%d:", h);
		ret += SString::sprintf("%02d:%02d.%03d", m, ti, ms);
		return ret;
	}
	time_t ti = value;
	struct tm tm = Convert::localtime(ti);
	switch (fmt)
	{
	case 'T': return SString::sprintf("%04d-%02d-%02d %02d:%02d:%02d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	case 't': return SString(Convert::asctime(tm).c_str());
	case 'y': return SString::sprintf("%04d-%02d-%02d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday);
	case 'm': return SString::sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, int(1000 * (value - (double)ti)));
	}
	return SString();
}

SString ExtValue::format(const SString& fmt, const ExtValue **values, int count)
{
	SString ret;
	// "..........%.........%..........%........"
	//  ^_cur     ^_next
	//  ^^^^^^^^^^___sub
	//
	// "..........%.........%..........%........"
	//            ^-cur     ^-next
	//            ^^^^^^^^^^___sub
	const char* begin = fmt.c_str(), *end = begin + fmt.len(), *curr = begin;
	int type = 0;

	class Args
	{
		const ExtValue **values;
		int count;
		int arg;
	public:
		Args(const ExtValue **v, int c) :values(v), count(c), arg(0) {}
		bool finished() { return arg >= count; }
		const ExtValue *getNext() { const ExtValue *ret = NULL; if ((arg < count) && values[arg]) ret = values[arg]; arg++; return ret; }
	};
	Args args(values, count);

	while (curr < end)
	{
		const char* next = strchr(curr, '%');
		if (!next) next = end; else if ((next == curr) && (curr > begin))
		{
			next = strchr(next + 1, '%'); if (!next) next = end;
		}
		type = 0;
		if (curr > begin)
		{
			type = 0;
			for (const char* t = curr; t < next; t++)
				switch (*t)
				{
				case 'd': case 'x': case 'X': case 'u': case 'p': case 'c': type = 'd'; t = next; break;
				case 'f': case 'g': case 'e': type = 'f'; t = next; break;
				case 's': type = 's'; t = next; break;
				case 't': case 'T': case 'y': case 'i': case 'm': type = *t; t = next; break;
				case '%': if (t > begin) { type = *t; t = next; } break;
				}
		}
		if (curr > begin) curr--;
		const ExtValue *a;
		if (args.finished() && (type != 0) && (type != '%'))
		{
			ret += fmt.substr((int)(curr - begin));
			break;
		}
		SString sub = fmt.substr((int)(curr - begin), (int)(next - curr));
		switch (type)
		{
		case 'd': a = args.getNext(); ret += SString::sprintf(sub.c_str(), a ? a->getInt() : 0); break;
		case 'f': a = args.getNext(); ret += SString::sprintf(sub.c_str(), a ? a->getDouble() : 0); break;
		case 's': {a = args.getNext(); SString tmp; if (a) tmp = a->getString(); ret += SString::sprintf(sub.c_str(), tmp.c_str()); } break;
		case 't': case 'T': case 'i': case 'y': case 'm':
			a = args.getNext();
			ret += formatTime(type, a ? a->getDouble() : 0);
			ret += sub.substr(2);
			break;
		case '%': ret += '%'; ret += sub.substr(2); break;
		case 0: ret += sub; break;
		}
		curr = next + 1;
	}
	return ret;
}


void ExtValue::modInt(paInt a)
{
	if (a)
		idata() %= a;
	else
	{
		logPrintf("ExtValue", "modulo", LOG_ERROR, "Modulo by zero: %d%%0", idata());
		setInvalid();
	}
}

void ExtValue::modDouble(double a)
{
	if (a == 0.0)
	{
		logPrintf("ExtValue", "modulo", LOG_ERROR, "Modulo by zero: %s%%0.0", getString().c_str());
		setInvalid();
	}
	else
		setDouble(fmod(ddata(), a));
}

void ExtValue::operator%=(const ExtValue& src)
{
	switch (type)
	{
	case TInt: modInt(src.getInt()); break;
	case TDouble: modDouble(src.getDouble()); break;

	case TString:
	{
		VectorObject *vec = VectorObject::fromObject(src.getObject(), false);
		if (vec)
			sdata() = format(sdata(), (const ExtValue**)&vec->data.getref(0), vec->data.size());
		else
		{
			const ExtValue *ptr = &src; sdata() = ExtValue::format(sdata(), &ptr, 1);
		}
	}
	break;

	case TObj: case TUnknown: case TInvalid:
		logPrintf("ExtValue", "modulo", LOG_WARN, "Can't apply modulo to %s", typeDescription().c_str());

	default:;
	}
}

bool ExtValue::parseInt(const char* s, paInt &result, bool strict, bool error)
{
	ExtValue tmp;
	const char* after = tmp.parseNumber(s, strict ? TInt : TUnknown);
	if ((after == NULL) || (after[0] != 0))
	{
		if (error)
			logPrintf("ExtValue", "parseInt", LOG_ERROR, "Could not parse '%s'%s", s, strict ? " (strict)" : "");
		return false;
	}
	result = tmp.getInt();
	return true;
}

bool ExtValue::parseDouble(const char* s, double &result, bool error)
{
	ExtValue tmp;
	const char* after = tmp.parseNumber(s, TDouble);
	if ((after == NULL) || (after[0] != 0))
	{
		if (error)
			logPrintf("ExtValue", "parseDouble", LOG_ERROR, "Could not parse '%s'", s);
		return false;
	}
	result = tmp.getDouble();
	return true;
}

paInt ExtValue::getInt(const char* s, bool strict)
{
	paInt result;
	if (parseInt(s, result, strict, true))
		return result;
	return 0;
}

double ExtValue::getDouble(const char* s)
{
	double result;
	if (parseDouble(s, result, true))
		return result;
	return 0;
}

paInt ExtValue::getInt() const
{
	switch (type)
	{
	case TInt: return idata();
	case TDouble:
	{
		bool toobig = false;
		if (((toobig = (ddata() > (double)INT_MAX)))
			|| (ddata() < (double)INT_MIN))
		{
			logPrintf("ExtValue", "getInt", LOG_ERROR, "Overflow when converting floating point value to integer; using %s allowed integer '%d' instead of '%s'", toobig ? "maximal" : "minimal", toobig ? INT_MAX : INT_MIN, getString().c_str());
			return toobig ? INT_MAX : INT_MIN;
		}
		return (int)ddata();
	}
	case TString: return getInt(sdata().c_str());
	case TObj:
		logPrintf("ExtValue", "getInt", LOG_ERROR, "Getting integer value from object reference (%s)", getString().c_str());
		return (paInt)(intptr_t)odata().param;
	case TUnknown:
	case TInvalid:
		logPrintf("ExtValue", "getInt", LOG_ERROR, "Getting integer value from %s", getString().c_str());
		return 0;
	default:;
	}
	return 0;
}

double ExtValue::getDouble() const
{
	switch (type)
	{
	case TDouble: return ddata();
	case TInt: return (double)idata();
	case TString: return getDouble(sdata().c_str());
	case TObj:
		logPrintf("ExtValue", "getDouble", LOG_ERROR, "Getting floating point value from object reference (%s)", getString().c_str());
		return (double)(intptr_t)odata().param;
	case TUnknown:
	case TInvalid:
		logPrintf("ExtValue", "getInt", LOG_ERROR, "Getting floating point value from %s", getString().c_str());
		return 0.0;
	default:;
	}
	return 0.0;
}

SString ExtValue::getString() const
{
	switch (type)
	{
	case TString: return sdata();
	case TInt: return SString::valueOf(idata());
	case TDouble: return SString::valueOf(ddata());
	case TObj: return odata().toString();
	case TInvalid: 	return SString("invalid");
	default: return SString("null");
	}
}

const SString* ExtValue::getStringPtr() const
{
	if (type == TString)
		return &sdata();
	return NULL;
}

SString ExtValue::serialize(SerializationFormat format) const
{
	switch (type)
	{
	case TString:
	{
		SString q = sdata();
		sstringQuote(q);
		return SString("\"") + q + SString("\"");
	}
	case TInt:
		return SString::valueOf(idata());
	case TDouble:
		return SString::valueOf(ddata());
	case TObj:
		return odata().serialize(format);
	case TInvalid:
		if (format == NativeSerialization)
			return SString("invalid");
		// else null --v
	default:
		return SString("null");
	}
}

/// returns the first character after the parsed number, or NULL if not a number
/// @param strict_type = restrict the allowed return value (TUnknown = unrestricted)
const char* ExtValue::parseNumber(const char* in, ExtPType strict_type)
{
	char* after;
	if (in == NULL) return NULL;
	if (in[0] == 0) return NULL;
	while (isspace(*in)) in++;
	bool minus = (in[0] == '-');
	bool plus = (in[0] == '+');
	if (((in[0] == '0') && ((in[1] == 'x') || (in[1] == 'X')))
		|| (((minus || plus) && (in[1] == '0') && ((in[2] == 'x') || (in[2] == 'X')))))
	{
		in += (minus || plus) ? 3 : 2;
		if (isspace(*in)) return NULL;
		errno = 0;
		unsigned long intvalue = strtoul(in, &after, 16);
		if ((after > in) && (errno == 0) && (intvalue <= 0xffffffff))
		{
			if (strict_type == TDouble)
				setDouble(minus ? -(double)intvalue : (double)intvalue);
			else
				setInt(minus ? -(paInt)intvalue : (paInt)intvalue);
			return after;
		}
		else
			return NULL;
	}

	errno = 0;
	double fpvalue = strtod(in, &after);
	if ((after > in) && (errno == 0))
	{
		if (strict_type != TDouble)
		{
			if ((memchr(in, '.', after - in) == NULL) && (memchr(in, 'e', after - in) == NULL) && (memchr(in, 'E', after - in) == NULL) // no "special" characters
				&& (fpvalue == floor(fpvalue)) // value is integer
				&& (fpvalue >= INT_MIN) && (fpvalue <= INT_MAX)) // within limits
			{
				setInt(fpvalue);
				return after;
			}
			else if (strict_type == TInt)
				return NULL;
		}
		setDouble(fpvalue);
		return after;
	}
	return NULL;
}

PtrListTempl<ParamInterface*> &ExtValue::getDeserializableClasses()
{
	static PtrListTempl<ParamInterface*> classes;
	return classes;
}

ParamInterface *ExtValue::findDeserializableClass(const char* name)
{
	FOREACH(ParamInterface*, cls, getDeserializableClasses())
		if (!strcmp(cls->getName(), name))
			return cls;
	return NULL;
}

static const char* skipWord(const char* in)
{
	while (isalpha(*in) || (*in == '_'))
		in++;
	return in;
}

//returns the first character after the parsed portion or NULL if invalid format
const char* ExtValue::deserialize_inner(const char* in)
{
	while (isspace(*in)) in++;
	const char* ret = parseNumber(in);
	if (ret)
		return ret;
	else if (*in == '\"')
	{
		ret = skipQuoteString(in + 1, NULL);
		SString s(in + 1, (int)(ret - (in + 1)));
		sstringUnquote(s);
		setString(s);
		if (*ret == '\"')
			return ret + 1;
		else
		{
			logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing '\"' in string: '%s'", ret);
			return NULL;
		}
	}
	else if (*in == '[')
	{
		VectorObject *vec = new VectorObject;
		ExtObject o(&VectorObject::par, vec);
		tlsGetRef(ExtObject_serialization).add(o);
		const char* p = in + 1;
		ExtValue tmp; bool first = true, comma = false;
		const char* prev_element = p;
		while (true)
		{
			while (isspace(*p)) p++;
			if (*p == 0)
			{
				logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing ']' in Vector");
				return NULL;
			}
			if (*p == ']')
			{
				if (comma)
				{
					logPrintf("ExtValue", "deserialize", LOG_ERROR, "No element after ',' in Vector");
					return NULL;
				}
				p++;
				break;
			}
			if (!first && !comma)
			{
				logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing ',' in Vector: '%s'", prev_element);
				return NULL;
			}
			ret = tmp.deserialize(p);
			prev_element = p; first = false; comma = false;
			if (ret)
			{
				vec->data += new ExtValue(tmp);
				p = ret;
				if (*p == ',') { p++; comma = true; }
			}
			else
			{
				p = NULL;
				break;
			}
		}
		setObject(o);
		return p;
	}
	else if (*in == '{')
	{
		DictionaryObject *dic = new DictionaryObject;
		ExtObject o(&DictionaryObject::par, dic);
		tlsGetRef(ExtObject_serialization).add(o);
		const char* p = in + 1;
		ExtValue args[2]/*={value,key}*/, dummy_ret;
		bool first = true, comma = false;
		const char* prev_element = p;
		while (true)
		{
			while (isspace(*p)) p++;
			if (*p == 0)
			{
				logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing '}' in Dictionary");
				return NULL;
			}
			if (*p == '}')
			{
				if (comma)
				{
					logPrintf("ExtValue", "deserialize", LOG_ERROR, "No element after ',' in Dictionary");
					return NULL;
				}
				p++;
				break;
			}
			if (!first && !comma)
			{
				logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing ',' in Dictionary: '%s'", prev_element);
				return NULL;
			}
			ret = args[1].deserialize(p);
			prev_element = p; first = false; comma = false;
			if ((!ret) || (args[1].getType() != TString)) { p = NULL; break; }
			p = ret;
			if (*p != ':') { logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing ':' in Dictionary: '%s'", p); p = NULL; break; }
			p++;
			ret = args[0].deserialize(p);
			if (!ret) { p = NULL; break; }
			p = ret;
			dic->p_set(args, &dummy_ret);
			if (*p == ',') { p++; comma = true; }
		}
		setObject(o);
		return p;
	}
	else if (!strncmp(in, "null", 4))
	{
		setEmpty();
		return in + 4;
	}
	else if (!strncmp(in, "true", 4))
	{
		setInt(1);
		return in + 4;
	}
	else if (!strncmp(in, "false", 5))
	{
		setInt(0);
		return in + 5;
	}
	else if (!strncmp(in, "invalid", 7))
	{
		setInvalid();
		return in + 7;
	}
	else if (*in == '<')
	{ //unserializable object
		const char* end = in + 1;
		while (*end)
			if (*end == '>')
			{
				setError(SString("Unserializable class: ") + SString(in + 1, end - in - 1));
				return end + 1;
			}
			else
				end++;
		logPrintf("ExtValue", "deserialize", LOG_ERROR, "Missing '>'");
		return NULL;
	}
	else if (*in == '^')
	{
		in++;
		ExtValue ref;
		ret = ref.parseNumber(in, TInt);
		if (ret && (ref.getType() == TInt))
		{
			const ExtObject* o = tlsGetRef(ExtObject_serialization).get(ref.getInt());
			if (o)
			{
				setObject(*o);
				return ret;
			}
		}
		logPrintf("ExtValue", "deserialize", LOG_ERROR, "Invalid reference: '%s'", in - 1);
		return NULL;
	}
	else if ((ret = skipWord(in)) && (ret != in))
	{
		SString clsname(in, (int)(ret - in));
		ExtValue tmp;
		ret = tmp.deserialize(ret);
		ParamInterface *cls = findDeserializableClass(clsname.c_str());
		if (cls && (tmp.getType() != TUnknown) && (tmp.getType() != TInvalid))
		{
			VectorObject *vec = VectorObject::fromObject(tmp.getObject(), false);
			if (vec)
			{
				int m = cls->findId("newFromVector");
				if (m >= 0)
				{
					cls->call(m, &tmp, this);
					tlsGetRef(ExtObject_serialization).replace(tmp.getObject(), getObject());
					return ret;
				}
			}
			DictionaryObject *dic = DictionaryObject::fromObject(tmp.getObject(), false);
			if (dic)
			{
				int m = cls->findId("newFromDictionary");
				if (m >= 0)
				{
					cls->call(m, &tmp, this);
					tlsGetRef(ExtObject_serialization).replace(tmp.getObject(), getObject());
					return ret;
				}
			}
			if (tmp.getType() == TString)
			{
				int m = cls->findId("newFromString");
				if (m >= 0)
				{
					cls->call(m, &tmp, this);
					tlsGetRef(ExtObject_serialization).replace(tmp.getObject(), getObject());
					return ret;
				}
			}
			tlsGetRef(ExtObject_serialization).remove(tmp.getObject());
			setEmpty();
		}
		setEmpty();
		logPrintf("ExtValue", "deserialize", LOG_WARN, "object of class \"%s\" could not be deserialized", clsname.c_str());
		return ret;
	}
	logPrintf("ExtValue", "deserialize", LOG_ERROR, "Bad syntax: '%s'", in);
	setEmpty();
	return NULL;
}

const char* ExtValue::deserialize(const char* in)
{
	tlsGetRef(ExtObject_serialization).begin();
	const char* ret = deserialize_inner(in);
	if (ret) while (isspace(*ret)) ret++;
	tlsGetRef(ExtObject_serialization).end();
	return ret;
}

ExtObject ExtValue::getObject() const
{
	if (type == TObj) return odata();
	return ExtObject();
}

ExtValue ExtValue::getExtType()
{
	static const char* typenames[] = { "null", "int", "float", "string", "", "invalid" };
	if (getType() != TObj)
		return ExtValue(typenames[(int)getType()]);
	ExtObject& o = odata();
	return ExtValue(SString(o.isEmpty() ? "" : o.interfaceName()));
}

SString SString::valueOf(const ExtValue& v)
{
	return v.getString();
}
SString SString::valueOf(const ExtObject& v)
{
	return v.toString();
}

#define FIELDSTRUCT ErrorObject
static ParamEntry errorobject_paramtab[] =
{
	{ "Error", 1, 3, "Error", },
	{ "message", 0, 0, "Message", "s", FIELD(message), },
	{ "newFromString", 0, 0, "create new object", "p oError(s message)", PROCEDURE(p_newfromstring), },
	{ "toString", 0, PARAM_READONLY | PARAM_NOSTATIC, "Textual form", "s", GETONLY(toString), },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

Param& ErrorObject::getParam()
{
	static Param param(errorobject_paramtab);
	return param;
}

ExtObject ErrorObject::makeDynamicObject(ErrorObject* e)
{
	return ExtObject(&getParam(), (DestrBase*)e);
}

const SString ErrorObject::TO_STRING_PREFIX = "Error: ";

void ErrorObject::get_toString(ExtValue* ret)
{
	ret->setString(TO_STRING_PREFIX + message);
}

void ErrorObject::p_newfromstring(ExtValue *args, ExtValue *ret)
{
	ErrorObject *err = new ErrorObject();
	err->message = args[0].getString();
	if (err->message.startsWith(TO_STRING_PREFIX.c_str()))
		err->message = err->message.substr(TO_STRING_PREFIX.len());
	*ret = makeDynamicObject(err);
}

REGISTER_DESERIALIZABLE(ErrorObject)
