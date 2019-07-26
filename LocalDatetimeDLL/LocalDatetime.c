#include "LocalDatetime.h"

// Make portable across Windows and Linux
#if (defined (WIN32) || defined (_WIN32) || defined (__WIN32)) && !defined (__CYGWIN__)
// Windows
#include <Windows.h>
BOOL APIENTRY DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		createTimeZonePeriods();
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#else
// Linux or OS X
__atribute__((constructor)) createTimeZonePeriods;
#endif


/* A local datetime is based on a time zone.  A time zone is a
	region that has a uniform standard time for legal, commercial,
	and social purposes.  It is convenient for areas in close
	commercial or other communication to keep the same time, so
	time zones tend to follow the boundaries of countries and their
	subdivisions.
	
	A time zone has several	parameters, which, together, define
	an offset to UTC datetime.	This offset is designed to maintain
	sunrise and sunset at approximately the same time of day around
	the globe.

	Most of the time zones on land are offset from Coordinated
	Universal Time (UTC) by a whole number of hours
	(UTC-12 to UTC+14), but a few are offset by 30 or 45 minutes
	(for example Newfoundland Standard Time is UTC -03:30 and
	Nepal Standard Time is UTC +05:45).

	Many time zones introduce the concept of daylight savings
	time, which causes a change in the offset at some point
	during the year.  The modern DST concept was proposed
	in 1907 and was in widespread use in 1916 as a wartime
	measure aimed at conserving coal.  Numerous reasons are given 
	for continuing to practice daylight savings time,
	including to enable farmers to work at reasonable hours and
	to enhance the safety of children travelling to and from school.
	The time zone defines rules for the day of the year and
	time of day at which the transition to daylight savings time occurs
	and	for the day of the year and time of day at which the
	transition back to standard time occurs.

	Since leap seconds are added contemporaneously around the
	globe, the time of a leap second will be different for
	different time zones.

	Time zone databases generally contain coordinates to
	define the geographic scope of the time zone.  For the
	purposes of this library, those coordinates are not
	relevant, as the assumption is the time zone will 
	be provided as input, not computed from geographic
	coordinates.

	Time zones and the rules that define them have
	continually evolved since their introduction.  For
	example, the transition to DST in the US had occurred
	on the first Sunday in April from 1987 through 2006.
	However, beginning in 2007, the transition occurred
	on the second Sunday in March.  This evolution
	necessitates the maintenance of a historical time
	zone database to faciliate the accurate conversion
	of historical datetimes to UTC.

	Time zones have unique names.  There are also
	abbreviations for standard and daylight savings
	time, which are not unique. 
	
	Prior to the 1880's, regions generally used Local Mean
	Time, which was based on mean solar time.  LMT expresses
	the offset from UTC	based on the position of the sun in
	the sky.  These	offsets are in hours:minutes:seconds.  
	From 1909 to 1937, the Netherlands clocks were legally
	UT+00:19:32.13, the only known instance of a fractional
	second offset.
	
	What does the TYPE of a rule signify?
	What do 's' and 'u' mean in the AT field of rules?
	From function rulesub in zic.c:
						ToDisStd	ToDisGMT
		's' - Standard	true		false
		'w' - Wall		false		false
		'g' - Greenwich	true		true
		'u' - Universal	true		true
		'z' - Zulu		true		true
	From function stringrule in zic.c:
		DisGMT adjusts time of day by gmtoff
		DisStd adjusts time of day by dstoff */


UTCOffset createUTCOffset(uint8_t isNegative, uint8_t days, uint8_t hours, uint8_t minutes,
	uint8_t seconds, uint8_t hundredths, char offsetType[])
{
	/* Create a UTCOffset from the elements, checking the validity of the elements.*/
	UTCOffset uTCOff;
	uint8_t offsetIndex;

	// Check UTCOffset elements
	offsetIndex = numOffsetTypes;
	for (uint8_t i = 0; i < numOffsetTypes; i++)
	{
		if (strncmp(OffsetTypes[i], offsetType, 3) == 0)
		{
			offsetIndex = i;
			break;
		}
	}
	uint8_t init = checkUTCOffsetElements(isNegative, days, hours, minutes, seconds, hundredths, offsetIndex);

	// If any of the above tests have failed, set uOffInit and return
	uTCOff.uOffInit = init;
	if (init != 0)
	{
		uTCOff.isNegative = 0;
		uTCOff.days = 0;
		uTCOff.hours = 0;
		uTCOff.minutes = 0;
		uTCOff.seconds = 0;
		uTCOff.hundredths = 0;
		uTCOff.offsetType = 0;
		return uTCOff;
	}

	// Set the fields of uTCOff	
	uTCOff.isNegative = isNegative;
	uTCOff.days = days;
	uTCOff.hours = hours;
	uTCOff.minutes = minutes;
	uTCOff.seconds = seconds;
	uTCOff.hundredths = hundredths;
	uTCOff.offsetType = offsetIndex;
	return uTCOff;
}

uint8_t checkUTCOffsetElements(uint8_t isNegative, uint8_t days, uint8_t hours, uint8_t minutes,
	uint8_t seconds, uint8_t hundredths, uint8_t offsetIndex)
{
	/* Check the values of UTCOffset elements*/
	// Initialize the initialization bit field
	uint8_t init = 0;

	if (isNegative > 1) init |= OffInvalidIsNegative;
	if (days > 1) init |= OffDaysGtMax;
	if (hours > 23) init |= OffHoursGtMax;
	if (minutes > 59) init |= OffMinutesGtMax;
	if (seconds > 59) init |= OffSecondsGtMax;
	if (hundredths > 99) init |= OffHundredthsGtMax;
	if (offsetIndex >= numOffsetTypes) init |= OffInvalidType;
	return init;
}

char * reportUTCOffsetConstructionError(uint8_t init, char buffer[], int lenBuffer, int failTest)
{
	// Report errors in UTC Offset construction based on bit flag, which
	//	is set in createUTCOffset

	int offset = 0;
	int numError = 0;
	memset(buffer, ' ', lenBuffer - 1);

	if (init != 0)
	{
		// There is at least one error.  Write the errors one per line.
		//	Write FAILED OR	PASSED, depending on whether the test was designed to pass or fail
		offset += (failTest ? sprintf_s(buffer, lenBuffer, "PASSED ") :
			sprintf_s(buffer, lenBuffer, "FAILED "));
		if (init & OffInvalidIsNegative)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[0]);
			numError++;
		}
		if (init & OffDaysGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[1]);
			numError++;
		}
		if (init & OffHoursGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[2]);
			numError++;
		}
		if (init & OffMinutesGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[3]);
			numError++;
		}
		if (init & OffSecondsGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[4]);
			numError++;
		}
		if (init & OffHundredthsGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[5]);
			numError++;
		}
		if (init & OffInvalidType)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[6]);
			numError++;
		}
		if (init & OffInvalidOperation)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCOffErrorStrings[7]);
			numError++;
		}
	}
	return buffer;
}

// UTCOffset comparison functions

int compareUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	/* Compare the elements of 2 UTCOffsets.
	Return 1 if utc1 > utc2
	0 if utc1 < utc2
	-1 if utc1 == utc2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	int sign;

	//	Check the signs
	if (utc1.isNegative != utc2.isNegative)
	{
		// Different signs - if utc2 is negative, return 1, otherwise return 0
		return (utc2.isNegative);
	}
	else
	{
		//  The signs are the same.  Compare the elements in descending order
		sign = (utc1.isNegative ? -1 : 1);
		if (utc1.days == utc2.days)
		{
			// Go to the next element
			if (utc1.hours == utc2.hours)
			{
				// Go to the next element
				if (utc1.minutes == utc2.minutes)
				{
					// Go to the next element
					if (utc1.seconds == utc2.seconds)
					{
						// Go to the next element
						if (utc1.hundredths == utc2.hundredths)
						{
							// The values are equal
							return -1;
						}
						else
						{
							return (sign * utc1.hundredths > sign * utc2.hundredths);
						}
					}
					else
					{
						return (sign * utc1.seconds > sign * utc2.seconds);
					}
				}
				else
				{
					return (sign * utc1.minutes > sign * utc2.minutes);
				}
			}
			else
			{
				// The year elements are different
				return (sign * utc1.hours > sign * utc2.hours);
			}
		}
		else
		{
			// The gigayear elements are different
			return (sign * utc1.days > sign * utc2.days);
		}
	}
}

int isEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Use compare function to perform equality check
	return (compareUTCOffsets(utc1, utc2) == -1);
}
int isGreaterUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Use compare function to perform greater than check
	return (compareUTCOffsets(utc1, utc2) == 1);
}
int isLessUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Use compare function to perform less than check
	return (compareUTCOffsets(utc1, utc2) == 0);
}
int isNotEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Use equality function
	return !isEqualUTCOffsets(utc1, utc2);
}
int isGreaterOrEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Use less than function
	return !isLessUTCOffsets(utc1, utc2);
}
int isLessOrEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2)

{
	// Use greater than function
	return !isGreaterUTCOffsets(utc1, utc2);
}

const char * asStringUTCOffset(UTCOffset utc)
{
	/* Format a UTCOffset as follows:
		-1d 12:13:24.12 u-s   */
	static char buffer[20];
	int len = sizeof(buffer);
	int ioff = 0, n;

	// Only required fields are hours and minutes
	if (utc.isNegative && (utc.days != 0 || utc.hours != 0 || utc.minutes != 0
		|| utc.seconds != 0 || utc.hundredths != 0))
	{
		buffer[ioff] = '-';
		ioff++;
	}
	if (utc.days != 0)
	{
		// Days can only be 0 or 1
		buffer[ioff] = '1';
		buffer[ioff + 1] = 'd';
		buffer[ioff + 2] = ' ';
		ioff += 3;
	}
	n = sprintf_s(buffer + ioff, len - ioff, "%02d:%02d", utc.hours, utc.minutes);
	ioff += 5;
	// If hundredths are non-zero, must print seconds
	if (utc.seconds != 0 || utc.hundredths != 0)
	{
		n = sprintf_s(buffer + ioff, len - ioff, ":%02d", utc.seconds);
		ioff += 3;
	}
	if (utc.hundredths != 0)
	{
		n = sprintf_s(buffer + ioff, len - ioff, ".%02d", utc.hundredths);
		ioff += 3;
	}
	buffer[ioff] = ' ';
	buffer[ioff+1] = OffsetTypes[utc.offsetType][0];
	buffer[ioff+2] = OffsetTypes[utc.offsetType][1];
	buffer[ioff+3] = OffsetTypes[utc.offsetType][2];
	buffer[ioff+4] = '\0';
	return buffer;
}

LocalCalCoords createLocalCalCoords(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar, const char timezone[], uint8_t frame)
{
	// Create a LocalCalCoords from the elements
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoords(gigayear, year, month, dayOfMonth, hour, minute, second, nanosecond,
		attosecond, calendar);
	return createLocalCalCoordsFromCalCoords(cc, timezone, frame);
}

LocalCalCoords createLocalCalCoordsFromCalCoords(CalCoords cc, const char timezone[],
	uint8_t frame)
{
	// Create a LocalCalCoords from a CalCoords and time zone elements
	LocalCalCoords lcc;
	uint32_t i, j;

	//  Load the cc portion, but do not assume it was initialized correctly
	lcc.cc = cc;
	lcc.lccInit = cc.ccInit;

	// Check the validity of the time zone
	//	Do not allow a blank time zone
	if (strnlen_s(timezone, maxTZNameLength) == 0 ||
		(strnlen_s(timezone, maxTZNameLength) == 1 && timezone[0] == ' '))
	{
		// Blank time zone
		lcc.timezone[0] = '\0';
		lcc.lccInit |= InvalidTimeZone;
	}
	else
	{
		for (i = 0; i < numTimeZones; i++)
		{
			for (j = 0; j < maxNamesPerTimeZone; j++)
			{
				if (strncmp(timezone, TimeZones[i][j], maxTZNameLength) == 0)
				{
					// Time zone is valid
					// Load the time zone name instead of the link to speed searches
					strcpy_s(lcc.timezone, sizeof(lcc.timezone), TimeZones[i][0]);
					goto end;
				}
			}
		}
		end:
		// Check whether a match was found
		if (i == numTimeZones)
		{
			// No match found
			lcc.timezone[0] = '\0';
			lcc.lccInit |= InvalidTimeZone;
		}
	}
	// Check the frame
	if (frame == 'u' || frame == 'U' || frame == 0) lcc.frame = 0;
	else if (frame == 's' || frame == 'S' || frame == 1) lcc.frame = 1;
	else if (frame == 'w' || frame == 'W' || frame == 2) lcc.frame = 2;
	else
	{
		// Invalid frame of reference
		lcc.frame = 3;
		lcc.lccInit |= InvalidTimeFrame;
	}
	return lcc;
}

LocalCalCoords createLocalCalCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame)
{
	// Create a LocalCalCoords from day of year, instead of month and day of month
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoordsFromDayOfYear(gigayear, year, dayOfYear, hour, minute, second, nanosecond,
		attosecond, calendar);
	return createLocalCalCoordsFromCalCoords(cc, timezone, frame);
}

LocalCalCoords createLocalCalCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame)
{
	// Create a LocalCalCoords from a weekday rule, instead of month and day of month
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoordsFromWeekdayRule(gigayear, year, month, afterDayOfMonth, weekNumber, dayOfWeek,
		hour, minute, second, nanosecond, attosecond, calendar);
	return createLocalCalCoordsFromCalCoords(cc, timezone, frame);
}

LocalCalCoordsDT createLocalCalCoordsDT(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust)
{
	// Create a LocalCalCoordsDT from the elements
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoords(gigayear, year, month, dayOfMonth, hour, minute, second, nanosecond,
		attosecond, calendar);
	return createLocalCalCoordsDTFromCalCoords(cc, timezone, frame, bOrA, futureAdjust);
}

LocalCalCoordsDT createLocalCalCoordsDTFromCalCoords(CalCoords cc, const char timezone[],
	uint8_t frame, uint8_t bOrA, uint8_t futureAdjust)
{
	// Create a LocalCalCoordsDT from a CalCoords and time zone elements
	LocalCalCoordsDT lcc, lccU;
	UTCOffset off;
	uint32_t i, j;

	//  Load the cc portion, but do not assume it was initialized correctly
	lcc.cc = cc;
	lcc.lccInit = cc.ccInit;

	// Check the validity of the time zone
	//	Do not allow a blank time zone
	if (strnlen_s(timezone, maxTZNameLength) == 0 ||
		(strnlen_s(timezone, maxTZNameLength) == 1 && timezone[0] == ' '))
	{
		// Blank time zone
		lcc.timezone[0] = '\0';
		lcc.timezoneIndex = numTimeZones;
		lcc.lccInit |= InvalidTimeZone;
	}
	else
	{
		for (i = 0; i < numTimeZones; i++)
		{
			for (j = 0; j < maxNamesPerTimeZone; j++)
			{
				if (strncmp(timezone, TimeZones[i][j], maxTZNameLength) == 0)
				{
					// Time zone is valid
					// Load the time zone name instead of the link to speed searches
					strcpy_s(lcc.timezone, sizeof(lcc.timezone), TimeZones[i][0]);
					goto end;
				}
			}
		}
	end:
		// Check whether a match was found
		if (i == numTimeZones)
		{
			// No match found
			lcc.timezone[0] = '\0';
			lcc.lccInit |= InvalidTimeZone;
		}
		lcc.timezoneIndex = i;
	}
	// Check other elements
	if (frame == 'u' || frame == 'U' || frame == 0) lcc.frame = 0;
	else if (frame == 's' || frame == 'S' || frame == 1) lcc.frame = 1;
	else if (frame == 'w' || frame == 'W' || frame == 2) lcc.frame = 2;
	else
	{
		// Invalid frame of reference
		lcc.frame = 3;
		lcc.lccInit |= InvalidTimeFrame;
	}
	if (bOrA == ' ' || bOrA == 0) lcc.bOrA = 0;
	else if (bOrA == 'b' || bOrA == 'B' || bOrA == 1) lcc.bOrA = 1;
	else if (bOrA == 'a' || bOrA == 'A' || bOrA == 2) lcc.bOrA = 2;
	else
	{
		// Invalid bOrA
		lcc.bOrA = 3;
		lcc.lccInit |= MissingBOrA;
	}
	if (futureAdjust < 3) lcc.futureAdjust = futureAdjust;
	else
	{
		// Invalid future adjust
		lcc.futureAdjust = 3;
		lcc.lccInit |= InvalidFutureAdjust;
	}
	// If the previous validity checks have been passed, compute the time zone related fields
	//	using the PeriodTimeZone array
	if (lcc.lccInit == 0) lcc = computeTZFields(lcc);
	// Check leap second specification
	//	By the leap second era, all GMT offsets were in even numbers of minutes, so
	//	 a leap second would always be during the 60th second
	if (lcc.lccInit == 0 && lcc.cc.time.second == 60)
	{
		// Convert to universal basis
		//   First convert to proleptic Gregorian calendar, if necessary
		lccU = lcc;
		if (lccU.cc.date.calendar != 0) lccU.cc = convertCalToUTCGregorian(lcc.cc);
		//	 Translate to standard frame of reference, if necessary
		if (lccU.frame == 2)
		{
			off = negateUTCOffset(createUTCOffset
					(0, 0, lccU.sToWMinutes / 60, lccU.sToWMinutes % 60, 0, 0, "s-w"));
			lccU = offsetLocalCalCoordsDT(lccU, off);
		}
		//	 Translate to universal frame of reference, if necessary
		if (lccU.frame == 1)
		{
			off = negateUTCOffset(PeriodTimeZones[lccU.timezoneIndex].periods[lccU.periodIndex].gMTOffset);
			lccU = offsetLocalCalCoordsDT(lccU, off);
		}
		if (!isLeapSecondDay(lccU.cc.date.gigayear, lccU.cc.date.year, lccU.cc.date.month,
				lccU.cc.date.dayOfMonth) ||	lccU.cc.time.hour != 23 || lccU.cc.time.minute != 59)
			lcc.lccInit |= NotALeapSecond;
	}
	return lcc;
}

LocalCalCoordsDT createLocalCalCoordsDTFromUTCDatetime(UTCDatetime utc, const char timezone[],
	uint8_t frame, uint32_t calendar, uint8_t futureAdjust)
{
	// Create a LocalCalCoordsDT from a UTCDatetime, time zone elements and calendar
	LocalCalCoordsDT lcc;
	uint32_t i, j;
	uint8_t frameInt;

	//  Load the cc portion, but do not assume it was initialized correctly
	lcc.cc = createCalCoords(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute,
		utc.second, getNanosecond(utc.tai), getAttosecond(utc.tai), 0);
	lcc.bOrA = 0; // Initial value consistent with creation from UTCDatetime
	lcc.frame = 0; // Initial value consistent with creation from UTCDatetime
	lcc.lccInit = utc.taiInit;

	// Check the validity of the time zone
	//	Do not allow a blank time zone
	if (strnlen_s(timezone, maxTZNameLength) == 0 ||
		(strnlen_s(timezone, maxTZNameLength) == 1 && timezone[0] == ' '))
	{
		// Blank time zone
		lcc.timezone[0] = '\0';
		lcc.timezoneIndex = numTimeZones;
		lcc.lccInit |= InvalidTimeZone;
	}
	else
	{
		for (i = 0; i < numTimeZones; i++)
		{
			for (j = 0; j < maxNamesPerTimeZone; j++)
			{
				if (strncmp(timezone, TimeZones[i][j], maxTZNameLength) == 0)
				{
					// Time zone is valid
					// Load the time zone name instead of the link to speed searches
					strcpy_s(lcc.timezone, sizeof(lcc.timezone), TimeZones[i][0]);
					goto end;
				}
			}
		}
	end:
		// Check whether a match was found
		if (i == numTimeZones)
		{
			// No match found
			lcc.timezone[0] = '\0';
			lcc.lccInit |= InvalidTimeZone;
		}
		lcc.timezoneIndex = i;
	}
	// Check other elements
	if (frame == 'u' || frame == 'U' || frame == 0) frameInt = 0;
	else if (frame == 's' || frame == 'S' || frame == 1) frameInt = 1;
	else if (frame == 'w' || frame == 'W' || frame == 2) frameInt = 2;
	else
	{
		// Invalid frame of reference
		lcc.frame = 3;
		lcc.lccInit |= InvalidTimeFrame;
	}
	if (futureAdjust < 3) lcc.futureAdjust = futureAdjust;
	else
	{
		// Invalid future adjust
		lcc.futureAdjust = 3;
		lcc.lccInit |= InvalidFutureAdjust;
	}
	// If the previous validity checks have been passed, compute the time zone related fields
	//	using the PeriodTimeZone array
	if (lcc.lccInit == 0) lcc = computeTZFields(lcc);
	// Check leap second specification
	// A UTCDatetime is on the proleptic Gregorian calendar and Universal time frame so
	//	no conversion or translation is necessary to perform a leap second check.
	//	By the leap second era, all GMT offsets were in even numbers of minutes, so
	//	 a leap second would always be during the 60th second
	if (lcc.lccInit == 0 && lcc.cc.time.second == 60)
	{
		if (!isLeapSecondDay(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month,
			lcc.cc.date.dayOfMonth) || lcc.cc.time.hour != 23 || lcc.cc.time.minute != 59)
			lcc.lccInit |= NotALeapSecond;
	}
	// Do a calendar conversion, if necessary
	if (calendar != 0) lcc.cc = convertCalToCalendar(lcc.cc, calendar);
	// Next translate the frame of reference
	return translateFromUniversal(lcc, timezone, frameInt);
}

LocalCalCoordsDT createLocalCalCoordsDTFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust)
{
	// Create a LocalCalCoordsDT from day of year, instead of month and day of month
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoordsFromDayOfYear(gigayear, year, dayOfYear, hour, minute, second, nanosecond,
		attosecond, calendar);
	return createLocalCalCoordsDTFromCalCoords(cc, timezone, frame, bOrA, futureAdjust);
}

LocalCalCoordsDT createLocalCalCoordsDTFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust)
{
	// Create a LocalCalCoordsDT from a weekday rule, instead of month and day of month
	CalCoords cc;

	// Create the CalCoords portion
	cc = createCalCoordsFromWeekdayRule(gigayear, year, month, afterDayOfMonth, weekNumber, dayOfWeek,
		hour, minute, second, nanosecond, attosecond, calendar);
	return createLocalCalCoordsDTFromCalCoords(cc, timezone, frame, bOrA, futureAdjust);
}

// UTCOffset arithmetic
UTCOffset addUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Add utc1 to utc2
	/*  The types to be added must be compatible as follows:
		u-s + s-w = u-w  0 + 1 = 2 / 1 + 0 = 2
		u-w + w-s = u-s  2 + 7 = 0 / 7 + 2 = 0
		u-w + s-u = s-w  2 + 6 = 1 / 6 + 2 = 1
									In this section a jump refers to the discontinuity
									encountered when crossing time zone periods.  A forward
									jump represents a move from an earlier time period to
									a later time period.  Note in these cases the addition
									is not commutative.
		u-s + s-u = sp1  0 + 6 = 3 (forward jump in standard time when GMT offset changes)
		s-u + u-s = sm1	 6 + 0 = 9 (backward jump in standard time when GMT offset changes)
		s-w + w-s = dp1  1 + 7 = 4 (forward jump in DST offset when DSTR changes)
		w-s + s-u = dm1  7 + 1 = 10(backward jump in DST offset when DSTR changes)
		u-w + w-u = wp1  2 + 8 = 5 (forward jump in wall time when GMT Offset
												and/or DSTR changes)
		w-u + u-w = wm1  8 + 2 = 11 (backward jump in wall time when GMT Offset
												and/or DSTR changes)
		u-s + sp1 = u-s  0 + 3 = 0 / 3 + 0 = 0 (calculate u-s in the n+1 period
												 from u-s in the nth period
		s-w + dp1 = s-w  1 + 4 = 1 / 4 + 1 = 1
		u-w + wp1 = u-w  2 + 5 = 2 / 2 + 5 = 2
		u-s + sm1 = u-s  0 + 9 = 0 / 9 + 0 = 0 (calculate u-s in the n-1 period
												 from u-s in the nth period
		s-w + dm1 = s-w  1 + 10 = 1 / 10 + 1 = 1
		u-w + wm1 = u-w  2 + 11 = 2 / 11 + 2 = 2
		
		sp1 + dp1 = wp1  3 + 4 = 5 / 4 + 3 = 5
		sm1 + dm1 = wm1  9 + 10 = 11 / 10 + 9 = 11*/
	//
	//  Initialize the sum to zero to handle the case when the signs are different,
	//	 but the magnitudes are the same.
	UTCOffset utcsum = { 0, 0, 0, 0, 0, 0, 0 }, utcS, utcL;
	int8_t hundredths, seconds, minutes, hours, days;

	// Determine the type of addition to be performed
	if ((utc1.offsetType == 0 && utc2.offsetType == 1) ||
		(utc1.offsetType == 1 && utc2.offsetType == 0))
	{
		// u-s + s-w = u-w
		utcsum.offsetType = 2;
	}
	else if ((utc1.offsetType == 2 && utc2.offsetType == 7) ||
			 (utc1.offsetType == 7 && utc2.offsetType == 2))
	{
		// u-w + w-s = u-s
		utcsum.offsetType = 0;
	}
	else if ((utc1.offsetType == 2 && utc2.offsetType == 6) ||
		(utc1.offsetType == 6 && utc2.offsetType == 2))
	{
		// u-w + s-u = s-w
		utcsum.offsetType = 1;
	}
	else if (utc1.offsetType == 0 && utc2.offsetType == 6) 
	{
		// u-s + s-u = sp1
		utcsum.offsetType = 3;
	}
	else if (utc1.offsetType == 6 && utc2.offsetType == 0)
	{
		// s-u + u-s = sm1
		utcsum.offsetType = 9;
	}
	else if (utc1.offsetType == 1 && utc2.offsetType == 7)
	{
		// s-w + w-s = dp1
		utcsum.offsetType = 4;
	}
	else if (utc1.offsetType == 7 && utc2.offsetType == 1)
	{
		// w-s + s-w = dm1
		utcsum.offsetType = 10;
	}
	else if (utc1.offsetType == 2 && utc2.offsetType == 8)
	{
		// w-u + u-w = wp1
		utcsum.offsetType = 5;
	}
	else if (utc1.offsetType == 8 && utc2.offsetType == 2)
	{
		// u-w + w-u = wm1
		utcsum.offsetType = 11;
	}
	else if ((utc1.offsetType == 0 && utc2.offsetType == 3) ||
		(utc1.offsetType == 3 && utc2.offsetType == 0))
	{
		// u-s + sp1 = u-s
		utcsum.offsetType = 0;
	}
	else if ((utc1.offsetType == 1 && utc2.offsetType == 4) ||
		(utc1.offsetType == 4 && utc2.offsetType == 1))
	{
		// s-w + dp1 = s-w
		utcsum.offsetType = 1;
	}
	else if ((utc1.offsetType == 2 && utc2.offsetType == 5) ||
		(utc1.offsetType == 5 && utc2.offsetType == 2))
	{
		// u-w + wp1 = u-w
		utcsum.offsetType = 2;
	}
	else if ((utc1.offsetType == 0 && utc2.offsetType == 9) ||
		(utc1.offsetType == 9 && utc2.offsetType == 0))
	{
		// u-s + sm1 = u-s
		utcsum.offsetType = 0;
	}
	else if ((utc1.offsetType == 1 && utc2.offsetType == 10) ||
		(utc1.offsetType == 10 && utc2.offsetType == 1))
	{
		// s-w + dm1 = s-w
		utcsum.offsetType = 1;
	}
	else if ((utc1.offsetType == 2 && utc2.offsetType == 11) ||
		(utc1.offsetType == 11 && utc2.offsetType == 2))
	{
		// u-w + wm1 = u-w
		utcsum.offsetType = 2;
	}
	else if ((utc1.offsetType == 3 && utc2.offsetType == 4) ||
		(utc1.offsetType == 4 && utc2.offsetType == 3))
	{
		// sp1 + dp1 = wp1
		utcsum.offsetType = 5;
	}
	else if ((utc1.offsetType == 9 && utc2.offsetType == 10) ||
		(utc1.offsetType == 10 && utc2.offsetType == 9))
	{
		// sm1 + dm1 = wm1
		utcsum.offsetType = 11;
	}
	else
	{
		// Invalid addition
		utcsum.uOffInit |= OffInvalidOperation;
		return utcsum;
	}
	// There are two primary cases, when utc1 and utc2 have the same
	//	sign, and when they have different signs
	if (utc1.isNegative == utc2.isNegative)
	{
		// The signs are the same, so perform add with carry
		utcsum.isNegative = utc1.isNegative;

		utcsum.hundredths = utc1.hundredths + utc2.hundredths;
		// Carry, if necessary
		if (utcsum.hundredths >= 100)
		{
			utcsum.hundredths -= 100;
			utcsum.seconds ++;
		}

		utcsum.seconds += utc1.seconds + utc2.seconds;
		if (utcsum.seconds >= 60)
		{
			utcsum.seconds -= 60;
			utcsum.minutes++;
		}

		utcsum.minutes += utc1.minutes + utc2.minutes;
		if (utcsum.minutes >= 60)
		{
			utcsum.minutes -= 60;
			utcsum.hours++;
		}

		utcsum.hours += utc1.hours + utc2.hours;
		if (utcsum.hours >= 24)
		{
			utcsum.hours -= 24;
			utcsum.days++;
		}

		// Days cannot overflow, since max value of utc1 or utc2.days is 1
		utcsum.days += utc1.days + utc2.days;
	}
	else
	{
		// The signs are different, so perform subtraction with borrow
		// Subtract the smaller value from the larger value, then set the sign appropriately
		if (isEqualUTCOffsets(absValueUTCOffset(utc1), absValueUTCOffset(utc2)))
		{
			// The values are equal, but opposite in sign, so return 0.
			utcsum.isNegative = 0;
			return utcsum;
		}
		else if (isGreaterUTCOffsets(absValueUTCOffset(utc1), absValueUTCOffset(utc2)))
		{
			// utc1 is larger than utc2
			utcL = utc1;
			utcS = utc2;
			utcsum.isNegative = utc1.isNegative;
		}
		else
		{
			// utc2 is larger than utc1
			utcL = utc2;
			utcS = utc1;
			utcsum.isNegative = utc2.isNegative;
		}
		// When subtracting, values may be temporarily negative, so cannot store directly in utcsum
		hundredths = utcL.hundredths - utcS.hundredths;
		seconds = 0;
		minutes = 0;
		hours = 0;
		days = 0;
		// Borrow, if necessary
		if (hundredths < 0)
		{
			hundredths += 100;
			seconds--;
		}
		seconds += utcL.seconds - utcS.seconds;
		if (seconds < 0)
		{
			seconds += 60;
			minutes--;
		}
		minutes += utcL.minutes - utcS.minutes;
		if (minutes < 0)
		{
			minutes += 60;
			hours--;
		}
		hours += utcL.hours - utcS.hours;
		if (hours < 0)
		{
			hours += 24;
			days--;
		}
		// Days cannot underflow
		days += utcL.days - utcS.days;

		utcsum.hundredths = hundredths;
		utcsum.seconds = seconds;
		utcsum.minutes = minutes;
		utcsum.hours = hours;
		utcsum.days = days;
	}
	return utcsum;
}

