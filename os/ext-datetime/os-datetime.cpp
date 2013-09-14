#include "os-datetime.h"
#include "../objectscript.h"
#include "../os-binder.h"

// #include <boost/date_time/gregorian/gregorian.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>

// using boost::gregorian;
// using boost::posix_time;

#include "mx\mxDateTime.h"

namespace ObjectScript {

class DateTimeOS: public OS
{
public:

	static void triggerError(OS * os, const OS::String& msg)
	{
		os->getGlobal(OS_TEXT("DateTimeException"));
		os->pushGlobals();
		os->pushString(msg);
		os->call(1, 1);
		os->setException();
	}

	static void triggerError(OS * os, const char * msg)
	{
		os->getGlobal(OS_TEXT("DateTimeException"));
		os->pushGlobals();
		os->pushString(msg);
		os->call(1, 1);
		os->setException();
	}

	static void initLibrary(OS * os);

	struct DateTime
	{
		boost::posix_time::ptime time;

		DateTime(const boost::posix_time::ptime& p_time): time(p_time)
		{
		}

		std::string toString()
		{
			return to_simple_string(time);
		}

		static int __construct(OS * os, int params, int, int, void * user_param);
		static int now(OS * os, int params, int, int, void * user_param);
	};

};

template <> struct CtypeName<DateTimeOS::DateTime>{ static const OS_CHAR * getName(){ return OS_TEXT("DateTime"); } };
template <> struct CtypeValue<DateTimeOS::DateTime*>: public CtypeUserClass<DateTimeOS::DateTime*>{};
template <> struct UserDataDestructor<DateTimeOS::DateTime>
{
	static void dtor(ObjectScript::OS * os, void * data, void * user_param)
	{
		OS_ASSERT(data && dynamic_cast<DateTimeOS::DateTime*>((DateTimeOS::DateTime*)data));
		DateTimeOS::DateTime * buf = (DateTimeOS::DateTime*)data;
		buf->~DateTime();
		os->free(buf);
	}
};

int DateTimeOS::DateTime::now(OS * os, int params, int, int, void * user_param)
{
	boost::posix_time::ptime res = boost::posix_time::microsec_clock::local_time();
	// boost::posix_time::ptime r(date, time);
	DateTime * dt = new (os->malloc(sizeof(DateTime) OS_DBG_FILEPOS)) DateTime(res);
	pushCtypeValue(os, dt);
	return 1;
}

int DateTimeOS::DateTime::__construct(OS * os, int params, int, int, void * user_param)
{
	if(params < 1){
		// boost::gregorian::date date = boost::date_time::day_clock::local_day();
		boost::posix_time::ptime res = boost::posix_time::microsec_clock::local_time();
		// boost::posix_time::ptime r(date, time);
		DateTime * dt = new (os->malloc(sizeof(DateTime) OS_DBG_FILEPOS)) DateTime(res);
		pushCtypeValue(os, dt);
		return 1;
	}
	boost::gregorian::greg_year year = os->toInt(-params+0);
	boost::gregorian::greg_month month = params >= 2 ? os->toInt(-params+1) : 1;
	boost::gregorian::greg_day day = params >= 3 ? os->toInt(-params+2) : 1;
	long hours = params >= 4 ? os->toInt(-params+3) : 0;
	long minutes = params >= 5 ? os->toInt(-params+4) : 0;
	long seconds = params >= 6 ? os->toInt(-params+5) : 0;
	double millisec = params >= 7 ? os->toDouble(-params+6) : 0;
	long sec_frac = (long)(millisec * boost::posix_time::time_duration::ticks_per_second() * 0.001);
			
	boost::gregorian::date date(year, month, day);
	boost::posix_time::time_duration time(hours, minutes, seconds, sec_frac);
	boost::posix_time::ptime res(date, time);
			
	DateTime * dt = new (os->malloc(sizeof(DateTime) OS_DBG_FILEPOS)) DateTime(res);
	pushCtypeValue(os, dt);
	return 1;
}

void DateTimeOS::initLibrary(OS * os)
{
	{
		OS::FuncDef funcs[] = {
			{OS_TEXT("__construct"), DateTime::__construct},
			{OS_TEXT("now"), DateTime::now},
			def(OS_TEXT("valueOf"), DateTime::toString),
			{}
		};
		registerUserClass<DateTime>(os, funcs);
	}
}

void initDateTimeLibrary(OS* os)
{
	DateTimeOS::initLibrary(os);
}

} // namespace ObjectScript

