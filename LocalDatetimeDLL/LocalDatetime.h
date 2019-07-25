#include "../UltimateDatetimeDLL/UltimateDatetime.h"
#include "rules.h"
#include "timezones.h"

// The following were computed from a call to createTimeZonePeriods 
#define maxTimeZonePeriods 54 // The maximum number of periods for a time zone
#define maxTimeZoneVersionPeriods 52 // The maximum number of periods for a time zone verson

#define e7 10000000

typedef struct
{
	/* The elements of a UTCOffset */

	/* A UTCOffset is used to translate from local time coordinates to UTC.
	The elements are specified as calendar coordinates, instead of as
	a relative datetime, since the local time cannot be converted to
	ticks prior to the conversion to UTC */
	uint8_t isNegative; // 0 for positive, 1 for negative
	uint8_t days; // allow offsets with 0 or 1 day.  Used for offsets >= 24 hours.
	uint8_t hours; // hours in the offset.  If >=24 hours, set days to 1.
	uint8_t minutes; // minutes in the offset
	uint8_t seconds; // seconds in the offset.  Seconds only allowed prior to
					 //	the advent of legal time zones
	uint8_t hundredths; // hundredths of a second in the offset
	uint8_t offsetType; /* Offset type:
						0 = u to s - GMT Offset - add to universal time to calculate standard time
						1 = s to w - DST Offset - add to standard time to calculate wall time
						When no time is being saved, wall time = standard time
						2 = u to w - GMT Offset + DST Offset - add to univeral time to calculate wall time
						3 = sp1 - jump in standard time associated with a change in GMT offset for
						a particular time zone
						4 = dp1 - jump in saved time associated with a change in effective DSTR for
						a particular time zone
						5 = wp1 - jump in wall time associated with a change in GMT Offset and/or
						a change in effective DSTR for a particular time zone
						6 = s to u - negative GMT Offset - add to standard time to calculate universal time
						7 = w to s - negative DST Offset - add to wall time to calculate standard time
						When no time is being saved, wall time = standard time
						8 = w to u - -(GMT Offset + DST Offset) - add to wall time to calculate universal time
						9 = sm1 -  negative jump in standard time associated with a change in GMT offset for
						a particular time zone
						10 = dm1 - negative jump in saved time associated with a change in effective DSTR for
						a particular time zone
						11 = wm1 - negative jump in wall time associated with a change in GMT Offset and/or
						a change in effective DSTR for a particular time zone
						*/
	uint8_t uOffInit; /* Bit field indicating status of initialization and error codes
					  0000h indicates initialized without error.
					  Other values indicate specification errors as follows, by bit number:
					  1 Invalid isNegative,							 2 Days > 1,
					  3 Hours > 23,									 4 Minutes > 59,
					  5 Seconds > 59,								 6 Hundredths > 99,
					  7 Type > 7,									 8 Invalid Operation - attempt to generate
					  an offset via an invalid operation
					  */

} UTCOffset;

typedef struct
{
	/* CalCoords with timezone and frame parameters added. 
	   This structure is useful for defining the boundaries of the time zone periods,
	   and as a precursor to a LocalDatetime. */
	CalCoords cc; /* The calendar coordinates - gigayear through attosecond, plus calendar.*/
	char timezone[maxTZNameLength]; /* The name of a timezone from the IANA time zone database.
									It is storage inefficient to use the name, but if it is
									replaced by an index, care must be taken to not change
									index values when updating the time zone database. */
	uint8_t frame; // Frame of reference.  0 = universal, 1 = standard and 2 = wall.
				   //   Wall is daylight savings time when a DSTR is in effect
				   //   and standard time when there is no effective DSTR.
	uint16_t lccInit  /* Bit field indicating status of initialization and error codes
					  0000h indicates initialized without error.
					  Other values indicate specification errors as per ccInit
					  */;
} LocalCalCoords;

