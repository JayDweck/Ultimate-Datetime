#define Ticks64

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h> // Contains declarations and defintions for fixed width integers
#include <math.h>
#include <float.h>

// Undefine min and max, as they should not be defined in stdlib.h
#undef min
#undef max

#define e9 1000000000
#define e18 1000000000000000000
#define Lower16(A) ((A) & 0177777L)
#define Upper16(A) (((A)>>16) & 0177777L)

// Make portable across Windows and Linux
#if (defined (WIN32) || defined (_WIN32) || defined (__WIN32)) && !defined (__CYGWIN__)
	// Windows
	#define DLLExport __declspec( dllexport )
#else
	// Linux or OS X
	#define DLLExport __attribute__ ((visibility ("default")))
	#define sprintf_s snprintf
    #define fopen_s(pFile, filename, mode) ((*(pFile)) = fopen((filename),(mode)))==NULL
#endif

typedef struct
{
	/* Structure to store a Precision / Uncertainty pair to enable the
		values to be returned from a function call */
	int8_t precision;
	int8_t uncertainty;
} PrecisionUncertainty;

typedef struct
{
	/* Structure to store the result of the multiplication of 2 @ unsigned 32 bit integers */
	uint32_t low;
	uint32_t high;
} UInt32MultResult;

typedef struct
{
	/* Double value with precision and uncertainty */
	double value;
	double precision; // Smallest significant digit.  Generally will be a power of 10.
					  // Bounded based on a limit of 16 significant digits,
					  //	the limit for a double precision.
	double uncertainty; // Multiplier on precision
} UncertainDouble;

typedef struct
{
	/* Tick count from epoch using 2 @ 64-bit integers. */
	uint64_t seconds;
	uint64_t attoseconds;
} TAITicks64;

typedef struct
{
	/* Tick count from epoch using 4 @ 32-bit integers. */
	uint32_t gigaseconds;
	uint32_t seconds;
	uint32_t nanoseconds;
	uint32_t attoseconds;
} TAITicks32;

typedef struct
{
	/* Relative ticks using 2 @ 64-bit integers. */
	uint64_t seconds;
	uint64_t attoseconds;
} TAIRelTicks64;

typedef struct
{
	/* Relative ticks using 4 @ 32-bit integers. */
	uint32_t gigaseconds;
	uint32_t seconds;
	uint32_t nanoseconds;
	uint32_t attoseconds;
} TAIRelTicks32;

#ifdef Ticks64
	typedef TAITicks64 TAITicks;
	typedef TAIRelTicks64 TAIRelTicks;
	const TAITicks64 alignmentInstant = {436320211189032010, 0};  // 1 Jan 1972 00:00:00 UTC = 1 Jan 1972 00:00:10 TAI
	const TAITicks64 julianEpoch =      {436320000000000000, 0};  // 1 Jan 4713 BCE 12:00:00 UTC
#else
	typedef TAITicks32 TAITicks;
	typedef TAIRelTicks32 TAIRelTicks;
	const TAITicks32 alignmentInstant = {436320211, 189032010, 0, 0}; // 1 Jan 1972 00:00:00 UTC = 1 Jan 1972 00:00:10 TAI
	const TAITicks32 julianEpoch =      {436320000, 0, 0, 0}; // 1 Jan 4713 BCE 12:00:00 UTC
#endif


typedef struct
{
	/* Gigaday, day, hour, minute, second representation of 
		relative ticks.  Each element is measured in 
		TAI space, using a standard number of SI seconds,
		e.g., day is 86400 seconds, hour is 3600 seconds. */
	uint16_t gigadays;
	uint32_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	int8_t precision; /* 99=unspecified, 16=10^13 day, 15=10^12 day,...,
					  3=day, 2=hour, 1=minute, 0=second, -1=10^-1 second,
					  -n=10^-n second */
	int8_t uncertainty; /* As a multiplier of precision - 0 = unspecified
							For example, if precision=-3 and uncertainty=9, the time
							is known to +/- 9 milliseconds. 
							If the precision is specified and the uncertainty is not,
							treat as if the uncertainty is 0.5, which is the minimum
							allowed.  For precision >=3 or precision <= -1, 
							the maximum uncertainty is 9.  For precision == 2,
							the maximum uncertainty is 23.  For precision == 1,
							the maximum uncertainty is 59.  For precision == 0,
							the maximum uncertainty is 59.  Uncertainties 
							greater than the maximum result in decreased precision. 
							A negative value of uncertainty indicates a number of
							units of the next higher precision.  Thus, a precision
							of 0 and an uncertainty of -8, represents 0.8 seconds.
							The minimum allowed uncertatinty is still 0.5 of the
							precision unit.*/
	TAIRelTicks relTicks;
	uint8_t relInit; /* Bit field indicating status of initialization and error codes
						00h indicates initialized without error.
						Other values indicate specification errors as follows, by bit number:
						1 Relative datetime greater than max,			 2 Days >= 1e9,  
						3 Hours > 23					,				 4 Minutes > 59,
						5 Seconds > 59			,						 6 Nano >= 1e9,
						7 Atto >= 1e9,									 8 Invalid Precision */
} TAIRelDatetime;

/* The following constants are based on the Big Bang Epoch
	As of 2014, the age of the Universe is estimated to be 4.354 +/- 0.012 x 10^17 seconds.

	In order to precisely establish the epoch for our TAI data type, we need to select an age
	of the universe within the known range that is convenient for data analysis.  We elected
	to choose a time interval prior to the Julian day epoch where the trailing digits of the 
	number of days and the number of seconds computed for a date coordinate are the same as 
	if the Julian day epoch were used.

	The age of the universe in 86400 second days is 5.039 x 10^12 within an uncertainly of .014 x 10^12.
	This corresponds with an age in seconds of 4.354 x 10^17 with an uncertainty of .012 x 10^17.
	We observe that multiplying the number of seconds by a multiple of 5 produces a result with
	the minimum number of digits, 3.  If we choose 5.05 x 10^12 days as the time interval, we are
	within the range of uncertainty.  The number of seconds is 4.3632 x 10^17.

	The current Julian day count is on the order of 2,000,000, so the resulting day count with 
	the selected epoch is of the form 5,050,002,xxx,xxx.  Similarly, the number of TAI seconds
	corresponding to the current Julian day count is on the order of 200,000,000,000 so the 
	resulting number of seconds with the selected epoch is 436,320,2xx,xxx,xxx,xxx.  In this way,
	the Julian day count is the trailing 7 digits, while the second count is the trailing 12 digits.

	Given the selected epoch, the Big Bang occurred at 12h Dec 25 13,826,151,189 BCE.  Given that
	October 4, 1582, was a Thursday, we can compute that the Big Bang occurred on a Wednesday.  The UTC 
	instant, 1 Jan 1972 00:00:00 occurs at a day count of 5,050,002,444,317.5 and a TAI second count
	of 436,320,211,189,032,010.0.
*/

