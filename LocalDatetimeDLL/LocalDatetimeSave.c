#include "LocalDatetime.h"

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

	Time zone databases generally continue coordinates to
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
		'g' - Greenwich	false		false
		'u' - Universal	false		false
		'z' - Zulu		true		true
	From function stringrule in zic.c:
		DisGMT adjusts time of day by gmtoff
		DisStd adjusts time of day by dstoff */