typedef struct
{
	/* A local datetime expressed in terms of calendar coordinates without a tick count */
	CalCoords cc; /* The calendar coordinates - gigayear through attosecond, plus calendar.*/
	char timezone[maxTZNameLength]; /* The name of a timezone from the IANA time zone database.
						   It is storage inefficient to use the name, but if it is
						   replaced by an index, care must be taken to not change
						   index values when updating the time zone database. */
	uint8_t frame; // Frame of reference.  0 = universal, 1 = standard and 2 = wall.
						//   Wall is daylight savings time when a DSTR is in effect
						//   and standard time when there is no effective DSTR.
	uint8_t bOrA; /* Flag indicating whether a specified time is before a negative jump
				  in u-w or after.  The most common requirement for this specification is
				  for the 1 or 2 hours after the time is reset to standard time.  For
				  example, in the eastern time zone at 2 am on the first Sunday
				  in November, the time resets to 1 am.  Thus all of the times
				  between 1:00:00 and 1:59:59 are repeated, so, to avoid
				  ambiguity, a 'B' or 'A' needs to be specified.

				  The value may be either ' ', 'a', 'A', 'b', 'B', 0, 1 or 2.
				  ' ' or 0 indicates unspecified, which will result in an error if
					this is an amibuguous time.
				  'b', 'B', or 1 indicates before the jump.
				  'a', 'A', or 2 indicates after the jump.

				  This specification also could be required to disambiguate standard
				  times, in the case of a negative jump in u-s, which occurs when
				  there is a negative change in GMT Offset. */
	uint8_t ambigS; /* A flag indicating whether the LocalCalCoords is an ambiguous region when
						translated to a standard time. 
						Ambiguous regions can occur for standard times, when there is a negative
						change in GMT offset, or for wall times, when there is negative jump in
						GMT offset + S-W offset.  When a LocalCalCoords is translated to either
						a standard or wall frame of reference, a check must be made for ambiguous
						regions and the appropriate value of bOrA assigned.  This check can be
						complex, requiring much of the logic in computeOffsets.  To avoid these
						calculations upon every translation, ambigS and ambigW are computed 
						in the computeOffsets call and saved within the LocalCalCoords struct. */
	uint8_t ambigW; /* A flag indicating whether the LocalCalCoords is an ambiguous region when
						translated to a wall time. */
	uint8_t beforeFlag; /* For a LocalCalCoords created in the universal frame of reference,
							or for a LocalCalCoords created in an unambiguous region for
							a standard or wall frame of reference.
						    bOrA will be 0.  As such, there is no indication when the 
							LocalCalCoords is translated to standard or wall frame of
							reference, and there is ambiguity, whether bOrA is before
							or after.  This flag provides that indication. 
							One could conceive of a pathological case where a before value
							in standard frame of reference is actually an after value in
							wall frame of reference.  This case does not exist in practice
							and would likely cause problems in other sections of the algorithm.
							Thus, we will assume that whenever there is ambiguity in both the
							standard and wall frames of reference, both are before the jump
							or both are after the jump.*/
	uint8_t futureAdjust; /* Same concept as for UTC, except for time zone and/or rules.  Ie, if 
						  the datetime is within the last time zone period, provision must be made
						  for future changes to the GMTOffset or DSTR, which can occur when
						  new IANA timezone files are received.  If futureAdjust == 1, the
						  CalCoords values will be preserved for standard and wall frames of
						  reference.  This has the effect of adjusting the associated Universal time.
						  (futureAdjust is not relevant for CalCoords with a Universal
						  time frame.)  If futureAdjust == 2, the CalCoords will be adjusted to
						  preserve the associated Universal time.  
						  If the datetime is before the last time zone period, either when
						  created or on a subsequent call to adjustCalCoords after a change
						  to GMTOffset or DSTR, futureAdjust is set to 0. */
	uint32_t timezoneIndex; // The index of the time zone in the PeriodTimeZones array
	uint8_t periodIndex; /* The index of the period in the PeriodTimeZones array.
							GMT Offset can be retrieved: 
						     PeriodTimeZones[timezoneIndex].periods[periodIndex].gMTOffset 
							 Could have stored GMTOffset, but that is 64 bits, as opposed to 40 bits */
	uint8_t sToWMinutes; /* The DST (aka S-W) offset based on the time zone and DSTR in minutes.
								All DST offsets are in even number of minutes, with the largest
								in the IANA files being 2 hours.  Could have stored sToWOffset, but
								that is 64 bits. 
								S-W offset can be computed by calling: 
								createUTCOffset(0, 0, sToWMinutes/60, sToWMinutes % 60, ....) */
	uint16_t lccInit  /* Bit field indicating status of initialization and error codes
						0000h indicates initialized without error.
						Other values indicate specification errors as per ccInit
					   */;
} LocalCalCoordsDT;