UTCOffset negateUTCOffset(UTCOffset utc)
{
	// Negate a UTC Offset by changing the isNegative value
	UTCOffset utc2;
	utc2 = utc;
	utc2.isNegative = !utc.isNegative;
	// The type of the offset also must be 'reversed'
	utc2.offsetType = (utc.offsetType + 6) % 12;
	return utc2;
}

UTCOffset absValueUTCOffset(UTCOffset utc)
{
	// Absolute value of a UTC Offset
	return (utc.isNegative ? negateUTCOffset(utc) : utc);
}

UTCOffset subtractUTCOffsets(UTCOffset utc1, UTCOffset utc2)
{
	// Subtract utc2 from utc1 by negating utc2, then adding
	return addUTCOffsets(utc1, negateUTCOffset(utc2));
}

int compareLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	/* Compare the elements of 2 local datetime coordinates.
	Return 1 if lcc1 > lcc2
	0 if lcc1 < lcc2
	-1 if lcc1 == lcc2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	// Check whether time zones are different while at least one of the
	//	frames of reference is not universal or the frames of reference
	//	are different
	if (
		((strncmp(lcc1.timezone, lcc2.timezone, maxTZNameLength) != 0) &&
			(lcc1.frame != 0 || lcc2.frame != 0)) ||
			lcc1.frame != lcc2.frame)
	{
		// Time zones are different while at least one of the frames of reference
		//	is not universal and/or frames of reference are different
		// Invalid comparison
		abort();
	}
	//  Use the CalCoords comparison function
	return compareCalCoords(lcc1.cc, lcc2.cc);
}

int isEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform equality check
	return (compareLocalCalCoords(lcc1, lcc2) == -1);
}
int isGreaterLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform greater than check
	return (compareLocalCalCoords(lcc1, lcc2) == 1);
}
int isLessLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform less than check
	return (compareLocalCalCoords(lcc1, lcc2) == 0);
}
int isNotEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	// Use equality function
	return !isEqualLocalCalCoords(lcc1, lcc2);
}
int isGreaterOrEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)
{
	// Use less than function
	return !isLessLocalCalCoords(lcc1, lcc2);
}
int isLessOrEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2)

{
	// Use greater than function
	return !isGreaterLocalCalCoords(lcc1, lcc2);
}

const char * asStringLocalCalCoords(LocalCalCoords lcc, char stringCal[])
{
	// Format a LocalCalCoords as a readable string
	//
	// * stringUTC must be dimensioned at least 130 
	// Use asStringDatetime
	//
	return asStringDatetime(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth,
		lcc.cc.time.hour, lcc.cc.time.minute, lcc.cc.time.second, lcc.cc.time.nanosecond, lcc.cc.time.attosecond,
		lcc.cc.date.calendar, lcc.frame, -2, 0, lcc.timezone, 0, stringCal);
}

LocalCalCoords offsetLocalCalCoords_old(LocalCalCoords lcc, UTCOffset off)
{
	//**** Deprecated version, which extends calendars when offset extend into
	//		skipped days due Julian to Gregorian transitions.  New version
	//      does not change calendar.
	/* Apply a positive or negative UTCOffset to a LocalCalCoords to
		translate between either a local wall time, local standard time,
		or universal datetime, and a local wall time, local standard time, or
		universal datetime.  Here, we differentiate between a univeral datetime
		and a UTC datetime.  A UTC datetime is a univeral datetime with
		calendar == 0, aka proleptic Gregorian.
		
		The fromBasis and lcc1.frame arguments indicate
		the direction of the conversion as follows:
			0 = universal datetime
			1 = local standard time
			2 = local wall time

		Several factors must be noted.  First, a translation occurs at
		an instant in time.  Even though the calculation appears to move
		the time forward or backward, in fact what is occurring is a
		translation across coordinate sytems.

		Second, offsets are defined in a particular direction, as follows:

		1. A GMT offset is added to universal (on a consistent calendar) to get local standard time
		2. A daylight savings time offset is added to standard wall time to get local wall time
		3. A beginJump is added to wall time at the end of one period to get wall time at the
			beginning of the next period

		Third, the type of an offset is defined by the from and to basis, but the
		sign is independent of the direction of translation.  Thus, for example,
		when translating from standard time to wall time, the offset is a daylight
		savings time offset, and the offset is added to the CalCoords.  When translating
		from wall time to standard time, the offset is also a daylight savings time offset,
		but the offset is subtracted from CalCoords (alternatively, the negative of the
		offset is added to CalCoords).

		Fourth, given that universal is continuous across changes in time zone periods,
		a translation from universal to universal is a no op.

		Fifth, validity, as defined by the transition from Julian to Gregorian is defined
		in terms of local wall time. 

		Wall to wall is a jump - don't need to differentiate forward vs backward 
		as far as specification, but need to consider the sign of the offset
		when checking Julian / Gregorian transition.  Not sure how to check for
		leap seconds here - i.e., before the jump or after the jump or both.  **
		Doesn't matter, as it will be the same, regardless.

		Standard to standard also is a jump.  We handle this case, but there is
		a potential problem,
		as we have inadequate information to determine whether the translation
		necessitates a calendar extension due to triggering a Julian to Grergorian
		transition.

		This computation is different from one that adds relative
		calendar coordinates to calendar cooridnates.  That calculation
		involves moving along the datetime axis within a coordinate system, while
		this calcuation is used to translate between coordiante systems at
		the same point in time.

		As this computation takes place at a point in time, daylight savings
		time transitions and intervening leap seconds are not considered.  In
		fact, one of the primary uses of this computation is to perfrom
		daylight savings time transitions.

		We do need to consider whether the adjusted time is outside the
		allowed range and whether a coordinate specified within the 61st second
		is during a leap second.

		In the transition from the Julian to Gregorian calendar, days are
		skipped, which results in an interval of invalid dates.  We assume
		the skipped days are on even day boundaries in local wall time
		coordinates.  E.g., if the calendar transition took place on
		June 2nd, 1642 Julian, the next day is June 13th, 1642 Gregorian.
		Thus, the dates from June 3 - June 12th, 1642 are invalid.  We assume
		the transiton occurs at 2-Jun-1642 23:59:59.999999999 999999999 local
		wall time.  Thus, 3-Jun-1642 00:00:00 is an invalid local wall time.
		If the GMT Offset is -3:00:00 and there is 1 hour of daylight savings time,
		the period of invalidity begins at 2-Jun-1642 23:00:00 local standard time
		and 3-Jun-1642 02:00:00 UTC.

		As a result of the above, validity (as a result of skipped days) should
		only be considered when translating to wall time.  If the translation is
		from wall time, the validity should be checked upon entry to the function
		and an error returned if the date is not valid.  In all other cases,
		the from and to values should be considered valid, i.e., not to be
		on skipped days.

		Thus, on a translation from wall time, when the day needs to be incremented
		or decremented, the next or previous day, respectively, on a full calendar
		(i.e., one without skipped days) should be used.  On a translation to wall
		time, when the day needs to be incremented or decremented, move to the
		next valid wall day.

		In the above example, 2-Jun-1642 23:00:00 local wall time translates to
		3-Jun-1642 01:00:00 UTC, even though 3-Jun-1642 is an invalid date in
		local wall time.  3-Jun-1642 05:00:00 UTC, however, translates to
		13-Jun-1642 03:00:00 local wall time.

		If cc1 is a UTCDatetime (i.e., if fromUTC is true), assume it was
		previously checked and is within the allowed date range for a
		UTCDatetime.
		
		The valid offset calculations are as follows:
		
		u + u-s = s  0 + 0 = 1
		s + s-u = s  1 + 6 = 0
		s + s-w = w  1 + 1 = 2 
		w + w-s = s  2 + 7 = 1
		u + u-w = w  0 + 2 = 2
		w + w-u = u  2 + 8 = 0
		s + sp1 = s  1 + 3 = 1
		s + sm1 = s  1 + 9 = 1
		w + wp1 = w  2 + 5 = 2
		w + wm1 = w  2 + 11 = 2*/

	int32_t second, minute, hour, daysInYear, dayOfYear, doySave;
	DateCoords date, datecc, dateUTC;

	// Initialize lcc1 to lcc
	LocalCalCoords lcc1 = lcc;

	// Establish the frame of the result
	if (lcc.frame == 0 && off.offsetType == 0)
	{
		// u + u-s = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 1 && off.offsetType == 6)
	{
		// s + s-u = u
		lcc1.frame = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 1)
	{
		// s + s-w = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 7)
	{
		// w + w-s = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 0 && off.offsetType == 2)
	{
		// u + u-w = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 8)
	{
		// w + w-u = u
		lcc1.frame = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 3)
	{
		// s + sp1 = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 1 && off.offsetType == 9)
	{
		// s + sm1 = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 2 && off.offsetType == 5)
	{
		// w + wp1 = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 11)
	{
		// w + wm1 = w
		lcc1.frame = 2;
	}
	else
	{
		// Invalid calculation
		lcc1.frame = 3;
		lcc1.lccInit |= InvalidTimeFrame;
		return lcc1;
	}

	// Assume cc is valid - i.e., has passed checkCalCoordsElements
	//
	// Determine the day of the year for cc
	//	The below will return the correct value in for all time frames, 
	//		and for calendars adjusted due to translation
	dayOfYear = dayOfYearFromDate(createDateCoords(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth, lcc.cc.date.calendar));
	doySave = dayOfYear;

	if (off.isNegative)
	{ 
		// The offset is negative

		// Start with the smallest element to facilitate borrows
		if (lcc1.cc.time.nanosecond >= e7 * (uint32_t)off.hundredths)
		{
			lcc1.cc.time.nanosecond -= e7 * off.hundredths;
			second = lcc1.cc.time.second;
		}
		else
		{
			// Need to borrow from second
			lcc1.cc.time.nanosecond += e9 - e7 * off.hundredths;
			second = lcc1.cc.time.second - 1;
		}
		// Now compute second
		second -= off.seconds;
		if (second >= 0)
		{
			lcc1.cc.time.second = second;
			minute = lcc1.cc.time.minute;
		}
		else
		{
			// Borrow from minute
			/* This calculation becomes more complicated if we allow for the situation where the minute
				from which the borrow is taking place is a leap minute (which has 61 seconds).  However,
				since UTCOffsets all were adjusted to an even number of minutes before the advent of leap
				seconds, this cannot happen, so ignore this case.*/
			lcc1.cc.time.second = 60 + second;
			minute = lcc1.cc.time.minute - 1;
		}
		// Now compute minute
		minute -= off.minutes;
		if (minute >= 0)
		{
			lcc1.cc.time.minute = minute;
			hour = lcc1.cc.time.hour;
		}
		else
		{
			// Borrow from hour
			lcc1.cc.time.minute = 60 + minute;
			hour = lcc1.cc.time.hour - 1;
		}
		// Now compute hour
		hour -= off.hours;
		if (hour >= 0)
		{
			lcc1.cc.time.hour = hour;
		}
		else
		{
			// Borrow from day of year
			lcc1.cc.time.hour = 24 + hour;
			dayOfYear--;
		}
		// Now compute day of year
		dayOfYear -= off.days;
		// Check whether day of year is before the beginning of the year
		if (dayOfYear <= 0)
		{
			// Borrow from year
			lcc1.cc.date.year--;
			// Determine whether gigayear needs to be decremented
			//	For negative gigayears, year ranges from -999999999 to 0, so a borrow is required at year == -e9
			//	For gigayear == 0, year ranges from -99999999 to 99999999, so a borrow is required at year == -e9.
			if ((lcc1.cc.date.gigayear <= 0) && (lcc1.cc.date.year == -e9))
			{
				lcc1.cc.date.gigayear--;
				lcc1.cc.date.year = 0;
			}
			//	For gigayear > 0, year ranges from 0 to 99999999, so a borrow is required at year == -1.
			if ((lcc1.cc.date.gigayear > 0) && (lcc1.cc.date.year == -1))
			{
				lcc1.cc.date.gigayear--;
				lcc1.cc.date.year = e9 - 1;
			}
			// Determine the number of days in the year
			daysInYear = numDaysInYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, lcc1.cc.date.calendar);
			dayOfYear += daysInYear;
		}
	}
	else
	{
		// The offset is positive

		// Start with the smallest element to minimize the number of calculations
		lcc1.cc.time.nanosecond += e7 * off.hundredths;
		if (lcc1.cc.time.nanosecond >= e9)
		{
			// Perform carry
			lcc1.cc.time.nanosecond -= e9;
			lcc1.cc.time.second ++;
		}
		// As leap seconds were introduced after all countries transitioned to offsets in even
		//	numbers of minutes, an offset with non zero seconds and hundredths will never be
		//	applied to a leap second, or produce a leap second.  However, an offset may still
		//	be applied to a leap second, so can't just check for lcc1.cc.time.second > 59
		second = lcc1.cc.time.second + off.seconds;
		if (lcc1.cc.time.second !=60 || off.hundredths != 0)
		{
			if (second > 59)
			{
				// Perform carry
				lcc1.cc.time.second = second - 60;
				lcc1.cc.time.minute++;
			}
			else lcc1.cc.time.second = second;
		}
		lcc1.cc.time.minute += off.minutes;
		if (lcc1.cc.time.minute > 59)
		{
			// Perform carry
			lcc1.cc.time.minute -= 60;
			lcc1.cc.time.hour ++;
		}
		lcc1.cc.time.hour += off.hours;
		if (lcc1.cc.time.hour > 23)
		{
			// Perform carry
			lcc1.cc.time.hour -= 24;
			dayOfYear ++;
		}
		dayOfYear += off.days;
		// Determine whether dayOfYear is past the end of the year
		daysInYear = numDaysInYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, lcc1.cc.date.calendar);
		if ( dayOfYear > daysInYear)
		{
			// Perform carry
			dayOfYear -= daysInYear;
			lcc1.cc.date.year++;
		}
		// Determine whether the gigayear needs to be incremented
		//	For negative gigayears, year ranges from -999999999 to 0, so a carry is required at year == 1
		if ((lcc1.cc.date.gigayear < 0) && (lcc1.cc.date.year == 1))
		{
			lcc1.cc.date.gigayear++;
			lcc1.cc.date.year = 1 - e9;
		}
		//	For gigayear == 0, year ranges from -99999999 to 99999999, so a carry is required at year == e9.
		//	For gigayear > 0, year ranges from 0 to 99999999, so a carry is required at year == e9.
		if ((lcc1.cc.date.gigayear >= 0) && (lcc1.cc.date.year == e9))
		{
			lcc1.cc.date.gigayear++;
			lcc1.cc.date.year = 0;
		}
	}
	// Derive month and day of month from day of year
	//
	//	Determine whether day of year has changed
	if (dayOfYear != doySave)
	{
		// Day of year has changed, so month and day of month must be recalculated
		// If lcc1.frame is wall or if the calendar already has been extended,
		//  we can use createDayCoordsFromDayOfYear to determine
		//	the month and day of month
		if (lcc1.frame == 2 || lcc1.cc.date.calendar >= e9)
		{
			// createDateCoordsFromDayOfYear provides valid dates on a wall time basis
			//	even when a transition between Julian and Gregorian calendars is involved
			date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
			lcc1.cc.date.month = date.month;
			lcc1.cc.date.dayOfMonth = date.dayOfMonth;
		}
		else
		{
			// If lcc1.frame is either universal or standard time, increment or decrement
			//	the date, ignoring skipped days for a transition between Julian and
			//	Gregorian calendars.  This has the effect of extending the range of validity
			//	by 1 day, which is appropriate since the transition from Julian to Gregorian
			//	calendars is assumed to take place at midnight wall time.
			//
			if (off.isNegative)
			{
				// The translation has the effect of moving to the previous day
				//	Determine whether cc is the first Gregorian date
				datecc = createDateCoords(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth, lcc.cc.date.calendar);
				if (isEqualDateCoords(datecc, firstGregorianDate(lcc.cc.date.calendar)))
				{
					// Decrement the date by 1.  Cannot use dayOfYear.
					//	Use skipGregorianDays function
					date = skipGregorianDays(numDaysToSkip(lcc1.cc.date.calendar) - 1,
						lastJulianDate(lcc1.cc.date.calendar));
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
					//  Extend the calendar
					lcc1.cc.date.calendar += 2 * e9;
				}
				else
				{
					// Use dayOfYear
					date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
				}
			}
			else
			{
				// The translation has the effect of moving to the next day
				//  Determine whether cc is the last Julian date
				datecc = createDateCoords(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth, lcc.cc.date.calendar);
				if (isEqualDateCoords(datecc, lastJulianDate(lcc.cc.date.calendar)))
				{
					// Increment the date by 1.  Cannot use dayOfYear.
					date = skipGregorianDays(0,	lastJulianDate(lcc.cc.date.calendar));
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
					//  Extend the calendar
					lcc1.cc.date.calendar += e9;
				}
				else
				{
					// Use dayOfYear
					date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
				}
			}
		}
	}
	//
	// If the translated value is universal time, check validity of leap second specification.
	//	Convert to proleptic Gregorian calendar, first
	//
	if (lcc1.frame == 0 && lcc1.cc.time.second == 60)
	{
		dateUTC = convertToUTCGregorian(lcc1.cc.date);
		// Leap seconds occur at the end of specific UTC days
		if (!isLeapSecondDay(dateUTC.gigayear, dateUTC.year, dateUTC.month, dateUTC.dayOfMonth) ||
			lcc1.cc.time.hour != 23 || lcc1.cc.time.minute != 59) lcc1.lccInit |= NotALeapSecond;
	}
	//
	// If the input value is on an extended calendar, check whether the translated value
	//	needs to remain on an extended calendar.  Assume that when the day changes
	//	the calendar no longer needs to be extended.  Otherwise, this would imply a
	//	translation has taken place that moves by more than 1 day, which is not allowed.
	//
	if (lcc.cc.date.calendar >= e9 && dayOfYear != doySave) lcc1.cc.date.calendar = lcc1.cc.date.calendar % e9;
	return lcc1;
}

LocalCalCoords offsetLocalCalCoords(LocalCalCoords lcc, UTCOffset off)
{
	/* Apply a positive or negative UTCOffset to a LocalCalCoords to
	translate between either a local wall time, local standard time,
	or universal datetime, and a local wall time, local standard time, or
	universal datetime.  Here, we differentiate between a univeral datetime
	and a UTC datetime.  A UTC datetime is a univeral datetime with
	calendar == 0, aka proleptic Gregorian.

	The fromBasis and lcc1.frame arguments indicate
	the direction of the conversion as follows:
	0 = universal datetime
	1 = local standard time
	2 = local wall time

	Several factors must be noted.  First, a translation occurs at
	an instant in time.  Even though the calculation appears to move
	the time forward or backward, in fact what is occurring is a
	translation across coordinate sytems.

	Second, offsets are defined in a particular direction, as follows:

	1. A GMT offset is added to universal (on a consistent calendar) to get local standard time
	2. A daylight savings time offset is added to standard wall time to get local wall time
	3. A beginJump is added to wall time at the end of one period to get wall time at the
	beginning of the next period

	Third, the type of an offset is defined by the from and to basis, but the
	sign is independent of the direction of translation.  Thus, for example,
	when translating from standard time to wall time, the offset is a daylight
	savings time offset, and the offset is added to the CalCoords.  When translating
	from wall time to standard time, the offset is also a daylight savings time offset,
	but the offset is subtracted from CalCoords (alternatively, the negative of the
	offset is added to CalCoords).

	Fourth, given that universal is continuous across changes in time zone periods,
	a translation from universal to universal is a no op.

	Fifth, a translation from one frame of reference to another does not require skipping
	days due to a Julian to Gregorian calendar transition. For example, consider a calendar
	where the last Julian date is 1/10/1919 and the first Gregorian date is 1/24/1919 and
	the timezone has a 2 hour GMT offset and 1 hour s-w offset on 1/24/1919.  The transition
	to the Gregorian calendar occurs at 1/24/1919 00:00 (w).  If we translate 1/24/1919 00:15 to
	standard time, the result is 1/23/1919 23:15 (s).  Similarly, if we translate 1/24/1919 00:15 to
	univeral time, the result is 1/23/1919 21:15 (u).

	Wall to wall is a jump - don't need to differentiate forward vs backward
	as far as specification, but need to consider the sign of the offset
	when checking Julian / Gregorian transition.  Not sure how to check for
	leap seconds here - i.e., before the jump or after the jump or both.  **
	Doesn't matter, as it will be the same, regardless.

	Standard to standard also is a jump.  We handle this case, but there is
	a potential problem,
	as we have inadequate information to determine whether the translation
	necessitates a calendar extension due to triggering a Julian to Grergorian
	transition.

	This computation is different from one that adds relative
	calendar coordinates to calendar coordinates.  That calculation
	involves moving along the datetime axis within a coordinate system, while
	this calcuation is used to translate between coordiante systems at
	the same point in time.

	As this computation takes place at a point in time, daylight savings
	time transitions and intervening leap seconds are not considered.  In
	fact, one of the primary uses of this computation is to perfrom
	daylight savings time transitions.

	We do need to consider whether the adjusted time is outside the
	allowed range and whether a coordinate specified within the 61st second
	is during a leap second.

	In the transition from the Julian to Gregorian calendar, days are
	skipped, which results in an interval of invalid dates.  We assume
	the skipped days are on even day boundaries in local wall time
	coordinates.  E.g., if the calendar transition took place on
	June 2nd, 1642 Julian, the next day is June 13th, 1642 Gregorian.
	Thus, the dates from June 3 - June 12th, 1642 are invalid.  We assume
	the transiton occurs at 2-Jun-1642 23:59:59.999999999 999999999 local
	wall time.  Thus, 3-Jun-1642 00:00:00 is an invalid local wall time.
	If the GMT Offset is -3:00:00 and there is 1 hour of daylight savings time,
	the period of invalidity begins at 2-Jun-1642 23:00:00 local standard time
	and 3-Jun-1642 02:00:00 UTC.

	As a result of the above, validity (as a result of skipped days) should
	only be considered when translating to wall time.  If the translation is
	from wall time, the validity should be checked upon entry to the function
	and an error returned if the date is not valid.  In all other cases,
	the from and to values should be considered valid, i.e., not to be
	on skipped days.

	Thus, on a translation from wall time, when the day needs to be incremented
	or decremented, the next or previous day, respectively, on a full calendar
	(i.e., one without skipped days) should be used.  On a translation to wall
	time, when the day needs to be incremented or decremented, move to the
	next valid wall day.

	In the above example, 2-Jun-1642 23:00:00 local wall time translates to
	3-Jun-1642 01:00:00 UTC, even though 3-Jun-1642 is an invalid date in
	local wall time.  3-Jun-1642 05:00:00 UTC, however, translates to
	13-Jun-1642 03:00:00 local wall time.

	If cc1 is a UTCDatetime (i.e., if fromUTC is true), assume it was
	previously checked and is within the allowed date range for a
	UTCDatetime.

	The valid offset calculations are as follows:

	u + u-s = s  0 + 0 = 1
	s + s-u = s  1 + 6 = 0
	s + s-w = w  1 + 1 = 2
	w + w-s = s  2 + 7 = 1
	u + u-w = w  0 + 2 = 2
	w + w-u = u  2 + 8 = 0
	s + sp1 = s  1 + 3 = 1
	s + sm1 = s  1 + 9 = 1
	w + wp1 = w  2 + 5 = 2
	w + wm1 = w  2 + 11 = 2*/

	int32_t second, minute, hour, dayOfMonth, month, daysInMonth, daysInPrevMonth;

	// Initialize lcc1 to lcc
	LocalCalCoords lcc1 = lcc;

	// Establish the frame of the result
	if (lcc.frame == 0 && off.offsetType == 0)
	{
		// u + u-s = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 1 && off.offsetType == 6)
	{
		// s + s-u = u
		lcc1.frame = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 1)
	{
		// s + s-w = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 7)
	{
		// w + w-s = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 0 && off.offsetType == 2)
	{
		// u + u-w = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 8)
	{
		// w + w-u = u
		lcc1.frame = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 3)
	{
		// s + sp1 = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 1 && off.offsetType == 9)
	{
		// s + sm1 = s
		lcc1.frame = 1;
	}
	else if (lcc.frame == 2 && off.offsetType == 5)
	{
		// w + wp1 = w
		lcc1.frame = 2;
	}
	else if (lcc.frame == 2 && off.offsetType == 11)
	{
		// w + wm1 = w
		lcc1.frame = 2;
	}
	else
	{
		// Invalid calculation
		lcc1.frame = 3;
		lcc1.lccInit |= InvalidTimeFrame;
		return lcc1;
	}
	// Assume cc is valid - i.e., has passed checkCalCoordsElements
	//
	dayOfMonth = lcc.cc.date.dayOfMonth;
	if (off.isNegative)
	{
		// The offset is negative

		// Start with the smallest element to facilitate borrows
		if (lcc1.cc.time.nanosecond >= e7 * (uint32_t)off.hundredths)
		{
			lcc1.cc.time.nanosecond -= e7 * off.hundredths;
			second = lcc1.cc.time.second;
		}
		else
		{
			// Need to borrow from second
			lcc1.cc.time.nanosecond += e9 - e7 * off.hundredths;
			second = lcc1.cc.time.second - 1;
		}
		// Now compute second
		second -= off.seconds;
		if (second >= 0)
		{
			lcc1.cc.time.second = second;
			minute = lcc1.cc.time.minute;
		}
		else
		{
			// Borrow from minute
			/* This calculation becomes more complicated if we allow for the situation where the minute
			from which the borrow is taking place is a leap minute (which has 61 seconds).  However,
			since UTCOffsets all were adjusted to an even number of minutes before the advent of leap
			seconds, this cannot happen, so ignore this case.*/
			lcc1.cc.time.second = 60 + second;
			minute = lcc1.cc.time.minute - 1;
		}
		// Now compute minute
		minute -= off.minutes;
		if (minute >= 0)
		{
			lcc1.cc.time.minute = minute;
			hour = lcc1.cc.time.hour;
		}
		else
		{
			// Borrow from hour
			lcc1.cc.time.minute = 60 + minute;
			hour = lcc1.cc.time.hour - 1;
		}
		// Now compute hour
		hour -= off.hours;
		if (hour >= 0)
		{
			lcc1.cc.time.hour = hour;
		}
		else
		{
			// Borrow from day of month
			lcc1.cc.time.hour = 24 + hour;
			dayOfMonth--;
		}
		// Now compute day of month
		dayOfMonth -= off.days;
		// Check whether day of month is before the beginning of the month
		if (dayOfMonth < 1)
		{
			// Move to the previous month.  Assume offset is small enough that there is no
			//	need to move more than 1 month
			month = lcc1.cc.date.month - 1;
			// Determine whether year needs to be decremented
			if (month == 0)
			{
				// Year needs to be decremented
				month = 12;
				dayOfMonth += 31;
				lcc1.cc.date.year--;
				// Determine whether gigayear needs to be decremented
				//	For negative gigayears, year ranges from -999999999 to 0, so a borrow is required at year == -e9
				//	For gigayear == 0, year ranges from -99999999 to 99999999, so a borrow is required at year == -e9.
				if ((lcc1.cc.date.gigayear <= 0) && (lcc1.cc.date.year == -e9))
				{
					lcc1.cc.date.gigayear--;
					lcc1.cc.date.year = 0;
				}
				//	For gigayear > 0, year ranges from 0 to 99999999, so a borrow is required at year == -1.
				if ((lcc1.cc.date.gigayear > 0) && (lcc1.cc.date.year == -1))
				{
					lcc1.cc.date.gigayear--;
					lcc1.cc.date.year = e9 - 1;
				}
			}
			else
			{
				// Remain in the same year, but move to the previous month
				daysInPrevMonth = 31;
				// April, June, September and November only have 30 days
				if (month == 4 || month == 6 || month == 9 || month == 11) daysInPrevMonth = 30;
				// February 
				if (month == 2) daysInPrevMonth = 28 + numLeapDays(0, lcc1.cc.date.year,
					lcc1.cc.date.calendar);
				dayOfMonth += daysInPrevMonth;
			}
		}
		else
		{
			// Remain in the current month
			month = lcc1.cc.date.month;
		}
	}
	else
	{
		// The offset is positive

		// Start with the smallest element to minimize the number of calculations
		lcc1.cc.time.nanosecond += e7 * off.hundredths;
		if (lcc1.cc.time.nanosecond >= e9)
		{
			// Perform carry
			lcc1.cc.time.nanosecond -= e9;
			lcc1.cc.time.second++;
		}
		// As leap seconds were introduced after all countries transitioned to offsets in even
		//	numbers of minutes, an offset with non zero seconds and hundredths will never be
		//	applied to a leap second, or produce a leap second.  However, an offset may still
		//	be applied to a leap second, so can't just check for lcc1.cc.time.second > 59
		second = lcc1.cc.time.second + off.seconds;
		if (lcc1.cc.time.second != 60 || off.hundredths != 0)
		{
			if (second > 59)
			{
				// Perform carry
				lcc1.cc.time.second = second - 60;
				lcc1.cc.time.minute++;
			}
			else lcc1.cc.time.second = second;
		}
		lcc1.cc.time.minute += off.minutes;
		if (lcc1.cc.time.minute > 59)
		{
			// Perform carry
			lcc1.cc.time.minute -= 60;
			lcc1.cc.time.hour++;
		}
		lcc1.cc.time.hour += off.hours;
		if (lcc1.cc.time.hour > 23)
		{
			// Perform carry
			lcc1.cc.time.hour -= 24;
			dayOfMonth++;
		}
		dayOfMonth += off.days;
		// Determine whether dayOfMonth is past the end of the month
		daysInMonth = 31;
		month = lcc1.cc.date.month;
		// April, June, September and November only have 30 days
		if (month == 4 || month == 6 || month == 9 || month == 11) daysInMonth = 30;
		// February 
		if (month == 2) daysInMonth = 28 + numLeapDays(0, lcc1.cc.date.year,
			lcc1.cc.date.calendar);
		if (dayOfMonth > daysInMonth)
		{
			// Move to the next month
			month++;
			dayOfMonth -= daysInMonth;
			// Determine whether the year needs to be incremented
			if (month > 12)
			{
				// Increment the year
				month = 1;
				lcc1.cc.date.year++;
				// Determine whether the gigayear needs to be incremented
				//	For negative gigayears, year ranges from -999999999 to 0, so a carry is required at year == 1
				if ((lcc1.cc.date.gigayear < 0) && (lcc1.cc.date.year == 1))
				{
					lcc1.cc.date.gigayear++;
					lcc1.cc.date.year = 1 - e9;
				}
				//	For gigayear == 0, year ranges from -99999999 to 99999999, so a carry is required at year == e9.
				//	For gigayear > 0, year ranges from 0 to 99999999, so a carry is required at year == e9.
				if ((lcc1.cc.date.gigayear >= 0) && (lcc1.cc.date.year == e9))
				{
					lcc1.cc.date.gigayear++;
					lcc1.cc.date.year = 0;
				}
			}
		}
	}
	// Assign month and day of month
	lcc1.cc.date.month = (uint8_t)month;
	lcc1.cc.date.dayOfMonth = (uint8_t)dayOfMonth;

	return lcc1;
}

int compareLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	/* Compare the elements of 2 local datetime coordinates.
	Return 1 if lcc1 > lcc2
	0 if lcc1 < lcc2
	-1 if lcc1 == lcc2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	UTCOffset GMTOffset1, GMTOffset2, SToWOffset1, SToWOffset2;

	// Check whether time zones are different while at least one of the
	//	frames of reference is not universal or the frames of reference
	//	are different
	//****** Consider change to comparing the timezoneIndex, which will elim the need
	//	  to fill in the timezone in the julianUntilCoords function 
	if (
		(lcc1.timezoneIndex != lcc2.timezoneIndex &&
		(lcc1.frame != 0 || lcc2.frame != 0)) ||
		lcc1.frame != lcc2.frame)
	{
		// Time zones are different while at least one of the frames of reference
		//	is not universal and/or frames of reference are different
		// Determine whether the values have been initialized
		if (lcc1.beforeFlag != 3 && lcc2.beforeFlag != 3)
		{
			// The values have been initialized - i.e., all required
			//	time zone analysis has been performed.
			// Translate both to universal frame of reference and proceed
			if (lcc1.frame == 2)
			{
				// Translate to standard frame of reference
				SToWOffset1 = createUTCOffset(0, 0, lcc1.sToWMinutes / 60, lcc1.sToWMinutes % 60, 0, 0, "s-w");
				lcc1 = offsetLocalCalCoordsDT(lcc1, negateUTCOffset(SToWOffset1));
			}
			if (lcc2.frame == 2)
			{
				// Translate to standard frame of reference
				SToWOffset2 = createUTCOffset(0, 0, lcc2.sToWMinutes / 60, lcc2.sToWMinutes % 60, 0, 0, "s-w");
				lcc2 = offsetLocalCalCoordsDT(lcc2, negateUTCOffset(SToWOffset2));
			}
			if (lcc1.frame == 1)
			{
				// Translate to universal frame of reference
				GMTOffset1 = PeriodTimeZones[lcc1.timezoneIndex].periods[lcc1.periodIndex].gMTOffset;
				lcc1 = offsetLocalCalCoordsDT(lcc1, negateUTCOffset(GMTOffset1));
			}
			if (lcc2.frame == 2)
			{
				// Translate to universal frame of reference
				GMTOffset2 = PeriodTimeZones[lcc2.timezoneIndex].periods[lcc2.periodIndex].gMTOffset;
				lcc2 = offsetLocalCalCoordsDT(lcc2, negateUTCOffset(GMTOffset2));
			}
		}
		else
		{
			// Invalid comparison
			abort();
		}
	}
	//  Determine whether the bOrA values are comparable
	//  If either value is 0, or both values are the same, the
	//	 comparison can be performed whether or not time zone
	//	 initialization has been performed.  Also, if either
	//   value is uninitialized, treat this as bOrA == 0, 
	//   and the comparison can be performed.
	//
	if (lcc1.bOrA != 0 && lcc2.bOrA != 0 && lcc1.bOrA != lcc2.bOrA &&
		lcc1.beforeFlag != 3 && lcc2.beforeFlag != 3)
	{
		// Both values have been initialized and their bOrA values
		//  are > 0, but not equal.  Translate both to universal
		//  frame of reference and proceed.
		if (lcc1.frame == 2)
		{
			// Translate to standard frame of reference
			SToWOffset1 = createUTCOffset(0, 0, lcc1.sToWMinutes / 60, lcc1.sToWMinutes % 60, 0, 0, "s-w");
			lcc1 = offsetLocalCalCoordsDT(lcc1, negateUTCOffset(SToWOffset1));
		}
		if (lcc2.frame == 2)
		{
			// Translate to standard frame of reference
			SToWOffset2 = createUTCOffset(0, 0, lcc2.sToWMinutes / 60, lcc2.sToWMinutes % 60, 0, 0, "s-w");
			lcc2 = offsetLocalCalCoordsDT(lcc2, negateUTCOffset(SToWOffset2));
		}
		if (lcc1.frame == 1)
		{
			// Translate to universal frame of reference
			GMTOffset1 = PeriodTimeZones[lcc1.timezoneIndex].periods[lcc1.periodIndex].gMTOffset;
			lcc1 = offsetLocalCalCoordsDT(lcc1, negateUTCOffset(GMTOffset1));
		}
		if (lcc2.frame == 2)
		{
			// Translate to universal frame of reference
			GMTOffset2 = PeriodTimeZones[lcc2.timezoneIndex].periods[lcc2.periodIndex].gMTOffset;
			lcc2 = offsetLocalCalCoordsDT(lcc2, negateUTCOffset(GMTOffset2));
		}
	}
	//  Use the CalCoords comparison function
	return compareCalCoords(lcc1.cc, lcc2.cc);
}

int isEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	// Use compare function to perform equality check
	return (compareLocalCalCoordsDT(lcc1, lcc2) == -1);
}
int isGreaterLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	// Use compare function to perform greater than check
	return (compareLocalCalCoordsDT(lcc1, lcc2) == 1);
}
int isLessLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	// Use compare function to perform less than check
	return (compareLocalCalCoordsDT(lcc1, lcc2) == 0);
}
int isNotEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	// Use equality function
	return !isEqualLocalCalCoordsDT(lcc1, lcc2);
}
int isGreaterOrEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)
{
	// Use less than function
	return !isLessLocalCalCoordsDT(lcc1, lcc2);
}
int isLessOrEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2)

{
	// Use greater than function
	return !isGreaterLocalCalCoordsDT(lcc1, lcc2);
}

// Subtract LocalCalCoordsDTs
TAIRelDatetime diffLocalCalCoordsDTs(LocalCalCoordsDT u1, LocalCalCoordsDT u2)
{
	// Subtract LocalCalCoordsDTs and return a TAIRelDatetime
	UTCDatetime utc1, utc2;

	// Create UTCDatetimes, which will create ticks.
	// LocalCalCoords do not have precision and uncertainty.
	utc1 = createUTCDatetimeFromLocalCalCoordsDT(u1, 99, 0);
	utc2 = createUTCDatetimeFromLocalCalCoordsDT(u2, 99, 0);

	// Subtract the ticks 
	TAIRelTicks r1 = diffTicks(utc1.tai, utc2.tai);

	// Derive the TAIRelDatetime from the reltick value
	return deriveTAIRelDatetime(r1, 99, 0);
}

LocalCalCoordsDT addRelToLocalCalCoordsDT(LocalCalCoordsDT u1, TAIRelDatetime re1, uint8_t futureAdjust)
{
	// Add a TAI relative datetime to a LocalCalCoordsDT and return a LocalCalCoordsDT.
	//	Allow for a futureAdjust value different than the input UTC LocalCalCoords, but
	//	keep the same calendar.

	UTCDatetime utc1, utc2;

	// Create a UTCDatetime, which will create ticks.
	// LocalCalCoords do not have precision and uncertainty.
	utc1 = createUTCDatetimeFromLocalCalCoordsDT(u1, 99, 0);

	// Add the tick values
	TAITicks t1 = addRelTicksToTicks(utc1.tai, re1.relTicks);

	// Derive a UTCDatetime from the tick value
	//  If there was an overflow, t1 will be set to EndOfTimePlus, triggering an error
	//		in deriveLocalCalCoordsDT
	utc2 = deriveUTCDatetime(t1, 99, 0, futureAdjust);

	// Create a LocalCalCoordsDT from utc2
	return createLocalCalCoordsDTFromUTCDatetime(utc2, u1.timezone, u1.frame, u1.cc.date.calendar, futureAdjust);
}

LocalCalCoordsDT subtractRelFromLocalCalCoordsDT(LocalCalCoordsDT u1, TAIRelDatetime re1, uint8_t futureAdjust)
{
	// Subtract a TAI relative datetime from a LocalCalCoordsDT and return a LocalCalCoordsDT.
	//	Allow for a futureAdjust value different than the input UTC LocalCalCoords, but
	//	keep the same calendar.

	UTCDatetime utc1, utc2;

	// Create a UTCDatetime, which will create ticks.
	// LocalCalCoords do not have precision and uncertainty.
	utc1 = createUTCDatetimeFromLocalCalCoordsDT(u1, 99, 0);

	// Add the tick values
	TAITicks t1 = subtractRelTicksFromTicks(utc1.tai, re1.relTicks);

	// Derive a UTCDatetime from the tick value
	//  If there was an overflow, t1 will be set to EndOfTimePlus, triggering an error
	//		in deriveLocalCalCoordsDT
	utc2 = deriveUTCDatetime(t1, 99, 0, futureAdjust);

	// Create a LocalCalCoordsDT from utc2
	return createLocalCalCoordsDTFromUTCDatetime(utc2, u1.timezone, u1.frame, u1.cc.date.calendar, futureAdjust);
}

const char * asStringLocalCalCoordsDT(LocalCalCoordsDT lcc, char stringCal[])
{
	// Format a LocalCalCoordsDT as a readable string
	//
	// * stringUTC must be dimensioned at least 130 
	// Use asStringDatetime
	//
	return asStringDatetime(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth,
		lcc.cc.time.hour, lcc.cc.time.minute, lcc.cc.time.second, lcc.cc.time.nanosecond, lcc.cc.time.attosecond,
		lcc.cc.date.calendar, lcc.frame, -2, 0, lcc.timezone, lcc.bOrA, stringCal);
}

LocalCalCoordsDT offsetLocalCalCoordsDT_old(LocalCalCoordsDT lcc, UTCOffset off)
{
	//**** Deprecated version, which extends calendars when offset extend into
	//		skipped days due Julian to Gregorian transitions.  New version
	//      does not change calendar.
	/* Apply a positive or negative UTCOffset to a LocalCalCoordsDT to
	translate between either a local wall time, local standard time,
	or universal datetime, and a local wall time, local standard time, or
	universal datetime.  Here, we differentiate between a univeral datetime
	and a UTC datetime.  A UTC datetime is a univeral datetime with
	calendar == 0, aka proleptic Gregorian.

	The fromBasis and lcc1.frame arguments indicate
	the direction of the conversion as follows:
	0 = universal datetime
	1 = local standard time
	2 = local wall time

	Several factors must be noted.  First, a translation occurs at
	an instant in time.  Even though the calculation appears to move
	the time forward or backward, in fact what is occurring is a
	translation across coordinate sytems.

	Second, offsets are defined in a particular direction, as follows:

	1. A GMT offset is added to universal (on a consistent calendar) to get local standard time
	2. A daylight savings time offset is added to standard wall time to get local wall time

	Third, the type of an offset is defined by the from and to basis, but the
	sign is independent of the direction of translation.  Thus, for example,
	when translating from standard time to wall time, the offset is a daylight
	savings time offset, and the offset is added to the CalCoords.  When translating
	from wall time to standard time, the offset is also a daylight savings time offset,
	but the offset is subtracted from CalCoords (alternatively, the negative of the
	offset is added to CalCoords).

	Fourth, given that universal is continuous across changes in time zone periods,
	a translation from universal to universal is a no op.

	Fifth, validity, as defined by the transition from Julian to Gregorian is defined
	in terms of local wall time.

	This computation is different from one that adds relative
	calendar coordinates to calendar cooridnates.  That calculation
	involves moving along the datetime axis within a coordinate system, while
	this calcuation is used to translate between coordiante systems at
	the same point in time.

	As this computation takes place at a point in time, daylight savings
	time transitions and intervening leap seconds are not considered.  In
	fact, one of the primary uses of this computation is to perfrom
	daylight savings time transitions.

	We do need to consider whether the adjusted time is outside the
	allowed range and whether a coordinate specified within the 61st second
	is during a leap second.

	In the transition from the Julian to Gregorian calendar, days are
	skipped, which results in an interval of invalid dates.  We assume
	the skipped days are on even day boundaries in local wall time
	coordinates.  E.g., if the calendar transition took place on
	June 2nd, 1642 Julian, the next day is June 13th, 1642 Gregorian.
	Thus, the dates from June 3 - June 12th, 1642 are invalid.  We assume
	the transiton occurs at 2-Jun-1642 23:59:59.999999999 999999999 local
	wall time.  Thus, 3-Jun-1642 00:00:00 is an invalid local wall time.
	If the GMT Offset is -3:00:00 and there is 1 hour of daylight savings time,
	the period of invalidity begins at 2-Jun-1642 23:00:00 local standard time
	and 3-Jun-1642 02:00:00 UTC.

	As a result of the above, validity (as a result of skipped days) should
	only be considered when translating to wall time.  If the translation is
	from wall time, the validity should be checked upon entry to the function
	and an error returned if the date is not valid.  In all other cases,
	the from and to values should be considered valid, i.e., not to be
	on skipped days.

	Thus, on a translation from wall time, when the day needs to be incremented
	or decremented, the next or previous day, respectively, on a full calendar
	(i.e., one without skipped days) should be used.  On a translation to wall
	time, when the day needs to be incremented or decremented, move to the
	next valid wall day.

	In the above example, 2-Jun-1642 23:00:00 local wall time translates to
	3-Jun-1642 01:00:00 UTC, even though 3-Jun-1642 is an invalid date in
	local wall time.  3-Jun-1642 05:00:00 UTC, however, translates to
	13-Jun-1642 03:00:00 local wall time.

	If cc1 is a UTCDatetime (i.e., if fromUTC is true), assume it was
	previously checked and is within the allowed date range for a
	UTCDatetime.

	The valid offset calculations are as follows:

	u + u-s = s  0 + 0 = 1
	s + s-u = s  1 + 6 = 0
	s + s-w = w  1 + 1 = 2
	w + w-s = s  2 + 7 = 1
	u + u-w = w  0 + 2 = 2
	w + w-u = u  2 + 8 = 0 */

	int32_t second, minute, hour, daysInYear, dayOfYear, doySave;
	DateCoords date, datecc, datedy;

	// Initialize lcc1 to lcc
	LocalCalCoordsDT lcc1 = lcc;

	// Establish the frame of the result
	if (lcc.frame == 0 && off.offsetType == 0)
	{
		// u + u-s = s
		lcc1.frame = 1;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigS == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 1 && off.offsetType == 6)
	{
		// s + s-u = u
		lcc1.frame = 0;
		lcc1.bOrA = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 1)
	{
		// s + s-w = w
		lcc1.frame = 2;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigW == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 2 && off.offsetType == 7)
	{
		// w + w-s = s
		lcc1.frame = 1;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigS == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 0 && off.offsetType == 2)
	{
		// u + u-w = w
		lcc1.frame = 2;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigW == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 2 && off.offsetType == 8)
	{
		// w + w-u = u
		lcc1.frame = 0;
		lcc1.bOrA = 0;
	}
	else
	{
		// Invalid calculation
		lcc1.frame = 3;
		lcc1.lccInit |= InvalidTimeFrame;
		return lcc1;
	}

	// Assume cc is valid - i.e., has passed checkCalCoordsElements
	//
	// Determine the day of the year for cc
	//	The below will return the correct value in for all time frames, 
	//		and for calendars adjusted due to translation
	//    Cannot use createDateCoords in the following, because do not want checkDateElements
	//		to fail due to skipped date check.
	//
	datedy.calendar = lcc.cc.date.calendar;
	datedy.gigayear = lcc.cc.date.gigayear;
	datedy.year = lcc.cc.date.year;
	datedy.month = lcc.cc.date.month;
	datedy.dayOfMonth = lcc.cc.date.dayOfMonth;
	datedy.dateInit = 0;
	dayOfYear = dayOfYearFromDate(datedy);
	doySave = dayOfYear;

	if (off.isNegative)
	{
		// The offset is negative

		// Start with the smallest element to facilitate borrows
		if (lcc1.cc.time.nanosecond >= e7 * (uint32_t)off.hundredths)
		{
			lcc1.cc.time.nanosecond -= e7 * off.hundredths;
			second = lcc1.cc.time.second;
		}
		else
		{
			// Need to borrow from second
			lcc1.cc.time.nanosecond += e9 - e7 * off.hundredths;
			second = lcc1.cc.time.second - 1;
		}
		// Now compute second
		second -= off.seconds;
		if (second >= 0)
		{
			lcc1.cc.time.second = second;
			minute = lcc1.cc.time.minute;
		}
		else
		{
			// Borrow from minute
			/* This calculation becomes more complicated if we allow for the situation where the minute
			from which the borrow is taking place is a leap minute (which has 61 seconds).  However,
			since UTCOffsets all were adjusted to an even number of minutes before the advent of leap
			seconds, this cannot happen, so ignore this case.*/
			lcc1.cc.time.second = 60 + second;
			minute = lcc1.cc.time.minute - 1;
		}
		// Now compute minute
		minute -= off.minutes;
		if (minute >= 0)
		{
			lcc1.cc.time.minute = minute;
			hour = lcc1.cc.time.hour;
		}
		else
		{
			// Borrow from hour
			lcc1.cc.time.minute = 60 + minute;
			hour = lcc1.cc.time.hour - 1;
		}
		// Now compute hour
		hour -= off.hours;
		if (hour >= 0)
		{
			lcc1.cc.time.hour = hour;
		}
		else
		{
			// Borrow from day of year
			lcc1.cc.time.hour = 24 + hour;
			dayOfYear--;
		}
		// Now compute day of year
		dayOfYear -= off.days;
		// Check whether day of year is before the beginning of the year
		if (dayOfYear <= 0)
		{
			// Borrow from year
			lcc1.cc.date.year--;
			// Determine whether gigayear needs to be decremented
			//	For negative gigayears, year ranges from -999999999 to 0, so a borrow is required at year == -e9
			//	For gigayear == 0, year ranges from -99999999 to 99999999, so a borrow is required at year == -e9.
			if ((lcc1.cc.date.gigayear <= 0) && (lcc1.cc.date.year == -e9))
			{
				lcc1.cc.date.gigayear--;
				lcc1.cc.date.year = 0;
			}
			//	For gigayear > 0, year ranges from 0 to 99999999, so a borrow is required at year == -1.
			if ((lcc1.cc.date.gigayear > 0) && (lcc1.cc.date.year == -1))
			{
				lcc1.cc.date.gigayear--;
				lcc1.cc.date.year = e9 - 1;
			}
			// Determine the number of days in the year
			daysInYear = numDaysInYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, lcc1.cc.date.calendar);
			dayOfYear += daysInYear;
		}
	}
	else
	{
		// The offset is positive

		// Start with the smallest element to minimize the number of calculations
		lcc1.cc.time.nanosecond += e7 * off.hundredths;
		if (lcc1.cc.time.nanosecond >= e9)
		{
			// Perform carry
			lcc1.cc.time.nanosecond -= e9;
			lcc1.cc.time.second++;
		}
		// As leap seconds were introduced after all countries transitioned to offsets in even
		//	numbers of minutes, an offset with non zero seconds and hundredths will never be
		//	applied to a leap second, or produce a leap second.  However, an offset may still
		//	be applied to a leap second, so can't just check for lcc1.cc.time.second > 59
		second = lcc1.cc.time.second + off.seconds;
		if (lcc1.cc.time.second != 60 || off.hundredths != 0)
		{
			if (second > 59)
			{
				// Perform carry
				lcc1.cc.time.second = second - 60;
				lcc1.cc.time.minute++;
			}
			else lcc1.cc.time.second = second;
		}
		lcc1.cc.time.minute += off.minutes;
		if (lcc1.cc.time.minute > 59)
		{
			// Perform carry
			lcc1.cc.time.minute -= 60;
			lcc1.cc.time.hour++;
		}
		lcc1.cc.time.hour += off.hours;
		if (lcc1.cc.time.hour > 23)
		{
			// Perform carry
			lcc1.cc.time.hour -= 24;
			dayOfYear++;
		}
		dayOfYear += off.days;
		// Determine whether dayOfYear is past the end of the year
		daysInYear = numDaysInYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, lcc1.cc.date.calendar);
		if (dayOfYear > daysInYear)
		{
			// Perform carry
			dayOfYear -= daysInYear;
			lcc1.cc.date.year++;
		}
		// Determine whether the gigayear needs to be incremented
		//	For negative gigayears, year ranges from -999999999 to 0, so a carry is required at year == 1
		if ((lcc1.cc.date.gigayear < 0) && (lcc1.cc.date.year == 1))
		{
			lcc1.cc.date.gigayear++;
			lcc1.cc.date.year = 1 - e9;
		}
		//	For gigayear == 0, year ranges from -99999999 to 99999999, so a carry is required at year == e9.
		//	For gigayear > 0, year ranges from 0 to 99999999, so a carry is required at year == e9.
		if ((lcc1.cc.date.gigayear >= 0) && (lcc1.cc.date.year == e9))
		{
			lcc1.cc.date.gigayear++;
			lcc1.cc.date.year = 0;
		}
	}
	// Derive month and day of month from day of year
	//
	//	Determine whether day of year has changed
	if (dayOfYear != doySave)
	{
		// Day of year has changed, so month and day of month must be recalculated
		// If lcc1.frame is wall or if the calendar already has been extended,
		//  we can use createDayCoordsFromDayOfYear to determine
		//	the month and day of month
		if (lcc1.frame == 2 || lcc1.cc.date.calendar >= e9)
		{
			// createDateCoordsFromDayOfYear provides valid dates on a wall time basis
			//	even when a transition between Julian and Gregorian calendars is involved
			date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
			lcc1.cc.date.month = date.month;
			lcc1.cc.date.dayOfMonth = date.dayOfMonth;
		}
		else
		{
			// If lcc1.frame is either universal or standard time, increment or decrement
			//	the date, ignoring skipped days for a transition between Julian and
			//	Gregorian calendars.  This has the effect of extending the range of validity
			//	by 1 day, which is appropriate since the transition from Julian to Gregorian
			//	calendars is assumed to take place at midnight wall time.
			//
			if (off.isNegative)
			{
				// The translation has the effect of moving to the previous day
				//	Determine whether cc is the first Gregorian date
				datecc = createDateCoords(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth, lcc.cc.date.calendar);
				if (isEqualDateCoords(datecc, firstGregorianDate(lcc.cc.date.calendar)))
				{
					// Decrement the date by 1.  Cannot use dayOfYear.
					//	Use skipGregorianDays function
					date = skipGregorianDays(numDaysToSkip(lcc1.cc.date.calendar) - 1,
						lastJulianDate(lcc1.cc.date.calendar));
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
					//  Extend the calendar
					lcc1.cc.date.calendar += 2 * e9;
				}
				else
				{
					// Use dayOfYear
					date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
				}
			}
			else
			{
				// The translation has the effect of moving to the next day
				//  Determine whether cc is the last Julian date
				datecc = createDateCoords(lcc.cc.date.gigayear, lcc.cc.date.year, lcc.cc.date.month, lcc.cc.date.dayOfMonth, lcc.cc.date.calendar);
				if (isEqualDateCoords(datecc, lastJulianDate(lcc.cc.date.calendar)))
				{
					// Increment the date by 1.  Cannot use dayOfYear.
					date = skipGregorianDays(0, lastJulianDate(lcc.cc.date.calendar));
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
					//  Extend the calendar
					lcc1.cc.date.calendar += e9;
				}
				else
				{
					// Use dayOfYear
					date = createDateCoordsFromDayOfYear(lcc1.cc.date.gigayear, lcc1.cc.date.year, dayOfYear, lcc1.cc.date.calendar);
					lcc1.cc.date.month = date.month;
					lcc1.cc.date.dayOfMonth = date.dayOfMonth;
				}
			}
		}
	}
	//
	// If the input value is on an extended calendar, check whether the translated value
	//	needs to remain on an extended calendar.  Assume that when the day changes
	//	the calendar no longer needs to be extended.  Otherwise, this would imply a
	//	translation has taken place that moves by more than 1 day, which is not allowed.
	//
	if (lcc.cc.date.calendar >= e9 && dayOfYear != doySave) lcc1.cc.date.calendar = lcc1.cc.date.calendar % e9;
	return lcc1;
}

LocalCalCoordsDT offsetLocalCalCoordsDT(LocalCalCoordsDT lcc, UTCOffset off)
{
	/* Apply a positive or negative UTCOffset to a LocalCalCoordsDT to
	translate between either a local wall time, local standard time,
	or universal datetime, and a local wall time, local standard time, or
	universal datetime.  Here, we differentiate between a univeral datetime
	and a UTC datetime.  A UTC datetime is a univeral datetime with
	calendar == 0, aka proleptic Gregorian.

	Several factors must be noted.  First, a translation occurs at
	an instant in time.  Even though the calculation appears to move
	the time forward or backward, in fact what is occurring is a
	translation across coordinate sytems.

	Second, offsets are defined in a particular direction, as follows:

	1. A GMT offset is added to universal (on a consistent calendar) to get local standard time
	2. A daylight savings time offset is added to standard wall time to get local wall time

	Third, the type of an offset is defined by the from and to basis, but the
	sign is independent of the direction of translation.  Thus, for example,
	when translating from standard time to wall time, the offset is a daylight
	savings time offset, and the offset is added to the CalCoords.  When translating
	from wall time to standard time, the offset is also a daylight savings time offset,
	but the offset is subtracted from CalCoords (alternatively, the negative of the
	offset is added to CalCoords).

	Fourth, given that universal is continuous across changes in time zone periods,
	a translation from universal to universal is a no op.

	Fifth, a translation from one frame of reference to another does not require skipping
	days due to a Julian to Gregorian calendar transition. For example, consider a calendar
	where the last Julian date is 1/10/1919 and the first Gregorian date is 1/24/1919 and 
	the timezone has a 2 hour GMT offset and 1 hour s-w offset on 1/24/1919.  The transition
	to the Gregorian calendar occurs at 1/24/1919 00:00 (w).  If we translate 1/24/1919 00:15 to
	standard time, the result is 1/23/1919 23:15 (s).  Similarly, if we translate 1/24/1919 00:15 to
	univeral time, the result is 1/23/1919 21:15 (u).  

	This computation is different from one that adds relative
	calendar coordinates to calendar cooridnates.  That calculation
	involves moving along the datetime axis within a coordinate system, while
	this calcuation is used to translate between coordiante systems at
	the same point in time.

	As this computation takes place at a point in time, daylight savings
	time transitions and intervening leap seconds are not considered.  In
	fact, one of the primary uses of this computation is to perfrom
	daylight savings time transitions.

	The valid offset calculations are as follows:

	u + u-s = s  0 + 0 = 1
	s + s-u = s  1 + 6 = 0
	s + s-w = w  1 + 1 = 2
	w + w-s = s  2 + 7 = 1
	u + u-w = w  0 + 2 = 2
	w + w-u = u  2 + 8 = 0 */

	int32_t second, minute, hour, dayOfMonth, month, daysInPrevMonth, daysInMonth;

	// Initialize lcc1 to lcc
	LocalCalCoordsDT lcc1 = lcc;

	// Establish the frame of the result
	if (lcc.frame == 0 && off.offsetType == 0)
	{
		// u + u-s = s
		lcc1.frame = 1;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigS == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 1 && off.offsetType == 6)
	{
		// s + s-u = u
		lcc1.frame = 0;
		lcc1.bOrA = 0;
	}
	else if (lcc.frame == 1 && off.offsetType == 1)
	{
		// s + s-w = w
		lcc1.frame = 2;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigW == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 2 && off.offsetType == 7)
	{
		// w + w-s = s
		lcc1.frame = 1;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigS == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 0 && off.offsetType == 2)
	{
		// u + u-w = w
		lcc1.frame = 2;
		// This function can be called before or after beforeFlag, ambigS and ambigW have been established.
		//	If before, preserve bOrA.  Otherwise, set appropriately.
		if (lcc1.beforeFlag < 2) lcc1.bOrA = (lcc1.ambigW == 1 ? 2 - lcc1.beforeFlag : 0);
	}
	else if (lcc.frame == 2 && off.offsetType == 8)
	{
		// w + w-u = u
		lcc1.frame = 0;
		lcc1.bOrA = 0;
	}
	else
	{
		// Invalid calculation
		lcc1.frame = 3;
		lcc1.lccInit |= InvalidTimeFrame;
		return lcc1;
	}

	// Assume cc is valid - i.e., has passed checkCalCoordsElements
	//
	dayOfMonth = lcc.cc.date.dayOfMonth;
	if (off.isNegative)
	{
		// The offset is negative

		// Start with the smallest element to facilitate borrows
		if (lcc1.cc.time.nanosecond >= e7 * (uint32_t)off.hundredths)
		{
			lcc1.cc.time.nanosecond -= e7 * off.hundredths;
			second = lcc1.cc.time.second;
		}
		else
		{
			// Need to borrow from second
			lcc1.cc.time.nanosecond += e9 - e7 * off.hundredths;
			second = lcc1.cc.time.second - 1;
		}
		// Now compute second
		second -= off.seconds;
		if (second >= 0)
		{
			lcc1.cc.time.second = second;
			minute = lcc1.cc.time.minute;
		}
		else
		{
			// Borrow from minute
			/* This calculation becomes more complicated if we allow for the situation where the minute
			from which the borrow is taking place is a leap minute (which has 61 seconds).  However,
			since UTCOffsets all were adjusted to an even number of minutes before the advent of leap
			seconds, this cannot happen, so ignore this case.*/
			lcc1.cc.time.second = 60 + second;
			minute = lcc1.cc.time.minute - 1;
		}
		// Now compute minute
		minute -= off.minutes;
		if (minute >= 0)
		{
			lcc1.cc.time.minute = minute;
			hour = lcc1.cc.time.hour;
		}
		else
		{
			// Borrow from hour
			lcc1.cc.time.minute = 60 + minute;
			hour = lcc1.cc.time.hour - 1;
		}
		// Now compute hour
		hour -= off.hours;
		if (hour >= 0)
		{
			lcc1.cc.time.hour = hour;
		}
		else
		{
			// Borrow from day of month
			lcc1.cc.time.hour = 24 + hour;
			dayOfMonth--;
		}
		// Now compute day of month
		dayOfMonth -= off.days;
		// Check whether day of month is before the beginning of the month
		if (dayOfMonth < 1)
		{
			// Move to the previous month.  Assume offset is small enough that there is no
			//	need to move more than 1 month
			month = lcc1.cc.date.month - 1;
			// Determine whether year needs to be decremented
			if (month == 0)
			{
				// Year needs to be decremented
				month = 12;
				dayOfMonth += 31;
				lcc1.cc.date.year--;
				// Determine whether gigayear needs to be decremented
				//	For negative gigayears, year ranges from -999999999 to 0, so a borrow is required at year == -e9
				//	For gigayear == 0, year ranges from -99999999 to 99999999, so a borrow is required at year == -e9.
				if ((lcc1.cc.date.gigayear <= 0) && (lcc1.cc.date.year == -e9))
				{
					lcc1.cc.date.gigayear--;
					lcc1.cc.date.year = 0;
				}
				//	For gigayear > 0, year ranges from 0 to 99999999, so a borrow is required at year == -1.
				if ((lcc1.cc.date.gigayear > 0) && (lcc1.cc.date.year == -1))
				{
					lcc1.cc.date.gigayear--;
					lcc1.cc.date.year = e9 - 1;
				}
			}
			else
			{
				// Remain in the same year, but move to the previous month
				daysInPrevMonth = 31;
				// April, June, September and November only have 30 days
				if (month == 4 || month == 6 || month == 9 || month == 11) daysInPrevMonth = 30;
				// February 
				if (month == 2) daysInPrevMonth = 28 + numLeapDays(0, lcc1.cc.date.year,
					lcc1.cc.date.calendar);
				dayOfMonth += daysInPrevMonth;
			}
		}
		else
		{
			// Remain in the current month
			month = lcc1.cc.date.month;
		}
	}
	else
	{
		// The offset is positive

		// Start with the smallest element to minimize the number of calculations
		lcc1.cc.time.nanosecond += e7 * off.hundredths;
		if (lcc1.cc.time.nanosecond >= e9)
		{
			// Perform carry
			lcc1.cc.time.nanosecond -= e9;
			lcc1.cc.time.second++;
		}
		// As leap seconds were introduced after all countries transitioned to offsets in even
		//	numbers of minutes, an offset with non zero seconds and hundredths will never be
		//	applied to a leap second, or produce a leap second.  However, an offset may still
		//	be applied to a leap second, so can't just check for lcc1.cc.time.second > 59
		second = lcc1.cc.time.second + off.seconds;
		if (lcc1.cc.time.second != 60 || off.hundredths != 0)
		{
			if (second > 59)
			{
				// Perform carry
				lcc1.cc.time.second = second - 60;
				lcc1.cc.time.minute++;
			}
			else lcc1.cc.time.second = second;
		}
		lcc1.cc.time.minute += off.minutes;
		if (lcc1.cc.time.minute > 59)
		{
			// Perform carry
			lcc1.cc.time.minute -= 60;
			lcc1.cc.time.hour++;
		}
		lcc1.cc.time.hour += off.hours;
		if (lcc1.cc.time.hour > 23)
		{
			// Perform carry
			lcc1.cc.time.hour -= 24;
			dayOfMonth++;
		}
		dayOfMonth += off.days;
		// Determine whether dayOfMonth is past the end of the month
		daysInMonth = 31;
		month = lcc1.cc.date.month;
		// April, June, September and November only have 30 days
		if (month == 4 || month == 6 || month == 9 || month == 11) daysInMonth = 30;
		// February 
		if (month == 2) daysInMonth = 28 + numLeapDays(0, lcc1.cc.date.year,
			lcc1.cc.date.calendar);
		if (dayOfMonth > daysInMonth)
		{
			// Move to the next month
			month++;
			dayOfMonth -= daysInMonth;
			// Determine whether the year needs to be incremented
			if (month > 12)
			{
				// Increment the year
				month = 1;
				lcc1.cc.date.year++;
				// Determine whether the gigayear needs to be incremented
				//	For negative gigayears, year ranges from -999999999 to 0, so a carry is required at year == 1
				if ((lcc1.cc.date.gigayear < 0) && (lcc1.cc.date.year == 1))
				{
					lcc1.cc.date.gigayear++;
					lcc1.cc.date.year = 1 - e9;
				}
				//	For gigayear == 0, year ranges from -99999999 to 99999999, so a carry is required at year == e9.
				//	For gigayear > 0, year ranges from 0 to 99999999, so a carry is required at year == e9.
				if ((lcc1.cc.date.gigayear >= 0) && (lcc1.cc.date.year == e9))
				{
					lcc1.cc.date.gigayear++;
					lcc1.cc.date.year = 0;
				}
			}
		}
	}
	// Assign month and day of month
	lcc1.cc.date.month = (uint8_t)month;
	lcc1.cc.date.dayOfMonth = (uint8_t)dayOfMonth;

	return lcc1;
}

int compareLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	/* Compare the elements of an LocalCalCoordsDT and a LocalCalCords.
	Return 1 if lcc1 > lcc2
	0 if lcc1 < lcc2
	-1 if lcc1 == lcc2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	// Check whether time zones are different while at least one of the
	//	frames of reference is not universal or the frames of reference
	//	are different
	if (
		((strncmp(lcc1.timezone, lcc2.timezone, maxTZNameLength) != 0) &&
		(lcc1.frame != 0 || lcc2.frame != 0)) ||
		lcc1.frame != lcc2.frame)
	{
		// Time zones are different while at least one of the frames of reference
		//	is not universal and/or frames of reference are different
		// Invalid comparison
		abort();
	}
	//  Use the CalCoords comparison function
	return compareCalCoords(lcc1.cc, lcc2.cc);
}

int isEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform equality check
	return (compareLDTLCC(lcc1, lcc2) == -1);
}
int isGreaterLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform greater than check
	return (compareLDTLCC(lcc1, lcc2) == 1);
}
int isLessLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	// Use compare function to perform less than check
	return (compareLDTLCC(lcc1, lcc2) == 0);
}
int isNotEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	// Use equality function
	return !isEqualLDTLCC(lcc1, lcc2);
}
int isGreaterOrEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)
{
	// Use less than function
	return !isLessLDTLCC(lcc1, lcc2);
}
int isLessOrEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2)

{
	// Use greater than function
	return !isGreaterLDTLCC(lcc1, lcc2);
}

int createTimeZonePeriods(void)
{
	// Create the time zone periodss array from the rules and timezones arrays
	uint32_t numNames, i;
	uint8_t numTZVersions, hasDSTR, isRuleEffAtUntil, nRuleVer, numPerVers, numPeriods, j,
		endRule;
	uint32_t vers, ruleSetIndex, ruleVersionIndex, prevRuleSetIndex, prevRuleVersionIndex,
		untilYear, lastApplicableYear, maxTZPeriods, maxVersPeriods, numTZPeriods;
	uint32_t rsind, rvind, fromYear, nextRuleSetIndex, untilYearPrevVers;
	TZPeriod p, prevPeriod, versionPeriods[maxTimeZoneVersionPeriods];  // Size was determined by running the function
	TimeZone tzver;
	TZRule rulever;
	LocalCalCoords lccTZVerUntil, lccNextRuleEffective,
		lccPrevTZVerUntil, lccFirstEffective, lccPeriodUntil, lccBegYear,
		lccUntilCoordsJ, lccFromCoordsJP1;
	UTCOffset stow, gMTOffset, stowBefore, stowZero, begJump;

	// Create a zero stow UTCOffset for convenience
	stowZero = createUTCOffset(0, 0, 0, 0, 0, 0, "s-w");

	// Initialize the counters
	maxTZPeriods = 0; // Maximum number of periods computed for a time zone
	maxVersPeriods = 0; // Maximum number of periods computed for a time zone version

						// Loop through the time zones
	for (i = 0; i < numTimeZones; i++)
	{
		// Initialize the period counter for this time zone
		numTZPeriods = 0;
		// Count the number of time zone names
		numNames = 0;
		for (j = 0; j < maxNamesPerTimeZone; j++)
		{
			if (strlen(TimeZones[i][j]) == 0) break;
			numNames++;
		}
		PeriodTimeZones[i].numNames = numNames;
		// Establish the names array
		strcpy_s(PeriodTimeZones[i].names[0], sizeof(TimeZones[i]), TimeZones[i][0]);
		// Create the time zone periods
		//
		// Loop through the time zone versions
		numTZVersions = TimeZoneIndices[i][1] - TimeZoneIndices[i][0] + 1;
		numPeriods = 0;
		// Initialize the until datetime of the (tzvind - 1) version to the beginning of year 1
		lccPrevTZVerUntil = createLocalCalCoords(0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
			TimeZones[i][0], 'w');
		for (uint8_t tzvind = 0; tzvind < numTZVersions; tzvind++)
		{
			// Establish the fields that apply to all periods within the time zone version
			vers = TimeZoneIndices[i][0] + tzvind;
			tzver = TimeZoneVersions[vers];
			// Determine whether the time zone version has a DSTR
			hasDSTR = strlen(tzver.daylightSavingsTimeRule) != 0;
			// Establish the GMT Offset
			//	Create a UTCOffset from the fields in the time zone version
			//	 Assume the values are correct, so no need to check for errors on return
			gMTOffset = createUTCOffset(tzver.isNegativeGMTOffset, 0, tzver.hoursOfGMTOffset, tzver.minutesOfGMTOffset,
				tzver.secondsOfGMTOffset, tzver.hundredthsOfGMTOffset, "u-s");
			//   Create a LocalCalCoords representing the until time of this time zone version.
			//		The time zone version actually ends at the attosecond before the until time.
			//		The until time would have been the first instant in the first period of the
			//		next time zone version, if there were no change to GMTOffset or savings offset
			//		in the transition to the next time zone version.
			//	 A time zone version without an Until date (often the case for the last version of
			//	 a time zone) will have the year = 1,000,000,000.  In these cases, set the Until
			//	 time to maximum local time.
			//
			if (tzver.untilYear == e9)
			{
				// Set to maximum UTC time adjusted for this time zone
				//
				//	Begin by setting to 1 attosecond beyond maximum UTC time.
				//
				lccTZVerUntil = createLocalCalCoords(100, 0, 1, 1, 0, 0, 0, 0, 0, 0,
					tzver.name, 'u');
			}
			else
			{
				// Create a LocalCalCoords representing the until datetime
				lccTZVerUntil = createLocalCalCoords(0, tzver.untilYear, tzver.untilMonth, tzver.untilDayOfMonth,
					tzver.untilHour, tzver.untilMinute, tzver.untilSecond, tzver.untilHundredth * e7, 0, 0,
					tzver.name, tzver.untilFrame);
			}
			// Set the untilCoords for the first period (which is the last period in the time zone
			//	version) to the untilCoords for the time zone version
			lccPeriodUntil = lccTZVerUntil;
			// Establish the until year for the previous version
			//  Since this until datetime was not derived from a fromCoords,
			//  set beginJump to zero.  BegJump is only used to get the proper
			//  value of untilYear.  Specifically, when the until coords are derived
			//  from a fromCoords for the previous period (in sequence, but the later
			//  period in time) and that fromCoords was set to the instant a year starts,
			//  we want untilYear to be the previous year.  However, if there were a jump
			//  between the periods, the untilCoords will have moved.  The begJump enables
			//  getUntilYear to move the value back to the fromCoords value to determine
			//  the proper year.
			//
			begJump = createUTCOffset(0, 0, 0, 0, 0, 0, "wp1");
			untilYearPrevVers = getUntilYear(lccPrevTZVerUntil, begJump);
			// Initialize the number of periods for the time zone version to 1
			numPerVers = 1;
			// Compute periods working backwards through the time zone version
			//	until the until coords for the previous time zone version is reached
			while (1)
			{
				// Starting point for the processing of a period within the time zone version
				// Initialize rule set index and isRuleEffAtUntil
				ruleSetIndex = numRuleVersionSets;
				isRuleEffAtUntil = 0;
				// Set fields that apply to all periods
				p.tZVersion = vers;
				p.gMTOffset = gMTOffset;
				// Establish untilYear for this period
				//  If this is not the first period in this time zone version, set
				//   begJump to p.beginJump of the last period processed
				//   otherwise, use the zero begJump set above
				if (numPerVers > 1) begJump = versionPeriods[numPerVers - 2].beginJump;
				untilYear = getUntilYear(lccPeriodUntil, begJump);
				// Convert untilCoords to wall frame of reference, if necessary
				//  As a first step, apply GMT Offsset to translate to standard frame 
				//    of reference, if necessary
				if (lccPeriodUntil.frame == 0)
					lccPeriodUntil = offsetLocalCalCoords(lccPeriodUntil, p.gMTOffset);
				//	Find the applicable rule set at the Until date time, if any
				if (hasDSTR)
				{
					//  This function will work for untilYear == e9, since the applicable rule
					//	set would also have a toYear == e9
					ruleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, untilYear);
					//
					//  If there is no applicable rule set in the untilYear, treat as if
					//	  no rule set has been specified.
					isRuleEffAtUntil = (ruleSetIndex != numRuleVersionSets);
					if (!isRuleEffAtUntil)
					{
						// No rule set is applicable during the until year
						p = setPeriodFieldsNoDSTR(p, lccPeriodUntil);
					}
					else
					{
						// Find the effective rule at the until datetime
						// The rule versions in each rule version set have been sorted by effective
						//	month.  This leaves the possiblity that 2 rule versions in a set would
						//	have the same effective month and the sort might not be correct.  This
						//	situation is unlikely, as it would cause 2 clock shifts in a single month.
						//	In fact, none of the rule version sets in the current IANA file have
						//	two rules with effective dates in the same month.
						//
						//	The effective rule could be the rule that was effective at the end
						//	of the previous year, which could be from a different rule set.
						//  
						//	To faciliate this possibility, initialize
						//	previous rule version index to the index of the effective rule
						//	at the end of the last year.
						//
						//  There is an ambiguity regarding the transition between time zone
						//  versions.  When the previous time zone version ends in the
						//  previous year, or even in the until year, we can have a situation
						//  where a version of the rule for the current time zone version is
						//  applicable either in the previous year or in the until year, but
						//  is effective before the untilCoords of the previous time zone
						//  version.  
						//
						//  In this situation, we either can determine the effectiveness of
						//  the rule as if the previous time zone version did not exist, or
						//  only allow the rule to become effective after the end of the
						//  previous time zone version.  
						//
						//  The original implementation took the latter approach, but
						//  consideration of some of the rule sets indicates the former is
						//  likely what is intended.
						//
						//  An example is America/Argentina/San_Luis.  It transitions from
						//  Arg rules to SanLuis rules on 1/21/2008.  This is the only time
						//  zone that uses SanLuis rules.  There is 1 rule that moves to
						//  daylight savings time on the second Sunday in October, which is
						//  in effect from 2007 to 2008.  There is a second rule that moves
						//  back to standard time on the second Sunday in March, which is
						//  in effect from 2008 to 2009.  If we took the latter approach
						//  the specification of a daylight savings time transition in
						//  October, 2007, would have no effect.  There would be a period
						//  between 1/21/2008 and 3/9/2008 where no DSTR was in effect.
						//  This does not seem like what is desired, given that there is 
						//  1 hour of daylight savings time in effect at the end of the
						//  last time zone based on the Arg rules and 1 hour of daylight savings
						//  time in effect after 3/9/2008 based on the SanLuis rule.
						//
						/*  4/2/2018 =========================================================================
						// We need to allow for the possibility that all or part of the previous year is
						//	in the previous time zone version and, as such could have a
						//	different rule.
						//
						if (untilYearPrevVers == untilYear - 1)
						{
						//  The previous time zone version ends during the year
						//	 before this period ends.
						//
						//  First, determine whether any version of the rule
						//   associated with the current time zone version
						//   is  effective during the period of untilYear - 1
						//   after the until datetime of the previous time zone
						//   version.
						//
						//  Only need to check the last rule version of the
						//   applicable rule set in untilYear - 1
						//
						prevRuleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, untilYear - 1);
						if (prevRuleSetIndex != numRuleVersionSets)
						{
						//  The rule set is applicable in unitlYear - 1
						//  We could just check whether the last rule version in the
						//   set is effective after untilCoords.  However,
						//   to do this accurately, we need to know the s-w offset
						//   for the time preceding that rule becoming effective.
						//  To do this, loop through all of the rules in the rule
						//   set using the gmtoffset and endSaved values of the previous time zone
						//   version, until one is found with an effective time
						//   greater than untilCoords.  At this point, there are
						//   2 possibilities:
						//   1. There is no such rule.  In this case, the rule
						//      set is not effective during the part of the year
						//      that would have been in the current period.
						//      Subsequent processing will create a period with
						//      no applicable rule set for the portion of untilYear - 1
						//      after untilCoords.  For the purposes of this section,
						//      set prevRuleSetIndex to numRuleVersionSets.
						//   2. There is at least 1 such rule.  Leave prevRuleSetIndex as is.
						//      PrevRuleVersionIndex is the last rule in the set.
						for (j = 0; j < TZRuleVersionSets[prevRuleSetIndex].numRuleVers; j++)
						{
						ruleVersionIndex = TZRuleVersionSets[prevRuleSetIndex].ruleVersionIndices[j];
						lccNextRuleEffective = createAtLocalCalCoords(
						TZRuleVersions[ruleVersionIndex],
						tzver.name, untilYear - 1, lccPrevTZVerUntil.frame,
						PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
						PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
						if (isGreaterOrEqualLocalCalCoords(lccNextRuleEffective, lccPrevTZVerUntil))
						{
						// This rule becomes effective
						//  after the previous TZ version until datetime
						break;
						}
						}
						if (j == TZRuleVersionSets[prevRuleSetIndex].numRuleVers)
						{
						// No rule becomes effective after untilCoords
						prevRuleSetIndex = numRuleVersionSets;
						}
						}
						}
						else if (untilYearPrevVers == untilYear)
						{
						//  The previous time zone ends during the year this period ends
						//  Findthe last verion of the rule, if any, that becomes
						//   effective between the untilCoords of the previous version
						//   and the end of the period.
						//
						// ***** This section was not completed prior to changing approach
						}
						else
						{
						prevRuleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, untilYear - 1);
						}
						===================================================================4/2/2018 */
						// The following line was added 4/2/2018
						prevRuleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, untilYear - 1);
						// Check whether there was an applicable rule set in the previous year
						if (prevRuleSetIndex == numRuleVersionSets)
						{
							// No applicable rule set in the previous year
							//   Need to consider the special case where the previous time zone version
							//    ends in the untilYear
							if (untilYearPrevVers == untilYear)
							{
								// In this case, the saved offset must be extracted from
								//  the last time zone version
								prevPeriod = PeriodTimeZones[i].periods[numPeriods - 1];
								stowBefore = prevPeriod.endSaved;
							}
							else
							{
								stowBefore = stowZero;
							}
							prevRuleVersionIndex = numRuleVersions;
						}
						else
						{
							// Select the last rule in the previous rule set, as
							//	they are ordered by effective month
							prevRuleVersionIndex = TZRuleVersionSets[prevRuleSetIndex].
								ruleVersionIndices[TZRuleVersionSets[prevRuleSetIndex].numRuleVers - 1];
							rulever = TZRuleVersions[prevRuleVersionIndex];
							stowBefore = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
						} // End of the section to determine starting values for prevRuleVersionIndex
						  //   and stowBefore which will be used for the effective values at
						  //   lccPeriodUntil in the event there is no effective rule in untilYear
						  //   prior to untilCoords.
						// Loop through the rule versions to find the latest effective datetime before
						//	the until datetime in the frame of reference of the until datetime
						//
						//  Initialize the value of endRule, which is the index into the rule
						//	  versions array of the effective rule from the current rule set at
						//	  the untilCoords.  If endRule == numRuleVers, there is no rule from
						//	  the current rule set effective at untilCoords
						endRule = TZRuleVersionSets[ruleSetIndex].numRuleVers;
						for (j = 0; j < TZRuleVersionSets[ruleSetIndex].numRuleVers; j++)
						{
							ruleVersionIndex = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[j];
							lccNextRuleEffective = createAtLocalCalCoords(
								TZRuleVersions[ruleVersionIndex],
								tzver.name, untilYear, lccPeriodUntil.frame, p.gMTOffset, stowBefore);
							if (isGreaterOrEqualLocalCalCoords(lccNextRuleEffective, lccPeriodUntil))
							{
								// The At datetime is beyond the until datetime, so 
								//	the rule version before this is the applicable one
								break;
							}
							prevRuleVersionIndex = ruleVersionIndex;
							rulever = TZRuleVersions[prevRuleVersionIndex];
							stowBefore = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							endRule = j;
						}
						// If j == 0, set ruleSetIndex to prevRuleSetIndex
						if (j == 0)
						{
							ruleSetIndex = prevRuleSetIndex;
							endRule = TZRuleVersionSets[ruleSetIndex].numRuleVers - 1;
						}
						// If prevRuleVersionIndex == numRuleVersions, the earliest rule in the
						//	applicable rule set does not become effective until after the until date
						//	and there is no applicable rule set in the previous year.
						//	Treat as if there is no applicable rule set.
						if (prevRuleVersionIndex == numRuleVersions)
						{
							// No effective DSTR this year or last
							p = setPeriodFieldsNoDSTR(p, lccPeriodUntil);
							ruleSetIndex = numRuleVersionSets;
							isRuleEffAtUntil = 0;
							stowBefore = stowZero;
						}
						else
						{
							//  The next statement works even if we reached the end of the rule set
							rulever = TZRuleVersions[prevRuleVersionIndex];
							// Calculate the s-w offset
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							//  Translate untilCoords to wall frame of reference, if necessary
							p.untilCoords = (lccPeriodUntil.frame == 1 ?
								offsetLocalCalCoords(lccPeriodUntil, stow) : lccPeriodUntil);
							//  Set other period fields
							p.nRuleVersions = TZRuleVersionSets[ruleSetIndex].numRuleVers;
							for (j = 0; j < TZRuleVersionSets[ruleSetIndex].numRuleVers; j++)
							{
								p.ruleVersions[j] = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[j];
							}
							p.endSaved = stow;
							//  p.endRule only is intended to contain the index of the effective rule
							//	 if the rule is in the rule set of the current period.  Where the effective
							//   rule is from the previous period, the index will be numRuleVersions
							p.endRule = endRule;
						}
					}
				}
				else
				{
					// No applicable DSTR
					p.nRuleVersions = 0;
					p.endRule = 0;
					// Create the saved time UTCOffset from tzver
					p.tZSaved = createUTCOffset(0, 0, tzver.hoursSaved, tzver.minutesSaved, 0, 0, "s-w");
					p.endSaved = p.tZSaved;
					p.untilCoords = (lccPeriodUntil.frame == 1 ? offsetLocalCalCoords(lccPeriodUntil,
						p.tZSaved) : lccPeriodUntil);
				}
				// Find the beginning of the period then set p.beginSaved, p.beginJump and p.fromCoords
				//
				/*	3 cases must be considered:

				1. No DSTR is specified for this time zone version
				In this case, the time zone version only has one period, which
				begins at the untilCoords of the previous time zone version.
				2. A DSTR is specifed for this time zone version, but there is
				no effective rule version at untilCoords of this period.
				3. A DSTR is specified for this time zone version and there is
				an effective rule version at untilCoords of this period.
				*/
				if (!hasDSTR)
				{
					// No DSTR specified
					// Set the beginning fields of the period
					// The amount of time saved is constant throughout the time period,
					//	so beginSaved = p.tzSaved.
					if (numPeriods == 0)
					{
						// This is the first time zone version processed for this time zone.
						//	As such, there is no previous period
						p = setPeriodBeginFieldsWithoutPrevPeriod(p, p.tZSaved);
					}
					else
					{
						// This is not the first time zone version processed for this time zone
						//	Set beginning fields for this period
						p = setPeriodBeginFieldsWithPrevPeriod(p, p.tZSaved,
							PeriodTimeZones[i].periods[numPeriods - 1]);
						// No DSTR has been specified, so p.begRule = p.numRules, which is 0
						p.begRule = 0;
					}
					//  Finished computing periods for this time zone version
					break;
				}
				else if (!isRuleEffAtUntil)
				{
					// DSTR has been specified, but there is no effective rule at untilCoords
					// Find the latest year any version of the rule set is applicable before the
					//	until year.
					//	At this point, we know no rule was effective at untilCoords, so we
					//	don't want to find a rule set that is applicable in the until year,
					//	since the earliest rule in that set must not have become effective
					//	before untilCoords.
					//
					lastApplicableYear = lastApplicableYearBeforeUntil(tzver.daylightSavingsTimeRule,
						untilYear);
					if (lastApplicableYear == 2 * e9) abort(); // Error in rule name or a rule version is
															   //   effective at untilCoords
															   // In general, the first version of a time zone has no DSTR;  it is used to
															   //  establish mean solar time and, by its nature of being the first version
															   //  begins at creation.  
															   // There are, however, 8 special cases, e.g., WET, where time zones were created
															   //  for compatibility with early versions of the data.  These time zones have
															   //  only 1 version and have a DSTR.  Once we get beyond (iterating backwards
															   //  through time) the earliest rule version, lastApplicableYearBeforeUntil
															   //  will return 0.  If this occurs and this is the first version of the time zone
															   //  we have found the last period.
															   //
					if (lastApplicableYear == 0 && numPeriods == 0)
					{
						// There is no effective DSTR, so time saved is constant throughout the period
						// This is the first time zone version processed for this time zone.
						//	As such, there is no previous period
						p.tZSaved = createUTCOffset(0, 0, tzver.hoursSaved, tzver.minutesSaved, 0, 0, "s-w");
						p = setPeriodBeginFieldsWithoutPrevPeriod(p, p.tZSaved);
						break;
					}
					// If latest applicable year is < the year of the last time zone until date
					//	the period begins at lccPrevTZVerUntil and period computation is complete
					if (lastApplicableYear < untilYearPrevVers)
					{
						// The period begins at lccPrevTZVerUntil + beginJump and processing
						//	of this time zone version is complete
						// This cannot be the first time zone version of this time zone, since
						//	lccPrevTZVerUntil year would be 0.
						// Set the beginning fields of the period
						// The amount of time saved is constant throughout the time period,
						//	so beginSaved = p.endSaved.
						p = setPeriodBeginFieldsWithPrevPeriod(p, p.endSaved,
							PeriodTimeZones[i].periods[numPeriods - 1]);
						p.begRule = p.nRuleVersions;
						//  Finished computing periods for this time zone version
						break;
					}
					// If latest applicable year == the year of the last time zone version until date,
					//	determine whether the first rule became effective before lccPrevTZVerUntil.	
					else if (lastApplicableYear == untilYearPrevVers)
					{
						// Rule is applicable in the year of the last time zone until date
						// Eliminated the section between the ====, because if a time zone version
						//	begins in year n all that need be considered is whether the rule
						//	set that is applicable in year n becomes effective before untilCoords.
						// For example, if the previous time zone version ends on 3/15/61 and
						//  the first rule for this time zone version does not become effective
						//  until 7/1/61 and the second in 11/1/61, it does not seem appropriate
						//	to say the second rule would have been effective on 11/1/60, and,
						//  therefore is in effect on 3/15/61. 
						/*=======================================================================
						//	Determine whether it was applicable in the preceding year
						// This cannot be the first time zone version processed for this time zone
						rsind = findRuleSetIndex(tzver.daylightSavingsTimeRule, lastApplicableYear);
						if (TZRuleVersionSets[rsind].fromYear != TZRuleVersionSets[rsind].toYear)
						{
						// The rule set was applicable before the until year of the last
						//	time zone, so was effective at untilCoords of the last time zone.
						// The period begins at lccPrevTZVerUntil + beginJump and processing
						//	of this time zone version is complete
						// Set the beginning fields of the period
						// The amount of time saved is constant throughout the time period,
						//	so beginSaved = p.endSaved.
						p = setPeriodBeginFieldsWithPrevPeriod(p, p.endSaved,
						PeriodTimeZones[i].periods[numPeriods - 1]);
						//  Finished computing periods for this time zone version
						break;
						}
						else
						=======================================================================*/
						// The rule set is applicable for the until year of the last
						//	time zone version.  Determine whether it was effective at untilCoords.
						rsind = findRuleSetIndex(tzver.daylightSavingsTimeRule, lastApplicableYear);
						//**** It is not necessary to determine when the first rule becomes effective
						//   as the processing of the current time period is the same.  The difference
						//   between the cases where the rule becomes effective before the untilCoords
						//   and after the untilCoords is how many periods there are, 1 and 2, respectively
						//   between the untilCoords and the end of the year.  However, as the period
						//   immediately before the current period will have the same effective rule,
						//   regardless, the check if not necessary.
						// Find the effective date of the first rule version in lastApplicableYear
						rvind = TZRuleVersionSets[rsind].ruleVersionIndices[0];
						lccFirstEffective = createAtLocalCalCoords(
							TZRuleVersions[rvind], tzver.name, lastApplicableYear, 2,
							PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
						if (isLessOrEqualLocalCalCoords(lccFirstEffective,
							PeriodTimeZones[i].periods[numPeriods - 1].untilCoords))
						{
							// The first rule in the rule set becomes effective before
							//  the untilCoords of the last time zone version, and,
							//  thus is effective at untilCoords.  Since it is only applicable
							//  for that year (as this is the lastApplicableYear for the rule set),
							//  there will be a short time zone period from
							//  the untilCoords of the last time zone version to 1/1 of the
							//  next year.
							// The period begins on 1/1 of the year after the last year
							//  of applicability of the rule set
							p.fromCoords = createLocalCalCoords(0, lastApplicableYear + 1, 1, 1,
								0, 0, 0, 0, 0, 0, tzver.name, 'w');
							p.beginSaved = p.endSaved;
							// We need the gMTOffset and endSaved for the short time period
							//	to compute the remaining fields for this period.
							// GMTOffset is the same.  endSaved is computed by
							//  determining the effective rule version on 12/31 of the lastApplicable Year
							//
							nRuleVer = TZRuleVersionSets[rsind].numRuleVers;
							rvind = TZRuleVersionSets[rsind].ruleVersionIndices[nRuleVer - 1];
							rulever = TZRuleVersions[rvind];
							// Calculate the s-w offset
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							p.beginJump = addUTCOffsets(
								subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
								subtractUTCOffsets(p.beginSaved, stow));
							p.begRule = p.nRuleVersions;
							goto nextPeriod;
						}
						else
						{
							// The first rule in the rule set becomes effective after
							//	untilCoords of the last time zone version.
							//   There will be 2 short periods after untilCoords,
							//    one with no DSTR, which extends from the untilCoords
							//    of the last time zone version until the datetime of
							//    the first effective rule, and one with the DSTR
							//    of the rule set, which extends until the end of the year.
							// Set the beginning fields of the period
							// The period begins on 1/1 of the year after the last year
							//  of applicability of the rule set
							p.fromCoords = createLocalCalCoords(0, lastApplicableYear + 1, 1, 1,
								0, 0, 0, 0, 0, 0, tzver.name, 'w');
							p.beginSaved = p.endSaved;
							// We need the gMTOffset and endSaved for the short time period
							//	to compute the remaining fields for this period.
							// GMTOffset is the same.  endSaved is computed by
							//  determining the effective rule version on 12/31 of the lastApplicable Year
							//
							nRuleVer = TZRuleVersionSets[rsind].numRuleVers;
							rvind = TZRuleVersionSets[rsind].ruleVersionIndices[nRuleVer - 1];
							rulever = TZRuleVersions[rvind];
							// Calculate the s-w offset
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							p.beginJump = addUTCOffsets(
								subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
								subtractUTCOffsets(p.beginSaved, stow));
							p.begRule = p.nRuleVersions;
							goto nextPeriod;
						}
					}
					// If last applicable year > the year of the last time zone until date,
					//	the period begins on 1/1 of the year after the last applicable year.
					//	This could cause a problem if a time zone version lasted less than a year.
					else
					{
						// The rule set is applicable in a year which is after the until
						//	year of the last time zone version.  This period begins on 1/1 of
						//	the year following the last applicable year.
						//
						p.fromCoords = createLocalCalCoords(0, lastApplicableYear + 1, 1, 1,
							0, 0, 0, 0, 0, 0, tzver.name, 'w');
						p.beginSaved = p.endSaved;
						// There will be at least one more period before this
						//	one in this time zone version, defined by the
						//	rule set that was effective in the lastApplicableYear
						// We need the gMTOffset and endSaved for that time period
						//	to compute the remaining fields for this period.
						// GMTOffset is the same.  endSaved is computed by
						//  determine the effective rule version on 12/31 of the lastApplicable Year
						//
						rsind = findRuleSetIndex(tzver.daylightSavingsTimeRule, lastApplicableYear);
						nRuleVer = TZRuleVersionSets[rsind].numRuleVers;
						rvind = TZRuleVersionSets[rsind].ruleVersionIndices[nRuleVer - 1];
						rulever = TZRuleVersions[rvind];
						// Calculate the s-w offset
						stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
							0, 0, "s-w");
						p.beginJump = addUTCOffsets(
							subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
							subtractUTCOffsets(p.beginSaved, stow));
						p.begRule = p.nRuleVersions;
						goto nextPeriod;
					}
				} // End of block for DSTR specified, but not effective
				else
				{
					// DSTR has been specified and there is an effective rule at untilCoords
					// There are 3 possibilities:
					//	1. This is the first period in the time zone version
					//  2. The period before this one has no applicable rule set
					//  3. The period before this one has an applicable rule set
					//
					// If the from year is during the year of the untilCoords of
					//	the last time zone version - if the first rule becomes effective
					//	before untilCoords, this is the first period in the time zone version.
					//	Otherwise there is another period.
					// If the from year is after the year of the untilCoords of
					//	the last time zone version, there is another period.
					// If there is another period, find the to year of the rule set
					//	immediately before the applicable rule set.  If there is no such
					//	rule set, there is a period with no applicable rule set, which is
					//  the first period of the time zone.
					// If there is such a rule set and the to year is the current from year - 1
					//  compute the previous period that has as an applicable rule set.
					//
					// Find the from year of the applicable rule set
					fromYear = TZRuleVersionSets[ruleSetIndex].fromYear;
					// If the from year is before the year of the untilCoords of
					//	the last time zone version, this is the first period in the time zone version.
					if (fromYear < untilYearPrevVers)
					{
						// The rule set becomes effective before
						//  the untilCoords of the last time zone version
						// The period begins at lccPrevTZVerUntil + beginJump and processing
						//	of this time zone version is complete
						// Set the beginning fields of the period
						//
						// Find the effective rule at lccPrevTZVerUntil
						p = pBegNoPeriodTZArray(lccPrevTZVerUntil, tzver, ruleSetIndex,
							PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							PeriodTimeZones[i].periods[numPeriods - 1].endSaved, p);
						p = setPeriodBeginFieldsWithPrevPeriod(p, p.beginSaved,
							PeriodTimeZones[i].periods[numPeriods - 1]);
						//  Finished computing periods for this time zone version
						break;
					}
					else if (fromYear == untilYearPrevVers)
					{
						// The rule set is applicable for the until year of the last
						//	time zone version.  
						/* 4/2/2018 ==================================================
						//Determine whether it was effective at untilCoords.
						rsind = findRuleSetIndex(tzver.daylightSavingsTimeRule, fromYear);
						// Find the effective date of the first rule version in fromYear
						rvind = TZRuleVersionSets[rsind].ruleVersionIndices[0];
						lccFirstEffective = createAtLocalCalCoords(
						TZRuleVersions[rvind], tzver.name, fromYear, 2,
						PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
						PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
						if (isLessOrEqualLocalCalCoords(lccFirstEffective,
						PeriodTimeZones[i].periods[numPeriods - 1].untilCoords))
						{
						// The rule set becomes effective before the untilCoords
						// The period begins at lccPrevTZVerUntil + beginJump and processing
						//	of this time zone version is complete
						// Set the beginning fields of the period
						//
						// Find the effective rule at lccPrevTZVerUntil
						stow = sToWNoPeriodTZArray(lccPrevTZVerUntil, tzver, ruleSetIndex,
						PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
						PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
						p = setPeriodBeginFieldsWithPrevPeriod(p, stow,
						PeriodTimeZones[i].periods[numPeriods - 1]);
						break;
						}
						else
						{
						// The rule set becomes effective after the untilCoords
						//  The period begins when the rule becomes effective
						================================================
						p.fromCoords = lccFirstEffective; // Already in wall frame
						// Find the saved offset at fromCoords
						//   Is either the first or last rule
						rvind = TZRuleVersionSets[rsind].ruleVersionIndices[0];
						lccFirstEffective = createAtLocalCalCoords(
						TZRuleVersions[rvind], tzver.name, fromYear + 1, 2,
						p.gMTOffset);
						// The first rule version cannot be less than fromCoords
						if (isEqualLocalCalCoords(lccFirstEffective, p.fromCoords))
						{
						// The first rule version is effective
						rulever = TZRuleVersions[rvind];
						}
						else
						{
						// The last rule version is effective
						nRuleVer = TZRuleVersionSets[rsind].numRuleVers;
						rvind = TZRuleVersionSets[rsind].ruleVersionIndices[nRuleVer - 1];
						rulever = TZRuleVersions[rvind];
						}
						=================================================
						rulever = TZRuleVersions[rvind];
						p.beginSaved = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
						0, 0, "s-w");
						//  There is no effective rule prior to the effective date,
						//	  so create a zero s-w offset
						stow = createUTCOffset(0, 0, 0, 0, 0, 0, "s-w");
						//  The GMTOffsets are the same, since this is within the same
						//	  time zone version.
						p.beginJump = addUTCOffsets(
						subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
						subtractUTCOffsets(p.beginSaved, stow));
						//  The following is the untilCoords of the next period in wall frame
						p.fromCoords = lccFirstEffective;
						//  Offset by the beginning jump to move to an after basis
						p.fromCoords = offsetLocalCalCoords(p.fromCoords, p.beginJump);
						goto nextPeriod;
						}
						======================================================= 4/2/2018 */
						//  The following section added 4/2/2018
						// The period begins at lccPrevTZVerUntil + beginJump and processing
						//	of this time zone version is complete
						// Set the beginning fields of the period
						//
						// Find the effective rule at lccPrevTZVerUntil
						p = pBegNoPeriodTZArray(lccPrevTZVerUntil, tzver, ruleSetIndex,
							PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							PeriodTimeZones[i].periods[numPeriods - 1].endSaved, p);
						p = setPeriodBeginFieldsWithPrevPeriod(p, p.beginSaved,
							PeriodTimeZones[i].periods[numPeriods - 1]);
						break;
					} // End of block for fromYear == untilYearPrevVers
					else
					{
						// fromYear is after the year of lccPrevTZVerUntil
						//	There is at least one more period in this time zone version
						//	
						//	Determine whether that period has an applicable rule set.
						//	If it does, the rule set will be applicable in the year
						//	before fromYear
						//	
						nextRuleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, fromYear - 1);
						if (nextRuleSetIndex == numRuleVersionSets)
						{
							// There is no applicable rule set in the previous year,
							//	so the next period (i.e., the one before this one chronologically) has no DSTR
							// This period begins on 1/1 of the fromYear
							p.fromCoords = createLocalCalCoords(0, fromYear, 1, 1,
								0, 0, 0, 0, 0, 0, tzver.name, 'w');
							// Find the saved offset at fromCoords
							//   Is either the first or last rule
							rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[0];
							lccFirstEffective = createAtLocalCalCoords(
								TZRuleVersions[rvind], tzver.name, fromYear, 2,
								p.gMTOffset, stowZero);
							// The first rule version only can be effective if it has an At time of 1/1 at 0:00:00
							lccBegYear = createLocalCalCoords(0, fromYear, 1, 1,
								0, 0, 0, 0, 0, 0, tzver.name, 'w');
							if (isEqualLocalCalCoords(lccFirstEffective, lccBegYear))
							{
								// The first rule version is effective
								rulever = TZRuleVersions[rvind];
								p.begRule = 0;
							}
							else
							{
								// The last rule version is effective
								nRuleVer = TZRuleVersionSets[ruleSetIndex].numRuleVers;
								rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[nRuleVer - 1];
								rulever = TZRuleVersions[rvind];
								p.begRule = nRuleVer - 1;
							}
							p.beginSaved = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							//  There is no effectivee rule on 12/31 of the previous year,
							//	  so create a zero s-w offset
							stow = createUTCOffset(0, 0, 0, 0, 0, 0, "s-w");
							//  The GMTOffsets are the same, since this is within the same
							//	  time zone version.
							p.beginJump = addUTCOffsets(
								subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
								subtractUTCOffsets(p.beginSaved, stow));
							goto nextPeriod;
						}
						else
						{
							// There is an applicable rule set
							//	This period begins when the first rule becomes effective in
							//	the fromYear.  
							// ******* The following section was omitted upon code review.
							//  The exception described does not exist, as a period is created
							//  in fromYear - 1 whether the rule becomes effective before
							//  or after the untilCoords of the last time zone version.
							//  The only exception is when untilYearPrevVers ==
							//  fromYear - 1 and the applicable rule set does not become
							//  effective before the untilCoords.  In that case, there is
							//  no DSTR in the next period and this period begins on 1/1 of the
							//  fromYear
							//if (untilYearPrevVers == fromYear - 1)
							//{
							//	// There is an applicable rule set in fromYear - 1, but
							//	//	as this is the untilYear of the previous time zone version
							//	//  we need to determine whether the rule set becomes
							//	//  effective before or after untilCoords
							//	// Find the effective date of the first rule version in fromYear - 1
							//	rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[0];
							//	lccFirstEffective = createAtLocalCalCoords(
							//		TZRuleVersions[rvind], tzver.name, fromYear, 2,
							//		PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							//		PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
							//	if (isLessOrEqualLocalCalCoords(lccFirstEffective,
							//		PeriodTimeZones[i].periods[numPeriods - 1].untilCoords))
							//	{
							//		// Rule becomes effective before untilCoords
							//		// The period begins at lccPrevTZVerUntil + beginJump and processing
							//		//	of this time zone version is complete
							//		// Set the beginning fields of the period
							//		//
							//		// Find the effective rule at lccPrevTZVerUntil
							//		stow = sToWNoPeriodTZArray(lccPrevTZVerUntil, tzver, ruleSetIndex,
							//			PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							//			PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
							//		p = setPeriodBeginFieldsWithPrevPeriod(p, stow,
							//			PeriodTimeZones[i].periods[numPeriods - 1]);
							//		break;
							//	}
							//	else
							//	{
							//		// Rule becomes effective after untilCoords
							//		//  The period begins on 1/1 of the fromYear and
							//		//	the next period has no effective DSTR
							//		p.fromCoords = createLocalCalCoords(0, fromYear, 1, 1,
							//			0, 0, 0, 0, 0, 0, tzver.name, 'w', 'a', 2);
							//		// Find the saved offset at fromCoords
							//		//   Is either the first or last rule
							//		rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[0];
							//		lccFirstEffective = createAtLocalCalCoords(
							//			TZRuleVersions[rvind], tzver.name, fromYear, 2,
							//			p.gMTOffset, stowZero);
							//		// The first rule version only can be effective if it has an At time of 1/1 at 0:00:00
							//		lccBegYear = createLocalCalCoords(0, fromYear, 1, 1,
							//			0, 0, 0, 0, 0, 0, tzver.name, 'w', 'b', 2);
							//		if (isEqualLocalCalCoords(lccFirstEffective, lccBegYear))
							//			// The first rule version cannot be less than fromCoords
							//		{
							//			// The first rule version is effective
							//			rulever = TZRuleVersions[rvind];
							//		}
							//		else
							//		{
							//			// The last rule version is effective
							//			nRuleVer = TZRuleVersionSets[ruleSetIndex].numRuleVers;
							//			rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[nRuleVer - 1];
							//			rulever = TZRuleVersions[rvind];
							//		}
							//		p.beginSaved = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
							//			0, 0, "s-w");
							//		//  There is no effectivee rule on 12/31 of the previous year,
							//		//	  so create a zero s-w offset
							//		stow = createUTCOffset(0, 0, 0, 0, 0, 0, "s-w");
							//		//  The GMTOffsets are the same, since this is within the same
							//		//	  time zone version.
							//		p.beginJump = addUTCOffsets(
							//			subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
							//			subtractUTCOffsets(p.beginSaved, stow));
							//		goto nextPeriod;
							//	}
							//}
							//else
							//{
							//**********************************************
							// There is an applicable rule set in fromYear - 1, which
							//	is effective in that year.  This period begins when
							//	the first rule becomes effective in the fromYear.								//
							prevRuleSetIndex = findRuleSetIndex(tzver.daylightSavingsTimeRule, fromYear - 1);
							// Select the last rule in the previous rule set, as
							//	they are ordered by effective month
							prevRuleVersionIndex = TZRuleVersionSets[prevRuleSetIndex].
								ruleVersionIndices[TZRuleVersionSets[prevRuleSetIndex].numRuleVers - 1];
							rulever = TZRuleVersions[prevRuleVersionIndex];
							stowBefore = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							rvind = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[0];
							rulever = TZRuleVersions[rvind];
							//  The next computation computes the untilCoords for the next time period
							//   Need to determine the effective s-w offset at the time this rule
							//	 becomes effective.
							p.fromCoords = createAtLocalCalCoords(rulever, tzver.name, fromYear, 2,
								p.gMTOffset, stowBefore);
							// Period begins at first effective date of the rule set in fromYear
							p.begRule = 0;
							p.beginSaved = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							// Find the last rule that is effective in fromYear - 1
							nRuleVer = TZRuleVersionSets[nextRuleSetIndex].numRuleVers;
							rvind = TZRuleVersionSets[nextRuleSetIndex].ruleVersionIndices[nRuleVer - 1];
							rulever = TZRuleVersions[rvind];
							/* 4/2/2018 =====================================================
							//  If fromYear - 1 == untilYearPrevVers and the last rule becomes
							//   effective before the untilCoords of the previous time zone version
							//   it is not effective at the end of the year.
							if (untilYearPrevVers == fromYear - 1)
							{
							// Determine the effective date of the rule version, using
							//  GMTOffset and endSaved of the previous time zone version.
							lccNextRuleEffective = createAtLocalCalCoords(rulever, tzver.name,
							fromYear - 1, 2, PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset,
							PeriodTimeZones[i].periods[numPeriods - 1].endSaved);
							if (isGreaterOrEqualLocalCalCoords(lccNextRuleEffective, lccPrevTZVerUntil))
							{
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
							0, 0, "s-w");
							}
							else stow = stowZero;
							}
							else
							{
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
							0, 0, "s-w");
							}
							===================================================================================== 4/2/2018 */
							//  Following line was added 4/2/2018
							stow = createUTCOffset(0, 0, rulever.hoursSaved, rulever.minutesSaved,
								0, 0, "s-w");
							//  The GMTOffsets are the same, since this is within the same
							//	  time zone version.
							p.beginJump = addUTCOffsets(
								subtractUTCOffsets(p.gMTOffset, p.gMTOffset),
								subtractUTCOffsets(p.beginSaved, stow));
							//  Apply the beginning jump to compute fromCoords
							p.fromCoords = offsetLocalCalCoords(p.fromCoords, p.beginJump);
							goto nextPeriod;
						}
					} // End of block for fromYear > untilYearPrevVers
				} // End of block for DSTR specified and effective
				  // Store this period and move on to the next period
			nextPeriod:
				versionPeriods[numPerVers - 1] = p;
				numPerVers++;
				if (p.endRule > p.nRuleVersions) abort();
				if (p.begRule > p.nRuleVersions) abort();
				if (numPerVers > maxTimeZoneVersionPeriods) abort();
				// Compute untilCoords for the next period
				lccPeriodUntil = offsetLocalCalCoords(p.fromCoords, negateUTCOffset(p.beginJump));
			} // End of while loop for period computation for the time zone version
			maxVersPeriods = max(maxVersPeriods, numPerVers);
			// Finished processing the time zone version
			//  Store the last period in the temporary array
			versionPeriods[numPerVers - 1] = p;
			//  Version Periods is in reverse chornological order.  Store in PeriodTimeZones
			//	 in chronological order
			for (j = 0; j < numPerVers; j++)
			{
				PeriodTimeZones[i].periods[numPeriods] = versionPeriods[numPerVers - j - 1];
				numPeriods++;
				if (numPeriods > maxTimeZonePeriods) abort();
			}
			PeriodTimeZones[i].numPeriods = numPeriods;
			// Establish lccPrevTZVerUntil
			lccPrevTZVerUntil = PeriodTimeZones[i].periods[numPeriods - 1].untilCoords;
			// 
		} // End of time zone version for loop
		maxTZPeriods = max(maxTZPeriods, numPeriods);
		//
		//  Perform consistency checks
		//
		for (j = 0; j < numPeriods - 1; j++)
		{
			//  Retrieve untilCoords in wall frame of reference
			lccUntilCoordsJ = PeriodTimeZones[i].periods[j].untilCoords;
			//  Retrieve fromCoords of next period in wall frame of reference
			lccFromCoordsJP1 = PeriodTimeZones[i].periods[j + 1].fromCoords;
			//
			//  The fromCoords of period j+1 must equal the untilCoords of period j
			//    offset by beginJump of period j+1
			//
			if (!isEqualLocalCalCoords(offsetLocalCalCoords(lccUntilCoordsJ,
				PeriodTimeZones[i].periods[j + 1].beginJump), lccFromCoordsJP1)) abort();
			//
			// The untilCoords of period j must equal the fromCoords of period j+1
			//   in a universal frame of reference
			//
			//  Translate untilCoords to standard frame of reference
			lccUntilCoordsJ = offsetLocalCalCoords(lccUntilCoordsJ,
				negateUTCOffset(PeriodTimeZones[i].periods[j].endSaved));
			//  Translate untilCoords to universal frame of reference
			lccUntilCoordsJ = offsetLocalCalCoords(lccUntilCoordsJ,
				negateUTCOffset(PeriodTimeZones[i].periods[j].gMTOffset));
			//  Translate fromCoords to standard frame of reference
			lccFromCoordsJP1 = offsetLocalCalCoords(lccFromCoordsJP1,
				negateUTCOffset(PeriodTimeZones[i].periods[j + 1].beginSaved));
			//  Translate fromCoords to universal frame of reference
			lccFromCoordsJP1 = offsetLocalCalCoords(lccFromCoordsJP1,
				negateUTCOffset(PeriodTimeZones[i].periods[j + 1].gMTOffset));
			//  Compare the values
			if (!isEqualLocalCalCoords(lccFromCoordsJP1, lccUntilCoordsJ)) abort();
			//  Compute and store the tick values
			//   Set the precision and uncertainty to unspecified to avoid adjustment of the 
			//		coordinates
			PeriodTimeZones[i].periods[j].untilTAI = createUTCDatetimeFromCalCoords(lccUntilCoordsJ.cc, 99,
				0, 1).tai;
			PeriodTimeZones[i].periods[j+1].fromTAI = createUTCDatetimeFromCalCoords(lccFromCoordsJP1.cc, 99,
				0, 1).tai;
			//  Compare the values
			if (!isEqualTicks(PeriodTimeZones[i].periods[j].untilTAI, 
					PeriodTimeZones[i].periods[j+1].fromTAI)) abort();
		}
		// Calculate ticks for first period from coords and last period until coords
		//   Convert each to standard then to universal
		lccFromCoordsJP1 = offsetLocalCalCoords(PeriodTimeZones[i].periods[0].fromCoords,
			negateUTCOffset( PeriodTimeZones[i].periods[0].beginSaved));
		lccFromCoordsJP1 = offsetLocalCalCoords(lccFromCoordsJP1,
			negateUTCOffset(PeriodTimeZones[i].periods[0].gMTOffset));
		PeriodTimeZones[i].periods[0].fromTAI = createUTCDatetimeFromCalCoords(lccFromCoordsJP1.cc, 99,
			0, 1).tai;
		//  Check that fromTAI == Creation
		if (!isEqualTicks(PeriodTimeZones[i].periods[0].fromTAI, Creation)) abort();
		lccUntilCoordsJ = offsetLocalCalCoords(PeriodTimeZones[i].periods[numPeriods - 1].untilCoords,
			negateUTCOffset(PeriodTimeZones[i].periods[numPeriods - 1].endSaved));
		lccUntilCoordsJ = offsetLocalCalCoords(lccUntilCoordsJ,
			negateUTCOffset(PeriodTimeZones[i].periods[numPeriods - 1].gMTOffset));
		PeriodTimeZones[i].periods[numPeriods - 1].untilTAI = createUTCDatetimeFromCalCoords(lccUntilCoordsJ.cc, 99,
			0, 1).tai;
		//  Check that untilTAI == endOfTimePlus
		if (!isEqualTicks(PeriodTimeZones[i].periods[numPeriods - 1].untilTAI, EndOfTimePlus)) abort();
		//  Check that p.begRule is consistent with p.begSaved and p.endRule is consistent with p.endSaved
		for (j = 0; j < numPeriods; ++j)
		{
			p = PeriodTimeZones[i].periods[j];
			if (p.begRule != p.nRuleVersions)
			{
				// There is an effective rule at the beginning of the period
				rulever = TZRuleVersions[p.ruleVersions[p.begRule]];
				// Compute begSaved from p.begRule
				if (!isEqualUTCOffsets(p.beginSaved, createUTCOffset(0, 0, rulever.hoursSaved,
					rulever.minutesSaved, 0, 0, "s-w"))) abort();
			}
			else
			{
				// There is not an effective rule at the beginning of the period
				if (!isEqualUTCOffsets(p.beginSaved, p.tZSaved)) abort();
			}
			if (p.endRule != p.nRuleVersions)
			{
				// There is an effective rule at the end of the period
				rulever = TZRuleVersions[p.ruleVersions[p.endRule]];
				// Compute endSaved from p.endRule
				if (!isEqualUTCOffsets(p.endSaved, createUTCOffset(0, 0, rulever.hoursSaved,
					rulever.minutesSaved, 0, 0, "s-w"))) abort();
			}
			else
			{
				// There is not an effective rule at the end of the period
				if (!isEqualUTCOffsets(p.endSaved, p.tZSaved)) abort();
			}
		}
	} // End of time zone for loop
	return 1;
}