/* A calendar is a coordinate system to map terestial notions, e.g., the rotation of the earth on
	its axis and the revolution of the earth around the sun, to the progression of time.  The
	progression of time is marked by fixed length seconds, aka SI seconds, which were defined in terms of
	vibrations of the cesium atom starting 1 Jan 1958 00:00:00 UT1.  This progression of time is known as
	International Atomic Time, aka TAI.

	The SI second was determined to match the ephemeris second as closely as possible.  The
	ephemeris second is the duration of time that, when used as the independent variable in the 
	laws of motion that govern the movement of the planets and moons in the solar system, causes
	the laws of motion to accurately predict the observed positions of solar system bodies. Within
	the limits of observing accuracy, ephemeris seconds are of constant length, as are atomic 
	seconds.

	In 1967, the SI second was officially redefined in terms of the frequency that had
	provisionally been used in TAI since 1958. The SI second was defined as 9,192,631,770 periods
	of the radiation emitted by a cesium-133 atom in the transition between the two hyperfine levels
	of its ground state. That value agreed to 1 part in 10^10 with the astronomical (ephemeris) second
	then in use, which is as close an agreeement as could be obtained using an integer number of
	periods.  It was also close to 1/86400 of the mean solar day as averaged between 1750 and 1892.
 
	Naturally, there is also a desire for the length of a terestial day, i.e., the time it
	takes for the earth to make one rotation about its axis, to be 86400 seconds.  However, as the
	rotational speed of the earth has varied parabolically, decreasing from ancient times until
	the 19th century and increasing since then at a rate corresponding to an increase in the solar
	day length of 1.7 ms per century, the terestial day cannot be fixed at 86400 seconds.  The
	terestial day was closest to 86400 SI seconds in the mid 19th century.
	Universal Time, UT, is a time standard based on the rotation of the earth.  It is a modern
	continuation of Greenwich Mean Time (GMT).  There are multiple versions of UT.  UT1 is the
	principal form of UT.  It is based on mean solar time at 0 degrees longitude.  Since precise
	measurements of the sun are difficult, it is computed from distant quasars, laser ranging
	the moon and the determination of GPS satellite orbits.  UT1R and UT2 are smoothed versions
	of UT1.

	UTC, coordinated universal time, is an atomic timescale that approximated UT2 whan it was
	created in 1961, but was modified to track UT1 in 1972.  The TAI instant 1 January 1961 
	00:00:01.422818 exactly was identified as UTC instant 1 January 1961 00:00:00.000000 exactly
	(recognizing the drift that had occurred since 1958),
	and UTC ticked exactly one second for every 1.000000015 s of TAI.  Time steps occurred every
	few months thereafter and frequency changes were made at the end of each year.  This approach
	enabled UTC to approximate UT2 within around 0.1s, but was very cumbersome.

	In 1967, the SI second was officially redefined in terms of the frequency that had
	provisionally been used in TAI since 1958. The SI second was defined as 9,192,631,770 periods
	of the radiation emitted by a cesium-133 atom in the transition between the two hyperfine levels
	of its ground state. That value agreed to 1 part in 10^10 with the astronomical (ephemeris) second
	then in use, which is as close an agreeement as could be obtained using and integer number of
	periods.  It was also close to 1/86400 of the mean solar day as averaged between 1750 and 1892.

	It was soon recognized that having two types of second with different lengths, namely the UTC
	second and the SI second used in TAI, was a bad idea. It was thought that it would be better
	for time signals to maintain a consistent frequency, and that that frequency should match the
	SI second. Thus it would be necessary to rely on time steps alone to maintain the approximation
	of UT.  There was also dissatisfaction with the frequent jumps in UTC.

	In 1968, Louis Essen, the inventor of the cesium atomic clock, and G. M. R. Winkler both 
	independently proposed that steps should be of 1 s only. This system was eventually approved, 
	along with the idea of maintaining the UTC second equal to the TAI second. At the end of 1971,
	there was a final irregular jump of exactly 0.107758 TAI seconds, so that 
	1 January 1972 00:00:00 UTC was 1 January 1972 00:00:10 TAI exactly, making the difference 
	between UTC and TAI an integer number of seconds. At the same time, the tick rate of UTC was 
	changed to exactly match TAI. UTC also started to track UT1 rather than UT2.

	The scheduling of leap seconds was initially delegated to the Bureau International de l'Heure 
	(BIH), but passed to the International Earth Rotation and Reference Systems Service (IERS) on
	January 1, 1988. IERS usually decides to apply a leap second whenever the difference between UTC
	and UT1 approaches 0.6 s, in order to keep the difference between UTC and UT1 from exceeding 
	0.9 s.  The first leap second occurred on 30 June 1972. Since then, leap seconds have occurred on 
	average about once every 19 months, always on 30 June or 31 December. As of January 2015, 
	there have been 25 leap seconds in total, all positive, putting UTC 35 seconds behind TAI.
	(The leap second system provides for the possibility of negative leap seconds, should
	the earth's rotation speed up, but this phenomenon has yet to be observed.)
	Another leap second has been announced for June 30, 2015.

	As the Earth's rotation continues to slow, positive leap seconds are required more frequently. 
	The long-term rate of change of LOD (length of day) is approximately +1.7 ms per century. At 
	the end of the 21st century, LOD will be roughly 86,400.004 s, requiring leap seconds every 
	250 days. Over several centuries, the frequency of leap seconds will become problematic.

	Some time in the 22nd century, two leap seconds will be required every year. The current use 
	of only the leap second opportunities in June and December will be insufficient, and the March
	and September options will have to be used. In the 25th century, four leap seconds will be 
	required every year, so the current quarterly options will be insufficient. Thereafter there 
	will need to be the possibility of leap seconds at the end of any month. In about two thousand
	years, even that will be insufficient, and there will have to be leap seconds that are not at 
	the end of a month.

	In a few tens of thousands of years (the timing is uncertain), LOD will exceed 86,401 s, causing
	the current form of UTC to break down due to requiring more than one leap second per day. It 
	would be possible to then continue with double leaps, but this becomes increasingly untenable.

	Both the one-leap-second-per-month and one-leap-second-per-day milestones are considered 
	(by different theorists) to mark the theoretical limit of the applicability of UTC. The 
	actual number of leap seconds to keep track of time would become unwieldy by current standards
	well before these, but presumably if UTC were to continue then horological systems would be 
	redesigned to cope with regular leap seconds much better than current systems do.

	There is a proposal to redefine UTC and abolish leap seconds, such that sundials would slowly 
	get further out of sync with civil time. The resulting gradual shift of the sun's movements 
	relative to civil time is analogous to the shift of seasons relative to the yearly calendar
	that results from the calendar year not precisely matching the tropical year length. 
	This would be a major practical change in civil timekeeping, but would take effect slowly over
	several centuries. UTC (and TAI) would be more and more ahead of UT; it would coincide with 
	local mean time along a meridian drifting slowly eastward (reaching Paris and beyond).
	Thus, the time system would lose its fixed connection to the geographic coordinates based on 
	the IERS meridian. The difference between UTC and UT could reach 0.5 hour after the year 2600
	and 6.5 hours around 4600.

	Ultimate Datetime Approach
	==========================

	Our approach begins by extending TAI back from 1 Jan 1958 00:00:00 UT1, the point at
	which the scales were defined to be synchronized, to the Big Bang, which for computational
	convenience, we define to be 5.05e12 days before the Julian epoch (1 Jan 4713 BCE 12:00:00.0).
	Tick 0 occurs at the instant of the Big Bang.  A tick occurs every 1^-18 SAI second after the
	Big Bang.

	We also extend UTC from its inception 1 Jan 1961 00:00:00 back to the Big Bang.  The initial
	extension back to 1 Jan 1958 00:00:00 is done by using UT1 in place of UTC.  We apply the
	Gregorian calendar convention for leap years back to 15 Oct 1582, then the Julian convention
	from 4 Oct 1582, back to the Big Bang.  (We allow the specificaton of Julian style dates, perpetually.
	Greece was the last European country to switch from the Julian to the Gregorian
	calendar, with the last Julian date being 15 Feb 1923.  The next day was 1 Mar 1923, a skip
	of 13 days.  Other countries, e.g. Turkey (1927) and the Nationalist Government of China (1929)
	adopted the Gregorian calendar later, but had not been using the Julian calendar, previously.
	This does not affect our UTC calendar, which shifts after 4 Oct 1582, skipping 10 days to 5 Oct
	1582.)  We incorporate leap seconds into the UTC calendar beginning in 1972.

	Given the above, the alignment of the UTC coordinate system with TAI ticks works as follows:

	1. 25 Dec 13,826,151,189 BCE 12:00:00.0 - 31 Dec 13,826,151,189 BCE 23:59:59.999999999999999999 "Creation"
		86400 SI seconds per day
	2. 1 Jan 13,826,151,188 BCE 00:00:00.0 - 31 Dec 4714BCE 23:59:59.999999999999999999 "Prehistoric Period"
		86400 SI seconds per day, first leap year in 13,826,151,185 BCE and every 4th year thereafter
	2. 1 Jan 4713 BCE 00:00:00.0 - 4 Oct 1582 23:59:59.999999999999999999 "Julian Period"
		Begins 12 hours before Julian day count begins to simplify calculations.
		86400 SI seconds per day, first leap year in 4713 BCE and every 4th year thereafter
	4. 15 Oct 1582 00:00:00.0 - 31 Dec 1957 23:59:59.999999999999999999 "Gregorian Period"
		86400 SI seconds per day, leap year in 1582 and every 4th year thereafter, except for every century
		that is not a multiple of 400.
	5. 1 Jan 1958 00:00:00.0 - 31 Dec 1960 23:59:59.999999999999999999 "Early Atomic Period"
		Day length accounts for the 1.422818 second drift that occurred.  This drift over a
		3 year period is equivalent to adding 15,025,365,808	ticks (approximately 15 nanoseconds) to an
		SI second to create a calendar second.  For simplicity and to mmore closely mirror what was
		done in practice, add 15 nanoseconds to each second.  The 15 nanoseconds will be
		added at the end of each second.  As such, the time interval from 1 Jan 1958 00:00:00 to
		1 Jan 1958 00:00:01 will be 1.000000015 second, while the time interval from
		1 Jan 1958 00:00:00 to 1 Jan 1958 00:00:00.5 will be 0.5 second.  This approach will
		result in an irregular jump of 2.402 milliseconds at the end of the interval.
		Leap years as for Gregorian period.
	6. 1 Jan 1961 00:00:00.0 - 31 Dec 1971 23:59:59.999999999999999999 "Early UTC Period"
		and "Early UTC Period II".  As of 1 Jan 1972 00:00:00, the drift is exactly 10 seconds.
		This means there was an additionaldrift of 8.577182 seconds between 1 Jan 1961 00:00:00
		and 1 Jan 1972 00:00:00.  As there was an irregular jump of 0.107758 seconds at the end
		of the interval, the drift during the period was 8.469424 seconds.  This drift over an 
		11 year period is equivalent to	adding 24,402,723,609 ticks to an SI second during the 
		interval.  To best approximate this using adjustments of an integral number of 
		nanoseconds, apply a 24 nanosecond adjustment for the first 7 years and a 25 nanosecond
		adjustment for the last 4 years.  This will result in a 5.3001216 second adjustment at the
		end of 7 years and an 8.4558816 second adjustment at the end of 11 years.  This adjustment
		is 13.5424 milliseconds too small, so add a 13.5424 millisecond jump at the start of 1968.
		Leap years as for Gregorian period.
	7. 1 Jan 1972 00:00:00.0 - 31 Dec 2399 23:59:59.999999999999999999 "Leap Second Period"
		Extend for a full 400 year cycle after the last known leap year
		Day length 86400 SI seconds except when leap seconds are applied.  Each calendar second is an SI
		second.  Leap years as for Gregorian period.
	8. 1 Jan 2400 00:00:00.0 +					"Post Leap Second Period
	    Perform calculations assuming no further leap seconds.  This will simplify and speed the
		calculations.  As a pracitcal matter, the existence of these leap seconds won't be
		known for almost 400 years.
		86400 SI seconds per day, leap year in 2400 and every 4th year thereafter, except for every century
		that is not a multiple of 400.

*/