typedef struct
{
	/* A relatively storage efficient local datetime, which levers the UTCDatetime
		struct and a packed representation of the time zone related parameters. */
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
					//	nanosecond and attosecond have been removed from the data structure because
					//	they are large and quickly calculable from tai.
	int8_t precision; /* 99=unspecified, 15=10^10 year, 14=10^9 year,...,
					  5=year, 4=month,
					  3=day, 2=hour, 1=minute, 0=second, -1=10^-1 second,
					  -n=10^-n second */
	int8_t uncertainty; /* As a multiplier of precision - 0 = unspecified
						For example, if precision=-3 and uncertainty=9, the time
						is known to +/- 9 milliseconds.
						If the precision is specified and the uncertainty is not,
						treat as if the uncertainty is 0.5, which is the minimum
						allowed.For precision >= 5 or precision <= -1,
						the maximum uncertainty is 9.  For precision=4,
						the maximum uncertainty is 11. For precision=3,
						the maximum uncertainty is 30.  For precision == 2,
						the maximum uncertainty is 23.  For precision == 1,
						the maximum uncertainty is 59.  For precision == 0,
						the maximum uncertainty is 59.  Uncertainties
						greater than the maximum result in decreased precision.
						A negative value of uncertainty indicates a number of
						units of the next higher precision.  Thus, a precision
						of 0 and an uncertainty of -8, represents 0.8 seconds.
						The minimum allowed uncertatinty is still 0.5 of the
						precision unit.*/
	TAITicks tai; // The tick count at the time of specification.
	uint32_t calendar;  /* 0 indicates Gregorian calendar with earliest last Julian date
					   1 indicates Julian calendar
					   2 indicates Swedish calendar
					   > 10000000 indicates encoded last Julian date yyyymmdd
					   e9 added to the above values indicates first Gregorian date
					   is 1 day earlier to deal with translations from wall time
					   to standard or universal time that take place on the first
					   Gregorian date and result in a time on the previous day
					   2 *e9 added to the above values indicates last Julian date
					   is 1 day later to deal with translations from wall time
					   to standard or universal time that take place on the last
					   Julian date and result in a time on the next day */
	uint32_t tzFields; /* Packed representation of time zone and related fields 
					    Bits 0 - 8 - timezone Index    Both of these fields need to be recomputed 
						Bits 9 - 15 - period Index      if the IANA time zone files are changed.
						Bits 16 - 17 - frame
						Bit 18 - ambigS
						Bit 19 - ambigW
						Bit 20 - beforeFlag
						Bits 21 - 22 - futureAdjust - Applies both to time zone and tai.
						  Ie, if
						  the datetime is within the last time zone period, provision must be made
						  for future changes to the GMTOffset or DSTR, which can occur when
						  new IANA timezone files are received.  If futureAdjust == 1, the
						  CalCoords values will be preserved for standard and wall frames of
						  reference.  This has the effect of adjusting the associated Universal time.
						  (futureAdjust is not relevant for CalCoords with a Universal
						  time frame.)  If futureAdjust == 2, the CalCoords will be adjusted to
						  preserve the associated Universal time.
						  If the datetime is before the last time zone period, either when
						  created or on a subsequent call to adjustCalCoords after a change
						  to GMTOffset or DSTR, no further adjustments are necessary to handle
						  IANA timezone file changes, but the value must be maintained to
						  handle leap second changes. 

						  FutureAdjust also must be used when calculating the tick count.
						  If the value is nonzero, the tick count or datetime elements
						  are to be adjusted, if necessary, when leap seconds are added.
						  This is only applicable when a datetime past the end of the leap
						  second table is specified.  The tick count that is calculated for
						  this datetime will reflect the number of leap seconds known at the
						  time of specification.  If another leap second is added to the table
						  before the specified datetime, the tick count and the datetime elements
						  will be inconsistent.  I.e., the tick count will reflect the time one
						  second earlier.  If futureAdjust is 1, the tick count will be
						  recalculated to be consistent with the specified datetime on subsequent
						  calls to calcTicks.  If futureAdjust is 2, the datetime elements will
						  be adjusted to be consistent with the tick count on a subsequent
						  call to adjustDatetime.  If the datetime is no longer after the
						  last leap second, no further adjustments are necessary for leap
						  seconds, but still could be required for IANA timezone file changes.
						  
						  In the case where the datetime is before the last time zone period and
						  before the last leap second, futureAdjust can be set to 0.  
						  
						  The ticks must be recalculated whenever there is a change to the
						  IANA time zone files, since the offset from universal time might
						  have changed.  
						Bits 23 - 29 - fracOffset - If this is a standard or wall time and the offset from universal
							  time contains a fractional second component, this is the number
							  of hundredths of a second in that offset.  This is required to
							  make the getNano accessor more efficient.  getNano uses the ticks,
							  which are based on UTC.  If the offset has fractional seconds,
							  they need to be added to the getNano result.  If this value were
							  not stored, it would have to be computed from data in the time
							  zone period.
						Bits 30 - 31 - reserved */
	uint8_t sToWMinutes; /* The DST (aka S-W) offset based on the time zone and DSTR in minutes.
						 All DST offsets are in even number of minutes, with the largest
						 in the IANA files being 2 hours.  Could have stored sToWOffset, but
						 that is 64 bits.
						 S-W offset can be computed by calling:
						 createUTCOffset(0, 0, sToWMinutes/60, sToWMinutes % 60, ....) */
	uint16_t ldtInit; /* Bit field indicating status of initialization and error codes
					  0000h indicates initialized without error.
					  Other values indicate specification errors as per ccInit
					  */
} LocalDatetime;

typedef struct
{
	// The unpacked representation of time zone fields
	//	This is the strcuture produced when the the tzFields element of a LocalDatetime is unpacked
	char timezone[maxTZNameLength];
	uint32_t timeZoneIndex; // Index of the timezone in the TimeZones array
	uint8_t period;
	uint8_t frame;
	uint8_t bOrA;
	uint8_t ambigS;
	uint8_t ambigW;
	uint8_t beforeFlag;
	uint8_t futureAdjust;
	uint8_t fracOffset;
} TimeZoneFields;

