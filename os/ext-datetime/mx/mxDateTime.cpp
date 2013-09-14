#define _CRT_SECURE_NO_WARNINGS

#include "../../objectscript.h"

/* Some additional switches are needed on some platforms to make
strptime() and timegm() available. */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

#include "mxDateTime.h"

# if defined(HAVE_SYS_TIME_H) && defined(TIME_WITH_SYS_TIME)
#  include <sys/time.h>
#  include <time.h>
# else
#  include <time.h>
# endif
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# ifdef HAVE_FTIME
#  include <sys/timeb.h>
# endif

/* We need floor() and ceil() for ticks conversions. */
#include <math.h>

/* The module makes use of two functions called strftime() and
strptime() for the conversion between strings and date/time
values. Since not all C compilers know about these functions,
you can turn these features on/off be defining the following
symbols (if you ran the Python configuration script then it will
provide this information for you -- on other platforms than
Unix you may have to define them manually). */
/*#define HAVE_STRFTIME*/
/*#define HAVE_STRPTIME*/
/*#define HAVE_TIMEGM*/

/* The start size for output from strftime. */
#define STRFTIME_OUTPUT_SIZE	1024

/* Define to have the seconds part rounded when assigning to tm_sec in
tm structs; note that rounding can result in the seconds part to be
displayed as 60 instead of wrapping to the next minute, hour,
etc. */
/*#define ROUND_SECONDS_IN_TM_STRUCT*/

/* Seconds in a day (as double) */
#define SECONDS_PER_DAY ((double) 86400.0)

/* abstime value limit (as double). The limit itself does not belong
to the range of accepted values. Includes one leap second per
day. */
#define MAX_ABSTIME_VALUE ((double) 86401.0)

/* Test for negativeness of doubles */
#define DOUBLE_IS_NEGATIVE(x) ((x) < (double) 0.0)

static int mxDateTime_POSIXConform = 0;		/* Does the system use POSIX
											time_t values ? */

static int mxDateTime_DoubleStackProblem = 0;	/* Does the system
												have a problem
												passing doubles on
												the stack ? */