typedef struct
{
	/* A Datetime on the UTC calendar as described in the comment below */
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
					//	nanosecond and attosecond have been removed from the utc data structure because
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
	uint8_t futureAdjust; /* If the value is nonzero, the tick count or datetime elements
							are to be adjusted, if necessary, when leap seconds are added.  
							This is only applicable when a datetime past the end of the leap 
							second table is specified.  The	tick count that is calculated for
							this datetime will reflect the number of leap seconds known at the
							time of specification.  If another leap second is added to the table
							before the specified datetime, the tick count and the datetime elements
							will be inconsistent.  I.e., the tick count will reflect the time one 
							second earlier.  If futureAdjust is 1, the tick count will be 
							recalculated to be consistent with the specified datetime on subsequent
							calls to calcTicks.  If futureAdjust is 2, the datetime elements will
							be adjusted to be consistent with the tick count on a subsequent
							call to adjustDatetime.  If on a call to
							to calcTicks or adjustDatetime, the specified datetime is no longer
							past the end of the table, futureAdjust will be set to 0,
							indicating no further adjustments are required.   */ 
	TAITicks tai; // The tick count at the time of specification.
	uint16_t taiInit; /* Bit field indicating status of initialization and error codes
						0000h indicates initialized without error.
						Other values indicate specification errors as per ccInit
						*/
} UTCDatetime;

typedef struct {
	/* The coordinates of a date*/
	int8_t gigayear; // Gigayear of the date - maximum value is 100, minimum is -13
	int32_t year;  // Positive years = CE, 0 = 1BCE, -1 = 2 BCE, ... -n = n+1 BCE
	uint8_t month; // 1 = Jan, 2 = Feb, ... 12 = Dec
	uint8_t dayOfMonth;
	uint32_t calendar; /* 0 indicates Gregorian calendar with earliest last Julian date
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
	uint16_t dateInit; /* Bit field indicating status of initialization and error codes
					   0000h indicates initialized without error.
					   Other values indicate specification errors as per ccInit */
} DateCoords;

typedef struct {
	/* The coordinates of a time*/
	uint8_t hour; // Calendar hour
	uint8_t minute; // Calendar minute
	uint8_t second; // Calendar second
	uint32_t nanosecond; // Calendar nanosecond
	uint32_t attosecond; // Calendar attosecond
	uint16_t timeInit; /* Bit field indicating status of initialization and error codes
					   0000h indicates initialized without error.
					   Other values indicate specification errors as per ccInit
					   */
} TimeCoords;

typedef struct
{
	/* The calendar coordinates of a datetime.  Forms a partial specification of a datetime.
	Additional data must be provided to complete either a UTC or local datetime specification.
	Levers the DateCoords and TimeCoords structs */
	DateCoords date;
	TimeCoords time;
	uint16_t ccInit; /* Bit field indicating status of initialization and error codes
					 0000h indicates initialized without error.
					 Other values indicate specification errors as follows, by bit number:
					 1 Date before the Big Bang,					 2 GigaYear >= 100,
					 3 Invalid Year <= -1e9 || >= 1e9,				 4 Invalid month,
					 5 Invalid day of the month,					 6 Invalid calendar,
					 7 Hour > 23 or Min > 59 or Sec > 60,			 8 Nanosecond >= 1e9,
					 9 Attosecond >= 1e9,							10 Not a leap second, but second == 60,
					11 Invalid precision,							12 Invalid futureAdjust,
					13 Invalid timezone,							14 Invalid frame of reference,
					15 Invalid dateime (skipped period),			16 Missing disambiguation
					 */
} CalCoords;

typedef struct
{
	/* Relative calendar coordinates.  
		Enables CalCoords arithmetic. 
		RelCalCoords result when 2 CalCoords are subtracted.  
		There are specific algorithms to add RelCalCoords to and subtract RelCalCoords from
		CalCoords, which handle invalid days (e.g., Feb 31st, when a month is added to Jan 31st),
		and which guarantee that if cc1 + rcc = cc2, cc2 -rcc = cc1. 
		
		When performing arithmetic operations, treat invalid dates like imaginary numbers.  
		I.e., it is OK to land on an invalid date during the calculation, but the end date
		must be valid.  
		
		For example, if we add 1 year, 1 month to 2/29/1964.  We first land on 2/29/1965, which
		would be invalid.  However, when we add a month, we are at 3/29/1964, which is valid. */
	uint8_t isNegative;
	uint8_t gigayears;
	uint32_t years; // If years >= e9, will be converted to gigayears and years 
	uint8_t months; // If months >= 12, will be converted to years and months
	uint32_t days; // Days will not be converted to months, years or gigayears,
				   //   as there is no exact conversion.
				   //  In order to add a RelCalCoords with days specified to a CalCoords,
				   //	consider converting the CalCoords to Julian, adding the days, then
				   //	converting back from Julian.
	uint8_t hours; // Hours will be converted to days and hours
	uint8_t minutes; // Minutes will be converted to hours and minutes
	uint8_t seconds; // Seconds will be converted to minutes and seconds
	uint32_t nanoseconds; // Nanoseconds will be converted to seconds and nanoseconds
	uint32_t attoseconds; // Attoseconds will be converted to nanoseconds and attoseconds
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
	uint8_t rccInit; /* Bit field indicating status of initialization and error codes
					 00h indicates initialized without error.
					 Other values indicate specification errors as follows, by bit number:
					 1 Relative datetime greater than max,			 2 N/A,
					 3 N/A,											 4 N/A,
					 5 N/A,											 6 N/A,
					 7 N/A,											 8 Invalid Precision */
} RelCalCoords;

// Error flags
enum UTCErrors {
	NegativeTime		= 0x0001,
	TimeGtMax			= 0x0002,
	InvalidYear			= 0x0004,
	InvalidMonth		= 0x0008,
	InvalidDayOfMonth	= 0x0010,
	InvalidCalendar		= 0x0020,
	HourMinSecGtMax		= 0x0040,
	NanoGtMax			= 0x0080,
	AttoGtMax			= 0x0100,
	NotALeapSecond		= 0x0200,
	InvalidPrecision	= 0x0400,
	InvalidFutureAdjust = 0x0800,
	InvalidTimeZone		= 0x1000,
	InvalidTimeFrame	= 0x2000,
	InvalidDatetime		= 0x4000,
	MissingBOrA			= 0x8000
};

static const char* UTCErrorStrings[] =
{
	"Datetime is before the Big Bang",
	"Datetime is greater than the maximum datetime",
	"Year is invalid",
	"Month is invalid",
	"Invalid day of the month",
	"Invalid calendar",
	"Hour is greater than 23, minute is greater than 59, and/or second is greater than 60",
	"Nanosecond is greater than or equal to 1e9",
	"Attosecond is greater than or equal to 1e9",
	"Second equals 60, but this is not a leap second",
	"Invalid precision and/or uncertainty",
	"Future adjustment is greater than 2, or is 0 when datetime is after the last leap second",
	"Invalid timezone",
	"Invalid frame of reference",
	"Datetime is within a skipped period created by a daylight savings time or GMT offset transition",
	"Before / after specification invalid or missing during an ambiguous period created by a daylight savings time or GMT offset transition"
};

enum TAIRelErrors {
	RelTimeGtMax			= 0x0001,
	DaysGtMax				= 0x0002,
	HoursGtMax				= 0x0004,
	MinutesGtMax			= 0x0008,
	SecondsGtMax			= 0x0010,
	NanosGtMax				= 0x0020,
	AttosGtMax				= 0x0040,
	InvalidRelPrecision		= 0x0080
};