typedef struct
{
	// Defines an interval of time where the effective time series version and effective rule versions are unchanged
	//	A distinction is made between the applicable rule version and the effective version.  The period of applicability
	//  of a rule verion is defined in terms of whole years.  However, a rule only becomes effective at a certain point
	//	during its first year of applicability.  Similarly, a rule can remain effective for part of the year after its
	//	period of applicability.
	//
	//	The following formulas help define the meanings and sign conventions of the verious offsets:
	//
	//	  (1) UTC + GMTOffset = StandardTime
	//
	//	  (2) StandardTime + Saved = WallTime (Saved is 0, when the standard DSTR is effective.)
	//
	//    (3) WallTime at end of previous period + BeginJump = WallTime at beginning of this period
	//
	//    (4) UTC at end of previous period = UTC at beginning of this period 
	//
	//	 UTC is continuous, except for Oct 4 1582 at 23:59:59.999999999 999999999, which is before any DSTR have
	//	  been created, or before there are any changes in GMT Offset within a time zone.  This
	//	  does mean, however, that skipped days (which will not occur on even day boundaries)
	//	  must be considered when checking the validity of and performing operations on local times.
	//
	//	From (1), (2), and (4): BeginJump = GMTOffset (this period) - GMTOffset (last period) + 
	//								Saved (Beginning of this period) - Saved (End of last period)
	//
	uint32_t tZVersion; // Index of the effective time zone version in the TimeZoneVersions array
	uint8_t  nRuleVersions; // Number of applicable daylight savings time rule (DSTR) versions during the time period.
							  //    numRuleVersions == 0 indicates either a tZSaved offset is specified in the 
							  //	  time series version, instead of a DSTR, or that none of the specified
							  //		DSTR versions is effective during this time period.
	uint32_t ruleVersions[maxRuleVersionsPerSet]; // Indices of the applicable DSTR versions, if any,
										   //   in the rules array.
	UTCOffset gMTOffset; // The offset to add to UTC to calculate standard time throughout the period
	UTCOffset tZSaved; // The offset to add to standard time to calculate wall time throughout the period.
					   //   Only applicable when the time zone version specifies saved time, instead of a DSTR
	LocalCalCoords fromCoords; // The calendar coords of the wall time at the start of the period.
							   //   The fromCoords will be before the untilCoords of the previous period
							   //	when beginJump < 0 and after the untilCoords of the previous period
							   //   when beginJump > 0.  When beginJump == 0, fromCoords of this
							   //   period == untilCoords of the previous period.
	LocalCalCoords untilCoords; // The calendar coords of the wall time 1 tick after the end of the period
	UTCOffset beginSaved; // The offset to add to standard time to calculate wall time at the start of the period
	UTCOffset endSaved; // The offset to add to standard time to calculate wall time at the end of the period
	UTCOffset beginJump; // The offset to add to wall time at the end of the previous period to calculate
						 //   wall time at the beginning of this period.
						 // When there is a forward jump, an interval of invalid times is created.
						 //   Since the algorithm for determining the applicable time zone period given
						 //   a local wall (or standard) time compares the time to the until times,
						 //   the applicable period for the invalid times will be the period after the
						 //   jump.  The invalid period is beteween untilCoords of the previous period
						 //   and untilCoords of the previous period plus beginJump of this period.
						 // When there is a backward jump, there is a repeated time interval is created.
						 //   The first instance of this period occurs in the previous period, while
						 //   the second instance occurs in the current period. To disambiguate the
						 //   time, the local time must specify a before or after jump flag.  The algorithm
						 //   to determine the applicable time zone period much check whether the time
	                     //   is within the last beginJump (of the current period) time of the previous
						 //   period, instead of simply checking whether the time is >= untilCoords.
	uint8_t begRule;  // The index of the rule version in the ruleVersions array, which is
					  //  effective at the beginning of the period.  nRuleVersions if no rule is effective.
	uint8_t endRule;  // The index of the rule version in the ruleVersions array, which is
					  //  effective at the end of the period.  nRuleVersions if no rule is effective.
	TAITicks fromTAI;  // The TAI time in ticks corresponding to fromCoords
	TAITicks untilTAI; // The TAI time in ticks corresponding to untilCoords
} TZPeriod;

typedef struct
{
	// Time zone composed of periods
	uint8_t numNames; // Number of names for this time zone
	char names[maxNamesPerTimeZone][maxTZNameLength]; // Array of all nammes (i.e., time zone name plus all links)
	uint8_t numPeriods; // Number of periods for this time zone
	TZPeriod periods[maxTimeZonePeriods]; // Array of time zone periods
} PTimeZone;

typedef struct
{
	// Period segment
	//  A period segment is a time span within a period where the time saved
	//	 is unchanged.  These are used to derive LocalDatetimes from LocalCalCoords.
	//
	//   The from and to coords define a semi-open interval - i.e. fromCoords <= lcc < untilCoords
	LocalCalCoords fromCoords; // The beginning of the segment in wall frame of reference
	LocalCalCoords untilCoords; // The end of the segment in wall frame of reference
	UTCOffset sToW; // The s-w offset for this segment
} PeriodSegment;

typedef struct
{
	// Bracketing segments
	//  An ordered list of period segements that bracket a LocalCalCoords.
	//  Generally contains all of the segments for the year of the lcc,
	//  plus the last segment for the previous year.  Only exception is
	//  when the lcc is in the from year of a period.
	//
	uint8_t numSegments;
	PeriodSegment segments[maxPeriodSegments];
} BracketingSegments;