int outputTimeZonePeriods(FILE * tzp)
{
	// Output time zone periods to a file
	TZPeriod per;
	char csign;
	UTCOffset jump, stow;
	char stringBuffer1[130], stringBuffer2[130];

	// Write out time zone period information
	for (uint32_t i = 0; i < numTimeZones; i++)
	{
		fprintf(tzp, "\n%s  %d periods\n", PeriodTimeZones[i].names[0], PeriodTimeZones[i].numPeriods);
		for (uint8_t j = 0; j < PeriodTimeZones[i].numPeriods - 1; j++)
		{
			per = PeriodTimeZones[i].periods[j];
			fprintf(tzp, "  %s [%s] ",
				asStringLocalCalCoords(per.fromCoords, stringBuffer1),
				asStringUTCOffset(per.gMTOffset));
			// Print the rule version offsets or the time zone version offset
			if (per.nRuleVersions > 0)
			{
				//  There is at least one applicable rule version
				for (uint8_t k = 0; k < per.nRuleVersions; k++)
				{
					stow = createUTCOffset(0, 0, TZRuleVersions[per.ruleVersions[k]].hoursSaved,
						TZRuleVersions[per.ruleVersions[k]].minutesSaved, 0, 0, "s-w");
					fprintf(tzp, "(%s in %s) ", asStringUTCOffset(stow), 
						months[TZRuleVersions[per.ruleVersions[k]].transMonth]);
				}
			}
			else
			{
				// There are no applicable rule versions
				//  Output the time zone version s-w offset
				fprintf(tzp, "[%s] ", asStringUTCOffset(per.tZSaved));
			}
			fprintf(tzp, " %s\n", asStringLocalCalCoords(per.untilCoords, stringBuffer2));
			jump = PeriodTimeZones[i].periods[j + 1].beginJump;
			csign = (jump.isNegative ? ' ' : '+');
			fprintf(tzp, "                         %c%s\n", csign,
				asStringUTCOffset(jump));
		}
		per = PeriodTimeZones[i].periods[PeriodTimeZones[i].numPeriods - 1];
		fprintf(tzp, "  %s [%s] %s\n",
			asStringLocalCalCoords(per.fromCoords, stringBuffer1),
			asStringUTCOffset(per.gMTOffset),
			asStringLocalCalCoords(per.untilCoords, stringBuffer2));
	}
	return 0;
}