/* Table with day offsets for each month (0-based, without and with leap) */
static int month_offset[2][13] = {
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/* Table of number of days in a month (0-based, without and with leap) */
static int days_in_month[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/* Flag telling us whether the module was initialized or not. */
static int mxDateTime_Initialized = 0;

enum {
	mxDateTime_Error,
	mxDateTime_RangeError,
	PyExc_SystemError,
	PyExc_TypeError
};

void triggerError(int t, const char * m)
{
}

void triggerError(int t, const char * m, int v1)
{
}

#define Py_Error(t, m) do{ triggerError(t, m); goto onError; }while(false)
#define Py_Assert(a, t, m) if(!(a)) Py_Error(t, m)
#define Py_AssertWithArg(a, t, m, v1) if(!(a)) do{ triggerError(t, m, v1); goto onError; }while(false)


#define PyErr_BadInternalCall()
#define PyErr_Occurred() 0
#define DPRINTF(...)

static bool _mxDateTime_Check(mxDateTimeObject *)
{
	return true;
}

static bool _mxDateTimeDelta_Check(mxDateTimeDeltaObject *)
{
	return true;
}

/* Returns the current time in Unix ticks.

The function tries to use the most accurate API available on the
system.

-1.0 is returned in case of an error.

*/

static double mxDateTime_GetCurrentTime(void)
{
# if defined(HAVE_CLOCK_GETTIME)

	/* Use clock_gettime(), which has ns resolution */
	struct timespec ts;

	if (!clock_gettime(CLOCK_REALTIME, &ts))
		return ((double)ts.tv_sec + (double)ts.tv_nsec * 1e-9);
	else
		return -1.0;

# elif defined(HAVE_GETTIMEOFDAY)

	/* Use gettimeofday(), which has us resolution */
	struct timeval tv;

#  ifdef GETTIMEOFDAY_NO_TZ
	if (!gettimeofday(&tv))
#  else
	if (!gettimeofday(&tv, 0))
#  endif
		return ((double)tv.tv_sec + (double)tv.tv_usec * 1e-6);
	else
		return -1.0;

# elif defined(HAVE_FTIME)

	/* Use ftime(), which provides ms resolution */
	struct timeb tb;

	ftime(&tb);
	return ((double)tb.time + (double)tb.millitm * 1e-3);

# else

	/* Use time(), which usually only has seconds resolution */
	time_t ticks;

	time(&ticks);
	return (double) ticks;

# endif
}

/* Try to determine the clock resolution. 

*/

static double mxDateTime_GetClockResolution(void)
{
# if defined(HAVE_CLOCK_GETTIME)
#  if defined(HAVE_CLOCK_GETRES)

	/* clock_gettime() is supposed to have ns resolution, but apparently
	this is not true on all systems. */
	struct timespec ts;

	if (!clock_getres(CLOCK_REALTIME, &ts))
		return ((double)ts.tv_sec + (double)ts.tv_nsec * 1e-9);
	else
		return -1.0;
#  else
	/* We'll have to believe the man-page */
	return 1e-9;
#  endif

# elif defined(HAVE_GETTIMEOFDAY)

	/* gettimeofday() has us resolution according to the man-page */
	return 1e-6;

# elif defined(HAVE_FTIME)

	/* ftime() provides ms resolution according to the man-page*/
	return 1e-3;

# else

	/* time() usually only has seconds resolution */
	return 1.0;

# endif
}

/* Fix a second value for display as string.

Seconds are rounded to the nearest microsecond in order to avoid
cases where e.g. 3.42 gets displayed as 03.41 or 3.425 is diplayed
as 03.42.

Special care is taken for second values which would cause rounding
to 60.00 -- these values are truncated to 59.99 to avoid the value
of 60.00 due to rounding to show up even when the indictated time
does not point to a leap second. The same is applied for rounding
towards 61.00 (leap seconds).

The second value returned by this function should be formatted
using '%05.2f' (which rounds to 2 decimal places).

*/

static double mxDateTime_FixSecondDisplay(double second)
{
	/* Special case for rounding towards 60. */
	if (second >= 59.995 && second < 60.0)
		return 59.99;

	/* Special case for rounding towards 61. */
	if (second >= 60.995 && second < 61.0)
		return 60.99;

	/* Round to the nearest microsecond */
	second = (second * 1e6 + 0.5) / 1e6;

	return second;
}

/* This function checks whether the system uses the POSIX time_t rules
(which do not support leap seconds) or a time package with leap
second support enabled. Return 1 if it uses POSIX time_t values, 0
otherwise.

POSIX: 1986-12-31 23:59:59 UTC == 536457599

With leap seconds:		  == 536457612

(since there were 13 leapseconds in the years 1972-1985 according
to the tz package available from ftp://elsie.nci.nih.gov/pub/)

*/

static int mxDateTime_POSIX(void)
{
	time_t ticks = 536457599;
	struct tm *tm;

	memset(&tm,0,sizeof(tm));
	tm = gmtime(&ticks);
	if (tm == NULL)
		return 0;
	if (tm->tm_hour == 23 &&
		tm->tm_min == 59 &&
		tm->tm_sec == 59 &&
		tm->tm_mday == 31 &&
		tm->tm_mon == 11 &&
		tm->tm_year == 86)
		return 1;
	else
		return 0;
}

static int mxDateTime_CheckDoubleStackProblem(double value)
{
	if (value == SECONDS_PER_DAY)
		return 1;
	else
		return 0;
}

#ifndef HAVE_TIMEGM
/* Calculates the conversion of the datetime instance to Unix ticks.

For instances pointing to localtime, localticks will hold the
corresponding Unix ticks value. In case the instance points to GMT
time, gmticks will hold the correct ticks value.

In both cases, gmtoffset will hold the GMT offset (local-GMT).

Returns -1 (and sets an exception) to indicate errors; 0
otherwise. 

Note:

There's some integer rounding error in the mktime() function that
triggers near MAXINT on Solaris. The error was reported by Joe Van
Andel <vanandel@ucar.edu> among others:

Ooops: 2038-01-18 22:52:31.00 t = 2147467951 diff = -4294857600.0

On 64-bit Alphas running DEC OSF, Tony Ibbs <tony@lsl.co.uk>
reports:

Ooops: 1901-12-13 21:57:57.00 t = 2147487973 diff = -4294967296.0
...(the diffs stay the same)...
Ooops: 1969-12-31 10:10:54.00 t = 4294917550 diff = -4294967296.0

Note the years ! Some rollover is happening near 2^31-1 even
though Alphas happen to use 64-bits. This could be a bug in this
function or in DEC's mktime() implementation.

*/

static int mxDateTime_CalcTicks(mxDateTimeObject *datetime,
	double *localticks,
	double *gmticks,
	double *gmtoffset)
{
	struct tm tm;
	struct tm *gmt;
	time_t ticks;
	double offset;

	Py_Assert(datetime->calendar == MXDATETIME_GREGORIAN_CALENDAR,
		mxDateTime_Error,
		"can only convert the Gregorian calendar to ticks");
	Py_Assert((long)((int)datetime->year) == datetime->year,
		mxDateTime_RangeError,
		"year out of range for ticks conversion");

	/* Calculate floor()ed ticks value  */
	memset(&tm,0,sizeof(tm));
	tm.tm_hour = (int)datetime->hour;
	tm.tm_min = (int)datetime->minute;
	tm.tm_sec = (int)datetime->second;
	tm.tm_mday = (int)datetime->day;
	tm.tm_mon = (int)datetime->month - 1;
	tm.tm_year = (int)datetime->year - 1900;
	tm.tm_wday = -1;
	tm.tm_yday = (int)datetime->day_of_year - 1;
	tm.tm_isdst = -1; /* unknown */
	ticks = mktime(&tm);
	if (ticks == (time_t)-1 && tm.tm_wday == -1) {
		/* XXX Hack to allow conversion during DST switching. */
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		ticks = mktime(&tm);
		if (ticks == (time_t)-1 && tm.tm_wday == -1)
			Py_Error(mxDateTime_Error,
			"cannot convert value to a Unix ticks value");
		ticks += ((int)datetime->hour * 3600
			+ (int)datetime->minute * 60
			+ (int)datetime->second);
	}

	/* Add fraction for localticks */
	*localticks = ((double)ticks
		+ (datetime->abstime - floor(datetime->abstime)));

	/* Now compare local time and GMT time */
	gmt = gmtime(&ticks);
	if (gmt == NULL)
		Py_Error(mxDateTime_Error,
		"cannot convert value to a Unix ticks value");

	/* Check whether we have the same day and prepare offset */
	if (gmt->tm_mday != tm.tm_mday) {
		double localdate = (tm.tm_year * 10000 + 
			tm.tm_mon *  100 +
			tm.tm_mday);
		double gmdate = (gmt->tm_year * 10000 +
			gmt->tm_mon * 100 + 
			gmt->tm_mday);
		if (localdate < gmdate)
			offset = -SECONDS_PER_DAY;
		else
			offset = SECONDS_PER_DAY;
	}
	else
		offset = 0.0;

	/* Calculate difference in seconds */
	offset += ((datetime->hour - gmt->tm_hour) * 3600.0
		+ (datetime->minute - gmt->tm_min) * 60.0
		+ (floor(datetime->second) - (double)gmt->tm_sec));
	*gmtoffset = offset;
	*gmticks = *localticks + offset;
	return 0;

onError:
	return -1;
}
#endif

/* These functions work for positive *and* negative years for
compilers which round towards zero and ones that always round down
to the nearest integer. */

/* Return 1/0 iff year points to a leap year in calendar. */

static int mxDateTime_Leapyear(long year, int calendar)
{
	if (calendar == MXDATETIME_GREGORIAN_CALENDAR)
		return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
	else
		return (year % 4 == 0);
}

/* Return the day of the week for the given absolute date. */

static int mxDateTime_DayOfWeek(long absdate)
{
	int day_of_week;
	if (absdate >= 1)
		day_of_week = (absdate - 1) % 7;
	else
		day_of_week = 6 - ((-absdate) % 7);
	return day_of_week;
}

/* Return the year offset, that is the absolute date of the day
31.12.(year-1) in the given calendar.

For the Julian calendar we shift the absdate (which is measured
using the Gregorian Epoch) value by two days because the Epoch
(0001-01-01) in the Julian calendar lies 2 days before the Epoch in
the Gregorian calendar.

Years around the Epoch (mathematical approach, not necessarily
historically correct):

Year  0005 (5 AD) - yearoffset: 1461,not a leap year
Year  0004 (4 AD) - yearoffset: 1095,leap year
Year  0003 (3 AD) - yearoffset: 730, not a leap year
Year  0002 (2 AD) - yearoffset: 365, not a leap year
Year  0001 (1 AD) - yearoffset: 0, not a leap year
Year  0000 (1 BC) - yearoffset: -366, leap year
Year -0001 (2 BC) - yearoffset: -731, not a leap year
Year -0002 (3 BC) - yearoffset: -1096, not a leap year
Year -0003 (4 BC) - yearoffset: -1461, not a leap year
Year -0004 (5 BC) - yearoffset: -1827, leap year
Year -0005 (6 BC) - yearoffset: -2192, not a leap year

*/

static long mxDateTime_YearOffset(long year, int calendar)
{
	if (year >= 1) {
		/* For years >= 1, we can simply count the number of days
		between the Epoch and the given year */
		year--;
		if (calendar == MXDATETIME_GREGORIAN_CALENDAR)
			return year*365 + year/4 - year/100 + year/400;

		else if (calendar == MXDATETIME_JULIAN_CALENDAR)
			return year*365 + year/4 - 2;

		else {
			Py_Error(mxDateTime_Error,
				"unknown calendar");
		}
	} else {
		/* For years <= 0, we need to reverse the sign of the year (to
		avoid integer rounding issues with negative numbers on some
		platforms) and compensate for the year 0 being a leap
		year */
		year = -year;
		if (calendar == MXDATETIME_GREGORIAN_CALENDAR)
			return -(year*365 + year/4 - year/100 + year/400) - 366;

		else if (calendar == MXDATETIME_JULIAN_CALENDAR)
			return -(year*365 + year/4) - 366 - 2;

		else {
			Py_Error(mxDateTime_Error,
				"unknown calendar");
		}
	}

onError:
	return -1;
}


/* Normalize the data and calculate the absolute date, year offset and
whether the year is a leap year or not.

Returns -1 in case of an error, 0 otherwise.

*/

static int mxDateTime_NormalizedDate(long year,
	int month,
	int day,
	int calendar,
	long *absdate_output,
	long *yearoffset_output,
	int *leap_output,
	long *normalized_year,
	int *normalized_month,
	int *normalized_day)
{
	int leap;
	long yearoffset, absdate;

	/* Range check */
	Py_AssertWithArg(year > -(LONG_MAX / 366) && year < (LONG_MAX / 366),
		mxDateTime_RangeError,
		"year out of range: %ld",
		year);

	/* Is it a leap year ? */
	leap = mxDateTime_Leapyear(year, calendar);

	/* Negative month values indicate months relative to the years end */
	if (month < 0)
		month += 13;
	Py_AssertWithArg(month >= 1 && month <= 12,
		mxDateTime_RangeError,
		"month out of range (1-12): %i",
		month);

	/* Negative values indicate days relative to the months end */
	if (day < 0)
		day += days_in_month[leap][month - 1] + 1;
	Py_AssertWithArg(day >= 1 && day <= days_in_month[leap][month - 1],
		mxDateTime_RangeError,
		"day out of range: %i",
		day);

	yearoffset = mxDateTime_YearOffset(year, calendar);
	if (yearoffset == -1 && PyErr_Occurred())
		goto onError;

	absdate = day + month_offset[leap][month - 1] + yearoffset;

	DPRINTF("mxDateTime_NormalizedDate: "
		"year=%ld month=%i day=%i yearoffset=%ld leap=%i absdate=%ld\n",
		year, month, day, yearoffset, leap, absdate);

	if (absdate_output)
		*absdate_output = absdate;
	if (yearoffset_output)
		*yearoffset_output = yearoffset;
	if (leap_output)
		*leap_output = leap;
	if (normalized_year)
		*normalized_year = year;
	if (normalized_month)
		*normalized_month = month;
	if (normalized_day)
		*normalized_day = day;
	return 0;

onError:
	return -1;
}


#ifdef HAVE_PYDATETIME

/* Note: This API is currently only used to support datetime module
interaction */

/* Return the absolute date of the given date in absdate.

Returns -1 in case of an error, 0 otherwise.

*/

static int mxDateTime_AbsDate(long year,
	register int month,
	register int day,
	int calendar,
	long *absdate)
{
	return mxDateTime_NormalizedDate(year, month, day,
		calendar, absdate,
		NULL, NULL,
		NULL, NULL, NULL);
}

#endif

/* Sets the date part of the DateTime object using the indicated
calendar. 

XXX This could also be done using some integer arithmetics rather
than with this iterative approach...

*/

static int mxDateTime_SetFromAbsDate(mxDateTimeObject *datetime,
	long absdate,
	int calendar)
{
	long year;
	long yearoffset;
	int leap, dayoffset;
	int *monthoffset;

	DPRINTF("mxDateTime_SetFromAbsDate(datetime=%x,absdate=%ld,calendar=%i)\n",
		datetime,absdate,calendar);

	/* Approximate year */
	if (calendar == MXDATETIME_GREGORIAN_CALENDAR)
		year = (long)(((double)absdate) / 365.2425);
	else if (calendar == MXDATETIME_JULIAN_CALENDAR)
		year = (long)(((double)absdate) / 365.25);
	else
		Py_Error(mxDateTime_Error,
		"unknown calendar");
	if (absdate > 0)
		year++;

	/* Apply corrections to reach the correct year */
	while (1) {
		/* Calculate the year offset */
		yearoffset = mxDateTime_YearOffset(year, calendar);
		if (yearoffset == -1 && PyErr_Occurred())
			goto onError;
		DPRINTF(" trying year = %ld yearoffset = %ld\n",
			year, yearoffset);

		/* Backward correction: absdate must be greater than the
		yearoffset */
		if (yearoffset >= absdate) {
			year--;
			DPRINTF(" backward correction\n");
			continue;
		}

		dayoffset = absdate - yearoffset;
		leap = mxDateTime_Leapyear(year, calendar);

		/* Forward correction: years only have 365/366 days */
		if (dayoffset > 365) {
			if (leap && dayoffset > 366) {
				year++;
				DPRINTF(" forward correction (leap year)\n");
				continue;
			}
			else if (!leap) {
				year++;
				DPRINTF(" forward correction (non-leap year)\n");
				continue;
			}
		}

		/* Done */
		DPRINTF(" using year = %ld leap = %i dayoffset = %i\n",
			year,leap,dayoffset);

		break;
	}

	datetime->year = year;
	datetime->calendar = calendar;

	/* Now iterate to find the month */
	monthoffset = month_offset[leap];
	{
		int month;
		for (month = 1; month < 13; month++)
			if (monthoffset[month] >= dayoffset)
				break;
		datetime->month = month;
		datetime->day = dayoffset - month_offset[leap][month-1];
	}

	datetime->day_of_week = mxDateTime_DayOfWeek(absdate);
	datetime->day_of_year = dayoffset;

	return 0;

onError:
	return -1;
}

/* Sets the time part of the DateTime object. */

static
	int mxDateTime_SetFromAbsTime(mxDateTimeObject *datetime,
	double abstime)
{
	int inttime;
	int hour,minute;
	double second;

	DPRINTF("mxDateTime_SetFromAbsTime(datetime=%x,abstime=%.20f)\n",
		(long)datetime,abstime);

	inttime = (int)abstime;
	if (inttime == 86400) {
		/* Special case for leap seconds */
		hour = 23;
		minute = 59;
		second = 60.0 + abstime - (double)inttime;
	}
	else {
		hour = inttime / 3600;
		minute = (inttime % 3600) / 60;
		second = abstime - (double)(hour*3600 + minute*60);
	}

	datetime->hour = hour;
	datetime->minute = minute;
	datetime->second = second;

	return 0;
}

/* Set the instance's value using the given date and time. calendar
may be set to the flags: MXDATETIME_GREGORIAN_CALENDAR,
MXDATETIME_JULIAN_CALENDAR to indicate the calendar to be used. */

static
	int mxDateTime_SetFromDateAndTime(mxDateTimeObject *datetime,
	long year,
	int month,
	int day,
	int hour,
	int minute,
	double second,
	int calendar)
{
	double comdate;

	if (datetime == NULL) {
		PyErr_BadInternalCall();
		goto onError;
	}

	DPRINTF("mxDateTime_SetFromDateAndTime("
		"datetime=%x year=%ld month=%i day=%i "
		"hour=%i minute=%i second=%f calendar=%i)\n",
		datetime,year,month,day,hour,minute,second,calendar);

	/* Calculate the absolute date */
	{
		long yearoffset,absdate;

		if (mxDateTime_NormalizedDate(year, month, day, 
			calendar,
			&absdate, &yearoffset, NULL,
			&year, &month, &day))
			goto onError;
		DPRINTF("mxDateTime_SetFromDateAndTime: "
			"yearoffset=%ld absdate=%ld "
			"year=%ld month=%i day=%i (normalized)\n",
			yearoffset,absdate,
			year,month,day);

		datetime->absdate = absdate;

		datetime->year = year;
		datetime->month = month;
		datetime->day = day;

		datetime->day_of_week = mxDateTime_DayOfWeek(absdate);
		datetime->day_of_year = (short)(absdate - yearoffset);

		datetime->calendar = calendar;

		comdate = (double)absdate - 693594.0;
	}

	/* Calculate the absolute time */
	{
		Py_AssertWithArg(hour >= 0 && hour <= 23,
			mxDateTime_RangeError,
			"hour out of range (0-23): %i",
			hour);
		Py_AssertWithArg(minute >= 0 && minute <= 59,
			mxDateTime_RangeError,
			"minute out of range (0-59): %i",
			minute);
		Py_AssertWithArg(second >= (double)0.0 && 
			(second < (double)60.0 || 
			(hour == 23 && minute == 59 && 
			second < (double)61.0)),
			mxDateTime_RangeError,
			"second out of range (0.0 - <60.0; <61.0 for 23:59): %i",
			(int)second);

		datetime->abstime = (double)(hour*3600 + minute*60) + second;

		datetime->hour = hour;
		datetime->minute = minute;
		datetime->second = second;

		if (DOUBLE_IS_NEGATIVE(comdate))
			comdate -= datetime->abstime / SECONDS_PER_DAY;
		else
			comdate += datetime->abstime / SECONDS_PER_DAY;
		datetime->comdate = comdate;
	}
	return 0;
onError:
	return -1;
}

/* Set the instance's value using the given absolute date and
time. The calendar used is the Gregorian. */

static
	int mxDateTime_SetFromAbsDateTime(mxDateTimeObject *datetime,
	long absdate,
	double abstime,
	int calendar)
{
	if (datetime == NULL) {
		PyErr_BadInternalCall();
		goto onError;
	}

	DPRINTF("mxDateTime_SetFromAbsDateTime(datetime=%x,"
		"absdate=%ld,abstime=%.20f,calendar=%i)\n",
		datetime,absdate,abstime,calendar);
	DPRINTF("mxDateTime_SetFromAbsDateTime: "
		"abstime is %.20f, diff %.20f, as int %i\n", 
		abstime,
		abstime - SECONDS_PER_DAY,
		(int)abstime);

	/* Bounds check */
	Py_AssertWithArg(abstime >= 0.0 && abstime < MAX_ABSTIME_VALUE,
		mxDateTime_RangeError,
		"abstime out of range (0.0 - <86401.0): %i",
		(int)abstime);

	datetime->absdate = absdate;
	datetime->abstime = abstime;

	/* Calculate COM date */
	{
		double comdate;

		comdate = (double)(datetime->absdate - 693594);
		if (DOUBLE_IS_NEGATIVE(comdate))
			comdate -= datetime->abstime / SECONDS_PER_DAY;
		else
			comdate += datetime->abstime / SECONDS_PER_DAY;
		datetime->comdate = comdate;
	}

	/* Calculate the date */
	if (mxDateTime_SetFromAbsDate(datetime,
		datetime->absdate,
		calendar))
		goto onError;

	/* Calculate the time */
	if (mxDateTime_SetFromAbsTime(datetime,
		datetime->abstime))
		goto onError;

	return 0;
onError:
	return -1;
}

/* Set the instance's value using the given Windows COM date.  The
calendar used is the Gregorian. */

static
	int mxDateTime_SetFromCOMDate(mxDateTimeObject *datetime,
	double comdate)
{
	long absdate;
	double abstime;

	if (datetime == NULL) {
		PyErr_BadInternalCall();
		goto onError;
	}

	datetime->comdate = comdate;

	/* XXX should provide other means to calculate the broken down
	values for these huge values. */
	Py_AssertWithArg(-(double)LONG_MAX <= comdate &&
		comdate <= (double)LONG_MAX,
		mxDateTime_RangeError,
		"DateTime COM date out of range: %i",
		(int)comdate);

	absdate = (long)comdate;
	abstime = (comdate - (double)absdate) * SECONDS_PER_DAY;
	if (DOUBLE_IS_NEGATIVE(abstime))
		abstime = -abstime;
	absdate += 693594;

	DPRINTF("mxDateTime_SetFromCOMDate: absdate=%ld abstime=%f\n",
		absdate,abstime);

	datetime->absdate = absdate;
	datetime->abstime = abstime;

	/* Calculate the date */
	if (mxDateTime_SetFromAbsDate(datetime,
		absdate,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	/* Calculate the time */
	if (mxDateTime_SetFromAbsTime(datetime,
		abstime))
		goto onError;

	return 0;
onError:
	return -1;
}

mxDateTimeObject * mxDateTime_New()
{
	return new mxDateTimeObject();
}

void mxDateTime_Free(mxDateTimeObject * dt)
{
	delete dt;
}

/* --- API functions --- */

static
	mxDateTimeObject *mxDateTime_FromDateAndTime(long year,
	int month,
	int day,
	int hour,
	int minute,
	double second)
{
	mxDateTimeObject *datetime;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	if (mxDateTime_SetFromDateAndTime(datetime,
		year,month,day,
		hour,minute,second,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;
onError:
	mxDateTime_Free(datetime);
	return NULL;
}

/* Alias */
#define mxDateTime_FromGregorianDateAndTime mxDateTime_FromDateAndTime

static
	mxDateTimeObject *mxDateTime_FromJulianDateAndTime(long year,
	int month,
	int day,
	int hour,
	int minute,
	double second)
{
	mxDateTimeObject *datetime;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	if (mxDateTime_SetFromDateAndTime(datetime,
		year,month,day,
		hour,minute,second,
		MXDATETIME_JULIAN_CALENDAR))
		goto onError;

	return datetime;
onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromAbsDateAndTime(long absdate,
	double abstime)
{
	mxDateTimeObject *datetime;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	if (mxDateTime_SetFromAbsDateTime(datetime,
		absdate,
		abstime,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;
onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromAbsDateTime(long absdate,
	double abstime,
	int calendar)
{
	mxDateTimeObject *datetime;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	if (mxDateTime_SetFromAbsDateTime(datetime,
		absdate,
		abstime,
		calendar))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

/* Creates a new DateTime instance using datetime as basis by adding
the given offsets to the value of datetime and then re-normalizing
them.

The resulting DateTime instance will use the same calendar as
datetime.

*/

static
	mxDateTimeObject *mxDateTime_FromDateTimeAndOffset(mxDateTimeObject *datetime,
	long absdate_offset,
	double abstime_offset)
{
	mxDateTimeObject *dt;
	long days;
	long absdate = datetime->absdate;
	double abstime = datetime->abstime;

	absdate += absdate_offset;
	abstime += abstime_offset;

	/* Normalize */
	if (abstime < 0 && abstime >= -SECONDS_PER_DAY) {
		abstime += SECONDS_PER_DAY;
		absdate -= 1;
	}
	if (abstime >= SECONDS_PER_DAY && abstime < 2*SECONDS_PER_DAY) {
		abstime -= SECONDS_PER_DAY;
		absdate += 1;
	}
	/* Some compilers and/or processors (e.g. gcc 2.95.3 on Mandrake)
	have troubles with getting rounding right even though 86400.0
	IS exactly representable using IEEE floats... that's why we are
	extra careful here. */
	while (DOUBLE_IS_NEGATIVE(abstime)) {
		days = (long)(-abstime / SECONDS_PER_DAY);
		if (days == 0)
			days = 1;
		days++;
		abstime += days * SECONDS_PER_DAY;
		absdate -= days;
	}
	while (abstime >= SECONDS_PER_DAY) {
		days = (long)(abstime / SECONDS_PER_DAY);
		if (days == 0)
			days = 1;
		abstime -= days * SECONDS_PER_DAY;
		absdate += days;
	}
	if (mxDateTime_DoubleStackProblem &&
		abstime >= (double)8.63999999999999854481e+04) {
			DPRINTF("mxDateTime_FromDateTimeAndOffset: "
				"triggered double work-around: "
				"abstime is %.20f, diff %.20e, as int %i\n", 
				abstime,
				abstime - SECONDS_PER_DAY,
				(int)abstime);
			absdate += 1;
			abstime = 0.0;
	}

	dt = mxDateTime_New();
	if (dt == NULL)
		return NULL;
	if (mxDateTime_SetFromAbsDateTime(dt,
		absdate,
		abstime,
		datetime->calendar))
		goto onError;

	return dt;

onError:
	mxDateTime_Free(dt);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromAbsDays(double absdays)
{
	mxDateTimeObject *datetime;
	long absdate;
	double abstime,fabsdays;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	fabsdays = floor(absdays);
	Py_AssertWithArg(fabsdays > -LONG_MAX && fabsdays < LONG_MAX,
		mxDateTime_RangeError,
		"absdays out of range: %i",
		(int)absdays);
	absdate = (long)fabsdays + 1;
	abstime = (absdays - fabsdays) * SECONDS_PER_DAY;
	if (mxDateTime_SetFromAbsDateTime(datetime,
		absdate,
		abstime,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromTmStruct(struct tm *tm)
{
	mxDateTimeObject *datetime = 0;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;

	if (mxDateTime_SetFromDateAndTime(datetime,
		tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		(double)tm->tm_sec,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromTicks(double ticks)
{
	mxDateTimeObject *datetime = 0;
	struct tm *tm;
	double seconds;
	time_t tticks = (time_t)ticks;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;

	/* Conversion is done to local time */
	tm = localtime(&tticks);
	if (tm == NULL)
		Py_Error(mxDateTime_Error,
		"could not convert ticks value to local time");
	/* Add fraction */
	seconds = floor((double)tm->tm_sec) + (ticks - floor(ticks));

	if (mxDateTime_SetFromDateAndTime(datetime,
		tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		seconds,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromGMTicks(double ticks)
{
	mxDateTimeObject *datetime = 0;
	struct tm *tm;
	double seconds;
	time_t tticks = (time_t)ticks;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	/* Conversion is done to GMT time */
	tm = gmtime(&tticks);
	/* Add fraction */
	seconds = floor((double)tm->tm_sec) + (ticks - floor(ticks));
	if (mxDateTime_SetFromDateAndTime(datetime,
		tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		seconds,
		MXDATETIME_GREGORIAN_CALENDAR))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
	mxDateTimeObject *mxDateTime_FromCOMDate(double comdate)
{
	mxDateTimeObject *datetime = 0;

	datetime = mxDateTime_New();
	if (datetime == NULL)
		return NULL;
	if (mxDateTime_SetFromCOMDate(datetime,comdate))
		goto onError;

	return datetime;

onError:
	mxDateTime_Free(datetime);
	return NULL;
}

static
struct tm *mxDateTime_AsTmStruct(mxDateTimeObject *datetime,
struct tm *tm)
{
	Py_Assert((long)((int)datetime->year) == datetime->year,
		mxDateTime_RangeError,
		"year out of range for tm struct conversion");

	memset(tm,0,sizeof(tm));
	tm->tm_hour = (int)datetime->hour;
	tm->tm_min = (int)datetime->minute;
#if ROUND_SECONDS_IN_TM_STRUCT
	tm->tm_sec = (int)(datetime->second + 0.5); /* Round the value */
#else
	tm->tm_sec = (int)datetime->second;
#endif
	tm->tm_mday = (int)datetime->day;
	tm->tm_mon = (int)datetime->month - 1;
	tm->tm_year = (int)datetime->year - 1900;
	tm->tm_wday = ((int)datetime->day_of_week + 1) % 7;
	tm->tm_yday = (int)datetime->day_of_year - 1;
	tm->tm_isdst = -1; /* unknown */
	return tm;

onError:
	return NULL;
}

static
	double mxDateTime_AsCOMDate(mxDateTimeObject *datetime)
{
	return datetime->comdate;
}

/* This global is set to
-1 if mktime() auto-corrects the value of the DST flag to whatever the
value should be for the given point in time (which is bad)
0 if the global has not yet been initialized
1 if mktime() does not correct the value and returns proper values
*/

static int mktime_works = 0;

static
	int init_mktime_works(void)
{
	struct tm tm;
	time_t a,b;

	/* Does mktime() in general and specifically DST = -1 work ? */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 5;
	tm.tm_year = 98;
	tm.tm_isdst = -1;
	a = mktime(&tm);
	Py_Assert(a != (time_t)-1,
		PyExc_SystemError,
		"mktime() returned an error (June)");
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 0;
	tm.tm_year = 98;
	tm.tm_isdst = -1;
	a = mktime(&tm);
	Py_Assert(a != (time_t)-1,
		PyExc_SystemError,
		"mktime() returned an error (January)");

	/* Some mktime() implementations return (time_t)-1 when setting
	DST to anything other than -1. Others adjust DST without
	looking at the given setting. */

	/* a = (Summer, DST = 0) */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 5;
	tm.tm_year = 98;
	tm.tm_isdst = 0;
	a = mktime(&tm);
	if (a == (time_t)-1) {
		mktime_works = -1;
		return 0;
	}

	/* b = (Summer, DST = 1) */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 5;
	tm.tm_year = 98;
	tm.tm_isdst = 1;
	b = mktime(&tm);
	if (a == (time_t)-1 || a == b) {
		mktime_works = -1;
		return 0;
	}

	/* a = (Winter, DST = 0) */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 0;
	tm.tm_year = 98;
	tm.tm_isdst = 0;
	a = mktime(&tm);
	if (a == (time_t)-1) {
		mktime_works = -1;
		return 0;
	}

	/* b = (Winter, DST = 1) */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = 1;
	tm.tm_mon = 0;
	tm.tm_year = 98;
	tm.tm_isdst = 1;
	b = mktime(&tm);
	if (a == (time_t)-1 || a == b) {
		mktime_works = -1;
		return 0;
	}

	mktime_works = 1;
	return 0;
onError:
	return -1;
}

/* Returns the ticks value for datetime assuming it stores a datetime
value in local time. 

offsets is subtracted from the resulting ticks value (this can be
used to implement DST handling). 

dst is passed to the used mktime() C lib API and can influence the
calculation: dst == 1 means that the datetime value should be
interpreted with DST on, dst == 0 with DST off. Note that this
doesn't work on all platforms. dst == -1 means: use the DST value
in affect at the given point in time.

*/

static
	double mxDateTime_AsTicksWithOffset(mxDateTimeObject *datetime,
	double offset,
	int dst)
{
	struct tm tm;
	time_t tticks;
	double ticks;

	Py_Assert(datetime->calendar == MXDATETIME_GREGORIAN_CALENDAR,
		mxDateTime_Error,
		"can only convert the Gregorian calendar to ticks");
	Py_Assert((long)((int)datetime->year) == datetime->year,
		mxDateTime_RangeError,
		"year out of range for ticks conversion");

	memset(&tm,0,sizeof(tm));
	tm.tm_hour = (int)datetime->hour;
	tm.tm_min = (int)datetime->minute;
	tm.tm_sec = (int)datetime->second;
	tm.tm_mday = (int)datetime->day;
	tm.tm_mon = (int)datetime->month - 1;
	tm.tm_year = (int)datetime->year - 1900;
	tm.tm_wday = -1;
	tm.tm_yday = (int)datetime->day_of_year - 1;
	tm.tm_isdst = dst;
	/* mktime uses local time ! */
	tticks = mktime(&tm);
	if (tticks == (time_t)-1 && tm.tm_wday == -1)
		Py_Error(mxDateTime_Error,
		"cannot convert value to a time value");
	/* Check if mktime messes up DST */
	if (dst >= 0 && mktime_works <= 0) {
		if (mktime_works == 0) {
			if (init_mktime_works() < 0)
				goto onError;
		}
		if (mktime_works < 0)
			Py_Error(PyExc_SystemError,
			"mktime() doesn't support setting DST to anything but -1");
	}
	/* Add fraction and turn into a double and subtract offset */
	ticks = (double)tticks
		+ (datetime->abstime - floor(datetime->abstime))
		- offset;
	return ticks;

onError:
	return -1.0;
}

static
	double mxDateTime_AsTicks(mxDateTimeObject *datetime)
{
	return mxDateTime_AsTicksWithOffset(datetime,0,-1);
}

/* Returns the ticks value for datetime assuming it stores a UTC
datetime value. 

offsets is subtracted from the resulting ticks value before
returning it. This is useful to implement time zone handling.

*/

static
	double mxDateTime_AsGMTicksWithOffset(mxDateTimeObject *datetime,
	double offset)
{
	Py_Assert(datetime->calendar == MXDATETIME_GREGORIAN_CALENDAR,
		mxDateTime_Error,
		"can only convert the Gregorian calendar to ticks");

	/* For POSIX style calculations there's nothing much to do... */
	if (mxDateTime_POSIXConform) {
		return ((datetime->absdate - 719163) * SECONDS_PER_DAY 
			+ datetime->abstime
			- offset);
	}

#ifdef HAVE_TIMEGM
	{
		/* Use timegm() API */
		struct tm tm;
		time_t tticks;

		Py_Assert((long)((int)datetime->year) == datetime->year,
			mxDateTime_RangeError,
			"year out of range for ticks conversion");

		/* Use timegm() if not POSIX conform: the time package knows about
		leap seconds so we use that information too. */
		memset(&tm,0,sizeof(tm));
		tm.tm_hour = (int)datetime->hour;
		tm.tm_min = (int)datetime->minute;
		tm.tm_sec = (int)datetime->second;
		tm.tm_mday = (int)datetime->day;
		tm.tm_mon = (int)datetime->month - 1;
		tm.tm_year = (int)datetime->year - 1900;
		tm.tm_wday = ((int)datetime->day_of_week + 1) % 7;
		tm.tm_yday = (int)datetime->day_of_year - 1;
		tm.tm_isdst = 0;
		/* timegm uses UTC ! */
		tticks = timegm(&tm);
		Py_Assert(tticks != (time_t)-1,
			mxDateTime_Error,
			"cannot convert value to a time value");
		/* Add fraction and turn into a double */
		return ((double)tticks
			+ (datetime->abstime - floor(datetime->abstime))
			- offset);
	}
#else
	{
		/* Work around with a trick... */
		double localticks,gmticks,gmtoffset;

		if (mxDateTime_CalcTicks(datetime,
			&localticks,&gmticks,&gmtoffset))
			goto onError;
		return gmticks - offset;
	}
#endif

onError:
	return -1.0;
}

static
	double mxDateTime_AsGMTicks(mxDateTimeObject *datetime)
{
	return mxDateTime_AsGMTicksWithOffset(datetime,0);
}

/* Returns the UTC offset at the given time; assumes local time is
stored in the instance. */

static
	double mxDateTime_GMTOffset(mxDateTimeObject *datetime)
{
	double gmticks,ticks;

	gmticks = mxDateTime_AsGMTicks(datetime);
	if (gmticks == -1.0 && PyErr_Occurred())
		goto onError;
	ticks = mxDateTime_AsTicksWithOffset(datetime,0,-1);
	if (ticks == -1.0 && PyErr_Occurred())
		goto onError;
	return gmticks - ticks;

onError:
	return -1.0;
}

/* Return the instance's value in absolute days: days since 0001-01-01
0:00:00 using fractions for parts of a day. */

static
	double mxDateTime_AsAbsDays(mxDateTimeObject *datetime)
{
	return ((double)(datetime->absdate - 1) + 
		datetime->abstime / SECONDS_PER_DAY);
}

/* Return broken down values of the instance. This call returns the
values as stored in the instance regardeless of the used
calendar. */

static
	int mxDateTime_BrokenDown(mxDateTimeObject *datetime,
	long *year,
	int *month,
	int *day,
	int *hour,
	int *minute,
	double *second)
{
	if (year)
		*year = (long)datetime->year;
	if (month)
		*month = (int)datetime->month;
	if (day)
		*day = (int)datetime->day;
	if (hour)
		*hour = (int)datetime->hour;
	if (minute)
		*minute = (int)datetime->minute;
	if (second)
		*second = (double)datetime->second;
	return 0;
}

/* Return the instance's value as broken down values using the Julian
calendar. */

static
	int mxDateTime_AsJulianDate(mxDateTimeObject *datetime,
	long *pyear,
	int *pmonth,
	int *pday,
	int *phour,
	int *pminute,
	double *psecond,
	int *pday_of_week,
	int *pday_of_year)
{
	long absdate = datetime->absdate;
	long year;
	int month,day,dayoffset;

	/* Get the date in the Julian calendar */
	if (datetime->calendar != MXDATETIME_JULIAN_CALENDAR) {
		mxDateTimeObject temp;

		/* Recalculate the date from the absdate value */
		if (mxDateTime_SetFromAbsDate(&temp,
			absdate,
			MXDATETIME_JULIAN_CALENDAR))
			goto onError;
		year = temp.year;
		month = temp.month;
		day = temp.day;
		dayoffset = temp.day_of_year;
	}
	else {
		year = datetime->year;
		month = datetime->month;
		day = datetime->day;
		dayoffset = datetime->day_of_year;
	}

	if (pyear)
		*pyear = (long)year;
	if (pmonth)
		*pmonth = (int)month;
	if (pday)
		*pday = (int)day;

	if (phour)
		*phour = (int)datetime->hour;
	if (pminute)
		*pminute = (int)datetime->minute;
	if (psecond)
		*psecond = (double)datetime->second;

	if (pday_of_week)
		*pday_of_week = mxDateTime_DayOfWeek(absdate);
	if (pday_of_year)
		*pday_of_year = (int)dayoffset;

	return 0;

onError:
	return -1;
}

/* Return the instance's value as broken down values using the Gregorian
calendar. */

static
	int mxDateTime_AsGregorianDate(mxDateTimeObject *datetime,
	long *pyear,
	int *pmonth,
	int *pday,
	int *phour,
	int *pminute,
	double *psecond,
	int *pday_of_week,
	int *pday_of_year)
{
	long absdate = datetime->absdate;
	long year;
	int month,day,dayoffset;

	/* Recalculate the date in the Gregorian calendar */
	if (datetime->calendar != MXDATETIME_GREGORIAN_CALENDAR) {
		mxDateTimeObject temp;

		/* Recalculate the date  from the absdate value */
		if (mxDateTime_SetFromAbsDate(&temp,
			absdate,
			MXDATETIME_GREGORIAN_CALENDAR))
			goto onError;
		year = temp.year;
		month = temp.month;
		day = temp.day;
		dayoffset = temp.day_of_year;
	}
	else {
		year = datetime->year;
		month = datetime->month;
		day = datetime->day;
		dayoffset = datetime->day_of_year;
	}

	if (pyear)
		*pyear = (long)year;
	if (pmonth)
		*pmonth = (int)month;
	if (pday)
		*pday = (int)day;

	if (phour)
		*phour = (int)datetime->hour;
	if (pminute)
		*pminute = (int)datetime->minute;
	if (psecond)
		*psecond = (double)datetime->second;

	if (pday_of_week)
		*pday_of_week = mxDateTime_DayOfWeek(absdate);
	if (pday_of_year)
		*pday_of_year = (int)dayoffset;

	return 0;

onError:
	return -1;
}

/* Returns the DST setting for the given DateTime instance assuming it
refers to local time. -1 is returned in case it cannot be
determined, 0 if it is not active, 1 if it is. For calendars other
than the Gregorian the function always returns -1. 

XXX If mktime() returns -1 for isdst, try harder using the hack in
timegm.py.

*/

static
	int mxDateTime_DST(mxDateTimeObject *datetime)
{
	struct tm tm;
	time_t ticks;

	if (datetime->calendar != MXDATETIME_GREGORIAN_CALENDAR)
		return -1;
	if ((long)((int)datetime->year) != datetime->year)
		return -1;

	memset(&tm,0,sizeof(tm));
	tm.tm_hour = (int)datetime->hour;
	tm.tm_min = (int)datetime->minute;
	tm.tm_sec = (int)datetime->second;
	tm.tm_mday = (int)datetime->day;
	tm.tm_mon = (int)datetime->month - 1;
	tm.tm_year = (int)datetime->year - 1900;
	tm.tm_wday = -1;
	tm.tm_isdst = -1;
	ticks = mktime(&tm);
	if (ticks == (time_t)-1 && tm.tm_wday == -1)
		return -1;
	return tm.tm_isdst;
}

/* Returns a Python string containing the locale's timezone name for
the given DateTime instance (assuming it refers to local time).
"???"  is returned in case it cannot be determined.  */

static
	ObjectScript::OS::String mxDateTime_TimezoneString(ObjectScript::OS * os, mxDateTimeObject *datetime)
{
	if (datetime->calendar != MXDATETIME_GREGORIAN_CALENDAR)
		return ObjectScript::OS::String(os, "???");
	if ((long)((int)datetime->year) != datetime->year)
		return ObjectScript::OS::String(os, "???");

#ifndef HAVE_STRFTIME
	return ObjectScript::OS::String(os, "???");
#else
	struct tm tm;
	time_t ticks;
	char tz[255];

	memset(&tm,0,sizeof(tm));
	tm.tm_hour = (int)datetime->hour;
	tm.tm_min = (int)datetime->minute;
	tm.tm_sec = (int)datetime->second;
	tm.tm_mday = (int)datetime->day;
	tm.tm_mon = (int)datetime->month - 1;
	tm.tm_year = (int)datetime->year - 1900;
	tm.tm_wday = -1;
	tm.tm_isdst = mxDateTime_DST(datetime);
	ticks = mktime(&tm);
	if (ticks == (time_t)-1 && tm.tm_wday == -1)
		return PyString_FromString("???");
	strftime(tz,sizeof(tm),"%Z",&tm);
	return ObjectScript::OS::String(os, tz);
#endif
}

/* Returns the ISO week notation for the given DateTime instance as
tuple (year,isoweek,isoday). The algorithm also Works for negative
dates.

XXX Check this algorithm for the Julian calendar.

*/

static
	int mxDateTime_ISOWeekTuple(ObjectScript::OS * os, mxDateTimeObject *datetime)
{
	int week;
	long year = datetime->year;
	int day;

	/* Estimate */
	week = (datetime->day_of_year-1) - datetime->day_of_week + 3;
	if (week >= 0)
		week = week / 7 + 1;
	day = datetime->day_of_week + 1;
	DPRINTF("mxDateTime_ISOWeekTuple: estimated year, week, day = %ld, %i, %i\n",
		year,week,day);

	/* Verify */
	if (week < 0) {
		/* The day lies in last week of the previous year */
		year--;
		if ((week > -2) || 
			(week == -2 && mxDateTime_Leapyear(year,datetime->calendar)))
			week = 53;
		else	    
			week = 52;
	}
	else if (week == 53) {
		/* Check if the week belongs to year or year+1 */
		if (31-datetime->day + datetime->day_of_week < 3) {
			week = 1;
			year++;
		}
	}
	DPRINTF("mxDateTime_ISOWeekTuple: corrected year, week, day = %ld, %i, %i\n",
		year,week,day);
	os->pushNumber(year);
	os->pushNumber(week);
	os->pushNumber(day);
	return 3; // Py_BuildValue("lii",year,week,day);
}

/* Return a string identifying the used calendar. */

static
	ObjectScript::OS::String mxDateTime_CalendarString(ObjectScript::OS * os, mxDateTimeObject *datetime)
{
	switch (datetime->calendar) {
	case MXDATETIME_GREGORIAN_CALENDAR:
		return ObjectScript::OS::String(os, "GREGORIAN");
	
	case MXDATETIME_JULIAN_CALENDAR:
		return ObjectScript::OS::String(os, "JULIAN");
	
	default:
		Py_Error(PyExc_SystemError,
			"Internal error in mxDateTime: wrong calendar value");
	}

onError:
	return ObjectScript::OS::String(os, "???");
}

/* Writes a string representation to buffer. If the string does not
fit the buffer, nothing is written. */

static
	void mxDateTime_AsString(mxDateTimeObject *self,
	char *buffer,
	int buffer_len)
{
	double second;

	if (!buffer || buffer_len < 50)
		return;
	second = mxDateTime_FixSecondDisplay(self->second);
	if (self->year >= 0)
		sprintf(buffer,"%04li-%02i-%02i %02i:%02i:%05.2f",
		(long)self->year,(int)self->month,(int)self->day,
		(int)self->hour,(int)self->minute,
		(float)second);
	else
		sprintf(buffer,"-%04li-%02i-%02i %02i:%02i:%05.2f",
		(long)-self->year,(int)self->month,(int)self->day,
		(int)self->hour,(int)self->minute,
		(float)second);
}

/* Returns a string indicating the date in ISO format. */

static
	ObjectScript::OS::String mxDateTime_DateString(ObjectScript::OS * os, mxDateTimeObject *self)
{
	char buffer[50];

	if (self->year >= 0)
		sprintf(buffer,"%04li-%02i-%02i",
		(long)self->year,(int)self->month,(int)self->day);
	else
		sprintf(buffer,"-%04li-%02i-%02i",
		(long)-self->year,(int)self->month,(int)self->day);

	return ObjectScript::OS::String(os, buffer);
}

/* Returns a string indicating the time in ISO format. */

static
	ObjectScript::OS::String mxDateTime_TimeString(ObjectScript::OS * os, mxDateTimeObject *self)
{
	char buffer[50];
	double second;

	second = mxDateTime_FixSecondDisplay(self->second);
	sprintf(buffer,"%02i:%02i:%05.2f",
		(int)self->hour,(int)self->minute,(float)second);

	return ObjectScript::OS::String(os, buffer);
}

/* --- methods --- */

#define datetime ((mxDateTimeObject*)self)

#ifdef HAVE_STRFTIME
Py_C_Function( mxDateTime_strftime,
	"strftime(formatstr)")
{
	PyObject *v;
	char *fmt = 0;
	char *output = 0;
	Py_ssize_t len_output,size_output = STRFTIME_OUTPUT_SIZE;
	struct tm tm;

	Py_GetArg("|s",fmt);

	if (!fmt)
		/* We default to the locale's standard date/time format */
		fmt = "%c";

	Py_Assert((long)((int)datetime->year) == datetime->year,
		mxDateTime_RangeError,
		"year out of range for strftime() formatting");

	/* Init tm struct */
	memset(&tm,0,sizeof(tm));
	tm.tm_mday = (int)datetime->day;
	tm.tm_mon = (int)datetime->month - 1;
	tm.tm_year = (int)datetime->year - 1900;
	tm.tm_hour = (int)datetime->hour;
	tm.tm_min = (int)datetime->minute;
#if ROUND_SECONDS_IN_TM_STRUCT
	tm.tm_sec = (int)(datetime->second + 0.5); /* Round the value */
#else
	tm.tm_sec = (int)datetime->second;
#endif
	tm.tm_wday = ((int)datetime->day_of_week + 1) % 7;
	tm.tm_yday = (int)datetime->day_of_year - 1;
	tm.tm_isdst = mxDateTime_DST(datetime);

	output = new(char,size_output);

	while (1) {
		if (output == NULL) {
			PyErr_NoMemory();
			goto onError;
		}
		len_output = strftime(output,size_output,fmt,&tm);
		if (len_output == size_output) {
			size_output *= 2;
			output = resize(output,char,size_output);
		}
		else
			break;
	}
	v = PyString_FromStringAndSize(output,len_output);
	if (v == NULL)
		goto onError;
	free(output);
	return v;

onError:
	if (output)
		free(output);
	return NULL;
}
#endif

static
	long mxDateTime_Hash(mxDateTimeObject *self)
{
	long x = 0;
	long z[sizeof(double)/sizeof(long)+1];
	register int i;

	/* Clear z */
	for (i = sizeof(z) / sizeof(long) - 1; i >= 0; i--)
		z[i] = 0;

	/* Copy the double onto z */
	*((double *)z) = self->absdate * SECONDS_PER_DAY + self->abstime;

	/* Hash the longs in z together using XOR */
	for (i = sizeof(z) / sizeof(long) - 1; i >= 0; i--)
		x ^= z[i];

	/* Errors are indicated by returning -1, so we have to fix
	that hash value by hand. */
	if (x == -1)
		x = 19980427;

	return x;
}

static
	int mxDateTime_Compare(mxDateTimeObject *self, mxDateTimeObject *other)
{
	DPRINTF("mxDateTime_Compare: "
		"%s op %s\n",
		left->ob_type->tp_name,
		right->ob_type->tp_name);

	if (self == other)
		return 0;

	/* Short-cut */
	if (_mxDateTime_Check(self) && _mxDateTime_Check(other)) {
		long d0 = self->absdate, d1 = other->absdate;
		double t0 = self->abstime, t1 = other->abstime;

		return (d0 < d1) ? -1 : (d0 > d1) ? 1 :
			(t0 < t1) ? -1 : (t0 > t1) ? 1 : 0;
	}
	Py_Error(PyExc_TypeError,
		"can't compare types");

onError:
	return -1;
}

/* --- DateTimeDelta Object -----------------------------------------*/

/* --- allocation --- */

static
	mxDateTimeDeltaObject *mxDateTimeDelta_New(void)
{
	return new mxDateTimeDeltaObject();
}

/* --- deallocation --- */

static
	void mxDateTimeDelta_Free(mxDateTimeDeltaObject *delta)
{
	delete delta;
}

/* --- internal functions --- */

/* We may have a need for this in the future: */

#define mxDateTimeDelta_SetFromDaysEx(delta,days,seconds) \
	mxDateTimeDelta_SetFromSeconds(delta, SECONDS_PER_DAY*days + seconds)

static
	int mxDateTimeDelta_SetFromSeconds(mxDateTimeDeltaObject *delta,
	double seconds)
{
	if (delta == NULL) {
		PyErr_BadInternalCall();
		goto onError;
	}

	/* Store the internal seconds value as-is */
	delta->seconds = seconds;

	/* The broken down values are always positive: force seconds to be
	positive. */
	if (DOUBLE_IS_NEGATIVE(seconds))
		seconds = -seconds;

	/* Range check */
	Py_AssertWithArg(seconds <= SECONDS_PER_DAY * (double)LONG_MAX,
		mxDateTime_RangeError,
		"DateTimeDelta value out of range: %i seconds",
		(int)seconds);

	/* Calculate the broken down time */
	{
		long wholeseconds;
		long day = 0;
		short hour,minute;
		double second;

		/* Calculate day part and then normalize seconds to be in the
		range 0 <= seconds < 86400.0 */ 
		day += (long)(seconds / SECONDS_PER_DAY);
		seconds -= SECONDS_PER_DAY * (double)day;
		/* Some compilers (e.g. gcc 2.95.3 on Mandrake) have troubles
		with getting rounding right even though 86400.0 IS exactly
		representable using IEEE floats... */
		if (seconds >= SECONDS_PER_DAY) {
			day++;
			seconds -= SECONDS_PER_DAY;
		}
		/* Calculate the other parts based on the normalized seconds
		value */
		wholeseconds = (long)seconds;
		hour = (short)(wholeseconds / 3600);
		minute = (short)((wholeseconds % 3600) / 60);
		second = seconds - (double)(hour*3600 + minute*60);
		/* Fix a possible rounding error */
		if (DOUBLE_IS_NEGATIVE(second))
			second = 0.0;

		DPRINTF("mxDateTimeDelta_SetFromSeconds: "
			"seconds=%f day=%ld hour=%i minute=%i second=%f\n",
			delta->seconds,day,hour,minute,second);

		delta->day = day;
		delta->hour = (signed char)hour;
		delta->minute = (signed char)minute;
		delta->second = second;
	}
	return 0;

onError:
	return -1;
}

static
	double mxDateTimeDelta_AsDouble(mxDateTimeDeltaObject *delta)
{
	return delta->seconds;
}

static
	double mxDateTimeDelta_AsDays(mxDateTimeDeltaObject *delta)
{
	return delta->seconds / SECONDS_PER_DAY;
}

static
	int mxDateTimeDelta_BrokenDown(mxDateTimeDeltaObject *delta,
	long *day,
	int *hour,
	int *minute,
	double *second)
{
	if (day)
		*day = (long)delta->day;
	if (hour)
		*hour = (int)delta->hour;
	if (minute)
		*minute = (int)delta->minute;
	if (second)
		*second = (double)delta->second;
	return 0;
}

/* Writes a string representation to buffer. If the string does not
fit the buffer, nothing is written. */

static
	void mxDateTimeDelta_AsString(mxDateTimeDeltaObject *self,
	char *buffer,
	int buffer_len)
{
	double second;

	if (!buffer || buffer_len < 50)
		return;
	second = mxDateTime_FixSecondDisplay(self->second);
	if (self->day != 0) {
		if (self->seconds >= 0.0)
			sprintf(buffer,"%ld:%02i:%02i:%05.2f",
			(long)self->day,(int)self->hour,
			(int)self->minute,(float)second);
		else
			sprintf(buffer,"-%ld:%02i:%02i:%05.2f",
			(long)self->day,(int)self->hour,
			(int)self->minute,(float)second);
	}
	else {
		if (self->seconds >= 0.0)
			sprintf(buffer,"%02i:%02i:%05.2f",
			(int)self->hour,(int)self->minute,(float)second);
		else
			sprintf(buffer,"-%02i:%02i:%05.2f",
			(int)self->hour,(int)self->minute,(float)second);
	}
}

/* --- methods --- (should have lowercase extension) */

#define delta ((mxDateTimeDeltaObject*)self)

static
	long mxDateTimeDelta_Hash(mxDateTimeDeltaObject *self)
{
	long x = 0;
	long z[sizeof(double)/sizeof(long)+1];
	register int i;

	/* Clear z */
	for (i = sizeof(z) / sizeof(long) - 1; i >= 0; i--)
		z[i] = 0;

	/* Copy the double onto z */
	*((double *)z) = self->seconds;

	/* Hash the longs in z together using XOR */
	for (i = sizeof(z) / sizeof(long) - 1; i >= 0; i--)
		x ^= z[i];

	/* Errors are indicated by returning -1, so we have to fix
	that hash value by hand. */
	if (x == -1)
		x = 19980428;

	return x;
}

static
	int mxDateTimeDelta_Compare(mxDateTimeDeltaObject *self, mxDateTimeDeltaObject *other)
{
	DPRINTF("mxDateTimeDelta_Compare: "
		"%s op %s\n",
		left->ob_type->tp_name,
		right->ob_type->tp_name);

	if (self == other)
		return 0;

	/* Short-cut */
	if (_mxDateTimeDelta_Check(self) && _mxDateTimeDelta_Check(other)) {
		double i = self->seconds;
		double j = other->seconds;

		return (i < j) ? -1 : (i > j) ? 1 : 0;
	}

	Py_Error(PyExc_TypeError,
		"can't compare types");

onError:
	return -1;
}

void mxDateTime_init()
{
	if(!mxDateTime_Initialized){
		mxDateTime_POSIXConform = mxDateTime_POSIX();
		mxDateTime_DoubleStackProblem = mxDateTime_CheckDoubleStackProblem(
			SECONDS_PER_DAY - (double)7.27e-12);
		mxDateTime_Initialized = 1;
	}
}