// Error flags
enum UTCOffErrors {
	OffInvalidIsNegative = 0x0001,
	OffDaysGtMax = 0x0002,
	OffHoursGtMax = 0x0004,
	OffMinutesGtMax = 0x0008,
	OffSecondsGtMax = 0x0010,
	OffHundredthsGtMax = 0x0020,
	OffInvalidType = 0x0040,
	OffInvalidOperation = 0x0080
};

static const char* UTCOffErrorStrings[] =
{
	"Invalid value of isNegative.  Only 0 and 1 are allowed.",
	"Days greater than the maximum.  Only 0 and 1 are allowed.",
	"Hours are greater than 23",
	"Minutes are greater than 59",
	"Seconds are greater than 59",
	"Hundredths of a second are greater than 99",
	"Invalid offset type.  Value must be between 0 and 7, inclusive.",
	"Invalid operation attempted"
};

static const uint8_t numOffsetTypes = 12;
static const char* OffsetTypes[] = { "u-s", "s-w", "u-w", "sp1", "dp1", "wp1",
"s-u", "w-s", "w-u", "sm1", "dm1", "wm1" };
static const char FramesOfReference[3] = { 'u', 's', 'w' };

static PTimeZone PeriodTimeZones[numTimeZones];

// Masks to set and get time zone fileds from packed representation
static const uint32_t tzMask =    0x000001FF; // Lowest 9 bits
static const uint32_t perMask =   0x0000FE00; // Bits 9 - 15
static const uint32_t frameMask = 0x00030000; // Bits 16 - 17
static const uint32_t ambigSMask = 0x00040000; // Bit 18
static const uint32_t ambigWMask = 0x00080000; // Bit 19
static const uint32_t bFlagMask = 0x00100000; // Bit 20
static const uint32_t faMask =    0x00600000; // Bits 21 - 22
static const uint32_t fracMask =  0x3F800000; // Bits 23 - 29

// Time Zone field retrieval functions
uint32_t getTimeZone(uint32_t tZField)
{
	// Lowest 9 bits
	return (tZField & tzMask);
}

uint8_t getTZPeriod(uint32_t tZField)
{
	// Bits 9 - 15
	return (uint8_t)((tZField & perMask) >> 9);
}

uint8_t getFrame(uint32_t tZField)
{
	// Bits 16 - 17
	return (uint8_t)((tZField & frameMask) >> 16);
}

uint8_t getAmbigS(uint32_t tZField)
{
	// Bit 18
	return (uint8_t)((tZField & ambigSMask) >> 18);
}

uint8_t getAmbigW(uint32_t tZField)
{
	// Bit 19
	return (uint8_t)((tZField & ambigWMask) >> 19);
}

uint8_t getBeforeFlag(uint32_t tZField)
{
	// Bit 20
	return (uint8_t)((tZField & bFlagMask) >> 20);
}

uint8_t getBOrA(uint32_t tZField)
{
	// Derive from frame, ambigS, ambigW and beforeFlag
	uint8_t frame;
	frame = getFrame(tZField);
	if (frame == 0) return 0;
	else if (frame == 1) return (getAmbigS(tZField) == 1 ? 2 - getBeforeFlag(tZField) : 0);
	else return (getAmbigW(tZField) == 1 ? 2 - getBeforeFlag(tZField) : 0);
}

uint8_t getFutureAdjust(uint32_t tZField)
{
	// Bits 21 - 22
	return (uint8_t)((tZField & faMask) >> 21);
}

uint8_t getFracOffset(uint32_t tZField)
{
	// Bits 23 - 29
	return (uint8_t)((tZField & fracMask) >> 23);
}

uint32_t setTimeZone(uint32_t tZField, uint32_t tZIndex)
{
	// Set the time zone into the lowest 9 bits
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value 
	return ((tZField & ~tzMask) | tZIndex);
}

uint32_t setTZPeriod(uint32_t tZField, uint8_t tZPeriod)
{
	// Set the time zone period into bits 9 - 15
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~perMask) | (((uint32_t)tZPeriod) << 9));
}

uint32_t setFrame(uint32_t tZField, uint8_t frame)
{
	// Set the frame of reference flag into bits 16 - 17
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~frameMask) | (((uint32_t)frame) << 16));
}

uint32_t setAmbigS(uint32_t tZField, uint8_t ambigS)
{
	// Set the standard frame of reference ambiguity flag into bit 20
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~ambigSMask) | (((uint32_t)ambigS) << 18));
}

uint32_t setAmbigW(uint32_t tZField, uint8_t ambigW)
{
	// Set the standard frame of reference ambiguity flag into bit 20
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~ambigWMask) | (((uint32_t)ambigW) << 19));
}

uint32_t setBeforeFlag(uint32_t tZField, uint8_t bFlag)
{
	// Set the standard frame of reference ambiguity flag into bit 20
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~bFlagMask) | (((uint32_t)bFlag) << 20));
}

uint32_t setFutureAdjust(uint32_t tZField, uint8_t futureAdjust)
{
	// Set the future adjust flag into bits 21 - 22
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~faMask) | (((uint32_t)futureAdjust) << 21));
}