static const char* TAIRelErrorStrings[] =
{
	"Relative datetime is larger than entire UTC datetime range",
	"Days greater than or equal to 1e9",
	"Hours greater than 23",
	"Minutes greater than 59",
	"Seconds greater than 59",
	"Nanoseconds greater than or equal to 1e9",
	"Attoseconds greater than or equal to 1e9",
	"Invalid precision and/or uncertainty"
};

// Leap seconds
#define numLeapSeconds 27  // Need to increment when a new leap second is added
static const int UTCLeapDates[numLeapSeconds][3] = // UTC dates on which leap seconds occur.
{											   //   Local dates could be different due to time zone effects	
	1972, 6, 30,
	1972, 12, 31,
	1973, 12, 31,
	1974, 12, 31,
	1975, 12, 31,
	1976, 12, 31,
	1977, 12, 31,
	1978, 12, 31,
	1979, 12, 31,
	1981, 6, 30,
	1982, 6, 30,
	1983, 6, 30,
	1985, 6, 30,
	1987, 12, 31,
	1989, 12, 31,
	1990, 12, 31,
	1992, 6, 30,
	1993, 6, 30,
	1994, 6, 30,
	1995, 12, 31,
	1997, 6, 30,
	1998, 12, 31,
	2005, 12, 31,
	2008, 12, 31,
	2012, 6, 30,
	2015, 6, 30,
	2016, 12, 31
};

static const uint32_t leapTicks[numLeapSeconds][2] = // Number of giga seconds and seconds at the start of each leap second
{
	436320210, 945556800,	// 30-Jun-1972
	436320210, 961454400,	// 31-Dec-1972
	436320210, 992990400,	// 31-Dec-1973
	436320211, 24526400,	// 31-Dec-1974
	436320211, 56062400,	// 31-Dec-1975
	436320211, 87684800,	// 31-Dec-1976
	436320211, 119220800,	// 31-Dec-1977
	436320211, 150756800,	// 31-Dec-1978
	436320211, 182292800,	// 31-Dec-1979
	436320211, 229553600,	// 30-Jun-1981
	436320211, 261089600,	// 30-Jun-1982
	436320211, 292625600,	// 30-Jun-1983
	436320211, 355784000,	// 30-Jun-1985
	436320211, 434753600,	// 31-Dec-1987
	436320211, 497912000,	// 31-Dec-1989
	436320211, 529448000,	// 31-Dec-1990
	436320211, 576708800,	// 30-Jun-1992
	436320211, 608244800,	// 30-Jun-1993
	436320211, 639780800,	// 30-Jun-1994
	436320211, 687214400,	// 31-Dec-1995
	436320211, 734475200,	// 30-Jun-1997
	436320211, 781908800,	// 31-Dec-1998
	436320212, 2833600,		// 31-Dec-2005
	436320212, 97528000,	// 31-Dec-2008
	436320212, 207860800,	// 30-Jun-2012
	436320212, 302468800,	// 30-Jun-2015
	436320212, 349988800	// 31-Dec-2016
};
#define maxPrecision -18 // Maximum precision for a UTCDatetime or a TAIRelDatetime
#define minUTCPrecision 15 // Minimum precision for a UTCDatetime
#define minRelPrecision 16 // Minimum precision for a TAIRelDatetime
#define numExtraPrecisions 2 // Number of extra precisions stored beyond the maximum
// The offset into the maxUncertainty and precSeconds arrays to add to the 
//  precision to retrieve the maximum  uncertainty for that precision
#define poff (numExtraPrecisions - maxPrecision)
// The length of the UTC precision and uncertainty arrays
#define nPrecUTC (numExtraPrecisions - maxPrecision + minUTCPrecision + 1) 
// The length of the TAIRel precision and uncertainty arrays
#define nPrecRel (numExtraPrecisions - maxPrecision + minRelPrecision + 1)

static const double precSecondsUTCDatetime[nPrecUTC] = // Precision measured in seconds for a UTCDatetime
{
	// First value is for precision of -20 to simplify coding
	1.e-20, 1.e-19, 1.e-18, 1.e-17, 1.e-16, 1.e-15, 1.e-14, 1.e-13, 1.e-12, 1.e-11, 1.e-10, 1.e-9,
	1.e-8, 1.e-7, 1.e-6, 1.e-5, 1.e-4, 1.e-3, 1.e-2, 1.e-1, 1., 60., 3600., 86400., 2592000.,
	3.15576e7, 3.15576e8, 3.15576e9 , 3.15576e10, 3.15576e11, 3.15576e12, 3.15576e13, 3.15576e14,
	3.15576e15, 3.15576e16, 3.15576e17
};

static const double precSecondsTAIRelDatetime[nPrecRel] = // Precision measured in seconds for a TAIRelDatetime
{
	// First value is for precision of -20 to simplify coding
	1.e-20, 1.e-19, 1.e-18, 1.e-17, 1.e-16, 1.e-15, 1.e-14, 1.e-13, 1.e-12, 1.e-11, 1.e-10, 1.e-9,
	1.e-8, 1.e-7, 1.e-6, 1.e-5, 1.e-4, 1.e-3, 1.e-2, 1.e-1, 1., 60., 3600., 86400., 864000.,
	8.64e6, 8.64e7, 8.64e8, 8.64e9 , 8.64e10, 8.64e11, 8.64e12, 8.64e13, 8.64e14,
	8.64e15, 8.64e16, 8.64e17
};

static const int8_t maxUnceraintyUTCDatetime[nPrecUTC] = // Maximum allowed uncertainty by precision for a UTCDatetime
{
	// First value is for precision of -20 to simplify coding
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 59, 59, 23, 29, 11, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9
};

static const int8_t maxUnceraintyTAIRelDatetime[nPrecRel] = // Maximum allowed uncertainty by precision for a TAIRelDatetime
{
	// First value is for precision of -20 to simplify coding
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 59, 59, 23, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9
};


// Useful constants
static const int32_t secondsPerDay = 86400;
static const uint32_t secondsInNonLeap = 31536000; // 365 * secondsPerDay;
static const uint32_t daysInNonLeapCentury = 36524; // 76*365 + 24*366
static const uint32_t daysInLeapCentury = 36525; // 75*365 + 24*366
static const uint32_t daysIn4GregorianCenturies = 146097; // 303*365 + 97*366
static const uint32_t bbdayOfYear = 360; // day of year of Big Bang - 25 Dec in leap year
static const int32_t bbYear = -826151188; // Big Bang year + 13e9
// Constrain the magnitude of the relative time to be within the range of valid times
//  From the Big Bang (25-Dec-13,826,151,189 BCE 12:00:00) to maximum time 
//	(31-Dec-99,999,999,999 23:59:59.999999999 999999999) is 
//	41,574,251,721,059d 12h 0m 35.999999999 999999999s
static const uint16_t maxGigaSIDays = 41574;
static const uint32_t maxSIDays = 251721059;
//
static const char *months[] = { " ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
							"Sep", "Oct", "Nov", "Dec"};
static const double doublePrecision = 9.007199254740992e15; // Double precision values are precise to
														   //   1 part in 9.007199254740992e15
static const double maxExact = 9007199254740992.; // Largest integer that is guaranteed to be represented
												  //	exactly as a double 2^53
// For a double precision value, the theoretical minimum uncertainty is 1.0 / doublePrecision
//	which is 1.110223024625157e-16, however experimentation showed 2.e-16 to be
//	a better value.  An example of why is that 9. + 1.e-15 is represented as 9.000 000 000 000 002.
static const double minFracUncertainty = 2.e-16; 
// Allow 16 significant decimal digits for a double precision number.  The following factor
//	when multiplied by the value then reduced to the nearest even power of 10 establishes
//	the appropriate precision.  The reduction to an even power of 10 makes the effective
//	maximum fractional precision 1.e-16 (consider a mantissa of 9.999 999 999 999 999.  
//	The fractional precision is 1. / 9 999 999 999 999 999. which is 1.000 000 000 000 000 1e-16.
//	As the 17th significant digit is ignored, this values is 1.e-16.).
//
static const double maxPrecisionFactor = 1.e-15;
static const double maxFracPrecision = 1.e-16;
static const uint32_t uint32Max = 4294967295; // Largest unsigned, 32-bit integer
static const double dMaxRelTimeGiga = 3592015348.699541; // Smallest double precision value > maxRelTime
static const uint16_t uint16Max = 65535;

// Establish the beginning of the periods