uint32_t findRuleIndex(char ruleName[])
{
	// Find the index of a rule by searching the TZRules array
	for (uint32_t j = 0; j < numRules; j++)
	{
		if (strncmp(ruleName, TZRules[j], maxRuleNameLength) == 0) return j;
	}
	//  If the rule is not found, return numRules
	return numRules;
}

uint32_t findRuleSetIndex(char ruleName[], uint32_t year)
{
	// Find the index of the applicable rule set for a specified year
	//
	uint32_t ruleIndex, firstRuleSetIndex, lastRuleSetIndex, i;
	//	Find the index of the rule in the TZRules array
	ruleIndex = findRuleIndex(ruleName);
	//	Check for error
	if (ruleIndex == numRules)
	{
		// Invalid rule name
		return numRuleVersionSets;
	}
	//  Find the indices of the first and last rule sets
	firstRuleSetIndex = TZRuleSetIndices[ruleIndex][0];
	lastRuleSetIndex = TZRuleSetIndices[ruleIndex][1];
	//  Loop through the rule sets until the one with the proper date range is located
	for (i = firstRuleSetIndex; i <= lastRuleSetIndex; i++)
	{
		if (year >= TZRuleVersionSets[i].fromYear && year <= TZRuleVersionSets[i].toYear)
		{
			// Found the rule set
			return i;
		}
	}
	// If we arrived here, there is no applicable rule set for the year
	return numRuleVersionSets;
}

