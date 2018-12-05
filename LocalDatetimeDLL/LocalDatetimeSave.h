#include "..\UltimateDatetimeDLL\UltimateDatetime.h"

#define DLLExport __declspec( dllexport )

typedef struct
{
	/* A local daatetime */
	char timezone[30]; /* The name of a timezone from the IANA time zone database.
						   It is storage inefficient to use the name, but if it is
						   replaced by an index, care must be taken to not change
						   index values when updating the time zone database. 
						   The futureAdjust flag on the utc applies to time zone,
						   as well as leap second.  I.e., when desired, call futureAdjust,
						   which will adjust UTC and, therefore, the tick count, if
						   the time zone parameters or daylight savings time rules
						   have changed since the date was initialized. */
	/* Time zones are associated with daylight savings rules.  Both the time zone elements
		and the */
	int8_t futureAdjust; // Same as for UTC, except for time zone and/or rules.  Sets value for UTC
	char sOrD; /* Flag indicating whether a specified time is daylight savings time (D)
					or standard time (S).  This specification is only necessary
					for the 1 or 2 hours after the time is reset to standard time.  For
					example, in the eastern time zone at 2 am on the first Sunday
					in November, the time resets to 1 am.  Thus all of the times
					between 1:00:00 and 1:59:59 are repeated, so, to avoid
					ambiguity, an 'S' or 'D' needs to be specified. */
	UTCDatetime utc; // The UTC time corresponding to this local time.
} LocalDatetime;

typedef struct
{
	/* The coordinates of a local datetime */

	/* This struct is used to determine the applicable rule set for determining the 
		GMT and DST offsets for a local datetime, to enable conversion to a UTCDatetime.
		A time zone has a series of 
		rule sets with each element of the series applicable for a specific 
		datetime range.  Given a local datetime and time zone, in order to
		determine the applicable rule set, we need to determine in which 
		datetime range the falls.  While this could be done easily were the
		the given ranges and local dateimte converted to TAI, this is not
		possible until the offsets are known.  As such, we express the ranges
		in terms of local datetime coordinates then perform the comparisons
		in local datetime coordinate space.  
		
		Since the finest precsion at which a rule set transtion time is defined for
		any of the time zones is 0.1 seconds, we can omit the attosecond element. */
	uint8_t dayOfMonth;
	uint8_t month; // 1 = Jan, 2 = Feb, ... 12 = Dec
	int32_t year;  /* Positive years = CE, 0 = 1BCE, -1 = 2 BCE, ... -n = n+1 BCE
				      Used a 32-bit value, even though years can go from 13e9 BCE
					  to over 100e9 CE for 32-bit Ticks and 500e9 CE for 64-bit Ticks.
					  The reason for this is to avoid creating two data strcutures,
					  one that uses a 64-bit int for years and one that uses 2 32-bit
					  ints.*/
	int8_t gigayear; /* Number of billions of years.  Using an 8-bit int limits
							the value to 127 billion years. */
	uint8_t hour; // Calendar hour
	uint8_t minute; // Calendar minute
	uint8_t second; // Calendar second
	uint32_t nanosecond; // Calendar nanosecond

} DTCoords