uint32_t setFracOffset(uint32_t tZField, uint8_t fracOffset)
{
	// Set the number of hundredths of a second of offset into bits 24 - 30
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((tZField & ~fracMask) | (((uint32_t)fracOffset) << 23));
}

uint32_t create32BitMask(uint8_t numberOfBits, uint8_t startingBit)
{
	//  Create a 32 bit mask of length numberOfBits, starting in bit startingBit
	//	  Don't check values to save times
	static const uint32_t mask = 0xFFFFFFFF;
	return (mask >> (32 - numberOfBits)) << startingBit;
}

uint8_t getPacked8BitValue(uint32_t packedValue, uint8_t numberOfBits, uint8_t startingBit)
{
	// Retrieve an 8 bit value from a 32-bit packed field
	//	Create the required mask
	uint32_t mask = create32BitMask(numberOfBits, startingBit);
	// Get the value
	return (uint8_t)((packedValue & mask) >> startingBit);
}

uint32_t getPacked32BitValue(uint32_t packedValue, uint8_t numberOfBits, uint8_t startingBit)
{
	// Retrieve a value between 9 and 32 bits long from a 32-bit packed field
	//	Create the required mask
	uint32_t mask = create32BitMask(numberOfBits, startingBit);
	// Get the value
	return (packedValue & mask) >> startingBit;
}

uint32_t setPacked8BitValue(uint32_t packedValue, uint8_t numberOfBits, uint8_t startingBit, uint8_t value)
{
	// Update a 32-bit packed field with an 8-bit value
	//	Create the required mask
	uint32_t mask = create32BitMask(numberOfBits, startingBit);
	// Set the value
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((packedValue & ~mask) | (((uint32_t)value) << startingBit));
}

uint32_t setPacked32BitValue(uint32_t packedValue, uint8_t numberOfBits, uint8_t startingBit, uint32_t value)
{
	// Update a 32-bit packed field with a value between 9 and 32-bits
	//	Create the required mask
	uint32_t mask = create32BitMask(numberOfBits, startingBit);
	// Set the value
	//  Clear the relevant bits by &ing with the Ones complement of the mask
	//	  then or with the value left shifted the appropriate number of digits
	return ((packedValue & ~mask) | (value << startingBit));
}

uint32_t packTimeZoneFieldsTZIndex(uint32_t timeZoneIndex, uint8_t period, uint8_t frame, 
	uint8_t ambigS, uint8_t ambigW, uint8_t beforeFlag, uint8_t futureAdjust, uint8_t fracOffset)
{
	// Pack the time zone fields into a 32-bit unsigned int

	// Initialize tzFields
	uint32_t tzFields = 0;

	// Pack the values
	tzFields = setPacked32BitValue(tzFields, 9, 0, timeZoneIndex);
	tzFields = setPacked8BitValue(tzFields, 7, 9, period);
	tzFields = setPacked8BitValue(tzFields, 2, 16, frame);
	tzFields = setPacked8BitValue(tzFields, 1, 18, ambigS);
	tzFields = setPacked8BitValue(tzFields, 1, 19, ambigW);
	tzFields = setPacked8BitValue(tzFields, 1, 20, beforeFlag);
	tzFields = setPacked8BitValue(tzFields, 2, 21, futureAdjust);
	tzFields = setPacked8BitValue(tzFields, 7, 23, fracOffset);

	return tzFields;
}

uint32_t packTimeZoneFields(char timezone[], uint8_t period, uint8_t frame, uint8_t ambigS,
	uint8_t ambigW, uint8_t beforeFlag, uint8_t futureAdjust, uint8_t fracOffset)
{
	// Pack the time zone fields into a 32-bit unsigned int given the time zone or link name
	uint32_t timeZoneIndex, tzFields = 0, i, j;
	//
	// Check the validity of the time zone
	//	Allow a blank time zone
	if (strnlen_s(timezone, maxTZNameLength) == 0 ||
		(strnlen_s(timezone, maxTZNameLength) == 1 && timezone[0] == ' '))
	{
		// Blank time zone
		timeZoneIndex = 511; // Set to maximum 9 bit value
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
					// Save the time zone name index
					timeZoneIndex = i;
					goto end;
				}
			}
		}
		end:
		// Check whether a match was found
		if (i == numTimeZones)
		{
			// No match found
			timeZoneIndex = 511; // Set to maximum 9 bit value
		}
	}
	return packTimeZoneFieldsTZIndex(timeZoneIndex, period, frame, ambigS, ambigW, beforeFlag, futureAdjust, fracOffset);
}

TimeZoneFields unpackTimeZoneFieldsTZIndex(uint32_t tzFields)
{
	// Unpack tzFields, returning the time zone index
	TimeZoneFields tzf;

	// Unpack the values
	tzf.timeZoneIndex = getPacked32BitValue(tzFields, 9, 0);
	tzf.period = getPacked8BitValue(tzFields, 7, 9);
	tzf.frame = getPacked8BitValue(tzFields, 2, 16);
	tzf.bOrA = getBOrA(tzFields);  // Derived from frame, ambigS, ambigW and beforeFlag
	tzf.ambigS = getPacked8BitValue(tzFields, 1, 18);
	tzf.ambigW = getPacked8BitValue(tzFields, 1, 19);
	tzf.beforeFlag = getPacked8BitValue(tzFields, 1, 20);
	tzf.futureAdjust = getPacked8BitValue(tzFields, 2, 21);
	tzf.fracOffset = getPacked8BitValue(tzFields, 7, 23);
	tzf.timezone[0] = '\0';

	return tzf;
}