// TAITicks Creation = createTicks(0,0,0,0); // 25 Dec 13,826,151,189 BCE 12:00:00
// TAITicks Prehistoric = createTicks(0, lengthOfCreation, 0, 0); // 1 Jan 13,826,151,188 BCE 00:00:00
// TAITicks Julian = createTicks(436320000-1,e9-secondsPerDay/2,0,0); // 1 Jan 4713 BCE 00:00:00
// TAITicks Gregorian = createTicks(436320198, 647467200, 0, 0); // 15 Oct 1582 00:00:00
// TAITicks Start1583 = createTicks(436320199, 190664000, 0, 0); // 1 Jan 1600 00:00:00
// TAITicks Start1600 = createTicks(436320199, 190664000, 0, 0); // 1 Jan 1600 00:00:00
// TAITicks Start1923Julian = createTicks(436320209, 384654400, 0, 0); // 1 Jan 1923 00:00:00 Julian
// TAITicks EarlyAtomic = createTicks(436320210, 488068800, 0, 0); // 1 Jan 1958 00:00:00
// TAITicks EarlyUTC = createTicks(436320210, 582763200, 0, 0); // 1 Jan 1961 00:00:00 (unadjusted)
// TAITicks EarlyUTCadj = createTicks(436320210, 582763201, 422818000, 0); // 1 Jan 1961 00:00:00 (adjusted)
// TAITicks EarlyUTC2 = createTicks(436320210, 803601600, 0, 0); // 1 Jan 1968 00:00:00 (unadjusted)
// TAITicks EarlyUTC2adj = createTicks(436320210, 803601606, 736482000, 0); // 1 Jan 1968 00:00:00 (adjusted)
// TAITicks LeapSecond = createTicks(436320210, 929832010, 0, 0); // 1 Jan 1972 00:00:00 (adjusted)
// TAITicks32 Start2000 = createTicks{436320211, 813444832, 0, 0}; // 1 Jan 2000 00:00:00 (adjusted)
// TAITicks PostLeapSecond = createTicks(436320224, 436225636, 0, 0); // 1 Jan 2400 00:00:00 (adjusted)

#ifdef Ticks64
	 static const TAITicks64 Creation =		  {0, 0};
	 static const TAITicks64 Prehistoric =	  {561600, 0};
	 static const TAITicks64 Julian =		  {436319999999956800, 0};
	 static const TAITicks64 Gregorian =	  {436320198647467200, 0};
	 static const TAITicks64 Start1583 =	  {436320198654206400, 0};
	 static const TAITicks64 Start1600 =	  {436320199190664000, 0};
	 static const TAITicks64 Start1923Julian =	  {436320209384654400, 0};
	 static const TAITicks64 EarlyAtomic =	  {436320210488068800, 0};
	 static const TAITicks64 EarlyUTC =		  {436320210582763200, 0};
	 static const TAITicks64 EarlyUTCAdj =	  {436320210582763201, 422818000000000000};
	 static const TAITicks64 EarlyUTC2 =	  {436320210803601600, 0};
	 static const TAITicks64 EarlyUTC2Adj =	  {436320210803601606, 736482000000000000};
	 static const TAITicks64 LeapSecond =	  {436320210929832010, 0};
	 static const TAITicks64 Start2000 =	  {436320211813444832, 0};
	 static const TAITicks64 PostLeapSecond = {436320224436225610 + numLeapSeconds, 0};
#else
	 static const TAITicks32 Creation =		  {0, 0, 0, 0};
	 static const TAITicks32 Prehistoric =	  {0, 561600, 0, 0};
	 static const TAITicks32 Julian =		  {436319999, 999956800, 0, 0};
	 static const TAITicks32 Gregorian =	  {436320198, 647467200, 0, 0};
	 static const TAITicks32 Start1583 =	  {436320198, 654206400, 0, 0};
	 static const TAITicks32 Start1600 =	  {436320199, 190664000, 0, 0};
	 static const TAITicks32 Start1923Julian =	  {436320209, 384654400, 0, 0};
	 static const TAITicks32 EarlyAtomic =	  {436320210, 488068800, 0, 0};
	 static const TAITicks32 EarlyUTC =		  {436320210, 582763200, 0, 0};
	 static const TAITicks32 EarlyUTCAdj =	  {436320210, 582763201, 422818000, 0};
	 static const TAITicks32 EarlyUTC2 =	  {436320210, 803601600, 0, 0};
	 static const TAITicks32 EarlyUTC2Adj =	  {436320210, 803601606, 736482000, 0};
	 static const TAITicks32 LeapSecond =	  {436320210, 929832010, 0, 0};
	 static const TAITicks32 Start2000 =	  {436320211, 813444832, 0, 0};
	 static const TAITicks32 PostLeapSecond = {436320224, 436225610 + numLeapSeconds, 0, 0};
#endif

// Establish the maximum value for ticks
	 static const TAITicks64 EndOfTime64 =	  {3592015348699540809 + numLeapSeconds, 999999999999999999};
	 static const TAITicks64 EndOfTimePlus64 = {3592015348699540810 + numLeapSeconds, 0};
	 static const TAITicks32 EndOfTime32 =	  {3592015348, 699540809 + numLeapSeconds, 999999999, 999999999};
	 static const TAITicks32 EndOfTimePlus32 = {3592015348, 699540810 + numLeapSeconds, 0, 0};
#ifdef Ticks64
	 static const TAITicks64 EndOfTime =	  {3592015348699540809 + numLeapSeconds, 999999999999999999};
	 static const TAITicks64 EndOfTimePlus = {3592015348699540810 + numLeapSeconds, 0};
#else
	 static const TAITicks32 EndOfTime =	  {3592015348, 699540809 + numLeapSeconds, 999999999, 999999999};
	 static const TAITicks32 EndOfTimePlus = {3592015348, 699540810 + numLeapSeconds, 0, 0};
#endif
// Establish the maximum range for relticks
	 static const TAIRelTicks64 MaxRelTime64 = {3592015348699540809 + numLeapSeconds, 999999999999999999};
	 static const TAIRelTicks64 MaxRelTimePlus64 = {3592015348699540810 + numLeapSeconds, 0};
	 static const TAIRelTicks32 MaxRelTime32 = {3592015348, 699540809 + numLeapSeconds, 999999999, 999999999};
	 static const TAIRelTicks32 MaxRelTimePlus32 = {3592015348, 699540810 + numLeapSeconds, 0, 0};
#ifdef Ticks64
	 static const TAIRelTicks64 MaxRelTime = {3592015348699540809 + numLeapSeconds, 999999999999999999};
	 static const TAIRelTicks64 MaxRelTimePlus = {3592015348699540810 + numLeapSeconds, 0};
#else
	 static const TAIRelTicks32 MaxRelTime = {3592015348, 699540809 + numLeapSeconds, 999999999, 999999999};
	 static const TAIRelTicks32 MaxRelTimePlus = {3592015348, 699540810 + numLeapSeconds, 0, 0};
#endif

// Earleist transition date from Julian to Gregorian calendar
static const DateCoords EarliestTransitionDate = { 0, 1582, 10, 4, 1, 0 };
// Latest transition date from Julian to Gregorian calendar
//	As a practical matter, the last transition occurred in the 1920s,
//	however, we allow transitions up to a number of skipped days where
//	the new date is as late as the end of the next month.  As the shortest
//	month has 28 days, we can allow a skip of 28 days.  Up until 2/28/1700,
//	the number of skipped days is 10.  From 2/29/1700 through 2/28/1800,
//	the number of skipped days increases to 11.  From 2/29/1800 through
//	2/28/1900, the number of skipped days increases to 12.  From 2/29/1900
//	through 2/28/2100, the number of skipped days increase to 13 days.
//	Following this same pattern, the last day on which the number of skipped
//	days is <= 28 is 2/28/4100.
static const DateCoords LatestTransitionDate = { 0, 4100, 2, 28, 1, 0 };
// The calendar that corrersponds to the earliest transition date
static const uint32_t MinCalendar = 15821004;
// The calendar that corrersponds to the latest transition date
static const uint32_t MaxCalendar = 41000228; 
// The corresponding first Gregorian date to the latest transition date is 3/29/4100
static const DateCoords LatestFirstGregorianDate = { 0, 4100, 3, 29, 0, 0 };
// The maximum Julian date is the latest date is defined to keep the respective
//	Gregorian date within the range of a DateCoords.
// Every 400 Gregorian years corresponds to 400 Julian years - 3 days
//	This is equivalent to the ratio of 365.2425 / 365.25.
//	The time period from 12/31/1599 to 12/31/99,999,999,999 is 99,999,998,400 Gregorian years,
//	which corresponds to 99,997,945,011.9425 Julian years.  This equates to year 99,997,946,610.9425,
//	which is the 345th day of 99,997,946,610, which is a leap year.  Recalling that the Gregorian
//	calendar is 10 days ahead of the Julian calendar on 12/31/1599, this equates to the 335th day
//	of the year, which is 11/30.  Finally, as we wish to allow an extra day for local times, the
//	last allowed Julian date is 1-Dec-99,997,946,611.
//
static const DateCoords MaxJulianDate = { 99, 997946611, 12, 1, 1, 0 };
//  Useful dates for Swedish calendar
static const DateCoords feb281700 = { 0, 1700, 2, 28, 1, 0 };
static const DateCoords mar011712 = { 0, 1712, 3, 1, 1, 0 };
static const DateCoords feb171753 = { 0, 1753, 2, 17, 1, 0 };
static const DateCoords mar011753 = { 0, 1753, 3, 1, 0, 0 };