uint32_t lastApplicableYearBeforeUntil(char ruleName[], uint32_t untilYear)
{
	// Find the last year a rule is applicable before a specified untilYear
	//	This function assumes the rule is not effective during the untilYear,
	//	  based on its intended usage in createTimeZoneRegion.  Based on 
	//	  this assumption, only the toYear of each rule set needs to be
	//	  checked.
	//  For example, assume there is a rule set applicable from 1940 to 1949, 1950 to 1960,
	//	  and one applicable from 1963 to 1968.  We could call this function
	//	  for the years from 1961 or 1962 (or 1963, if the until coords are
	//	  before the effective date of the first rule in the 1963 to 1968 rule
	//	  set.  In these cases, the function would return 1960.  If we were to
	//	  erroneously call the function in 1958, it would return 1949, even
	//	  though the implied interpretation from the name of the function
	//	  would suggest a return value of 1957.
	//
	uint32_t ruleIndex, firstRuleSetIndex, lastRuleSetIndex, i, maxYear;
	//
	//	Find the index of the rule in the TZRules array
	ruleIndex = findRuleIndex(ruleName);
	//	Check for error
	if (ruleIndex == numRules)
	{
		// Invalid rule name
		return 2*e9;
	}
	//  Find the indices of the first and last rule sets
	firstRuleSetIndex = TZRuleSetIndices[ruleIndex][0];
	lastRuleSetIndex = TZRuleSetIndices[ruleIndex][1];
	//  Loop through the rule sets until the one with the largest toYear < untilYear is located
	maxYear = 0;
	for (i = firstRuleSetIndex; i <= lastRuleSetIndex; i++)
	{
		if (TZRuleVersionSets[i].toYear >= untilYear) break;
		maxYear = TZRuleVersionSets[i].toYear;
	}
	// If maxYear == 0, the toYear of the first rule set is >= untilYear.
	//	If the fromYear of that rule set is < untilYear, the function was called
	//	in error, otherwise there is no rule applicable before the untilYear.
	// In all other cases, maxYear is the desired result
	if (maxYear == 0 && TZRuleVersionSets[firstRuleSetIndex].fromYear < untilYear) return 2 * e9;
	else return maxYear;
}

TZPeriod setPeriodFieldsNoDSTR(TZPeriod p, LocalCalCoords untilStandard)
{
	// Set time zone period fields for a period with no effective daylight times rules
	p.nRuleVersions = 0;
	p.endRule = 0;
	p.tZSaved = createUTCOffset(0, 0, 0, 0, 0, 0, "s-w");
	p.endSaved = p.tZSaved;
	// untilStandard can either be standard or wall frame of reference
	p.untilCoords = (untilStandard.frame == 1 ?	offsetLocalCalCoords(untilStandard, p.tZSaved)
		: untilStandard);

	return p;
}

TZPeriod setPeriodBeginFieldsWithPrevPeriod(TZPeriod p, UTCOffset beginSaved, TZPeriod pPrev)
{
	// Set the beginning fileds of a time zone period when there is a previous period in
	//	this time zone version
	p.beginSaved = beginSaved;
	//  p.beginJump = (GMTOffset(i) - GMTOffset(i-1)) + (BeginSaved(i) - EndSaved(i-1))
	p.beginJump = addUTCOffsets(
		subtractUTCOffsets(p.gMTOffset, pPrev.gMTOffset),
		subtractUTCOffsets(p.beginSaved, pPrev.endSaved));
	//  The period begins at pPrev.untilCoords + beginJump.
	//	  beginJump must be added, since pPrev.untilCoords is specified
	//	  in terms of the GMTOffset and DSTR of the previous version. 
	//	In mathematical terms, a time zone period is closed at beginCoords
	//	  and open at untilCoords.  I.e., is valid for beginCoords <= t < untilCoords
	//
	p.fromCoords = offsetLocalCalCoords(pPrev.untilCoords, p.beginJump);
	return p;
}

TZPeriod setPeriodBeginFieldsWithoutPrevPeriod(TZPeriod p, UTCOffset beginSaved)
{
	// Set the beginning fields of the first period of a time zone version
	LocalCalCoords creation;
	p.beginSaved = beginSaved;
	p.begRule = p.nRuleVersions;  
	// First period for this time zone, so beginJump = 0
	p.beginJump = createUTCOffset(0, 0, 0, 0, 0, 0, "wp1");
	//  The first period of a time zone version begins at the beginning
	//	of time in local time
	//  Create a LocalCalCoords at Creation with a 'u' frame of reference
	creation = createLocalCalCoords(-13, bbYear, 12, 25, 12, 0, 0, 0, 0, 0, 
		TimeZoneVersions[p.tZVersion].name, 'u');
	//	Offset Creation to standard time
	p.fromCoords = offsetLocalCalCoords(creation, p.gMTOffset);
	//	Offset Creation to wall time
	p.fromCoords = offsetLocalCalCoords(p.fromCoords,
		createUTCOffset(0, 0, 0, 0, 0, 0, "s-w"));
	return p;
}

int32_t getUntilYear(LocalCalCoords lcc, UTCOffset beginJump)
{
	// Retrieve the year from a LocalCalCoords and make appropriate adjustments
	//	to be a valid untilYear for createTimeZonePeriods
	//
	int32_t untilYear;
	//
	//  Handle the cases where the until datetime is intended to represent the end of time
	//   Keep the value below e9 to avoid failing checkDateCoordsElements
	if (lcc.cc.date.year == e9 || lcc.cc.date.gigayear != 0) return e9 - 1;
	//
	// In a number of situations, the fromCoords for a period is set to the instant a year
	//  starts.  In these cases, the untilYear must be the previous year, since the period 
	//  defined by the until datetime actually ends the attosecond before the until datetime.
	//
	// To identify these cases, apply the beginning jump to the untilCoords to determine
	//  the relevant fromCoords.
	//
	if (beginJump.hours != 0 || beginJump.minutes !=0 || beginJump.seconds !=0)
		lcc = offsetLocalCalCoords(lcc, beginJump);
	untilYear = lcc.cc.date.year;
	//
	if (lcc.cc.date.month == 1 && lcc.cc.date.dayOfMonth == 1 && lcc.cc.time.hour == 0 &&
		lcc.cc.time.minute == 0 && lcc.cc.time.second == 0 && lcc.cc.time.nanosecond == 0 &&
		lcc.cc.time.attosecond == 0) return untilYear - 1;
	else return untilYear;
}