TimeZoneFields unpackTimeZoneFields(uint32_t tzFields)
{
	// Unpack tzFields, returning the time zone index and the time zone name
	TimeZoneFields tzf = unpackTimeZoneFieldsTZIndex(tzFields);

	// Copy the time zone name from the TimeZones array
	strcpy_s(tzf.timezone, sizeof(tzf.timezone), TimeZones[tzf.timeZoneIndex][0]);
	return tzf;
}

// Create UTCOffset
DLLExport UTCOffset createUTCOffset(uint8_t isNegative, uint8_t days, uint8_t hours, uint8_t minutes,
									uint8_t seconds, uint8_t hundredths, char offsetType[]);

// Check the validity of the elements of a UTC Offset
DLLExport uint8_t checkUTCOffsetElements(uint8_t isNegative, uint8_t days, uint8_t hours, uint8_t minutes,
	uint8_t seconds, uint8_t hundredths, uint8_t offsetIndex);

// Report errors encountered during the contruction of a UTCOffset based on init bit flag
DLLExport char * reportUTCOffsetConstructionError(uint8_t init, char buffer[], int lenBuffer, int failTest);

// UTCOffset comparison
DLLExport int compareUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isGreaterUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isLessUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isNotEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isGreaterOrEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport int isLessOrEqualUTCOffsets(UTCOffset utc1, UTCOffset utc2);

// UTCOffset arithmetic
DLLExport UTCOffset addUTCOffsets(UTCOffset utc1, UTCOffset utc2);
DLLExport UTCOffset negateUTCOffset(UTCOffset utc);
DLLExport UTCOffset absValueUTCOffset(UTCOffset utc);
DLLExport UTCOffset subtractUTCOffsets(UTCOffset utc1, UTCOffset utc2);

// Format UTCOffset
DLLExport const char * asStringUTCOffset(UTCOffset utc);

// Create LocalCalCoords
DLLExport LocalCalCoords createLocalCalCoords(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar, const char timezone[], uint8_t frame);
DLLExport LocalCalCoords createLocalCalCoordsFromCalCoords(CalCoords cc, const char timezone[],
	uint8_t frame);
DLLExport LocalCalCoords createLocalCalCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame);
DLLExport LocalCalCoords createLocalCalCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame);

// LocalCalCoords comparison
DLLExport int compareLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isGreaterLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isLessLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isNotEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isGreaterOrEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);
DLLExport int isLessOrEqualLocalCalCoords(LocalCalCoords lcc1, LocalCalCoords lcc2);

// Format LocalCalCoords
DLLExport const char * asStringLocalCalCoords(LocalCalCoords lcc, char stringCal[]);

// Apply UTCOffset to LocalCalCoords
DLLExport LocalCalCoords offsetLocalCalCoords_old(LocalCalCoords lcc, UTCOffset off);
DLLExport LocalCalCoords offsetLocalCalCoords(LocalCalCoords lcc, UTCOffset off);

// Create LocalCalCoordsDT
DLLExport LocalCalCoordsDT createLocalCalCoordsDT(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust);
DLLExport LocalCalCoordsDT createLocalCalCoordsDTFromCalCoords(CalCoords cc, const char timezone[],
	uint8_t frame, uint8_t bOrA, uint8_t futureAdjust);
DLLExport LocalCalCoordsDT createLocalCalCoordsDTFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust);
DLLExport LocalCalCoordsDT createLocalCalCoordsDTFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
	uint32_t attosecond, uint32_t calendar, const char timezone[], uint8_t frame, uint8_t bOrA, uint8_t futureAdjust);

// LocalCalCoordsDT comparison
DLLExport int compareLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isGreaterLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isLessLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isNotEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isGreaterOrEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);
DLLExport int isLessOrEqualLocalCalCoordsDT(LocalCalCoordsDT lcc1, LocalCalCoordsDT lcc2);

// Subtract LocalCalCoordsDTs
DLLExport TAIRelDatetime diffLocalCalCoordsDTs(LocalCalCoordsDT u1, LocalCalCoordsDT u2);

// Add TAIRelDatetime to LocalCalCoordsDT
DLLExport LocalCalCoordsDT addRelToLocalCalCoordsDT(LocalCalCoordsDT u1, TAIRelDatetime re1, uint8_t futureAdjust);

// Subtract TAIRelDatetime from LocalCalCoordsDT
DLLExport LocalCalCoordsDT subtractRelFromLocalCalCoordsDT(LocalCalCoordsDT u1, TAIRelDatetime re1, uint8_t futureAdjust);

// Format LocalCalCoordsDT
DLLExport const char * asStringLocalCalCoordsDT(LocalCalCoordsDT lcc, char stringCal[]);