// Get the number of bits used for each element of TAITicks and TAIRelTicks
DLLExport int getTickBits(void);

// 64 and 32 bit comparison functions for TAITicks
DLLExport int isEqualTicks64 (TAITicks64 t1, TAITicks64 t2);
DLLExport int isEqualTicks32 (TAITicks32 t1, TAITicks32 t2);
DLLExport int isEqualTicks (TAITicks t1, TAITicks t2);
DLLExport int isGreaterTicks64 (TAITicks64 t1, TAITicks64 t2);
DLLExport int isGreaterTicks32 (TAITicks32 t1, TAITicks32 t2);
DLLExport int isGreaterTicks (TAITicks t1, TAITicks t2);
DLLExport int isLessTicks64 (TAITicks64 t1, TAITicks64 t2);
DLLExport int isLessTicks32 (TAITicks32 t1, TAITicks32 t2);
DLLExport int isLessTicks (TAITicks t1, TAITicks t2);
DLLExport int isNotEqualTicks (TAITicks t1, TAITicks t2);
DLLExport int isGreaterOrEqualTicks (TAITicks t1, TAITicks t2);
DLLExport int isLessOrEqualTicks (TAITicks t1, TAITicks t2);

// 64 and 32 bit comparison functions for TAIRelTicks
DLLExport int isEqualRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2);
DLLExport int isEqualRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2);
DLLExport int isEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2);
DLLExport int isGreaterRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2);
DLLExport int isGreaterRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2);
DLLExport int isGreaterRelTicks (TAIRelTicks t1, TAIRelTicks t2);
DLLExport int isLessRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2);
DLLExport int isLessRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2);
DLLExport int isLessRelTicks (TAIRelTicks t1, TAIRelTicks t2);
DLLExport int isNotEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2);
DLLExport int isGreaterOrEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2);
DLLExport int isLessOrEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2);

// 64 and 32 bit tick utility functions
DLLExport uint32_t getNanosecond64 (TAITicks64 t1);
DLLExport uint32_t getNanosecond32 (TAITicks32 t1);
DLLExport uint32_t getNanosecond (TAITicks t1);
DLLExport uint32_t getAttosecond64 (TAITicks64 t1);
DLLExport uint32_t getAttosecond32 (TAITicks32 t1);
DLLExport uint32_t getAttosecond (TAITicks t1);

// 64 and 32 bit relative tick utility functions
DLLExport uint32_t getGigaseconds64 (TAIRelTicks64 r1);
DLLExport uint32_t getGigaseconds32 (TAIRelTicks32 r1);
DLLExport uint32_t getGigaseconds (TAIRelTicks r1);
DLLExport uint32_t getSeconds64 (TAIRelTicks64 r1);
DLLExport uint32_t getSeconds32 (TAIRelTicks32 r1);
DLLExport uint32_t getSeconds (TAIRelTicks r1);
DLLExport uint32_t getNanoseconds64 (TAIRelTicks64 r1);
DLLExport uint32_t getNanoseconds32 (TAIRelTicks32 r1);
DLLExport uint32_t getNanoseconds (TAIRelTicks r1);
DLLExport uint32_t getAttoseconds64 (TAIRelTicks64 r1);
DLLExport uint32_t getAttoseconds32 (TAIRelTicks32 r1);
DLLExport uint32_t getAttoseconds (TAIRelTicks r1);
DLLExport double doubleSeconds(TAIRelTicks r1);


// Difference between 2 TAITicks
DLLExport TAIRelTicks64 diffTicks64 (TAITicks64 t1, TAITicks64 t2);
DLLExport TAIRelTicks32 diffTicks32 (TAITicks32 t1, TAITicks32 t2);
DLLExport TAIRelTicks diffTicks (TAITicks t1, TAITicks t2);

// String utility functions
DLLExport int rightTrim (char * str);
DLLExport char * underscoreToBlank (char str[]);

// Add relative ticks to ticks
DLLExport TAITicks64 addRelTicksToTicks64(TAITicks64 t1, TAIRelTicks64 r1);
DLLExport TAITicks32 addRelTicksToTicks32(TAITicks32 t1, TAIRelTicks32 r1);
DLLExport TAITicks addRelTicksToTicks(TAITicks t1, TAIRelTicks r1);

// Subtract relative ticks from ticks
DLLExport TAITicks64 subtractRelTicksFromTicks64(TAITicks64 t1, TAIRelTicks64 r1);
DLLExport TAITicks32 subtractRelTicksFromTicks32(TAITicks32 t1, TAIRelTicks32 r1);
DLLExport TAITicks subtractRelTicksFromTicks(TAITicks t1, TAIRelTicks r1);

// Determine sign of relative ticks
DLLExport int isNegativeRelTicks64(TAIRelTicks64 r1);
DLLExport int isNegativeRelTicks32(TAIRelTicks32 r1);
DLLExport int isNegativeRelTicks(TAIRelTicks r1);

// Negate relative ticks
DLLExport TAIRelTicks64 negateRelTicks64(TAIRelTicks64 r1);
DLLExport TAIRelTicks32 negateRelTicks32(TAIRelTicks32 r1);
DLLExport TAIRelTicks negateRelTicks(TAIRelTicks r1);

// Add relative ticks to relative ticks
DLLExport TAIRelTicks64 addRelTicks64(TAIRelTicks64 r1, TAIRelTicks64 r2);
DLLExport TAIRelTicks32 addRelTicks32(TAIRelTicks32 r1, TAIRelTicks32 r2);
DLLExport TAIRelTicks addRelTicks(TAIRelTicks r1, TAIRelTicks r2);

// Subtract relative ticks from relative ticks
DLLExport TAIRelTicks64 subtractRelTicks64(TAIRelTicks64 r1, TAIRelTicks64 r2);
DLLExport TAIRelTicks32 subtractRelTicks32(TAIRelTicks32 r1, TAIRelTicks32 r2);
DLLExport TAIRelTicks subtractRelTicks(TAIRelTicks r1, TAIRelTicks r2);

// Format ticks, relative ticks and datetimes
DLLExport const char * asStringTAITicks64 (TAITicks64 t1);
DLLExport const char * asStringTAITicks32 (TAITicks32 t1);
DLLExport const char * asStringTAITicks (TAITicks t1);
DLLExport const char * asStringTAIRelTicks64 (TAIRelTicks64 r1);
DLLExport const char * asStringTAIRelTicks32 (TAIRelTicks32 r1);
DLLExport const char * asStringTAIRelTicks (TAIRelTicks r1);
DLLExport const char * asStringUncertainDouble(UncertainDouble u1);
DLLExport const char * asStringDateCoords(DateCoords y1, char stringDate[]);
DLLExport char * sprintfComma (int32_t n);
DLLExport char * sprintfDoubleComma(double value, double precision, int isAdjustable);
DLLExport char * sprintfUncertaintyUTCDatetime(int8_t uncertainty, int8_t precision);
DLLExport char * sprintfUncertaintyTAIRelDatetime (int8_t uncertainty, int8_t precision);
DLLExport char * sprintfYear (int8_t gigaYear, int32_t year);
DLLExport char * sprintfDays (uint32_t gigaDays, uint32_t days);
DLLExport const char * asStringDatetime(int8_t inGigayear, int32_t inYear, uint8_t inMonth,
	uint8_t inDayOfMonth, uint8_t inHour, uint8_t inMinute,
	uint8_t inSecond, uint32_t inNanosecond, uint32_t inAttosecond,
	uint32_t calendar, uint8_t frame, int8_t precision, int8_t uncertainty,
	char timeZone[], uint8_t BorA, char stringDT[]);
DLLExport const char * asStringUTCDatetime (UTCDatetime u1, char stringUTC[], uint32_t calendar);
DLLExport const char * asStringCalCoords(CalCoords cc, char stringCal[]);
DLLExport const char * readableNumberString (const char * tickString, char readableTickString[]);
DLLExport const char * asStringTAIRelDatetime (TAIRelDatetime re, char tickElemString[]);

// Create ticks and relativee ticks
DLLExport TAITicks64 createTicks64(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds);
DLLExport TAITicks32 createTicks32(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds);
DLLExport TAITicks createTicks(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds);
DLLExport TAIRelTicks64 createRelTicks64(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds, int isNegative);
DLLExport TAIRelTicks32 createRelTicks32(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds, int isNegative);
DLLExport TAIRelTicks createRelTicks(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoseconds, int isNegative);

