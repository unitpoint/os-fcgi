#ifndef MXDATETIME_H
#define MXDATETIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for the calendar ID: */
#define MXDATETIME_GREGORIAN_CALENDAR	0
#define MXDATETIME_JULIAN_CALENDAR	1

/* Strings for the calendars */
#define MXDATETIME_GREGORIAN_CALENDAR_STRING	"Gregorian"
#define MXDATETIME_JULIAN_CALENDAR_STRING	"Julian"


/* --- DateTime Object ------------------------------------------*/

/* Note: The objects internal values are only calculated once and
   are thereafter considered immutable ! */

typedef struct {   
    /* Representation used to do calculations */
    long absdate;		/* number of days since 31.12. in the year 1 BC
				   calculated in the Gregorian calendar. */
    double abstime;		/* seconds since 0:00:00.00 (midnight)
				   on the day pointed to by absdate */

    /* COM Date representation */
    double comdate;
    
    /* Broken down values (set at creation time and using the calendar
       specified in the calendar flag); depend on the calendar used. */
    long year;			/* starting from year 1 */
    signed char month;		/* 1-12 */
    signed char day;		/* 1-31 */
    signed char hour;		/* 0-24 */
    signed char minute;		/* 0-59 */
    double second;		/* 0-60.999... */

    signed char day_of_week;	/* 0 (Monday) - 6 (Sunday) */
    short day_of_year;		/* 1-366 */

    unsigned char calendar;	/* Calendar ID; for possible values see
				   above. */
} mxDateTimeObject;

/* --- DateTimeDelta Object ----------------------------------*/

/* Note: The objects internal values are only calculated once and
   are thereafter considered immutable ! */

typedef struct {
    double seconds;		/* number of delta seconds */

    /* Broken down values (set at creation time); the sign can be
       deduced from seconds' sign. */
    long day;			/* >=0 */
    signed char hour;		/* 0-23 */
    signed char minute;		/* 0-59 */
    double second;		/* 0-60.999 */
    
} mxDateTimeDeltaObject;

/* EOF */
#ifdef __cplusplus
}
#endif
#endif