LocalCalCoords createAtLocalCalCoords(TZRule rule, char timezone[], uint32_t year, uint8_t requiredFrame,
	UTCOffset gMTOffset, UTCOffset sToW)
{
	// Create a LocalCalCoords representing the At or effective datetime of the rule version
	//  for a specified year.
	//	This datetime is the first instant the rule becomes effective.  Alternatively,
	//	and, more accurately, the previous attosecond is the last instant of the
	//	previous period.  To understand why the latter is more accurate, consider the
	//	case where the rule is adding 1 hour of daylight savings time at 1:00 a.m.. 
	//  In this case, while the rule becomes effecitve at 1:00 a.m. wall time, the
	//	time instantly becomes 2:00 a.m..  Thus, the time goes from 12:59:59.999999999 999999999
	//	to 2:00:00.  The most accurate statement is the rule becomes effective at what
	//	would have been 1:00 a.m. by the rules of the previous period.
	//
	//  Use the proleptic Gregorian calendar, set bOrA to 'B' and futureAdjust to 1
	//
	//	  The required frame of reference is provided, as well as the gMTOffSet and
	//	  the s-w offset to translate from the rule frame of reference to the required 
	//    frame of reference, if necessary.
	//
	//  The s-w UTCOffset cannot be calculated from the rule, since the At coordinates 
	//    is in terms of the rule in effect the instant before this rule becomes effective.
	//  For example, the switch from standard to daylight savings time in the US takes place
	//    at 2:00 standard time, so the At is 2:00.  If we were to apply the offset contained
	//    in the rule, we would perform the transiton at 3:00, which would be incorrect.
	//
	LocalCalCoords lccAt;
	UTCOffset wtos, stou;
	// Create a LocalCalCoords using the frame of reference of the rule
	//  Check whether a weekday rule or specified day of month is being used
	if (rule.weekNumber == 0)	lccAt = createLocalCalCoords(0, year, rule.transMonth, rule.afterDayOfMonth, 
		rule.hourOfTransition, rule.minuteOfTransition, rule.secondOfTransition, 
		rule.hundredthOfTransition * e7, 0, 0, timezone, rule.frameOfTransition);
	else lccAt = createLocalCalCoordsFromWeekdayRule(0, year, rule.transMonth, rule.afterDayOfMonth,
		rule.weekNumber, rule.dayOfWeek, rule.hourOfTransition, rule.minuteOfTransition, rule.secondOfTransition,
		rule.hundredthOfTransition * e7, 0, 0, timezone, rule.frameOfTransition);
	//
	// Translate frame of reference, if necessary
	if (rule.frameOfTransition == requiredFrame)
	{
		// No translation necessary
		return lccAt;
	}
	else if (requiredFrame == 2)
	{
		// Required frame of reference is wall
		if (rule.frameOfTransition == 0)
		{
			// At frame of reference is universal
			//  Translate to standard
			lccAt = offsetLocalCalCoords(lccAt, gMTOffset);
		}
		// At this point the frame of reference is standard
		// Translate to wall
		return offsetLocalCalCoords(lccAt, sToW);
	}
	else if (requiredFrame == 1)
	{
		// Required frame of reference is standard
		if (rule.frameOfTransition == 2)
		{
			// At frame of reference is wall
			//	Translate to standard
			//	Create a w-s UTCOffset 
			wtos = negateUTCOffset(sToW);
			return offsetLocalCalCoords(lccAt, wtos);
		}
		else
		{
			// At frame of reference is universal
			//  Translate to standard
			return offsetLocalCalCoords(lccAt, gMTOffset);
		}
	}
	else
	{
		// Required frame of reference is universal
		if (rule.frameOfTransition == 2)
		{
			// At frame of reference is wall
			//	Translate to standard
			//	Create a w-s - the negative of the time saved
			wtos = negateUTCOffset(sToW);
			lccAt = offsetLocalCalCoords(lccAt, wtos);
		}
		// At this point the frame of reference is standard
		//	Create a s-u UTCOffset from GMTOffset - the negative of the GMTOffset
		stou = negateUTCOffset(gMTOffset);
		// Translate to universal
		return offsetLocalCalCoords(lccAt, stou);
	}
}

LocalCalCoordsDT computeTZFields(LocalCalCoordsDT lcc)
{
	// Compute the time zone related fields for a LocalCalCoordsDT
	//	Called after initial input validation has been performed
	LocalCalCoords fromCoords[maxTimeZonePeriods], untilCoords[maxTimeZonePeriods], 
		lastPeriodUntil, nextPeriodFrom;
	LocalCalCoordsDT lccInput, lccWall;
	BracketingSegments brackSeg, brackSegU;
	UTCOffset gMTOff;
	uint32_t i, pTZInd;
	int32_t j, pMin, pMax, pMid, perInd, segInd;
	//
	//  Save the input value of lcc
	//
	lccInput = lcc;
	//
	//  Make sure bOrA is consistent with frame
	//
	if (lcc.frame == 0 && lcc.bOrA != 0)
	{
		// BOrA must be 0 for universal frame of reference
		lccInput.lccInit |= MissingBOrA;
		return lccInput;
	}
	//
	//  Convert to UTC Gregorian calendar to facilitate PeriodTimeZone lookup
	//
	if (lcc.cc.date.calendar != 0)
	{
		// Perform the conversion.  The conversion does not change the frame of reference.
		lcc.cc = convertCalToUTCGregorian(lcc.cc);
	}
	// 
	//  Create arrays of untilCoords and fromCoords for the periods of the time zone.
	//   Convert the untilCoords and fromCoords to the frame of reference of the LocalCalCoords
	//   to facilitate comparison.
	//
	// No need to check for invalid timezone as we assume this was done prior to calling this function
	pTZInd = lcc.timezoneIndex;
	for (i = 0; i < PeriodTimeZones[pTZInd].numPeriods; ++i)
	{
		fromCoords[i] = PeriodTimeZones[pTZInd].periods[i].fromCoords;
		untilCoords[i] = PeriodTimeZones[pTZInd].periods[i].untilCoords;
		// Translate to standard frame of reference, if necessary
		if (lcc.frame != 2)
		{
			fromCoords[i] = offsetLocalCalCoords(fromCoords[i],
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].beginSaved));
			untilCoords[i] = offsetLocalCalCoords(untilCoords[i],
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].endSaved));
		}
		// Translate to universal frame of reference, if necessary
		if (lcc.frame == 0)
		{
			fromCoords[i] = offsetLocalCalCoords(fromCoords[i],
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].gMTOffset));
			untilCoords[i] = offsetLocalCalCoords(untilCoords[i],
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].gMTOffset));
		}
	}
	//  Locate the period into which this LocalCalCoords falls
	//     Need to consider 2 cases, based on bOrA value
	//
	//	The values of ambigS, ambigW and beforeFlag can be set in several places.
	//	 Initialize to an unspecified value to ensure they are set
	//	 once and only once.
	//
	lccInput.ambigS = 3;
	lccInput.ambigW = 3;
	lccInput.beforeFlag = 3;
	//
	if (lcc.bOrA == 2) // Can only get here if lcc.frame != 0
	{
		//  Specified as an after jump value, so compare to fromCoords
		//   Find the largest fromCoords where lcc >= fromCoords
		//   The below works when numPeriods == 1
		// Perform a half interval search
		pMin = 0;
		pMax = PeriodTimeZones[pTZInd].numPeriods - 1;
		while (pMin <= pMax)
		{
			pMid = (pMax + pMin) / 2;
			if (isGreaterOrEqualLDTLCC(lcc, fromCoords[pMid])) pMin = max(pMid, pMin + 1);
			else pMax = min(pMid, pMax - 1);
		}
		// The search is complete
		if (pMid > pMax) perInd = pMid - 1;
		else perInd = pMid;
		//	Check for the edge condition where lcc < fromCoords for the first period.
		//	  As this is always the beginning of time in the universal frame of reference, this can
		//		occur when the lcc is on a basis other than universal and is early in the day
		//      at Creation.  LocalCalCoords are allowed on that day, as we cannot
		//      know the associated datetime is invalid until we have analyzed
		//	    the LocalCalCoords given the timezone.
		//
		//	 When this edge condition occurs, perInd = -1
		//
		if (perInd == -1)
		{
			lccInput.lccInit |= NegativeTime;
			return lccInput;
		}
		//  Determine whether the datetime is invalid - i.e., falls in a jump between periods -
		//	 for standard and wall frames of reference
		//		Check whether lcc >= untilCoords
		if (isGreaterOrEqualLDTLCC(lcc, untilCoords[perInd]))
		{
			//  This is an invalid datetime
			lccInput.lccInit |= InvalidDatetime;
			return lccInput;
		}
		//  Determine whether the datetime falls within an ambiguous region at the
		//	 start of the current period.  Cannot be at the end of the current
		//	 period, because the algorithm compares fromCoords, and would have placed
		//	 the lcc in the next period had it been in the overlapping era.
		//  For standard frame of reference, the ambiguity can only be at the start
		//	 or end of the current period.  For wall frame of reference, the ambiguity
		//	 can be at the start or end of a period, or the start or end of a segment
		//   within a year.
		//
		//	 Check the beginning of the period
		if (perInd > 0)
		{
			//  There is a previous period
			//   Th untilCoords array already has been translated to the appropriate frame of reference
			lastPeriodUntil = untilCoords[perInd - 1];
			//  If lccInput is before lastPeriodUntil, it falls within an ambiguous
			//	 period.  Set bOrA to A.
			//
			// The following function supports comparisons with different calendars
			if (isLessLDTLCC(lccInput, lastPeriodUntil))
			{
				// Falls within an ambiguous period.  This time is after the jump.
				lccInput.bOrA = 2;
				lccInput.beforeFlag = 0;
				// Set the ambiguity flag for the relevant frame of reference
				if (lcc.frame == 1)lccInput.ambigS = 1;
				else lccInput.ambigW = 1;
			}
		}
		//	If has not already been flagged as ambiguous, does not fall within an ambiguous period
		//  If this is a standard time, we can set bOrA and ambigS, but we can't set bOrA or ambigW for a wall time,
		//	 as it may be ambiguous near a segment boundary
		if (lcc.frame == 1 && lccInput.ambigS == 3)
		{
			lccInput.ambigS = 0;
			// Reset bOrA.  If bOrA was specified as 1 or 2, no need to issue an error.
			lccInput.bOrA = 0;
			// Can't set beforeFlag, as may be ambiguous in wall frame of reference
		}
	}
	else
	{
		// Specified as before jump value or not specified
		// Find the smallest untilCoords where lcc < untilCoords
		// Perform a half interval search
		pMin = 0;
		pMax = PeriodTimeZones[pTZInd].numPeriods - 1;
		while (pMin <= pMax)
		{
			pMid = (pMax + pMin) / 2;
			if (isLessLDTLCC(lcc, untilCoords[pMid])) pMax = min(pMid, pMax - 1);
			else pMin = max(pMid, pMin + 1);
		}
		// The search is complete
		if (pMid < pMin) perInd = pMid + 1;
		else perInd = pMid;
		//	Check for the edge condition where lcc > untilCoords for the last period
		//	  As this is always max time in the universal frame of reference, this can
		//		occur when the lcc is on a basis other than universal and is the day
		//      after max time.  LocalCalCoords are allowed on that day, as we cannot
		//      know the associated datetime is invalid until we have analyzed
		//	    the LocalCalCoords given the timezone.
		//
		//	 When this edge condition occurs, perInd = PeriodTimeZones[pTZInd].numPeriods
		//
		if (perInd == PeriodTimeZones[pTZInd].numPeriods)
		{
			lccInput.lccInit |= TimeGtMax;
			return lccInput;
		}
		else
		{
			//  Check for invalid or ambiguous times if not universal frame of reference
			if (lcc.frame != 0)
			{
				//  Determine whether the datetime is invalid - i.e., falls in a jump between periods
				//		Check whether lcc < fromCoords
				if (isLessLDTLCC(lcc, fromCoords[perInd]))
				{
					//  This is an invalid datetime
					lccInput.lccInit |= InvalidDatetime;
					return lccInput;
				}
				//  Determine whether the datetime falls within an ambiguous region at the
				//	 end of the current period.  Cannot be at the start of the current
				//	 period, because the algorithm compares untilCoords, and would have placed
				//	 the lcc in the previous period had it been in the overlapping era.
				//  For standard frame of reference, the ambiguity can only be at the start
				//	 or end of the current period.  For wall frame of reference, the ambiguity
				//	 can be at the start or end of a period, or the start or end of a segment
				//   within a year.
				//
				//   Check the end of the period
				if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
				{
					//  There is a next period
					//	 The fromCoords array already has been translated to the appropriate frame of reference
					nextPeriodFrom = fromCoords[perInd + 1];
					//  If lccInput is >= nextPeriodFrom, it falls within an ambiguous
					//	 period.  Set bOrA to B.
					if (isGreaterOrEqualLDTLCC(lccInput, nextPeriodFrom))
					{
						// Falls within an ambiguous period.  This time is before the jump.
						if (lccInput.bOrA == 1)
						{
							// BOrA Specification was correct
							// Set the ambiguity flag for the relevant frame of reference
							if (lcc.frame == 1)lccInput.ambigS = 1;
							else lccInput.ambigW = 1;
							lccInput.beforeFlag = 1;
						}
						else
						{
							// BOrA Specification was incorrect
							lccInput.lccInit |= MissingBOrA;
							return lccInput;
						}
					}
				}
			}
			//  If this is a standard time, and the time has not been found to be ambiguous
			//	 we can set bOrA and ambigS.  We cannot set either for a wall time,
			//	 as it may be ambiguous near a segment boundary
			if (lcc.frame == 1 && lccInput.ambigS == 3)
			{
				lccInput.ambigS = 0;
				// Reset bOrA.  If bOrA was specified as 1 or 2, no need to issue an error.
				lccInput.bOrA = 0;
				// Cannot set beforeFlag, as may be ambiguous in wall frame of reference
			}
		}
	}
	//
	//  At this point, we have established the following:
	//	  lcc.periodIndex
	//	  bOrA for universal frame of reference
	//	  bOrA and ambigS for standard frame of reference
	//	  bOrA and ambigW for wall frame of reference, if ambiguous at a period boundary
	//	  begforeFlag, if ambiguous in the input frame of reference at a period boundary
	//
	//  To compute the remaining parameters, need to analyze the bracketing segments
	//	  for lcc.year
	//
	//  Create the bracketing period segments for the year of lcc
	//
	brackSeg = createBracketingSegments(PeriodTimeZones[pTZInd].periods[perInd], lcc.cc.date.year);
	brackSegU = brackSeg;
	//  Translate the segments into the appropriate frame of reference
	gMTOff = PeriodTimeZones[pTZInd].periods[perInd].gMTOffset;
	for (i = 0; i < brackSegU.numSegments; ++i)
	{
		if (lcc.frame != 2)
		{
			brackSegU.segments[i].fromCoords = offsetLocalCalCoords(brackSegU.segments[i].fromCoords,
				negateUTCOffset(brackSegU.segments[i].sToW));
			brackSegU.segments[i].untilCoords = offsetLocalCalCoords(brackSegU.segments[i].untilCoords,
				negateUTCOffset(brackSegU.segments[i].sToW));
		}
		if (lcc.frame == 0)
		{
			brackSegU.segments[i].fromCoords = offsetLocalCalCoords(brackSegU.segments[i].fromCoords,
				negateUTCOffset(gMTOff));
			brackSegU.segments[i].untilCoords = offsetLocalCalCoords(brackSegU.segments[i].untilCoords,
				negateUTCOffset(gMTOff));
		}
	}
	//  Locate the segment into which this LocalCalCoords falls
	//     Need to consider 2 cases, based on bOrA value
	//
	if (lcc.bOrA == 2)
	{
		//  Specified as an after jump value, so compare to fromCoords
		//   Find the largest fromCoords where lcc >= fromCoords
		for (j = 0; j < brackSegU.numSegments; ++j)
		{
			if (isLessLDTLCC(lcc, brackSegU.segments[j].fromCoords))
			{
				// The previous segment contains lcc, unless j == 0
				if (j > 0)
				{
					//  The segment that contains lcc is j-1
					//   Make sure lcc does not fall into an invalid region
					if (isGreaterOrEqualLDTLCC(lcc, brackSegU.segments[j - 1].untilCoords))
					{
						// Invalid time range
						lccInput.lccInit |= InvalidDatetime;
						return lccInput;
					}
					break;
				}
				else
				{
					// Should not be possible to get here, as
					//	the first segment begins on 12/31/n-1
					lccInput.lccInit |= InvalidDatetime;
					return lccInput;
				}
			}
		}
		segInd = j - 1;
	}
	else
	{
		// lcc.bOrA == 1 or 0
		// Specified as before jump value or not specified
		// Find the smallest untilCoords where lcc < untilCoords
		for (j = 0; j < brackSegU.numSegments; ++j)
		{
			if (isLessLDTLCC(lcc, brackSegU.segments[j].untilCoords))
			{
				// The current segment contains lcc
				//  Make sure lcc does not fall within an invalid region
				if (isLessLDTLCC(lcc, brackSegU.segments[j].fromCoords))
				{
					// Invalid time range
					lccInput.lccInit |= InvalidDatetime;
					return lccInput;
				}
				// If bOrA is unspecified, need to check for ambiguous region
				if (lcc.frame != 0 && lcc.bOrA == 0)
				{
					// Check for an ambiguous region.  While an overlap can occur at either end
					//	of the segment, only need to check the end of the segment, as if there were an
					//	overlap in the beginning of the region, the algorithm would have identified
					//  the earlier segment as the target segment.
					//
					if (j < brackSegU.numSegments - 1)
					{
						// There is a next segment
						if (isGreaterOrEqualLDTLCC(lcc, brackSegU.segments[j + 1].fromCoords))
						{
							// In an ambiguous region.
							lccInput.lccInit |= MissingBOrA;
							return lccInput;
						}
					}
				}
				break;
			}
		}
		//  If j == brackSegU.numSegments, we are in an invalid region at the end of the year
		if (j == brackSegU.numSegments)
		{
			// Should not get here, as the last segment extends to the end of 1/1/n+1
			lccInput.lccInit |= InvalidDatetime;
			return lccInput;
		}
		segInd = j;
	}
	// The bracketing segment has been located
	lccInput.sToWMinutes = brackSegU.segments[segInd].sToW.hours * 60 + brackSegU.segments[segInd].sToW.minutes;
	lccInput.timezoneIndex = pTZInd;
	lccInput.periodIndex = perInd;
	// Establish ambigW and ambigS for wall frame of reference.
	if (lcc.frame == 2)
	{
		// Begin with ambigW
		// If bOrA is 0, have already determined there is no ambiguity, because
		//	if there were, would have returned with an error before this.
		if (lcc.bOrA == 0) lccInput.ambigW = 0;
		else
		{
			//  Check for overlap at the beginning and end of the segment
			//
			//   Requires special check for beginning of first segment and end of last segment
			//	  since the overlaps creating the ambiguous regions is with other periods.
			//
			if (perInd > 0)
			{
				// There is a previous period
				if (isLessLDTLCC(lccInput, PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords))
				{
					// The lcc is in an ambiguous region created by an overlap with the previous period
					//	Set bOrA to A
					lccInput.ambigW = 1;
					lccInput.beforeFlag = 0;
				}
			}
			if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
			{
				// There is a next period
				if (isGreaterOrEqualLDTLCC(lccInput,
					PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords))
				{
					// The lcc is in an ambiguous region created by an overlap with the next period
					//	Set bOrA to B
					lccInput.ambigW = 1;
					lccInput.beforeFlag = 1;
				}
			}
			//  Check for an overlap at the beginning of the segment.
			//	  This check needs to be performed in wall frame of reference.
			if (segInd > 0)
			{
				// There is a previous segment
				if (isLessLDTLCC(lccInput, brackSegU.segments[segInd - 1].untilCoords))
				{
					// The lcc is in an ambiguous region created by an overlap with the previous segment
					lccInput.ambigW = 1;
					lccInput.beforeFlag = 0;
				}
			}
			// Check for an overlap at the end of the segment
			if (segInd < brackSegU.numSegments - 1)
			{
				// There is a next segment
				if (isGreaterOrEqualLDTLCC(lccInput, brackSegU.segments[segInd + 1].fromCoords))
				{
					// The lcc is in an ambiguous region created by an overlap with the next segment
					lccInput.ambigW = 1;
					lccInput.beforeFlag = 1;
				}
			}
			// If ambigW has not been set, the lcc is not in an ambiguous region
			if (lccInput.ambigW == 3)
			{
				lccInput.ambigW = 0;
				// Reset bOrA
				lccInput.bOrA = 0;
			}
		}
		// Now determine ambigS
		//  Standard time ambiguities only can occur at the beginning and end of the period
		if (perInd > 0)
		{
			// There is a previous period
			//  Perform the comparison in the standard frame of reference
			if (isLessLDTLCC(
				offsetLocalCalCoordsDT(lccInput, negateUTCOffset(brackSegU.segments[segInd].sToW)),
				offsetLocalCalCoords(PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords,
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd - 1].endSaved))))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous period
				lccInput.ambigS = 1;
				lccInput.beforeFlag = 0;
			}
		}
		if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
		{
			// There is a next period
			//  Perform the comparison in the standard frame of reference
			if (isGreaterOrEqualLDTLCC(
				offsetLocalCalCoordsDT(lccInput, negateUTCOffset(brackSegU.segments[segInd].sToW)),
				offsetLocalCalCoords(PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords,
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd + 1].beginSaved))))
			{
				// The lcc is in an ambiguous region created by an overlap with the next period
				lccInput.ambigS = 1;
				lccInput.beforeFlag = 1;
			}
		}
		// If ambigS has not been set, the lcc is not in an ambiguous region when
		//	translated to standard frame of reference
		if (lccInput.ambigS == 3) lccInput.ambigS = 0;
		// If either ambigS or ambigW has been set, beforeFlag already has been set to the
		//	proper value.  If beforeFlag has not been set, the value is irrelevant, so set to 0
		if (lccInput.beforeFlag == 3) lccInput.beforeFlag = 0;
	}
	// Establish ambigW for standard frame of reference
	if (lcc.frame == 1)
	{
		//  Check for overlap at the beginning and end of the segment
		//
		//   Requires special check for beginning of first segment and end of last segment
		//	  since the overlaps creating the ambiguous regions is with other periods.
		//
		if (perInd > 0)
		{
			// There is a previous period
			//  Perform the comparison in the wall frame of reference
			if (isLessLDTLCC(
				offsetLocalCalCoordsDT(lccInput, brackSegU.segments[segInd].sToW),
				PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous period
				lccInput.ambigW = 1;
				lccInput.beforeFlag = 0;
			}
		}
		if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
		{
			// There is a next period
			if (isGreaterOrEqualLDTLCC(
				offsetLocalCalCoordsDT(lccInput, brackSegU.segments[segInd].sToW), 
				PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the next period
				lccInput.ambigW = 1;
				lccInput.beforeFlag = 1;
			}
		}
		//  Check for an overlap at the beginning of the segment.
		//	  This check needs to be performed in wall frame of reference.
		if (segInd > 0)
		{
			// There is a previous segment
			if (isLessLDTLCC(
				offsetLocalCalCoordsDT(lccInput, brackSegU.segments[segInd].sToW),
				offsetLocalCalCoords(brackSegU.segments[segInd - 1].untilCoords,
				brackSegU.segments[segInd - 1].sToW)))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous segment
				//				lccInput.bOrA = 2;
				lccInput.ambigW = 1;
				lccInput.beforeFlag = 0;
			}
		}
		// Check for an overlap at the end of the segment
		if (segInd < brackSegU.numSegments - 1)
		{
			// There is a next segment
			if (isGreaterOrEqualLDTLCC(
				offsetLocalCalCoordsDT(lccInput, brackSegU.segments[segInd].sToW),
				offsetLocalCalCoords(brackSegU.segments[segInd + 1].fromCoords,
					brackSegU.segments[segInd + 1].sToW)))
			{
				// The lcc is in an ambiguous region created by an overlap with the next segment
				lccInput.ambigW = 1;
				lccInput.beforeFlag = 1;
			}
		}
		// If ambigW has not been set, the lcc is not in an ambiguous region in wall frame of reference
		if (lccInput.ambigW == 3)	lccInput.ambigW = 0;
		// If either ambigS or ambigW has been set, beforeFlag already has been set to the
		//	proper value.  If beforeFlag has not been set, the value is irrelevant, so set to 0
		if (lccInput.beforeFlag == 3) lccInput.beforeFlag = 0;
	}
	// Establish ambigW, ambigS and beforeFlag for universal frame of reference
	if (lcc.frame == 0)
	{
		// Begin with ambigS
		//  Standard time ambiguities only can occur at the beginning and end of the period
		if (perInd > 0)
		{
			// There is a previous period
			//  Perform the comparison in the standard frame of reference
			if (isLessLDTLCC(
				offsetLocalCalCoordsDT(lccInput, PeriodTimeZones[pTZInd].periods[perInd ].gMTOffset),
				offsetLocalCalCoords(PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords,
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd - 1].endSaved))))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous period
				lccInput.ambigS = 1;
				//  When translated to standard time, bOrA is After
				lccInput.beforeFlag = 0;
			}
		}
		if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
		{
			// There is a next period
			//  Perform the comparison in the standard frame of reference
			if (isGreaterOrEqualLDTLCC(
				offsetLocalCalCoordsDT(lccInput, PeriodTimeZones[pTZInd].periods[perInd].gMTOffset),
				offsetLocalCalCoords(PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords,
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd + 1].beginSaved))))
			{
				// The lcc is in an ambiguous region created by an overlap with the next period
				lccInput.ambigS = 1;
				//  When translated to standard time, bOrA is Before
				lccInput.beforeFlag = 1;
			}
		}
		// If ambigS has not been set, the lcc is not in an ambiguous region when
		//	translated to standard frame of reference
		if (lccInput.ambigS == 3)
		{
			lccInput.ambigS = 0;
		}
		// Now determine ambigW
		lccWall = offsetLocalCalCoordsDT(lccInput, PeriodTimeZones[pTZInd].periods[perInd].gMTOffset);
		lccWall = offsetLocalCalCoordsDT(lccWall, brackSegU.segments[segInd].sToW);
		if (perInd > 0)
		{
			// There is a previous period
			//  Perform the comparison in the wall frame of reference
			if (isLessLDTLCC(
				lccWall,
				PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous period
				lccInput.ambigW = 1;
				//  When translated to wall time, bOrA is After
				lccInput.beforeFlag = 0;
			}
		}
		if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
		{
			// There is a next period
			if (isGreaterOrEqualLDTLCC(
				lccWall,
				PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the next period
				lccInput.ambigW = 1;
				//  When translated to wall time, bOrA is Before
				lccInput.beforeFlag = 1;
			}
		}
		//  Check for an overlap at the beginning of the segment.
		//	  This check needs to be performed in wall frame of reference.
		if (segInd > 0)
		{
			// There is a previous segment
			if (isLessLDTLCC(
				lccWall,
				offsetLocalCalCoords(
					offsetLocalCalCoords(brackSegU.segments[segInd - 1].untilCoords,
						PeriodTimeZones[pTZInd].periods[perInd].gMTOffset),
					brackSegU.segments[segInd - 1].sToW)))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous segment
				//				lccInput.bOrA = 2;
				lccInput.ambigW = 1;
				//  When translated to wall time, bOrA is After
				lccInput.beforeFlag = 0;
			}
		}
		// Check for an overlap at the end of the segment
		if (segInd < brackSegU.numSegments - 1)
		{
			// There is a next segment
			if (isGreaterOrEqualLDTLCC(
				lccWall,
				offsetLocalCalCoords(
					offsetLocalCalCoords(brackSegU.segments[segInd + 1].fromCoords,
						PeriodTimeZones[pTZInd].periods[perInd].gMTOffset),
					brackSegU.segments[segInd + 1].sToW)))
			{
				// The lcc is in an ambiguous region created by an overlap with the next segment
				lccInput.ambigW = 1;
				//  When translated to wall time, bOrA is Before
				lccInput.beforeFlag = 1;
			}
		}
		// If ambigW has not been set, the lcc is not in an ambiguous region in wall frame of reference
		if (lccInput.ambigW == 3) lccInput.ambigW = 0;
		// If either ambigS or ambigW has been set, beforeFlag already has been set to the
		//	proper value.  If beforeFlag has not been set, the value is irrelevant, so set to 0
		if (lccInput.beforeFlag == 3) lccInput.beforeFlag = 0;
	}
	return lccInput;
}