// Correct a relative tick for drift for the period 1-Jan-1958 through 31-Dec-1971
DLLExport TAIRelTicks adjustRelTicks(TAIRelTicks r1, uint32_t nanoPerSecondAdjustment);
// Calculate the uncorrected tick value for a tick corrected for drift between 1-Jan-1958 and 31-Dec-1971
DLLExport TAIRelTicks unAdjustRelTicks(TAIRelTicks r1Adj, uint8_t period);

// Derive relative tick elements from relative ticks
DLLExport TAIRelDatetime deriveTAIRelDatetime(TAIRelTicks r1, int8_t precision, int8_t uncertainty);

// Convert from day of year to Month/day and vice versa.  These functons ignore skipped days during transition
//	years, so should not generally be used.  Use dayOfYearFromDate and createDateCoordsFromDayOfYear, instead.
DLLExport uint8_t * dayOfYearToMonthDay(uint32_t dayOfYear, uint8_t numLeapDays);
DLLExport uint32_t monthDayToDayOfYear(uint8_t month, uint8_t day, uint8_t numLeapDays);

// Determine the day of the week corresponding to a date, which may be either local or UTC
//DLLExport uint8_t dayOfTheWeek(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar);
DLLExport uint8_t dayOfTheWeek(DateCoords date);

// Find date given a weekday rule
DLLExport uint8_t dayOfMonthFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month, uint8_t onOrAfterDayOfMonth,
	uint8_t weekNumber, uint8_t dayOfTheWeek, uint32_t calendar);

// Return number of leap days in a year
DLLExport uint8_t numLeapDays(int8_t gigayear, int32_t year, uint32_t calendar);

// Determine whether a UTC date is one that ends with a leap second.
//	This function should not be used with local dates
DLLExport int isLeapSecondDay(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth);


// Return cumulative leap seconds as of a tick prior to adjustment for leap seconds
DLLExport int cumLeapSecondsUnadj(TAITicks unadjt1, int isPotentialLeapSecond);

// Return cumulative leap seconds as of a tick after adjustment for leap seconds
DLLExport int cumLeapSecondsAdj(TAITicks adjt1);

// Create UTCDateTime
DLLExport UTCDatetime createUTCDatetime(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
						   int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust);
DLLExport UTCDatetime createUTCDatetimeFromDayFrac(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, double dayFrac,
						   int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust);
DLLExport UTCDatetime createUTCDatetimeFromSecondFrac(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute, double secondPlusFrac, 
						   int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust);

// Create CalCoords
DLLExport CalCoords createCalCoords(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
						   uint32_t calendar);
DLLExport CalCoords createCalCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
						   uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
						   uint32_t attosecond,	uint32_t calendar);
DLLExport CalCoords createCalCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek,
						   uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond,
						   uint32_t attosecond, uint32_t calendar);

// Create DateCoords
DLLExport DateCoords createDateCoords(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar);

// Construct a DateCoords from day of year, instead of month and day of month
DLLExport DateCoords createDateCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear, uint32_t calendar);

// Construct a DateCoords from a week day rule
DLLExport DateCoords createDateCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month, uint8_t afterDayOfMonth,
	uint8_t weekNumber, uint8_t dayOfWeek, uint32_t calendar);

// Create TimeCoords
DLLExport TimeCoords createTimeCoords(uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond);

// Create TAIRelDateTime
DLLExport TAIRelDatetime createTAIRelDatetime(uint16_t gigaSIdays, uint32_t sIdays, uint8_t sIhours,
							uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds, 
							uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty);
DLLExport TAIRelDatetime createTAIRelDatetimeFromDayFrac(double daysPlusFrac,
							int8_t precision, int8_t uncertainty);
DLLExport TAIRelDatetime createTAIRelDatetimeFromSecondFrac(double secondsPlusFrac, 
							int8_t precision, int8_t uncertainty);

// Create RelCalCoords
DLLExport RelCalCoords createRelCalCoords(uint8_t gigayears, uint32_t years, uint32_t months,
	uint32_t days, uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t nanoseconds,
	uint32_t attoseconds, uint8_t isNegative, int8_t precision, int8_t uncertainty);

// Create an UncertainDouble, ensuring the validity of the precision and uncertainty
DLLExport UncertainDouble createUncertainDouble(double ivalue, double iprecision, double iuncertainty);

// UTCDatetime comparison functions
DLLExport int mustEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mayNotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isNotEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int canEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int cannotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isEqualRangeUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isNotEqualRangeUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mustBeGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mayBeLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isGreaterMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isLessOrEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int canBeGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mustBeLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mustBeLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mayBeGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isLessMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isGreaterOrEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int canBeLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int mustBeGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);

//**** Deprecate after the above are implemented and tested
DLLExport int isEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isNotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);
DLLExport int isLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2);

// UTCDatetime utility functions
//	Calculate the number of TAI seconds since Creation at a UTC Datetime
DLLExport double doubleSecondsUTCDatetime(UTCDatetime utc); 

// TAIRelDatetime comparison functions
DLLExport int isEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);
DLLExport int isGreaterRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);
DLLExport int isLessRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);
DLLExport int isNotEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);
DLLExport int isGreaterOrEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);
DLLExport int isLessOrEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2);

// TAIRelDatetime utility functions
//  Convert TAIRelDatetime to seconds
DLLExport double doubleSecondsRelDatetime(TAIRelDatetime re);

// Check the validity of calendar coordinates
DLLExport uint16_t checkCalCoordsElements(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
						   uint32_t calendar);

// Check whether a date is skipped in the transition from Julian 
//	to Gregorian calendar
DLLExport int isSkipped(uint32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar);

// Check the validity of Date coordinates
DLLExport uint16_t checkDateCoordsElements(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint32_t calendar);

// Check the validity of Time coordinates
DLLExport uint16_t checkTimeCoordsElements(uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond);

// Check the validity of datetime elements
DLLExport uint16_t checkDatetimeElements(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
						   int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust);

// Adjust CalCoords to be consistent with specified precision and return a CalCoords
DLLExport CalCoords adjustCalCoords(CalCoords cc, int8_t precision);

// Adjust UTCDatetime elements to be consistent with precision and return a UTCDatetime
DLLExport UTCDatetime adjustUTCDatetimeElements(int8_t gigayear, int32_t year, uint8_t month,
							uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
							uint8_t second, uint32_t nanosecond, uint32_t attosecond,
							int8_t precision, int8_t uncertainty, uint8_t futureAdjust);

// Adjust TAIRelDatetime elements to be consistent with precision
DLLExport TAIRelDatetime adjustRelDatetimeElements(uint16_t gigaSIdays, uint32_t sIdays,
					uint8_t sIhours, uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds,
					uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty);

// Check the validity of relative datetime elements
DLLExport uint8_t checkRelDatetimeElements(uint16_t gigaSIdays, uint32_t sIdays, uint8_t sIhours,
							uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds, 
							uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty);

// Check the validity of an uncertainty given a precision
DLLExport int isValidUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime);

// Adjust a zero uncertainty given a precision
DLLExport int8_t adjustZeroUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime);

// Adjust a precision / uncertainty pair so the uncertainty falls within the allowed range
DLLExport PrecisionUncertainty adjustPrecisionUncertainty(int8_t precision, int uncertainty,
	int8_t isRelDatetime);

// Calculate the absolute value of uncertainty in seconds
DLLExport double secondsUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime);

// Calculate the absolute value of uncertainty in seconds, bounded by the limits of 
//	double precision representation
DLLExport double boundedSecondsUncertainty(int8_t uncertainty, int8_t precision, 
	int8_t isRelDatetime, double value);

// Derive a precision and uncertainty from an uncertainty in seconds
DLLExport PrecisionUncertainty precisionFromSecondsUncertainty(double secondsUncertainty,
	int8_t isRelDatetime);

// Calculate the ratio of two relative datetimes as a double with uncertainty
DLLExport UncertainDouble ratioRelDatetimes(TAIRelDatetime num, TAIRelDatetime den);

// Multiply a relative datetime by a double with an uncertainty
DLLExport TAIRelDatetime doubleMultRelDatetime(UncertainDouble rr, TAIRelDatetime re1);

// Multiply relative datetime by a 32 bit signed integer
DLLExport TAIRelDatetime intMultRelDatetime(int32_t mult, TAIRelDatetime re1);

// Multiply relative ticks by 2, 3 or 4
DLLExport TAIRelTicks smallIntMultTAIRelTicks(uint32_t mult, TAIRelTicks r1);
// Multiply relative ticks by a 32 bit signed integer
DLLExport TAIRelTicks intMultTAIRelTicks(int32_t mult, TAIRelTicks r1);

// Report errors encountered during the contruction of a UTCDatetime based on init bit flag
DLLExport char * reportUTCConstructionError(uint16_t init, char buffer[], int lenBuffer, int failTest);

// Report errors encountered during the contruction of a TAIRelDatetime based on init bit flag
DLLExport char * reportTAIRelConstructionError(uint8_t init, char buffer[], int lenBuffer, int failTest);