// Apply UTCOffset to LocalCalCoordsDT
DLLExport LocalCalCoordsDT offsetLocalCalCoordsDT_old(LocalCalCoordsDT lcc, UTCOffset off);
DLLExport LocalCalCoordsDT offsetLocalCalCoordsDT(LocalCalCoordsDT lcc, UTCOffset off);

// LocalCalCoordsDT / LocalCalCoords comparison.  
//	Necessary for functions used to initialize LocalCalCoordsDT
DLLExport int compareLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isGreaterLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isLessLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isNotEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isGreaterOrEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);
DLLExport int isLessOrEqualLDTLCC(LocalCalCoordsDT lcc1, LocalCalCoords lcc2);

// Create the time zone periods from the rules and timezones arrays
int createTimeZonePeriods(void);

// Output the time zone periods
DLLExport int outputTimeZonePeriods(FILE * tzp);

// Find the index of a rule in the TZRule array
DLLExport uint32_t findRuleIndex(char ruleName[]);

// Find the index of the applicable rule version set in the TZRuleVersionSets array for a specified year
DLLExport uint32_t findRuleSetIndex(char ruleName[], uint32_t year);

// Find the last year a rule is applicable before a specified until year
DLLExport uint32_t lastApplicableYearBeforeUntil(char ruleName[], uint32_t untilYear);

// Set the fields for a time zone period with no effective daylight savings time rules
DLLExport TZPeriod setPeriodFieldsNoDSTR(TZPeriod p, LocalCalCoords untilStandard);

// Set the beginning fields for a time zone period when there is a previous period
DLLExport TZPeriod setPeriodBeginFieldsWithPrevPeriod(TZPeriod p, UTCOffset beginSaved, TZPeriod pPrev);

// Set the beginning fields for the first period of a time zone version
DLLExport TZPeriod setPeriodBeginFieldsWithoutPrevPeriod(TZPeriod p, UTCOffset beginSaved);

// Retrieve until year from LocalCalCoords
DLLExport int32_t getUntilYear(LocalCalCoords lcc, UTCOffset beginJump);

// Create a LocalCalCoords representing a rule 'At' datetime for a specified year
DLLExport LocalCalCoords createAtLocalCalCoords(TZRule rule, char timezone[], uint32_t year, 
	uint8_t requiredFrame, UTCOffset gMTOffset, UTCOffset sToW);

// Compute p.begRule and p.begSaved at a specified LocalCalCoords without using the PeriodTimeZones array
DLLExport TZPeriod pBegNoPeriodTZArray(LocalCalCoords lccPrevTZVerUntil, TimeZone tzver, uint32_t ruleVersionIndex,
	UTCOffset prevGMTOffset, UTCOffset prevEndSaved, TZPeriod p);

// Create the bracketing segments for a specified year
DLLExport BracketingSegments createBracketingSegments(TZPeriod period, uint32_t year);

// Compute nanoSecond field of a LocalDatetime
DLLExport uint32_t getLDTNanoSecond(LocalDatetime ldt);

// Return the maximum timezone name length.  Used by Julia to dimension a vector in LocalCalCoords struct
DLLExport int getMaxTZNameLength(void);

// Compute time zone related fields for a LocalCalCoordsDT
DLLExport LocalCalCoordsDT computeTZFields(LocalCalCoordsDT lcc);

// Translate a LocalCalCoordsDT from wall or standard frame of reference to universal frame of reference
//****** Deprecate this one
DLLExport LocalCalCoordsDT translateToUniversal(LocalCalCoordsDT lcc);

// Translate a LocalCalCoordsDT from one frame of reference to another within the same time zone
DLLExport LocalCalCoordsDT translateFrame(LocalCalCoordsDT lcc, uint8_t frame);

// Translate a LocalCalCoordsDT from univeral frame of reference to a specified time zone and frame of reference
DLLExport LocalCalCoordsDT translateFromUniversal(LocalCalCoordsDT lcc, char timezone[], uint8_t frame);

// Translate a LocalCalCoordsDT to a specified time zone and frame of reference
DLLExport LocalCalCoordsDT translateLocalCalCoordsDT(LocalCalCoordsDT lcc, char timezone[], uint8_t frame);

// Create a LocalDatetime from a LocalCalCoordsDT
DLLExport LocalDatetime createLocalDatetimeFromLocalCalCoordsDT(LocalCalCoordsDT cc, int8_t precision,
	int8_t uncertainty);

// Create a UTCDatetime from a LocalCalCoordsDT
DLLExport UTCDatetime createUTCDatetimeFromLocalCalCoordsDT(LocalCalCoordsDT cc, int8_t precision,
	int8_t uncertainty);

// Determine the untilCoords associated with the last Julian day for a specified time zone and frame of reference
DLLExport LocalCalCoordsDT julianUntilCoords(uint32_t calendar, uint32_t timezoneIndex, uint8_t frame);

// Determine the fromCoords associated with the first Gregorian day for a specified time zone and frame of reference
DLLExport LocalCalCoordsDT gregorianFromCoords(uint32_t calendar, uint32_t timezoneIndex, uint8_t frame);