LocalCalCoordsDT translateToUniversal(LocalCalCoordsDT lcc)
{
	// Translate a LocalCalCoords in a wall or standard frame of reference to a univeral frame of reference
	//	Calendar is maintained in translation - i.e., not converted to proleptic UTC Gregorian
	//   Return the index of the period in the PeroidTimeZones array through the
	//	  argument list to efficiently provide the value to createLocalDatetimeFromLocalCalCoords
	LocalCalCoords fromCoords[maxTimeZonePeriods], untilCoords[maxTimeZonePeriods];
	LocalCalCoordsDT lccInput;
	BracketingSegments brackSeg;
	uint32_t i, pTZInd;
	int32_t j, pMin, pMax, pMid, perInd, segInd;
	//
	//  Save the input value of lcc
	//
	lccInput = lcc;
	//
	//  Convert to UTC Gregorian calendar to facilitate PeriodTimeZone lookup
	//
	if (lcc.cc.date.calendar != 0)
	{
		// Perform the conversion.  The conversion does not change the frame of reference.
		lcc.cc = convertCalToUTCGregorian(lcc.cc);
	}
	//
	//  If the frame of reference is not universal, translate to universal
	//
//	*periodIndex = 0;
	if (lcc.frame != 0)
	{
		// 
		//  Create arrays of untilCoords and fromCoords for the periods of the time zone.
		//   Convert the untilCoords and fromCoords to the frame of reference of the LocalCalCoords
		//   to facilitate comparison.
		//
		//  Find the index of the timezone in the PTimeZones array
		//
		for (i = 0; i < numTimeZones; ++i)
		{
			for (j = 0; j < PeriodTimeZones[i].numNames; ++j)
			{
				if (strncmp(lcc.timezone, PeriodTimeZones[i].names[j], maxTZNameLength) == 0)
				{
					// Found a match
					pTZInd = i;
					break;
				}
			}
			if (j < PeriodTimeZones[i].numNames) break;
		}
//		*timezoneIndex = i;
		// No need to check for invalid timezone as we assume this was done when LocalCalCoords was created
		for (i = 0; i < PeriodTimeZones[pTZInd].numPeriods; ++i)
		{
			fromCoords[i] = PeriodTimeZones[pTZInd].periods[i].fromCoords;
			untilCoords[i] = PeriodTimeZones[pTZInd].periods[i].untilCoords;
			// Translate to standard frame of reference, if necessary
			if (lcc.frame == 1)
			{
				fromCoords[i] = offsetLocalCalCoords(fromCoords[i],
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].beginSaved));
				untilCoords[i] = offsetLocalCalCoords(untilCoords[i],
					negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].endSaved));
			}
		}
		//  Locate the period into which this LocalCalCoords falls
		//     Need to consider 2 cases, based on bOrA value
		//
		if (lcc.bOrA == 2)
		{
			//  Specified as an after jump value, so compare to fromCoords
			//   Find the largest fromCoords where lcc >= fromCoords
			//   The below works when numPeriods == 1
			// Perform a half interval search
			pMin = 0;
			pMax = PeriodTimeZones[pTZInd].numPeriods - 1;
			while (pMin <= pMax)
			{
				pMid = (pMax + pMin) / 2;
				if (isGreaterOrEqualLDTLCC(lcc, fromCoords[pMid])) pMin = max(pMid, pMin + 1);
				else pMax = min(pMid, pMax - 1);
			}
			// The search is complete
			if (pMid > pMax) perInd = pMid - 1;
			else perInd = pMid;
			//  Determine whether the datetime is invalid - i.e., falls in a jump between periods
			//		Check whether lcc >= untilCoords
			if (isGreaterOrEqualLDTLCC(lcc, untilCoords[perInd]))
			{
				//  This is an invalid datetime
				lccInput.lccInit |= InvalidDatetime;
				return lccInput;
			}
		}
		else
		{
			// Specified as before jump value or not specified
			// Find the smallest untilCoords where lcc < untilCoords
			// Perform a half interval search
			pMin = 0;
			pMax = PeriodTimeZones[pTZInd].numPeriods - 1;
			while (pMin <= pMax)
			{
				pMid = (pMax + pMin) / 2;
				if (isLessLDTLCC(lcc, untilCoords[pMid])) pMax = min(pMid, pMax - 1);
				else pMin = max(pMid, pMin + 1);
			}
			// The search is complete
			if (pMid < pMin) perInd = pMid + 1;
			else perInd = pMid;
			//	Check for the edge condition where lcc > untilCoords for the last period
			//	  As this is always max time in the universal frame of reference, this can
			//		occur when the lcc is on a basis other than universal and is the day
			//      after max time.  LocalCalCoords are allowed on that day, as we cannot
			//      know the associated datetime is invalid until we have created the
			//	    PeriodTimeZones array.
			//
			//	 When this edge condition occurs, perInd = PeriodTimeZones[pTZInd].numPeriods
			//
			if (perInd == PeriodTimeZones[pTZInd].numPeriods)
			{
				lccInput.lccInit |= TimeGtMax;
				return lccInput;
			}
			else
			{
				//  Determine whether the datetime is invalid - i.e., falls in a jump between periods
				//		Check whether lcc < fromCoords
				if (isLessLDTLCC(lcc, fromCoords[perInd]))
				{
					//  This is an invalid datetime
					lccInput.lccInit |= InvalidDatetime;
					return lccInput;
				}
				//  If bOrA is unspecified, check whether lcc falls within an ambiguous period
				if (lcc.bOrA == 0)
				{
					/**** The below should not be necessary
					//  Need to check for overlaps in the previous period and the next period
					if (perInd > 0)
					{
					// There is a previous period
					//	Check whether lcc < the previous untilCoords
					lcc.bOrA = 1;
					if (isLessLocalCalCoords(lcc, untilCoords[perInd - 1]))
					{
					//  There is an overlap.
					//	 This is an invalid datetime due to ambiguity
					lccInput.lccInit |= MissingBOrA;
					return lccInput;
					}
					}******/
					if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
					{
						// There is a next period
						//  Check whether lcc is >= the next fromCoords
						if (isGreaterOrEqualLDTLCC(lcc, fromCoords[perInd + 1]))
						{
							//  There is an overlap.
							//	 This is an invalid datetime due to ambiguity
							lccInput.lccInit |= MissingBOrA;
							return lccInput;
						}
					}
				}
			}
		}
		//
//		*periodIndex = perInd;
		//
		//  If the frame is standard, apply the gmtOffset and return
		//
		if (lcc.frame == 1) return offsetLocalCalCoordsDT(lccInput, 
			negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd].gMTOffset));
		//
		//	This is a wall time.
		//  Create the bracketing period segments for the year of lcc
		//
		brackSeg = createBracketingSegments(PeriodTimeZones[pTZInd].periods[perInd], lcc.cc.date.year);
		//  Locate the segment into which this LocalCalCoords falls
		//     Need to consider 2 cases, based on bOrA value
		//
		if (lcc.bOrA == 2)
		{
			//  Specified as an after jump value, so compare to fromCoords
			//   Find the largest fromCoords where lcc >= fromCoords
			for (j = 0; j < brackSeg.numSegments; ++j)
			{
				if (isLessLDTLCC(lcc, brackSeg.segments[j].fromCoords))
				{
					// The previous segment contains lcc, unless j == 0
					if (j > 0)
					{
						//  The segment that contains lcc is j-1
						//   Make sure lcc does not fall into an invalid region
						if (isGreaterOrEqualLDTLCC(lcc, brackSeg.segments[j - 1].untilCoords))
						{
							// Invalid time range
							lccInput.lccInit |= InvalidDatetime;
							return lccInput;
						}
						break;
					}
					else
					{
						// Should not be possible to get here, as
						//	the first segment begins on 12/31/n-1
						lccInput.lccInit |= InvalidDatetime;
						return lccInput;
					}
				}
			}
			segInd = j - 1;
		}
		else
		{
			// lcc.bOrA == 1 or 0
			// Specified as before jump value or not specified
			// Find the smallest untilCoords where lcc < untilCoords
			for (j = 0; j < brackSeg.numSegments; ++j)
			{
				if (isLessLDTLCC(lcc, brackSeg.segments[j].untilCoords))
				{
					// The current segment contains lcc
					//  Make sure lcc does not fall within an invalid region
					if (isLessLDTLCC(lcc, brackSeg.segments[j].fromCoords))
					{
						// Invalid time range
						lccInput.lccInit |= InvalidDatetime;
						return lccInput;
					}
					// If bOrA is unspecified, need to check for ambiguous region
					if (lcc.bOrA == 0)
					{
						// Check for an ambiguous region.  While an overlap can occur at either end
						//	of the segment, only need to check the end of the segment, as if there were an
						//	overlap in the beginning of the region, the algorithm would have identified
						//  the earlier segment as the target segment.
						//
						if (j < brackSeg.numSegments - 1)
						{
							// There is a next segment
							if (isGreaterOrEqualLDTLCC(lcc, brackSeg.segments[j + 1].fromCoords))
							{
								// In an ambiguous region.
								lccInput.lccInit |= MissingBOrA;
								return lccInput;
							}
						}
					}
					break;
				}
			}
			//  If j == brackSeg.numSegments, we are in an invalid region at the end of the year
			if (j == brackSeg.numSegments)
			{
				// Should not get here, as the last segment extends to the end of 1/1/n+1
				lccInput.lccInit |= InvalidDatetime;
				return lccInput;
			}
			segInd = j;
		}
		// Now that the bracketing segment has been located, translate to universal frame of reference
		lccInput = offsetLocalCalCoordsDT(lccInput,
			negateUTCOffset(brackSeg.segments[segInd].sToW));
		lccInput = offsetLocalCalCoordsDT(lccInput,
			negateUTCOffset(PeriodTimeZones[pTZInd].periods[perInd].gMTOffset));
	}
	// timezone must be blank for universal frame of reference
	lccInput.timezone[0] = '\0';
	// bOrA must be unspecified for universal frame of reference
	lccInput.bOrA = 0;
	return lccInput;
}

LocalCalCoordsDT translateFrame(LocalCalCoordsDT lcc, uint8_t frame)
{
	// Translate a LocalCalCoordsDT between frames of reference within the same time zone
	//
	UTCOffset off;
	//  Check for a no-op
	if (frame == lcc.frame) return lcc;
	else
	{
		// A translation is necessary
		//
		// Check the validity of the required frame
		//
		if (frame > 2)
		{
			// Invalid frame
			lcc.lccInit |= InvalidTimeFrame;
			return lcc;
		}
		//  Branch based on the output frame
		if (frame == 0)
		{
			// Output is universal
			// Input is either wall or standard
			if (lcc.frame == 2)
			{
				off = negateUTCOffset(createUTCOffset
				(0, 0, lcc.sToWMinutes / 60, lcc.sToWMinutes % 60, 0, 0, "s-w"));
				lcc = offsetLocalCalCoordsDT(lcc, off);
			}
			//	 Translate to universal frame of reference
			off = negateUTCOffset(PeriodTimeZones[lcc.timezoneIndex].periods[lcc.periodIndex].gMTOffset);
			lcc = offsetLocalCalCoordsDT(lcc, off);
			return lcc;
		}
		if (frame == 1)
		{
			// Output is standard
			// Input is either universal or standard
			if (lcc.frame == 0)
			{
				// Input frame is universal
				off = PeriodTimeZones[lcc.timezoneIndex].periods[lcc.periodIndex].gMTOffset;
				lcc = offsetLocalCalCoordsDT(lcc, off);
				return lcc;
			}
			else
			{
				// Input frame is wall
				off = negateUTCOffset(createUTCOffset
				(0, 0, lcc.sToWMinutes / 60, lcc.sToWMinutes % 60, 0, 0, "s-w"));
				lcc = offsetLocalCalCoordsDT(lcc, off);
				return lcc;
			}
		}
		if (frame == 2)
		{
			// Output is wall
			// Input is either universal or standard
			if (lcc.frame == 0)
			{
				// Input frame is universal
				off = PeriodTimeZones[lcc.timezoneIndex].periods[lcc.periodIndex].gMTOffset;
				lcc = offsetLocalCalCoordsDT(lcc, off);
			}
			// Translate to wall frame of reference
			off = createUTCOffset(0, 0, lcc.sToWMinutes / 60, lcc.sToWMinutes % 60, 0, 0, "s-w");
			lcc = offsetLocalCalCoordsDT(lcc, off);
			return lcc;
		}
		// Should never get here
		return lcc;
	}
}

LocalCalCoordsDT translateFromUniversal(LocalCalCoordsDT lcc, const char timezone[], uint8_t frame)
{
	// Translate a LocalCalCoordsDT in a universal frame of reference to a LocalCalCoordsDT
	//	in a specified (generally different) time zone and frame of reference
	//	Assign the appropriate value for bOrA.
	//	Calendar is maintained in translation - i.e., not converted to UTC Gregorian
//	LocalCalCoords fromCoords[maxTimeZonePeriods], lastPeriodUntil, nextPeriodFrom;
	LocalCalCoordsDT lccInput;
//	BracketingSegments brackSeg, brackSegU;
//	UTCOffset gMTOff;
	uint32_t i, pTZInd;
	uint8_t j; //, pMin, pMax, pMid, perInd, segInd;
	//
	//  Save the input value of lcc
	//
	lccInput = lcc;
	//
	// Check the validity of the input frame of reference
	//
	if (lcc.frame != 0)
	{
		// Input lcc is not universal
		lccInput.lccInit |= InvalidTimeFrame;
		return lccInput;
	}
	//
	// Check the validity of the time zone
	//
	for (i = 0; i < numTimeZones; ++i)
	{
		for (j = 0; j < PeriodTimeZones[i].numNames; ++j)
		{
			if (strncmp(timezone, PeriodTimeZones[i].names[j], maxTZNameLength) == 0)
			{
				// Found a match
				pTZInd = i;
				break;
			}
		}
		if (j < PeriodTimeZones[i].numNames) break;
	}
	lccInput.timezoneIndex = i;
	if (i == numTimeZones)
	{
		// Could not find a match.  Invalid time zone.
		lccInput.lccInit |= InvalidTimeZone;
		return lccInput;
	}
	//
	//  Copy the time zone name, instead of the link to speed searches
	strcpy_s(lccInput.timezone, sizeof(lccInput.timezone), TimeZones[i][0]);
	strcpy_s(lcc.timezone, sizeof(lcc.timezone), TimeZones[i][0]);
	//
	// Check the validity of the required frame
	//
	if (frame > 2)
	{ 
		// Invalid frame
		lccInput.lccInit |= InvalidTimeFrame;
		return lccInput;
	}
/*	//
	//  Convert to UTC Gregorian calendar to facilitate PeriodTimeZone lookup
	//
	if (lcc.cc.date.calendar != 0)
	{
		// Perform the conversion.  The conversion does not change the frame of reference.
		lcc.cc = convertCalToUTCGregorian(lcc.cc);
	} */
	//
	//  If the required time zone is different from the input time zone, 
	//	 recompute the time zone dependent fields
	//
	if (lcc.timezoneIndex != lccInput.timezoneIndex) lccInput = computeTZFields(lccInput);
	//  If the required frame is not universal, translate to the required frame
	return translateFrame(lccInput, frame);
/*		//
		//  Create arrays of fromCoords for the periods of the required time zone in
		//	 universal frame of reference.  As fromCoords for the n+1 period == untilCoords
		//	 for the nth period, we don't need both arrays.
		//
		for (i = 0; i < PeriodTimeZones[pTZInd].numPeriods; ++i)
		{
			fromCoords[i] = PeriodTimeZones[pTZInd].periods[i].fromCoords;
			// Translate to universal frame of reference
			fromCoords[i] = offsetLocalCalCoords(fromCoords[i], 
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].beginSaved));
			fromCoords[i] = offsetLocalCalCoords(fromCoords[i],
				negateUTCOffset(PeriodTimeZones[pTZInd].periods[i].gMTOffset));
		}
		//  Locate the period into which this LocalCalCoords falls
		//   Find the largest fromCoords where lcc >= fromCoords
		//   The below works when numPeriods == 1
		// Perform a half interval search
		pMin = 0;
		pMax = PeriodTimeZones[pTZInd].numPeriods - 1;
		while (pMin <= pMax)
		{
			pMid = (pMax + pMin) / 2;
			if (isGreaterOrEqualLDTLCC(lcc, fromCoords[pMid])) pMin = max(pMid, pMin + 1);
			else pMax = min(pMid, pMax - 1);
		}
		// The search is complete
		if (pMid > pMax) perInd = pMid - 1;
		else perInd = pMid;
		//  No need to check for invalid (i.e., skipped) region because universal times are continuous
		//
		//	Check for the edge condition where lcc < fromCoords for the first period
		//		This should not occur, as lcc is assumed to have been checked for validity
		//		 and the first period always should start from the beginning of time.
		//
		if (perInd == -1)
		{
			lccInput.lccInit |= NegativeTime;
			return lccInput;
		}
		lccInput.periodIndex = perInd;
		//
		//  If the frame is standard, apply the gmtOffset, set bOrA and return
		//
		if (frame == 1)
		{
			lccInput = offsetLocalCalCoordsDT(lccInput, PeriodTimeZones[pTZInd].periods[perInd].gMTOffset);
			lccInput.frame = 1;
			//  Set bOrA
			//   Check for ambiguous region.  This can occur for standard times when the gmtOffset changes
			//	 from one period to the next.  Can occur at either end of the period.
			//
			//	 Check the beginning of the period
			if (perInd > 0)
			{
				//  There is a previous period
				//   Calculate the until coords of the previous period in standard time
				lastPeriodUntil = fromCoords[perInd];
				lastPeriodUntil = offsetLocalCalCoords(lastPeriodUntil,
					PeriodTimeZones[pTZInd].periods[perInd - 1].gMTOffset);
				//  If lccInput is before lastPeriodUntil, it falls within an ambiguous
				//	 period.  Set bOrA to A.
				// The following function supports comparisons with different calendars
				if (isLessLDTLCC(lccInput, lastPeriodUntil))
				{
					// Falls within an ambiguous period.  This time is after the jump.
					lccInput.bOrA = 2;
					return lccInput;  // No need to check the end of the time period, as must
									  //  be near the beginning to be in this ambiguous region.
				}
			}
			//   Check the end of the period
			if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
			{
				//  There is a next period
				//	 Calculate the from coords of the next period in standard time
				nextPeriodFrom = offsetLocalCalCoords(fromCoords[perInd + 1],
					PeriodTimeZones[pTZInd].periods[perInd + 1].gMTOffset);
				//  If lccInput is >= nextPeriodFrom, it falls within an ambiguous
				//	 period.  Set bOrA to B.
				if (isGreaterOrEqualLDTLCC(lccInput, nextPeriodFrom))
				{
					// Falls within an ambiguous period.  This time is before the jump.
					lccInput.bOrA = 1;
					return lccInput;
				}
			}
			//	Does not fall within an ambiguous period
			lccInput.bOrA = 0;
			return lccInput;
		}
		//
		//	This is a wall time.
		//  Create the bracketing period segments for the year of lcc
		//
		brackSeg = createBracketingSegments(PeriodTimeZones[pTZInd].periods[perInd], lcc.cc.date.year);
		brackSegU = brackSeg;
		//  Translate the segments into universal frame of reference
		gMTOff = PeriodTimeZones[pTZInd].periods[perInd].gMTOffset;
		for (i = 0; i < brackSegU.numSegments; ++i)
		{
			brackSegU.segments[i].fromCoords = offsetLocalCalCoords(brackSegU.segments[i].fromCoords,
				negateUTCOffset(brackSegU.segments[i].sToW));
			brackSegU.segments[i].fromCoords = offsetLocalCalCoords(brackSegU.segments[i].fromCoords,
				negateUTCOffset(gMTOff));
			brackSegU.segments[i].untilCoords = offsetLocalCalCoords(brackSegU.segments[i].untilCoords,
				negateUTCOffset(brackSegU.segments[i].sToW));
			brackSegU.segments[i].untilCoords = offsetLocalCalCoords(brackSegU.segments[i].untilCoords,
				negateUTCOffset(gMTOff));
		}
		//  Locate the segment into which this LocalCalCoords falls
		//
		//	Find the first segment where lcc < untilCoords.
		for (i = 0; i < brackSegU.numSegments; ++i)
		{
			if (isLessLDTLCC(lcc, brackSegU.segments[i].untilCoords))
			{
				// lcc is within the ith segment
				//	Translate to wall time
				lccInput = offsetLocalCalCoordsDT(lccInput, gMTOff);
				lccInput = offsetLocalCalCoordsDT(lccInput, brackSegU.segments[i].sToW);
				break;
			}
		}
		//  The last segment should go to the end of universal time, so the following branch
		//	  should not be hit.
		if (i == brackSegU.numSegments)
		{
			// Time greater than max time
			lccInput.lccInit |= TimeGtMax;
			return lccInput;
		}
		segInd = i;
		//  Set bOrA
		//	Check for ambiguous region
		//
		lccInput.frame = 2;
		//
		//   Requires special check for beginning of first segment and end of last segment
		//	  since the overlaps creating the ambiguous regions is with other periods.
		//
		if (perInd > 0)
		{
			// There is a previous period
			if (isLessLDTLCC(lccInput, PeriodTimeZones[pTZInd].periods[perInd - 1].untilCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous period
				//	Set bOrA to A
				lccInput.bOrA = 2;
				return lccInput;
			}
		}
		if (perInd < PeriodTimeZones[pTZInd].numPeriods - 1)
		{
			// There is a next period
			if (isGreaterOrEqualLDTLCC(lccInput,
				PeriodTimeZones[pTZInd].periods[perInd + 1].fromCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the next period
				//	Set bOrA to B
				lccInput.bOrA = 1;
				return lccInput;
			}
		}
		//  Check for an overlap at the beginning of the segment.
		//	  This check needs to be performed in wall frame of reference.
		if (segInd > 0)
		{
			// There is a previous segment
			if (isLessLDTLCC(lccInput, brackSeg.segments[segInd - 1].untilCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the previous segment
				lccInput.bOrA = 2;
				return lccInput;
			}
		}
		// Check for an overlap at the end of the segment
		if (segInd < brackSeg.numSegments - 1)
		{
			// There is a next segment
			if (isGreaterOrEqualLDTLCC(lccInput, brackSeg.segments[segInd + 1].fromCoords))
			{
				// The lcc is in an ambiguous region created by an overlap with the next segment
				lccInput.bOrA = 1;
				return lccInput;
			}
		}
		// The lcc is not in an ambiguous region
		lccInput.bOrA = 0; 
	} 
	return lccInput; */
}

LocalCalCoordsDT translateLocalCalCoordsDT(LocalCalCoordsDT lcc, const char timezone[], uint8_t frame)
{
	// Translate a LocalCalCoordsDT to a specified time zone and frame of reference
	//	 Translate to universal frame of reference
	lcc = translateFrame(lcc, 0);
	//	 Translate to required time zone and frame of reference
	return translateFromUniversal(lcc, timezone, frame);
}

LocalDatetime createLocalDatetimeFromLocalCalCoordsDT(LocalCalCoordsDT lcc, int8_t precision,
	int8_t uncertainty)
{
	// Create a Local Datetime from a LocalCalCoords
	//	Assume LocalCalCoords is valid
	LocalDatetime ldt;
	UTCDatetime utc;
	CalCoords ccSave;
	uint8_t periodIndex, fracOffset;
	uint32_t timezoneIndex;
	//
	//   Adjust the CalCoords to be consistent with the precision
	//
	lcc.cc = adjustCalCoords(lcc.cc, precision);
	//
	//	 Save the value of CalCoords, before translating to universal
	//
	ccSave = lcc.cc;
	//
	//   Translate the frame of reference to universal levering the PeriodTimeZones array
	//
	periodIndex = 0;
	timezoneIndex = numTimeZones;
	lcc = translateToUniversal(lcc);
	//
	//  Create a UTCDatetime if other inputs are valid.  	
	if (lcc.lccInit != 0)
	{
		ldt.ldtInit = lcc.lccInit;
		return ldt;
	}
	utc = createUTCDatetimeFromCalCoords(lcc.cc, precision, uncertainty, lcc.futureAdjust);
	//
	// Assign values to the ldt parameters.  Values may have changed when creating UTCDatetime due
	//   to precision and uncertainty specifications
	//
	if (utc.taiInit != 0)
	{
		ldt.ldtInit = utc.taiInit;
		return ldt;
	}
	ldt.calendar = ccSave.date.calendar;
	ldt.gigayear = ccSave.date.gigayear;
	ldt.year = ccSave.date.year;
	ldt.month = ccSave.date.month;
	ldt.dayOfMonth = ccSave.date.dayOfMonth;
	ldt.hour = ccSave.time.hour;
	ldt.minute = ccSave.time.minute;
	ldt.second = ccSave.time.second;
	ldt.tai = utc.tai;
	ldt.precision = utc.precision;
	ldt.uncertainty = utc.uncertainty;
	// Find the fractional second value of the GMT Offset in hundredths of a second
	fracOffset = PeriodTimeZones[timezoneIndex].periods[periodIndex].gMTOffset.hundredths;
	ldt.tzFields = packTimeZoneFields(lcc.timezone, periodIndex, lcc.frame, lcc.ambigS,
		lcc.ambigW, lcc.beforeFlag, lcc.futureAdjust, fracOffset);
	ldt.sToWMinutes = lcc.sToWMinutes;
	ldt.ldtInit = 0;
	return ldt;
}

UTCDatetime createUTCDatetimeFromLocalCalCoordsDT(LocalCalCoordsDT lcc, int8_t precision,
	int8_t uncertainty)
{
	// Create a UTCDatetime from a LocalCalCoords
	//	Assume LocalCalCoords is valid
	UTCDatetime utc;
	//
	//   Adjust the CalCoords to be consistent with the precision
	//
	lcc.cc = adjustCalCoords(lcc.cc, precision);
	//
	//   Translate the frame of reference to universal levering the PeriodTimeZones array
	//
	lcc = translateToUniversal(lcc);
	//
	//  Create a UTCDatetime if other inputs are valid.  	
	if (lcc.lccInit != 0)
	{
		utc.taiInit = lcc.lccInit;
		return utc;
	}
	return createUTCDatetimeFromCalCoords(lcc.cc, precision, uncertainty, lcc.futureAdjust);
}

TZPeriod pBegNoPeriodTZArray(LocalCalCoords lccPrevTZVerUntil, TimeZone tzver, uint32_t ruleSetIndex,
	UTCOffset prevGMTOffset, UTCOffset prevEndSaved, TZPeriod p)
{
	// Compute the effective rule and associated s-w offset at the start of the first period of a
	//	new time zone version.
	//  lccPrevTZUntil is the until coords of the previous time zone version, which is expressed
	//	as a wall time in terms of the GMTOffset and DSTR of the previous time zone.
	//
	// To ensure we compute the correct s-w UTCOffset, we need to convert lccPrevTZUntil to a standard
	//	time, then compute the sp1 UTCOffset so we know the corresponding standard time in this
	//  period.
	//
	//	Only should be used by createTimeZonePeriods
	//
	uint32_t prevRuleVersionIndex, ruleVersionIndex, year;
	uint8_t j;
	LocalCalCoords lccBegin, lccNextRuleEffective;
	TZRule ruleVer;
	UTCOffset gMTOffset, sp1, stowBefore;

	// Compute GMTOffset in the current time zone verson
	gMTOffset = createUTCOffset(tzver.isNegativeGMTOffset, 0, tzver.hoursOfGMTOffset, tzver.minutesOfGMTOffset,
		tzver.secondsOfGMTOffset, tzver.hoursOfGMTOffset, "u-s");
	// Compute the standard time jump
	sp1 = subtractUTCOffsets(gMTOffset, prevGMTOffset);
	// Compute the LocalCalCoords at the beginning of the period in standard time
	lccBegin = offsetLocalCalCoords(lccPrevTZVerUntil, negateUTCOffset(prevEndSaved));
	lccBegin = offsetLocalCalCoords(lccBegin, sp1);

	year = lccBegin.cc.date.year;

	//	The effective rule could be the rule that was effective at the end
	//	of the previous year, which, in this case, could not be from a different rule set.
	//  This is due to the fact that this function is called already knowing the rule set is
	//	applicable in the previous year.
	// Select the last rule in the rule set, as they are ordered by effective month
	prevRuleVersionIndex = TZRuleVersionSets[ruleSetIndex].
		ruleVersionIndices[TZRuleVersionSets[ruleSetIndex].numRuleVers - 1];
	//  Need to calculate s-w offset in effect at the At time
	ruleVer = TZRuleVersions[prevRuleVersionIndex];
	stowBefore = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");


	// Loop through the rule versions to find the latest effective datetime before
	//	the until datetime in the frame of reference of the until datetime
	for (j = 0; j < TZRuleVersionSets[ruleSetIndex].numRuleVers; j++)
	{
		ruleVersionIndex = TZRuleVersionSets[ruleSetIndex].ruleVersionIndices[j];
		lccNextRuleEffective = createAtLocalCalCoords(
			TZRuleVersions[ruleVersionIndex],
			tzver.name, year, lccBegin.frame, gMTOffset, stowBefore);
		if (isGreaterLocalCalCoords(lccNextRuleEffective, lccBegin))
		{
			// The At datetime is beyond the until datetime, so 
			//	the rule version before this is the applicable one
			break;
		}
		prevRuleVersionIndex = ruleVersionIndex;
		ruleVer = TZRuleVersions[prevRuleVersionIndex];
		stowBefore = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
	}
	//  The next statement works even if we reached the end of the rule set
	ruleVer = TZRuleVersions[prevRuleVersionIndex];
	p.begRule = (j > 0 ? j - 1 : TZRuleVersionSets[ruleSetIndex].numRuleVers - 1);
	// Calculate the s-w offset
	p.beginSaved = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved,
		0, 0, "s-w");
	return p;
}