// Convert UTCDatetime to ticks
DLLExport UTCDatetime calcTicks(UTCDatetime utc, uint32_t nanosecond, uint32_t attosecond);

// Derive UTCDatetime from ticks
DLLExport UTCDatetime deriveUTCDatetime(TAITicks t1, int8_t precision, int8_t uncertainty,
							uint8_t futureAdjust);

// Subtract UTCDatetimes
DLLExport TAIRelDatetime diffUTCDatetimes(UTCDatetime u1, UTCDatetime u2);

// Add TAIRelDatetime to UTCDatetime
DLLExport UTCDatetime addRelToUTCDatetime(UTCDatetime u1, TAIRelDatetime re1, uint8_t futureAdjust);

// Subtract TAIRelDatetime from UTCDatetime
DLLExport UTCDatetime subtractRelFromUTCDatetime(UTCDatetime u1, TAIRelDatetime re1, uint8_t futureAdjust);

// Add TAIRelDatetime to TAIRelDatetime
DLLExport TAIRelDatetime addRelDatetimes(TAIRelDatetime re1, TAIRelDatetime re2);

// Subtract TAIRelDatetime from TAIRelDatetime
DLLExport TAIRelDatetime subtractRelDatetimes(TAIRelDatetime re1, TAIRelDatetime re2);

// Negate TAIRelDatetime
DLLExport TAIRelDatetime negateRelDatetime(TAIRelDatetime re1);

// UncertainDouble comparison functions
/*  There are 20 useful comparison functions for uncertain values.
	 The second function in each pair is the negation of the first function.

	 Equality
	 ========
	 mustEqual						Midpoint values are equal, both uncertainties are 0.
	 mayNotEqual					Midpoint values are different, or either uncertainty > 0

	 isEqualMidpoint				Midpoint values are equal
	 isNotEqualMidpoint				Midpoint values are not equal

	 canEqual						Ranges overlap
	 cannotEqual					Ranges do not overlap

	 isEqualRange					Midpoint values are equal, uncertainties are equal
	 isNotEqualRange				Midpoint values are not equal, or uncertainties are not equal

	 Greater Than / Less Than or Equal
	 =================================
	 mustBeGreater					Lower bound of range 1 > Upper bound of range 2
	 mayBeLessOrEqual				Lower bound or range 1 <= Upper bound or range 2

	 isGreaterMidpoint				Midpoint of range 1 > Midpoint of range 2
	 isLessOrEqualMidpoint			Midpoint of range 1 <= Midpoint of range 2

	 canBeGreater					Upper bound of range 1 > Lower bound of range 2
	 mustBeLessOrEqual				Upper bound of range 1 <= Lower bound of range 2

	 Less Than / Greater Than or Equal
	 =================================
	 mustBeLess						Upper bound of range 1 < Lower bound of range 2
	 mayBeGreaterOrEqual			Upper bound of range 1 >= Lower bound of range 2 

	 isLessMidpoint					Midpoint of range 1 < Midpoint of range 2
	 isGreaterOrEqualMidpoint		Midpoint of range 1 >= Midpoint of range 2

	 canBeLess						Lower bound of range 1 < Upper bound of range 2
	 mustBeGreaterOrEqual			Lower bound of range 1 >= Upper bound of range 2
	 
*/

DLLExport int mustEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mayNotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isNotEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int canEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int cannotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isEqualRangeUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isNotEqualRangeUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mustBeGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mayBeLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isGreaterMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isLessOrEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int canBeGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mustBeLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mustBeLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mayBeGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isLessMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isGreaterOrEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int canBeLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int mustBeGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

//**** Deprecate when above are tested
DLLExport int isEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isNotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);
DLLExport int isLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

// Negate UncertainDouble
DLLExport UncertainDouble negateUncertainDouble(UncertainDouble u1);

// Add UncertainDouble to UncertainDouble
DLLExport UncertainDouble addUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

// Subtract UncertainDouble from UncertainDouble
DLLExport UncertainDouble subtractUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

// Multiply UncertainDoubles
DLLExport UncertainDouble multiplyUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

// Divide UncertainDoubles
DLLExport UncertainDouble divideUncertainDoubles(UncertainDouble u1, UncertainDouble u2);

// Math utilities
// Min and max functions
DLLExport int max ( int a, int b ) { return a > b ? a : b; };
DLLExport int min ( int a, int b ) { return a < b ? a : b; };
// Multiply 2 32 bit integers and produce a 64 bit integer represented as 2 32 bit integers
DLLExport UInt32MultResult uInt32Mult (uint32_t f1, uint32_t f2);
// Integer exponentiation
DLLExport int power(int base, unsigned int exp);

// Propagate precision and uncertainty
DLLExport PrecisionUncertainty addPrecisionUncertainty(int8_t precision1, int8_t uncertainty1, 
	int8_t isRelDatetime1, int8_t precision2, int8_t uncertainty2, int8_t isRelDatetime2); 

// Compute subsecond precision from double precision value
DLLExport int8_t subSecondPrecision(double value, double precision);


// Datetime Coordinate comparison functions
DLLExport int compareCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isEqualCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isGreaterCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isLessCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isNotEqualCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isGreaterOrEqualCalCoords (CalCoords calc1, CalCoords calc2);
DLLExport int isLessOrEqualCalCoords (CalCoords calc1, CalCoords calc2);

// Year Month Day Coordinate comparison functions ignoring calendar
//	These functions are used in the functions that convert
//	between calendars.  Given that the calendar conversion
//	functions are needed to rigorously compare DateCoords
//	a second set of functions is needed.
DLLExport int compareDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isEqualDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isGreaterDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isLessDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isNotEqualDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isGreaterOrEqualDateCoords_NC(DateCoords date1, DateCoords date2);
DLLExport int isLessOrEqualDateCoords_NC(DateCoords date1, DateCoords date2);

// Year Month Day Coordinate comparison functions including calendar
DLLExport int compareDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isEqualDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isGreaterDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isLessDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isNotEqualDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isGreaterOrEqualDateCoords(DateCoords date1, DateCoords date2);
DLLExport int isLessOrEqualDateCoords(DateCoords date1, DateCoords date2);

// Time Coordinate comparison functions
DLLExport int compareTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isEqualTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isGreaterTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isLessTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isNotEqualTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isGreaterOrEqualTimeCoords(TimeCoords time1, TimeCoords time2);
DLLExport int isLessOrEqualTimeCoords(TimeCoords time1, TimeCoords time2);

// Skip days on the Gregorian calendar to enable transition from Julian to Gregorian calendar
DLLExport DateCoords skipGregorianDays(uint8_t numberOfDays, DateCoords date1);

// Extract last Julian date for a calendar
DLLExport DateCoords lastJulianDate(uint32_t calendar);
// Find first Gregorian date for a calendar
DLLExport DateCoords firstGregorianDate(uint32_t calendar);
// Find the number of days to skip in a transition from the Julian to Gregorian calendar
DLLExport uint8_t numDaysToSkip(uint32_t calendar);
// Find the number of days in a year given a calendar, considering skipped days
DLLExport uint32_t numDaysInYear(int8_t gigayear, int32_t year, uint32_t calendar);

// Convert a Julian date to a UTC Gregorian date
DLLExport DateCoords julianToUTCGregorian(DateCoords dateJulian);
// Convert a UTC Gregorian date to a Julian date
DLLExport DateCoords uTCGregorianToJulian(DateCoords dateUTCGreg);

// Convert a date on a specified calendar to a date on the Gregorian calendar
DLLExport DateCoords convertToUTCGregorian(DateCoords dateCal);
// Convert a date on a specified calendar to a date on the Julian calendar
DLLExport DateCoords convertToJulian(DateCoords dateCalIn);
// Convert a Gregorian date to a date on a specified calendar
DLLExport DateCoords convertFromUTCGregorian(DateCoords dateUTCGreg, uint32_t calendar);
// Convert a date on any calendar to a date on a specified calendar
DLLExport DateCoords convertToCalendar(DateCoords date, uint32_t calendar);

// Convert a CalCoords on a specified calendar to a CalCoords on the Gregorian calendar
DLLExport CalCoords convertCalToUTCGregorian(CalCoords cc1);
// Convert a CalCoords on a specified calendar to a CalCoords on the Julian calendar
DLLExport CalCoords convertCalToJulian(CalCoords cc1);
// Convert a CalCoords on any calendar to a CalCoords on a specified calendar
DLLExport CalCoords convertCalToCalendar(CalCoords cc1, uint32_t calendar);

// Extract day of year from DateCoords
DLLExport uint32_t dayOfYearFromDate(DateCoords date);

// Create a UTCDatetime from CalCoords
DLLExport UTCDatetime createUTCDatetimeFromCalCoords(CalCoords cc, int8_t precision,
	int8_t uncertainty, uint8_t futureAdjust);


//******** Add RelCalCoords