BracketingSegments createBracketingSegments(TZPeriod period, uint32_t year)
{
	// Create the bracketing period segments for a specified year in the wall frame of reference.
	//	To support translations from wall time to standard time, need to cover from 1/1/year 0:00 (w)
	//	to 1/1/year+1 0:00 (w).  To support translations from standard time to wall time, need to cover
	//	from 1/1/year 0:00 (s) to 1/1/year+1 0:00 (s).  As the from and to years of the rule sets
	//	are assumed to be defined in terms of wall time, the most accurate to cover a standard year
	//	is to extend the wall year by a day on either side.
	//
	BracketingSegments brackSeg;
	PeriodSegment perSeg;
	UTCOffset stow, begJump, sToWPrev;
	TZRule ruleVer;
	LocalCalCoords ruleEffective, begYearBefore, begYearAfter, begNextYearBefore, nextFrom,
		begDec31Before, begDec31After, endJan1Before;
	uint8_t numSeg, i, begRuleIndex;
	char timezone[maxTZNameLength];
	//
	// Divide the year into segments based on s-w offset.
	//
	//  If there are n applicable rules in a year, the year can consist of n+1
	//	 segments.  Additionally, we can have ambiguous regions at the very end
	//   and / or very beginning of the year, which can create an additional segment.
	//	 Finally we can add an extra segment to cover 12/31 of the previous year
	//	 and one to cover 1/1 of the next year.
	//
	//  The following is an example of the above:
	//   A rule set has 3 rules with the following At coordinates all in wall time with
	//		gmtoffset = 0
	//	 Rule 1 At 1/1 01:00 Offset -3 hours
	//	 Rule 2 At 5/1 01:00 Offset -2 hours
	//   Rule 3 At 10/1 0:00 Offset 0 hours
	//	 The year we are analyzing is 1969
	//
	//   As the 0 hour offset of Rule 3 is effective for the purposes of calculating
	//	  the At coordinates of Rule 1, Rule 1 becomes effective at 1/1/69 01:00.  
	//    The time immediately moves to 12/31 22:00, creating am ambiguous region
	//    from 12/31/68 22:00 to 1/1/69 01:00.  The last hour of that region is within
	//    the year.  
	//	  In this case, we create a first segment beginning at 12/31/68 22:00
	//	  with untilCoords of 1/1/69 01:00.  The second segment begins at 1/1/69 0:00 
	//    and ends at 5/1/69 01:00.  At this point, the time immediately moves to
	//    5/1/69 02:00, creating a 1 hour invalid region.  The third segment begins
	//    at 5/1/69 02:00 and ends at 10/1/69 0:00.  At this point, the time immediately
	//    moves to 10/1/69 02:00, creating a 2 hour invalid region.  The fourth segment
	//    begins at 10/1/69 02:00 and ends at 1/1/70 0:00.  However, as there is another
	//    segment beginning at 1/1/70 01:00, which causes a 3 hour jump backwards, we
	//    need to add a 5th segment beginning at 12/31/69 22:00 and ending at 1/1/70 01:00.
	//
	//	Given this structure, the algorithm to determine into which segment a LocalCalCoords
	//	  in wall frame of reference falls is as follows:
	//  If lcc.bOrA == 'b', iterate through the segments finding the first one (smallest one) where lcc < untilCoords.
	//     An lcc of 1/1/69 00:30 would be identified as occurring in the first segment, which is
	//	   correct, while an lcc of 12/31/69 23:15 would be identified as occurring in the fourth 
	//	   segment, which is also correct.
	//  If lcc.bOrA == 'a', iterate through the segements finding the last one where lcc >= fromCoords.
	//	   From the above example, an lcc of 1/1/69 00:30 would be identified as occurring in the second
	//	   segment, while an lcc of 12/31/69 23:15 would be identified as occuring in the fifth segment.
	//  If lcc.bOrA == ' ', set to b.  Iterate through the segments finding the first one where lcc < untilCoords.
	//     If this is not the first segment, check for overlap with previous segment.
	//     If this is the last segment, we have found the proper segment.  If not the last segment,
	//     change lcc.bOrA to 'a', and check whether lcc >= fromCoords of next segment.  If it is,
	//     there is an error, as we are in an ambiguous regaion.  If not, this is the correct segment.
	//
	//	The fromCoords of the first segment is the start of the year,
	//	 unless this is the from year of the period, in which case it is the
	//	 fromCoords of the period,
	//	 while the toCoords of the last segment is the start of the next year,
	//   unless this is the to year of the period, in which case it is the
	//   toCoords of the period.
	//
	//	Retrieve the time zone
	//
	strcpy_s(timezone, maxTZNameLength, TimeZoneVersions[period.tZVersion].name);
	//
	//  Create LocalCalCoords for beginning of this year and beginning of next year
	//
	begYearBefore = createLocalCalCoords(0, year, 1, 1, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	begYearAfter = createLocalCalCoords(0, year, 1, 1, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	begNextYearBefore = createLocalCalCoords(0, year + 1, 1, 1, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	//
	//  Create LocalCalCoords for the start of 12/31 of the previous year and the end
	//	 of 1/1 of the next year
	//
	begDec31Before = createLocalCalCoords(0, year - 1, 12, 31, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	begDec31After = createLocalCalCoords(0, year-1, 12, 31, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	endJan1Before = createLocalCalCoords(0, year + 1, 1, 2, 0, 0, 0, 0, 0, 0, timezone,
		'w');
	//
	//  If there are no rules, there is only one segment
	//
	if (period.nRuleVersions == 0)
	{
		// Make the segment span the year
		//  No need to be concerned about overlaps at either end of the span,
		//    since could only occur at fromCoords or untilCoords of the period
		//    and any such overlaps would have been resolved in the function that
		//    determines the proper period (e.g., createLocalDatetimeFromLocalCalCoords)
		//    prior to calling this function.
		brackSeg.numSegments = 1;
		if (year == period.fromCoords.cc.date.year) perSeg.fromCoords = period.fromCoords;
		else if(year - 1 == period.fromCoords.cc.date.year && 
			isGreaterLocalCalCoords(period.fromCoords, begDec31After)) perSeg.fromCoords = period.fromCoords;
		else perSeg.fromCoords = begDec31After;
		//
		if (year == period.untilCoords.cc.date.year) perSeg.untilCoords = period.untilCoords;
		else if(year + 1 == period.untilCoords.cc.date.year &&
			isLessLocalCalCoords(period.untilCoords, endJan1Before)) perSeg.untilCoords = period.untilCoords;
		else perSeg.untilCoords = endJan1Before;
		//
		perSeg.sToW = period.tZSaved;
		brackSeg.segments[0] = perSeg;
		return brackSeg;
	}
	else if (period.nRuleVersions == 1)
	{
		// There is only one rule verison, so there is only 1 segment.
		//	 The rule is effective for the entire year in all years after the from year.
		//   In the from year, if the rule is not effective at the beginning of the year,
		//   the period will not start until it is effective, therefore there is no need
		//   for a second segment.
		//
		brackSeg.numSegments = 1;
		if (year == period.fromCoords.cc.date.year) perSeg.fromCoords = period.fromCoords;
		else if (year - 1 == period.fromCoords.cc.date.year &&
			isGreaterLocalCalCoords(period.fromCoords, begDec31After)) perSeg.fromCoords = period.fromCoords;
		else perSeg.fromCoords = begDec31After;
		//
		if (year == period.untilCoords.cc.date.year) perSeg.untilCoords = period.untilCoords;
		else if (year + 1 == period.untilCoords.cc.date.year &&
			isLessLocalCalCoords(period.untilCoords, endJan1Before)) perSeg.untilCoords = period.untilCoords;
		else perSeg.untilCoords = endJan1Before;
		//
		ruleVer = TZRuleVersions[period.ruleVersions[0]];
		perSeg.sToW = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
		brackSeg.segments[0] = perSeg;
		return brackSeg;
	}
	else
	{
		// Construct the period segments
		numSeg = 1;
		//
		// Construct the first segment.  
		//
		//   Different approach if this is the from year
		//
		if (year == period.fromCoords.cc.date.year)
		{
			// No need to check for overlapping segments or invalid regions, as the calling function already has
			//	determined this to be the correct period.  Ie, if there is an overlapping segment that includes
			//	the beginning of this period, the required disambiguation would have been performed in the 
			//	function that calls that one and the previous period selected, if appropriate.)
			perSeg.fromCoords = period.fromCoords;
			perSeg.sToW = period.beginSaved;
			begRuleIndex = period.begRule + 1;
			if (begRuleIndex == period.nRuleVersions) begRuleIndex = 0;
			sToWPrev = period.beginSaved;
		}
		else if (year - 1 == period.fromCoords.cc.date.year &&
			isGreaterLocalCalCoords(period.fromCoords, begDec31After))
		{
			// The fromCoords in the previous year is on 12/31.  Create a first segment
			//	beginning on fromCoords
			perSeg.fromCoords = period.fromCoords;
			perSeg.sToW = period.beginSaved;
			// At this point, there are 2 possibilities:
			//	1. Another rule version becomes effective later in the day on 12/31
			//  2. The segment extends until the first rule version becomes effective 
			//
			// Determine when the last rule version becomes effective in the previous year
			sToWPrev = period.beginSaved;
			ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
			ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
				year - 1, 2, period.gMTOffset, sToWPrev);
			// Check whether the rule version becomes effective after fromCoords
			if (isGreaterLocalCalCoords(ruleEffective, period.fromCoords))
			{
				// The last rule becomes effective after fromCoords
				//   Find the stow offset associated with the last rule version.
				stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
				//	 Find the jump associated with the transition to the last rule version.
				begJump = addUTCOffsets(
					subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
					subtractUTCOffsets(stow, sToWPrev));
				//   Apply begJump to the effective date of the last rule version
				nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
				//   Store the segment starting at fromCoords
				perSeg.untilCoords = ruleEffective;
				brackSeg.segments[numSeg - 1] = perSeg;
				//   Create another segment
				numSeg++;
				perSeg.fromCoords = nextFrom;
				perSeg.sToW = stow;
				sToWPrev = stow;
				//	 This segment ends when the first rule version becomes effective
				begRuleIndex = 0;
			}
			else
			{
				// The last rule version becomes effective before fromCoords, so this segment
				//	ends when the first rule becomes effective
				begRuleIndex = 0;
			}
		}
		else
		{
			// Find the rule that was in effect at midnight of 12/31 of the previous year.
			//	Will either be the last rule version or next to last rule version.
			// 
			//	Determine when the last rule version becomes effective
			//	  First, determine the s-w offset associated with the next to last rule version.  (We know there
			//	  are at least two rule versions.)  
			ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 2]];
			sToWPrev = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//   Find the effective date of the last rule version.
			ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
			ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
				year - 1, 2, period.gMTOffset, sToWPrev);
			//   If this is after 12/31 0:00, the next to last rule version is effective, otherwise
			//	  the last rule version is effective.
			if (isGreaterLocalCalCoords(ruleEffective, begDec31Before))
			{
				// The next to last rule version is effective
				// Create a segment beginning at midnight on 12/31, which will end when the
				//	last rule becomes effective.  No need to consider overlaps or invalid regions
				//	at the start of 12/31, as the period of interest is at the end of 12/31 and
				//	any overlaps / invalid regions will have been resolved by then.
				//
				perSeg.fromCoords = begDec31After;
				ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 2]];
				stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
				perSeg.sToW = stow;
				perSeg.untilCoords = ruleEffective;
				brackSeg.segments[numSeg - 1] = perSeg;
				// Create another segment, which will end when the first rule becomes effective.
				numSeg++;
				sToWPrev = stow;
				ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
				//   Find the stow offset associated with the last rule version.
				stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
				//	 Find the jump associated with the transition to the last rule version.
				begJump = addUTCOffsets(
					subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
					subtractUTCOffsets(stow, sToWPrev));
				//   Apply begJump to the effective date of the last rule version
				nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
				perSeg.fromCoords = nextFrom;
				perSeg.sToW = stow;
				sToWPrev = stow;
				begRuleIndex = 0;
			}
			else
			{
				// The last rule version is effective
				// Create a segment beginning at midnight on 12/31, which will end when the
				//	first rule becomes effective.  No need to consider overlaps or invalid regions
				//	at the start of 12/31, as the period of interest is at the end of 12/31 and
				//	any overlaps / invalid regions will have been resolved by then.
				//
				perSeg.fromCoords = begDec31After;
				ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
				stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
				perSeg.sToW = stow;
				sToWPrev = stow;
				begRuleIndex = 0;
			}
			//// Check for an invalid region at the start of the year
			////	An invalid region can be created by a positive jump after a transition late in the
			////	 day on 12/31 or by a transition at the instant the year begins with a positive jump.
			////	(Theoretically, the transition for the first rule of the year could occur before the start
			////	  of the year if the transition is specified as a universal or standard time transition.  The
			////	  current IANA database does not have any such transitions, so we won't treat them.  If
			////	  there were to be such transitions, code outside of this function would need to change.)
			////
			////  Find the datetime of the last transition of the previous year.
			////	  First, determine the s-w offset associated with the next to last rule version.  (We know there
			////	  are at least two rule versions.)  
			//ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 2]];
			//sToWPrev = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			////   Find the effective date of the last rule version.
			//ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions-1]];
			//ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
			//	year - 1, 2, period.gMTOffset, sToWPrev);
			////   We will not check for the pathological case where the previous year contains fromCoords and
			////	  fromCoords is after the last transition and the last transition is at the end of the day on 12/31.
			////   Find the stow offset associated with the last rule version.
			//stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			////	 Find the jump associated with the transition to the last rule version.
			//begJump = addUTCOffsets(
			//	subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
			//	subtractUTCOffsets(stow, sToWPrev));
			////   Apply begJump to the effective date of the last rule version
			//nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
			////   Determine whether nextFrom is in the current year
			//if (nextFrom.cc.date.year == year)
			//{
			//	// The jump moves the datetime into the current year, creating an invalid region at the
			//	//	start of the year.
			//	// Handle this by setting fromCoords for the first segment accordingly.
			//	perSeg.fromCoords = nextFrom;
			//	perSeg.sToW = stow;
			//	begRuleIndex = 2;
			//	sToWPrev = stow;
			//}
			//else
			//{
			//	// Check whether the first transition occurs at the instant the year starts and there
			//	//	is a positive jump.
			//	// Find the datetime of the first transition of the year
			//	//   First, find the s-w offset at the end of the previous year, which will
			//	//		always be the offset of the last rule version.
			//	ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
			//	sToWYearEnd = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//	//   Find the effective date of the first rule version.
			//	ruleVer = TZRuleVersions[period.ruleVersions[0]];
			//	ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
			//		year, 2, period.gMTOffset, sToWYearEnd);
			//	//   Determine whether the rule becomes effective at the instant the year begins
			//	if (isEqualLocalCalCoords(ruleEffective, begYearBefore))
			//	{
			//		// The transition to the first rule occurs at the instant the year begins
			//		//   Find the stow offset associated with the first rule version.
			//		stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//		//	 Find the jump associated with the transition to the first rule version.
			//		begJump = addUTCOffsets(
			//			subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
			//			subtractUTCOffsets(stow, sToWYearEnd));
			//		//   Apply begJump to the effective date of the first rule
			//		nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
			//		//	 Determine whether there is an invalid region
			//		if (isGreaterLocalCalCoords(nextFrom, begYearAfter))
			//		{
			//			// There is an invalid region.  Set fromCoords accordingly.
			//			perSeg.fromCoords = nextFrom;
			//			perSeg.sToW = stow;
			//			begRuleIndex = 2;
			//			sToWPrev = stow;
			//		}
			//		else
			//		{
			//			// There is not an invalid region.  Begin the segment at the beginning of the year.
			//			perSeg.fromCoords = begYearAfter;
			//			perSeg.sToW = stow;
			//			begRuleIndex = 2;
			//			sToWPrev = stow;
			//		}
			//	}
			//	else
			//	{
			//		// There is no invalid region at the start of the year.  Still need to check for
			//		//	an overlapping segment.
			//		//	An overlapping segment occurs when the transition is just after the year begins and
			//		//	there is a negative jump.  There is not an overlapping segment if the transition occurs at
			//		//	the instant the year begins.
			//		//
			//		//   First, find the s-w offset at the end of the previous year, which will
			//		//		always be the offset of the last rule version.
			//		ruleVer = TZRuleVersions[period.ruleVersions[period.nRuleVersions - 1]];
			//		sToWYearEnd = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//		//   Find the effective date of the first rule version.
			//		ruleVer = TZRuleVersions[period.ruleVersions[0]];
			//		ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
			//			year, 2, period.gMTOffset, sToWYearEnd);
			//		//   Find the stow offset associated with the first rule version.
			//		stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//		//	 Find the jump associated with the transition to the first rule version.
			//		begJump = addUTCOffsets(
			//			subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
			//			subtractUTCOffsets(stow, sToWYearEnd));
			//		//   Apply begJump to the effective date of the first rule
			//		nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
			//		//   Determine whether nextFrom is in the previous year
			//		if (nextFrom.cc.date.year != year)
			//		{
			//			//  There is an overlap at the start of the year.  Insert a
			//			//	  short segment to handle this overlap.
			//			//  Ignore the pathological case where period.untilCoords is before the first
			//			//	  rule version becomes effective in the case where the jump crosses to the previous year.
			//			//
			//			perSeg.fromCoords = nextFrom;
			//			perSeg.untilCoords = ruleEffective;
			//			perSeg.sToW = sToWYearEnd;
			//			brackSeg.segments[numSeg - 1] = perSeg;
			//			//  Create another segment starting at begYear, which will end either when the second
			//			//	  rule version becomes effective or when untilCoords is reached
			//			numSeg++;
			//			perSeg.fromCoords = begYearAfter;
			//			perSeg.sToW = stow;
			//			begRuleIndex = 2;
			//			sToWPrev = stow;
			//		}
			//		else if (isEqualLocalCalCoords(nextFrom, begYearAfter))
			//		{
			//			// This is an edge condition, where the first rule version becomes effective (after the jump)
			//			//	 the instant the year begins.
			//			//  The first segment starts at the beginning of the year and ends either when
			//			//	 the second rule version becomes effective or untilCoords is reached
			//			perSeg.fromCoords = begYearAfter;
			//			perSeg.sToW = stow;
			//			begRuleIndex = 2;
			//			sToWPrev = stow;
			//		}
			//		else
			//		{
			//			// The first segment starts at the beginning of the year and ends either when
			//			//	the first rule version becomes effective or untilCoords is reached
			//			perSeg.fromCoords = begYearAfter;
			//			perSeg.sToW = sToWYearEnd;
			//			if (isGreaterOrEqualLocalCalCoords(ruleEffective, period.untilCoords))
			//			{
			//				perSeg.untilCoords = period.untilCoords;
			//				brackSeg.numSegments = numSeg;
			//				brackSeg.segments[numSeg - 1] = perSeg;
			//				return brackSeg;
			//			}
			//			else
			//			{
			//				perSeg.untilCoords = ruleEffective;
			//				brackSeg.segments[numSeg - 1] = perSeg;
			//				//  Create another segment starting after the jump after the first rule version becomes effective,
			//				//    which will end either when the second rule version becomes effective or when untilCoords is reahed
			//				numSeg++;
			//				perSeg.fromCoords = nextFrom;
			//				perSeg.sToW = stow;
			//				begRuleIndex = 2;
			//				sToWPrev = stow;
			//			}
			//		}
			//	}
			//}
		}
		//
		//	Create the remaining segments for the year
		//
		//	Loop the rule versions beginning with next rule version.  A new segment begins
		//	 when each rule version becomes effective.
		//
		for (i = begRuleIndex; i < period.nRuleVersions; ++i)
		{
			// Determine the effective date of rule version i
			ruleVer = TZRuleVersions[period.ruleVersions[i]];
			ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
				year, 2, period.gMTOffset, sToWPrev);
			// Check whether untilCoords has been reached
			if (isGreaterOrEqualLocalCalCoords(ruleEffective, period.untilCoords))
			{
				// This rule version becomes effective after untilCoords is reached
				//	Finished processing this year
				perSeg.untilCoords = period.untilCoords;
				brackSeg.numSegments = numSeg;
				brackSeg.segments[numSeg - 1] = perSeg;
				return brackSeg;
			}
			//	Add the period segment to the array
			perSeg.untilCoords = ruleEffective;
			brackSeg.segments[numSeg - 1] = perSeg;
			//
			//  Create another segment
			//
			if (i != begRuleIndex) sToWPrev = stow;
			//   Find the stow offset associated with this rule version.
			stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
			//	 Find the jump associated with the transition to this rule version.
			begJump = addUTCOffsets(
				subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
				subtractUTCOffsets(stow, sToWPrev));
			//   Apply begJump to the effective date of this rule version
			nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
			////	Check for an invalid region at the end of the year.
			////  If the last rule becomes effective on 12/31 of the year towards the
			////		end of the day, and there is a forward jump past 1/1 0:00, there is an invalid
			////		region at the end of the year.  This is handled by not creating another segment.
			////
			////		If this is the case, nextFrom will be in the next year
			////
			//if (nextFrom.cc.date.year != year)
			//{
			//	brackSeg.numSegments = numSeg;
			//	return brackSeg;
			//}
			//else
			//{
			numSeg++;
			perSeg.fromCoords = nextFrom;
			perSeg.sToW = stow;
			sToWPrev = stow;
		}
		// 
		//  All rule versions have been processed for the year.  
		//	  Handle the end of the year and 1/1/year+1
		//
		//	Add a segment that ends at the earliest of untilCoords,
		//	 the effective date of the first rule in year + 1 (if the
		//	 effective date is on 1/1), or the end of the day on 1/1
		//
		//	If this is the until year, the current segment ends at untilCoords
		//	 and processing is complete.
		//
		if (year == period.untilCoords.cc.date.year)
		{
			perSeg.untilCoords = period.untilCoords;
			brackSeg.numSegments = numSeg;
			brackSeg.segments[numSeg - 1] = perSeg;
			return brackSeg;
		}
		//
		//   Find the effective date of the first rule version in the next year.
		//
		ruleVer = TZRuleVersions[period.ruleVersions[0]];
		ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
			year + 1, 2, period.gMTOffset, perSeg.sToW);
		//
		//   Check whether the rule version becomes effective before the end of the day on 1/1
		//
		if (isLessLocalCalCoords(ruleEffective, endJan1Before))
		{
			// Rule version becomes effective before the end of Jan 1 of next year
			//	Determine whether untilCoords is before 
			if (isLessLocalCalCoords(period.untilCoords, ruleEffective))
			{
				// untilCoords is on 1/1 of the next year, before the first rule becomes effective
				//	The current segment ends at untilCoords and processing is complete
				perSeg.untilCoords = period.untilCoords;
				brackSeg.numSegments = numSeg;
				brackSeg.segments[numSeg - 1] = perSeg;
				return brackSeg;
			}
			else
			{
				// The current segment ends when the first rule becomes effective
				perSeg.untilCoords = ruleEffective;
				// Store this segment
				brackSeg.segments[numSeg - 1] = perSeg;
				// Create another segment
				sToWPrev = stow;
				//   Find the stow offset associated with this rule version.
				stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
				//	 Find the jump associated with the transition to this rule version.
				begJump = addUTCOffsets(
					subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
					subtractUTCOffsets(stow, sToWPrev));
				//   Apply begJump to the effective date of this rule version
				nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
				numSeg++;
				perSeg.fromCoords = nextFrom;
				perSeg.sToW = stow;
				sToWPrev = stow;
				//	 This segment ends either at the earlier of untilCoords or the end of 1/1/year+1
				if (isLessLocalCalCoords(period.untilCoords, endJan1Before))
				{
					perSeg.untilCoords = period.untilCoords;
					brackSeg.numSegments = numSeg;
					brackSeg.segments[numSeg - 1] = perSeg;
					return brackSeg;
				}
				else
				{
					perSeg.untilCoords = endJan1Before;
					brackSeg.numSegments = numSeg;
					brackSeg.segments[numSeg - 1] = perSeg;
					return brackSeg;
				}
			}
		}
		else
		{
			// The rule version does not become effective before the end of January 1
			//	The next segment ends at the earlier of untilCoords and the end of January 1
			if (isLessLocalCalCoords(period.untilCoords, endJan1Before))
			{
				perSeg.untilCoords = period.untilCoords;
				brackSeg.numSegments = numSeg;
				brackSeg.segments[numSeg - 1] = perSeg;
				return brackSeg;
			}
			else
			{
				perSeg.untilCoords = endJan1Before;
				brackSeg.numSegments = numSeg;
				brackSeg.segments[numSeg - 1] = perSeg;
				return brackSeg;
			}
		}
	//	if (isLessOrEqualLocalCalCoords(period.untilCoords, begNextYearBefore))
	//	{
	//		// This segment ends at untilCoords.  Processing of the year is complete.
	//		//   No need to consider overlapping segment at the end of the year, as this
	//		//   has already been resolved by the function determining the proper period
	//		//   (e.g., createLocalDatetimeFromLocalCalCoords) prior to calling this function.
	//		perSeg.untilCoords = period.untilCoords;
	//		brackSeg.numSegments = numSeg;
	//		brackSeg.segments[numSeg - 1] = perSeg;
	//		return brackSeg;
	//	}
	//	// The segment ends at the end of the year
	//	perSeg.untilCoords = begNextYearBefore;
	//	brackSeg.segments[numSeg - 1] = perSeg;
	//	// Check whether the first segment of the next year overlaps into this year
	//	//   Find the effective date of the first rule version in the next year.
	//	ruleVer = TZRuleVersions[period.ruleVersions[0]];
	//	ruleEffective = createAtLocalCalCoords(ruleVer, timezone,
	//		year + 1, 2, period.gMTOffset, perSeg.sToW);
	//	//   Find the stow offset associated with the first rule version.
	//	stow = createUTCOffset(0, 0, ruleVer.hoursSaved, ruleVer.minutesSaved, 0, 0, "s-w");
	//	//	 Find the jump associated with the transition to the first rule version.
	//	begJump = addUTCOffsets(
	//		subtractUTCOffsets(period.gMTOffset, period.gMTOffset),
	//		subtractUTCOffsets(stow, perSeg.sToW));
	//	//   Apply begJump to the effective date of the first rule
	//	nextFrom = offsetLocalCalCoords(ruleEffective, begJump);
	//	//   Determine whether nextFrom is in the current year
	//	if (nextFrom.cc.date.year == year)
	//	{
	//		// Create an overlapping segment
	//		numSeg++;
	//		perSeg.fromCoords = nextFrom;
	//		perSeg.sToW = stow;
	//		perSeg.untilCoords = ruleEffective;
	//		brackSeg.segments[numSeg - 1] = perSeg;
	//	}
	//	brackSeg.numSegments = numSeg;
	//	return brackSeg;
	}
}

uint32_t getLDTNanoSecond(LocalDatetime ldt)
{
	// Compute the nanoSecond field for a LocalDatetime
	//  
	//  The tai field of a LocalDatetime contains the nanosecond and attosecond
	//	 values of the datetime in a universal frame of reference.  
	//	 The daylight savings time offset is always in hours and minutes, so the
	//	 nanosecond and attosecond fields are not affected.  However, the GMT offset
	//	 can have hundredths of a second.  
	//  To compute nanosecond, need to add the fractional second part of the GMT offset,
	//	which is stored in the tzFields field.
	//
	return (getNanosecond(ldt.tai) + (uint32_t)getFracOffset(ldt.tzFields)) % e9;
}

int getMaxTZNameLength(void)
{
	// Return the maximum timezone name length.  
	//	Used by Julia to dimension a vector in LocalCalCoords struct
	return (int)maxTZNameLength;
}

LocalCalCoordsDT julianUntilCoords(uint32_t calendar, uint32_t timezoneIndex, uint8_t frame)
{
	// Determine the untilCoords for the Julian portion of a calendar for a specified time zone
	//	and frame of reference.
	//
	// We assume the transition from the Julian to the Gregorian calendar occurs at midnight of
	//	the day after the last Julian date in the wall frame of reference.  Using the convention
	//	of the Until datetime in the IANA file, the last Julian instant is the tick before the
	//  until coords.
	//
	// The untilCoords cannot be ambiguous in the wall frame of reference, as the calendar
	//	specification does not have a bOrA flag.  However, when translated to the standard
	//	frame of reference, the time might be ambiguous, requiring that the value be a
	//	LocalCalCoordsDT and not a LocalCalCoords
	//
	LocalCalCoordsDT lccdt, lccdtP1;
	CalCoords cc;
	DateCoords dateLJ, dateLJP1;
	uint8_t daysInMonth = 31;
	//
	//  Check that the calendar is within a valid range.  This function should not be invoked with
	//	 an extended calendar.
	//
	if (calendar >= e9)
	{
		// Invalid calendar
		lccdt.lccInit |= InvalidCalendar;
		return lccdt;
	}
	//
	//  Assume timezoneIndex and frame of reference have been checked prior to calling this function
	//
	//   Determine the last Julian date for this calendar.  
	//		The lastJulianDate function returns the correct date for wall frame of reference
	//
	dateLJ = lastJulianDate(calendar);
	//
	//	Increment the date by 1 day, using Julian calendar conventions, as the
	//	  julianUntilCoords will be midnight of the next day. In this way, validity of
	//	  a date on the specified calendar is checked via isLessLocalCalCoordsDT
	//
	dateLJP1 = dateLJ;
	// April, June, September and November only have 30 days
	if (dateLJP1.month == 4 || dateLJP1.month == 6 || dateLJP1.month == 9 || dateLJP1.month == 11) daysInMonth = 30;
	// February 
	if (dateLJP1.month == 2) daysInMonth = 28 + numLeapDays(0, dateLJP1.year, 1);
	dateLJP1.dayOfMonth++;
	if (dateLJP1.dayOfMonth > daysInMonth)
	{
		// Carry
		dateLJP1.dayOfMonth = 1;
		dateLJP1.month++;
		// Check for end of year
		if (dateLJP1.month > 12)
		{
			dateLJP1.month = 1;
			dateLJP1.year++;
		}
		// No reason to be concerned about gigayear carry, as maxLastJulianDate is in the 0th gigayear
	}
	//
	//  Convert the last Julian date to a proleptic Gregorian date to facilitate lookup in the PeriodTimeZones array
	//
	dateLJ = julianToUTCGregorian(dateLJ);
	//
	//  Build a LocalCalCoordsDT at the end of the last Julian date to facilitate a call to computeTZFields
	//	  We are using the end of the day, instead of midnight of the next to day to ensure we get the
	//	    proper values for GMTOffset and S-W, in the edge case where they change at the start of the
	//	    next day.
	//		Cannot use createLocalCalCoordsDT, as that would result in a recursive call to this function
	//
	cc.date = dateLJ;
	//  Build the timecoords
	cc.time.hour = 23;
	cc.time.minute = 59;
	cc.time.second = 59;
	cc.time.nanosecond = 0;
	cc.time.attosecond = 0;
	cc.time.timeInit = 0;
	lccdt.cc = cc;
	// Set the other elements required for the computeTZFields call
	lccdt.frame = 2; // Wall frame of reference
	lccdt.bOrA = 0; // Ambiguous times not allowed for Julian to Gregorian transition boundaries
	lccdt.futureAdjust = 2; // Not relevant for this call
	// Load the timezone field so comparison checks work
	strcpy_s(lccdt.timezone, sizeof(lccdt.timezone), TimeZones[timezoneIndex][0]);
	lccdt.timezoneIndex = timezoneIndex;
	lccdt.lccInit = 0;
	//
	//  Call computeTZFields to fill in the remaining fields
	//
	lccdt = computeTZFields(lccdt);
	//
	//  At this point, lccdt is in the wall frame of reference on a proleptic Gregorian calendar,
	//	  and represents the last second of the last Julian date
	//
	//  Construct another lccdt, at midnight of the day after the last Julian date, with the time
	//	  zone fields computed for lccdt
	//
	lccdtP1 = lccdt;
	lccdtP1.cc.date = dateLJP1;
	lccdtP1.cc.time.hour = 0;
	lccdtP1.cc.time.minute = 0;
	lccdtP1.cc.time.second = 0;
	//
	//  Translate to the required frame of reference, using the fields computed for lccdt,
	//	  and copied into lccdtP1
	//
	return translateFrame(lccdtP1, frame);
}

LocalCalCoordsDT gregorianFromCoords(uint32_t calendar, uint32_t timezoneIndex, uint8_t frame)
{
	// Determine the fromCoords for the Gregorian portion of a calendar for a specified time zone
	//	and frame of reference.
	//
	// We assume the transition from the Julian to the Gregorian calendar occurs at midnight of
	//	the day after the last Julian date in the wall frame of reference.  Using the convention
	//	of the From datetime in the PeriodTimeZones array, the first Gregorian instant is the tick of the
	//  from coords.
	//
	// The fromCoords cannot be ambiguous in the wall frame of reference, as the calendar
	//	specification does not have a bOrA flag.  However, when translated to the standard
	//	frame of reference, the time might be ambiguous, requiring that the value be a
	//	LocalCalCoordsDT and not a LocalCalCoords
	//
	LocalCalCoordsDT lccdt;
	CalCoords cc;
	DateCoords date;
	uint8_t daysInMonth = 31;
	//
	//  Check that the calendar is within a valid range.  This function should not be invoked with
	//	 an extended calendar.
	//
	if (calendar >= e9)
	{
		// Invalid calendar
		lccdt.lccInit |= InvalidCalendar;
		return lccdt;
	}
	//
	//  Assume timezoneIndex and frame of reference have been checked prior to calling this function
	//
	//   Determine the first Gregorian date for this calendar.  
	//		The firstGregorianDate function returns the correct date for wall frame of reference
	//
	date = firstGregorianDate(calendar);
	//
	//  Build a LocalCalCoordsDT to facilitate a call to computeTZFields
	//		Cannot use createLocalCalCoordsDT, as that would result in a recursive call to this function
	//
	cc.date = date;
	//  Build the timecoords
	cc.time.hour = 0;
	cc.time.minute = 0;
	cc.time.second = 0;
	cc.time.nanosecond = 0;
	cc.time.attosecond = 0;
	cc.time.timeInit = 0;
	lccdt.cc = cc;
	// Set the other elements required for the computeTZFields call
	lccdt.frame = 2; // Wall frame of reference
	lccdt.bOrA = 0; // Ambiguous times not allowed for transition boundaries
	lccdt.futureAdjust = 2; // Not relevant for this call
	// Load the timezone field so comparison checks work
	strcpy_s(lccdt.timezone, sizeof(lccdt.timezone), TimeZones[timezoneIndex][0]);
	lccdt.timezoneIndex = timezoneIndex;
	lccdt.lccInit = 0;
	//
	//  Call computeTZFields to fill in the remaining fields
	//
	lccdt = computeTZFields(lccdt);
	//
	//  Translate to the required frame of reference
	//
	return translateFrame(lccdt, frame);
}