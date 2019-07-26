#include "UltimateDatetime.h"
//#include <Windows.h> // Necessary to implement DllMain

//BOOL APIENTRY DllMain(
//HANDLE hModule,	// Handle to DLL module
//	DWORD ul_reason_for_call,
//	LPVOID lpReserved ) // Reserved
//{
//	switch ( ul_reason_for_call )
//	{
//		case DLL_PROCESS_ATTACH:
//		// A process is loading the DLL.
//			initLeapSeconds();
//		break;
//		case DLL_THREAD_ATTACH:
//		// A process is creating a new thread.
//		break;
//		case DLL_THREAD_DETACH:
//		// A thread exits normally.
//		break;
//		case DLL_PROCESS_DETACH:
//		// A process unloads the DLL.
//		break;
//	}
//	return TRUE;
//}

// Get number of Tick bits
int getTickBits()
{
#ifdef Ticks64
	return 64;
#else
	return 32;
#endif
}

// Integer exponentiation
int power(int base, unsigned int exp) {
    unsigned int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }

// Multiplication of 32 bit integers
UInt32MultResult uInt32Mult (uint32_t f1, uint32_t f2)
{
	// Multiply 2 @ unsigned 32 bit integers, using only 32 bits
	UInt32MultResult res;
	uint16_t f1L, f1H, f2L, f2H, bL, bH, cL, cH, dL, dH;
	uint32_t a, b, c, d, two16th;

	// Begin by dividing the two factors into 16-bit components
	f1L = Lower16(f1);
	f1H = Upper16(f1);
	f2L = Lower16(f2);
	f2H = Upper16(f2);

	// Result is f1H*f2H*2^32 + (f1L*f2H + f2L*f1H)*2^16 + f1L*f2L

	// Form the partial products
	a = f1H*f2H;
	b = f1L*f2H;
	c = f2L*f1H;
	d = f1L*f2L;

	// Divide b, c and d into 16-bit components
	bL = Lower16(b);
	bH = Upper16(b);
	cL = Lower16(c);
	cH = Upper16(c);
	dL = Lower16(d);
	dH = Upper16(d);

	// Sum the terms that multiply 2^32
	res.high = a + bH + cH;

	// Sum the terms that multiply 2^16
	two16th = bL + cL + dH;

	// The upper 16 bits of two16th contribute to res.high, while
	//	the lower 16 bits contribute to res.low
	res.high += Upper16(two16th);
	res.low = Lower16(two16th) + dL;

	return res;
}

int isEqualTicks64 (TAITicks64 t1, TAITicks64 t2)
{
	return ( (t1.seconds == t2.seconds) && (t1.attoseconds == t2.attoseconds) ? 1 : 0);
}

int isEqualTicks32 (TAITicks32 t1, TAITicks32 t2)
{
	return ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) &&
			 (t1.nanoseconds == t2.nanoseconds) && (t1.attoseconds == t2.attoseconds) ? 1 : 0);
}

int isEqualTicks (TAITicks t1, TAITicks t2)
{
#ifdef Ticks64
	return isEqualTicks64(t1, t2);
#else
	return isEqualTicks32(t1,t2);
#endif
}

int isGreaterTicks64 (TAITicks64 t1, TAITicks64 t2)
{
	return ( (t1.seconds > t2.seconds) || ( (t1.seconds == t2.seconds) && (t1.attoseconds > t2.attoseconds) ) ? 1 : 0);
}

int isGreaterTicks32 (TAITicks32 t1, TAITicks32 t2)
{
	return ( (t1.gigaseconds > t2.gigaseconds) || ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds > t2.seconds) ) ||
			 ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) && (t1.nanoseconds > t2.nanoseconds) ) ||
			 ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
			 (t1.attoseconds > t2.attoseconds) ) ? 1 : 0) ;
}

int isGreaterTicks (TAITicks t1, TAITicks t2)
{
#ifdef Ticks64
	return isGreaterTicks64(t1, t2);
#else
	return isGreaterTicks32(t1,t2);
#endif
}

int isLessTicks64 (TAITicks64 t1, TAITicks64 t2)
{
	return ( (t1.seconds < t2.seconds) || ( (t1.seconds == t2.seconds) && (t1.attoseconds < t2.attoseconds) ) ? 1 : 0);
}

int isLessTicks32 (TAITicks32 t1, TAITicks32 t2)
{
	return ( (t1.gigaseconds < t2.gigaseconds) || ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds < t2.seconds) ) ||
			 ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) && (t1.nanoseconds < t2.nanoseconds) ) ||
			 ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
			 (t1.attoseconds < t2.attoseconds) ) ? 1 : 0) ;
}

int isLessTicks (TAITicks t1, TAITicks t2)
{
#ifdef Ticks64
	return isLessTicks64(t1, t2);
#else
	return isLessTicks32(t1,t2);
#endif
}

int isNotEqualTicks (TAITicks t1, TAITicks t2)
{
	return !isEqualTicks(t1, t2);
}

int isGreaterOrEqualTicks (TAITicks t1, TAITicks t2)
{
	return !isLessTicks (t1, t2);
}

int isLessOrEqualTicks (TAITicks t1, TAITicks t2)
{
	return !isGreaterTicks (t1, t2);
}

int isEqualRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2)
{
	// Works for positive and negative values
	return ( (t1.seconds == t2.seconds) && (t1.attoseconds == t2.attoseconds) ? 1 : 0);
}

int isEqualRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2)
{
	// Works for positive and negative values
	return ( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds == t2.seconds) &&
			 (t1.nanoseconds == t2.nanoseconds) && (t1.attoseconds == t2.attoseconds) ? 1 : 0);
}

int isEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
#ifdef Ticks64
	return isEqualRelTicks64(t1, t2);
#else
	return isEqualRelTicks32(t1,t2);
#endif
}

int isGreaterRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2)
{
	// Need to consider t1 and t2 both positive, both negative, t1 positive / t2 negative and t1 negative / t2 positive
	//	The case where t1 is negative and t2 is positive is handled by default.
	return ( ( (t2.attoseconds >= e18) && (t1.attoseconds < e18) ) || // t1 positive and t2 negative
			 ( (t1.attoseconds < e18) && (t2.attoseconds < e18) && ( (t1.seconds > t2.seconds) || // t1 and t2 positive
				( (t1.seconds == t2.seconds) && (t1.attoseconds > t2.attoseconds) ) ) ) ||
			 ( (t1.attoseconds >= e18) && (t2.attoseconds >= e18) && ( (t1.seconds < t2.seconds) || // t1 and t2 negative
				( (t1.seconds == t2.seconds) && (t1.attoseconds < t2.attoseconds) ) ) ) ? 1 : 0);
}

int isGreaterRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2)
{
	// Need to consider t1 and t2 both positive, both negative, t1 positive / t2 negative and t1 negative / t2 positive
	//	The case where t1 is negative and t2 is positive is handled by default.
	return ( ( (t2.attoseconds >= e9) && (t1.attoseconds < e9) ) || // t1 positive and t2 negative
				// t1 and t2 positive
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds > t2.gigaseconds) || 
				( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds > t2.seconds) ) ) ) ||
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds > t2.nanoseconds) ) ) ||
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
				(t1.attoseconds > t2.attoseconds) ) ) ||
				// t1 and t2 negative
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds < t2.gigaseconds) || 
				( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds < t2.seconds) ) ) ) ||
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds < t2.nanoseconds) ) ) ||
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
				(t1.attoseconds < t2.attoseconds) ) ) ? 1 : 0) ;
}

int isGreaterRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
#ifdef Ticks64
	return isGreaterRelTicks64(t1, t2);
#else
	return isGreaterRelTicks32(t1,t2);
#endif
}

int isLessRelTicks64 (TAIRelTicks64 t1, TAIRelTicks64 t2)
{
	// Need to consider t1 and t2 both positive, both negative, t1 positive / t2 negative and t1 negative / t2 positive
	//	The case where t1 is positive and t2 is negative is handled by default.
	return ( ( (t1.attoseconds >= e18) && (t2.attoseconds < e18) ) || // t1 negative and t2 positive
			 ( (t1.attoseconds >= e18) && (t2.attoseconds >= e18) && ( (t1.seconds > t2.seconds) || // t1 and t2 negative 
				( (t1.seconds == t2.seconds) && (t1.attoseconds > t2.attoseconds) ) ) ) ||
			 ( (t1.attoseconds < e18) && (t2.attoseconds < e18) && ( (t1.seconds < t2.seconds) || // t1 and t2 positive
				( (t1.seconds == t2.seconds) && (t1.attoseconds < t2.attoseconds) ) ) ) ? 1 : 0);
}

int isLessRelTicks32 (TAIRelTicks32 t1, TAIRelTicks32 t2)
{
	// Need to consider t1 and t2 both positive, both negative, t1 positive / t2 negative and t1 negative / t2 positive
	//	The case where t1 is positive and t2 is negative is handled by default.
	return ( ( (t1.attoseconds >= e9) && (t2.attoseconds < e9) ) || // t1 negative and t2 positive
				// t1 and t2 negative
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds > t2.gigaseconds) || 
				( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds > t2.seconds) ) ) ) ||
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds > t2.nanoseconds) ) ) ||
			 ( (t1.attoseconds >= e9) && (t2.attoseconds >= e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
				(t1.attoseconds > t2.attoseconds) ) ) ||
				// t1 and t2 positive
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds < t2.gigaseconds) ||
				( (t1.gigaseconds == t2.gigaseconds) && (t1.seconds < t2.seconds) ) ) ) ||
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds < t2.nanoseconds) ) ) ||
			 ( (t1.attoseconds < e9) && (t2.attoseconds < e9) && ( (t1.gigaseconds == t2.gigaseconds) && 
				(t1.seconds == t2.seconds) && (t1.nanoseconds == t2.nanoseconds) &&
				(t1.attoseconds < t2.attoseconds) ) ) ? 1 : 0) ;
}

int isLessRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
#ifdef Ticks64
	return isLessRelTicks64(t1, t2);
#else
	return isLessRelTicks32(t1,t2);
#endif
}

int isNotEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
	return !isEqualRelTicks(t1, t2);
}

int isGreaterOrEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
	return !isLessRelTicks (t1, t2);
}

int isLessOrEqualRelTicks (TAIRelTicks t1, TAIRelTicks t2)
{
	return !isGreaterRelTicks (t1, t2);
}

// Tick utilities
uint32_t getNanosecond64 (TAITicks64 t1)
{
	return (uint32_t)(t1.attoseconds / e9);
}

uint32_t getNanosecond32 (TAITicks32 t1)
{
	return t1.nanoseconds;
}

uint32_t getNanosecond (TAITicks t1)
{
	/* Between 1-Jan-1958 and 1-Jan-1972, the nanosecond
		value derived from the tick value is not equal to 
		the value specified with the UTC value due to the
		nature of the adjustment.  Make the correction,
		if necessary */
	TAIRelTicks r1, r1Adj;
	if ( isGreaterOrEqualTicks(t1, EarlyAtomic) && isLessTicks(t1, EarlyUTCAdj) )
	{
		// Within the early atomic period
		r1Adj = diffTicks(t1, EarlyAtomic);
		r1 = unAdjustRelTicks(r1Adj, 0);
		t1 = addRelTicksToTicks(EarlyAtomic, r1);
	}
	if ( isGreaterOrEqualTicks(t1, EarlyUTCAdj) && isLessTicks(t1, EarlyUTC2Adj) )
	{
		// Within the early atomic period
		r1Adj = diffTicks(t1, EarlyUTCAdj);
		r1 = unAdjustRelTicks(r1Adj, 1);
		t1 = addRelTicksToTicks(EarlyUTC, r1);
	}
	if ( isGreaterOrEqualTicks(t1, EarlyUTC2Adj) && isLessTicks(t1, LeapSecond) )
	{
		// Within the early atomic period
		r1Adj = diffTicks(t1, EarlyUTC2Adj);
		r1 = unAdjustRelTicks(r1Adj, 2);
		t1 = addRelTicksToTicks(EarlyUTC2, r1);
	}
#ifdef Ticks64
	return getNanosecond64 (t1);
#else
	return getNanosecond32 (t1);
#endif
}

uint32_t getAttosecond64 (TAITicks64 t1)
{
	return (uint32_t)(t1.attoseconds % e9);
}

uint32_t getAttosecond32 (TAITicks32 t1)
{
	return t1.attoseconds;
}

uint32_t getAttosecond (TAITicks t1)
{
	/* Between 1-Jan-1958 and 1-Jan-1972, the nanosecond
		value derived from the tick value is not equal to 
		the value specified with the UTC value due to the
		nature of the adjustment.  However, since the corrections
		are in integral numbers of nanoseconds, the attoseconds
		are unaffected. */
#ifdef Ticks64
	return getAttosecond64 (t1);
#else
	return getAttosecond32 (t1);
#endif
}

// Relative tick utilities
uint32_t getGigaseconds64 (TAIRelTicks64 r1)
{
	return (uint32_t)(r1.seconds / e9);
}

uint32_t getGigaseconds32 (TAIRelTicks32 r1)
{
	return r1.gigaseconds;
}

uint32_t getGigaseconds (TAIRelTicks r1)
{
#ifdef Ticks64
	return getGigaseconds64 (r1);
#else
	return getGigaseconds32 (r1);
#endif
}

uint32_t getSeconds64 (TAIRelTicks64 r1)
{
	return (uint32_t)(r1.seconds % e9);
}

uint32_t getSeconds32 (TAIRelTicks32 r1)
{
	return r1.seconds;
}

uint32_t getSeconds (TAIRelTicks r1)
{
#ifdef Ticks64
	return getSeconds64 (r1);
#else
	return getSeconds32 (r1);
#endif
}

uint32_t getNanoseconds64 (TAIRelTicks64 r1)
{
	return (uint32_t)( (r1.attoseconds % e18) / e9);
}

uint32_t getNanoseconds32 (TAIRelTicks32 r1)
{
	return r1.nanoseconds;
}

uint32_t getNanoseconds (TAIRelTicks r1)
{
#ifdef Ticks64
	return getNanoseconds64 (r1);
#else
	return getNanoseconds32 (r1);
#endif
}

uint32_t getAttoseconds64 (TAIRelTicks64 r1)
{
	return (uint32_t)( (r1.attoseconds % e18) % e9);
}

uint32_t getAttoseconds32 (TAIRelTicks32 r1)
{
	return r1.attoseconds % e9;
}

uint32_t getAttoseconds (TAIRelTicks r1)
{
#ifdef Ticks64
	return getAttoseconds64 (r1);
#else
	return getAttoseconds32 (r1);
#endif
}

double doubleSeconds(TAIRelTicks r1)
{
	// Return the value in seconds of a TAIRelTicks as a double
	double seconds;
	seconds = (double)(getGigaseconds(r1))*1e9 + (double)getSeconds(r1) +
		(double)(getNanoseconds(r1)) / 1e9 + (double)(getAttoseconds(r1)) / 1e18;
	return (isNegativeRelTicks(r1) ? -seconds : seconds);
}

// Difference between two tick values
TAIRelTicks64 diffTicks64 (TAITicks64 t1, TAITicks64 t2)
{
	/* Subtract t2 from t1 and return a TAIRelTicks64 */
	TAIRelTicks64 diff = {0, 0}; // Initialize to 0
	/* Use a signed value for the attosecond difference to simplify subtraction.
		A signed value can be used, since the maximum value is 1e18 - 1 and the range
		of a signed 64 bit int is -9e18 to +9e18
	*/
	int64_t attodiff;
	// If t1 == t2, return 0
	if (isEqualTicks64(t1, t2)) return diff;

	// If t1 > t2
	if (isGreaterTicks64(t1, t2))
	{
		attodiff = t1.attoseconds - t2.attoseconds;
		if (attodiff >= 0)
		{
			diff.attoseconds = attodiff;
			diff.seconds = t1.seconds - t2.seconds;
		}
		else
		{
			diff.attoseconds = e18 + attodiff;
			diff.seconds = t1.seconds - t2.seconds - 1;
		}
		return diff;
	}
	else
	{
		attodiff = t2.attoseconds - t1.attoseconds;
		if (attodiff >= 0)
		{
			diff.attoseconds = attodiff;
			diff.seconds = t2.seconds - t1.seconds;
		}
		else
		{
			diff.attoseconds = e18 + attodiff;
			diff.seconds = t2.seconds - t1.seconds - 1;
		}
		/* A negative relative value is denoted by adding 1*e18 to the attoseconds value.
			This saves space, as another byte is not required for the sign.  Since the maximum
			value of attoseconds is 1*e18 - 1, and the maximum value of a 64-bit unsigned integer
			is 1.8e19, there is enough headroom to add 1*e18.  The actual value can be recovered
			by executing a mod function. */
		diff.attoseconds += e18;
		return diff;
	}
}

TAIRelTicks32 diffTicks32 (TAITicks32 t1, TAITicks32 t2)
{
	/* Subtract t2 from t1 and return a TAIRelTicks32 */
	TAIRelTicks32 diff = {0, 0, 0, 0}; // Initialize to 0
	/* Use a signed value for the attosecond, nanosecond and second differences to simplify subtraction.
		A signed value can be used, since the maximum value is 1e9 - 1 and the range
		of a signed 32 bit int is -2.1e9 to +2.1e9
	*/
	int32_t attodiff, nanodiff, secdiff;
	// If t1 == t2, return 0
	if (isEqualTicks32(t1, t2)) return diff;

	// If t1 > t2
	if (isGreaterTicks32(t1, t2))
	{
		attodiff = t1.attoseconds - t2.attoseconds;
		if (attodiff >= 0)
		{ // Positive atodiff
			diff.attoseconds = attodiff;
			nanodiff = t1.nanoseconds - t2.nanoseconds;
			if (nanodiff >= 0)
			{ // Positive nanodiff
				diff.nanoseconds = nanodiff;
				secdiff = t1.seconds - t2.seconds;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds - 1;
				}
			}
			else
			{ // Negative nanodiff, borrow from seconds
				diff.nanoseconds = e9 + nanodiff;
				secdiff = t1.seconds - t2.seconds - 1;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds - 1;
				}
			}
		}
		else
		{ // Negative atodiff, borrow from nanodiff
			diff.attoseconds = e9 + attodiff;
			nanodiff = t1.nanoseconds - t2.nanoseconds - 1;
			if (nanodiff >= 0)
			{ // Positive nanodiff
				diff.nanoseconds = nanodiff;
				secdiff = t1.seconds - t2.seconds;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds - 1;
				}
			}
			else
			{ // Negative nanodiff, borrow from seconds
				diff.nanoseconds = e9 + nanodiff;
				secdiff = t1.seconds - t2.seconds - 1;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t1.gigaseconds - t2.gigaseconds - 1;
				}
			}
		}
		return diff;
	}
	else
	{ // t1 < t2
		attodiff = t2.attoseconds - t1.attoseconds;
		if (attodiff >= 0)
		{ // Positive atodiff
			diff.attoseconds = attodiff;
			nanodiff = t2.nanoseconds - t1.nanoseconds;
			if (nanodiff >= 0)
			{ // Positive nanodiff
				diff.nanoseconds = nanodiff;
				secdiff = t2.seconds - t1.seconds;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds - 1;
				}
			}
			else
			{ // Negative nanodiff, borrow from seconds
				diff.nanoseconds = e9 + nanodiff;
				secdiff = t2.seconds - t1.seconds - 1;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds - 1;
				}
			}
		}
		else
		{ // Negative atodiff, borrow from nanodiff
			diff.attoseconds = e9 + attodiff;
			nanodiff = t2.nanoseconds - t1.nanoseconds - 1;
			if (nanodiff >= 0)
			{ // Positive nanodiff
				diff.nanoseconds = nanodiff;
				secdiff = t2.seconds - t1.seconds;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds - 1;
				}
			}
			else
			{ // Negative nanodiff, borrow from seconds
				diff.nanoseconds = e9 + nanodiff;
				secdiff = t2.seconds - t1.seconds - 1;
				if (secdiff >=0)
				{ // Positive secdiff
					diff.seconds = secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds;
				}
				else
				{ // Negative secdiff, borrow from gigaseconds
					diff.seconds = e9 + secdiff;
					diff.gigaseconds = t2.gigaseconds - t1.gigaseconds - 1;
				}
			}
		}
		/* A negative relative value is denoted by adding 1*e9 to the attoseconds value.
			This saves space, as another byte is not required for the sign.  Since the maximum
			value of attoseconds is 1*e9 - 1, and the maximum value of a 32-bit unsigned integer
			is 4.2e9, there is enough headroom to add 1*e9.  The actual value can be recovered
			by executing a mod function. */
		diff.attoseconds += e9;
		return diff;
	}
}

TAIRelTicks diffTicks (TAITicks t1, TAITicks t2)
{
#ifdef Ticks64
	return diffTicks64(t1, t2);
#else
	return diffTicks32(t1,t2);
#endif
}

int rightTrim (char * str)
{
	/* Trims the trailing blanks from a string by substituting nulls for blanks.
	    then returns the number of blanks removed. */
	int iter;
	int numRemoved = 0;
	// str[strlen(str)] is already null
	for (iter = (int)strlen(str) - 1; iter >= 0; iter--)
	{
		if (str[iter] == ' ')
		{
			str[iter] = '\0';
			numRemoved++;
		}
		else
		{// First non-blank character has been found
			break;
		}
	}
	return numRemoved;
}

char * underscoreToBlank (char str[])
{
	/* Replace underscores with blanks 
		Some of the test results have strings with blanks.  However, blank
		is a delimiter for scanf, so when such results are specifed in
		the input file, underscores are used instead of blanks. */
	uint32_t iter;
	int numUnderscores = 0;

	for (iter = 0; iter < strlen(str); iter++)
	{
		if (str[iter] == '_')
		{
			numUnderscores ++;
			str[iter] = ' ';
		}
	}
	return str;
}

int isNegativeRelTicks64(TAIRelTicks64 r1)
{
	return r1.attoseconds >= e18;
}

int isNegativeRelTicks32(TAIRelTicks32 r1)
{
	return r1.attoseconds >= e9;
}

int isNegativeRelTicks(TAIRelTicks r1)
{
#ifdef Ticks64
	return isNegativeRelTicks64(r1);
#else
	return isNegativeRelTicks32(r1);
#endif
}

TAIRelTicks smallIntMultTAIRelTicks(uint32_t mult, TAIRelTicks r1)
{
	// Multiply a relative tick value by 1, 2, 3 or 4.  
	//	These values were selected, since the seconds, nanoseconds and attoseconds fields
	//	all must be less than 1e9, so will not overflow when multiplied by these numbers.
	//
	// In the event of an overflow, the maximum possible value plus a tick is returned.
	//
	uint32_t atto1, nano1, sec1, giga1, atto2, nano2, sec2, giga2, carry;
	TAIRelTicks r2;

	// Check that mult is within range
	if (mult < 1 || mult > 4) return createRelTicks(0,0,0,0,0);
	if (mult == 1) return r1;

	atto1 = getAttoseconds(r1);
	nano1 = getNanoseconds(r1);
	sec1 = getSeconds(r1);
	giga1 = getGigaseconds(r1);

	// First multiply each element
	atto2 = mult*atto1;
	nano2 = mult*nano1;
	sec2 = mult*sec1;

	// Check for overflow of gigaseconds
	if (giga1 > uint32Max / mult)
	{
		// Multiplication would cause overflow.  Return max allowed value plus a tick.
		return (isNegativeRelTicks(r1) ? negateRelTicks(MaxRelTimePlus) : MaxRelTimePlus);
	}
	giga2 = mult*giga1;

	// Perform carry calculation
	carry = atto2 / e9;
	nano2 += carry;
	atto2 -= carry*e9;

	carry = nano2 / e9;
	sec2 += carry;
	nano2 -= carry*e9;

	carry = sec2 / e9;

	// Giga2 can overflow on carry
	if (giga2 > uint32Max - carry)
	{
		// Carry would cause an overflow.  Return the max value with the sign switched
		//	to indicate the overflow
		return (isNegativeRelTicks(r1) ? negateRelTicks(MaxRelTimePlus) : MaxRelTimePlus);
	}

	giga2 += carry;
	sec2 -= carry*e9;

	r2 = createRelTicks(giga2, sec2, nano2, atto2, isNegativeRelTicks(r1));

	// Even if the value doesn't overflow, might still be greater than the allowed max
	if (isGreaterRelTicks(r2, MaxRelTime) || isLessRelTicks(r2, negateRelTicks(MaxRelTime)))
		return (isNegativeRelTicks(r1) ? negateRelTicks(MaxRelTimePlus) : MaxRelTimePlus);

	return r2;
}

TAIRelTicks intMultTAIRelTicks(int32_t mult, TAIRelTicks r1)
{
	// Multiply a relative tick value by a 32 bit signed integer.  If the
	//	calculation overflows, return the maximum value with the wrong sign.
	TAIRelTicks r2, r3;
	TAIRelTicks powersOf4[16]; // Need to support up to 4^15 to construct a value up to 2^31 - 1
	int ipower = 0;
	uint32_t fact;
	double gigasecs;

	int isNegativeMult = mult < 0;
	int isNegativeResult;
	uint32_t amult;

	// Determine the sign of the product
	isNegativeResult = ( (isNegativeMult && !isNegativeRelTicks(r1)) || (!isNegativeMult && isNegativeRelTicks(r1)));

	// Perform a conservative check for greater than allowed max using double precision values
	amult = abs(mult);
	gigasecs = (double)(getGigaseconds(r1)) + (double)(getSeconds(r1))/1e9 + (double)(getNanoseconds(r1))/1e18 +
					(double)(getAttoseconds(r1))/1e27;
	if ((double)amult * gigasecs > dMaxRelTimeGiga)
	{
		// The multiplication will produce a value greater than the max.  Set to maximum value plus a tick.
		return (isNegativeResult ? negateRelTicks(MaxRelTimePlus) : MaxRelTimePlus);
	}

	// We can multiply a relative tick value by 2, 3, or 4 by using
	//	the smallIntMult function.
	//
	// Perform the multiplication by dividing mult into the sum
	//	of powers of 4, each multiplied by 1, 2 or 3.
	// For example, to multiply by 109, do the following:
	//	1. Multiply by 64 - leaves 45
	//  2. Multiply by 16, then by 2, and add to the above - leaves 13
	//  3. Multiply by 4, then by 3, and add to the above - leaves 1
	//  4. Add 1 times the amount
	//

	// Create as many powers of 4 as necessary
	fact = amult;
	powersOf4[0] = r1;
	while (fact > 4)
	{
		fact /= 4;
		powersOf4[ipower+1] = smallIntMultTAIRelTicks(4, powersOf4[ipower]);
		ipower++;
	}

	// Build up the product starting with the highest power of 4
	r2 = smallIntMultTAIRelTicks(fact, powersOf4[ipower]);
	// Check for overflow
	if (isEqualRelTicks(r2, MaxRelTimePlus) || isEqualRelTicks(r2, negateRelTicks(MaxRelTimePlus)))
	{
		// Overflow on multiply
		return r2;
	}
	amult -= fact*power(4,ipower);
	while(ipower > 0)
	{
		// Calculate the remaining value
		ipower--;
		fact = amult / power(4,ipower);
		if (fact > 0)
		{
			r3 = smallIntMultTAIRelTicks(fact, powersOf4[ipower]);
			// Check for overflow
			if (isEqualRelTicks(r3, MaxRelTimePlus) || isEqualRelTicks(r3, negateRelTicks(MaxRelTimePlus)))
			{
				// Overflow on multiply
				return r3;
			}
			r2 = addRelTicks(r2, r3);
			// Check for overflow
			if (isEqualRelTicks(r2, MaxRelTimePlus) || isEqualRelTicks(r2, negateRelTicks(MaxRelTimePlus)))
			{
				// Overflow on add
				return r2;
			}
			amult -= fact*power(4,ipower);
		}
	}
	// Even if the value doesn't overflow, might still be greater than the allowed max
	if (isGreaterRelTicks(r2, MaxRelTime) || isLessRelTicks(r2, negateRelTicks(MaxRelTime)))
		return (isNegativeResult ? negateRelTicks(MaxRelTimePlus) : MaxRelTimePlus);

	return r2;
}

// Add relative tick to tick and return relative tick
TAITicks64 addRelTicksToTicks64(TAITicks64 t1, TAIRelTicks64 r1)
{
	// Add a relative tick value to a tick value and return a tick value
	TAITicks64 t2, tr;
	TAIRelTicks64 rt;
	//  Different treatment for positive and negative relative values
	if (r1.attoseconds < e18) // Value >= e18 denotes negative value
	{
		// Relative value is positive
		t2.attoseconds = t1.attoseconds + r1.attoseconds;
		t2.seconds = t1.seconds + r1.seconds;
		// Check for overflow
		//  Need to check for a "32 bit" overflow.  I.e., as rt.seconds is 64 bits,
		//	  there will not be a true overflow until the value exceeds 1.84e19.  However,
		//	  this corresponds to 18.4 gigaseconds, which exceeds the 32 bit value that
		//	  will be returned by getgigaseconds64.
		//
		if ( (t2.seconds < t1.seconds) || (t2.seconds < r1.seconds) ||
			 (t2.seconds / e9 > uint32Max) )
		{
			// There is an overflow.  Set to the maximum value plus a tick
			return (EndOfTimePlus64);
		}
		// Check for carry
		if (t2.attoseconds >= e18)
		{
			t2.attoseconds -= e18;
			t2.seconds ++;
			// Check for overflow
			//  When adding 1, if there is an overflow, the sum is zero
			if ( (t2.seconds == 0) || (t2.seconds / e9 > uint32Max) )
			{
				// There is an overflow.  Set to the maximum value plus a tick
				return (EndOfTimePlus64);
			}
		}
		// Even if the value doesn't overflow, might still be greater than the allowed max
		if (isGreaterTicks64(t2, EndOfTime64)) return (EndOfTimePlus64);
	}
	else
	{
		// Relative value is negative
		//  Construct a tick value from the relative value
		tr.seconds = r1.seconds;
		tr.attoseconds= r1.attoseconds%e18;
		// First make sure the absolute value of the relative value is <= the ticks value.
		if (isGreaterTicks64(tr, t1))
		{
			// Cannot have negative tick value, so set to zero
			t2.seconds = 0;
			t2.attoseconds = 0;
		}
		else
		{
			// Perform a subtraction using diffTicks64
			rt = diffTicks64(t1, tr);
			// Create t2 from rt.  No need to check for negative value for rt
			t2.seconds = rt.seconds;
			t2.attoseconds = rt.attoseconds;
		}
	}
	return t2;
}

TAITicks32 addRelTicksToTicks32(TAITicks32 t1, TAIRelTicks32 r1)
{
	// Add a relative tick value to a tick value and return a tick value
	TAITicks32 t2, tr;
	TAIRelTicks32 rt;
	//  Different treatment for positive and negative relative values
	if (r1.attoseconds < e9) // Value >= e9 denotes negative value
	{
		// Relative value is positive
		t2.attoseconds = t1.attoseconds + r1.attoseconds;
		t2.nanoseconds = t1.nanoseconds + r1.nanoseconds;
		// Check for carry
		if (t2.attoseconds >= e9)
		{
			t2.attoseconds -= e9;
			t2.nanoseconds ++;
		}
		t2.seconds = t1.seconds + r1.seconds;
		// Check for carry
		if (t2.nanoseconds >= e9)
		{
			t2.nanoseconds -= e9;
			t2.seconds ++;
		}
		t2.gigaseconds = t1.gigaseconds + r1.gigaseconds;
		// Check for overflow
		if ( (t2.gigaseconds < t1.gigaseconds) || (t2.gigaseconds < r1.gigaseconds) )
		{
			// There is an overflow.  Set to the maximum value plus a tick
			return (EndOfTimePlus32);
		}
		// Check for carry
		if (t2.seconds >= e9)
		{
			t2.seconds -= e9;
			t2.gigaseconds ++;
			//  When adding 1, if there is an overflow, the sum is zero
			if  (t2.gigaseconds == 0) 
			{
				// There is an overflow.  Set to the maximum value plus a tick
				return (EndOfTimePlus32);
			}
		}
		// Even if the value doesn't overflow, might still be greater than the allowed max
		if (isGreaterTicks32(t2, EndOfTime32)) return (EndOfTimePlus32);
	}
	else
	{
		// Relative value is negative
		//  Construct a tick value from the relative value
		tr.gigaseconds = r1.gigaseconds;
		tr.seconds = r1.seconds;
		tr.nanoseconds = r1.nanoseconds;
		tr.attoseconds= r1.attoseconds%e9;
		// First make sure the absolute value of the relative value is <= the ticks value.
		if (isGreaterTicks32(tr, t1))
		{
			// Cannot have negative tick value, so set to zero
			t2.gigaseconds = 0;
			t2.seconds = 0;
			t2.nanoseconds = 0;
			t2.attoseconds = 0;
		}
		else
		{
			// Perform a subtraction using diffTicks32
			rt = diffTicks32(t1, tr);
			// Create t2 from rt.  No need to check for negative value for rt
			t2.gigaseconds = rt.gigaseconds;
			t2.seconds = rt.seconds;
			t2.nanoseconds = rt.nanoseconds;
			t2.attoseconds = rt.attoseconds;
		}
	}
	return t2;
}

TAITicks addRelTicksToTicks(TAITicks t1, TAIRelTicks r1)
{
	TAITicks t2;
#ifdef Ticks64
	t2 = addRelTicksToTicks64(t1, r1);
#else
	t2 = addRelTicksToTicks32(t1, r1);
#endif
	return t2;
}

// Add relative tick to relative tick and return relative tick
TAIRelTicks64 addRelTicks64(TAIRelTicks64 r1, TAIRelTicks64 r2)
{
	// Add a relative tick value to a relative tick value and return a relative tick value
	TAIRelTicks64 rt;
	TAITicks64 t1, t2;
	//  Different treatment when both relative values have the same sign vs differeing signs
	if ( (r1.attoseconds < e18 && r2.attoseconds < e18) ||  
			(r1.attoseconds >= e18 && r2.attoseconds >= e18) ) // Value >= e18 denotes negative value
	{
		// Both have the same sign
		rt.attoseconds = r1.attoseconds % e18 + r2.attoseconds % e18;
		rt.seconds = r1.seconds + r2.seconds;

		// Check for overflow
		//  Need to check for a "32 bit" overflow.  I.e., as rt.seconds is 64 bits,
		//	  there will not be a true overflow until the value exceeds 1.84e19.  However,
		//	  this corresponds to 18.4 gigaseconds, which exceeds the 32 bit value that
		//	  will be returned by getgigaseconds64.
		//
		if ( (rt.seconds < r1.seconds) || (rt.seconds < r2.seconds) ||
			 (rt.seconds / e9 > uint32Max) )
		{
			// There is an overflow.  Set to the maximum value plus a tick
			return (r1.attoseconds < e18 ? MaxRelTimePlus64 : negateRelTicks64(MaxRelTimePlus64));
		}

		// Check for carry
		if (rt.attoseconds >= e18)
		{
			rt.attoseconds -= e18;
			rt.seconds ++;
			// Check for overflow
			//  When adding 1, if there is an overflow, the sum is zero
			if ( (rt.seconds == 0) || (rt.seconds / e9 > uint32Max) )
			{
				// There is an overflow.  Set to the maximum value plus a tick
				return (r1.attoseconds < e18 ? MaxRelTimePlus64 : negateRelTicks64(MaxRelTimePlus64));
			}
		}

		// Even if the value doesn't overflow, might still be greater than the allowed max
		if (isGreaterRelTicks64(rt, MaxRelTime64) || isLessRelTicks64(rt, negateRelTicks64(MaxRelTime64)))
			return (r1.attoseconds < e18 ? MaxRelTimePlus64 : negateRelTicks64(MaxRelTimePlus64));

		// Set the sign
		if (r1.attoseconds >= e18) rt.attoseconds += e18;
		return rt;
	}
	else
	{
		// Values have different signs
		//  Construct tick values from the relative values
		t1.seconds = r1.seconds;
		t1.attoseconds = r1.attoseconds%e18;
		t2.seconds = r2.seconds;
		t2.attoseconds = r2.attoseconds%e18;
		// Perform a subtraction using diffTicks64.
		if (isNegativeRelTicks64(r1))
		{
			rt = diffTicks64(t2, t1);
		}
		else
		{
			rt = diffTicks64(t1, t2);
		}
	}
	return rt;
}

TAIRelTicks32 addRelTicks32(TAIRelTicks32 r1, TAIRelTicks32 r2)
{
	// Add a relative tick value to a relative tick value and return a relative tick value
	TAIRelTicks32 rt;
	TAITicks32 t1, t2;
	//  Different treatment when both relative values have the same sign vs differeing signs
	if ( (r1.attoseconds < e9 && r2.attoseconds < e9) ||  
			(r1.attoseconds >= e9 && r2.attoseconds >= e9) ) // Value >= e9 denotes negative value
	{
		// Both have the same sign
		rt.attoseconds = r1.attoseconds % e9 + r2.attoseconds % e9;
		rt.nanoseconds = r1.nanoseconds + r2.nanoseconds;
		// Check for carry
		if (rt.attoseconds >= e9)
		{
			rt.attoseconds -= e9;
			rt.nanoseconds ++;
		}
		rt.seconds = r1.seconds + r2.seconds;
		// Check for carry
		if (rt.nanoseconds >= e9)
		{
			rt.nanoseconds -= e9;
			rt.seconds ++;
		}
		rt.gigaseconds = r1.gigaseconds + r2.gigaseconds;

		// Check for overflow
		//  When adding, if there is an overflow, the sum is less than one of the terms
		if ( (rt.gigaseconds < r1.gigaseconds) || (rt.gigaseconds < r2.gigaseconds) )
		{
			// There is an overflow.  Set to the maximum value plus a tick
			return (r1.attoseconds < e9 ? MaxRelTimePlus32 : negateRelTicks32(MaxRelTimePlus32));
		}

		// Check for carry
		if (rt.seconds >= e9)
		{
			rt.seconds -= e9;
			rt.gigaseconds ++;
			// Check for overflow
			//  When adding 1, if there is an overflow, the sum is zero
			if (rt.gigaseconds == 0)
			{
				// There is an overflow.  Set to the maximum value and flip the sign
				return (r1.attoseconds < e9 ? MaxRelTimePlus32 : negateRelTicks32(MaxRelTimePlus32));
			}
		}

		// Even if the value doesn't overflow, might still be greater than the allowed max
		if (isGreaterRelTicks32(rt, MaxRelTime32) || isLessRelTicks32(rt, negateRelTicks32(MaxRelTime32)))
			return (r1.attoseconds < e9 ? MaxRelTimePlus32 : negateRelTicks32(MaxRelTimePlus32));

		// Set the sign
		if (r1.attoseconds >= e9) rt.attoseconds += e9;
		return rt;
	}
	else
	{
		// Values have different signs
		//  Construct tick values from the relative values
		t1.gigaseconds = r1.gigaseconds;
		t1.seconds = r1.seconds;
		t1.nanoseconds = r1.nanoseconds;
		t1.attoseconds = r1.attoseconds%e9;

		t2.gigaseconds = r2.gigaseconds;
		t2.seconds = r2.seconds;
		t2.nanoseconds = r2.nanoseconds;
		t2.attoseconds = r2.attoseconds%e9;
		// Perform a subtraction using diffTicks32.
		if (isNegativeRelTicks32(r1))
		{
			rt = diffTicks32(t2, t1);
		}
		else
		{
			rt = diffTicks32(t1, t2);
		}
	}
	return rt;
}

TAIRelTicks addRelTicks(TAIRelTicks r1, TAIRelTicks r2)
{
	TAIRelTicks r3;
#ifdef Ticks64
	r3 = addRelTicks64(r1, r2);
#else
	r3 = addRelTicks32(r1, r2);
#endif
	return r3;
}

// Subtract relative tick from relative tick and return relative tick
TAIRelTicks64 subtractRelTicks64(TAIRelTicks64 r1, TAIRelTicks64 r2)
{
	// Subtract by adding the negative
	TAIRelTicks64 r3, r4;
	r3 = negateRelTicks64(r2);
	r4 = addRelTicks64(r1, r3);
	return r4;
}

TAIRelTicks32 subtractRelTicks32(TAIRelTicks32 r1, TAIRelTicks32 r2)
{
	// Subtract by adding the negative
	TAIRelTicks32 r3, r4;
	r3 = negateRelTicks32(r2);
	r4 = addRelTicks32(r1, r3);
	return r4;
}

TAIRelTicks subtractRelTicks(TAIRelTicks r1, TAIRelTicks r2)
{
	TAIRelTicks r3;
#ifdef Ticks64
	r3 = subtractRelTicks64(r1, r2);
#else
	r3 = subtractRelTicks32(r1, r2);
#endif
	return r3;
}

TAIRelTicks64 negateRelTicks64(TAIRelTicks64 r1)
{
	TAIRelTicks64 r2;
	r2.seconds = r1.seconds;
	// Set the value of r2.attoseconds appropriately
	if (r1.attoseconds < e18)
	{
		// Value is positive; make it negative
		r2.attoseconds = r1.attoseconds += e18;
	}
	else
	{
		// Value is negative; make it positive
		r2.attoseconds = r1.attoseconds -= e18;
	}
	return r2;
}

TAIRelTicks32 negateRelTicks32(TAIRelTicks32 r1)
{
	TAIRelTicks32 r2;
	r2.gigaseconds = r1.gigaseconds;
	r2.seconds = r1.seconds;
	r2.nanoseconds = r1.nanoseconds;
	// Set the value of r2.attoseconds appropriately
	if (r1.attoseconds < e9)
	{
		// Value is positive; make it negative
		r2.attoseconds = r1.attoseconds += e9;
	}
	else
	{
		// Value is negative; make it positive
		r2.attoseconds = r1.attoseconds -= e9;
	}
	return r2;
}

TAIRelTicks negateRelTicks(TAIRelTicks r1)
{
	TAIRelTicks r2;
#ifdef Ticks64
	r2 = negateRelTicks64(r1);
#else
	r2 = negateRelTicks32(r1);
#endif
	return r2;
}

TAITicks64 subtractRelTicksFromTicks64(TAITicks64 t1, TAIRelTicks64 r1)
{
	// Subtract by adding the negative
	TAITicks64 t2;
	TAIRelTicks64 r2;
	r2 = negateRelTicks64(r1);
	t2 = addRelTicksToTicks64(t1, r2);
	return t2;
}

TAITicks32 subtractRelTicksFromTicks32(TAITicks32 t1, TAIRelTicks32 r1)
{
	// Subtract by adding the negative
	TAITicks32 t2;
	TAIRelTicks32 r2;
	r2 = negateRelTicks32(r1);
	t2 = addRelTicksToTicks32(t1, r2);
	return t2;
}

TAITicks subtractRelTicksFromTicks(TAITicks t1, TAIRelTicks r1)
{
	TAITicks t2;
#ifdef Ticks64
	t2 = subtractRelTicksFromTicks64(t1, r1);
#else
	t2 = subtractRelTicksFromTicks32(t1, r1);
#endif
	return t2;
}

const char * asStringTAITicks64 (TAITicks64 t1)
{
	// Format a TAITicks as xxxxxxx.yyyyyy seconds
	static char buffer [40];
	char tempbuf [19];
	int n, m, ibuf;
	memset(buffer, ' ', sizeof(buffer) - 1);
	memset(tempbuf, ' ', sizeof(tempbuf) - 1);
	// Format the left side of the decimal point and fill the right side with zeroes
	n = sprintf_s (buffer, 40, "%llu.000000000000000000", t1.seconds);
	// Format the right side of the decimal point
	m = sprintf_s (tempbuf, 19, "%llu", t1.attoseconds);
	// Copy the right side into the buffer
	ibuf = n - m;
	memcpy (buffer + ibuf, tempbuf, m);
	ibuf = rightTrim(buffer);
	return buffer;
}

const char * asStringTAITicks32 (TAITicks32 t1)
{
	// Format a TAITicks as xxxxxxx.yyyyyy seconds
	static char buffer [40];
	char tempbuf [19];
	int n, m, ibuf;
	memset(buffer, ' ', sizeof(buffer) - 1);
	memset(tempbuf, ' ', sizeof(tempbuf) - 1);
	ibuf = 0;
	if (t1.gigaseconds != 0)
	{
		// Format the gigaseconds, padded to the left with blanks
		m = sprintf_s (tempbuf, 19, "%u", t1.gigaseconds);
		// Copy the gigaseconds into the buffer
		memcpy (buffer, tempbuf, m);
	}
	else
	{
		m = 0;
	}
	// Format the seconds
	n = sprintf_s (tempbuf, 19, "%u", t1.seconds);
	// Pad with zeroes if gigaseconds are nonzero
	ibuf += m;
	if (t1.gigaseconds != 0)
	{
		memset (buffer+ibuf, '0', 9-n);
		memcpy (buffer+ibuf+9-n, tempbuf, n);
		ibuf += 9;
	}
	else
	{
		memcpy (buffer+ibuf, tempbuf, n);
		ibuf += n;
	}
	// Insert the decimal point
	buffer[ibuf] = '.';
	ibuf += 1;
	// Format the nanoseconds
	n = sprintf_s (tempbuf, 19, "%u", t1.nanoseconds);
	// Pad with zeroes
	memset (buffer+ibuf, '0', 9-n);
	memcpy (buffer+ibuf+9-n, tempbuf, n);
	ibuf += 9;
	// Format the attoseconds
	n = sprintf_s (tempbuf, 19, "%u", t1.attoseconds);
	// Pad with zeroes
	memset (buffer+ibuf, '0', 9-n);
	memcpy (buffer+ibuf+9-n, tempbuf, n);
	ibuf = rightTrim(buffer);
	return buffer;
}

const char * asStringTAITicks (TAITicks t1)
{
	// Format a TAITicks as xxxxxxx.yyyyyy seconds
#ifdef Ticks64
	return asStringTAITicks64(t1);
#else
	return asStringTAITicks32(t1);
#endif
}


const char * asStringTAIRelTicks64 (TAIRelTicks64 r1)
{
	// Format a TAIRelTicks as -xxxxx.yyyyy seconds
	// Create a TAITicks to enable the utilization of asStringTAITicks
	TAITicks64 t1;
	static char buffer[41];
	int ioff;
	t1.seconds = r1.seconds;
	t1.attoseconds = r1.attoseconds%e18;
	memset(buffer, ' ', sizeof(buffer) - 1); // Initialize to blank
	// Insert a negative sign if necessary
	if (r1.attoseconds >= e18)
	{
		ioff = 1;
		buffer[0] = '-';
	}
	else
	{
		ioff = 0;
	}
	memcpy(buffer+ioff, asStringTAITicks64(t1), sizeof(buffer) - 1);
	ioff = rightTrim(buffer);
	return buffer;
}

const char * asStringTAIRelTicks32 (TAIRelTicks32 r1)
{
	// Format a TAIRelTicks as -xxxxx.yyyyy seconds
	// Create a TAITicks to enable the utilization of asStringTAITicks
	TAITicks32 t1;
	static char buffer[41];
	int ioff;
	t1.gigaseconds = r1.gigaseconds;
	t1.seconds = r1.seconds;
	t1.nanoseconds = r1.nanoseconds;
	t1.attoseconds = r1.attoseconds%e9;
	memset(buffer, ' ', sizeof(buffer) - 1); // Initialize to blank
	// Insert a negative sign if necessary
	if (r1.attoseconds >= e9)
	{
		ioff = 1;
		buffer[0] = '-';
	}
	else
	{
		ioff = 0;
	}
	memcpy(buffer+ioff, asStringTAITicks32(t1), sizeof(buffer) - 1);
	ioff = rightTrim(buffer);
	return buffer;
}

const char * asStringTAIRelTicks (TAIRelTicks r1)
{
	// Format a TAIRelTicks as -xxxxx.yyyyy seconds
	// Create a TAITicks to enable the utilization of asStringTAITicks
#ifdef Ticks64
	return asStringTAIRelTicks64(r1);
#else
	return asStringTAIRelTicks32(r1);
#endif
}

const char * asStringUncertainDouble(UncertainDouble u1)
{
	// Format an UncertainDouble in either fixed or scientific notation
	//	depending on the value and precision
	//	-3,124,347.23 +/- 0.04
	//	1.378 x 10^17 +/- 8 x 10^15
	// The maximum length of each is 31, so the maximum total length is 67
	//
	//	Assumes precision and uncertainty are valid, which can be ensured by
	//	  only creating uncertainDoubles via createUncertainDouble
	//
	static char buffer[68];
	char * valbuf, * uncbuf;
	double value, precision, uncertainty;
	int lenval, lenunc, i, len;
	//
	// Clear the strings
	//
	len = sizeof(buffer);
	memset(buffer, ' ', len - 1);
	//
	value = u1.value;
	precision = u1.precision;
	uncertainty = u1.uncertainty * u1.precision;
	//
	//	Format the value
	//
	valbuf = sprintfDoubleComma(value, precision, 1);
	//
	//	Copy the value into the buffer
	//
	lenval = (int)strlen(valbuf);
	for (i = 0; i < lenval; i++) buffer[i] = valbuf[i];
	//
	//	Add uncertainty if non-zero
	//
	lenunc = 0;
	if (uncertainty != 0.)
	{
		//
		//	Format the uncertainty
		// 
		uncbuf = sprintfDoubleComma(uncertainty, precision, 0);
		buffer[lenval] = ' ';
		buffer[lenval + 1] = '+';
		buffer[lenval + 2] = '/';
		buffer[lenval + 3] = '-';
		buffer[lenval + 4] = ' ';
		lenval += 5;
		lenunc = (int)strlen(uncbuf);
		for (i = 0; i < lenunc; i++) buffer[lenval + i] = uncbuf[i];
	}
	// Null terminate the string
	buffer[lenval + lenunc] = 0;
	return buffer;
}

const char * asStringDateCoords(DateCoords y1, char stringDate[])
{
	//
	//	Format a DateCoords
	//
	// * stringUTC must be dimensioned at least 130 
	// Use asStringDatetime
	//
	// Call asStringDatetime, setting frame, precision, uncertainty, time zone and BorA
	//	consistent with DateCoords
	//
	return asStringDatetime(y1.gigayear, y1.year, y1.month, y1.dayOfMonth, 0, 0, 0, 0, 0,
	y1.calendar, 2, 99, 0, "", 2, stringDate);
}
const char * asStringTimeCoords(TimeCoords u1)
{
	// Format a TimeCoords
	//
	// Size is: 12:34:56.123456789 123456789, which is 28 characters
	//
	static char buffer[29];
	int len, n;
	len = sizeof(buffer);
	n = sprintf_s(buffer, len, "%02d:%02d:%02d.%09d %09d", u1.hour, u1.minute, u1.second,
		u1.nanosecond, u1.attosecond);
	return buffer;
}

const char * asStringCalCoords(CalCoords cc, char stringCal[])
{
	// Format a CalCoords as a readable string
	//
	// * stringUTC must be dimensioned at least 130 
	// Use asStringDatetime
	//
	// Call asStringDatetime, setting frame, precision and uncertainty consistent with CalCoords
	//
	return asStringDatetime(cc.date.gigayear, cc.date.year, cc.date.month, cc.date.dayOfMonth,
		cc.time.hour, cc.time.minute, cc.time.second, cc.time.nanosecond, cc.time.attosecond,
		cc.date.calendar, 0, 99, 0, "", 2, stringCal);
}

char * sprintfComma (int32_t n)
{
	// Format a 32 bit integer with commas
	//  Max size is 14 bytes: -2,123,456,789
    int n2 = 0;
    int scale = 1;
	int m;
	int loc = 0;
	static char buffer[15];
	int len = sizeof(buffer);
	// Clear the string
	memset(buffer, ' ', len - 1);
	// Deal with negative values
    if (n < 0) {
        m = sprintf_s(buffer, 15, "-");
        n = -n;
		loc+=m;
    }
    while (n >= 1000) {
        n2 = n2 + scale * (n % 1000);
        n /= 1000;
        scale *= 1000;
    }
    m = sprintf_s(buffer + loc, 15 - loc, "%d", n);
	loc+=m;
    while (scale != 1) {
        scale /= 1000;
        n = n2 / scale;
        n2 = n2  % scale;
        m = sprintf_s (buffer + loc, 15 - loc, ",%03d", n);
		loc+=m;
    }
	return buffer;
}

char * sprintfDoubleComma(double value, double precision, int isAdjustable)
{
	// Format a double precision value with a specified precision
	//	Allow a total of 16 digits, as that is the maximum precision 
	//	of a double value
	//	There may be up to 16 digits to the left of the decimal point
	//	and 15 digits to the right of the decimal point.  Values that
	//	cannot be expressed within those constraints will be formatted
	//	using scientific notation.
	//
	//	When formatting a value of an uncertainDouble, the value may be adjusted
	//	based on the precision.  However, when formatting the uncertainty of an
	//	uncertainDouble, the value may not be adjusted.
	//	
	//	Maximum size of the field is 22 for fixed notation: -1,234,567,890,123,456
	//		or -123,456,789,012,345.3 or -0.000 000 000 000 001 while for scientific notation, maximum
	//		size is 31: -1.234 567 890 123 456 x 10^-123
	//
	//	Can't begin by converting to an integer, since the
	//	value could be too large.
	//
	//	Assumes precision and uncertainty are valid.  This can be ensured by only constructing
	//	  uncertainDoubles via a call to createUncertainDouble
	//
	double value2 = 0.;
	double scale = 1.;
	double prec10, frac, rndvalue;
	int i, j, m, nzero, lmantissa, nfracdigits, nfracwrite, ival, iexp;
	int loc = 0, tloc = 0, ndigits;
	int bufdim = 32;
	static char buffer[32];
	static char tempbuf[32];
	static char tempbuf2[32];
	int len = sizeof(buffer);
	// The following factor is used to prevent rounding errors
	double preverror = 1. + minFracUncertainty;
	// Clear the string
	memset(buffer, ' ', len - 1);
	memset(tempbuf, ' ', len - 1);
	memset(tempbuf2, ' ', len - 1);
	// Deal with negative values
	if (value < 0.) {
		m = sprintf_s(buffer, bufdim, "-");
		value = -value;
		loc += m;
	}
	//	Determine the power of 10 of the precision
	prec10 = pow(10., floor(log10(precision)));
	//	If the value being formatted is an uncertainty, and the order of magnitude of
	//	the value is less than the order of magnitude of the precision,
	//	  increase the precision.
	if (!isAdjustable && value/ prec10  * preverror< 1.) prec10/= 10.;
	//	Value == 0. and prec10 >= 1. || prec10 < 1.e-15 is a special case
	if (value == 0. && (prec10 >= 1. || prec10 < 1.e-15))
	{
		// For a zero value, simply write 0 x 10^+ppp, unless the exponent is 0.,
		//	in which case just write 0
		if (prec10 == 1.)
		{
			m = sprintf_s(buffer + loc, bufdim - loc, "0");
		}
		else
		{
			// The following has been demonstrated to always yield the correct answer
			//	without a rounding correction.  Specifically, the following statement was tested
			//	floor(log10(1.ex)) where x ranged from -100 to +100.  While a %18.16e representation
			//	of 1.ex ranged from 9.9999999999999990 to 1.0000000000000001, this statement always
			//	returned the proper value.
			//
			iexp = (int32_t)floor(log10(prec10));
			m = sprintf_s(buffer + loc, bufdim - loc, "0 x 10^%d", iexp);
		}
		return buffer;
	}
	//
	// Determine whether fixed or scientific notation is appropriate
	//	The check for precision >= 1.e17 is necessary for the case where value == 0.
	//		since the precision is unbounded.
	if (value >= 1.e16 || prec10 < 1.e-15)
	{
		// Use scientific notation
		//	No need to round, as E format performs rounding
		//	Determine the number of digits after the decimal point of the mantissa
		lmantissa = min((int32_t)floor(log10(value * preverror / prec10)), 15);
		//  Now use E format with specified precision
		m = sprintf_s(tempbuf, bufdim, "%.*E", lmantissa, value);
		// Reformat the string
		//	Copy the first character
		buffer[loc] = tempbuf[tloc];
		++loc;
		++tloc;
		//	If lmantissa > 0, there is a decimal point and a fractional part
		if (lmantissa > 0)
		{
			buffer[loc] = tempbuf[tloc];
			++loc;
			++tloc;
			//	Now copy the mantissa, inserting a space after every 3 characters
			for (i = 1; i <= lmantissa; i++)
			{
				buffer[loc] = tempbuf[tloc];
				++loc;
				++tloc;
				if (i % 3 == 0 && i < lmantissa)
				{
					buffer[loc] = ' ';
					++loc;
				}
			}
		}
		//	Insert ' x 10^'
		buffer[loc] = ' ';
		buffer[loc + 1] = 'x';
		buffer[loc + 2] = ' ';
		buffer[loc + 3] = '1';
		buffer[loc + 4] = '0';
		buffer[loc + 5] = '^';
		loc += 6;
		//  Skip the E
		++tloc;
		//	Skip the +, if present
		if (tempbuf[tloc] == '+') ++tloc;
		//	Copy the exponent
		for (i = 0; i <= m - tloc; i++) buffer[loc + i] = tempbuf[tloc + i];
		//
		return buffer;
	}
	else
	{
		// Use fixed notation
		//	Begin with the integer portion
		//	Format the integral part of a double with commas
		//  Max size is 22 bytes: -2,124,456,789,124,456.
		//	This size is chosen to enable 16 digits to be displayed,
		//	which is the maximum precision for a double.
		//
		// Round the value based on precision
		//	Round up if the first digit to be omitted is >=5
		//	A seemingly efficient way to perform this calculation is to check
		//	whether fmod(value, prec10) / prec10 >= 0.5.
		//	However, this calculation is susceptible to rounding problems,
		//	particularly when prec10 / value is close to the minimum allowed value.
		//	Attempting to correct the calculation by multiplying value by
		//	preverror solves some problems, but causes others.
		//
		// Another approach is the following:
		// Create a 17 digit representation of the value, then
		//	examine the first character to be omitted.
		// The commented lines implement this, but have rounding problems
		//	when computing the sets of digits.
		// Use sprintf to create a 17 digit representation in E format
		// 1.1234567890123456E...
		// m = sprintf_s(tempbuf2, bufdim, "%.16E", value);
		// Find the first digit to be omitted
		// round = tempbuf2[ndigits + 1];
		// if (round == '5' || round == '6' || round == '7' || round == '8' ||
		// 	round == '9' && isAdjustable) value += prec10;
		// rndvalue = value;
		//
		// A better approach is to truncate the value based on the precision
		//	as is done createUncertainDouble.  This makes the trailing digits
		//	zeroes.
		//
		// Determine the number of digits in the representation of value
		//	given the specified precision
		if (value != 0.) ndigits = (int32_t)floor(log10(value)) - 
			(int32_t)floor(log10(prec10)) + 1;
		else ndigits = -(int32_t)floor(log10(prec10)) + 1;
		//
		// The following works for all of the cases tried
		rndvalue = trunc(value / prec10 + 0.5) * prec10;
		if (rndvalue >= 1.)
		{
			// Insert commas
			while (rndvalue >= 1000.) {
				value2 = value2 + scale * floor(fmod(rndvalue, 1000.));
				rndvalue /= 1000.;
				scale *= 1000.;
			}
			m = sprintf_s(buffer + loc, bufdim - loc, "%d", (int32_t)rndvalue);
			loc += m;
			while (scale != 1.) {
				scale /= 1000.;
				rndvalue = value2 / scale;
				value2 = floor(fmod(value2, scale));
				m = sprintf_s(buffer + loc, bufdim - loc, ",%03d", (int32_t)rndvalue);
				loc += m;
			}
			// Zero out digits based on precision
			if (prec10 >= 1.)
			{
				nzero = (int32_t)(log10(prec10));
				i = 1;
				j = 1;
				while (i <= nzero)
				{
					if (buffer[loc - j] != ',')
					{
						buffer[loc - j] = '0';
						++i;
					}
					++j;
				}
			}
		}
		//	Now format the fractional portion
		//	Format as up to 6 groups of 3 digits separated by spaces
		//
		if (prec10 < 1.)
		{
			// Insert a leading zero, if necessary
			//   Reset rndvalue
			rndvalue = trunc(value / prec10 + 0.5) * prec10;
			if (rndvalue < 1.)
			{
				buffer[loc] = '0';
				++loc;
			}
			// Insert the decimal point
			buffer[loc] = '.';
			++loc;
			// Compute the fractional part
			frac = fmod(rndvalue, 1.);
			// Write up to 3 digits at a time until precision is reached
			nfracdigits = -(int32_t)floor(log10(prec10));
			nfracwrite = 0;
			while (nfracwrite < nfracdigits)
			{
				if (nfracdigits - nfracwrite >= 3)
				{
					ival = (int32_t)floor(1000. * frac * preverror);
					// If this is the last set of digits, may need to round due to accumulated error
					if (nfracdigits - nfracwrite == 3) ival =
						(int32_t)floor(1000. * frac + 0.5);
					m = sprintf_s(buffer + loc, bufdim - loc, "%03d", ival);
					loc += m;
					nfracwrite += 3;
					frac = fmod(1000.*frac, 1.);
				}
				else
				{
					// Writing the last group, which is a partial group
					if (nfracdigits - nfracwrite == 2)
					{
						ival = (int32_t)floor(100. * frac * preverror);
						// If this is the last set of digits, may need to round due to accumulated error
						if (nfracdigits - nfracwrite == 2) ival =
							(int32_t)floor(100. * frac + 0.5);
						m = sprintf_s(buffer + loc, bufdim - loc, "%02d", ival);
						loc += m;
						nfracwrite += 2;
					}
					else
					{
						ival = (int32_t)floor(10. * frac * preverror);
						// If this is the last set of digits, may need to round due to accumulated error
						if (nfracdigits - nfracwrite == 1) ival =
							(int32_t)floor(10. * frac + 0.5);
						m = sprintf_s(buffer + loc, bufdim - loc, "%01d", ival);
						loc += m;
						nfracwrite += 1;
					}
				}
				// If at least 1 more group needs to be written, insert a space
				if (nfracwrite < nfracdigits)
				{
					buffer[loc] = ' ';
					++loc;
				}
			}
		}
	}
	return buffer;
}

char * sprintfUncertaintyUTCDatetime(int8_t uncertainty, int8_t precision)
{
	/* Format the uncertainty of a UTCDatetime for printing.  The uncertainty is
	a multiplier on the precision.  The precision can be powers of 1000
	years, years, months days, hours, minutes, seconds or fractions of a second in
	negative powers of 10.

	Express uncertainties in one of the following:
	gigayears, megayears, kiloyears, years, months,
	days, hours, minutes, seconds, milliseconds,
	microseconds, nanoseconds, picoseconds,
	femtoseconds, or attoseconds.

	If the precision is negative, but not a multiple of
	-3, use the next smaller multiple of -3.  E.g., if
	the precision is -4, express the uncertainty in
	microseconds. */
	static char buffer[29]; // +/- 16,512,300 femtoseconds
	static const char* precisionString[] =
	{
		"second", "minute", "hour", "day", "month",
		"year", "kiloyear", "megayear", "gigayear",
		"millisecond", "microsecond", "nanosecond", 
		"picosecond", "femtosecond", "attosecond"
	};
	int lenBuffer = sizeof(buffer);
	char * numBuffer; // 16,512,300
	char * s = "s";
	char * blank = "";
	char * plural;
	int actUncertainty;
	int iprec;

	// Check whether uncertainty can be calculated
	if (precision == 99 || (precision != maxPrecision && uncertainty == 0))
	{
		// Precision not specified
		return blank;
	}
	// Negative uncertainty indicates units of the next higher precision
	if (uncertainty < 0 && precision > maxPrecision)
	{
		precision--;
		uncertainty = -uncertainty;
	}
	// Set the label
	if (precision >= 6)
	{
		// Power of 10 of years
		iprec = 5 + (precision - 5) / 3;
		actUncertainty = uncertainty * power(10, (precision - 2) % 3);
	}
	else if (precision >= 0 && precision <=5)
	{
		// year, month, day, hour, minute or second
		iprec = precision;
		actUncertainty = uncertainty;
	}
	else if (precision == maxPrecision && uncertainty <= 0)
	{
		if (uncertainty < 0)
		{
			// fraction of an attosecond is a special case
			sprintf_s(buffer, lenBuffer, " +/- 0.%d attosecond", -uncertainty);
			return buffer;
		}
		else
		{
			sprintf_s(buffer, lenBuffer, " +/- 0.5 attosecond");
			return buffer;
		}
	}
	else
	{
		// fractions of a second
		iprec = 9 - (precision + 1) / 3;
		actUncertainty = uncertainty * power(10, 2 - (-1 - precision) % 3);
	}
	numBuffer = sprintfComma(actUncertainty);
	if (actUncertainty == 1)
	{
		plural = blank;
	}
	else
	{
		plural = s;
	}
	sprintf_s(buffer, lenBuffer, " +/- %s %s%s", numBuffer, precisionString[iprec], plural);
	return buffer;
}

char * sprintfUncertaintyTAIRelDatetime (int8_t uncertainty, int8_t precision)
{
	/* Format the uncertainty of a TAIRelDatetime printing.  The uncertainty is
		a multiplier on the precision.  The precision can be powers of 1000
		days, days, hours, minutes, seconds or fractions of a second in
		negative powers of 10.  
		
		Express uncertainties in one of the following:
		teradays, gigadays, megadays, kilodays,
		days, hours, minutes, seconds, milliseconds,
		microseconds, nanoseconds, picoseconds,
		femtoseconds, or attoseconds. 
		
		If the precision is negative, but not a multiple of 
		-3, use the next smaller multiple of -3.  E.g., if
		the precision is -4, express the uncertainty in
		microseconds. */
	static char buffer[29]; // +/- 16,512,300 femtoseconds
	static const char* precisionString[] = 
	{
		"second", "minute", "hour", "day", "kiloday",
		"megaday", "gigaday", "teraday", "millisecond",
		"microsecond", "nanosecond", "picosecond", 
		"femtosecond", "attosecond"
	};
	int lenBuffer = sizeof(buffer);
	char * numBuffer; // 16,512,300
	char * s = "s";
	char * blank = "";
	char * plural;
	int actUncertainty;
	int iprec;

	// Check whether uncertainty can be calculated
	if (precision == 99 || (precision != maxPrecision && uncertainty == 0))
	{
		// Precision not specified
		return blank;
	}
	// Zero uncertainty indicates the minimum uncertainty for the specified precision,
	//	which is half the precision
	//
	// Negative uncertainty indicates units of the next higher precision
	if (uncertainty < 0 && precision > maxPrecision)
	{
		precision--;
		uncertainty = -uncertainty;
	}
	// Set the label
	if (precision >= 4)
	{
		// Power of 10 of days
		iprec = 3 + (precision - 3) / 3;
		actUncertainty = uncertainty * power(10, precision % 3);
	}
	else if (precision >= 0 && precision <= 3)
	{
		// day, hour, minute or second
		iprec = precision;
		actUncertainty = uncertainty;
	}
	else if (precision == maxPrecision && uncertainty <= 0)
	{
		if (uncertainty < 0)
		{
			// fraction of an attosecond is a special case
			sprintf_s(buffer, lenBuffer, " +/- 0.%d attosecond", -uncertainty);
			return buffer;
		}
		else
		{
			sprintf_s(buffer, lenBuffer, " +/- 0.5 attosecond");
			return buffer;
		}
	}
	else
	{
		// fractions of a second
		iprec = 13 - (precision - maxPrecision) / 3;
		actUncertainty = uncertainty * power(10, 2 - (-1 - precision) % 3);
	}
	numBuffer = sprintfComma( actUncertainty );
	if (actUncertainty == 1)
	{
		plural = blank;
	}
	else
	{
		plural = s;
	}
	sprintf_s(buffer, lenBuffer, " +/- %s %s%s", numBuffer, precisionString[iprec], plural);
	return buffer;
}

char * sprintfYear (int8_t gigaYear, int32_t year)
{
	// Format the year field in the date
	//   For |years| < 10000, no commas
	//   For |years| >= 10000, insert commas
	//   For years <= 0, append BCE

	static char buffer[19]; // 13,123,456,789 BCE
	char* tempbuf;
	char* BCE = " BCE";
	char* zeros = "000,000,000";
	int appendBCE = 0;
	int n;
	int loc=0;
	int i;
	// Adjust values when in the BCE range
	if (gigaYear < 0 || (gigaYear == 0 && year <= 0))
	{
		if (gigaYear != 0) gigaYear = -gigaYear;
		year = -year + 1;  // Year 0 = 1 BCE, -1 = 2 BCE
		if (year == 1e9)
		{
			gigaYear++;
			year = 0;
		}
		appendBCE = 1;
	}

	if (gigaYear > 0)
	{
		n = sprintf_s(buffer, 19, "%d,", gigaYear);
		loc += n;
		// Load the buffer with zeros
		memcpy(buffer+loc, zeros, 12);
		// Format the years
		tempbuf = sprintfComma(year);
		// Copy the year string into the buffer
		for (i = (int)strlen(tempbuf); i >= 0; i--)
		{
			buffer[loc+11-strlen(tempbuf)+i] = tempbuf[i];
		}
		loc += 11;
		if (appendBCE) n = sprintf_s(buffer+loc, 5, "%s", BCE);
		return buffer;
	}
	if (year < 10000)
	{
		n = sprintf_s(buffer+loc, 19-loc, "%d", year);
		loc += n;
	}
	else
	{
		// Insert commas
		n = sprintf_s(buffer+loc, 19-loc, "%s", sprintfComma(year));
		loc += n;
	}
	// Append BCE if necessary
	if (appendBCE) n = sprintf_s(buffer+loc, 5, "%s", BCE);
	return buffer;
}

char * sprintfDays (uint32_t gigaDays, uint32_t days)
{
	// Format the days field in a relative tick elements value

	static char buffer[19]; // 13,123,456,789,123
	char* tempbuf;
	char* zeros = "000,000,000";
	int n;
	int loc=0;
	int i;
	if (gigaDays > 0)
	{
		// gigaDays is non-zero, and may be between 1 and 5 digits
		//	Use sprintfComma to copy gigaDays into the buffer
		n = sprintf_s(buffer+loc, 20-loc, "%s", sprintfComma(gigaDays));
		loc += n;
		// Insert a comma
		buffer[loc] = ',';
		loc ++;
		// Load the buffer with zeros
		memcpy(buffer+loc, zeros, 12);
		// Format the days
		tempbuf = sprintfComma(days);
		// Copy the day string into the buffer
		for (i = (int)strlen(tempbuf); i >= 0; i--)
		{
			buffer[loc+11-strlen(tempbuf)+i] = tempbuf[i];
		}
		loc += 11;
		return buffer;
	}
	else
	{
		// Insert commas in days
		n = sprintf_s(buffer+loc, 20-loc, "%s", sprintfComma(days));
		loc += n;
	}
	return buffer;
}

const char * asStringDatetime(int8_t inGigayear, int32_t inYear, uint8_t inMonth,
	uint8_t inDayOfMonth, uint8_t inHour, uint8_t inMinute,
	uint8_t inSecond, uint32_t inNanosecond, uint32_t inAttosecond,
	uint32_t calendar, uint8_t frame, int8_t precision, int8_t uncertainty,
	char timeZone[], uint8_t borA, char stringDT[])
{
	// Format a local datetime, and, by specialization, a CalCoords or a UTCDatetime
	// Maximal form is:
	// 12-Jan-13,123,456,789 BCE 12:13:15.000000000 000000000 +/- 0.5 attosecond America/North_Dakota/New_Salem Standard (+) Julian
	//  which is 124 characters.
	//
	int n, i;
	int loc, nskip, stop, nleadskip, ncomma, year, gigayear, loc1d;
	int nadd = 0;
	static char tempBuffer[130];
	// Variables required to print time zone
	char tzBuffer[50];
	// Variables required to print frame.
	char * uTC = " UTC";
	char * universal = " Universal";
	char * standard = " Standard";
	char * wall = "";
	char * frameString;
	// Variables required to print BorA.  BorA is the flag used to disambiguate times
	//	when there is a negative jump, e.g., going from daylight savings time to standard
	//	time, causing times to be repeated.  0 indicates before the jump, 1 after the jump
	char * before = " (-)";
	char * after = " (+)";
	char * na = "";
	char * borAString;
	// Variables required to print calendar string
	char * julian = " Julian";
	char * swedish = " Swedish";
	char * gregorian = "";
	char * calString;
	int len = sizeof(tempBuffer);
	int8_t dayOfMonth, month;
	DateCoords y2;

	// Clear the strings
	memset(stringDT, ' ', len - 1);
	memset(tempBuffer, ' ', len - 1);

	int8_t BCEgigayear, maxBCEgigayear = 13, maxGigayear = 99;
	int32_t BCEyear, BCEbbyear = 1 - bbYear, m;


	// Do not round based on digits truncated based on precision.  A time represents an interval.
	//	For example, with a precision of seconds, a time of 6:23:01 represents the second starting
	//	at 6:23:01.  Fractional seconds truncated do not affect the time interval.  For example,
	//	if the time is 6:23:01.9, the second is still 6:23:01
	//
	// The only exception to the above is the year, which must be rounded if the precision is 
	//	10 years or less
	//
	//	Need to hanlde the BCE and CE periods, differently
	//
	//	Year 0 is 1 BCE.  Year -1 is 2 BCE.
	//	We are in the BCE period if either year < 0, gigayear < 0 or both == 0
	//   We will round the BCE period, according to how it is displayed.
	//	 Thus 5 BCE would round to 10 BCE, even though it is -4.
	//
	year = inYear;
	gigayear = inGigayear;

	if (year < 0 || gigayear < 0 || ((year == 0) && (gigayear == 0)))
	{
		// In the BCE period
		//	No need to provide a calendar, as only one calendar existed at the time
		calString = gregorian;
		dayOfMonth = inDayOfMonth;
		month = inMonth;
		//	Change the basis to BCE years for rounding calculations
		BCEyear = 1 - year;
		if (gigayear != 0) BCEgigayear = -gigayear;
		else BCEgigayear = 0;
		//	Carry, if necessary
		if (BCEyear == 1e9)
		{
			BCEgigayear++;
			BCEyear = 0;
		}
		// Round 10's of BCEgigayears if the first omitted digit >= 5
		if (precision == minUTCPrecision)
		{
			// Round BCEgigayears to neareast 10 BCEgigayears
			//  Since BCEgigayears is <= 13, no need to check for exceeding minimum time
			if ((BCEgigayear % 10) >= 5) BCEgigayear = BCEgigayear / 10 * 10 + 10;
			else BCEgigayear = BCEgigayear / 10 * 10;
			BCEyear = 0;
		}
		// Round BCEgigayears if BCEyear > 500,000,000
		else if (precision == minUTCPrecision - 1)
		{
			// Round BCEgigayears to nearest BCEgigayear
			//  Don't allow BCEgigayear to exceed maxBCEgigayear
			if (BCEyear >= e9 / 2 && BCEgigayear < maxBCEgigayear) BCEgigayear++;
			BCEyear = 0;
		}
		// Round BCEyears if necessary for precisions of powers of 10 of BCEyears
		if (precision >= 6 && precision <= 13)
		{
			// Round to nearest multiple of appropriate power of 10 of BCEyears
			//	Don't allow BCEyear to exceed BCEbbyear
			if (BCEyear % power(10, (precision - 5)) >= 5 * power(10, (precision - 6)))
			{
				m = BCEyear / power(10, (precision - 5)) * power(10, (precision - 5)) +
					power(10, (precision - 5));
				// If rounding up would exceed minimum UTC time, round down
				BCEyear = (m <= BCEbbyear || BCEgigayear < maxBCEgigayear ? m :
					m - power(10, (precision - 5)));
			}
			else
			{
				BCEyear = BCEyear / power(10, (precision - 5)) * power(10, (precision - 5));
			}
			if (BCEyear == e9)
			{
				// BCEgigayear cannot be equal maxBCEgigayear, so OK to increment 
				BCEgigayear++;
				BCEyear = 0;
			}
		}
		// Change basis back to negative years
		year = 1 - BCEyear;
		if (BCEgigayear != 0) gigayear = -BCEgigayear;
		// Borrow, if necessary
		if (year == 1 && gigayear != 0)
		{
			year = 1 - e9;
			gigayear++;
		}
	}
	else
	{
		// In the CE period
		//  Set the value of calString
		//	 Create a DateCoords to facilitate comparisons
		month = inMonth;
		dayOfMonth = inDayOfMonth;
		y2.gigayear = gigayear;
		y2.year = year;
		y2.month = inMonth;
		y2.dayOfMonth = inDayOfMonth;
		y2.calendar = calendar;
		y2.dateInit = 0;
		//	 If gigayear > 0, calendar is blank
		if (gigayear > 0) calString = gregorian;
		//
		//	Up to earliest transition date, calendar is blank
		//
		else if (isLessOrEqualDateCoords_NC(y2, EarliestTransitionDate) || calendar == 0) calString = gregorian;
		//  If past earliest transition date and calendar == 1, calString is Julian
		else if (calendar == 1) calString = julian;
		//  Sweden follows the Julian convention through 2/28/1700.  By not having a leap
		//	year in 1700, Sweden introduces a new convention beginning 3/1/1700.  This
		//	persisits through 2/30/1712, after which the convention becomes Julian, again.
		//	Finally, beginning 3/1/1753, the convention is Gregorian.
		else if (calendar == 2)
		{
			if (isLessOrEqualDateCoords_NC(y2, feb281700)) calString = julian;
			else if (isLessDateCoords_NC(y2, mar011712)) calString = swedish;
			else if (isLessDateCoords_NC(y2, mar011753)) calString = julian;
			else calString = gregorian;
			// Deal with the special case where a wall time on mar011753 was
			//	adjusted back to 
		}
		//  When the last Julian date is specified, the period between the earliest transition
		//		and the last Julian date, inclusive, has the Julian suffix.
		//	The below also properly handles the cases where the calendar has been
		//		extended to handle translations from wall to universal time.
		//	    In the case where an extra Julian day is added, the lastJulianDate
		//		function handles the situation.  When an extra Gregorian day is
		//	    added, the date is still beyond the last Julian date, so the value
		//	    will be Gregorian.
		else if (isGreaterDateCoords_NC(y2, EarliestTransitionDate) &&
			isLessOrEqualDateCoords_NC(y2, lastJulianDate(calendar))) calString = julian;
		//	In all other cases, the convention is Gregorian
		else calString = gregorian;
		// Round 10's of gigayears if the first omitted digit >= 5
		if (precision == minUTCPrecision)
		{
			// Round gigayears to neareast 10 gigayears
			//  Don't allow gigayear to exceed maxGigayear
			if ((gigayear % 10) >= 5)
			{
				m = gigayear / 10 * 10 + 10;
				gigayear = (m <= maxGigayear ? m : gigayear);
			}
			else gigayear = gigayear / 10 * 10;
			year = 0;
		}
		// Round gigayears if year > 500,000,000
		else if (precision == minUTCPrecision - 1)
		{
			// Round gigayears to nearest gigayear
			//  Don't allow gigayear to exceed maxGigayear
			if (year >= e9 / 2 && gigayear < maxGigayear) gigayear++;
			year = 0;
		}
		// Round years if necessary for precisions of powers of 10 of years
		if (precision >= 6 && precision <= 13)
		{
			// Round to nearest multiple of appropriate power of 10 of years
			if (year % power(10, (precision - 5)) >= 5 * power(10, (precision - 6)))
			{
				m = year / power(10, (precision - 5)) * power(10, (precision - 5)) +
					power(10, (precision - 5));
				// If rounding up causes the value to exceed the
				//	maximum UTC time, round down
				year = (m < 1e9 || gigayear < maxGigayear ? m :
					m - power(10, (precision - 5)));
			}
			else
			{
				year = year / power(10, (precision - 5)) * power(10, (precision - 5));
			}
			if (year == e9)
			{
				// Safe to carry based on calculation for year
				gigayear++;
				year = 0;
			}
		}
	}
	// Establish the values of frameString and borAString.
	//	Determine whether time zone has been specified
	if (strlen(timeZone) >= 1)
	{
		// Time zone has been specified
		if (frame == 0)
		{
			// Frame is either Universal or UTC based on value of calString
			if (calString == gregorian) frameString = uTC;
			else frameString = universal;
			// BorA string is na when frame is universal
			borAString = na;
			// Do not output time zone
			tzBuffer[0] = '\0';
		}
		else
		{
			if (frame == 1) frameString = standard;
			else frameString = wall;
			// Set borAString assuming borA has only been set to 1 or 2 when relevant
			if (borA == 1) borAString = before;
			else if (borA == 2) borAString = after;
			else borAString = na;
			// Output time zone after prepending a space
			tzBuffer[0] = ' ';
			strcpy_s(tzBuffer + 1, strlen(timeZone) + 1, timeZone);
		}
	}
	else
	{
		// Time zone has not been specified.
		//	Frame is either Universal or UTC based on value of calString
		if (calString == gregorian) frameString = uTC;
		else frameString = universal;
		// BorAString is na when time zone has not been specified
		borAString = na;
		// Do not output time zone
		tzBuffer[0] = '\0';
	}
	// Format the day, month and year
	n = sprintf_s(tempBuffer, len, "%02d-%s-%s %02d:%02d:%02d.%09d %09d%s%s%s%s%s", dayOfMonth, months[month],
		sprintfYear(gigayear, year), inHour, inMinute, inSecond, inNanosecond, inAttosecond,
		sprintfUncertaintyUTCDatetime(uncertainty, precision), tzBuffer, frameString,
		borAString, calString);

	// Remove elements based on the desired precision
	nleadskip = 0;
	nskip = 0;
	if (precision < -9)
	{
		// We need part of the attosecond field
		nskip = 18 + precision;
	}
	if (precision >= -9 && precision < 0)
	{
		// We need part of the nanosecond field
		nskip = 19 + precision; // There is a space between the nanosecond and attosecond fields
	}
	if (precision == 0)
	{
		// Only need through seconds
		nskip = 20;
	}
	if (precision == 1)
	{
		// Only need through minutes
		nskip = 23;
	}
	if (precision == 2)
	{
		// Only need through hours. Add 'h '
		nskip = 26;
		nadd = 1;
	}
	if (precision >= 3)
	{
		// Only need the date.
		nskip = 29;
	}
	if (precision == 4)
	{
		// Only need the month and year
		nleadskip = 3;
	}
	if (precision >= 5 && precision <= 15)
	{
		// Only need the year
		nleadskip = 7;
		// Zero out trailing digits of the years based on precision
		if (precision > 5)
		{
			// Locate the 1's digit of the year
			//  The 1's digit of the year is the location before the first space
			loc1d = 0;
			while (tempBuffer[loc1d] != ' ') loc1d++;
			ncomma = 0;
			for (i = 0; i <= precision - 6; i++)
			{
				if (tempBuffer[loc1d - 1 - i - ncomma] == ',') ncomma++;
				tempBuffer[loc1d - 1 - i - ncomma] = '0';
			}
		}
	}
	if (precision == 99)
	{
		// Not specified
		nskip = 0;
	}

	// Find the last digit of the attoseconds by adding 19 to the location of the decimal point
	loc = 0;
	while (tempBuffer[loc] != '.') loc++;
	stop = loc + 19;

	// Copy from the end of the tempBuffer to just before the last digit of the attoseconds 
	loc = n;
	while (loc > stop)
	{
		stringDT[loc - nskip + nadd - nleadskip] = tempBuffer[loc];
		loc--;
	}
	// Copy the rest of the string, omitting the leading day and month if appropriate
	// Add in 'h' if precision == 2
	if (precision == 2)
	{
		stringDT[loc - nskip + 1] = 'h';
		stringDT[loc - nskip + 2] = ' ';
	}
	while (loc >= nskip + nleadskip)
	{
		stringDT[loc - nskip - nleadskip] = tempBuffer[loc - nskip];
		loc--;
	}
	return stringDT;
}

const char * asStringUTCDatetime(UTCDatetime u1, char stringUTC[], uint32_t calendar)
{
	// Format a UTCDatetime as a readable string using the calendar convention specified
	// * stringUTC must be dimensioned at least 130 
	// Use asStringDatetime
	//
	DateCoords y1, y2;
	//	 Create a DateCoords and convert to the desired calendar convention
	y1.gigayear = u1.gigayear;
	y1.year = u1.year;
	y1.month = u1.month;
	y1.dayOfMonth = u1.dayOfMonth;
	y1.calendar = 0;
	y1.dateInit = 0;
	//
	y2 = convertFromUTCGregorian(y1, calendar);
	//
	// Call asStringDatetime, setting frame and borA consistent with UTC
	//
	return asStringDatetime(y2.gigayear, y2.year, y2.month, y2.dayOfMonth, u1.hour, u1.minute,
		u1.second, getNanosecond(u1.tai), getAttosecond(u1.tai), calendar,
		0, u1.precision, u1.uncertainty, "", 2, stringUTC);
}

const char * asStringTAIRelDatetime (TAIRelDatetime re, char tickElemString[])
{
	/*  Format the elements of a relative tick as follows:
		-32,319,287,316,284d 12h 3m 4.000000000 000000000s +/- 1,234,567 attoseconds TAI
		Since the total time range is < 113 e9 years, the
		number of days is < 50 e12.  
		tickElemString must be dimensioned at least 85 */
	int n, i, ncomma;
	int loc = 0, nskip, stop, maxLeadingElemRemoved, nfstop;
	int dloc, hloc, mloc, decloc;
	int nadd = 0;
	static char tempBuffer[85];
	int len = sizeof(tempBuffer);
	uint16_t m;
	uint32_t m2;

	// Rounded values
	uint32_t rgigadays = re.gigadays, rdays = re.days;
	int rnanoseconds = getNanoseconds(re.relTicks),
		rattoseconds = getAttoseconds(re.relTicks);
	uint8_t rhours = re.hours, rminutes = re.minutes,
		rseconds = re.seconds;

	// Clear the strings
	memset(tickElemString, ' ', len - 1);
	memset(tempBuffer, ' ', len - 1);

	// Round the value, if necessary, based on precision
	if (re.precision != 99 && re.precision > maxPrecision)
	{
		if (re.precision > 12)
		{
			// Round to nearest multiple of appropriate power of 10 of rgigadays
			if (rgigadays % power(10, (re.precision - 12)) >= (uint32_t)5 * power(10, (re.precision - 13)))
			{
				// Don't allow to exceed the maximum
				m = rgigadays / power(10, (re.precision - 12)) * power(10, (re.precision - 12)) +
					power(10, (re.precision - 12));
				// If rounding up exceeds maximum value, round down
				rgigadays = (m <= maxGigaSIDays ? m : m - power(10, (re.precision - 12)));
			}
			else
			{
				rgigadays = rgigadays / power(10, (re.precision - 12)) * power(10, (re.precision - 12));
			}
			// Zero remaining fields
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
			rminutes = 0;
			rhours = 0;
			rdays = 0;
		}
		if (re.precision == 12)
		{
			// Round to nearest gigaSIDay, rounding up, if necessary
			if (rdays >= e9 / 2 && rgigadays < maxGigaSIDays) rgigadays++;
			// Zero remaining fields
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
			rminutes = 0;
			rhours = 0;
			rdays = 0;
		}
		if (re.precision >= 4 && re.precision < 12)
		{
			// Round to nearest multiple of appropriate power of 10 of sIDays
			// Don't allow sIDays to exceed maxSIDays if rgigadays == maxGigaSIDays
			if (rdays % power(10, (re.precision - 3) >= 5 * power(10, (re.precision - 4))))
			{
				m2 = rdays / power(10, (re.precision - 3)) * power(10, (re.precision - 3)) +
					power(10, (re.precision - 3));
				// If rounding up would exceed maximum value, round down
				rdays = (m2 < maxSIDays || rgigadays < maxGigaSIDays ? m2 :
					m2 - power(10, (re.precision - 3)));
			}
			else
			{
				rdays = rdays / power(10, (re.precision - 3)) * power(10, (re.precision - 3));
			}
			// Zero remaining fields
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
			rminutes = 0;
			rhours = 0;
		}
		if (re.precision == 3)
		{
			// Don't allow sIdays to exceed maxSIDays if rgigadays == maxGigaSIDays
			if (rhours >= 12 && (rgigadays < maxGigaSIDays || rdays < maxSIDays)) rdays++;
			// Zero remaining fields
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
			rminutes = 0;
			rhours = 0;
		}
		if (re.precision == 2)
		{
			// Minutes will be zero when at the maximum value, so no potential problem rounding hours
			if (rminutes >= 30) rhours++;
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
			rminutes = 0;
		}
		if (re.precision == 1)
		{
			// Don't allow minumtes to exceed 0, when rgigadays, rdays and rhours are at the maximum
			if (rseconds >= 30 && (rgigadays < maxGigaSIDays || rdays < maxSIDays
				|| rhours < 12)) rminutes++;
			rattoseconds = 0;
			rnanoseconds = 0;
			rseconds = 0;
		}
		if (re.precision == 0)
		{
			// Don't allow seconds to exceed 9 + numLeapSeconds when other fields are at the maximum
			if (rnanoseconds >= e9 / 2 && (rgigadays < maxGigaSIDays || rdays < maxSIDays
				|| rhours < 12 || rseconds < 9 + numLeapSeconds)) rseconds++;
			rattoseconds = 0;
			rnanoseconds = 0;
		}
		if (re.precision == -9)
		{
			// Don't allow rnanoseconds to reach e9 when other fields are at the maximum
			if (rattoseconds >= e9 / 2 && (rgigadays < maxGigaSIDays || rdays < maxSIDays
				|| rhours < 12 || rseconds < 9 + numLeapSeconds ||
				rnanoseconds < e9 - 1)) rnanoseconds++;
			rattoseconds = 0;
		}
		if (re.precision < 0 && re.precision > -9)
		{
			// Round to the nearest multiple of appropriate power of 10 of nanoseconds
			// Don't allow rnanoseconds to reach e9 when other fields are at the maximum
			if ((rnanoseconds % power(10, 9 + re.precision)) >= 5 * power(10, 8 + re.precision))
			{
				m2 = (rnanoseconds / power(10, (9 + re.precision))) *
					power(10, (9 + re.precision))
					+ power(10, (9 + re.precision));
				rnanoseconds = (m2 < e9 || rgigadays < maxGigaSIDays || rdays < maxSIDays
					|| rhours < 12 || rseconds < 9 + numLeapSeconds ?
					m2 : m2 - power(10, (9 + re.precision)));
			}
			else
			{
				rnanoseconds = (rnanoseconds / power(10, (9 + re.precision))) *
					power(10, (9 + re.precision));
			}
			rattoseconds = 0;
		}
		if (re.precision < -9 && re.precision > maxPrecision)
		{
			// Round to the neareast multiple of appropriate power of 10 of attoseconds
			// Don't allow rattoseconds to reach e9 when other fields are at the maximum
			if ((rattoseconds % power(10, 18 + re.precision)) >= 5 * power(10, 17 + re.precision))
			{
				m2 = (rattoseconds / power(10, (18 + re.precision))) *
					power(10, (18 + re.precision))
					+ power(10, (18 + re.precision));
				rattoseconds = (m2 < e9 || rgigadays < maxGigaSIDays || rdays < maxSIDays
					|| rhours < 12 || rseconds < 9 + numLeapSeconds || rnanoseconds < e9 - 1 ?
					m2 : m2 - power(10, (18 + re.precision)));
			}
			else
			{
				rattoseconds = (rattoseconds / power(10, (18 + re.precision))) *
					power(10, (18 + re.precision));
			}
		}
		// Perform carry calculations
		if (rattoseconds == e9)
		{
			rnanoseconds++;
			rattoseconds = 0;
		}
		if (rnanoseconds == e9)
		{
			rseconds++;
			rnanoseconds = 0;
		}
		if (rseconds == 60)
		{
			rminutes++;
			rseconds = 0;
		}
		if (rminutes == 60)
		{
			rhours++;
			rminutes = 0;
		}
		if (rhours == 24)
		{
			rdays++;
			rhours = 0;
		}
		if (rdays == e9)
		{
			rgigadays++;
			rdays = 0;
		}
	}
	// Format the days, hours, minutes and seconds
	//	Include all elements beginning with the largest, non-zero element
	//		e.g., 3d 4h 1m 0s if days are non-zero
	//				    3m 1s if days and hours are zero

	// Insert a negative sign if necessary
	if (isNegativeRelTicks(re.relTicks))
	{
		tickElemString[0] = '-';
		nadd++;
	}

	n = sprintf_s (tempBuffer, len, "%sd %dh %dm %d.%09d %09ds%s TAI",
					sprintfDays(rgigadays, rdays), rhours, rminutes, rseconds,
					rnanoseconds, rattoseconds, 
					sprintfUncertaintyTAIRelDatetime(re.uncertainty, re.precision));

	// Remove trailing elements based on the desired precision

	//	Find the locations of the d, h, m and decimal point
	loc = 0;
	while (tempBuffer[loc] != 'd') loc++;
	dloc = loc;
	while (tempBuffer[loc] != 'h') loc++;
	hloc = loc;
	while (tempBuffer[loc] != 'm') loc++;
	mloc = loc;
	while (tempBuffer[loc] != '.') loc++;
	decloc = loc;
	maxLeadingElemRemoved = 0;
    nskip = 0;
	if (re.precision < -9)
	{
		// We need part of the attosecond field
		nskip = 18 + re.precision;
		maxLeadingElemRemoved = 3; // Days, hours and minutes may be removed if 0
	}
	if (re.precision >= -9 && re.precision < 0)
	{
		// We need part of the nanosecond field
		nskip = 19 + re.precision; // There is a space between the nanosecond and attosecond fields
		maxLeadingElemRemoved = 3; // Days, hours and minutes may be removed if 0
	}
	if (re.precision == 0)
	{
		// Only need through seconds
		nskip = 20;
		maxLeadingElemRemoved = 3; // Days, hours and minutes may be removed if 0
	}
	if (re.precision == 1)
	{
		// Only need through minutes.  Find the number of
		//	characters from the m to the decimal point
		nskip = 19 + decloc - mloc;
		maxLeadingElemRemoved = 2; // Days and hours may be removed if 0
	}
	if (re.precision == 2)
	{
		// Only need through hours.  Find the number of
		//	characters from the h to the decimal point
		nskip = 19 + decloc - hloc;
		maxLeadingElemRemoved = 1; // Days may be removed if 0
	}
	if (re.precision >= 3 && re.precision !=99)
	{
		// Only need days.  Find the number of
		//	characters from the h to the decimal point
		nskip = 19 + decloc - dloc;
		maxLeadingElemRemoved = 0; // No elements may be removed
		// Zero out trailing digits of the days based on precision
		if (re.precision >= 4)
		{
			ncomma = 0;
			for (i = 0; i <= re.precision - 4; i++)
			{
				if (tempBuffer[dloc - 1 - i - ncomma] == ',') ncomma++;
				tempBuffer[dloc - 1 - i - ncomma] = '0';
			}
		}
	}
	if (re.precision == 99)
	{
		// Not specified
		maxLeadingElemRemoved = 3;
		nskip = 0;
	}

	// Remove leading elements until first non-zero element is found.
	//	Ensure at least 1 element remains

	nfstop = 0;
	if ( (maxLeadingElemRemoved == 1) && (re.gigadays == 0) && (re.days == 0) ) 
		nfstop = dloc + 2;
	if ( (maxLeadingElemRemoved == 2) && (re.gigadays == 0) && (re.days == 0) )
	{
		nfstop = dloc + 2;
		if (re.hours == 0) nfstop = hloc + 2;
	}
	if ( (maxLeadingElemRemoved == 3) && (re.gigadays == 0) && (re.days == 0) )
	{
		nfstop = dloc + 2;
		if (re.hours == 0)
		{
			nfstop = hloc + 2;
			if (re.minutes == 0) nfstop = mloc + 2;
		}
	}

	// Find the last digit of the attoseconds by adding 19 to the location of the decimal point
	stop = decloc + 19;
	// If precsion > 0, no seconds, so do not include the s
	if (re.precision > 0 && re.precision !=99)
	{
		stop++;
		nskip++;
	}

	// Copy from the end of the tempBuffer to just before the last digit of the attoseconds 
	loc = n;
	while ( loc > stop )
	{
		tickElemString[loc-nfstop-nskip+nadd] = tempBuffer[loc];
		loc --;
	}
	// Copy the rest of the string
	while (loc - nfstop >= nskip)
	{
		tickElemString[loc-nfstop-nskip+nadd] = tempBuffer[loc-nskip];
		loc--;
	}
	return tickElemString;
}

const char * readableNumberString (const char * tickString, char readableTickString[])
{
	/* Insert commas to the left of the decimal point and spaces to the right
		to improve the readability of Tick and RelTick strings. 
		readableTickString must be dimensioned, at least, 55 */
	int decind = 0;
	int iter;
	int numLeft, numRight, numCommas;
	int ibuft, ibufr;
	int ndig, numRem;
	int size;
	// Determine the size of the ticksString
	size = (int)strlen(tickString) + 1;
	// No need to clear the readableTickString as we are writing from the
	//	beginning and explicitly null terminating.

	// Find the decimal point
	for (iter = 0; iter < size; iter++)
	{
		if (tickString[iter] == '.')
		{
			decind = iter;
			break;
		}
	}
	numLeft = decind;
	if (tickString[0] == '-') numLeft--; // One less digit for a negative number
	ibuft = 0;
	ibufr = 0;
	// Copy minus sign
	if (tickString[0] == '-')
	{
		readableTickString[0] = tickString[0];
		ibuft++;
		ibufr++;
	}
	// Copy first set of digits
	ndig = numLeft % 3;
	if (ndig == 0) ndig = 3;
	memcpy(readableTickString + ibufr, tickString + ibuft, ndig);
	ibufr += ndig;
	ibuft += ndig;
	// Copy the remaining left side digits, inserting commas as needed
	numCommas = numLeft / 3;
	if (numLeft % 3 == 0) numCommas--;
	for (iter = 0; iter < numCommas; iter++)
	{
		// Insert a comma then copy the next 3 digits
		readableTickString[ibufr] = ',';
		ibufr++;
		memcpy(readableTickString + ibufr, tickString + ibuft, 3);
		ibufr+=3;
		ibuft+=3;
	}
	// Insert the decimal point
	readableTickString[ibufr] = '.';
	ibufr++;
	ibuft++;
	// Determine the number of digits to the right of the decimal point
    numRight = 0;
	for (iter = decind + 1; iter < size; iter++)
	{
		if (tickString[iter] == '\0')
		{
			numRight = iter - decind - 1;
			break;
		}
	}
	// If the number of digits to the right is less than 4 no need for spaces
	if (numRight < 4)
	{
		memcpy(readableTickString + ibufr, tickString + ibuft, numRight);
	}
	else
	{
		// Write 3 digits at a time then a space
		numRem = numRight;
		while (numRem > 3)
		{
			memcpy(readableTickString + ibufr, tickString + ibuft, 3);
			ibufr+=3;
			ibuft+=3;
			numRem-=3;
			readableTickString[ibufr] = ' ';
			ibufr++;
		}
		// Write the remaining digits
		memcpy(readableTickString + ibufr, tickString + ibuft, numRem);
		// Null terminate
		readableTickString[ibufr+numRem] = '\0';
	}
	return readableTickString;
}

TAITicks64 createTicks64(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds)
{
	// For speed, do not check for carry
	TAITicks64 t1;
	uint64_t carry;
	t1.seconds = (uint64_t)gigaSeconds*e9 + seconds;
	t1.attoseconds = (uint64_t)nanoSeconds*e9 + attoSeconds;

	// Carry, if necessary
	carry = t1.attoseconds / e18;
	t1.seconds += carry;
	t1.attoseconds -= e18*carry;
	return t1;
}

TAITicks32 createTicks32(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds)
{
	// For speed, do not check for carry
	TAITicks32 t1;
	uint32_t carry;
	t1.gigaseconds = gigaSeconds;
	t1.seconds = seconds;
	t1.nanoseconds = nanoSeconds;
	t1.attoseconds = attoSeconds;

	// Carry, if necessary
	carry = t1.attoseconds / e9;
	t1.nanoseconds += carry;
	t1.attoseconds -= e9*carry;

	carry = t1.nanoseconds / e9;
	t1.seconds += carry;
	t1.nanoseconds -= e9*carry;

	carry = t1.seconds / e9;
	t1.gigaseconds += carry;
	t1.seconds -= e9*carry;

	return t1;
}


TAITicks createTicks(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds)
{
	TAITicks t1;
#ifdef Ticks64
	t1 = createTicks64(gigaSeconds, seconds, nanoSeconds, attoSeconds);
#else
	t1 = createTicks32(gigaSeconds, seconds, nanoSeconds, attoSeconds);
#endif
	return t1;
}

TAIRelTicks64 createRelTicks64(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds, int isNegative)
{
	TAIRelTicks64 r1;
	uint64_t carry;
	r1.seconds = (uint64_t)gigaSeconds*e9 + seconds;
	r1.attoseconds = (uint64_t)nanoSeconds*e9 + attoSeconds;

	// Carry, if necessary
	carry = r1.attoseconds / e18;
	r1.seconds += carry;
	r1.attoseconds -= e18*carry;
	// Add e18 to attoseconds if negative
	if (isNegative) r1.attoseconds+=e18;
	return r1;
}

TAIRelTicks32 createRelTicks32(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds, int isNegative)
{
	TAIRelTicks32 r1;
	uint32_t carry;
	r1.gigaseconds = gigaSeconds;
	r1.seconds = seconds;
	r1.nanoseconds = nanoSeconds;
	r1.attoseconds = attoSeconds;

	// Carry, if necessary
	carry = r1.attoseconds / e9;
	r1.nanoseconds += carry;
	r1.attoseconds -= e9*carry;

	carry = r1.nanoseconds / e9;
	r1.seconds += carry;
	r1.nanoseconds -= e9*carry;

	carry = r1.seconds / e9;
	r1.gigaseconds += carry;
	r1.seconds -= e9*carry;

	// Add e9 to attoseconds if negative
	if (isNegative) r1.attoseconds+=e9;
	return r1;
}


TAIRelTicks createRelTicks(uint32_t gigaSeconds, uint32_t seconds, uint32_t nanoSeconds, uint32_t attoSeconds, int isNegative)
{
	TAIRelTicks r1;
#ifdef Ticks64
	r1 = createRelTicks64(gigaSeconds, seconds, nanoSeconds, attoSeconds, isNegative);
#else
	r1 = createRelTicks32(gigaSeconds, seconds, nanoSeconds, attoSeconds, isNegative);
#endif
	return r1;
}


TAIRelTicks adjustRelTicks(TAIRelTicks r1, uint32_t nanoPerSecondAdjustment)
{
	/* Adjust a relative tick value by adding a specified number of nanoseconds
		per second.  The adjustment for the ith second is added at the
		start of the i+1st second.  I.e., there is no adjustment for the 0th
		second */
	TAIRelTicks r1Adj;
	uint32_t secondsAdj, nanoSecondsAdj;
	uint32_t fullSeconds1 = 0, fullSeconds2 = 0;
	int e13e8 = 130000000;

	/* The largest cumulative adjustment for one period is the
		over 5 second adjustment over the first part of the
		early UTC period (24 nanoseconds / second over 7
		years.  Multiplying nanoPerSecondAdjustment times
		the number of seconds will overflow a 32-bit integer
		at the end of the interval.

		The EarlyAtomic interval is 3 years, which is < 1e8 seconds.
		EarlyUTC is 7 years, which is > 2e8 seconds.
		EarlyUTC2 is 4 years, which is < 1.3e8 seconds.

		Recognizing the above, and noting that 1.3e8 * 24 = 3.12e9,
		divide intervals > 1.3e8 into two parts to avoid
		overflow. */

	fullSeconds1 = getSeconds(r1) % e13e8;
	fullSeconds2 = (getSeconds(r1) / e13e8) * e13e8;

	// The following value < 4.12e9, so will not overflow
	nanoSecondsAdj = nanoPerSecondAdjustment * fullSeconds1 + getNanoseconds(r1);
	// The value of the following is < 3e8
	secondsAdj = nanoSecondsAdj / e9 + getSeconds(r1);
	nanoSecondsAdj -= (nanoSecondsAdj / e9) * e9;

	if (fullSeconds2 > 0)
	{
		nanoSecondsAdj += nanoPerSecondAdjustment * fullSeconds2;
		secondsAdj += nanoSecondsAdj / e9;
		nanoSecondsAdj -= (nanoSecondsAdj / e9) * e9;
	}

	r1Adj = createRelTicks(0, secondsAdj, nanoSecondsAdj, getAttoseconds(r1), 0);
	return r1Adj;
}

TAIRelTicks unAdjustRelTicks(TAIRelTicks r1Adj, uint8_t period)
{
	/* Restore the unadjusted relative tick value for a realtive tick value
		adjusted by adding 15, 24 or 25 nanoseconds	per second.  The adjustment
		for the ith second is added at the start of the i+1st second.  I.e., there
		is no adjustment for the 0th second.
		
		The period is 0 for Early Atomic, 1 for Early UTC through 1967 and
			2 for Early UTC from 1968 through 1971. */

	TAIRelTicks r2;
	// Number of added seconds for each period
	static const int numAddedSeconds[3] = {1, 5, 3};
	// Number of elapsed seconds after which the correction exceeds an integral number of seconds
	static const uint32_t addedSecondTime[3][5] = 
	{
		66666667, 0, 0, 0, 0,
		41666667, 83333334, 125000000, 166666667,208333334, 
		40000000, 80000000, 120000000, 0, 0
	};
	// Size of the adjustment for each period
	static const int nanoPerSecondAdjustment[3] = {15, 24, 25};

	uint32_t secondsAdj, nanoSecondsAdj, numAdjustments;
	uint32_t fullSeconds, numAdjustments1, numAdjustments2;
	uint32_t r1AdjNanoseconds = getNanoseconds(r1Adj);
	int iter = 0, integralFlag = 0;
	int e13e8 = 130000000;

	/* If the unadjusted value had 0 nanoseconds, the algorithm for
		deriving the unadjusted value from the adjusted value would be
		quite simple.  We would begin by extracting the number of
		seconds from r1Adj.  If that number of seconds is less than
		1e9 / adjustement, we need simply multiply the adjustment *
		the number of seconds to determine the total adjustment.

		If, however, the number of seconds is >= 1e9 / adjustment, but
		< 2e9 / adjustment, the total adjustment will be over 1
		second.  This means the number of adjustments applied is
		actually 1 less than the number of seconds extracted from
		r1Adj.  Similarly, if the number of seconds is >= 2e9 / adjustment,
		but < 3e9 / adjustment, the total adjustment will be over
		2 seconds and the number of adjustments is 2 less than
		the number of seconds. 
		
		The algorithm becomes slightly more complicated if
		the unadjusted value had nonzero nanoseconds.  For
		exmaple, if the unadjusted value is 20,000,000 seconds
		and 800,000,000 nanoseconds and the adjustment per
		second is 15 nanoseconds, the adjustment is 300,000,000
		nanoseconds, but the total nanoseconds for the adjusted
		values is 1,100,000,000.  The carry calculation will
		add a second and reduce the number of nanoseconds to
		100,000,000.  
		
		In the above situation, the number of adjustments should
		be 1 less than the number of seconds.  Thus, whenever the
		number of nanoseconds in the unadjusted value causes
		a carry during the adjustment, the number of adjustments
		must be reduced by 1.  
		
		This situation can be identified by noting that when the
		number of nanoseconds in the unadjusted value causes a 
		carry during adjustment, the resulting number of 
		nanoseconds will be less than the number calculated by 
		multiplying the number of seconds by the adjustment 
		(and making the appropriate carries).  This is due to
		the fact that the number of nanoseconds in the unadjusted
		value must be < 1e9.  
		
		There is one special case, which also must be handled -
		when the number of seconds in the adjusted value
		equals a value where the integral number of seconds is 
		incremented - e.g.,	66,666,667 seconds with an adjustment
		of 15 nanoseconds.
		
		At 66,666,666 seconds, the total adjustment is
		999,999,990 nanoseconds.  If the number of nanoseconds in
		the unadjusted value is 0, the adjusted value is simply
		66,666,666 seconds and 999,999,990 nanoseconds.  At
		66,666,667 seconds, the total adjustment is 1 second
		plus 5 nanoseconds, so the adjusted value is 
		66,666,668 seconds and 5 nanoseconds.  
		
		Thus, if the unadjusted value has 0 nanoseconds,
		the adjusted value can never have 66,666,667 seconds.
		However, if we consider the above situation, where
		the unadjusted value has 666,666,666 seconds and
		100,000,000 nanoseconds, the adjusted value will
		be 666,666,667 seconds and 99,999,990 nanoseconds.
		Were the total adjustment to be calculated at
		666,666,666 seconds, the calculation would yield
		a value of 999,999,990 nanoseconds.  However,
		since the calculation is done at 666,666,667
		seconds, the adjustment is 1 second plus 5
		nanoseconds, so the resulting number of nanoseconds
		is only 5.  Here, even though the carry on adjustment
		was caused by the number of nanoseconds in the unadjusted
		value, it cannot be identified by comparing the number of
		nanoseconds in the adjusted value to the number of
		nanoseconds in the calculated adjustment.

		Fortunately, this situation can be identified by simply
		noting when the number of seconds in the adjusted value equals
		a value where the integral number of seconds is incremented.
		Since this can only happen when the unadjusted nanoseconds
		cause a carry on adjustment, decrement the number of
		adjustments in this case.
		*/
	// Determine the number of full seconds represented by r1Adj
	fullSeconds = getSeconds(r1Adj);

	/* The number of adjustments applied is the number of full seconds
		minus those full seconds added by adjustment.  For example,
		if the adjustment is 24 nanoseconds, after 41,666,667 seconds
		the adjustment exceeds 1 second.  */
	for (iter = 0; iter < numAddedSeconds[period]; iter++)
	{
		if (fullSeconds < addedSecondTime[period][iter] + iter + 1)
		{
			/* Have identified the number of integral seconds,
				i.e., iter, added by carry upon adjustment.
			  Check whether the number of seconds is equal to
				the value at which the number of integral seconds is
				incremented.  */
			integralFlag = (fullSeconds == addedSecondTime[period][iter] + iter);
			break;
		}
	}
	// If the loop was exited normally, fullSeconds is past the last integral
	//	value.  iter reflects this, and integralFlag remains 0.
	numAdjustments = fullSeconds - iter - integralFlag;

	/* The largest cumulative adjustment for one period is the
		over 5 second adjustment over the first part of the
		early UTC period (24 nanoseconds / second over 7
		years.  Multiplying nanoPerSecondAdjustment times
		the number of seconds will overflow a 32-bit integer
		at the end of the interval.

		The EarlyAtomic interval is 3 years, which is < 1e8 seconds.
		EarlyUTC is 7 years, which is > 2e8 seconds.
		EarlyUTC2 is 4 years, which is < 1.3e8 seconds.

		Recognizing the above, and noting that 1.3e8 * 24 = 3.12e9,
		divide intervals > 1.3e8 into two parts to avoid
		overflow. */

	numAdjustments1 = numAdjustments % e13e8;
	numAdjustments2 = (numAdjustments / e13e8) * e13e8;

	// The following value < 4.12e9, so will not overflow
	nanoSecondsAdj = nanoPerSecondAdjustment[period] * numAdjustments1;
	// The value of the following is < 3e8
	secondsAdj = nanoSecondsAdj / e9;
	nanoSecondsAdj -= (nanoSecondsAdj / e9) * e9;

	if (numAdjustments2 > 0)
	{
		nanoSecondsAdj += nanoPerSecondAdjustment[period] * numAdjustments2;
		secondsAdj += nanoSecondsAdj / e9;
		nanoSecondsAdj -= (nanoSecondsAdj / e9) * e9;
	}


	/* If r1Nanoseconds < nanoSecondsAdj, the unadjusted value must have had
		a sufficient number of nanoseconds to cause a carry when the adjustment
		was added, so reduce the adjustment.  The only time the adjustment should
		not be reduced is if fullSeconds equals the value at which another
		second is added to the adjustment.  This is becuase the adjustment
		was already reduced. */
	if ((r1AdjNanoseconds < nanoSecondsAdj) && !integralFlag) nanoSecondsAdj -= nanoPerSecondAdjustment[period];

	// Avoid negative value of nanoseconds
	if (nanoSecondsAdj > r1AdjNanoseconds)
	{
		r2 = createRelTicks(0, fullSeconds - secondsAdj - 1, e9 + r1AdjNanoseconds - nanoSecondsAdj, getAttoseconds(r1Adj), 0);
	}
	else
	{
		r2 = createRelTicks(0, fullSeconds - secondsAdj, r1AdjNanoseconds - nanoSecondsAdj, getAttoseconds(r1Adj), 0);
	}
	return r2;
}

uint8_t * dayOfYearToMonthDay(uint32_t dayOfYear, uint8_t numLeapDays)
{
	// ** Upgraded for Swedish calendar **
	// Fastest method is to reference an array
	//   Treat the arrays as 1 based
	static uint8_t monthDay[2];
	static uint8_t month[366] = 
	{0,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12};

	static uint8_t leapMonth[367] = 
	{0,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
	11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12};

	static uint8_t leapMonthSweden1712[368] =
	{ 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
		10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
		11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
		12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12 };

	static uint8_t day[366] =
	{0,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

	static uint8_t leapDay[367] =
	{0,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

	static uint8_t leapDaySweden1712[368] =
	{ 0,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
		1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 };

	switch (numLeapDays)
	{
	case 0:
		monthDay[0] = month[dayOfYear];
		monthDay[1] = day[dayOfYear];
		break;
	case 1:
		monthDay[0] = leapMonth[dayOfYear];
		monthDay[1] = leapDay[dayOfYear];
		break;
	case 2:
		monthDay[0] = leapMonthSweden1712[dayOfYear];
		monthDay[1] = leapDaySweden1712[dayOfYear];
		break;
	}
	return monthDay;
}

uint32_t monthDayToDayOfYear(uint8_t month, uint8_t day, uint8_t numLeapDays)
{
	// ** Upgraded for Swedish calendar **
	// Fastest method is to reference an array
	//   Treat the array as 1 based
	static uint32_t dayOfYear[13][32] = 
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		0,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,0,0,0,
		0,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
		0,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,0,
		0,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,
		0,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,0,
		0,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,
		0,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,
		0,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,0,
		0,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,
		0,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,0,
		0,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365
	};

	static uint32_t leapDayOfYear[13][32] = 
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		0,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,0,0,
		0,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,
		0,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,0,
		0,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,
		0,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,0,
		0,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,
		0,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,
		0,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,0,
		0,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,
		0,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,0,
		0,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366
	};
	// 1712 in Sweden had two leap days - February 29th and 30th
	static uint32_t leapDayOfYearSweden1712[13][32] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
		0,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,0,
		0,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,
		0,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,0,
		0,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,
		0,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,0,
		0,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,
		0,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,
		0,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,0,
		0,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,
		0,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,0,
		0,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367
	};
	switch (numLeapDays)
	{
	case 0:
		return dayOfYear[month][day];
	case 1:
		return leapDayOfYear[month][day];
	case 2:
		return leapDayOfYearSweden1712[month][day];
	}
	// Should not arrive here
	return 0;
}

uint8_t numLeapDays(int8_t gigayear, int32_t year, uint32_t calendar)
{
	// Determines whether a year is a leap year.
	DateCoords ljd;
	//	Returns 0 for non-leap year, 1 for ordinary leap year, 2 for double leap year, i.e., Sweden in 1712
	//	 Calendar = 0 indicates Julian until 4 Oct 1582 and Gregorian from 15 Oct 1582
	//			    1 indicates Julian for the entire range
	//				2 indicates Swedish calendar
	//				> 10000000  transition from Julian to Gregorian encoded in calendar
	// The earliest transition to the Gregorian calendar was in 1582.  The first missed leap year was 1700.
	//	As such, Julian rules could be used to identify leap years prior to 1700 in all cases.
	//	If Julian calendar is specified, independent of the date, use Julian rules
	if (gigayear < 0 || (gigayear == 0 && year < 1700) || (calendar == 1))
	{
		// Use Julian rules
		return (year % 4 == 0);
	}
	// If calendar == 0, begin using Gregorian rules in 1700.
	//	Also, for all but calendar == 1, use Gregorian rules after the lastest transition date
	else if ((calendar == 0 && gigayear == 0 && year >= 1700) || 
		year > (int)LatestFirstGregorianDate.year || gigayear > 0 )
	{
		// Use Gregorian rules
		return ((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0;
	}
	// If we have gotten to this point, we know gigayear == 0, 1700 <= year <= lastestTransitionDate, and calendar > 1
	// The Swedes planned to transition from Julian to Gregorian gradually.  In 1699, they decided there would be no leap years
	//	from 1700 to 1743, which would have the effect of skipping the required 11 days to accomplish the transition.  There
	//	was no leap day in 1700, but, due to war, 1704 and 1708 did have leap days.  In 1711, the Swedish
	//	government changed the approach due to the disruption caused by the inconsisitencies with other countries.  To get
	//	back on track, two leap days, February 29th and February 30th, were added in 1712.  At this point, Sweden was
	//	consistent with the Julian calendar.  On February 17th, 1753, Sweden transitioned to the Gregorian calendar by
	//	skipping 11 days, making the next day March 1, 1753.
	else if (calendar == 2)
	{
		// No leap year in 1700
		if (year == 1700) return 0;
		// 1704 and 1708 are leap years
		if (year < 1712) return (year % 4 == 0);
		// 1712 is a double leap year
		if (year == 1712) return 2;
		// Use Gregorian for the rest of the range
		return ((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0;
	}
	// At this point,  we know gigayear == 0, 1700 <= year <= latestTransitionDate, and the transition date is encoded in the calendar.
	//  For years after the transition year, use the
	//	Gregorian rules.  For years before the transition, use the Julian rules.  For the year of the transition,
	//	if the last Julian date is on or before February 28, use the Gregorian rules, as the transition was
	//	completed before a potential leap day.  If the last Julian date is on or after February 29th, use the Julian rules,
	//	as the leap day would have occurred prior to the transition.
	//	To illustrate, consider the case where the transition occurs in 1700.  If the transition were to occur on 2/17/1700,
	//	we would skip 10 days, so the next day would be 2/28/1700 Gregorian and the day after that 3/1/1700 Gregorian.
	//	If the transition occurred on 2/18/1700, the next date would be 3/1/1700.  In these cases, we are on the Gregorian
	//	calendar prior to the non-existent 2/29/1700 Gregorian, and the year has no leap day.
	//	If the transition occurred on 2/28/1700, the next date would be 3/11/1700.  Days 3/1/1700 through 3/10/1700 are
	//	skipped, and no leap day is assumed.  If the transition occurred on 2/29/1700, the leap days has already occurred, so the
	//	year has a leap day.  The next date is 3/12/1700, as days 3/1/1700 through 3/11/1700 are skipped.
	//
	else
	{
		ljd = lastJulianDate(calendar);
		if (year < ljd.year) return (year % 4 == 0);
		else if (year > ljd.year) 
			return ((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0;
		else
		{
			// Within the transition year
			if (ljd.month < 2 || (ljd.month == 2 && ljd.dayOfMonth < 29))
				return ((year % 4 == 0) && (year % 100 != 0)) || year % 400 == 0;
			else return (year % 4 == 0);
		}
	}
	// Should not get here
	return 0;
}

int isLeapSecondDay(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth)
{
	// Determine whether a UTC date has a leap second
	int i;
	if (gigayear != 0 || year < 1972) return 0;

	// Loop through all leap second dates looking for a match
	for(i = 0; i < numLeapSeconds; i++)
	{
		if ( (year == UTCLeapDates[i][0]) && (month == UTCLeapDates[i][1]) &&
				(dayOfMonth == UTCLeapDates[i][2]) ) return 1;
	}
	return 0;
}

int cumLeapSecondsUnadj(TAITicks unadjt1, int isPotentialLeapSecond)
{
	/* Returns the cumulative leap seconds as of an unadjusted, i.e.,
		for discontinuities from 1 Jan 1958 through the leap second
		period, tai.  If tai is not a leap second, return the
		cumulative number of leap seconds prior to tai.
		
		If the UTC time was specifed with second == 60, the second
		is intended to be a leap second.  This is indicated to this
		fuction by isPotentialLeapSecond being true.  

		If tai is a leap second, and isPotentialLeapSecond is true,
		return the negative number of cumulative leap seconds represented
		by this second. In this case, the UTC time is 23:59:60.  Since this
		is a leap second and the cumulative number of leap seconds is
		the value at the start of this second, the value is actually 1
		less.  However, if we used negative of the correct value,
		instead of the correct value + 1, we'd have a problem for
		the first leap second, since the cumulative leap seconds is
		0, and we can't express negative 0.
		
		If tai is a leap second, but isPotentialLeapSecond is not true,
		return the number of leap seconds represented by this
		second.  In this case, the UTC time is 00:00:00.

		If isPotentialLeapSecond is true,
		but the second is not a leap second, return the
		cumulative number of leap seconds + 1e9.  
		
		If the time is after the last leap second,
		return the cumulative number of leap seconds
		+ 2e9.  This indicates that future leap seconds
		may be added after the last leap second, but 
		before the specified time.  This information
		is required to implement the future adjustment
		logic.  */

	// Leap second data structures
	static int initialized = 0;
	int i;
	int cumLeap;
	static TAIRelTicks oneSecond;
	static TAITicks unadjLeapSeconds[numLeapSeconds]; // Unadjusted tick count at start of leap second

	// Only call this once
	if (!initialized)
	{
		for(i = 0; i < numLeapSeconds; i++)
		{
			unadjLeapSeconds[i] = createTicks(leapTicks[i][0], leapTicks[i][1], 0, 0);
		}
		oneSecond = createRelTicks(0, 1, 0, 0, 0);
		initialized = 1;
	}

	// Check whether the time is within the range where leap seconds are specified.
	if (isLessTicks(unadjt1, unadjLeapSeconds[0]))
	{
		return 0;
	}
	if (isGreaterOrEqualTicks(unadjt1, addRelTicksToTicks(unadjLeapSeconds[numLeapSeconds - 1], oneSecond)))
	{
		return numLeapSeconds + 2*e9;
	}
	if (isGreaterOrEqualTicks(unadjt1, unadjLeapSeconds[numLeapSeconds - 1]) &&
		isLessTicks(unadjt1, addRelTicksToTicks(unadjLeapSeconds[numLeapSeconds - 1], oneSecond)))
	{
		// Time equals last leap second 
		if (isPotentialLeapSecond)
		{
			// This is the leap second
			return -numLeapSeconds;
		}
		else
		{
			// This is the second after the leap second
			return numLeapSeconds;
		}
	}
	for(i = 0; i < numLeapSeconds - 1; i++)
	{
		if ( isGreaterOrEqualTicks(unadjt1, unadjLeapSeconds[i]) && 
			 isLessTicks(unadjt1, unadjLeapSeconds[i+1]) )
		{
			// Time is within this interval
			if ( isGreaterOrEqualTicks(unadjt1, addRelTicksToTicks(unadjLeapSeconds[i], oneSecond)))
			{
				// Time is after ith leap second
				cumLeap = i + 1;
				// If this was intended to be a leap second, return a flag
				//	indicating it is not.
				if (isPotentialLeapSecond) cumLeap += e9;
				return cumLeap;
			}
			else
			{
				// Time equals leap second 
				if (isPotentialLeapSecond)
				{
					// This is the leap second
					return -(i + 1);
				}
				else
				{
					// This is the second after the leap second
					return (i + 1);
				}
			}
		}
	}
	// Cannot get here
	return 0;
}

int cumLeapSecondsAdj(TAITicks adjt1)
{
	/* Returns the cumulative leap seconds as of an adjusted, i.e.,
		for discontinuities from 1 Jan 1958 through the leap second
		period, tai.  If tai is not a leap second, return the
		cumulative number of leap seconds prior to tai.
		
		If tai is a leap second, return the negative number of 
		cumulative leap seconds represented	by this second. 
		In this case, the UTC time is 23:59:60.  Since this
		is a leap second and the cumulative number of leap seconds is
		the value at the start of this second, the value is actually 1
		less.  However, if we used negative of the correct value,
		instead of the correct value + 1, we'd have a problem for
		the first leap second, since the cumulative leap seconds is
		0, and we can't express negative 0. */

	// Leap second data structures
	static int initialized = 0;
	int i;
	int cumLeap;
	static TAIRelTicks oneSecond;
	static TAITicks adjLeapSeconds[numLeapSeconds]; // Adjusted tick count at start of leap second

	// Only call this once
	if (!initialized)
	{
		for(i = 0; i < numLeapSeconds; i++)
		{
			adjLeapSeconds[i] = createTicks(leapTicks[i][0], leapTicks[i][1] + 10 + i, 0, 0);
		}
		oneSecond = createRelTicks(0, 1, 0, 0, 0);
		initialized = 1;
	}

	// Check whether the time is within the range where leap seconds are specified.
	if (isLessTicks(adjt1, adjLeapSeconds[0]))
	{
		return 0;
	}
	if (isGreaterOrEqualTicks(adjt1, addRelTicksToTicks(adjLeapSeconds[numLeapSeconds - 1], oneSecond)))
	{
		return numLeapSeconds;
	}
	if (isGreaterOrEqualTicks(adjt1, adjLeapSeconds[numLeapSeconds - 1]) &&
		isLessTicks(adjt1, addRelTicksToTicks(adjLeapSeconds[numLeapSeconds - 1], oneSecond)))
	{
		// Time equals last leap second 
		return -numLeapSeconds;
	}

	for(i = 0; i < numLeapSeconds - 1; i++)
	{
		if ( isGreaterOrEqualTicks(adjt1, adjLeapSeconds[i]) && 
			 isLessTicks(adjt1, adjLeapSeconds[i+1]) )
		{
			// Time is within this interval
			if (isGreaterOrEqualTicks(adjt1, addRelTicksToTicks(adjLeapSeconds[i], oneSecond)))
			{
				// Time is after ith leap second
				cumLeap = i + 1;
				return cumLeap;
			}
			else
			{
				// Time equals leap second 
				return -(i + 1);
			}
		}
	}
	// Cannot get here
	return 0;
}

//UTCDatetime calcTicks_old(UTCDatetime utc, uint32_t nanosecond, uint32_t attosecond)
//{
//	// Return a copy of utc, calculating tai and updating futureAdjust and taiInit, as necessary
//	//  This function is called on the initial creation of a UTCDatetime or when 
//	//		futureAdjust == 1.
//	 UTCDatetime u2 = utc;
//	 int dayOfYear;  // day of Year
//	 int isLeapYr;
//	 uint32_t gigaseconds;
//	 uint32_t megaseconds, kiloseconds;
//	 uint32_t seconds;
//	 uint32_t tempseconds;
//	 uint32_t full4Year;
//	 uint32_t fullGigaYears, full1KYears, full100Years, full1200Years;
//	 int firstYear;
//	 int remYears, carry, borrow;
//	 int cumLeapSecs = 0;
//	 uint32_t borrowGiga;
//
//	 int32_t oct4 = monthDayToDayOfYear(10, 4, 0);
//	 static const uint32_t secondsIn4JulianYears = 126230400; // 1461 * secondsPerDay; 
//	 static const uint32_t lengthOfCreation = 561600; // 6 * secondsPerDay + secondsPerDay / 2;
//
//	 // Julian year initializations - 4 year cycle with one leap year every 4 years
//	 //   There are 2 different year lengths, but every 4 year cycle is the same length
//
//	 // In order to avoid theese calculations being performed on every call, the variables need to be static.
//	 //		However, calculations can not be performed on the initialization of static variables, so
//	 //			just assign to a precomputed value.
//
//	 //uint32_t gigaSecsInJulianGigaYear = secondsIn4JulianYears / 4; // Value is an integer since seconds in 4 years is multiple of 400 - 31,557,600
//	 //
//	 //uint32_t gigaSecsInJulian1KYear = gigaSecsInJulianGigaYear / 1000000; // Value is truncated - 31(.5576)
//	 //uint32_t megaSecsInJulian1KYear = (gigaSecsInJulianGigaYear - 1000000*gigaSecsInJulian1KYear)/1000;  // Remaining number of mega seconds, truncated - 557(.6)
//	 //uint32_t kiloSecsInJulian1KYear = gigaSecsInJulianGigaYear - 1000000*gigaSecsInJulian1KYear - 1000*megaSecsInJulian1KYear; // Remaining number of kilosecs - 600
//
//	 //uint32_t gigaSecsInJulian100Year = gigaSecsInJulianGigaYear / 10000000; // Value is truncated - 3(.15576)
//	 //uint32_t megaSecsInJulian100Year = (gigaSecsInJulianGigaYear - 10000000*gigaSecsInJulian100Year)/10000;  // Remaining number of mega seconds, truncated
//																										  //	155(.76)
//	 //uint32_t kiloSecsInJulian100Year = (gigaSecsInJulianGigaYear - 10000000*gigaSecsInJulian100Year - 
//	 //										10000*megaSecsInJulian100Year) / 10; // Remaining number of kilosecs
//																				 //   760
//	 static const uint32_t gigaSecsInJulianGigaYear = 31557600;
//	 static const uint32_t gigaSecsInJulian1KYear = 31;
//	 static const uint32_t megaSecsInJulian1KYear = 557;
//	 static const uint32_t kiloSecsInJulian1KYear = 600;
//	 static const uint32_t gigaSecsInJulian100Year = 3;
//	 static const uint32_t megaSecsInJulian100Year = 155;
//	 static const uint32_t kiloSecsInJulian100Year = 760;
//
//	 /* Gregorian year initializations - 400 year cycle with leap years every 4 years except centuries not divisible by 400
//	    As for Julian years, there are 2 different year lenghts, however the cycle is 400 years, not 4 years
//	 		Given that the number of seconds in 400 years > 12e9, we need a slightly different approach than that used for
//	 		Julian years.  Specifically, we will consider an "average" Gregorian year, which has 365.2425 days, so has
//			a length of 31,556,952 seconds. */
//
//	 //uint32_t secondsInAverageGregorianYear = (303*365 + 97*366)*864/4; // 31,556,952
//	 //uint32_t gigaSecsInGregorianGigaYear = secondsInAverageGregorianYear; // Value is an integer
//	 //	A Gregorian 1KYear is not an acceptable unit, since it is not a multiple of 400.  As
//	 //		such, use Grigroian 1200 Year.
//	 //  Number of seconds in Gregorian1200Year is 1200 * 31,556,952 = 37,868,342,400
//	 //uint32_t gigaSecsInGregorian1200Year = 37
//	 //uint32_t megaSecsInGregorian1200Year = 868
//	 //uint32_t kiloSecsInGregorian1200Year = 342
//	 //uint32_t secsInGregorian1200Year = 400
//
//	// The following computation is for a 100 year period without a leap year in the initial year
//	 //uint32_t secondsInGregorianCentury = (76*365+ 24*366) * 86400; // 3,155,673,600
//	 //uint32_t gigaSecsInGregorian100Year = secondsInGregorianCentury / e9; // Value is truncated - 3(.1556736)
//	 //uint32_t megaSecsInGregorian100Year = (secondsInGregorianCentury - e9*gigaSecsInGregorian100Year)/1000000;
//																				  // Remaining number of mega seconds, truncated - 155(.6736)
//	 //uint32_t kiloSecsInGregorian100Year = (secondsInGregorianCentury - e9*gigaSecsInGregorian100Year 
//	 //										- 1000000*megaSecsInGregorian100Year) / 1000; // Remaining number of kilosecs, truncated - 673(.6)
//	 //uint32_t secsInGregorian100Year = secondsInGregorianCentury - e9*gigaSecsInGregorian100Year 
//	 //										- 1000000*megaSecsInGregorian100Year - 1000*kiloSecsInGregorian100Year; // Remaining number of seconds - 600
//	 
////	 static const uint32_t secondsInAverageGregorianYear = 31556952;
//	 static const uint32_t gigaSecsInGregorianGigaYear = 31556952;
//
//	 static const uint32_t gigaSecsInGregorian1200Year = 37;
//	 static const uint32_t megaSecsInGregorian1200Year = 868;
//	 static const uint32_t kiloSecsInGregorian1200Year = 342;
//	 static const uint32_t secsInGregorian1200Year = 400;
//
//	// The following computation is for a 100 year period without a leap year in the initial year
////	 static const uint32_t secondsInGregorianCentury = 3155673600;
//	 static const uint32_t gigaSecsInGregorian100Year = 3;
//	 static const uint32_t megaSecsInGregorian100Year = 155;
//	 static const uint32_t kiloSecsInGregorian100Year = 673;
//	 static const uint32_t secsInGregorian100Year = 600;
//
//
//	 TAIRelTicks r1, r1adj;
//
//	 // Initialize tai
//	 u2.tai = Creation;
//	 // For any datetime outside the leap second period, futureAdjust = 0.
//	 //  Initialize to zero, and reset if necessary.
//	 u2.futureAdjust = 0;
//
//	 // If all of the fields are valid, perform the calculations
//	if (utc.taiInit == 0)
//	{
//		// TAIticks has not been computed for this UTCDatetime
//
//		// Datetime is after the Big Bang based on having passed the checks
//		//	in createUTCDatetime
//
//		//  Detemine the period
//		if (utc.gigayear == -13 && utc.year == bbYear)
//		{
//			// Within the Creation period
//			if (utc.dayOfMonth == 25)
//			{
//				gigaseconds = 0;
//				seconds = (utc.hour - 12) * 3600 + utc.minute * 60 + utc.second;
//				u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
//				return u2;
//			}
//			else
//			{
//				// Between the 26th and 31st
//				gigaseconds = 0;
//				seconds = secondsPerDay / 2 + (utc.dayOfMonth - 26) * secondsPerDay + 
//					utc.hour * 3600 + utc.minute * 60 + utc.second;
//				u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
//				return u2;
//			}
//		}
//		if  ((utc.gigayear < 0) || 
//			(utc.gigayear == 0 && utc.year < -4712))  // Before 4713 BCE
//		{
//			// Within the Prehistoric period
//			/*  Julian calendar has 4 year repeating periods, each of length 3*365 + 366 = 1461 days
//			     Begin by determining the number of gigaseconds contained in the gigayears. */
//			fullGigaYears = utc.gigayear + 13;
//			borrow = 0;
//			if (utc.year <= bbYear) // Period starts 1 Jan -13,826,151,187.
//			{						//   To be a full billion years, must be
//				 fullGigaYears --;	// at least within -12,826,151,187.
//				 borrow = e9;
//			}
//			gigaseconds = gigaSecsInJulianGigaYear * fullGigaYears;
//
//			// Determine the number of 1K year periods remaining
//			//  The numbers of giga, mega and kilo in 1K year is each < 1000.
//			//		Therefore, can multiply each by 1e6, without an overflow
//
//			full1KYears = (utc.year + borrow - bbYear - 1) / 1000;
//			gigaseconds += full1KYears * gigaSecsInJulian1KYear;
//			megaseconds = full1KYears * megaSecsInJulian1KYear;
//			kiloseconds = full1KYears * kiloSecsInJulian1KYear;
//			remYears = utc.year + borrow - bbYear - 1 - 1000*full1KYears;
//
//			// Determine the number of 100 year periods remaining
//			full100Years = remYears / 100;
//			gigaseconds += full100Years * gigaSecsInJulian100Year;
//			megaseconds += full100Years * megaSecsInJulian100Year;
//			kiloseconds += full100Years * kiloSecsInJulian100Year;
//			remYears -= 100*full100Years;
//
//			// Perform carries
//			carry = kiloseconds / 1000;
//			megaseconds += carry;
//			kiloseconds -= carry*1000;
//
//			carry = megaseconds / 1000;
//			gigaseconds += carry;
//			megaseconds -= carry*1000;
//
//			// Compute seconds from megaseconds and kiloseconds
//			tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000
//
//			// Determine the number of 4 year periods remaining
//			full4Year = remYears / 4;
//			remYears -= 4*full4Year;
//
//			// Less than a century remains, so the number of seconds fits in 32 bits
//			//   The 4th year in the cycle is a leap year, so all remaining years are non leap
//			seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap;
//
//			// Perform carry
//			carry = seconds/e9;
//			gigaseconds += carry;
//			seconds -= carry*e9;
//
//			// Add in tempseconds 
//			seconds += tempseconds;
//
//			// Remaining number of seconds is not enough to require any more carries.
//			//	The createTicks function can take seconds > 1e9
//
//			// Add in the day of year, hours, minutes and seconds
//			//  If there are 3 years remaining, the date is within a leap year
//			isLeapYr = numLeapDays(utc.gigayear, utc.year, utc.calendar);
//			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
//			seconds += (dayOfYear - 1) * secondsPerDay+ utc.hour * 3600 + utc.minute * 60 + utc.second
//						+ lengthOfCreation;
//
//			// Perform carry
//			carry = seconds/e9;
//			gigaseconds += carry;
//			seconds -= carry*e9;
//
//			u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
//			return u2;
//		}
//		// 1582 is not a leap year
//		dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, 0);
//		if ( (utc.gigayear == 0 && utc.year < 1582)  ||  // Before 1582
//			 (utc.gigayear == 0 && utc.year == 1582 && dayOfYear <= oct4) || // Before change to Gregorian calendar
//			 (utc.gigayear == 0 && utc.year < 1923 && utc.calendar == 1) ) // Latest usage of Julian calendar
//		{
//			// Within the Julian period - 4713 BCE is a leap year
//			remYears = utc.year + 4712;
//
//			// Determine the number of 100 year periods remaining
//			full100Years = remYears / 100;
//			gigaseconds = full100Years * gigaSecsInJulian100Year;
//			megaseconds = full100Years * megaSecsInJulian100Year;
//			kiloseconds = full100Years * kiloSecsInJulian100Year;
//			remYears -= 100*full100Years;
//
//			// Perform carries
//			carry = kiloseconds / 1000;
//			megaseconds += carry;
//			kiloseconds -= carry*1000;
//
//			carry = megaseconds / 1000;
//			gigaseconds += carry;
//			megaseconds -= carry*1000;
//
//			// Compute seconds from megaseconds and kiloseconds
//			tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000
//
//			// Determine the number of 4 year periods remaining
//			full4Year = remYears / 4;
//			remYears -= 4*full4Year;
//
//			// Less than a century remains, so the number of seconds fits in 32 bits
//			seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap;
//			// The first remaining year is a leap year, since 4713 BCE is a leap year
//			if (remYears > 0) seconds += secondsPerDay;
//
//			// Perform carry
//			carry = seconds/e9;
//			gigaseconds += carry;
//			seconds -= carry*e9;
//
//			// Add in tempseconds 
//			seconds += tempseconds;
//
//			// Remaining number of seconds is not enough to require any more carries.
//			//	The createTicks function can take seconds > 1e9
//
//			// Add in the day of year, hours, minutes and seconds
//			//  If there are no years remaining, the date is within a leap year
//			isLeapYr = numLeapDays(utc.gigayear, utc.year, utc.calendar);
//			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
//			seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;
//
//			// Create a relative ticks value then add to the start date of the period
//			u2.tai = addRelTicksToTicks(Julian,   createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
//			return u2;
//		}
//		if (utc.gigayear == 0 && utc.year < 2400)
//		{
//			/* Within the Gregorian period.  Starting 1600, the calendar repeats in
//				400 year cycles.  The mapping to TAI also repeats in 400 year cycles,
//				except for the discontinuties introduced between 1 Jan 1958 and 1 Jan 1972, then
//				after 1 Jan 1972 with leap seconds.  As of the date this code was written, Jan, 2015,
//				the last known leap second was 30 Jun 2015.  Thus, the last block of 400 years
//				to have discontinuities is from 2000 - 2399.  Treat the period after that
//				separately.  */
//			// Check for the invalid period
//			//if (utc.year == 1582 && utc.month == 10 && utc.dayOfMonth > 4 && utc.dayOfMonth < 15
//			//		&& utc.calendar != 1)
//			//{
//			//	// Invalid date - calendar skips from Oct 4 to Oct 15, 1582.  Set to zero.
//			//	u2.tai = createTicks(0, 0, 0, 0);
//			//	u2.taiInit = 1; // Indicate tai has been initialized to avoid needless recalcs
//			//	return u2;
//			//}
//
//			//  Handle the end of 1582
//			if (utc.year == 1582)
//			{
//				dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, 0);
//				gigaseconds = 0;
//				seconds = (dayOfYear - monthDayToDayOfYear(10, 15, 0)) * secondsPerDay+ utc.hour * 3600 + utc.minute * 60 + utc.second;
//
//				// Create a relative ticks value then add to the start date of the period
//				u2.tai = addRelTicksToTicks(Gregorian,   createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
//				return u2;
//			}
//			//  Handle the end of the 16th century
//			if (utc.year < 1600)
//			{
//				remYears = utc.year - 1583;
//				full4Year = remYears / 4;
//				remYears -= 4*full4Year;
//				// The second year in the period is a leap year
//				seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap; // 17 years of seconds < 1e9
//				if (remYears > 1) seconds += secondsPerDay;
//
//				isLeapYr = numLeapDays(utc.gigayear, utc.year, utc.calendar); // There are no intervening centuries in this period
//				seconds +=  (monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr) - 1) * secondsPerDay +
//								utc.hour * 3600 + utc.minute * 60 + utc.second;
//				// Add in the seconds from Oct 15, 1582 through Oct 31, 1582 so Gregorian can be used as starting point
//				gigaseconds = 0;
//				seconds += 78*secondsPerDay;
//
//				// Create a relative ticks value then add to the start date of the period
//				u2.tai = addRelTicksToTicks(Gregorian,   createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
//				return u2;
//			}
//			else
//			{
//				// Find the number of centuries
//				remYears = utc.year - 1600;
//				full100Years = remYears / 100;
//				gigaseconds = full100Years * gigaSecsInJulian100Year;
//				megaseconds = full100Years * megaSecsInJulian100Year;
//				kiloseconds = full100Years * kiloSecsInJulian100Year;
//				remYears -= 100*full100Years;
//				// Perform carries
//				carry = kiloseconds / 1000;
//				megaseconds += carry;
//				kiloseconds -= carry*1000;
//
//				carry = megaseconds / 1000;
//				gigaseconds += carry;
//				megaseconds -= carry*1000;
//
//				// Compute seconds from megaseconds and kiloseconds
//				tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000
//
//				// During this time period, only the first and fifth centuies are full length.
//				//		The others are 1 day short.
//				if (full100Years > 1)
//				{
//					tempseconds -= (full100Years - 1) * secondsPerDay;
//				}
//				if (full100Years >= 5)
//				{
//					tempseconds += secondsPerDay;
//				}
//				// Find the number of 4 year periods remaining
//				full4Year = remYears / 4;
//				seconds = full4Year * secondsIn4JulianYears;
//
//				// The first 4 year period is 1 day short, unless the start of that period is a multiple of 400
//				if (full4Year > 0 && ( (utc.year / 100) % 4 != 0) ) seconds -= secondsPerDay;
//				remYears -= 4*full4Year;
//				// All remaining years, except, possibly the first is a non leap year
//				// The first remaining year is a leap year, unless it is a multiple of 100 but not of 400
//				seconds += remYears * secondsInNonLeap;
//				if (remYears > 0)
//				{
//						firstYear = utc.year - remYears;
//						if (numLeapDays(utc.gigayear, firstYear, utc.calendar)) seconds += secondsPerDay;
//				}
//
//				// Perform carry
//				carry = seconds/e9;
//				gigaseconds += carry;
//				seconds -= carry*e9;
//
//				// Add in tempseconds 
//				seconds += tempseconds;
//
//				// Remaining number of seconds is not enough to require any more carries.
//				//	The createTicks function can take seconds > 1e9
//
//				// Add in the day of year, hours, minutes and seconds
//				//  If there are no years remaining, the date is within a leap year
//				isLeapYr = numLeapDays(utc.gigayear, utc.year, utc.calendar); // There are no intervening centuries in this period
//				dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
//				seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;
//
//				// Create a relative ticks value then add to the start date of the period
//				u2.tai = addRelTicksToTicks(Start1600,   createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
//
//				// Handle the early atomic period
//				if (utc.year > 1957 && utc.year < 1961)
//				{
//					/* Correct for the drift.  Need to add a fraction of 1.422818 seconods
//						to every second.  The amount to be added is 15 nanoseconds for each full
//						second elapsed.  The time is effectively
//						added at the start of the next second.  So, for the first second, there is
//						no adjustment, while the second second has an adjustment of 15 nanoseconds.
//						This will result in a jump of 2.402 milliseconds at the end of the interval. */
//					r1 = diffTicks(u2.tai, EarlyAtomic);
//					r1adj = adjustRelTicks(r1, 15);
//					u2.tai = addRelTicksToTicks(EarlyAtomic, r1adj);
//				}
//				// Handle the first part of the early UTC period
//				if (utc.year > 1960 && utc.year < 1968)
//				{
//					/* Correct for the drift.  Need to add a fraction of 5.3021952 seconds
//						to every second.  The amount to be addedd is 24 nanoseconds for each full
//						second elapsed.  The time is effectively
//						added at the start of the next second.  So, for the first second, there is
//						no adjustment, while the second second has an adjustment of 24 nanoseconds.
//						This will result in a jump of 11.4688 milliseconds at the end of the interval.
//
//						Since tai is unadjusted, i.e., does not consider the 1.422818 seconds
//						that were added for the early atomic period, use an unadjusted
//						value of EarlyUTC to calculate the elapsed seconds.*/
//		
//					r1 = diffTicks(u2.tai, EarlyUTC);
//					r1adj = adjustRelTicks(r1, 24);
//					u2.tai = addRelTicksToTicks(EarlyUTC, r1adj);
//
//					// Create a relative tick value for the adjustment from 1 Jan 1958 to 1 Jan 1961
//					r1 = createRelTicks(0, 1, 422818000, 0, 0);
//					//  Add this realtive tick value to tai
//					u2.tai = addRelTicksToTicks(u2.tai, r1);
//				}
//				// Handle the second part of the early UTC period
//				if (utc.year > 1967 && utc.year < 1972)
//				{
//					/* Correct for the drift.  Need to add a fraction of 3.15576 seconds
//						to every second.  The amount to be addedd is 25 nanoseconds for each full
//						second elapsed.  The time is effectively
//						added at the start of the next second.  So, for the first second, there is
//						no adjustment, while the second second has an adjustment of 24 nanoseconds.
//						This will result in a jump of  0.107758 seconds at the end of the interval,
//						which is what occurred in practice.
//
//						Since tai is unadjusted, i.e., does not consider the 1.422818 seconds
//						that were added for the early atomic period, use an unadjusted
//						value of EarlyUTC to calculate the elapsed seconds.*/
//		
//					r1 = diffTicks(u2.tai, EarlyUTC2);
//					r1adj = adjustRelTicks(r1, 25);
//					u2.tai = addRelTicksToTicks(EarlyUTC2, r1adj);
//
//					// Create a relative tick value for the adjustment from 1 Jan 1958 to 1 Jan 1968
//					r1 = createRelTicks(0, 6, 736482000, 0, 0);
//					//  Add this realtive tick value to tai
//					u2.tai = addRelTicksToTicks(u2.tai, r1);
//				}
//				// Handle the leap second period
//				if (utc.year >= 1972)
//				{
//					/* Add a relative ticks value = 10 + elapsed leap seconds
//						 The current tai is unadjusted for the discontinuities.
//						 The leap second table contains both adusted and 
//						 unadjusted values.  The adjustment is determined by
//						 comparing tai to the unadjusted value.
//						 
//						 We must account for the fact that the
//						 unadjusted tai for a leap second, 
//						 e.g., 30 Jun 2012 23:59:60 UTC, is the
//						 same as for the second after the leap second,
//						 e.g., 1 Jul 2012 00:00:00 UTC. */
//					cumLeapSecs = cumLeapSecondsUnadj(u2.tai, utc.second == 60);
//					// Create a relative tick value for the adjustment.
//					if (cumLeapSecs >= 0)
//					{
//						// Adjust by 10 seconds for 1958 - 1972 plus the number of leap seconds
//						//	  If utc.second == 60, and this is not a leap second, cumleapsecs
//						//		is incremented by 1e9.
//						seconds = 10 + cumLeapSecs % e9;
//						// If the datetime is after the last leap second, futureAdjust must be 1 or 2
//						if (cumLeapSecs >= 2*e9) 
//						{
//							if (utc.futureAdjust == 0)
//							{
//								// Future adjustment specification error
//								u2.taiInit |= InvalidFutureAdjust;
//								u2.tai = Creation;
//								return u2;
//							}
//							else
//							{
//								// Preserve the value - either 1 or 2
//								u2.futureAdjust = utc.futureAdjust;
//							}
//						}
//					}
//					else
//					{
//						// This is a leap second.  Cumulative leap seconds is 
//						//	- (cumleapseconds + 1).  One has been added, since
//						//	  we can't have a value of -0, which would be the
//						//    value at the first leap second if we didn't add 1.
//						seconds = 10 - cumLeapSecs - 1;
//					}
//					// Create a relative tick value for the adjustment
//					r1 = createRelTicks(0, seconds, 0, 0, 0);
//					//  Add this realtive tick value to tai
//					u2.tai = addRelTicksToTicks(u2.tai, r1);
//				}
//				return u2;
//			}
//		}
//		if (utc.gigayear > 0 || (utc.gigayear == 0 && utc.year >= 2400))
//		{
//			// Post Leap Second period
//			/*	Leap seconds are determined 6 months in advance.  The first leap second
//				  for this period won't be determined until at least 2399, so no reason
//				  to check for intervening leap seconds.  As there are no discontinuities,
//				  we can assume 400 year cycles */
//			fullGigaYears = utc.gigayear;
//			borrowGiga = 0;
//			if (fullGigaYears > 0 && utc.year < 2400)
//			{
//				fullGigaYears --; // Subtract 1 if year < 2400
//				borrowGiga++;
//			}
//			gigaseconds = gigaSecsInGregorianGigaYear * fullGigaYears;
//
//			// Determine the number of 1K year periods remaining
//			//  The numbers of giga, mega and kilo in 1K year is each < 1000.
//			//		Therefore, can multiply each by 1e6, without an overflow
//
//			full1200Years = (utc.year + borrowGiga*e9 - 2400) / 1200;
//			gigaseconds += full1200Years * gigaSecsInGregorian1200Year;
//			megaseconds = full1200Years * megaSecsInGregorian1200Year;
//			kiloseconds = full1200Years * kiloSecsInGregorian1200Year;
//			seconds = full1200Years * secsInGregorian1200Year;
//			remYears = utc.year + borrowGiga*e9 - 2400 - 1200*full1200Years;
//
//			// Determine the number of 100 year periods remaining
//			full100Years = remYears / 100;
//			gigaseconds += full100Years * gigaSecsInGregorian100Year;
//			megaseconds += full100Years * megaSecsInGregorian100Year;
//			kiloseconds += full100Years * kiloSecsInGregorian100Year;
//			seconds += full100Years * secsInGregorian100Year;
//
//			// Since we are starting from 2400, the 1st, 5th and
//			//	9th centuries need an extra day, since they
//			//  begin with leap years
//			seconds += (full100Years / 4) * secondsPerDay;
//			if ( (full100Years % 4) > 0) seconds += secondsPerDay;
//
//			remYears -= 100*full100Years;
//
//			// Perform carries
//			carry = seconds / 1000;
//			kiloseconds += carry;
//			seconds -= carry*1000;
//
//			carry = kiloseconds / 1000;
//			megaseconds += carry;
//			kiloseconds -= carry*1000;
//
//			carry = megaseconds / 1000;
//			gigaseconds += carry;
//			megaseconds -= carry*1000;
//
//			// Compute seconds from megaseconds, kiloseconds and seconds
//			tempseconds = megaseconds * 1000000 + kiloseconds * 1000 + seconds;  // Max value is 999,999,000
//
//			// Determine the number of 4 year periods remaining
//			full4Year = remYears / 4;
//			remYears -= 4*full4Year;
//			seconds = full4Year * secondsIn4JulianYears;
//			// Adjust for Gregorian convention
//			if (full4Year > 0)  // If there are no full4year periods, adjustment is made in next section
//			{
//				// Determine whether the century preceding the date was a multiple of 400
//				if ( (utc.year / 100) % 4 != 0 ) seconds -= secondsPerDay;
//			}
//			
//			// The first year in the 4 year cycle is a leap year, unless
//			//	it is a multiple of 100, but not of 400.
//			// Determine whether the first remaining year is a leap year.
//			if (remYears > 0)
//			{
//				seconds += remYears * secondsInNonLeap;
//				firstYear = utc.year - (utc.year % 4);
//				isLeapYr = numLeapDays(utc.gigayear, firstYear, 0);
//				if (isLeapYr) seconds += secondsPerDay;
//			}
//
//			// Perform carry
//			carry = seconds/e9;
//			gigaseconds += carry;
//			seconds -= carry*e9;
//
//			// Add in tempseconds 
//			seconds += tempseconds;
//
//			// Remaining number of seconds is not enough to require any more carries.
//			//	The createTicks function can take seconds > 1e9
//
//			// Add in the day of year, hours, minutes and seconds
//			//  If there are 3 years remaining, the date is within a leap year
//			isLeapYr = numLeapDays(utc.gigayear, utc.year, utc.calendar);
//			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
//			seconds += (dayOfYear - 1) * secondsPerDay+ utc.hour * 3600 + utc.minute * 60 + utc.second;
//
//			// Perform carry
//			carry = seconds/e9;
//			gigaseconds += carry;
//			seconds -= carry*e9;
//
//			// Create a relative tick value representing the time since 2400
//			r1 = createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0);
//			// Add the period up to 2400
//			u2.tai = addRelTicksToTicks(PostLeapSecond, r1);
//			return u2;
//		}
//		// Can't get here.
//		return u2;
//	}
//	else
//	{
//		// Invalid specification of UTCDatetime
//		return u2;
//	}
//}

UTCDatetime calcTicks(UTCDatetime utcIn, uint32_t nanosecond, uint32_t attosecond)
{
	// Return a copy of utc, calculating tai and updating futureAdjust and taiInit, as necessary
	//  This function is called on the initial creation of a UTCDatetime or when 
	//		futureAdjust == 1.
	UTCDatetime u2 = utcIn;
	// Create a working copy of utcIn to handle calendar issues
	UTCDatetime utc = utcIn;

	uint32_t dayOfYear;  // day of Year
	uint8_t isLeapYr;
	uint32_t gigaseconds;
	uint32_t megaseconds, kiloseconds;
	uint32_t seconds;
	uint32_t tempseconds;
	uint32_t full4Year;
	uint32_t fullGigaYears, full1KYears, full100Years, full1200Years;
	int firstYear;
	int remYears, carry, borrow;
	int cumLeapSecs = 0;
	uint32_t borrowGiga;

	uint32_t oct4 = monthDayToDayOfYear(10, 4, 0);
	static const uint32_t secondsIn4JulianYears = 126230400; // 1461 * secondsPerDay; 
	static const uint32_t lengthOfCreation = 561600; // 6 * secondsPerDay + secondsPerDay / 2;

	// Julian year initializations - 4 year cycle with one leap year every 4 years
	//   There are 2 different year lengths, but every 4 year cycle is the same length

	// In order to avoid theese calculations being performed on every call, the variables need to be static.
	//		However, calculations can not be performed on the initialization of static variables, so
	//			just assign to a precomputed value.

	//uint32_t gigaSecsInJulianGigaYear = secondsIn4JulianYears / 4; // Value is an integer since seconds in 4 years is multiple of 400 - 31,557,600
	//
	//uint32_t gigaSecsInJulian1KYear = gigaSecsInJulianGigaYear / 1000000; // Value is truncated - 31(.5576)
	//uint32_t megaSecsInJulian1KYear = (gigaSecsInJulianGigaYear - 1000000*gigaSecsInJulian1KYear)/1000;  // Remaining number of mega seconds, truncated - 557(.6)
	//uint32_t kiloSecsInJulian1KYear = gigaSecsInJulianGigaYear - 1000000*gigaSecsInJulian1KYear - 1000*megaSecsInJulian1KYear; // Remaining number of kilosecs - 600

	//uint32_t gigaSecsInJulian100Year = gigaSecsInJulianGigaYear / 10000000; // Value is truncated - 3(.15576)
	//uint32_t megaSecsInJulian100Year = (gigaSecsInJulianGigaYear - 10000000*gigaSecsInJulian100Year)/10000;  // Remaining number of mega seconds, truncated
																											//	155(.76)
	//uint32_t kiloSecsInJulian100Year = (gigaSecsInJulianGigaYear - 10000000*gigaSecsInJulian100Year - 
	//										10000*megaSecsInJulian100Year) / 10; // Remaining number of kilosecs
																				 //   760
	static const uint32_t gigaSecsInJulianGigaYear = 31557600;
	static const uint32_t gigaSecsInJulian1KYear = 31;
	static const uint32_t megaSecsInJulian1KYear = 557;
	static const uint32_t kiloSecsInJulian1KYear = 600;
	static const uint32_t gigaSecsInJulian100Year = 3;
	static const uint32_t megaSecsInJulian100Year = 155;
	static const uint32_t kiloSecsInJulian100Year = 760;

	/* Gregorian year initializations - 400 year cycle with leap years every 4 years except centuries not divisible by 400
	As for Julian years, there are 2 different year lenghts, however the cycle is 400 years, not 4 years
	Given that the number of seconds in 400 years > 12e9, we need a slightly different approach than that used for
	Julian years.  Specifically, we will consider an "average" Gregorian year, which has 365.2425 days, so has
	a length of 31,556,952 seconds. */

	//uint32_t secondsInAverageGregorianYear = (303*365 + 97*366)*864/4; // 31,556,952
	//uint32_t gigaSecsInGregorianGigaYear = secondsInAverageGregorianYear; // Value is an integer
	//	A Gregorian 1KYear is not an acceptable unit, since it is not a multiple of 400.  As
	//		such, use Grigroian 1200 Year.
	//  Number of seconds in Gregorian1200Year is 1200 * 31,556,952 = 37,868,342,400
	//uint32_t gigaSecsInGregorian1200Year = 37
	//uint32_t megaSecsInGregorian1200Year = 868
	//uint32_t kiloSecsInGregorian1200Year = 342
	//uint32_t secsInGregorian1200Year = 400

	// The following computation is for a 100 year period without a leap year in the initial year
	//uint32_t secondsInGregorianCentury = (76*365+ 24*366) * 86400; // 3,155,673,600
	//uint32_t gigaSecsInGregorian100Year = secondsInGregorianCentury / e9; // Value is truncated - 3(.1556736)
	//uint32_t megaSecsInGregorian100Year = (secondsInGregorianCentury - e9*gigaSecsInGregorian100Year)/1000000;
	// Remaining number of mega seconds, truncated - 155(.6736)
	//uint32_t kiloSecsInGregorian100Year = (secondsInGregorianCentury - e9*gigaSecsInGregorian100Year 
	//										- 1000000*megaSecsInGregorian100Year) / 1000; // Remaining number of kilosecs, truncated - 673(.6)
	//uint32_t secsInGregorian100Year = secondsInGregorianCentury - e9*gigaSecsInGregorian100Year 
	//										- 1000000*megaSecsInGregorian100Year - 1000*kiloSecsInGregorian100Year; // Remaining number of seconds - 600

	//	 static const uint32_t secondsInAverageGregorianYear = 31556952;
	static const uint32_t gigaSecsInGregorianGigaYear = 31556952;

	static const uint32_t gigaSecsInGregorian1200Year = 37;
	static const uint32_t megaSecsInGregorian1200Year = 868;
	static const uint32_t kiloSecsInGregorian1200Year = 342;
	static const uint32_t secsInGregorian1200Year = 400;

	// The following computation is for a 100 year period without a leap year in the initial year
	//	 static const uint32_t secondsInGregorianCentury = 3155673600;
	static const uint32_t gigaSecsInGregorian100Year = 3;
	static const uint32_t megaSecsInGregorian100Year = 155;
	static const uint32_t kiloSecsInGregorian100Year = 673;
	static const uint32_t secsInGregorian100Year = 600;


	TAIRelTicks r1, r1adj;

	// Initialize tai
	u2.tai = Creation;
	// For any datetime outside the leap second period, futureAdjust = 0.
	//  Initialize to zero, and reset if necessary.
	u2.futureAdjust = 0;

	// If all of the fields are valid, perform the calculations
	if (utc.taiInit == 0)
	{
		// TAIticks has not been computed for this UTCDatetime

		// Datetime is after the Big Bang based on having passed the checks
		//	in createUTCDatetime

		//  Detemine the period
		if (utc.gigayear == -13 && utc.year == bbYear)
		{
			// Within the Creation period
			if (utc.dayOfMonth == 25)
			{
				gigaseconds = 0;
				seconds = (utc.hour - 12) * 3600 + utc.minute * 60 + utc.second;
				u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
				return u2;
			}
			else
			{
				// Between the 26th and 31st
				gigaseconds = 0;
				seconds = secondsPerDay / 2 + (utc.dayOfMonth - 26) * secondsPerDay +
					utc.hour * 3600 + utc.minute * 60 + utc.second;
				u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
				return u2;
			}
		}
		if ((utc.gigayear < 0) ||
			(utc.gigayear == 0 && utc.year < -4712))  // Before 4713 BCE
		{
			// Within the Prehistoric period
			/*  Julian calendar has 4 year repeating periods, each of length 3*365 + 366 = 1461 days
			Begin by determining the number of gigaseconds contained in the gigayears. */
			fullGigaYears = utc.gigayear + 13;
			borrow = 0;
			if (utc.year <= bbYear) // Period starts 1 Jan -13,826,151,187.
			{						//   To be a full billion years, must be
				fullGigaYears--;	// at least within -12,826,151,187.
				borrow = e9;
			}
			gigaseconds = gigaSecsInJulianGigaYear * fullGigaYears;

			// Determine the number of 1K year periods remaining
			//  The numbers of giga, mega and kilo in 1K year is each < 1000.
			//		Therefore, can multiply each by 1e6, without an overflow

			full1KYears = (utc.year + borrow - bbYear - 1) / 1000;
			gigaseconds += full1KYears * gigaSecsInJulian1KYear;
			megaseconds = full1KYears * megaSecsInJulian1KYear;
			kiloseconds = full1KYears * kiloSecsInJulian1KYear;
			remYears = utc.year + borrow - bbYear - 1 - 1000 * full1KYears;

			// Determine the number of 100 year periods remaining
			full100Years = remYears / 100;
			gigaseconds += full100Years * gigaSecsInJulian100Year;
			megaseconds += full100Years * megaSecsInJulian100Year;
			kiloseconds += full100Years * kiloSecsInJulian100Year;
			remYears -= 100 * full100Years;

			// Perform carries
			carry = kiloseconds / 1000;
			megaseconds += carry;
			kiloseconds -= carry * 1000;

			carry = megaseconds / 1000;
			gigaseconds += carry;
			megaseconds -= carry * 1000;

			// Compute seconds from megaseconds and kiloseconds
			tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000

																	   // Determine the number of 4 year periods remaining
			full4Year = remYears / 4;
			remYears -= 4 * full4Year;

			// Less than a century remains, so the number of seconds fits in 32 bits
			//   The 4th year in the cycle is a leap year, so all remaining years are non leap
			seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap;

			// Perform carry
			carry = seconds / e9;
			gigaseconds += carry;
			seconds -= carry*e9;

			// Add in tempseconds 
			seconds += tempseconds;

			// Remaining number of seconds is not enough to require any more carries.
			//	The createTicks function can take seconds > 1e9

			// Add in the day of year, hours, minutes and seconds
			//  If there are 3 years remaining, the date is within a leap year
			isLeapYr = numLeapDays(utc.gigayear, utc.year, 0);
			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
			seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second
				+ lengthOfCreation;

			// Perform carry
			carry = seconds / e9;
			gigaseconds += carry;
			seconds -= carry*e9;

			u2.tai = createTicks(gigaseconds, seconds, nanosecond, attosecond);
			return u2;
		}
		// 1582 is not a leap year
		dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, 0);
		if ((utc.gigayear == 0 && utc.year < 1582) ||  // Before 1582
			(utc.gigayear == 0 && utc.year == 1582 && dayOfYear <= oct4))  // Before change to Gregorian calendar
		{
			// Within the Julian period - 4713 BCE is a leap year
			remYears = utc.year + 4712;

			// Determine the number of 100 year periods remaining
			full100Years = remYears / 100;
			gigaseconds = full100Years * gigaSecsInJulian100Year;
			megaseconds = full100Years * megaSecsInJulian100Year;
			kiloseconds = full100Years * kiloSecsInJulian100Year;
			remYears -= 100 * full100Years;

			// Perform carries
			carry = kiloseconds / 1000;
			megaseconds += carry;
			kiloseconds -= carry * 1000;

			carry = megaseconds / 1000;
			gigaseconds += carry;
			megaseconds -= carry * 1000;

			// Compute seconds from megaseconds and kiloseconds
			tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000

																	   // Determine the number of 4 year periods remaining
			full4Year = remYears / 4;
			remYears -= 4 * full4Year;

			// Less than a century remains, so the number of seconds fits in 32 bits
			seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap;
			// The first remaining year is a leap year, since 4713 BCE is a leap year
			if (remYears > 0) seconds += secondsPerDay;

			// Perform carry
			carry = seconds / e9;
			gigaseconds += carry;
			seconds -= carry*e9;

			// Add in tempseconds 
			seconds += tempseconds;

			// Remaining number of seconds is not enough to require any more carries.
			//	The createTicks function can take seconds > 1e9

			// Add in the day of year, hours, minutes and seconds
			//  If there are no years remaining, the date is within a leap year
			isLeapYr = numLeapDays(utc.gigayear, utc.year, 0);
			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
			seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;

			// Create a relative ticks value then add to the start date of the period
			u2.tai = addRelTicksToTicks(Julian, createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
			return u2;
		}
		if (utc.gigayear == 0 && utc.year < 2400)
		{
			/* Within the Gregorian period.  Starting 1600, the calendar repeats in
			400 year cycles.  The mapping to TAI also repeats in 400 year cycles,
			except for the discontinuties introduced between 1 Jan 1958 and 1 Jan 1972, then
			after 1 Jan 1972 with leap seconds.  As of the date this code was written, Jan, 2015,
			the last known leap second was 31 Dec 2016.  Thus, the last block of 400 years
			to have discontinuities is from 2000 - 2399.  Treat the period after that
			separately.  */
			// Check for the invalid period
			//if (utc.year == 1582 && utc.month == 10 && utc.dayOfMonth > 4 && utc.dayOfMonth < 15
			//		&& utc.calendar != 1)
			//{
			//	// Invalid date - calendar skips from Oct 4 to Oct 15, 1582.  Set to zero.
			//	u2.tai = createTicks(0, 0, 0, 0);
			//	u2.taiInit = 1; // Indicate tai has been initialized to avoid needless recalcs
			//	return u2;
			//}

			//  Handle the end of 1582
			if (utc.year == 1582)
			{
				dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, 0);
				gigaseconds = 0;
				seconds = (dayOfYear - monthDayToDayOfYear(10, 15, 0)) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;

				// Create a relative ticks value then add to the start date of the period
				u2.tai = addRelTicksToTicks(Gregorian, createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
				return u2;
			}
			//  Handle the end of the 16th century
			if (utc.year < 1600)
			{
				remYears = utc.year - 1583;
				full4Year = remYears / 4;
				remYears -= 4 * full4Year;
				// The second year in the period is a leap year
				seconds = full4Year * secondsIn4JulianYears + remYears * secondsInNonLeap; // 17 years of seconds < 1e9
				if (remYears > 1) seconds += secondsPerDay;

				isLeapYr = numLeapDays(utc.gigayear, utc.year, 0); // There are no intervening centuries in this period
				seconds += (monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr) - 1) * secondsPerDay +
					utc.hour * 3600 + utc.minute * 60 + utc.second;
				// Add in the seconds from Oct 15, 1582 through Oct 31, 1582 so Gregorian can be used as starting point
				gigaseconds = 0;
				seconds += 78 * secondsPerDay;

				// Create a relative ticks value then add to the start date of the period
				u2.tai = addRelTicksToTicks(Gregorian, createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));
				return u2;
			}
			else
			{
				// Find the number of centuries
				remYears = utc.year - 1600;
				full100Years = remYears / 100;
				gigaseconds = full100Years * gigaSecsInJulian100Year;
				megaseconds = full100Years * megaSecsInJulian100Year;
				kiloseconds = full100Years * kiloSecsInJulian100Year;
				remYears -= 100 * full100Years;
				// Perform carries
				carry = kiloseconds / 1000;
				megaseconds += carry;
				kiloseconds -= carry * 1000;

				carry = megaseconds / 1000;
				gigaseconds += carry;
				megaseconds -= carry * 1000;

				// Compute seconds from megaseconds and kiloseconds
				tempseconds = megaseconds * 1000000 + kiloseconds * 1000;  // Max value is 999,999,000

																		   // During this time period, only the first and fifth centuies are full length.
																		   //		The others are 1 day short.
				if (full100Years > 1)
				{
					tempseconds -= (full100Years - 1) * secondsPerDay;
				}
				if (full100Years >= 5)
				{
					tempseconds += secondsPerDay;
				}
				// Find the number of 4 year periods remaining
				full4Year = remYears / 4;
				seconds = full4Year * secondsIn4JulianYears;

				// The first 4 year period is 1 day short, unless the start of that period is a multiple of 400
				if (full4Year > 0 && ((utc.year / 100) % 4 != 0)) seconds -= secondsPerDay;
				remYears -= 4 * full4Year;
				// All remaining years, except, possibly the first is a non leap year
				// The first remaining year is a leap year, unless it is a multiple of 100 but not of 400
				seconds += remYears * secondsInNonLeap;
				if (remYears > 0)
				{
					firstYear = utc.year - remYears;
					if (numLeapDays(utc.gigayear, firstYear, 0)) seconds += secondsPerDay;
				}

				// Perform carry
				carry = seconds / e9;
				gigaseconds += carry;
				seconds -= carry*e9;

				// Add in tempseconds 
				seconds += tempseconds;

				// Remaining number of seconds is not enough to require any more carries.
				//	The createTicks function can take seconds > 1e9

				// Add in the day of year, hours, minutes and seconds
				//  If there are no years remaining, the date is within a leap year
				isLeapYr = numLeapDays(utc.gigayear, utc.year, 0); // There are no intervening centuries in this period
				dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
				seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;

				// Create a relative ticks value then add to the start date of the period
				u2.tai = addRelTicksToTicks(Start1600, createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0));

				// Handle the early atomic period
				if (utc.year > 1957 && utc.year < 1961)
				{
					/* Correct for the drift.  Need to add a fraction of 1.422818 seconods
					to every second.  The amount to be added is 15 nanoseconds for each full
					second elapsed.  The time is effectively
					added at the start of the next second.  So, for the first second, there is
					no adjustment, while the second second has an adjustment of 15 nanoseconds.
					This will result in a jump of 2.402 milliseconds at the end of the interval. */
					r1 = diffTicks(u2.tai, EarlyAtomic);
					r1adj = adjustRelTicks(r1, 15);
					u2.tai = addRelTicksToTicks(EarlyAtomic, r1adj);
				}
				// Handle the first part of the early UTC period
				if (utc.year > 1960 && utc.year < 1968)
				{
					/* Correct for the drift.  Need to add a fraction of 5.3021952 seconds
					to every second.  The amount to be addedd is 24 nanoseconds for each full
					second elapsed.  The time is effectively
					added at the start of the next second.  So, for the first second, there is
					no adjustment, while the second second has an adjustment of 24 nanoseconds.
					This will result in a jump of 11.4688 milliseconds at the end of the interval.

					Since tai is unadjusted, i.e., does not consider the 1.422818 seconds
					that were added for the early atomic period, use an unadjusted
					value of EarlyUTC to calculate the elapsed seconds.*/

					r1 = diffTicks(u2.tai, EarlyUTC);
					r1adj = adjustRelTicks(r1, 24);
					u2.tai = addRelTicksToTicks(EarlyUTC, r1adj);

					// Create a relative tick value for the adjustment from 1 Jan 1958 to 1 Jan 1961
					r1 = createRelTicks(0, 1, 422818000, 0, 0);
					//  Add this realtive tick value to tai
					u2.tai = addRelTicksToTicks(u2.tai, r1);
				}
				// Handle the second part of the early UTC period
				if (utc.year > 1967 && utc.year < 1972)
				{
					/* Correct for the drift.  Need to add a fraction of 3.15576 seconds
					to every second.  The amount to be addedd is 25 nanoseconds for each full
					second elapsed.  The time is effectively
					added at the start of the next second.  So, for the first second, there is
					no adjustment, while the second second has an adjustment of 24 nanoseconds.
					This will result in a jump of  0.107758 seconds at the end of the interval,
					which is what occurred in practice.

					Since tai is unadjusted, i.e., does not consider the 1.422818 seconds
					that were added for the early atomic period, use an unadjusted
					value of EarlyUTC to calculate the elapsed seconds.*/

					r1 = diffTicks(u2.tai, EarlyUTC2);
					r1adj = adjustRelTicks(r1, 25);
					u2.tai = addRelTicksToTicks(EarlyUTC2, r1adj);

					// Create a relative tick value for the adjustment from 1 Jan 1958 to 1 Jan 1968
					r1 = createRelTicks(0, 6, 736482000, 0, 0);
					//  Add this realtive tick value to tai
					u2.tai = addRelTicksToTicks(u2.tai, r1);
				}
				// Handle the leap second period
				if (utc.year >= 1972)
				{
					/* Add a relative ticks value = 10 + elapsed leap seconds
					The current tai is unadjusted for the discontinuities.
					The leap second table contains both adusted and
					unadjusted values.  The adjustment is determined by
					comparing tai to the unadjusted value.

					We must account for the fact that the
					unadjusted tai for a leap second,
					e.g., 30 Jun 2012 23:59:60 UTC, is the
					same as for the second after the leap second,
					e.g., 1 Jul 2012 00:00:00 UTC. */
					cumLeapSecs = cumLeapSecondsUnadj(u2.tai, utc.second == 60);
					// Create a relative tick value for the adjustment.
					if (cumLeapSecs >= 0)
					{
						// Adjust by 10 seconds for 1958 - 1972 plus the number of leap seconds
						//	  If utc.second == 60, and this is not a leap second, cumleapsecs
						//		is incremented by 1e9.
						seconds = 10 + cumLeapSecs % e9;
						// If the datetime is after the last leap second, futureAdjust must be 1 or 2
						if (cumLeapSecs >= 2 * e9)
						{
							if (utc.futureAdjust == 0)
							{
								// Future adjustment specification error
								u2.taiInit |= InvalidFutureAdjust;
								u2.tai = Creation;
								return u2;
							}
							else
							{
								// Preserve the value - either 1 or 2
								u2.futureAdjust = utc.futureAdjust;
							}
						}
					}
					else
					{
						// This is a leap second.  Cumulative leap seconds is 
						//	- (cumleapseconds + 1).  One has been added, since
						//	  we can't have a value of -0, which would be the
						//    value at the first leap second if we didn't add 1.
						seconds = 10 - cumLeapSecs - 1;
					}
					// Create a relative tick value for the adjustment
					r1 = createRelTicks(0, seconds, 0, 0, 0);
					//  Add this realtive tick value to tai
					u2.tai = addRelTicksToTicks(u2.tai, r1);
				}
				return u2;
			}
		}
		if (utc.gigayear > 0 || (utc.gigayear == 0 && utc.year >= 2400))
		{
			// Post Leap Second period
			/*	Leap seconds are determined 6 months in advance.  The first leap second
			for this period won't be determined until at least 2399, so no reason
			to check for intervening leap seconds.  As there are no discontinuities,
			we can assume 400 year cycles */
			fullGigaYears = utc.gigayear;
			borrowGiga = 0;
			if (fullGigaYears > 0 && utc.year < 2400)
			{
				fullGigaYears--; // Subtract 1 if year < 2400
				borrowGiga++;
			}
			gigaseconds = gigaSecsInGregorianGigaYear * fullGigaYears;

			// Determine the number of 1K year periods remaining
			//  The numbers of giga, mega and kilo in 1K year is each < 1000.
			//		Therefore, can multiply each by 1e6, without an overflow

			full1200Years = (utc.year + borrowGiga*e9 - 2400) / 1200;
			gigaseconds += full1200Years * gigaSecsInGregorian1200Year;
			megaseconds = full1200Years * megaSecsInGregorian1200Year;
			kiloseconds = full1200Years * kiloSecsInGregorian1200Year;
			seconds = full1200Years * secsInGregorian1200Year;
			remYears = utc.year + borrowGiga*e9 - 2400 - 1200 * full1200Years;

			// Determine the number of 100 year periods remaining
			full100Years = remYears / 100;
			gigaseconds += full100Years * gigaSecsInGregorian100Year;
			megaseconds += full100Years * megaSecsInGregorian100Year;
			kiloseconds += full100Years * kiloSecsInGregorian100Year;
			seconds += full100Years * secsInGregorian100Year;

			// Since we are starting from 2400, the 1st, 5th and
			//	9th centuries need an extra day, since they
			//  begin with leap years
			seconds += (full100Years / 4) * secondsPerDay;
			if ((full100Years % 4) > 0) seconds += secondsPerDay;

			remYears -= 100 * full100Years;

			// Perform carries
			carry = seconds / 1000;
			kiloseconds += carry;
			seconds -= carry * 1000;

			carry = kiloseconds / 1000;
			megaseconds += carry;
			kiloseconds -= carry * 1000;

			carry = megaseconds / 1000;
			gigaseconds += carry;
			megaseconds -= carry * 1000;

			// Compute seconds from megaseconds, kiloseconds and seconds
			tempseconds = megaseconds * 1000000 + kiloseconds * 1000 + seconds;  // Max value is 999,999,000

																				 // Determine the number of 4 year periods remaining
			full4Year = remYears / 4;
			remYears -= 4 * full4Year;
			seconds = full4Year * secondsIn4JulianYears;
			// Adjust for Gregorian convention
			if (full4Year > 0)  // If there are no full4year periods, adjustment is made in next section
			{
				// Determine whether the century preceding the date was a multiple of 400
				if ((utc.year / 100) % 4 != 0) seconds -= secondsPerDay;
			}

			// The first year in the 4 year cycle is a leap year, unless
			//	it is a multiple of 100, but not of 400.
			// Determine whether the first remaining year is a leap year.
			if (remYears > 0)
			{
				seconds += remYears * secondsInNonLeap;
				firstYear = utc.year - (utc.year % 4);
				isLeapYr = numLeapDays(utc.gigayear, firstYear, 0);
				if (isLeapYr) seconds += secondsPerDay;
			}

			// Perform carry
			carry = seconds / e9;
			gigaseconds += carry;
			seconds -= carry*e9;

			// Add in tempseconds 
			seconds += tempseconds;

			// Remaining number of seconds is not enough to require any more carries.
			//	The createTicks function can take seconds > 1e9

			// Add in the day of year, hours, minutes and seconds
			//  If there are 3 years remaining, the date is within a leap year
			isLeapYr = numLeapDays(utc.gigayear, utc.year, 0);
			dayOfYear = monthDayToDayOfYear(utc.month, utc.dayOfMonth, isLeapYr);
			seconds += (dayOfYear - 1) * secondsPerDay + utc.hour * 3600 + utc.minute * 60 + utc.second;

			// Perform carry
			carry = seconds / e9;
			gigaseconds += carry;
			seconds -= carry*e9;

			// Create a relative tick value representing the time since 2400
			r1 = createRelTicks(gigaseconds, seconds, nanosecond, attosecond, 0);
			// Add the period up to 2400
			u2.tai = addRelTicksToTicks(PostLeapSecond, r1);
			return u2;
		}
		// Can't get here.
		return u2;
	}
	else
	{
		// Invalid specification of UTCDatetime
		return u2;
	}
}


uint16_t checkCalCoordsElements(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar)
{
	// Perform basic checks on calendar coordinates and return bit field
	//	The coordinates can represent a UTCDatetime or a localDatetime

	// Initialize the initialization bit field
	uint16_t init = 0;

	// Validate the date elements
	init = checkDateCoordsElements(gigayear, year, month, dayOfMonth, calendar);

	// Validate the time elements
	init += checkTimeCoordsElements(hour, minute, second, nanosecond, attosecond);

	return init;
}

int isSkipped(uint32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar)
{
	// Determine if a date is skipped during the transition
	//	from Julian to Gregorian calendar
	//
	DateCoords y1;
	y1.gigayear = 0;
	y1.dayOfMonth = dayOfMonth;
	y1.month = month;
	y1.year = year;
	y1.dateInit = 0;
	return (isGreaterDateCoords_NC(y1, lastJulianDate(calendar)) &&
		isLessDateCoords_NC(y1, firstGregorianDate(calendar)));
}

uint16_t checkDateCoordsElements(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint32_t calendar)
{
	// Perform basic checks on Date coordinates and return bit field
	//	The coordinates can represent a UTCDatetime or a localDatetime

	// Initialize the initialization bit field
	uint16_t init = 0;
	uint32_t dayOfYear;
	DateCoords y1;

	// Begin with simple range checks
	if (gigayear < -13) init |= NegativeTime;
	if (gigayear > 99) init |= TimeGtMax;
	// Expand the valid date range by one day on either side to support local datetimes
	if (gigayear == 100 && year == 0 && month == 1 && dayOfMonth == 1)
		init &= !TimeGtMax; // 1 Jan 100,000,000,000 is a valid local date
   // If gigayear != 0, year and gigayear must have the same sign
	if (year <= -e9 || year >= e9 ||
		(gigayear < 0 && year > 0) || (gigayear > 0 && year < 0)) init |= InvalidYear;
	if (month == 0 || month > 12) init |= InvalidMonth;
	// No month has more than 31 days
	if (dayOfMonth == 0 || dayOfMonth > 31) init |= InvalidDayOfMonth;
	// April, June, September and November only have 30 days
	if ((month == 4 || month == 6 || month == 9 || month == 11) &&
		(dayOfMonth > 30)) init |= InvalidDayOfMonth;
	// Check for valid calendar
	if (calendar == 0 || calendar == 1 || calendar == 2 ||
		(calendar >= MinCalendar && calendar <= MaxCalendar))
	{
		// February - there can be 0, 1 or 2 leap days
		if ((month == 2) && (dayOfMonth > 28 + numLeapDays(0, year, calendar))) init |= InvalidDayOfMonth;
		// Check for skipped day on transition from Julian to Gregorian calendar
		if (gigayear == 0 && isSkipped(year, month, dayOfMonth, calendar)) init |= InvalidDayOfMonth;
	}
	else
	{
		// Invalid calendar
		init |= InvalidCalendar;
	}
	// Having estabilshed fields are within range, do additional negative time checks
	//   Compute dayOfYear for the date as if the date
	//	   is in the creation year (which is a leap year) for the following check
	if (init == 0)
	{
		dayOfYear = monthDayToDayOfYear(month, dayOfMonth, 1);
		if ((gigayear == -13 && year < bbYear) ||				// Before 13,826,151,189 BCE
			(gigayear == -13 && year == bbYear &&
				dayOfYear < bbdayOfYear)) init |= NegativeTime;
		// Extend one day to support local time
		if (gigayear == -13 && year == bbYear && month == 12 && dayOfMonth == 24)
			init &= !NegativeTime; // 24 Dec 13,826,151,189 BCE is a valid local date
	   // Finally, for Julian dates, check that the date is not greater than the Julian max date
		if (calendar == 1)
		{
			y1.gigayear = gigayear;
			y1.year = year;
			y1.month = month;
			y1.dayOfMonth = dayOfMonth;
			y1.calendar = 1;
			if (isGreaterDateCoords_NC(y1, MaxJulianDate)) init |= TimeGtMax;
		}
	}
	return init;
}

uint16_t checkTimeCoordsElements(uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond)
{
	// Perform basic checks on time elements and return bit field
	// Initialize the initialization bit field
	uint16_t init = 0;
	if (hour > 23 || minute > 59 || second > 60) init |= HourMinSecGtMax;
	if (nanosecond >= e9) init |= NanoGtMax;
	if (attosecond >= e9) init |= AttoGtMax;
	return init;
}
	
uint16_t checkDatetimeElements(int8_t gigayear, int32_t year, uint8_t month,
					   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
					   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
					   int8_t precision, int8_t uncertainty, uint32_t calendar, 
					   uint8_t futureAdjust)
{
	// Perform basic checks on datetime elements and return bit field
	//	The only check that cannot be performed is whether futureAdjust is 0
	//	for a date after the last recorded leap second.

	uint32_t dayOfYear;
	DateCoords y1;
	// Initialize the initialization bit field
	uint16_t init = 0;

	// Begin with simple range checks
	if (gigayear < -13) init |= NegativeTime;
	if (gigayear > 99) init |= TimeGtMax;
	// Expand the valid date range by one day on either side to support local datetimes
	if (gigayear == 100 && year == 0 && month == 1 && dayOfMonth == 1)
		init &= !TimeGtMax; // 1 Jan 100,000,000,000 is a valid local date
	// If gigayear != 0, year and gigayear must have the same sign
	if (year <= -e9 || year >= e9 ||
		(gigayear < 0 && year > 0) || (gigayear > 0 && year < 0)) init |= InvalidYear;
	if (month == 0 || month > 12) init |= InvalidMonth;
	// No month has more than 31 days
	if (dayOfMonth == 0 || dayOfMonth > 31) init |= InvalidDayOfMonth;
	// April, June, September and November only have 30 days
	if ( (month == 4 || month == 6 || month == 9 || month == 11) &&
			(dayOfMonth > 30) ) init |= InvalidDayOfMonth;
	// Check for valid calendar
	if (calendar == 0 || calendar == 1 || calendar || 2 ||
		(calendar >= MinCalendar && calendar <= MaxCalendar))
	{
		// February - there can be 0, 1 or 2 leap days
		if ((month == 2) && (dayOfMonth > 28 + numLeapDays(0, year, calendar))) init |= InvalidDayOfMonth;
		// Check for skipped day on transition from Julian to Gregorian calendar
		if (gigayear == 0 && isSkipped(year, month, dayOfMonth, calendar)) init |= InvalidDayOfMonth;
	}
	else
	{
		// Invalid calendar
		init |= InvalidCalendar;
	}

	if (hour > 23 || minute > 59 || second > 60) init |= HourMinSecGtMax;
	if (nanosecond >= e9) init |= NanoGtMax;
	if (attosecond >= e9) init |= AttoGtMax;
	if (precision > 99 || precision < maxPrecision || (precision < 99 
			&& precision > minUTCPrecision) || !isValidUncertainty(uncertainty, precision, 0)) init |= InvalidPrecision;
	if (futureAdjust > 2) init |= InvalidFutureAdjust;
	// Having estabilshed fields are within range, do additional negative time checks
	//   Compute dayOfYear for the date as if the date
	//	   is in the creation year (which is a leap year) for the following check
	if (init == 0)
	{
		dayOfYear = monthDayToDayOfYear(month, dayOfMonth, 1);
		if ((gigayear == -13 && year < bbYear) ||				// Before 13,826,151,189 BCE
			(gigayear == -13 && year == bbYear &&
				dayOfYear < bbdayOfYear) ||				// Before 25 Dec in 13,826,151,189 BCE
				(gigayear == -13 && year == bbYear &&
					dayOfYear == bbdayOfYear && hour < 12))   // Before noon on 25 Dec 13,826,151,189 BCE
			init |= NegativeTime;
		// Extend one day to support local time
		if (gigayear == -13 && year == bbYear && month == 12 && dayOfMonth == 24)
			init &= !NegativeTime; // 24 Dec 13,826,151,189 BCE is a valid local date
		// Finally, for Julian dates, check that the date is not greater than the Julian max date
		if (calendar == 1)
		{
			y1.gigayear = gigayear;
			y1.year = year;
			y1.month = month;
			y1.dayOfMonth = dayOfMonth;
			y1.calendar = 1;
			if (isGreaterDateCoords_NC(y1, MaxJulianDate)) init |= TimeGtMax;
		}
	}
	// Check validity of leap second specification
	if (init == 0)
	{
		if ((second == 60) && ((hour != 23) || (minute != 59) ||
			!isLeapSecondDay(gigayear, year, month, dayOfMonth))) init |= NotALeapSecond;
	}
	return init;
}


CalCoords adjustCalCoords(CalCoords cc, int8_t precision)
{
	// Adjust the elements of a CalCoords to be consistent with a specified precision
	//	Assume the elements have been checked with checkDatetimeElements
	//	  prior to calling this function, indicating they represent
	//	  a valid UTCDatetime
	// Avoid problems associated with using a DEFINED token
	int8_t mp = maxPrecision;

	int8_t BCEgigayear, maxBCEgigayear = 13, maxGigayear = 99, minGigayear = -13, gigayear;
	int32_t BCEyear, BCEbbyear = 1 - bbYear, m, year;

	uint8_t month, dayOfMonth, hour, minute, second;
	uint32_t nanosecond, attosecond;
	CalCoords cc2 = cc;

	// Retrieve the input values from cc
	gigayear = cc.date.gigayear;
	year = cc.date.year;
	month = cc.date.month;
	dayOfMonth = cc.date.dayOfMonth;
	hour = cc.time.hour;
	minute = cc.time.minute;
	second = cc.time.second;
	nanosecond = cc.time.nanosecond;
	attosecond = cc.time.attosecond;

	// Do not round based on digits truncated based on precision.  A time represents an interval.
	//	For example, with a precision of seconds, a time of 6:23:01 represents the second starting
	//	at 6:23:01.  Fractional seconds truncated do not affect the time interval.  For example,
	//	if the time is 6:23:01.9, the second is still 6:23:01
	//
	// The only exception to the above is the year, which must be rounded if the precision is 
	//	10 years or less
	//
	//	Need to hanlde the BCE and CE periods, differently
	//
	//	Year 0 is 1 BCE.  Year -1 is 2 BCE.
	//	We are in the BCE period if either year < 0, gigayear < 0 or both == 0
	//   We will round the BCE period, according to how it is displayed.
	//	 Thus 5 BCE would round to 10 BCE, even though it is -4.
	//
	if (year < 0 || gigayear < 0 || ((year == 0) && (gigayear == 0)))
	{
		// In the BCE period
		//	Change the basis to BCE years for rounding calculations
		BCEyear = 1 - year;
		if (gigayear != 0) BCEgigayear = -gigayear;
		else BCEgigayear = 0;
		//	Carry, if necessary
		if (BCEyear == 1e9)
		{
			BCEgigayear++;
			BCEyear = 0;
		}
		// Round 10's of BCEgigayears if the first omitted digit >= 5
		if (precision == minUTCPrecision)
		{
			// Round BCEgigayears to neareast 10 BCEgigayears
			//  Since BCEgigayears is <= 13, no need to check for exceeding minimum time
			if ((BCEgigayear % 10) >= 5) BCEgigayear = BCEgigayear / 10 * 10 + 10;
			else BCEgigayear = BCEgigayear / 10 * 10;
			// Zero all other fields
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			// No special treatment required for hour, dayOfMonth or Month,
			//	as the only allowed values for BCEgigayear are 10 and 0, so cannot
			//	be in the creation year
			hour = 0;
			dayOfMonth = 1;
			month = 1;
			BCEyear = 0;
		}
		// Round BCEgigayears if BCEyear > 500,000,000
		else if (precision == minUTCPrecision - 1)
		{
			// Round BCEgigayears to nearest BCEgigayear
			//  Don't allow BCEgigayear to exceed maxBCEgigayear
			if (BCEyear >= e9 / 2 && BCEgigayear < maxBCEgigayear) BCEgigayear++;
			// Zero all other fields
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			// No special treatment required for hour, dayOfMonth or Month,
			//	as BCEyear will be set to zero, regardless of whether BCEgigayear
			//	is incremented, so cannot be in the creation year
			hour = 0;
			dayOfMonth = 1;
			month = 1;
			BCEyear = 0;
		}
		// Round BCEyears if necessary for precisions of powers of 10 of BCEyears
		if (precision >= 6 && precision <= 13)
		{
			// Round to nearest multiple of appropriate power of 10 of BCEyears
			//	Don't allow BCEyear to exceed BCEbbyear if BCEgigayear = maxBCEgigayear
			if (BCEyear % power(10, (precision - 5)) >= 5 * power(10, (precision - 6)))
			{
				m = BCEyear / power(10, (precision - 5)) * power(10, (precision - 5)) +
					power(10, (precision - 5));
				// If rounding up would exceed minimum UTC time, round down
				BCEyear = (m <= BCEbbyear || BCEgigayear < maxBCEgigayear ? m :
					m - power(10, (precision - 5)));
			}
			else
			{
				BCEyear = BCEyear / power(10, (precision - 5)) * power(10, (precision - 5));
			}
			if (BCEyear == e9)
			{
				// BCEgigayear cannot be equal maxBCEgigayear, so OK to increment 
				BCEgigayear++;
				BCEyear = 0;
			}
			// Zero all other fields, treating hour, dayOfMonth and month, specially
			//	during the creation year
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			if (BCEyear == BCEbbyear && BCEgigayear == maxBCEgigayear)
			{
				// Within creation year
				//	Move to January 1st of next year
				BCEyear--;
			}
			hour = 0;
			dayOfMonth = 1;
			month = 1;
		}
		// Change basis back to negative years
		year = 1 - BCEyear;
		if (BCEgigayear != 0) gigayear = -BCEgigayear;
		// Borrow, if necessary
		if (year == 1 && gigayear != 0)
		{
			year = 1 - e9;
			gigayear++;
		}
	}
	else
	{
		// In the CE period
		//
		// Round 10's of gigayears if the first omitted digit >= 5
		if (precision == minUTCPrecision)
		{
			// Round gigayears to neareast 10 gigayears
			//  Don't allow gigayear to exceed maxGigayear
			if ((gigayear % 10) >= 5)
			{
				m = gigayear / 10 * 10 + 10;
				// If rounding up causes the value to exceed the
				//	maximum UTC time, round down
				gigayear = (m <= maxGigayear ? m : gigayear / 10 * 10);
			}
			else gigayear = gigayear / 10 * 10;
			// Zero all other fields
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			hour = 0;
			dayOfMonth = 1;
			month = 1;
			year = 0;
		}
		// Round gigayears if year > 500,000,000
		else if (precision == minUTCPrecision - 1)
		{
			// Round gigayears to nearest gigayear
			//  Don't allow gigayear to exceed maxGigayear
			if (year >= e9 / 2 && gigayear < maxGigayear) gigayear++;
			// Zero all other fields
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			hour = 0;
			dayOfMonth = 1;
			month = 1;
			year = 0;
		}
		// Round years if necessary for precisions of powers of 10 of years
		if (precision >= 6 && precision <= 13)
		{
			// Round to nearest multiple of appropriate power of 10 of years
			if (year % power(10, (precision - 5)) >= 5 * power(10, (precision - 6)))
			{
				m = year / power(10, (precision - 5)) * power(10, (precision - 5)) +
					power(10, (precision - 5));
				// If rounding up causes the value to exceed the
				//	maximum UTC time, round down
				year = (m < 1e9 || gigayear < maxGigayear ? m :
					m - power(10, (precision - 5)));
			}
			else
			{
				year = year / power(10, (precision - 5)) * power(10, (precision - 5));
			}
			if (year == e9)
			{
				// Safe to carry based on calculation for year
				gigayear++;
				year = 0;
			}
			// Zero all other fields
			attosecond = 0;
			nanosecond = 0;
			second = 0;
			minute = 0;
			hour = 0;
			dayOfMonth = 1;
			month = 1;
		}
	}
	if (precision < -9 && precision > mp)
	{
		// Modify the attosecond field
		attosecond = attosecond / power(10, precision - mp) *
			power(10, precision - mp);
	}
	else if (precision >= -9 && precision < 0)
	{
		// Modify the nanosecond field
		nanosecond = nanosecond / power(10, precision + 9) *
			power(10, precision + 9);
		// Zero the attosecond field
		attosecond = 0;
	}
	else if (precision == 0)
	{
		// Zero the attosecond and nanosecond fields
		attosecond = 0;
		nanosecond = 0;
	}
	else if (precision == 1)
	{
		// Zero the attosecond, nanosecond and second fields
		attosecond = 0;
		nanosecond = 0;
		second = 0;
	}
	else if (precision == 2)
	{
		// Zero the attosecond, nanosecond, second and minute fields
		attosecond = 0;
		nanosecond = 0;
		second = 0;
		minute = 0;
	}
	else if (precision == 3)
	{
		// Zero the attosecond, nanosecond, second, minute and hour fields
		attosecond = 0;
		nanosecond = 0;
		second = 0;
		minute = 0;
		// Since the hours must be zero and that isn't possible on creation
		//	day, move to creation day + 1
		if (year == bbYear && gigayear == minGigayear && month == 12 &&
			dayOfMonth == 25) dayOfMonth = 26;
		hour = 0;
	}
	else if (precision == 4)
	{
		// Zero the attosecond, nanosecond, second, minute, hour
		//	and day of month fields
		attosecond = 0;
		nanosecond = 0;
		second = 0;
		minute = 0;
		// For month precision, day of month must be 1, so move to
		//	January of bbYear + 1 if in creation month
		if (year == bbYear && gigayear == minGigayear && month == 12)
		{
			// Move to January of bbyear + 1
			year++;
			month = 1;
		}
		dayOfMonth = 1;
		hour = 0;
	}
	else if (precision == 5)
	{
		// Zero the attosecond, nanosecond, second, minute, hour,
		//  day of month fields and month fields
		attosecond = 0;
		nanosecond = 0;
		second = 0;
		minute = 0;
		// For year precision, month and day of month must be 1, so
		//	move to January of bbYear + 1
		if (year == bbYear && gigayear == minGigayear)
		{
			// Move to January of bbyear + 1
			year++;
		}
		month = 1;
		dayOfMonth = 1;
		hour = 0;
	}
	// No adjustments required for precision == 99

	// Set the fields of cc2
	cc2.date.gigayear = gigayear;
	cc2.date.year = year;
	cc2.date.month = month;
	cc2.date.dayOfMonth = dayOfMonth;
	cc2.date.dateInit = 0;
	cc2.time.hour = hour;
	cc2.time.minute = minute;
	cc2.time.second = second;
	cc2.time.nanosecond = nanosecond;
	cc2.time.attosecond = attosecond;
	cc2.time.timeInit = 0;
	return cc2;
}

UTCDatetime adjustUTCDatetimeElements(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	int8_t precision, int8_t uncertainty, uint8_t futureAdjust)
{
	// Adjust UTCDatetime elements to be consistent with precision
	//	Assume the elements have been checked with checkDatetimeElements
	//	  prior to calling this function, indicating they represent
	//	  a valid UTCDatetime
	UTCDatetime utc;
	CalCoords cc, cc2;

	// Establish the input values to the adjustCalCoords function
	cc.date.gigayear = gigayear;
	cc.date.year = year;
	cc.date.month = month;
	cc.date.dayOfMonth = dayOfMonth;
	cc.date.dateInit = 0;
	cc.time.hour = hour;
	cc.time.minute = minute;
	cc.time.second = second;
	cc.time.nanosecond = nanosecond;
	cc.time.attosecond = attosecond;
	cc.time.timeInit = 0;

	cc2 = adjustCalCoords(cc, precision);

	// Update the datetime elements
	gigayear = cc2.date.gigayear;
	year = cc2.date.year;
	month = cc2.date.month;
	dayOfMonth = cc2.date.dayOfMonth;
	hour = cc2.time.hour;
	minute = cc2.time.minute;
	second = cc2.time.second;
	nanosecond = cc2.time.nanosecond;
	attosecond = cc2.time.attosecond;

	// Set the fields of utc
	utc.gigayear = gigayear;
	utc.year = year;
	utc.month = month;
	utc.dayOfMonth = dayOfMonth;
	utc.hour = hour;
	utc.minute = minute;
	utc.second = second;
	utc.precision = precision;
	utc.uncertainty = adjustZeroUncertainty(uncertainty, precision, 0);
	utc.futureAdjust = futureAdjust;

	// This function will only be called when taiInit == 0
	utc.taiInit = 0;

	// Calculate the adjusted ticks associated with this UTC datetime
	// Invalid futureAdjust and leap second are checked in call to calcTicks

	return calcTicks(utc, nanosecond, attosecond);
}

TAIRelDatetime adjustRelDatetimeElements(uint16_t gigaSIdays, uint32_t sIdays,
	uint8_t sIhours, uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds,
	uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty)
{
	// Adjust TAIRelDatetime elements to be consistent with precision
	//	Assume the elements have been checked with checkRelDatetimeElements
	//	  prior to calling this function, indicating they represent
	//	  a valid TAIRelDatetime
	uint32_t gigaseconds, rem10KDays, megaseconds, carry, remDays, secs;
	TAIRelDatetime rel;

	uint16_t m;
	uint32_t m2;

	// Round the value, if necessary, based on precision
	if (precision != 99 && precision > maxPrecision)
	{
		if (precision > 12)
		{
			// Round to nearest multiple of appropriate power of 10 of gigaSIDays
			if (gigaSIdays % power(10, (precision - 12)) >= (uint16_t)5 * power(10, (precision - 13)))
			{
				// Don't allow to exceed the maximum
				m = gigaSIdays / power(10, (precision - 12)) * power(10, (precision - 12)) +
					power(10, (precision - 12));
				// If rounding up exceeds maximum value, round down
				gigaSIdays = (m <= maxGigaSIDays ? m : m - power(10, (precision - 12)));
			}
			else
			{
				gigaSIdays = gigaSIdays / power(10, (precision - 12)) * power(10, (precision - 12));
			}
			// Zero remaining fields
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
			sIminutes = 0;
			sIhours = 0;
			sIdays = 0;
		}
		if (precision == 12)
		{
			// Round to nearest gigaSIDay, rounding up, if necessary
			if (sIdays >= e9 / 2 && gigaSIdays < maxGigaSIDays) gigaSIdays++;
			// Zero remaining fields
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
			sIminutes = 0;
			sIhours = 0;
			sIdays = 0;
		}
		if (precision >= 4 && precision < 12)
		{
			// Round to nearest multiple of appropriate power of 10 of sIDays
			// Don't allow sIDays to exceed maxSIDays if gigaSIDays == maxGigaSIDays
			if (sIdays % power(10, (precision - 3) >= 5 * power(10, (precision - 4))))
			{
				m2 = sIdays / power(10, (precision - 3)) * power(10, (precision - 3)) +
					power(10, (precision - 3));
				// If rounding up would exceed maximum value, round down
				sIdays = (m2 < maxSIDays || gigaSIdays < maxGigaSIDays ? m2 :
					m2 - power(10, (precision - 3)));
			}
			else
			{
				sIdays = sIdays / power(10, (precision - 3)) * power(10, (precision - 3));
			}
			// Zero remaining fields
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
			sIminutes = 0;
			sIhours = 0;
		}
		if (precision == 3)
		{
			// Don't allow sIdays to exceed maxSIDays if gigaSIDays == maxGigaSIDays
			if (sIhours >= 12 && (gigaSIdays < maxGigaSIDays || sIdays < maxSIDays)) sIdays++;
			// Zero remaining fields
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
			sIminutes = 0;
			sIhours = 0;
		}
		if (precision == 2)
		{
			// Minutes will be zero when at the maximum value, so no potential problem rounding hours
			if (sIminutes >= 30) sIhours++;
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
			sIminutes = 0;
		}
		if (precision == 1)
		{
			// Don't allow minumtes to exceed 0, when gigaSIDays, sIDays and sIhours are at the maximum
			if (seconds >= 30 && (gigaSIdays < maxGigaSIDays || sIdays < maxSIDays
				|| sIhours < 12)) sIminutes++;
			attoseconds = 0;
			nanoseconds = 0;
			seconds = 0;
		}
		if (precision == 0)
		{
			// Don't allow seconds to exceed 9 + numLeapSeconds when other fields are at the maximum
			if (nanoseconds >= e9 / 2 && (gigaSIdays < maxGigaSIDays || sIdays < maxSIDays
				|| sIhours < 12 || seconds < 9 + numLeapSeconds)) seconds++;
			attoseconds = 0;
			nanoseconds = 0;
		}
		if (precision == -9)
		{
			// Don't allow nanoseconds to reach e9 when other fields are at the maximum
			if (attoseconds >= e9 / 2 && (gigaSIdays < maxGigaSIDays || sIdays < maxSIDays
				|| sIhours < 12 || seconds < 9 + numLeapSeconds ||
				nanoseconds < e9 - 1)) nanoseconds++;
			attoseconds = 0;
		}
		if (precision < 0 && precision > -9)
		{
			// Round to the nearest multiple of appropriate power of 10 of nanoseconds
			// Don't allow nanoseconds to reach e9 when other fields are at the maximum
			if ((nanoseconds % power(10, 9 + precision)) >= (uint32_t)5 * power(10, 8 + precision))
			{
				m2 = (nanoseconds / power(10, (9 + precision))) *
					power(10, (9 + precision))
					+ power(10, (9 + precision));
				nanoseconds = (m2 < e9 || gigaSIdays < maxGigaSIDays || sIdays < maxSIDays
					|| sIhours < 12 || seconds < 9 + numLeapSeconds ?
					m2 : m2 - power(10, (9 + precision)));
			}
			else
			{
				nanoseconds = (nanoseconds / power(10, (9 + precision))) *
					power(10, (9 + precision));
			}
			attoseconds = 0;
		}
		if (precision < -9 && precision > maxPrecision)
		{
			// Round to the neareast multiple of appropriate power of 10 of attoseconds
			// Don't allow attoseconds to reach e9 when other fields are at the maximum
			if ((attoseconds % power(10, 18 + precision)) >= (uint32_t)5 * power(10, 17 + precision))
			{
				m2 = (attoseconds / power(10, (18 + precision))) *
					power(10, (18 + precision))
					+ power(10, (18 + precision));
				attoseconds = (m2 < e9 || gigaSIdays < maxGigaSIDays || sIdays < maxSIDays
					|| sIhours < 12 || seconds < 9 + numLeapSeconds || nanoseconds < e9 - 1 ?
					m2 : m2 - power(10, (18 + precision)));
			}
			else
			{
				attoseconds = (attoseconds / power(10, (18 + precision))) *
					power(10, (18 + precision));
			}
		}
		// Perform carry calculations
		//	Don't need to worry about exceeding maximum due to above checks
		if (attoseconds == e9)
		{
			nanoseconds++;
			attoseconds = 0;
		}
		if (nanoseconds == e9)
		{
			seconds++;
			nanoseconds = 0;
		}
		if (seconds == 60)
		{
			sIminutes++;
			seconds = 0;
		}
		if (sIminutes == 60)
		{
			sIhours++;
			sIminutes = 0;
		}
		if (sIhours == 24)
		{
			sIdays++;
			sIhours = 0;
		}
		if (sIdays == e9)
		{
			gigaSIdays++;
			sIdays = 0;
		}
	}
	// No adjustments required for precision == 99 or precision == 18

	// GigaSIDays can be converted to gigaseconds by multiplication without risk of overflow
	gigaseconds = gigaSIdays * secondsPerDay;
	// To convert SIDays, which can be up to 1e9 - 1, break into smaller periods.
	//	Use the fact that 10,000 days is equivalent to 864 megaseconds
	rem10KDays = sIdays / 10000; // < 100,000
	megaseconds = rem10KDays * (secondsPerDay / 100); // < 86,400,000
													  //  Perform carry
	carry = megaseconds / 1000;
	gigaseconds += carry;
	megaseconds -= 1000 * carry;

	remDays = sIdays - 10000 * rem10KDays; // < 10,000
										   // This cannot overflow after the carry
										   //           < 1e9                 < .864 e9
	secs = 1000000 * megaseconds + remDays*secondsPerDay + sIhours * 3600 +
		sIminutes * 60 + seconds;
	// Perform carry
	carry = secs / e9;
	gigaseconds += carry;
	secs -= carry*e9;

	// Set the fields of re1
	rel.gigadays = gigaSIdays;
	rel.days = sIdays;
	rel.hours = sIhours;
	rel.minutes = sIminutes;
	rel.seconds = seconds;
	rel.precision = precision;
	rel.uncertainty = adjustZeroUncertainty(uncertainty, precision, 1);
	// This function will only be called when taiInit == 0
	rel.relInit = 0;
	rel.relTicks = createRelTicks(gigaseconds, secs, nanoseconds, attoseconds, isNegative);

	return rel;
}

uint8_t checkRelDatetimeElements(uint16_t gigaSIdays, uint32_t sIdays, uint8_t sIhours,
							uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds, 
							uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty)
{
	// Perform checks on datetime elements and return bit field

	// Initialize the initialization bit field
	uint8_t init = 0;

	// Constrain the magnitude of the relative time to be within the range of valid times
	//  From the Big Bang (25-Dec-13,826,151,189 BCE 12:00:00) to maximum time 
	//	(31-Dec-99,999,999,999 23:59:59.999999999 999999999) is 
	//	41,574,251,721,059d 12h 0m [9 + numLeapSeconds].999999999 999999999s
	//
	if (gigaSIdays > maxGigaSIDays ||
	   (gigaSIdays == maxGigaSIDays && sIdays > maxSIDays) ||
	   (gigaSIdays == maxGigaSIDays && sIdays == maxSIDays && sIhours > 12) ||
	   (gigaSIdays == maxGigaSIDays && sIdays == maxSIDays && sIhours == 12 && (60*sIminutes + seconds)
			 > 9 + numLeapSeconds))
		init |= RelTimeGtMax;
	if (sIdays >= e9) init |= DaysGtMax;
	if (sIhours > 23) init |= HoursGtMax;
	if (sIminutes > 59) init |= MinutesGtMax;
	if (seconds > 59) init |= SecondsGtMax;
	if (nanoseconds >= e9) init |= NanosGtMax;
	if (attoseconds >= e9) init |= AttosGtMax;
	if (precision > 99 || precision < maxPrecision || (precision < 99
		&& precision > minRelPrecision) || !isValidUncertainty(uncertainty, precision, 1)) init |= InvalidRelPrecision;

	return init;
}

int isValidUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime)
{
	// Check the validity of an uncertainty given a precision for either a UTC or TAIRel datetime
	//	Precision is assumed to be valid (or unspecified)
	//
	int8_t negLower, negUpper, posLower, posUpper;
	//
	//	If precision is unspecified, uncertainty should be 0
	if (precision == 99) return (uncertainty == 0);
	//
	//  The uncertainty can be between 0 and the maximum value of the
	//	 precision unit or between 1/2 the precision unit and the maximum
	//	 value of the next higher precision unit.  The latter is indicated
	//	 by negative values of the uncertainty.
	//
	negLower = (isRelDatetime ? -maxUnceraintyTAIRelDatetime[precision + poff - 1] :
		-maxUnceraintyTAIRelDatetime[precision + poff - 1]);
	negUpper = (isRelDatetime ? -(maxUnceraintyTAIRelDatetime[precision + poff - 1] + 1) / 2 :
		-(maxUnceraintyTAIRelDatetime[precision + poff - 1] + 1) / 2);
	posLower = 0;
	posUpper = (isRelDatetime ? maxUnceraintyTAIRelDatetime[precision + poff] :
		maxUnceraintyTAIRelDatetime[precision + poff]);
	//
	return (((uncertainty >= negLower) && (uncertainty <= negUpper)) ||
			((uncertainty >= posLower) && (uncertainty <= posUpper)));
}

int8_t adjustZeroUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime)
{
	// Adjusts a zero uncertainty to the minimum allowed value when the precision is specified
	if (precision == 99) return 0;
	if (precision != 99 && uncertainty != 0) return uncertainty;
	//
	// Precision is specified and uncertainty is 0
	//  Set to the minimum allowed value
	//
	//	Handle each range
	//
	return (isRelDatetime ? -(maxUnceraintyTAIRelDatetime[precision + poff - 1] + 1) / 2 :
		-(maxUnceraintyUTCDatetime[precision + poff - 1] + 1) / 2);
}

PrecisionUncertainty adjustPrecisionUncertainty(int8_t precision, int uncertainty, int8_t isRelDatetime)
{
	// Adjust a precision / uncertainty pair so the uncertainty falls within the allowed range
	//	Not intended for random values of precision and uncertainty - solely to be used where
	//	precision and uncertainty are consistent, but where uncertainty might be too large,
	//	thereby requiring a decrease in precision.  Only can result in a 1 unit decrease in
	//	precision.  Uncertainty may be beyond the range of an int8_t on entry.
	//
	int8_t maxUnc[3], prec, minPrec;
	int maxu, unc;  // Will often be outside the range of an int8_t, since computed
				    //	based on next higher precision.
	PrecisionUncertainty pu1;
	//
	// Establish the maximum values for the uncertainty
	maxUnc[0] = (isRelDatetime ? maxUnceraintyTAIRelDatetime[precision + poff] :
		maxUnceraintyUTCDatetime[precision + poff]);
	// Establish the maximum values for the uncertainties for the next higher precision
	maxUnc[1] = (isRelDatetime ? maxUnceraintyTAIRelDatetime[precision + poff - 1] :
		maxUnceraintyUTCDatetime[precision + poff - 1]);
	// Establish the maximum values for the uncertainties for the precision that is 2 higher
	maxUnc[2] = (isRelDatetime ? maxUnceraintyTAIRelDatetime[precision + poff - 2] :
		maxUnceraintyUTCDatetime[precision + poff - 2]);
	prec = precision;
	minPrec = (isRelDatetime ? minRelPrecision : minUTCPrecision);
	//
	// Account for negative uncertainties, by converting uncertainty values
	//	to the next higher precision
	//
	unc = (uncertainty > 0 ? (maxUnc[1] + 1) * uncertainty : -uncertainty);
	//
	// Check whether the uncertainty exceeds the maximum.  The
	//	maximum value is 1 less than the value that would round to
	//	1 unit of the next lower precision.
	//  For example, if the precision is seconds, the current unit
	//	 of uncertainty is 0.1 seconds.  The maximum allowed uncertainty
	//	 is 9.4 seconds, which in the current units is 94.
	//  If the precision were in minutes, the current unit of
	//	 uncertainty is seconds.  The maximum allowed uncertainty is
	//	 1 second less than 59.5 minutes, which is 3569 seconds
	maxu = maxUnc[0] * (maxUnc[1] + 1) + (maxUnc[1] + 1) / 2 - 1;
	//
	//	If the uncertainty is over the maximum for the specified precision
	//	decrease the precision and adjust the uncertainty, unless the
	//	precision is already at the miniumum value
	if (unc > maxu && prec < minPrec)
	{
		// Adjust the precision and uncertainty
		//	As an example, assume the precision is minutes.
		//	The current unit of uncertainty is seconds.
		//	The uncertainty is greater than the maximum uncertainty,
		//	which is 3569.
		// Decrease the precision to hours.
		// Round the uncertainty to the nearest minute by adding
		//	1/2 minute, while remaining in units of seconds.
		// Divide by the number of seconds in an hour.
		prec++;
		unc = (unc + (maxUnc[1] + 1) / 2) / ((maxUnc[0] + 1) *
			(maxUnc[1] + 1));
	}
	// Report the uncertainty based on the precision
	//	If the uncertainty <= maxUnc[1], report as negative in the current units
	if (unc <= maxUnc[1]) uncertainty = -unc;
	//  Otherwise convert to the next lower precision
	//	If prec == minPrec, need to avoid overflow of int8_t
	else unc = min((unc + (maxUnc[1] + 1) / 2) / (maxUnc[1] + 1),127);
	//
	pu1.precision = prec;
	pu1.uncertainty = (int8_t)unc;
	return pu1;
}

double secondsUncertainty(int8_t uncertainty, int8_t precision, int8_t isRelDatetime)
{
	// Calculate the absolute value (as opoosed to a multiplier of precision) of uncertainty in seconds
	int8_t adjunc;
	//
	//	If precision is unspecified, return 0
	//
	if (precision == 99) return 0.;
	//
	//   Adjust zero uncertainty if necessary
	//
	adjunc = adjustZeroUncertainty(uncertainty, precision, isRelDatetime);
	if (isRelDatetime)
	{
		return  (adjunc > 0 ?
			(double)adjunc * precSecondsTAIRelDatetime[precision + poff] :
			-(double)adjunc * precSecondsTAIRelDatetime[precision + poff - 1]);
	}
	else
	{
		return  (adjunc > 0 ?
			(double)adjunc * precSecondsUTCDatetime[precision + poff] :
			-(double)adjunc * precSecondsUTCDatetime[precision + poff - 1]);
	}
}

double boundedSecondsUncertainty(int8_t uncertainty, int8_t precision,
	int8_t isRelDatetime, double value)
{
	// Calculate the absolute value (as opoosed to a multiplier of precision) of 
	//	uncertainty in seconds, bounded by the limits of double precision 
	//	representation.  The uncertainty is on the value, so it cannot
	//	be less than minFracUncertainty * value
	//
	double seconds;
	seconds = secondsUncertainty(uncertainty, precision, isRelDatetime);
	if (value == 0.)
	{
		return seconds;
	}
	else
	{
		return (seconds < minFracUncertainty * fabs(value) ?
			minFracUncertainty * fabs(value) : seconds);
	}
}

PrecisionUncertainty precisionFromSecondsUncertainty(double secondsUncertainty, int8_t isRelDatetime)
{
	// Derive a precision from an uncertainty specified in seconds
	int8_t i = 0, precision, uncertainty;
	double minSecondsUncertainty;
	PrecisionUncertainty pu1;
	//
	//  Bound secondsUncertainty to the minimum allowed value
	//
	minSecondsUncertainty = (isRelDatetime ? 0.5 * precSecondsTAIRelDatetime[poff + maxPrecision] :
		0.5 * precSecondsUTCDatetime[poff + maxPrecision]);
	secondsUncertainty = (secondsUncertainty < minSecondsUncertainty ? minSecondsUncertainty :
		secondsUncertainty);

	if (isRelDatetime)
	{
		// Search until a precision > than the uncertainty is located
		//	or until the minimum precision is checked
		while (precSecondsTAIRelDatetime[i] <= secondsUncertainty &&
			i < nPrecRel) ++i;
		//
		//	Calculate the uncertainty as a multiple of the precision
		//	 If i == nPrecRel, the uncertainty could overflow 
		//
		uncertainty = (int8_t)min((int)(secondsUncertainty / precSecondsTAIRelDatetime[i - 1] + 0.5), 127);
	}
	else
	{
		while (precSecondsUTCDatetime[i] <= secondsUncertainty &&
			i < nPrecUTC) ++i;
		//
		uncertainty = (int8_t)min((int)(secondsUncertainty / precSecondsUTCDatetime[i - 1] + 0.5), 127);

	}
	// Calculate the precision based on the offset into the array
	precision = i - 1 - poff;
	//  Uncertainty and precision must be adjusted if the precision is maxPrecision - 1.
	//	  This is because a precision of maxPrecision - 1 represents tenths of an attosecond.
	//	  Those values are stored as a negative uncertainty with a precision of maxPrecision
	if (precision == maxPrecision - 1 && uncertainty >= 5)
	{
		uncertainty = -uncertainty;
		precision++;
	}
	// If precision < maxPrecision - 1 or precision == maxPrecision - 1 and uncertainty < 5,
	//	 set to maximum precision and minimum uncertainty
	if (precision < maxPrecision - 1 || (precision == maxPrecision - 1 && uncertainty < 5))
	{
		uncertainty = -5;
		precision++;
	}
	//
	pu1.precision = precision;
	pu1.uncertainty = uncertainty;
	return pu1;
}

UTCDatetime createUTCDatetime(int8_t gigayear, int32_t year, uint8_t month,
					   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
					   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
					   int8_t precision, int8_t uncertainty, uint32_t calendar, 
					   uint8_t futureAdjust)
{
	// Create a UTCDatetime from elements.  Calculate ticks as part of the process.
	UTCDatetime utc;
	DateCoords date1, date2;

	// Check datetime elements
	uint16_t init = checkDatetimeElements(gigayear, year, month, dayOfMonth, hour, minute,
						   second, nanosecond, attosecond, precision, uncertainty, calendar, 
						   futureAdjust);

	// If any of the above tests have failed, set tai, taiInit and return
	utc.taiInit = init;
	if (init != 0)
	{
		utc.tai = createTicks(0,0,0,0);
		return utc;
	}
	// Convert to a UTC Gregorian date, if necessary
	if (calendar != 0)
	{
		// Create a DateCoords with the year, month and dayOfMonth
		date1.gigayear = gigayear;
		date1.year = year;
		date1.month = month;
		date1.dayOfMonth = dayOfMonth;
		date1.calendar = calendar;
		date1.dateInit = 0;
		// Convert to a UTC Gregorian
		date2 = convertToUTCGregorian(date1);
		gigayear = date2.gigayear;
		year = date2.year;
		month = date2.month;
		dayOfMonth = date2.dayOfMonth;
	}
	// Adjust the time elements to be consistent with the precision
	return adjustUTCDatetimeElements(gigayear, year, month, dayOfMonth, hour, minute,
		second, nanosecond, attosecond, precision, uncertainty, futureAdjust);
}

UTCDatetime createUTCDatetimeFromDayFrac(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, double dayFrac,
						   int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust)
{
	// Create a UTC datetime from date elements and fraction of a day
	UTCDatetime utc;
	DateCoords date1, date2;
	double seconds;
	int wholeSeconds, extraSecond;
	int8_t maxPrec;
	uint8_t hour, minute, second;
	uint32_t nanosecond, attosecond;

	// Check datetime elements
	uint16_t init = checkDatetimeElements(gigayear, year, month, dayOfMonth, 0, 0,
						   0, 0, 0, precision, uncertainty, calendar, futureAdjust);

	// The maximum precision for a datetime specified as a dayFrac is dayFrac * 86400 / 9.0073x10^15 seconds
	maxPrec = maxPrecision;
	if (dayFrac > 0.0) maxPrec = subSecondPrecision(dayFrac * secondsPerDay, doublePrecision);
	if (precision < maxPrec || !isValidUncertainty(uncertainty, precision, 0)) init |= InvalidPrecision;


	// Check validity of dayFrac
	if (dayFrac < 0.0 || dayFrac >= 1.0)
	{
		// Invalid dayFrac, so hour, minute, second, nanosecond and attosecond invalid
		init |= HourMinSecGtMax;
		init |= NanoGtMax;
		init |= AttoGtMax;
		// Atto not calculated from dayFrac, so don't set this flag
	}

	// If any of the above tests have failed, set tai, taiInit and return
	utc.taiInit = init;
	if (init != 0)
	{
		utc.tai = createTicks(0,0,0,0);
		return utc;
	}

	// Convert to a UTC Gregorian date, if necessary
	if (calendar != 0)
	{
		// Create a DateCoords with the year, month and dayOfMonth
		date1.gigayear = gigayear;
		date1.year = year;
		date1.month = month;
		date1.dayOfMonth = dayOfMonth;
		date1.calendar = calendar;
		date1.dateInit = 0;
		// Convert to a UTC Gregorian
		date2 = convertToUTCGregorian(date1);
		gigayear = date2.gigayear;
		year = date2.year;
		month = date2.month;
		dayOfMonth = date2.dayOfMonth;
	}

	// Add a second to day length for days with leap seconds
	extraSecond = isLeapSecondDay(gigayear, year, month, dayOfMonth);

	seconds = dayFrac * (double)(secondsPerDay + extraSecond);
	wholeSeconds = (int)(floor(seconds));
	nanosecond = (int)((seconds - wholeSeconds) * 1e9);
	attosecond = (int)((seconds - wholeSeconds - nanosecond/1e9) * 1e18);

	hour = (uint8_t) (wholeSeconds / 3600);

	// Detemrine whether the day fraction corresponds to the leap second
	if (hour == 24)
	{
		// This is the leap second
		hour = 23;
		minute = 59;
		second = 60;
	}
	else
	{
		wholeSeconds -= hour * 3600;
		minute = (uint8_t) (wholeSeconds / 60);
		second = (uint8_t) (wholeSeconds - 60 * minute);
	}

	// Adjust the time elements to be consistent with the precision
	return adjustUTCDatetimeElements(gigayear, year, month, dayOfMonth, hour, minute,
		second, nanosecond, attosecond, precision, uncertainty, futureAdjust);
}

UTCDatetime createUTCDatetimeFromSecondFrac(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   double secondsPlusFrac, int8_t precision, int8_t uncertainty, uint32_t calendar, 
						   uint8_t futureAdjust)
{
	// Create a UTC datetime from date elements and fraction of a day
	UTCDatetime utc;
	DateCoords date1, date2;
	int wholeSeconds;
	int8_t maxPrec;
	uint8_t second;
	uint32_t nanosecond, attosecond;

	// Check datetime elements
	uint16_t init = checkDatetimeElements(gigayear, year, month, dayOfMonth, hour, minute,
						   0, 0, 0, precision, uncertainty, calendar, futureAdjust);

	// The maximum precision for a datetime specified as a secondFrac is secondFrac / 9.0073x10^15 seconds
	maxPrec = maxPrecision;
	if (secondsPlusFrac > 0.0) maxPrec = subSecondPrecision(secondsPlusFrac, doublePrecision);
	if (precision < maxPrec || !isValidUncertainty(uncertainty, precision, 0)) init |= InvalidPrecision;

	// Check validity of seconds
	if (secondsPlusFrac < 0.0 || secondsPlusFrac >= 61.0)
	{
		// Invalid seconds, so second, nanosecond and attosecond invalid
		init |= HourMinSecGtMax;
		init |= NanoGtMax;
		init |= AttoGtMax;
	}

	// If any of the above tests have failed, set tai, taiInit and return
	utc.taiInit = init;
	if (init != 0)
	{
		utc.tai = createTicks(0,0,0,0);
		return utc;
	}

	// Convert to a UTC Gregorian date, if necessary
	if (calendar != 0)
	{
		// Create a DateCoords with the year, month and dayOfMonth
		date1.gigayear = gigayear;
		date1.year = year;
		date1.month = month;
		date1.dayOfMonth = dayOfMonth;
		date1.calendar = calendar;
		date1.dateInit = 0;
		// Convert to a UTC Gregorian
		date2 = convertToUTCGregorian(date1);
		gigayear = date2.gigayear;
		year = date2.year;
		month = date2.month;
		dayOfMonth = date2.dayOfMonth;
	}

	// Add a second to day length for days with leap seconds

	wholeSeconds = (int)(floor(secondsPlusFrac));
	nanosecond = (int)((secondsPlusFrac - wholeSeconds) * 1e9);
	attosecond = (int)((secondsPlusFrac - wholeSeconds - nanosecond/1e9) * 1e18);

	second = (uint8_t)wholeSeconds;

	// Detemrine whether the second corresponds to a leap second
	if ((second == 60) && (hour != 23 || minute != 59 || !isLeapSecondDay(gigayear, year, month, dayOfMonth)))
	{
		// second == 60, but not a valid leap second
		utc.taiInit |= NotALeapSecond;
		utc.tai = createTicks(0,0,0,0);
		return utc;
	}

	// Adjust the time elements to be consistent with the precision
	return adjustUTCDatetimeElements(gigayear, year, month, dayOfMonth, hour, minute,
		second, nanosecond, attosecond, precision, uncertainty, futureAdjust);
}

CalCoords createCalCoords(int8_t gigayear, int32_t year, uint8_t month,
						   uint8_t dayOfMonth, uint8_t hour, uint8_t minute,
						   uint8_t second, uint32_t nanosecond, uint32_t attosecond,
						   uint32_t calendar)
{
	// Create a CalCoords from elements.
	CalCoords calC;

	// Create the date portion
	calC.date = createDateCoords(gigayear, year, month, dayOfMonth, calendar);
	calC.ccInit = calC.date.dateInit;

	// Create the time portion
	calC.time = createTimeCoords(hour, minute, second, nanosecond, attosecond);
	calC.ccInit += calC.time.timeInit;

	return calC;
}

CalCoords createCalCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear,
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar)
{
	// Create a CalCoords from day of year, instead of month and day of month.
	CalCoords calC;

	// Create the date portion
	calC.date = createDateCoordsFromDayOfYear(gigayear, year, dayOfYear, calendar);
	calC.ccInit = calC.date.dateInit;

	// Create the time portion
	calC.time = createTimeCoords(hour, minute, second, nanosecond, attosecond);
	calC.ccInit += calC.time.timeInit;

	return calC;
}

CalCoords createCalCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month,
	uint8_t afterDayOfMonth, uint8_t weekNumber, uint8_t dayOfWeek, 
	uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond, uint32_t attosecond,
	uint32_t calendar)
{
	// Create a CalCoords from a weekday rule
	CalCoords calC;

	// Create the date portion
	calC.date = createDateCoordsFromWeekdayRule(gigayear, year, month, afterDayOfMonth,
		weekNumber, dayOfWeek, calendar);
	calC.ccInit = calC.date.dateInit;

	// Create the time portion
	calC.time = createTimeCoords(hour, minute, second, nanosecond, attosecond);
	calC.ccInit += calC.time.timeInit;

	return calC;
}

DateCoords createDateCoords(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar)
{
	// Create a DateCoords from the elements
	DateCoords y1;

	// Check DateCoords elements
	uint16_t init = checkDateCoordsElements(gigayear, year, month, dayOfMonth, calendar);

	// If any of the tests failed, set dateInit and return
	if (init != 0)
	{
		y1.gigayear = 0;
		y1.year = 0;
		y1.month = 0;
		y1.dayOfMonth = 0;
		y1.calendar = 0;
		y1.dateInit = init;
		return y1;
	}

	// Set the fields of y1
	y1.dayOfMonth = dayOfMonth;
	y1.month = month;
	y1.year = year;
	y1.gigayear = gigayear;
	y1.calendar = calendar;
	y1.dateInit = 0;

	return y1;
}


DateCoords createDateCoordsFromDayOfYear(int8_t gigayear, int32_t year, uint32_t dayOfYear, uint32_t calendar)
{
	// Construct a DateCoords from day of year, instead of month and day of month
	uint8_t * md;
	uint8_t nDaysSkipped;
	uint32_t dayJulian, dofy;
	DateCoords y1, transStart, ljd;

	// Check the validity of the elements, setting day and month to 1

	// Check DateCoords elements
	uint16_t init = checkDateCoordsElements(gigayear, year, 1, 1, calendar);

	// January 1st could be an invalid date, so reset init
	init &= !InvalidDayOfMonth;
	init &= !InvalidMonth;

	// If any of the tests failed, set dateInit and return
	if (init != 0)
	{
		y1.gigayear = 0;
		y1.year = 0;
		y1.month = 0;
		y1.dayOfMonth = 0;
		y1.calendar = 0;
		y1.dateInit = init;
		return y1;
	}

	// Check whether the day of year is valid
	if (dayOfYear == 0 || dayOfYear > numDaysInYear(gigayear, year, calendar))
	{
		// Day of year is invalid
		init |= InvalidMonth;
		init |= InvalidDayOfMonth;
		y1.gigayear = 0;
		y1.year = 0;
		y1.month = 0;
		y1.dayOfMonth = 0;
		y1.calendar = 0;
		y1.dateInit = init;
		return y1;
	}
	else
	{
		// The day of year is valid.  Determine the month and day of month.
		if (calendar == 1 || gigayear != 0 || year < EarliestTransitionDate.year ||
			year > LatestTransitionDate.year)
		{
			// Julian calendar or outside of the transition period, so no skipped days
			dofy = dayOfYear;
		}
		else
		{
			// If there is a transition from a Julian to a Gregorian calendar, there are 3
			//	possible cases:
			//  1. The transition occurs entirely within the year
			//	2. The transition occurs at the end of the previous year and extends into the year
			//	3. The transition occurs at the end of the year and extends into the next year
			//
			//	Determine the date the transition begins
			transStart = lastJulianDate(calendar);
			//  Adjust the day of year to a full year basis, if necessary
			if (transStart.year < year - 1 || transStart.year > year)
			{
				// Transition does not affect the year
				dofy = dayOfYear;
			}
			else
			{
				// The transition could affect the year
				//  Determine the number of days skipped
				nDaysSkipped = numDaysToSkip(calendar);
				if (transStart.year == year - 1 && (transStart.month != 12 ||
					transStart.dayOfMonth + nDaysSkipped <= 31))
				{
					// Skipped days are in the previous year
					dofy = dayOfYear;
				}
				else if (transStart.year == year - 1)
				{
					// Transition begins in previous year and carries over into this year
					//  Adjust day of year to full year basis
					dofy = dayOfYear + firstGregorianDate(calendar).dayOfMonth - 1;
				}
				else if (transStart.year == year && (transStart.month != 12 ||
					transStart.dayOfMonth + nDaysSkipped <= 31))
				{
					// Transition occurs entirely during the year
					//  Find the day of year of the last Julian date
					ljd = lastJulianDate(calendar);
					dayJulian = monthDayToDayOfYear(ljd.month,
						ljd.dayOfMonth, numLeapDays(gigayear, year, calendar));
					if (dayOfYear > dayJulian) dofy = dayOfYear + nDaysSkipped;
					else dofy = dayOfYear;
				}
				else
				{
					// Transition beings during the year and ends the next year
					//  No adjustment is necessary, since day of year cannot be
					//	 past the day of year of the last Julian date
					dofy = dayOfYear;
				}
			}
		}
		md = dayOfYearToMonthDay(dofy, numLeapDays(gigayear, year, calendar));
	}

	// Set the fields of y1
	y1.dayOfMonth = md[1];
	y1.month = md[0];
	y1.year = year;
	y1.gigayear = gigayear;
	y1.calendar = calendar;
	y1.dateInit = init;

	return y1;
}

DateCoords createDateCoordsFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month, uint8_t afterDayOfMonth,
	uint8_t weekNumber, uint8_t dayOfWeek, uint32_t calendar)
{
	// Construct a DateCoords from a weekday rule
	DateCoords y1;

	// Check the validity of the elements, setting day of month to 1

	// Check DateCoords elements
	uint16_t init = checkDateCoordsElements(gigayear, year, month, 1, calendar);

	// The first of the month could be an invalid date, so reset init
	init &= !InvalidMonth;

	// If any of the tests failed, set dateInit and return
	if (init != 0)
	{
		y1.gigayear = 0;
		y1.year = 0;
		y1.month = 0;
		y1.dayOfMonth = 0;
		y1.calendar = 0;
		y1.dateInit = init;
		return y1;
	}
	// Determine the day of the month
	y1.dayOfMonth = dayOfMonthFromWeekdayRule(gigayear, year, month, afterDayOfMonth,
		weekNumber, dayOfWeek, calendar);
	// Check validity
	if (y1.dayOfMonth == 0)
	{
		// Invalid specification
		y1.gigayear = 0;
		y1.year = 0;
		y1.month = 0;
		y1.dayOfMonth = 0;
		y1.calendar = 0;
		y1.dateInit = init |= InvalidDayOfMonth;
		abort();
		return y1;
	}
	else
	{
		// Populate the other fields of y1
		y1.gigayear = gigayear;
		y1.year = year;
		y1.month = month;
		y1.calendar = calendar;
		y1.dateInit = init;
		return y1;
	}
}

TimeCoords createTimeCoords(uint8_t hour, uint8_t minute,
	uint8_t second, uint32_t nanosecond, uint32_t attosecond)
{
	// Create a TimeCoords from elements
	TimeCoords t1;

	// Check the validity of the elements
	uint16_t init = checkTimeCoordsElements(hour, minute, second, nanosecond, attosecond);

	// If any of the tests failed, set timeInit and return
	if (init != 0)
	{
		// Invalid specification
		t1.hour = 0;
		t1.minute = 0;
		t1.second = 0;
		t1.nanosecond = 0;
		t1.attosecond = 0;
		t1.timeInit = init;
		return t1;
	}
	else
	{
		// Populate the fields
		t1.hour = hour;
		t1.minute = minute;
		t1.second = second;
		t1.nanosecond = nanosecond;
		t1.attosecond = attosecond;
		t1.timeInit = 0;
		return t1;
	}
}

TAIRelDatetime createTAIRelDatetime(uint16_t gigaSIdays, uint32_t sIdays, uint8_t sIhours,
							uint8_t sIminutes, uint8_t seconds, uint32_t nanoseconds, 
							uint32_t attoseconds, int isNegative, int8_t precision, int8_t uncertainty)
{
	// Create a TAIRelDatetime from elements
	TAIRelDatetime re1;
	uint8_t init;
	// Constrain isNegative flag
	if (isNegative != 0) isNegative = 1;

	// Check that elements are within range
	init = checkRelDatetimeElements(gigaSIdays, sIdays, sIhours,
							sIminutes, seconds, nanoseconds, attoseconds, 
							isNegative, precision, uncertainty);
	// If any of the above tests have failed, set tai, taiInit and return
	re1.relInit = init;
	if (init != 0)
	{
		re1.relTicks = createRelTicks(0,0,0,0,0);
		return re1;
	}
	// Adjust time elements to be consistent with the precision
	return adjustRelDatetimeElements(gigaSIdays, sIdays, sIhours, sIminutes, seconds, nanoseconds,
		attoseconds, isNegative, precision, uncertainty);
}

TAIRelDatetime createTAIRelDatetimeFromDayFrac(double daysPlusFrac, 
							int8_t precision, int8_t uncertainty)
{
	// Create a TAIRelDatetime from days plus a fraction of a day
	//	The maximum allowed value is 41,574,251,721,059 days, 12 hours and 36 seconds
	//		= 4.157 425 172 105 950 x 10^13.
	//	For a double precision number, the machine precision is 1.11x10^-16 * the value
	//
	//	Therefore, the transition points are determined by multiplying the precision
	//		by 9.0073x10^15
	//
	//		days					maximum precision
	//	 > 6.22507x10^12			1 hour
	//	 > 1.04251x10^11			1 minute
	//	 > 1.04251x10^10			1 second
	//		...						...
	//	 > 1.04251x10^10-i			1^-i second
	//	 > 1.04251x10^-7			1^-17 second
	//	 < 1.04251x10^-7			1^-18 second

	TAIRelDatetime re1;
	double secondsPlusFrac;
	double maxDaysPlusFrac = 4.157425172105950e13;
	int8_t maxPrec;
	uint8_t init = 0;
	uint32_t gigaseconds, seconds, nanoseconds, attoseconds;
	int isNegative = daysPlusFrac < 0;
	if (isNegative) daysPlusFrac = -daysPlusFrac;

	// The maximum precision for a relative datetime specified as daysPlusFrac is daysPlusFrac * 86400 / 9.0073x10^15 seconds
	maxPrec = maxPrecision;
	// Consider hour and minute precision separately
	secondsPlusFrac = daysPlusFrac*86400.;
	if (secondsPlusFrac >= 60. * doublePrecision) maxPrec = 2;
	if (secondsPlusFrac >= doublePrecision  && secondsPlusFrac < 60. * doublePrecision) maxPrec = 1; 
	if (secondsPlusFrac > 0.0 && secondsPlusFrac < doublePrecision) maxPrec = subSecondPrecision(daysPlusFrac * secondsPerDay, doublePrecision);
	if (precision < maxPrecision || !isValidUncertainty(uncertainty, precision, 1)) init |= InvalidRelPrecision;

	// Check validity of daysPlusFrac
	if (daysPlusFrac > maxDaysPlusFrac)
	{
		// Invalid daysPlusFrac, so days, hours, minutes, seconds, nanoseconds and attoseconds invalid
		init |= RelTimeGtMax;
		init |= DaysGtMax;
		init |= HoursGtMax;
		init |= MinutesGtMax;
		init |= SecondsGtMax;
		init |= NanosGtMax;
		init |= AttosGtMax;
	}

	// If any of the above tests have failed, set relTicks, relInit and return
	re1.relInit = init;
	if (init != 0)
	{
		re1.relTicks = createRelTicks(0,0,0,0,0);
		return re1;
	}

	// Calculate the elements
	gigaseconds = (uint32_t)(floor(secondsPlusFrac / 1e9));
	seconds = (uint32_t)(floor(secondsPlusFrac - gigaseconds*1e9));
	nanoseconds = (uint32_t)(floor((secondsPlusFrac - gigaseconds*1e9 - seconds)*1e9));
	attoseconds = (uint32_t)(floor((secondsPlusFrac - gigaseconds*1e9 - seconds - nanoseconds/1e9)*1e18));

	re1.relTicks = createRelTicks(gigaseconds, seconds, nanoseconds, attoseconds, isNegative);
	// If precision is unspecified, set to maxPrecision
	if (precision == 99)
	{
		precision = maxPrec;
		// Set the uncertainty to the minimum value, which is half the precision
		//	A 0 value with a precision != 99, will be interpreted as 0.5
		// 
		uncertainty = 0;
	}
	return deriveTAIRelDatetime(re1.relTicks, precision, adjustZeroUncertainty(uncertainty, precision, 1));
}

TAIRelDatetime createTAIRelDatetimeFromSecondFrac(double secondsPlusFrac,
							int8_t precision, int8_t uncertainty)
{
	// Create a TAIRelDatetime from a secondss plus a fraction of a second
	//	The maximum allowed value is 41,574,251,721,059 days, 12 hours and 36 seconds
	//		= 3.592 015 348 699 540 836 x 10^18 seconds
	//  Since we only have 16 digits of precision, round the last six digts to 540 000
	//		Round down to avoid exceeding the maximum.
	//	For a double precision number, the machine precision is 1.11x10^-16 * the value
	//
	//	Therefore, the transition points are determined by multiplying the precision
	//		by 9.0073x10^15
	//
	//		seconds					maximum precision
	//	 > 5.40438x10^17			1 hour
	//	 > 9.00730x10^15			1 minute
	//	 > 9.00730x10^14			1 second
	//		...						...
	//	 > 9.00730x10^14-i			1^-i second
	//	 > 9.00730x10^-3			1^-17 second
	//	 < 9.00730x10^-3			1^-18 second

	TAIRelDatetime re1;
	double maxSecondsPlusFrac = 3.592015348699540836e18;
	int8_t maxPrec;
	uint8_t init = 0;
	uint32_t gigaseconds, seconds, nanoseconds, attoseconds;
	int isNegative = secondsPlusFrac < 0;
	if (isNegative) secondsPlusFrac = -secondsPlusFrac;

	// The maximum precision for a relative datetime specified as secondsPlusFrac is secondsPlusFrac / 9.0073x10^15 seconds
	maxPrec = maxPrecision;
	// Consider hour and minute precision separately
	if (secondsPlusFrac >= 60. * doublePrecision) maxPrec = 2;
	if (secondsPlusFrac >= doublePrecision  && secondsPlusFrac < 60. * doublePrecision) maxPrec = 1; 
	if (secondsPlusFrac > 0.0 && secondsPlusFrac < doublePrecision) maxPrec = subSecondPrecision(secondsPlusFrac, doublePrecision);
	if (precision < maxPrecision || !isValidUncertainty(uncertainty, precision, 1)) init |= InvalidRelPrecision;

	// Check validity of daysPlusFrac
	if (secondsPlusFrac > maxSecondsPlusFrac)
	{
		// Invalid daysPlusFrac, so days, hours, minutes, seconds, nanoseconds and attoseconds invalid
		init |= RelTimeGtMax;
		init |= DaysGtMax;
		init |= HoursGtMax;
		init |= MinutesGtMax;
		init |= SecondsGtMax;
		init |= NanosGtMax;
		init |= AttosGtMax;
	}

	// If any of the above tests have failed, set relTicks, relInit and return
	re1.relInit = init;
	if (init != 0)
	{
		re1.relTicks = createRelTicks(0,0,0,0,0);
		return re1;
	}

	// Calculate the elements
	gigaseconds = (uint32_t)(floor(secondsPlusFrac / 1e9));
	seconds = (uint32_t)(floor(secondsPlusFrac - gigaseconds*1e9));
	nanoseconds = (uint32_t)(floor((secondsPlusFrac - gigaseconds*1e9 - seconds)*1e9));
	attoseconds = (uint32_t)(floor((secondsPlusFrac - gigaseconds*1e9 - seconds - nanoseconds/1e9)*1e18));

	re1.relTicks = createRelTicks(gigaseconds, seconds, nanoseconds, attoseconds, isNegative);
	// If precision is unspecified, set to maxPrecision
	if (precision == 99)
	{
		precision = maxPrec;
		// Set the uncertainty to the minimum value, which is half the precision
		//	A 0 value with a precision != 99, will be interpreted as 0.5
		// 
		uncertainty = 0;
	}
	return deriveTAIRelDatetime(re1.relTicks, precision, adjustZeroUncertainty(uncertainty, precision, 1));
}

RelCalCoords createRelCalCoords(uint8_t gigayears, uint32_t years, uint32_t months, 
	uint32_t days, uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t nanoseconds, 
	uint32_t attoseconds, uint8_t isNegative, int8_t precision, int8_t uncertainty)
{
	//  Create a RelCalCoords from the elements.
	uint32_t gy, y, m, d, h, min, sec, nan, att;
	RelCalCoords rcc;
	//
	//    The values need to be normalized by performing carry operations
	//
	//		Consider gigayears through months, separately from days through attoseconds,
	//		  as there is no robust method to convert days to months
	//
	rcc.rccInit = 0;
	//
	y = years + months / 12;
	m = months % 12;
	gy = gigayears + y / e9;
	y = y % e9;
	//
	//	Check for gy too large
	//
	if (gy > 113) rcc.rccInit |= RelTimeGtMax; // gigayears too large
	//		Now handle attoseconds through days
	nan = nanoseconds + attoseconds / e9;
	att = attoseconds % e9;
	//
	sec = seconds + nan / e9;
	nan = nan % e9;
	//
	min = minutes + sec / 60;
	sec = sec % 60;
	//
	h = hours + min / 60;
	min = min % 60;
	//
	d = days + h / 24;
	h = h % 24;
	// Check for value within range
	//  End of time is 31-Dec-99,999,999,999 23:59:59.999999999 999999999, which is
	//		1-Jan-100,000,000,000 00:00:00 minus 1 attosecond
	//  Creation is 25-Dec-13,826,151,189 BCE 12:00:00.
	//		13,826,151,189 BCE is year -13,826,151,188.
	//		Prior to year 0, there are 13,826,151,187 full years plus 6 days, 12 hours
	//  We already know gy < 114 
	if (gy == 113 &&
		(y > 826151187 || (y == 826151187 && (m > 0 || d > 6 ||
		(d == 6 && (h > 12 || (h == 12 && (min > 0 || sec > 0 || nan > 0 || att > 0))))))))
	{
		// Maximum range exceeded
		rcc.rccInit |= RelTimeGtMax;
	}
	if (precision > 99 || precision < maxPrecision || (precision < 99
		&& precision > minUTCPrecision) || !isValidUncertainty(uncertainty, precision, 0)) 
		rcc.rccInit |= InvalidPrecision;
	//
	//  Fill in the fields
	//
	rcc.gigayears = gy;
	rcc.years = y;
	rcc.months = m;
	rcc.days = d;
	rcc.hours = h;
	rcc.minutes = min;
	rcc.seconds = sec;
	rcc.nanoseconds = nan;
	rcc.attoseconds = att;
	//
	return rcc;
}
	
UncertainDouble createUncertainDouble(double ivalue, double iprecision, double iuncertainty)
{
	// Ensure the validity of the precision and uncertainty for an uncertain double
	//
	double value, precision, uncertainty, unadjprecision, redfact;
	int bufdim = 18, m, adjPrec = 0;
	static char tempbuf[18];

	UncertainDouble u1;


	//	The double precision format exactly represents integers up to 2^53, which is
	//	  9,007,199,254,740,992.  If the precision is 1 or unspecified, the uncertainty is 0, 
	//	  the absolute value is between -2^53 and + 2^53, and the fractional part
	//	  of the value is 0., assume this is an exact value.
	//
	if (ivalue >= -maxExact && ivalue <= maxExact && ivalue == trunc(ivalue) && 
		(iprecision == 1. || iprecision == 0.) && iuncertainty == 0.)
	{
		u1.value = ivalue;
		u1.precision = 1.; // If precision is unspecified, set to 1.
		u1.uncertainty = iuncertainty;
		return u1;
	}

	//	The precision is an absolute value and the uncertainty is a multiplier on the precision
	//	  As such, both values must be >= 0.
	//
	value = fabs(ivalue);
	precision = (iprecision >= 0. ? iprecision : 0.);
	uncertainty = (iuncertainty >= 0. ? iuncertainty : 0.);
	//	The maximum fractional precision for a double is maxFracPrecision
	if (precision < maxFracPrecision * value)
	{
		precision = maxFracPrecision * value;
		adjPrec = 1;
	}

	//	For zero values with zero precision, set the precision to maxfracprecision,
	//	 effectively treating the value as if it were 1.
	//
	if (value == 0. && precision == 0.) precision = maxFracPrecision;

	//	As we are assuming decimal representation of the values, adjust the precision
	//	  to the nearest power of 10 below the specified value.  No need to multiply by 1 + epsilon
	//	  to avoid floating point rounding problems, based on testing.
	//
	unadjprecision = precision;
	precision = pow(10., floor(log10(precision)));
	//	Limit the representation to 16 digits
	//	This is still necessary after the above test since the precision has been adjusted
	if (value != 0. && precision / value < maxFracPrecision)
	{
		precision *= 10.;
		adjPrec = 1;
	}
	//	Finally, consider the difference between: 9 999 999 999 999 999 and 10 000 000 000 000 000.
	//	A precision of 1 is valid for the first value, since it results in 16 significant digits,
	//	but not for the second, since it results in 17 significant digits.  However, both have
	//	a fractional precision of 1.e-16 when rounded to 16 digits.
	//
	//	This is an issue when precision == maxFracPrecision
	//	Test by printing the value and determining the number of digits
	//
	if (value != 0. && precision / value == maxFracPrecision)
	{
		// Sprintf the value over the precision.  If the result has 17 digits,
		//	reduce the precision
		// Clear the string
		memset(tempbuf, ' ', bufdim - 1);
		m = sprintf_s(tempbuf, bufdim, "%17.0f", value / precision);
		// If the first character is not blank, reduce the precision
		if (tempbuf[0] != ' ')
		{
			precision *= 10.;
			adjPrec = 1;
		}
	}
	//	Adjust the uncertainty, accordingly
	uncertainty = unadjprecision * uncertainty / precision;

	//	There is a relationship between the precision and the uncertainty.  
	//	  The minimum sensible value for the uncertainty is 0.5.  A lower value implies the
	//	  precision should be increased.  For example, a value of 1.00 with a 
	//	  precision of 0.01, and an uncertainty of 0.5, indicates the value is between 0.995 and
	//	  1.005.  This is sensible, as the next lower value is 0.99 and the next higher value
	//	  is 1.01.  An uncertainty of 0.1, suggests the value is between 0.999 and 1.001.  We
	//	  cannot have this level of certainty given the precision.
	//
	//   The only exception to the above is when the precision has been reduced
	//	  to limit the number of digits to 16.  In this case,
	//	  the uncertainty is bounded by the minimum fractional uncertainty, which can result
	//	  in a value as low as 0.2
	//
	if (uncertainty < 0.5)
	{
		if (adjPrec && value != 0.) uncertainty = value * minFracUncertainty / precision;
		else uncertainty = 0.5;
	}

	//	Similarly, there is a maximum sensible value for the uncertainty.  If we consider the
	//	  same example as above, but assign an uncertainty of 10., the value would be indicated
	//	  to be between 0.90 and 1.10.  As such, the practical precision is only 0.1, which
	//	  would be represented as 1.0 with a precision of 0.1 and an uncertainty of 1.
	//
	if (uncertainty >= 10.)
	{
		// Reduce the precision by the appropriate power of 10.
		//	minFracUncertainty is an appropriate value to use to avoid rounding errors.
		redfact = pow(10., floor(log10(uncertainty * (1. + minFracUncertainty))));
		precision *= redfact;
		uncertainty /= redfact;
	}

	//  If the magnitude of the precision is greater than the magnitude of the value, 
	//	  the value is effectively noise.  As such, change the value to 0.
	//
	if (value != 0. && (floor(log10(fabs(value))) < floor(log10(precision)))) value = 0.;

	//  Adjust the value based on the precision
	if (value != 0.) value = trunc(value / precision + 0.5) * precision;

	//	Create the uncertainDouble and return
	if (ivalue < 0. && value != 0.)
	{
		u1.value = -value;
	}
	else
	{
		u1.value = value;
	}
	u1.precision = precision;
	u1.uncertainty = uncertainty;
	return u1;
}

// UTCDatetime comparison functions
//	Considers a UTCDatetime to be a range of values
//	Comparisons based on tick values.
//	  Uncertainties are converted to a number of seconds, which will only be approximate in
//	   some cases - e.g., all months are assumed to be 30 days.
//	== 
int mustEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	//Tick value, precision and uncertainty must be equal
	return isEqualTicks(u1.tai, u2.tai) && u1.precision == u2.precision &&
		u1.uncertainty == 0. && u2.uncertainty == 0.;
}

// !=
int mayNotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of mustEqualUTCDatetimes
	return !mustEqualUTCDatetimes(u1, u2);
}

// =='
int isEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isEqualTicks(u1.tai, u2.tai);
}

// !='
int isNotEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Either use isNotEqualTicks or negate isEqualMidpointUTCDatetimes.
	return isNotEqualTicks(u1.tai, u2.tai);
}

// =="
int canEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of cannotEqualUTCDatetimes
	return !cannotEqualUTCDatetimes(u1, u2);
}

// !="
int cannotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Check for non-overlapping ranges
	//	Determine the maximum and minimum values for u1 and u2 by
	//	  creating a zero uncertainty TAIRelDatetime from the uncertainty
	//	  and precision and, respectively, adding it to and subtracting it
	//	  from the UTCDatetimes.  Finally, compare the resulting tick values.
	//
	UTCDatetime maxu1, minu1, maxu2, minu2;
	maxu1 = addRelToUTCDatetime(u1, 
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	minu1 = subtractRelFromUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	maxu2 = addRelToUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	minu2 = subtractRelFromUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isLessTicks(maxu1.tai, minu2.tai) || isLessTicks(maxu2.tai, minu1.tai);
}

// =="'
int isEqualRangeUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Define equal ranges as equality of minimum and maximum values.
	UTCDatetime maxu1, minu1, maxu2, minu2;
	maxu1 = addRelToUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	minu1 = subtractRelFromUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	maxu2 = addRelToUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	minu2 = subtractRelFromUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isEqualTicks(maxu1.tai, maxu2.tai) && isEqualTicks(minu1.tai, minu2.tai);
}

// !="'
int isNotEqualRangeUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of isEqualRangeUTCDatetimes
	return !isEqualRangeUTCDatetimes(u1, u2);
}

// >
int mustBeGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Define must be greater as minimum value of u1 > maximum value of u2
	UTCDatetime minu1, maxu2, z1;
	TAIRelDatetime t1;
	t1 = createTAIRelDatetimeFromSecondFrac(0., 99, 0);
	z1 = subtractRelFromUTCDatetime(u1, t1, 0);
	minu1 = subtractRelFromUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	maxu2 = addRelToUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isGreaterTicks(minu1.tai, maxu2.tai);
}

// <="
int mayBeLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of mustBeGreaterUTCDatetimes
	return !mustBeGreaterUTCDatetimes(u1, u2);
}

// >'
int isGreaterMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isGreaterTicks(u1.tai, u2.tai);
}

// <='
int isLessOrEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isLessOrEqualTicks(u1.tai, u2.tai);
}

// >"
int canBeGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Define can be greater as maxu1 > minu2
	UTCDatetime maxu1, minu2;
	maxu1 = addRelToUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	minu2 = subtractRelFromUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isGreaterTicks(maxu1.tai, minu2.tai);
}

// <=
int mustBeLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of canBeGreaterUTCDatetimes
	return !canBeGreaterUTCDatetimes(u1, u2);
}

// <
int mustBeLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Define must be less as maxu1 < minu2
	UTCDatetime maxu1, minu2;
	maxu1 = addRelToUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	minu2 = subtractRelFromUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isLessTicks(maxu1.tai, minu2.tai);
}

// >="
int mayBeGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of mustBeLessUTCDatetimes
	return !mustBeLessUTCDatetimes(u1, u2);
}

// <'
int isLessMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isLessTicks(u1.tai, u2.tai);
}

// >='
int isGreaterOrEqualMidpointUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isGreaterOrEqualTicks(u1.tai, u2.tai);
}

// <"
int canBeLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Define can be less as minu1 < maxu2
	UTCDatetime minu1, maxu2;
	minu1 = subtractRelFromUTCDatetime(u1,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u1.uncertainty, u1.precision, 0), 99, 0), 1);
	maxu2 = addRelToUTCDatetime(u2,
		createTAIRelDatetimeFromSecondFrac(
			secondsUncertainty(u2.uncertainty, u2.precision, 0), 99, 0), 1);
	return isLessTicks(minu1.tai, maxu2.tai);
}

// >=
int mustBeGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// The negation of canBeLessUTCDatetimes
	return !canBeLessUTCDatetimes(u1, u2);
}

//*** The below will be deprecated when the above have been completed.
//	Perform all comparisons based on the tick count.  This is a relatively lax
//	notion of equality as the precision, uncertainty, calendar and futureAdjust are
//	ignored.
//
/*int isEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isEqualTicks(u1.tai, u2.tai);
}

int isGreaterUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isGreaterTicks(u1.tai, u2.tai);
}

int isLessUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isLessTicks(u1.tai, u2.tai);
}

int isNotEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isNotEqualTicks(u1.tai, u2.tai);
}

int isGreaterOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isGreaterOrEqualTicks(u1.tai, u2.tai);
}

int isLessOrEqualUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	return isLessOrEqualTicks(u1.tai, u2.tai);
}*/

// UTCDatetime utility functions
double doubleSecondsUTCDatetime(UTCDatetime utc)
{
	// Return the number of TAI seconds since creation at a UTCDatetime
	return doubleSeconds(diffTicks(utc.tai, Creation));
}

// TAIRelDatetime comparison functions
//	Perform all comparisons based on the tick count.  This is a relatively lax
//	notion of equality as the precision, uncertainty, calendar and futureAdjust are
//	ignored.
//
int isEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isEqualRelTicks(r1.relTicks, r2.relTicks);
}

int isGreaterRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isGreaterRelTicks(r1.relTicks, r2.relTicks);
}

int isLessRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isLessRelTicks(r1.relTicks, r2.relTicks);
}

int isNotEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isNotEqualRelTicks(r1.relTicks, r2.relTicks);
}

int isGreaterOrEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isGreaterOrEqualRelTicks(r1.relTicks, r2.relTicks);
}

int isLessOrEqualRelDatetimes(TAIRelDatetime r1, TAIRelDatetime r2)
{
	return isLessOrEqualRelTicks(r1.relTicks, r2.relTicks);
}

// TAIRelDatetime utility functions
double doubleSecondsRelDatetime(TAIRelDatetime re)
{
	// Return the value in seconds of a TAIRelDatetime as a double
	return doubleSeconds(re.relTicks);
}

// Uncertain double comparison functions
//	Considers an uncertain double to be a range of values
int mustEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value == u2.value && u1.uncertainty == 0. && u2.uncertainty == 0.
		&& u1.precision == u2.precision;
}

int mayNotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of mustEqualUncertainDoubles
	return !mustEqualUncertainDoubles(u1, u2);
}

int isEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value == u2.value;
}

int isNotEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value != u2.value;
}

int canEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of cannotEqualUncertainDoubles
	return !cannotEqualUncertainDoubles(u1, u2);
}

int cannotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// Check for non-overlapping ranges
	double maxu1, minu1, maxu2, minu2;
	maxu1 = u1.value + u1.uncertainty * u1.precision;
	minu1 = u1.value - u1.uncertainty * u1.precision;
	maxu2 = u2.value + u2.uncertainty * u2.precision;
	minu2 = u2.value - u2.uncertainty * u2.precision;
	return maxu1 < minu2 || maxu2 < minu1;
}

int isEqualRangeUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// A strict interpretation could require that u1.uncertainty == u2.uncertainty &&
	//	u1.precision == u2.precision.  However, this would miss the valid case where the
	//	uncertainty is negative and the precision is 1 unit higher.  For example, an uncertainty of -5 
	//	and a precision of 0.001 is identical to an uncertainty of 5 and a precision of 0.0001.
	//  While arguably, two values with different precisions (e.g., 5 and 10) and compensating uncertainties
	//  (e.g., 2 and 1) could be considered to not have equal ranges, this case need not be considered as the
	//	precisions always will be powers of 10 and the uncertainties will be adjusted to be < 10.
	return u1.value == u2.value && u1.uncertainty * u1.precision == 
		u2.uncertainty * u2.precision;
}

int isNotEqualRangeUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of isEqualRangeUncertainDoubles
	return !isEqualRangeUncertainDoubles(u1, u2);
}

int mustBeGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double minu1, maxu2;
	minu1 = u1.value - u1.uncertainty * u1.precision;
	maxu2 = u2.value + u2.uncertainty * u2.precision;
	return minu1 > maxu2;
}

int mayBeLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of mustBeGreaterUncertainDoubles
	return !mustBeGreaterUncertainDoubles(u1, u2);
}

int isGreaterMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value > u2.value;
}

int isLessOrEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value <= u2.value;
}

int canBeGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double maxu1, minu2;
	maxu1 = u1.value + u1.uncertainty * u1.precision;
	minu2 = u2.value - u2.uncertainty * u2.precision;
	return maxu1 > minu2;
}

int mustBeLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of canBeGreaterUncertainDoubles
	return !canBeGreaterUncertainDoubles(u1, u2);
}

int mustBeLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double maxu1, minu2;
	maxu1 = u1.value + u1.uncertainty * u1.precision;
	minu2 = u2.value - u2.uncertainty * u2.precision;
	return maxu1 < minu2;
}

int mayBeGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of mustBeLessUncertainDoubles
	return !mustBeLessUncertainDoubles(u1, u2);
}

int isLessMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value < u2.value;
}

int isGreaterOrEqualMidpointUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value >= u2.value;
}

int canBeLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double minu1, maxu2;
	minu1 = u1.value - u1.uncertainty * u1.precision;
	maxu2 = u2.value + u2.uncertainty * u2.precision;
	return minu1 < maxu2;
}

int mustBeGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	// The negation of canBeLessUncertainDoubles
	return !canBeLessUncertainDoubles(u1, u2);
}

//**** Deprecate the below when the above have been tested
// UncertainDouble comparison functions
//	Perform all comparisons based on the value.  This is a relatively lax
//	notion of equality as the precision and uncertainty are ignored.
//
int isEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value == u2.value;
}
int isGreaterUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value > u2.value;
}
int isLessUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return u1.value < u2.value;
}
int isNotEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return !isEqualUncertainDoubles(u1, u2);
}
int isGreaterOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return !isLessUncertainDoubles(u1, u2);
}
int isLessOrEqualUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return !isGreaterUncertainDoubles(u1, u2);
}

// Negate UncertainDouble
UncertainDouble negateUncertainDouble(UncertainDouble u1)
{
	UncertainDouble u2;
	u2.value = -u1.value;
	u2.precision = u1.precision;
	u2.uncertainty = u1.uncertainty;
	return u2;
}

// Add UncertainDouble to UncertainDouble
UncertainDouble addUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double value, precision, uncertainty;
	value = u1.value + u2.value;
	precision = (u1.precision > u2.precision ? u1.precision : u2.precision);
	// Precision cannot be 0, so the following is valid:
	uncertainty = (u1.precision * u1.uncertainty + u2.precision * u2.uncertainty) /
		precision;
	return createUncertainDouble(value, precision, uncertainty);
}

// Subtract UncertainDouble from UncertainDouble
UncertainDouble subtractUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	return addUncertainDoubles(u1, negateUncertainDouble(u2));
}

// Multiply UncertainDoubles
UncertainDouble multiplyUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double value, precision, uncertainty, fracprec1, fracprec2, fracprec, fracunc;
	double uzero, unonzero, valnonzero;
	// There are several cases to consider.  In all cases, the value starts
	//	as the product of the values.
	//

	value = u1.value * u2.value;
	//
	//	1. Both values are exact 
	//
	if (u1.uncertainty == 0. && u2.uncertainty == 0.)
	{
		// Check whether the product is still within the range of exact numbers
		if (value >= -maxExact && value <= maxExact)
		{
			// Product is exact
			precision = 1.;
			uncertainty = 0.;
			return createUncertainDouble(value, precision, uncertainty);
		}
		else
		{
			// Product is outside the range of exact numbers
			//	Set precision and uncertainty to the maximum values (i.e., most precise)
			precision = maxFracPrecision * fabs(value);
			uncertainty = 1.;
			return createUncertainDouble(value, precision, uncertainty);
		}
	}
	//
	//	2. One of the values is exact and one is uncertain
	//
	if (u1.uncertainty == 0. || u2.uncertainty == 0.)
	{
		// Use the fractional precision and uncertainty of the uncertain value,
		//	which is equivalent to multiplying the precision by the exact value
		//	and leaving the uncertainty unchanged.
		//
		if (u1.uncertainty == 0.)
		{
			precision = u2.precision * fabs(u1.value);
			uncertainty = u2.uncertainty;
		}
		else
		{
			precision = u1.precision * fabs(u2.value);
			uncertainty = u1.uncertainty;
		}
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	// 3. Both values are non-zero
	//
	if (value != 0.)
	{
		// The fractional precision is the maximum (i.e., least precise) fractional
		//	precision of the 2 factors
		fracprec1 = u1.precision / fabs(u1.value);
		fracprec2 = u2.precision / fabs(u2.value);
		fracprec = (fracprec1 > fracprec2 ? fracprec1 : fracprec2);
		precision = fracprec * fabs(value);
		// The fractional uncertainty is the sum of the 2 fractional uncertainties
		fracunc = u1.uncertainty * u1.precision / fabs(u1.value) +
			u2.uncertainty * u2.precision / fabs(u2.value);
		uncertainty = fracunc * fabs(value) / precision;
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	// 4. Both values are zero
	//
	if (u1.value == 0. && u2.value == 0.)
	{
		// A zero value is assumed to be 0 times 10 to a power.
		//	As such, the precision is that power.
		//	Thus, the product of two zeros, would 0 times 10 to
		//	the sum of the powers, and the precision would be the 10
		//	to the sum of those powers, or the product of the precisions.
		//
		precision = u1.precision * u2.precision;
		// The allowed range for each value is 0 +/- uncertainty.
		//	Thus, the range for the product is 0 +/- u1*u2
		//	The precisions cancel in the numerator and denominator of the following
		uncertainty = u1.uncertainty * u2.uncertainty;
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	// 5. Exactly one of the values is zero
	//
	// The range of values for the zero value is 0 +/- uzero
	//	The range of values for the non-zero value is value +/- unonzero
	//  Therefore, the range of values for the product is,
	//	for positive values: 0 +/- uzero*(value + unonzero)
	//	for negative values: 0 +/- uzero*(value - unonzero)
	//
	//	As such, set the precision to uzero*value and the uncertainty to
	//	either (value + unonzero) or (value - unonzero) * uzero, remembering
	//	the uncertainty must be divided by the precision
	//
	if (u1.value == 0.)
	{
		// u2 is the non-zero value
		uzero = u1.uncertainty * u1.precision;
		valnonzero = u2.value;
		unonzero = u2.precision * u2.uncertainty;
	}
	else
	{
		// u1 is the non-zero value
		uzero = u2.uncertainty * u2.precision;
		valnonzero = u1.value;
		unonzero = u1.precision * u1.uncertainty;
	}
	//
	precision = uzero*fabs(valnonzero);
	//
	if (valnonzero > 0.)
	{
		uncertainty = (valnonzero + unonzero) * uzero / precision;
	}
	else
	{
		uncertainty = -(valnonzero - unonzero) * uzero / precision;
	}
	return createUncertainDouble(value, precision, uncertainty);
}

// Divide UncertainDoubles
UncertainDouble divideUncertainDoubles(UncertainDouble u1, UncertainDouble u2)
{
	double value, precision, uncertainty;
	double numval, denval, numprec, denprec, numunc, denunc;
	double fracprecnum, fracprecden, fracprec, fracunc;
	UncertainDouble u3;
	// There are several cases to consider
	//
	//	1. The denominator is 0.
	//
	if (u2.value == 0.)
	{
		// Set value, precision and uncertainty to infinity
		//  Do not call createUncertainDouble
		u3.value = DBL_MAX * 2.0;
		u3.precision = u3.value;
		u3.uncertainty = u3.value;
		return u3;
	}
	// At this point, we can compute the value
	//
	numval = u1.value;
	denval = u2.value;
	numprec = u1.precision;
	denprec = u2.precision;
	numunc = u1.uncertainty;
	denunc = u2.uncertainty;
	value = numval / denval;
	//
	//	2. Both the numerator and denominator are exact
	//
	if (numunc == 0. && denunc == 0.)
	{
		// Don't need to check whether the quotient is within range, but do need
		//	to check whether it is fractional and whether it is exact, since the 
		//	value == trunc(value) test could give a false positive for very small
		//  fractional quotients
		//
		if (numval >= denval && value == trunc(value))
		{
			// The quotient is exact
			precision = 1.;
			uncertainty = 0.;
		}
		else
		{
			// As both values were exact, assign the maximum fractional precision to
			//	the quotient
			precision = maxFracPrecision * fabs(value);
			uncertainty = 1.;
		}
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	//	3. Either the numerator or denominator, but not both, is exact
	//
	if (numunc == 0. || denunc == 0.)
	{
		// Use the fractional precision and uncertainty of the uncertain value
		//
		if (numunc == 0.)
		{
			// The numerator is exact
			precision = denprec / fabs(denval) * fabs(value);
			uncertainty = denunc;
		}
		else
		{
			// The denominator is exact
			if (numval != 0.)
			{
				precision = numprec / fabs(numval) * fabs(value);
				uncertainty = numunc;
			}
			else
			{
				// The numerator is an uncertain 0.
				//	Divide the precision of the numerator by the value of the denominator
				precision = numprec / fabs(denval);
				uncertainty = numunc;
			}
		}
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	//	4. Both the numerator and denominator are non-zero, uncertain values
	//
	if (numval != 0.)
	{
		// The fractional precision is the maximum (i.e., least precise) fractional
		//	precision of the numerator and denominator
		fracprecnum = numprec / fabs(numval);
		fracprecden = denprec / fabs(denval);
		fracprec = (fracprecnum > fracprecden ? fracprecnum : fracprecden);
		precision = fracprec * fabs(value);
		// Add the fractional uncertainties
		fracunc = numunc * numprec / fabs(numval) + denunc * denprec /
			fabs(denval);
		uncertainty = fracunc * fabs(value) / precision;
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	//	5. The numerator is an uncertain 0. and the denominator is a non-zero, uncertain value
	//
	value = 0.;
	//	Divide the precision of the numerator by the value of the denominator
	precision = numprec / fabs(denval);
	//	Multiply the uncertainty of the numerator by (1 + fractional uncertainty of the denominator)
	uncertainty = numunc * (1. + denprec * denunc / fabs(denval));
	return createUncertainDouble(value, precision, uncertainty);
}

char * reportUTCConstructionError(uint16_t init, char buffer[], int lenBuffer, int failTest)
{
	// Report errors in UTC construction based on bit flag, which
	//	is set in createUTCDatetime and calcTicks

	int offset = 0;
	int numError = 0;
	memset(buffer, ' ', lenBuffer - 1);

	if (init != 0)
	{
			// There is at least one error.  Write the errors one per line.
			//	Write FAILED OR	PASSED, depending on whether the test was designed to pass or fail
		offset += (failTest ? sprintf_s(buffer, lenBuffer, "PASSED ") :
			sprintf_s(buffer, lenBuffer, "FAILED "));
		if (init & NegativeTime)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[0]);
			numError ++;
		}
		if (init & TimeGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[1]);
			numError ++;
		}
		if (init & InvalidYear)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[2]);
			numError ++;
		}
		if (init & InvalidMonth)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[3]);
			numError ++;
		}
		if (init & InvalidDayOfMonth)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[4]);
			numError ++;
		}
		if (init & InvalidCalendar)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[5]);
			numError++;
		}
		if (init & HourMinSecGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[6]);
			numError ++;
		}
		if (init & NanoGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[7]);
			numError ++;
		}
		if (init & AttoGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[8]);
			numError ++;
		}
		if (init & NotALeapSecond)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[9]);
			numError ++;
		}
		if (init & InvalidPrecision)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[10]);
			numError ++;
		}
		if (init & InvalidFutureAdjust)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[11]);
			numError ++;
		}
		if (init & InvalidTimeZone)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[12]);
			numError ++;
		}
		if (init & InvalidTimeFrame)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[13]);
			numError ++;
		}
		if (init & InvalidDatetime)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[14]);
			numError++;
		}
		if (init & MissingBOrA)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", UTCErrorStrings[15]);
			numError++;
		}
	}
	return buffer;
}

char * reportTAIRelConstructionError(uint8_t init, char buffer[], int lenBuffer, int failTest)
{
	// Report errors in TAIRelDatetime construction based on bit flag, which
	//	is set in createTAIRelDatetime

	int offset = 0;
	int numError = 0;
	memset(buffer, ' ', lenBuffer - 1);

	if (init != 0)
	{
		// There is at least one error.  Write the errors one per line.
		//	Write FAILED OR	PASSED, depending on whether the test was designed to pass or fail
		offset += (failTest ? sprintf_s(buffer,  lenBuffer, "PASSED ") : 
			sprintf_s(buffer, lenBuffer, "FAILED "));
		if (init & RelTimeGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[0]);
			numError ++;
		}
		if (init & DaysGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[1]);
			numError ++;
		}
		if (init & HoursGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[2]);
			numError ++;
		}
		if (init & MinutesGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[3]);
			numError ++;
		}
		if (init & SecondsGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[4]);
			numError ++;
		}
		if (init & NanosGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[5]);
			numError ++;
		}
		if (init & AttosGtMax)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[6]);
			numError ++;
		}
		if (init & InvalidRelPrecision)
		{
			if (numError > 0) offset += sprintf_s(buffer + offset, lenBuffer - offset, " / ");
			offset += sprintf_s(buffer + offset, lenBuffer - offset, "%s", TAIRelErrorStrings[7]);
			numError ++;
		}
	}
	return buffer;
}

// Derive relative tick elements from relative ticks
TAIRelDatetime deriveTAIRelDatetime(TAIRelTicks r1, int8_t precision, int8_t uncertainty)
{
	TAIRelDatetime re;
	uint32_t gigaseconds, seconds, remGigaseconds, remSeconds;
	uint32_t days;

	// Check precision
	re.relInit = 0;
	if (precision > 99 || precision < maxPrecision || !isValidUncertainty(uncertainty, precision, 1)) re.relInit |= InvalidRelPrecision;
	// Check the reltick value is within the allowed range
	if (isGreaterRelTicks(r1, MaxRelTime) || isLessRelTicks(r1, negateRelTicks(MaxRelTime)))
	{
		// Tick value outside the allowed range
		re.relInit |= RelTimeGtMax;
		re.relTicks = (isNegativeRelTicks(r1) ? negateRelTicks(MaxRelTime) : MaxRelTime);
	}
	if (re.relInit !=0) return re;

	// Elements derived from gigaseconds and seconds
	gigaseconds = getGigaseconds(r1);
	seconds = getSeconds(r1);

	// Compute elements
	re.gigadays = gigaseconds / secondsPerDay;
	remGigaseconds = gigaseconds % secondsPerDay; // Must be < 86400

	// Calculate number of days in remaining gigaseconds, utilizing the
	//	fact that there are 10,000 days in 864 megaseconds
	days = (1000*remGigaseconds) / (secondsPerDay / 100) * 10000; // ((Megaseconds) / (Megaseconds / 10000 days)) * 10000
	// At this point, remaining time is < 864 megaseconds, so just add to 
	//	seconds, since will still be < 2e9
	remSeconds = (1000*remGigaseconds - (days/10000) * (secondsPerDay/100)) * 1000000 + seconds;
	re.days = days + remSeconds / secondsPerDay;

	// Now calculate hours, minutes and seconds
	remSeconds = remSeconds % secondsPerDay;
	re.hours = remSeconds / 3600;

	remSeconds = remSeconds % 3600;
	re.minutes = remSeconds / 60;

	re.seconds = remSeconds % 60;

	re.precision = precision;
	re.uncertainty = uncertainty;
	re.relTicks = r1;
	return adjustRelDatetimeElements(re.gigadays, re.days, re.hours, re.minutes, re.seconds, getNanoseconds(r1),
		getAttoseconds(r1), isNegativeRelTicks(r1), re.precision, re.uncertainty);
}

// Subtract UTCDatetimes
TAIRelDatetime diffUTCDatetimes(UTCDatetime u1, UTCDatetime u2)
{
	// Subtract UTCDatetimes and return a TAIRelDatetime

	// Subtract the ticks 
	TAIRelTicks r1 = diffTicks(u1.tai, u2.tai);

	// Calculate the precision and uncertainty
	PrecisionUncertainty pu1 = addPrecisionUncertainty(u1.precision, u1.uncertainty, 0, u2.precision,
		u2.uncertainty, 0);

	// Derive the TAIRelDatetime from the reltick value
	return deriveTAIRelDatetime(r1, pu1.precision, pu1.uncertainty);
}

UTCDatetime addRelToUTCDatetime(UTCDatetime u1, TAIRelDatetime re1, uint8_t futureAdjust)
{
	// Add a TAI relative datetime to a UTC datetime and return a UTC datetime.
	//	Allow for a futureAdjust value different than the input UTC datetime, but
	//	keep the same calendar.

	// Add the tick values
	TAITicks t1 = addRelTicksToTicks(u1.tai, re1.relTicks);

	// Calculate the precision and uncertainty
	PrecisionUncertainty pu1 = addPrecisionUncertainty(u1.precision, u1.uncertainty, 0, 
		re1.precision, re1.uncertainty, 1);

	// Derive the UTCDatetime from the tick value
	//  If there was an overflow, t1 will be set to EndOfTimePlus, triggering an error
	//		in deriveUTCDatetime
	return deriveUTCDatetime(t1, pu1.precision, pu1.uncertainty, futureAdjust);
}

UTCDatetime subtractRelFromUTCDatetime(UTCDatetime u1, TAIRelDatetime re1, uint8_t futureAdjust)
{
	// Subtract a TAI relative datetime from a UTC datetime and return a UTC datetime.
	//	Allow for a futureAdjust value different than the input UTC datetime, but
	//	keep the same calendar.

	// Add the tick values
	TAITicks t1 = subtractRelTicksFromTicks(u1.tai, re1.relTicks);

	// Calculate the precision and uncertainty
	PrecisionUncertainty pu1 = addPrecisionUncertainty(u1.precision, u1.uncertainty, 0,
		re1.precision, re1.uncertainty, 1);

	// Derive the UTCDatetime from the tick value
	//  If there was an overflow, t1 will be set to EndOfTimePlus, triggering an error
	//		in deriveUTCDatetime
	return deriveUTCDatetime(t1, pu1.precision, pu1.uncertainty, futureAdjust);
}

TAIRelDatetime addRelDatetimes(TAIRelDatetime re1, TAIRelDatetime re2)
{
	// Add two relative datetimes and return a relative date time

	// Add the relative tick values
	TAIRelTicks r1 = addRelTicks(re1.relTicks, re2.relTicks);

	// Calculate the precision and uncertainty
	PrecisionUncertainty pu1 = addPrecisionUncertainty(re1.precision, re1.uncertainty, 1,
		re2.precision, re2.uncertainty, 1);

	// Derive the TAIRelDatetime from the tick value
	//  If there was an overflow, r1 will be set to MaxRelTimePlus, triggering an error
	//		in deriveTAIRelDatetime
	return deriveTAIRelDatetime(r1, pu1.precision, pu1.uncertainty);
}

TAIRelDatetime subtractRelDatetimes(TAIRelDatetime re1, TAIRelDatetime re2)
{
	// Subtract a relative datetime from a relative datetime and return a relative date time

	// Subtract the relative tick values
	TAIRelTicks r1 = subtractRelTicks(re1.relTicks, re2.relTicks);

	// Calculate the precision and uncertainty
	PrecisionUncertainty pu1 = addPrecisionUncertainty(re1.precision, re1.uncertainty, 1,
		re2.precision, re2.uncertainty, 1);

	// Derive the TAIRelDatetime from the relative tick value
	//  If there was an overflow, r1 will be set to MaxRelTimePlus, triggering an error
	//		in deriveTAIRelDatetime
	return deriveTAIRelDatetime(r1, pu1.precision, pu1.uncertainty);
}

TAIRelDatetime negateRelDatetime(TAIRelDatetime re1)
{
	// Negate a relative datetime

	// Negate the relative tick value
	TAIRelTicks r1 = negateRelTicks(re1.relTicks);

	// Derive the TAIRelDatetime from the relative tick value
	return deriveTAIRelDatetime(r1, re1.precision, re1.uncertainty);
}

UncertainDouble ratioRelDatetimes(TAIRelDatetime num, TAIRelDatetime den)
{
	// Calculate the ratio of two TAIRelDatetimes
	//	Convert to double precision then divide
	//	Precision and uncertainty are in seconds throughout the function
	//	  throughout the function
	//
	UncertainDouble rr;

	double value, numval, denval, numunc, denunc, numprec, denprec;
	double fracprecnum, fracprecden, fracprec, precision, fracunc, uncertainty;
	int8_t iprecn, iprecd, iuncn, iuncd;

	numval = doubleSecondsRelDatetime(num);
	denval = doubleSecondsRelDatetime(den);

	// There are several cases to consider
	//
	//	1. The denominator is 0.
	//
	if (denval == 0.)
	{
		// Establish an infinite value for the ratio, precision and uncertainty
		rr.value = DBL_MAX * 2.0;
		rr.precision = rr.value;
		rr.uncertainty = rr.value;
		return rr;
	}
	else
	{
		// If the precision of either value is unspecified, set to maximum precision
		//	and minimum uncertainty
		if (num.precision == 99)
		{
			iprecn = maxPrecision;
			iuncn = -5;
		}
		else
		{
			iprecn = num.precision;
			iuncn = num.uncertainty;
		}
		if (den.precision == 99)
		{
			iprecd = maxPrecision;
			iuncd = -5;
		}
		else
		{
			iprecd = den.precision;
			iuncd = den.uncertainty;
		}
		numprec = precSecondsTAIRelDatetime[poff + iprecn];
		denprec = precSecondsTAIRelDatetime[poff + iprecd];
		// Use bounded uncertainties, since the values are represented as doubles
		numunc = boundedSecondsUncertainty(iuncn, iprecn, 1, numval);
		denunc = boundedSecondsUncertainty(iuncd, iprecd, 1, denval);
		value = numval / denval;

		// Determine the sign
		if ((isNegativeRelTicks(num.relTicks) && !isNegativeRelTicks(den.relTicks)) ||
			(!isNegativeRelTicks(num.relTicks) && isNegativeRelTicks(den.relTicks))) value = -value;

	}
	//
	//	2. Both the numerator and denominator are non-zero
	//
	if (numval != 0.)
	{
		// The fractional precision is the maximum (i.e., least precise) fractional
		//	precision of the numerator and denominator. 
		//
		fracprecnum = numprec / fabs(numval);
		fracprecden = denprec / fabs(denval);
		fracprec = (fracprecnum > fracprecden ? fracprecnum : fracprecden);
		// Bound fracprec to double precision
		fracprec = (fracprec < maxFracPrecision ? maxFracPrecision : fracprec);
		precision = fracprec * fabs(value);
		// Add the fractional uncertainties
		fracunc = numunc / fabs(numval) + denunc /
			fabs(denval);
		uncertainty = fracunc * fabs(value) / precision;
		return createUncertainDouble(value, precision, uncertainty);
	}
	//
	//	3. The numerator is an uncertain 0. and the denominator is a non-zero, uncertain value
	//
	value = 0.;
	//	Divide the precision of the numerator by the value of the denominator
	precision = numprec / fabs(denval);
	//	Multiply the uncertainty of the numerator by (1 + fractional uncertainty of the denominator)
	uncertainty = numunc * (1. + denprec * denunc / fabs(denval));
	return createUncertainDouble(value, precision, uncertainty);
}

TAIRelDatetime doubleMultRelDatetime(UncertainDouble rr, TAIRelDatetime re1)
{
	// Multiply a TAIRelDatetime by a double and return a TAIRelDatetime
	double gigasecs, gigaprod, amult, prec;
	double unc;
	int isNegativeResult;
	int8_t iprec, iunc;
	TAIRelTicks r1, r2;
	int8_t precision;
	int8_t uncertainty;
	uint32_t r2gig, r2sec, r2nan, r2att;
	double uzero, valnonzero, unonzero;
	double uncre1, fracunc;
	PrecisionUncertainty pu1;

	int isNegativeMult = rr.value < 0;

	r1 = re1.relTicks;

	// Express r1 as a number of gigaseconds in the form of a double
	gigasecs = fabs(doubleSeconds(r1)) / 1.e9;

	// Determine the sign of the product
	isNegativeResult = ((isNegativeMult && !isNegativeRelTicks(r1)) || (!isNegativeMult && isNegativeRelTicks(r1)));

	// Perform a conservative check for greater than allowed max using double precision values
	amult = fabs(rr.value);
	if ((double)amult * gigasecs > dMaxRelTimeGiga)
	{
		// The multiplication will produce a value greater than the max.  Set to maximum value plus a tick.
		r2 = MaxRelTimePlus;
		precision = 99;
		uncertainty = 0;
		return deriveTAIRelDatetime(r2, precision, adjustZeroUncertainty(uncertainty, precision, 1));
	}
	else
	{
		// Compute the number of gigaseconds in the product
		gigaprod = amult * gigasecs;

		// Derive the elements of r2
		r2gig = (uint32_t)floor(gigaprod);
		r2sec = (uint32_t)floor((gigaprod - (double)r2gig)*1e9);
		r2nan = (uint32_t)floor((gigaprod - (double)r2gig - (double)(r2sec) / 1e9)*1e18);
		r2att = (uint32_t)floor((gigaprod - (double)r2gig - (double)(r2sec) / 1e9 - (double)(r2nan) / 1e18)*1e27);
		r2 = createRelTicks(r2gig, r2sec, r2nan, r2att, 0);

		// Even if the value passed the initial check, might still be greater than the allowed max
		if (isGreaterRelTicks(r2, MaxRelTime))
		{
			r2 = MaxRelTimePlus;
			precision = 99;
			uncertainty = 0;
			return deriveTAIRelDatetime(r2, precision, adjustZeroUncertainty(uncertainty, precision, 1));
		}
		// Establish the sign
		if (isNegativeResult && gigaprod != 0.) r2 = negateRelTicks(r2);

		// Compute the precision and uncertainty
		// There are several cases to consider.
		//
		//	1. The uncertain double is exact
		//		Computes correctly for zero and non-zero values of re1.
		//
		if (rr.uncertainty == 0.)
		{
			// Use the same technique as in intMultTAIRelDatetime
			//
			// Calculate the precision and uncertainty by first determining the uncertainty
			//	then determining a precision so the uncertainty is within range
			//
			//  Calculate the absolute value of the uncertainty (i.e., uncertainty * precision)
			//		in seconds
			uncre1 = boundedSecondsUncertainty(re1.uncertainty, re1.precision, 1,
				gigasecs * 1.e9);
			unc = fabs(rr.value) * uncre1; // Uncertainty in seconds
			//
			//  Find the precision / uncertainty pair that is consistent with the uncertainty
			//			in seconds
			//
			pu1 = precisionFromSecondsUncertainty(unc, 1);
			//
			return deriveTAIRelDatetime(r2, pu1.precision, pu1.uncertainty);
		}
		//
		// 2. Both values are non-zero
		//
		if (gigaprod != 0.)
		{
			// The fractional precision is the maximum (i.e., least precise) fractional
			//	precision of the 2 factors
			// However, there is no need to compute the precision, as it will be derived
			//	from the uncertainty
			//***
			//fracprec1 = rr.precision / fabs(rr.value);
			//fracprec2 = 1.e9 * precSecondsTAIRelDatetime[re1.precision + poff] / gigasecs;
			//fracprec = (fracprec1 > fracprec2 ? fracprec1 : fracprec2);
			//prec = fracprec * gigaprod; // Absolute precision in gigaseconds
			//***
			//
			// The fractional uncertainty is the sum of the 2 fractional uncertainties
			//	Bound the uncertainty of the TAIRelDatetime, since the multiplication
			//	was performed with a double representation of gigasecs
			uncre1 = boundedSecondsUncertainty(re1.uncertainty, re1.precision, 1,
				gigasecs * 1.e9);
			fracunc = rr.uncertainty * rr.precision / fabs(rr.value) +
				uncre1 / gigasecs / 1.e9;
			unc = fracunc * gigaprod * 1.e9; // Uncertainty in seconds
			// Find the precision / uncertainty pair that is consistent with the uncertainty
			//		in seconds
			pu1 = precisionFromSecondsUncertainty(unc, 1);
			//
			return deriveTAIRelDatetime(r2, pu1.precision, pu1.uncertainty);
		}
		//
		// 3. Both values are zero
		//
		if (rr.value == 0. && gigasecs == 0.)
		{
			// A zero value is assumed to be 0 times 10 to a power.
			//	As such, the precision is that power.
			//	Thus, the product of two zeros, would be 0 times 10 to
			//	the sum of the powers, and the precision would be 10
			//	to the sum of those powers, or the product of the precisions.
			//
			// There is no need to calculate the precision, as it will be
			//	derived from the uncertainty.
			//	*** 	prec = rr.precision * precSecondsTAIRelDatetime[poff + iprec];
			if (re1.precision == 99)
			{
				//	If the precision of re1 is unspecified, set to the maximum
				iprec = maxPrecision;
				iunc = -5;
			}
			else
			{
				iprec = re1.precision;
				iunc = re1.uncertainty;
			}
			// The allowed range for each value is 0 +/- uncertainty, where uncertainty is not
			//		normalized by precision.
			//	Thus, the range for the product is 0 +/- u1*u2
			//	No need to bound the uncertainty for re1, since the value is 0.
			unc = rr.uncertainty * rr.precision * secondsUncertainty(iunc, iprec, 1);
			// Find the precision / uncertainty pair that is consistent with the uncertainty
			//		in seconds
			pu1 = precisionFromSecondsUncertainty(unc, 1);
			//
			return deriveTAIRelDatetime(r2, pu1.precision, pu1.uncertainty);
		}
		//
		// 4. Exactly one of the values is zero
		//
		// The range of values for the zero value is 0 +/- uzero
		//	The range of values for the non-zero value is value +/- unonzero
		//  Therefore, the range of values for the product is,
		//	for positive values: 0 +/- uzero*(value + unonzero)
		//	for negative values: 0 +/- uzero*(value - unonzero)
		//
		//	As such, set the precision to uzero*value and the uncertainty to
		//	either (value + unonzero) or (value - unonzero) * uzero, remembering
		//	the uncertainty must be divided by the precision
		//
		//	In this case, we will not calculate the precision.  It will be derived
		//	from the uncertainty.
		//
		if (rr.value == 0.)
		{
			// re1 is the non-zero value
			uzero = rr.uncertainty * rr.precision; // Cannot be zero or would have been case 1
			valnonzero = gigasecs * 1.e9;
			unonzero = boundedSecondsUncertainty(re1.uncertainty, re1.precision, 1, valnonzero);
		}
		else
		{
			// rr is the non-zero value
			uzero = boundedSecondsUncertainty(re1.uncertainty, re1.precision, 1, gigasecs*1.e9);
			valnonzero = rr.value;
			unonzero = rr.precision * rr.uncertainty;
		}
		//
		prec = uzero*fabs(valnonzero);
		//
		if (valnonzero > 0.)
		{
			unc = (valnonzero + unonzero) * uzero / prec;
		}
		else
		{
			unc = -(valnonzero - unonzero) * uzero / prec;
		}
		// Find the precision / uncertainty pair that is consistent with the uncertainty
		//		in seconds
		pu1 = precisionFromSecondsUncertainty(unc, 1);
		//
		return deriveTAIRelDatetime(r2, pu1.precision, pu1.uncertainty);
	}
}

TAIRelDatetime intMultRelDatetime(int32_t mult, TAIRelDatetime re1)
{
	// Multiply a TAIRelDatetime by a 32 bit integer and return a TAIRelDatetime
	int8_t precision, uncertainty;
	double unc, uncre1, gigasecs, amult;
	PrecisionUncertainty pu1;
	TAIRelTicks r1;

	// Express re1 as a number of gigaseconds in the form of a double
	gigasecs = doubleSeconds(re1.relTicks) / 1.e9;

	// Perform a conservative check for greater than allowed max using double precision values
	amult = fabs((double)mult);
	if (amult * gigasecs > dMaxRelTimeGiga)
	{
		// The multiplication will produce a value greater than the max.  Set to maximum value plus a tick.
		r1 = MaxRelTimePlus;
		precision = 99;
		uncertainty = 0;
	}
	else
	{
		// Calculate the relative tick value
		r1 = intMultTAIRelTicks(mult, re1.relTicks);

		// Even if the value passed the initial check, might still be greater than the allowed max
		if (isGreaterRelTicks(r1, MaxRelTime))
		{
			r1 = MaxRelTimePlus;
			precision = 99;
			uncertainty = 0;
		}
		else
		{
			if (re1.precision == 99)
			{
				// Precision unspecified.  As the multiplication is
				//	exact (since only integers are involved), no need
				//  to impose a maximum precision when precision is unspecified.
				//
				precision = 99;
				uncertainty = 0;
			}
			else
			{
				// Calculate the precision and uncertainty by first determining the uncertainty
				//	then determining a precision so the uncertainty is within range
				//
				//  Calculate the absolute value of the uncertainty (i.e., uncertainty * precision)
				//		in seconds
				uncre1 = secondsUncertainty(re1.uncertainty, re1.precision, 1);
				unc = amult * uncre1;
				//
				//  Find the precision / uncertainty pair that is consistent with the uncertainty
				//			in seconds
				//
				pu1 = precisionFromSecondsUncertainty(unc, 1);
				precision = pu1.precision;
				uncertainty = pu1.uncertainty;
			}
		}
	}
	// Derive the relative datetime elements
	return deriveTAIRelDatetime(r1, precision, uncertainty);
}

// Derive UTCDatetime from ticks
UTCDatetime deriveUTCDatetime(TAITicks t1, int8_t precision, int8_t uncertainty,
							uint8_t futureAdjust)
{
	// Return a copy of utc, with the date and time elements calculated
	UTCDatetime utc;
	uint32_t gigaYears;
	uint32_t tenMegaYears, hundredKYears, kYears, twentyYears, fourYears, years;
	uint32_t fourKYears, fourHundredYears;
	uint8_t hours, minutes, seconds;
	uint32_t days;
	uint32_t remTenMegaseconds, rem100Kseconds, remKseconds, rem20seconds, remSeconds;
	uint32_t rem4Kseconds, rem400seconds, rem10seconds;
	int numCenturies;
	uint32_t dayOfYear;
	uint8_t * md;
	int cumLeapSecs = 0;
	int isLeapSec;

	static const int32_t bbYear = -826151188; // Big Bang year + 13e9
//	int32_t oct4 = monthDayToDayOfYear(10, 4, 0);
	static const uint32_t secondsIn4JulianYears = 126230400; // 1461 * secondsPerDay; 
//	static const uint32_t lengthOfCreation = 561600; // 6 * secondsPerDay + secondsPerDay / 2;

	// Julian year initializations - 4 year cycle with one leap year every 4 years
	//   There are 2 different year lengths, but every 4 year cycle is the same length

	// In order to avoid theese calculations being performed on every call, the variables need to be static.
	//		However, calculations can not be performed on the initialization of static variables, so
	//			just assign to a precomputed value.

	//uint32_t secondsInAverageGregorianYear = (303*365 + 97*366)*864/4; // 31,556,952
	//uint32_t gigaSecsInJulianGigaYear = secondsIn4JulianYears / 4; // Value is an integer since seconds in 4 years is multiple of 400 - 31,557,600
	//
	static const uint32_t gigaSecsInJulianGigaYear = 31557600;

	/* Gregorian year initializations - 400 year cycle with leap years every 4 years except centuries not divisible by 400
	   As for Julian years, there are 2 different year lenghts, however the cycle is 400 years, not 4 years
			Given that the number of seconds in 400 years > 12e9, we need a slightly different approach than that used for
			Julian years.  Specifically, we will consider an "average" Gregorian year, which has 365.2425 days, so has
			a length of 31,556,952 seconds. */

	//uint32_t gigaSecsInGregorianGigaYear = secondsInAverageGregorianYear; // Value is an integer
	static const uint32_t gigaSecsInGregorianGigaYear = 31556952;
	TAIRelTicks r1, r1Adj;

	//
	//  Set input parameters
	//
	utc.taiInit = 0;
	if (precision > 99 || precision < maxPrecision || !isValidUncertainty(uncertainty, precision, 0)) utc.taiInit |= InvalidPrecision;
	if (futureAdjust > 2) utc.taiInit |= InvalidFutureAdjust;
	if (isGreaterTicks(t1, EndOfTime)) 
	{
		// Past the end of time.
		utc.taiInit |= TimeGtMax;
		t1 = EndOfTime;
	}
	utc.futureAdjust = futureAdjust;
	utc.precision = precision;
	utc.uncertainty = uncertainty;

	utc.tai = t1;
	// Return if any parameters are invalid
	if (utc.taiInit != 0) return utc;

	//  Detemine the period
	if (isGreaterOrEqualTicks(t1, Creation) && isLessTicks(t1, Prehistoric)) 
	{
		// Within the creation period.  This is only a 6.5 day period
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, Creation);
		days = getSeconds(r1) / secondsPerDay;
		remSeconds = getSeconds(r1) % secondsPerDay;

		hours = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		minutes = (uint8_t)(remSeconds / 60);
		seconds = (uint8_t)(remSeconds % 60);

		utc.gigayear = -13;
		utc.year = bbYear;
		utc.month = 12;

		if (days == 0 && hours < 12)
		{
			// First day
			utc.dayOfMonth = 25;
			utc.hour = 12 + hours;
			utc.minute = minutes;
			utc.second = seconds;
		}
		else
		{
			// After the first day
			//	Shift by 12 hours then recalculate
			days = (getSeconds(r1) - secondsPerDay/2) / secondsPerDay;
			remSeconds = getSeconds(r1) - secondsPerDay/2 - days*secondsPerDay;
			hours = (uint8_t)(remSeconds / 3600);
			remSeconds = remSeconds % 3600;

			minutes = (uint8_t)(remSeconds / 60);
			seconds = (uint8_t)(remSeconds % 60);

			utc.dayOfMonth = 26 + (uint8_t)days;
			utc.hour = hours;
			utc.minute = minutes;
			utc.second = seconds;
		}
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}

	if ( (isGreaterOrEqualTicks(t1, Prehistoric) && isLessTicks(t1, Gregorian)) )
	{
		// Within the Julian period
		//	4 year cycles
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, Prehistoric);

		gigaYears = getGigaseconds(r1) / gigaSecsInJulianGigaYear;
		remTenMegaseconds = 100 * (getGigaseconds(r1) % gigaSecsInJulianGigaYear); // Value < 3,200,000,000

		tenMegaYears = remTenMegaseconds / gigaSecsInJulianGigaYear; // gigaSecsInJulianGigaYear is multiple of 100
		rem100Kseconds = 100 * (remTenMegaseconds - tenMegaYears * gigaSecsInJulianGigaYear);

		hundredKYears = rem100Kseconds / gigaSecsInJulianGigaYear;
		remKseconds = 100 * (rem100Kseconds - hundredKYears * gigaSecsInJulianGigaYear);

		kYears = remKseconds / gigaSecsInJulianGigaYear;
		rem20seconds = 50 * (remKseconds - kYears * gigaSecsInJulianGigaYear);

		twentyYears = rem20seconds / gigaSecsInJulianGigaYear; // Cannot use 10 years, since must be a multiple
															   //   of 4 to use gigaSecsInJulianGigaYear
		remSeconds = 20 * (rem20seconds - twentyYears * gigaSecsInJulianGigaYear) + getSeconds(r1);

		fourYears = remSeconds / secondsIn4JulianYears;
		remSeconds -= fourYears * secondsIn4JulianYears;

		years = 10000000*tenMegaYears + 100000*hundredKYears + 1000*kYears + 20*twentyYears + 4*fourYears;

		utc.gigayear = -13 + gigaYears;
		utc.year = bbYear + 1 + years;

		// Time remaining < 4 years.  4th year is a leap year.
		days = remSeconds / secondsPerDay;
		remSeconds -= days*secondsPerDay;
		utc.year += days/365;
		md = dayOfYearToMonthDay((days % 365) + 1, days >= 3*365); 
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);

		// Deal with PreHistoric period not beginning on an even billion years
		if (utc.gigayear < 0 && utc.year > 0)
		{
			// Increment utc.gigayear and decrement utc.year
			utc.gigayear ++;
			utc.year -= e9;
		}
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}

	if (isGreaterOrEqualTicks(t1, Gregorian) && isLessTicks(t1, Start1583))
	{
		// Start of Gregorian period.  Only includes the end of 1582
		utc.futureAdjust = 0;
		utc.gigayear = 0;
		utc.year = 1582;
		r1 = diffTicks(t1, Gregorian);  // < 78 days
		remSeconds = getSeconds(r1);
		days = remSeconds / secondsPerDay;
		remSeconds -= days*secondsPerDay;
		// 1582 is not a leap year
		dayOfYear = monthDayToDayOfYear(10, 15, 0);
		dayOfYear += days;
		md = dayOfYearToMonthDay(dayOfYear, 0);
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}

	if (isGreaterOrEqualTicks(t1, Start1583) && isLessTicks(t1, Start1600))
	{
		// Beginning of 1583 through the end of 1599
		utc.futureAdjust = 0;
		utc.gigayear = 0;
		r1 = diffTicks(t1, Start1583); // < 18 years
		remSeconds = getSeconds(r1);

		fourYears = remSeconds / secondsIn4JulianYears;
		remSeconds -= fourYears * secondsIn4JulianYears;
		utc.year = 1583 + 4*fourYears;
		days = remSeconds / secondsPerDay;

		// Less than 4 years remains
		//	The second year is a leap year
		if (days < 365)
		{
			md = dayOfYearToMonthDay(days + 1, 0); // 1583 is not a leap year
		}
		else
		{
			if (days < 731)
			{
				utc.year++;
				md = dayOfYearToMonthDay(days - 365 + 1, 1); // 1584 is a leap year
			}
			else
			{
				utc.year += 2 + (days - 731) / 365;
				md = dayOfYearToMonthDay((days - 731) % 365 + 1, 0);
			}
		}

		remSeconds -= days*secondsPerDay;
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}

	if (isGreaterOrEqualTicks(t1, Start1600) && isLessTicks(t1, EarlyAtomic))
	{
		// From the beginning of 1600 through the end of 1957
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, Start1600);

		// Total duration is under 400 years < 13e9 seconds
		rem10seconds = 100000000 * getGigaseconds(r1);
		days = rem10seconds / (secondsPerDay / 10);

		remSeconds = 10 * (rem10seconds - days * (secondsPerDay / 10)) + getSeconds(r1);
		days += remSeconds / secondsPerDay;

		// Determine the century
		if (days < daysInLeapCentury)
		{
			// In first century of the 400 year cycle
			numCenturies = 0;
			fourYears = days / 1461;
			years = 4*fourYears;
			days -= fourYears * 1461;
			// The first year of the remaining period is a leap year
			if (days < 366)
			{
				md = dayOfYearToMonthDay(days + 1, 1);
			}
			else
			{
				years += 1 + (days - 366) / 365;
				md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
			}
		}
		else
		{
            numCenturies = 0;
			if (days >= daysInLeapCentury && days < daysInLeapCentury + daysInNonLeapCentury)
			{
				// In second century of the 400 year cycle
				numCenturies = 1;
			}
			if (days >= daysInLeapCentury + daysInNonLeapCentury && days < daysInLeapCentury + 2*daysInNonLeapCentury)
			{
				// In third century of the 400 year cycle
				numCenturies = 2;
			}
			if (days >= daysInLeapCentury + 2*daysInNonLeapCentury && days < daysInLeapCentury + 3*daysInNonLeapCentury)
			{
				// In last century of the 400 year cycle
				numCenturies = 3;
			}
			years = 100*numCenturies;
			days -= daysInLeapCentury + (numCenturies - 1)*daysInNonLeapCentury;
			// We are in a non leap century, so first four years has no leap day
			if (days < 1460)
			{
				// In the first 4 years
				years += days/365;
				md = dayOfYearToMonthDay((days % 365) + 1, 0);
			}
			else
			{
				fourYears = 1 + (days - 1460) / 1461;
				years += 4*fourYears;
				days -= 1460 + (fourYears - 1)*1461;
				// The first year of the remaining period is a leap year
				if (days < 366)
				{
					md = dayOfYearToMonthDay(days + 1, 1);
				}
				else
				{
					years += 1 + (days - 366) / 365;
					md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
				}
			}
		}
		utc.gigayear = 0;
		utc.year = years + 1600;

		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		remSeconds -= (remSeconds / secondsPerDay) * secondsPerDay ;
		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}

	if (isGreaterOrEqualTicks(t1, EarlyAtomic) && isLessTicks(t1, EarlyUTCAdj))
	{
		// From the beginning of 1958 through the end of 1960
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1Adj = diffTicks(t1, EarlyAtomic);

		// Remove the adjustment
		r1 = unAdjustRelTicks(r1Adj, 0);

		remSeconds = getSeconds(r1); // Lesss than 1e8 seconds
		days = remSeconds / secondsPerDay;

		// Less than 3 years remains
		//	The third year is a leap year
		utc.gigayear = 0;
		utc.year = 1958;

		if (days < 365)
		{
			md = dayOfYearToMonthDay(days + 1, 0); // 1958 is not a leap year
		}
		else
		{
			if (days < 730)
			{
				utc.year++;
				md = dayOfYearToMonthDay(days - 365 + 1, 0); // 1959 is not a leap year
			}
			else
			{
				utc.year += 2;
				md = dayOfYearToMonthDay(days - 730 + 1, 1); // 1960 is a leap year
			}
		}

		remSeconds -= days*secondsPerDay;
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600; // Drift already accounted for

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 

	}

	if (isGreaterOrEqualTicks(t1, EarlyUTCAdj) && isLessTicks(t1, EarlyUTC2Adj))
	{
		// From the beginning of 1961 through the end of 1967
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1Adj = diffTicks(t1, EarlyUTCAdj);

		// Remove the adjustment
		r1 = unAdjustRelTicks(r1Adj, 1);

		remSeconds = getSeconds(r1); // Lesss than 1e8 seconds
		days = remSeconds / secondsPerDay;

		// Less than 7 years remains
		//	The fourth year is a leap year
		utc.gigayear = 0;
		utc.year = 1961;

		if (days < 1095)
		{
			utc.year += days/365;
			md = dayOfYearToMonthDay( (days % 365)  + 1, 0); // 1961 - 1963 are not leap years
		}
		else
		{
			if (days >= 1095 && days < 1461)
			{
				utc.year = 1964;
				md = dayOfYearToMonthDay(days - 1095 + 1, 1); // 1964 is a leap year
			}
			else
			{
				utc.year = 1965 + (days - 1461) / 365;
				md = dayOfYearToMonthDay( (days - 1461) % 365 + 1, 0); // 1965 - 1967 ae notleap years
			}
		}

		remSeconds -= days*secondsPerDay;
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600; // Drift already accounted for

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 

	}

	if (isGreaterOrEqualTicks(t1, EarlyUTC2Adj) && isLessTicks(t1, LeapSecond))
	{
		// From the beginning of 1968 through the end of 1971
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1Adj = diffTicks(t1, EarlyUTC2Adj);

		// Remove the adjustment
		r1 = unAdjustRelTicks(r1Adj, 2);

		remSeconds = getSeconds(r1); // Lesss than 2.3e8 seconds
		days = remSeconds / secondsPerDay;

		// Less than 4 years remains
		//	The first year is a leap year
		utc.gigayear = 0;
		utc.year = 1968;

		if (days < 366)
		{
			md = dayOfYearToMonthDay(days + 1, 1); // 1968 is a leap year
		}
		else
		{
			utc.year = 1969 + (days - 366) / 365;
			md = dayOfYearToMonthDay( ((days - 366) % 365) + 1, 0);
		}

		remSeconds -= days*secondsPerDay;
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600; // Drift already accounted for

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 

	}

	if (isGreaterOrEqualTicks(t1, LeapSecond) && isLessTicks(t1, Start2000))
	{
		// Witin the early leap second period
		//	28 years duration
		utc.futureAdjust = 0; // No more leap seconds can be added during the period

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, LeapSecond);

		// Adjust by cumulative leap seconds since 1 Jan 1972
		//	Special handling for seconds that are leap seconds

		isLeapSec = 0;
		cumLeapSecs = cumLeapSecondsAdj(t1);
		if (cumLeapSecs < 0)
		{
			isLeapSec = 1;
			cumLeapSecs = -cumLeapSecs - 1;
		}
		remSeconds = getSeconds(r1) - cumLeapSecs; // Less than 1e9 seconds
		// Special handling for leap seconds
		//	Subtract a second then set seconds to 60
		if (isLeapSec) remSeconds--;

		fourYears = remSeconds / secondsIn4JulianYears;
		remSeconds -= fourYears * secondsIn4JulianYears;

		years = 4*fourYears;
		days = remSeconds / secondsPerDay;

		// Less than 4 years remains
		//	The first year is a leap year
		utc.gigayear = 0;
		utc.year = 1972 + years;

		if (days < 366)
		{
			md = dayOfYearToMonthDay(days + 1, 1); // 1972 is a leap year
		}
		else
		{
			utc.year += 1 + (days - 366) / 365;
			md = dayOfYearToMonthDay( ((days - 366) % 365) + 1, 0);
		}

		remSeconds -= days*secondsPerDay;

		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600; // Drift already accounted for

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		if (isLeapSec) utc.second = 60;
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 

	}

	if (isGreaterOrEqualTicks(t1, Start2000) && isLessTicks(t1, PostLeapSecond))
	{
		// Within the late leap second period
		//	400 years duration

		// Delay setting futureAdjust

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, Start2000);

		// Adjust by cumulative leap seconds since 1 Jan 2000
		//	Special handling for seconds that are leap seconds

		isLeapSec = 0;
		cumLeapSecs = cumLeapSecondsAdj(t1);
		if (cumLeapSecs < 0)
		{
			isLeapSec = 1;
			cumLeapSecs = -cumLeapSecs - 1;
		}
		cumLeapSecs -= cumLeapSecondsAdj(Start2000);

		// Compute the number of days, recognizing that number of seconds < 13e9

		// Number of gigaseconds < 13
		rem10seconds = (e9/10) * getGigaseconds(r1);
		days = rem10seconds / (secondsPerDay / 10);

		remSeconds = 10 * (rem10seconds - days * (secondsPerDay / 10)) + getSeconds(r1) - cumLeapSecs;
		// Special handling for leap seconds
		//	Subtract a second then set seconds to 60
		if (isLeapSec) remSeconds--;

		days += remSeconds / secondsPerDay;

		// Determine the century
		if (days < daysInLeapCentury)
		{
			// In first century of the 400 year cycle
			numCenturies = 0;
			fourYears = days / 1461;
			years = 4*fourYears;
			days -= fourYears * 1461;
			// The first year of the remaining period is a leap year
			if (days < 366)
			{
				md = dayOfYearToMonthDay(days + 1, 1);
			}
			else
			{
				years += 1 + (days - 366) / 365;
				md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
			}
		}
		else
		{
            numCenturies = 0;
			if (days >= daysInLeapCentury && days < daysInLeapCentury + daysInNonLeapCentury)
			{
				// In second century of the 400 year cycle
				numCenturies = 1;
			}
			if (days >= daysInLeapCentury + daysInNonLeapCentury && days < daysInLeapCentury + 2*daysInNonLeapCentury)
			{
				// In third century of the 400 year cycle
				numCenturies = 2;
			}
			if (days >= daysInLeapCentury + 2*daysInNonLeapCentury && days < daysInLeapCentury + 3*daysInNonLeapCentury)
			{
				// In last century of the 400 year cycle
				numCenturies = 3;
			}
			years = 100*numCenturies;
			days -= daysInLeapCentury + (numCenturies - 1)*daysInNonLeapCentury;
			// We are in a non leap century, so first four years has no leap day
			if (days < 1460)
			{
				// In the first 4 years
				years += days/365;
				md = dayOfYearToMonthDay((days % 365) + 1, 0);
			}
			else
			{
				fourYears = 1 + (days - 1460) / 1461;
				years += 4*fourYears;
				days -= 1460 + (fourYears - 1)*1461;
				// The first year of the remaining period is a leap year
				if (days < 366)
				{
					md = dayOfYearToMonthDay(days + 1, 1);
				}
				else
				{
					years += 1 + (days - 366) / 365;
					md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
				}
			}
		}
		utc.gigayear = 0;
		utc.year = years + 2000;
		
		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		remSeconds -= (remSeconds / secondsPerDay) * secondsPerDay ;
		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		if (isLeapSec) utc.second = 60;
		// Set futureAdjust
		utc.futureAdjust = futureAdjust;
		// Check for error.  If cumLeapSecs = numLeapSeconds, additional leap seconds could
		//	be added before this date.
		if ((cumLeapSecs == numLeapSeconds - cumLeapSecondsAdj(Start2000)) &&
			(futureAdjust == 0)) utc.taiInit |= InvalidFutureAdjust;
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute,
			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust);
	}

	if (isGreaterOrEqualTicks(t1, PostLeapSecond))
	{
		// Within the post leap second period
		//	400 year cycles
		utc.futureAdjust = 0;

		// Calculate relative ticks from the start of the period
		r1 = diffTicks(t1, PostLeapSecond);

		gigaYears = getGigaseconds(r1) / gigaSecsInGregorianGigaYear;
		remTenMegaseconds = 100 * (getGigaseconds(r1) % gigaSecsInGregorianGigaYear); // Value < 3,200,000,000

		tenMegaYears = remTenMegaseconds / gigaSecsInGregorianGigaYear; // gigaSecsInGregorianGigaYear is multiple of 100
		rem100Kseconds = 100 * (remTenMegaseconds - tenMegaYears * gigaSecsInGregorianGigaYear);

		hundredKYears = rem100Kseconds / gigaSecsInGregorianGigaYear;
		rem4Kseconds = 25 * (rem100Kseconds - hundredKYears * gigaSecsInGregorianGigaYear);

		fourKYears = rem4Kseconds / gigaSecsInGregorianGigaYear; // Cannot use 1000 years, since must be a multiple
															//   of 400 to use gigaSecsInGregorianYear
		rem400seconds = 10 * (rem4Kseconds - fourKYears * gigaSecsInGregorianGigaYear);

		fourHundredYears = rem400seconds / gigaSecsInGregorianGigaYear;
		years = 10000000*tenMegaYears + 100000*hundredKYears + 4000*fourKYears + 400*fourHundredYears;

		// At this point, less than 400 years = 13e9 seconds remains
		rem10seconds = 40 * (rem400seconds - fourHundredYears * gigaSecsInGregorianGigaYear); // < 1.3e9
		days = rem10seconds / (secondsPerDay / 10);

		remSeconds = 10 * (rem10seconds - days * (secondsPerDay / 10)) + getSeconds(r1);
		days += remSeconds / secondsPerDay;

		// Determine the century
		if (days < daysInLeapCentury)
		{
			// In first century of the 400 year cycle
			numCenturies = 0;
			fourYears = days / 1461;
			years += 4*fourYears;
			days -= fourYears * 1461;
			// The first year of the remaining period is a leap year
			if (days < 366)
			{
				md = dayOfYearToMonthDay(days + 1, 1);
			}
			else
			{
				years += 1 + (days - 366) / 365;
				md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
			}
		}
		else
		{
            numCenturies = 0;
			if (days >= daysInLeapCentury && days < daysInLeapCentury + daysInNonLeapCentury)
			{
				// In second century of the 400 year cycle
				numCenturies = 1;
			}
			if (days >= daysInLeapCentury + daysInNonLeapCentury && days < daysInLeapCentury + 2*daysInNonLeapCentury)
			{
				// In third century of the 400 year cycle
				numCenturies = 2;
			}
			if (days >= daysInLeapCentury + 2*daysInNonLeapCentury && days < daysInLeapCentury + 3*daysInNonLeapCentury)
			{
				// In last century of the 400 year cycle
				numCenturies = 3;
			}
			years += 100*numCenturies;
			days -= daysInLeapCentury + (numCenturies - 1)*daysInNonLeapCentury;
			// We are in a non leap century, so first four years has no leap day
			if (days < 1460)
			{
				// In the first 4 years
				years += days/365;
				md = dayOfYearToMonthDay((days % 365) + 1, 0);
			}
			else
			{
				fourYears = 1 + (days - 1460) / 1461;
				years += 4*fourYears;
				days -= 1460 + (fourYears - 1)*1461;
				// The first year of the remaining period is a leap year
				if (days < 366)
				{
					md = dayOfYearToMonthDay(days + 1, 1);
				}
				else
				{
					years += 1 + (days - 366) / 365;
					md = dayOfYearToMonthDay((days - 366) % 365 + 1, 0);
				}
			}
		}
		utc.gigayear = gigaYears;
		utc.year = years + 2400;
		// Carry, if necessary
		if (utc.year >= e9)
		{
			utc.gigayear++;
			utc.year -= e9;
		}

		utc.month = (uint8_t)md[0];
		utc.dayOfMonth = (uint8_t)md[1];

		remSeconds -= (remSeconds / secondsPerDay) * secondsPerDay ;
		utc.hour = (uint8_t)(remSeconds / 3600);
		remSeconds = remSeconds % 3600;

		utc.minute = (uint8_t)(remSeconds / 60);
		utc.second = (uint8_t)(remSeconds % 60);
		return adjustUTCDatetimeElements(utc.gigayear, utc.year, utc.month, utc.dayOfMonth, utc.hour, utc.minute, 
 			utc.second, getNanosecond(t1), getAttosecond(t1), utc.precision, utc.uncertainty, utc.futureAdjust); 
	}
	// Should never get here
	return utc;
}

//uint8_t dayOfTheWeek(int8_t gigayear, int32_t year, uint8_t month, uint8_t dayOfMonth, uint32_t calendar)
//{
//	// Calculate the day of the week, where 0 = Sunday, for a local or UTC date.
//	//	Return an initializaiton bit field incremented by 8 if the date is invalid.
//	/* Use an algorithm based only on these elements.  An alternative, which is to create a
//		UTCDatetime, then calculate the number of days since the Big Bang would be
//		complicated when applied to local datetimes as, for example, part of Thursday UTC is
//		Wednesday EST. */
//
//	// Perfomr error checking.  Need to allow dates from Dec 24 13,826,151,189 BCE through
//	//	1 Jan 100,000,000,000 to allow for local dates
//
//	/* The potential errors involve the first 6 bits of the flag.
//		Since the valid return values are 0 - 6, by adding 8 to init,
//		and returning init, we can identify the errors. */
//
//	int borrow, dayOfWeekJan1, nLeapYears, yearsIntoCycle;
//	uint32_t remYears, fullGigaYears;
//	DateCoords date1, date2;
//
//	// Estalish reference points
//	int dayOfWeekCreation = 4; // 25 Dec 13,826,151,189 BCE is a Thursday
//	int dayOfWeekPrehistoric = 4; // 1 Jan 13,826,151,188 BCE is a Thursday
//	int dayOfWeekGregorian = 5; // 15 Oct 1582 is a Friday 
//	int dayOfWeek1583 = 6; // 1 Jan 1583 is a Saturday 
//	int dayOfWeek1600 = 6; // 1 Jan 1600 is a Saturday 
//
//	// Check datetime elements
//	uint16_t init = checkDatetimeElements(gigayear, year, month, dayOfMonth, 0, 0,
//						   0, 0, 0, 99, 0, calendar, 0);
//
//	// Expand the valid date range by one day on either side to support local dates
//	if (gigayear == 100 && year == 0 && month == 1 && dayOfMonth == 1) 
//					init &= !TimeGtMax; // 1 Jan 100,000,000,000 is a valid local date
//	if (gigayear == -13 && year == bbYear && month == 12 && dayOfMonth == 24)
//					init &= !NegativeTime; // 24 Dec 13,826,151,189 BCE is a valid local date
//
//	// If any of the above tests have failed, return
//	if (init != 0)
//	{
//		return 8 + (uint8_t)init;
//	}
//	// Convert to a UTC Gregorian date, if necessary
//	if (calendar != 0)
//	{
//		// Create a DateCoords with the year, month and dayOfMonth
//		date1.gigayear = gigayear;
//		date1.year = year;
//		date1.month = month;
//		date1.dayOfMonth = dayOfMonth;
//		date1.calendar = calendar;
//		date1.dateInit = 0;
//		// Convert to a UTC Gregorian
//		date2 = convertToUTCGregorian(date1);
//		gigayear = date2.gigayear;
//		year = date2.year;
//		month = date2.month;
//		dayOfMonth = date2.dayOfMonth;
//	}
//	//  Detemine the period
//	if (gigayear == -13 && year == bbYear)
//	{
//		// Within the Creation period
//		//	The 25th is a Thursday
//		return (dayOfMonth - 25 + dayOfWeekCreation) % 7;
//	}
//	if  ((gigayear < 0) || 
//		(gigayear == 0 && year < 1582) || // Before 1582
//		(gigayear == 0 && year == 1582 && monthDayToDayOfYear(month, dayOfMonth, 0) <= monthDayToDayOfYear(10, 4, 0)) || // Before change to Gregorian calendar
//		(gigayear == 0 && year < 1923 && calendar == 1) ) // Latest usage of Julian calendar
//	{
//		/*  In the determination of day of the week, we observe there are 14 types of years:
//			Non-leap years beginning on Sunday, Monday,...Saturday, respecitvely and
//			leap years beginning on Sunday, Monday,...Saturday, respectively.  During the
//			Julian period, leap years appear in a regular, 4 year cycle.  As such, the
//			types of year from a day of the week, perspective repeat in a 28 year cycle.
//			E.g., If 1 Jan in year x is a Thursday, then 1 Jan in year 28 + x is also a
//			Thursday.*/
//		fullGigaYears = gigayear + 13;
//		borrow = 0;
//		if (year <= bbYear) // Period starts 1 Jan -13,826,151,187.
//		{						//   To be a full billion years, must be
//				fullGigaYears --;	// at least within -12,826,151,187.
//				borrow = e9;
//		}
//		//				    # of 10 year periods < 1.3e9
//		remYears = 10 * (((e9/10) * fullGigaYears) % 28); // < 280 years
//		remYears = (remYears + year + borrow - bbYear - 1) % 28; // < 28 years
//		/* The first year in a 28 year cycle begins on a Thursday.  The fourth
//			year in the cycle is the first leap year. 
//			
//			If 1 Jan in the first year (year 0) of the cycle is the ith day of the week,
//			 1 Jan in year n is i + [n + (number of leap years through year n - 1)] % 7.
//			 Number of leap years through year n - 1 is (n - 1) / 4   */
//
//		dayOfWeekJan1 =  (dayOfWeekPrehistoric + remYears + (remYears / 4)) % 7;
//		return (monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar)) - 1 + dayOfWeekJan1) % 7;
//	}
//	if (gigayear == 0 && year == 1582)
//	{
//		// At the end of 1582
//		return (dayOfWeekGregorian + monthDayToDayOfYear(month, dayOfMonth,0) - monthDayToDayOfYear(10,15,0)) % 7;
//	}
//	if (gigayear == 0 && year > 1582 && year < 1600)
//	{
//		// At the end of the 16th century - 2nd year is a leap year
//		remYears = year - 1583;
//		dayOfWeekJan1 =  (dayOfWeek1583 + remYears + ( (remYears + 2) / 4)) % 7;
//		return (monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar)) - 1 + dayOfWeekJan1) % 7;
//	}
//	if (gigayear > 0 || (gigayear == 0 && year >= 1600)) 
//	{
//		// The years in this period have 400 year cycles, so the day of the week of 1 Jan
//		//	has a 2800 year cycle.
//		fullGigaYears = gigayear;
//		borrow = 0;
//		if (fullGigaYears > 0 && year < 1600) // Period starts 1 Jan 1600.
//		{						//   To be a full billion years, must be
//				fullGigaYears --;	// at least after 1,000,001,600.
//				borrow = e9;
//		}
//		//				    # of 100 year periods < 1.3e8
//		remYears = 100 * (((e9/100) * fullGigaYears) % 28); // < 2800 years
//		remYears = (remYears + year + borrow - 1600) % 2800; // < 2800 years
//		yearsIntoCycle = remYears;
//		/* The first year in a 2800 year cycle begins on the same day as 1 Jan 1600.
//			The first year in the cycle is the first leap year on centuries that
//			are multiples of 400.  In other centuries, the fifth year in the cycle is
//			the first leap year.
//			
//			If 1 Jan in the first year (year 0) of the cycle is the ith day of the week,
//			 1 Jan in year n is i + [n + (number of leap years through year n - 1)] % 7.
//
//			 In order to compute the number of leap years through a given year, observe:
//			 - There are 97 leap years in every 400 year period
//			 - Centuries beginning with years that are not a multiple of 400 have 24 leap years
//			 - Centuries beginning with years that are multiples of 400 have 25 leap years */
//		
//		nLeapYears = (remYears / 400) * 97;
//		remYears = remYears % 400;
//
//		nLeapYears += (remYears / 100) * 24;
//		if (remYears >= 100) nLeapYears++;
//
//		remYears = remYears % 100;
//		nLeapYears += (remYears + 3) / 4; // First year in the cycle is a leap year
//
//		/* If the century preceding the date is not a multiple of 400,
//			the first four year period does not have a leap year.
//			Be sure not to subtract a leap year unless one was added
//			in the previous calculation. */
//		if ( ((year / 100) % 4 != 0 ) && (remYears >= 1) ) nLeapYears--;
//
//		dayOfWeekJan1 =  (dayOfWeek1600 + yearsIntoCycle + nLeapYears) % 7;
//		return (monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar)) - 1 + dayOfWeekJan1) % 7;
//	}
//	// Can't get here
//	return 0;
//}

uint8_t dayOfTheWeek(DateCoords date)
{
	// Calculate the day of the week, where 0 = Sunday, for a local or UTC date.

	/*  This function does not assume date is a valid date
	  	Return an initializaiton bit field incremented by 8 if the date is invalid.
	    The potential errors involve the first 6 bits of the flag.
		Since the valid return values are 0 - 6, by adding 8 to init,
		and returning init, we can identify the errors. */

	/* Use an algorithm based only on these elements.  An alternative, which is to create a
	UTCDatetime, then calculate the number of days since the Big Bang would be
	complicated when applied to local datetimes as, for example, part of Thursday UTC is
	Wednesday EST. */

	uint16_t init;
	int8_t gigayear;
	int32_t year;
	uint32_t borrow, remYears;
	uint8_t month, dayOfMonth, fullGigaYears, dayOfWeekJan1;
	DateCoords date2;

	// Estalish reference points
	int dayOfWeekCreation = 4; // 25 Dec 13,826,151,189 BCE is a Thursday
	int dayOfWeekPrehistoric = 4; // 1 Jan 13,826,151,188 BCE is a Thursday
	int dayOfWeekYearZero = 4; // 1 Jan 1 BCE is a Thursday

	// Check DateCoords elements
	init = checkDateCoordsElements(date.gigayear, date.year, date.month, date.dayOfMonth, date.calendar);
	if (init != 0)
	{
		return 8 + init;
	}

	// Convert to a Julian date.  Julian dates are used to simplify the algorithm
	//	as there are no skipped days.
	date2 = date;
	if (date.calendar != 1) date2 = convertToJulian(date);

	gigayear = date2.gigayear;
	year = date2.year;
	month = date2.month;
	dayOfMonth = date2.dayOfMonth;
	
	//  Detemine the period
	if (gigayear == -13 && year == bbYear)
	{
		// Within the Creation period
		//	The 25th is a Thursday
		return (dayOfMonth - 25 + dayOfWeekCreation) % 7;
	}
	/*  In the determination of day of the week, we observe there are 14 types of years:
	Non-leap years beginning on Sunday, Monday,...Saturday, respecitvely and
	leap years beginning on Sunday, Monday,...Saturday, respectively.  On the
	Julian calendar, leap years appear in a regular, 4 year cycle.  As such, the
	types of year from a day of the week, perspective repeat in a 28 year cycle.
	E.g., If 1 Jan in year x is a Thursday, then 1 Jan in year 28 + x is also a
	Thursday.*/

	// Handle negative and positive years (including 0), separately, for simplicity
	if (gigayear < 0 || (gigayear == 0 && year < 0))
	{
		fullGigaYears = gigayear + 13;
		borrow = 0;
		if (year <= bbYear) // Period starts 1 Jan -13,826,151,187.
		{						//   To be a full billion years, must be
			fullGigaYears--;	// at least within -12,826,151,187.
			borrow = e9;
		}
		//  Can't perform the calculation in terms of years within a uint_32, so
		//	  perform in terms of 10 year periods
		//	Compute the number of 10 year periods represented by the full gigayears,
		//	  then take mod 28 to
		//	  get the number of remaining 10 year periods.  By taking mod 28, we
		//	  are using a 280 year cycle.  Multiplying this number by 10 yields
		//	  the number of remaining years.
		remYears = 10 * (((e9 / 10) * fullGigaYears) % 28); // < 280 years
		remYears = (remYears + year + borrow - bbYear - 1) % 28; // < 28 years
		 /* The first year in a 28 year cycle begins on a Thursday.  The fourth
		 year in the cycle is the first leap year.
		 If 1 Jan in the first year (year 0) of the cycle is the ith day of the week,
		 1 Jan in year n is i + [n + (number of leap years through year n - 1)] % 7.
		 Number of leap years through year n - 1 is (n - 1) / 4   */

		dayOfWeekJan1 = (dayOfWeekPrehistoric + remYears + remYears / 4) % 7;
	}
	else
	{
		// Years are positive.  Gigayears <= 100, so can't use 10 year periods.
		//	Use 100 year periods.
		fullGigaYears = gigayear;  // No need to factor in borrow, as first year is year 0
		remYears = 100 * (((e9 / 100) * fullGigaYears) % 28); // < 2800 years
		remYears = (remYears + year) % 28; // < 28 years
   	    /* The first year in a 28 year cycle begins on a Thursday.  The first
		 year in the cycle is the first leap year.
		 If 1 Jan in the first year (year 0) of the cycle is the ith day of the week,
		 1 Jan in year n is i + [n + (number of leap years through year n - 1)] % 7.
		 Number of leap years through year (n - 1) is (n + 3) / 4   */

		dayOfWeekJan1 = (dayOfWeekYearZero + remYears + (remYears + 3) / 4) % 7;
	}
	//
	// Use monthDayToDayOfYear to count the days
	//
	return (monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, 1))
		- 1 + dayOfWeekJan1) % 7;
}

uint8_t dayOfMonthFromWeekdayRule(int8_t gigayear, int32_t year, uint8_t month, uint8_t onOrAfterDayOfMonth,
	uint8_t weekNumber, uint8_t dayOfWeek, uint32_t calendar)
{
	// Find the day of the month from a weekday rule for a given gigayear, year and month

	// Returns 0 for any error condition

	/* calendar can have the following values:
		0 - Julian calendar transitioning to Gregorian calendar on earliest possible transition date,
				which is from 10/4/1582 Julian to 10/15/1582 Gregorian
		1 - Julian calendar
		2 - Swedish calendar
		>1 && < e9 - Julian calendar transitioning to Gregorian calendar on the date specified by the value
				of calendar.
			 The transition date is the last Julian date and is encoded in the form yyyymmdd.
			 For example, for a transition on 3/16/1642, would have a calendar value of
			 16420316.
		e9 - Gregorian calendar after a translation from wall to universal time results
			 in an invalid day - i.e., 10/14/1582
		2*e9 - Gregorian calendar after a translation from wall to universal time results
			 in an invalid day - i.e., 10/5/1582
		2 + e9 - Swedish calendar after a translation from wall to univeral time results
			 in an invalid day - i.e., 2/28/1753
		2 + 2*e9 - Swedish calendar after a translation from wall to univeral time results
			 in an invalid day - i.e., 2/18/1753
		> 2 + e9 && < 2*e9 - Julian calendar transitioning to Gregorian calendar on the date 
			specified by the value of calendar, where a translation from wall to universal
			time resulted in the invalid day that is the before the first Gregorian date
		> 2 + 2*e9 - Julian calendar transitioning to Gregorian calendar on the date
			specified by the value of calendar, where a translation from wall to universal
			time resulted in the invalid day that is the day after the last Julian date
			*/

	/*  Rule is defined as follows:

		Find the day of the month that corresponds to the weekNumber dayOfWeek on or after onOrAfterDayOfMonth.

		For example, for a rule that says find the second Thursday of the month or or after the 7th of the month,
		the inputs would be:
			onOrAfterDayOfMonth - 7
			weekNumber - 2
			dayOfWeek - 4

		dayOfWeek is based on 0 = Sunday
		weekNumber == 6 indicates Last
		weekNumbers from 1 to 4 are valid for all days of the week for all months, while weekNumber == 5 is only
		valid for certain days of the week.
	 */
	uint8_t daysInMonth, lastDay, firstDay;
	int daysBack;
	DateCoords y1, y2;
	int i, numValidDays, daysLater, returnDayInd, offsetIndex;
	int validDays[31];

	// Validate inputs

	//	Set dayOfMonth to 26 when calling checkDateTimeElements to avoid false failure during
	//		December of the creation year.
	uint16_t init = checkDateCoordsElements(gigayear, year, month, 26, calendar);
	//	Jan 1 in year 100,000,000,000 is a valid local datetime so correct if this flagged an error
	if (gigayear == 100 && year == 0 && month == 1 && onOrAfterDayOfMonth == 1 && weekNumber == 1)
		init &= !TimeGtMax; // 1 Jan 100,000,000,000 is a valid local date

	if (init != 0) return 0;

	//  The transition from Julian to Gregorian can affect a month in 3 ways:
	//  1. The transition can begin in the previous month and extend into the month,
	//		creating invalid days in the beginning of the month
	//  2. The transition can begin and end in the month, creating invalid days
	//		in the middle of the month
	//  3. The transition can begin in the month and extend into the next month,
	//		creating invalid days at the end of the month

	// Create an array containing the valid days of the month

	daysInMonth = 31;
	// April, June, September and November only have 30 days
	if (month == 4 || month == 6 || month == 9 || month == 11) daysInMonth = 30;
	// February 
	if (month == 2) daysInMonth = 28 + numLeapDays(gigayear, year, calendar);

	// If the calendar is Julian or we are outside the transition period,
	//	all days are valid
	//
	if (calendar == 1 || gigayear != 0 || year < EarliestTransitionDate.year ||
		year > LatestTransitionDate.year)
	{
		// All days are valid
		numValidDays = daysInMonth;
		for (i = 0; i < daysInMonth; i++) validDays[i] = i + 1;
	}
	else
	{
		// Within the transition period, some days might not be valid
		numValidDays = 0;
		for (i = 1; i <= daysInMonth; i++)
		{
			if (!isSkipped(year, month, i, calendar))
			{
				// The ith day is a valid day
				validDays[numValidDays] = i;
				numValidDays++;
			}
		}
	}

	if (onOrAfterDayOfMonth > validDays[numValidDays-1]) return 0;
	if ((weekNumber < 1) || (weekNumber > 6)) return 0;
	if ((dayOfWeek < 0) || (dayOfWeek > 6)) return 0;

	// If in the creation month, only allow Last week specification, as time is undefined before the 25th
	//	day of the creation month
	if (gigayear == -13 && year == bbYear && month == 12 && weekNumber != 6)
		return 0;

	// First process Last week specification
	if (weekNumber == 6)
	{
		// Find the day of the week of the last day of the month
		// Create a DateCoords
		y1.gigayear = gigayear;
		y1.year = year;
		y1.month = month;
		y1.dayOfMonth = validDays[numValidDays - 1];
		y1.calendar = calendar;
		y1.dateInit = 0;
		lastDay = dayOfTheWeek(y1);

		// Go back the requisite number of days
		daysBack = lastDay - dayOfWeek;
		if (daysBack < 0) daysBack += 7;
		// Can't be after MaxTime based on previous checks, but could be before MinTime
		if (gigayear == -13 && year == bbYear && month == 12 && daysInMonth - daysBack < 24)
			return 0; // 24 Dec 13,826,151,189 BCE is the earliest valid local date

		// There is a theoretical case where there are so few valid days in the month,
		//	 that there is no last Wednesday, for example.
		//	 If, for example, the transition took place in the year 3999 at the end of
		//	 January, we would only have 1 or 2 valid days in February 
		//
		if (daysBack >= numValidDays) return 0;
		else return validDays[numValidDays - daysBack - 1];
	}

	// Find the day of the week of the day of the offset
	//  First find the index of the offset day
	for (i = 0; i < numValidDays; i++)
	{
		if (validDays[i] == onOrAfterDayOfMonth) break;
	}
	offsetIndex = i;

	// Create a DateCoords
	y2.gigayear = gigayear;
	y2.year = year;
	y2.month = month;
	y2.dayOfMonth = validDays[offsetIndex];
	y2.calendar = calendar;
	y2.dateInit = 0;
	firstDay = dayOfTheWeek(y2);

	daysLater = 7 * (weekNumber - 1) + dayOfWeek - firstDay;
	if (firstDay > dayOfWeek) daysLater += 7;

	// Check that we are still within range
	returnDayInd = offsetIndex + daysLater;
	if (returnDayInd < numValidDays)
	{
		// Check for after MaxTime or before MinTime
		if (gigayear == 100 && year == 0 && month == 1 && validDays[returnDayInd] > 1)
			return 0; // Only 1 Jan 100,000,000,000 is a valid local date
		if (gigayear == -13 && year == bbYear && month == 12 && validDays[returnDayInd] < 24)
			return 0; // 24 Dec 13,826,151,189 BCE is the earliest valid local date
		return validDays[returnDayInd];
	}
	else
	{
		return 0;
	}
	// Can't get here
	return 0;
}
// Propogate precision and uncertainty
PrecisionUncertainty  addPrecisionUncertainty(int8_t precision1, int8_t uncertainty1, int8_t isRelDatetime1,
												int8_t precision2, int8_t uncertainty2, int8_t isRelDatetime2)
{
	// Calculate the precision and uncertainty of the sum or difference when adding or subtracting
	//	datetimes and/or relative datetimes.
	//
	//	The resulting precision is the precision of the least precise term.
	//	The resulting uncertainty is the sum of the uncertainties.
	//
	//  UTCDatetimes have precisions ranging from maxPrecision to minUTCPrecision
	//	TAIRelDatetimes have precisions ranging from maxPrecision to minRelPrecision
	//	The UTCDatetimes from 4 to 15 are months, then powers of 10 of years
	//	The TAIRelDatetimes from 4 to 16 are powers of 10 of days
	//	As a result, a UTCDatetime with a precision of 4 through 15 is
	//	less precise than a TAIRelDatetime with a precison of 4 through 15
	//
	//	If both values are for UTCDatetimes, the resulting value is a TAIRelDatetime
	//	If one value is for a UTCDatetime and the other is for a TAIRelDatetime, the resulting
	//		value is a UTCDatetime
	//	If both values are for TAIRelDatetimes, the result is a TAIRelDatetime
	//
	int8_t precision, p1, p2, isRelResult;
	int un1, un2;
	double func1, func2, func;
	PrecisionUncertainty pu1;

	//  Establish whether the result is a UTCDatetime or a TAIRelDatetime
	//	A TAIRelDatetime is produced when both are UTCDatetimes or both are TAIRelDatetimes
	isRelResult = (isRelDatetime1 == isRelDatetime2);
	//
	//  Check whether precisions are specified
	if (precision1 != 99 && precision2 != 99)
	{
		// Both are specified, so choose the maximum
		//	Even though the precisions may be on a different basis, the values are
		//	still comparable
		precision = max(precision1, precision2);
	}
	// One or both are unspecified, so choose the minimum
	else precision = min(precision1, precision2);

	// The uncertainty is the sum of the uncertainties of the two datetimes

	if (precision == 99)
	{
		// The precision is unspecified, so set the uncertainty to 0
		pu1.precision = 99;
		pu1.uncertainty = 0;
	}
	else
	{
		// At least 1 datetime has a specified precision
		//	We can assume its uncertainty is nonzero based on the requirement for a call to adjustZeroUncertainty
		//		during the construction of the datetime
		p1 = precision1;
		p2 = precision2;
		// If a datetime has an unspecified precision, treat as having the maximum precision
		//	and minimum uncertainty
		if (precision1 == 99)
		{
			p1 = maxPrecision;
			un1 = -5;
		}
        else un1 = uncertainty1;
        
		if (precision2 == 99)
		{
			p2 = maxPrecision;
			un2 = -5;
		}
        else un2 = uncertainty2;
		{
			// The precisions of the terms may be different from each other and/or the result.
			// Sum the uncertainties by calculating on the common basis of seconds.
			//
			func1 = secondsUncertainty(un1, p1, isRelDatetime1);
			func2 = secondsUncertainty(un2, p2, isRelDatetime2);
			func = func1 + func2;
			//	Back out the precision from the uncertainty
			pu1 = precisionFromSecondsUncertainty(func, isRelResult);
		}
	}
	return pu1;
}

int8_t subSecondPrecision(double value, double dprecision)
{
	// Compute the TAIRelDatetime precision from a double value and
	//	associated absolute, as opposed to relative, precision
	double x, prec;
	x = log10(value / dprecision);
	prec = floor(x);
	// Increment prec, unless log10 is an exact integer
	if (prec != x) prec++;

	return max((int8_t)prec, maxPrecision);
}

// Datetime Coordiante comparison functions

int compareCalCoords (CalCoords calc1, CalCoords calc2)
{
	/* Compare the elements of 2 datetime coordinates.  
		Return 1 if calc1 > calc2
			   0 if calc1 < calc2
			  -1 if calc1 == calc2						
		Adopt this unconventional approach to return values
		to eliminate extra assignments. */

	// Compare date then time.
	//  Use the date comparison function that is calendar aware
	if (isEqualDateCoords(calc1.date, calc2.date))
	{
		// Go to the time
		if (isEqualTimeCoords(calc1.time, calc2.time))
		{
			// The values are equal
			return -1;
		}
		else
		{
			// The times are different
			return isGreaterTimeCoords(calc1.time, calc2.time);
		}
	}
	else
	{
		// The dates are different
		return isGreaterDateCoords(calc1.date, calc2.date);
	}
}

int isEqualCalCoords (CalCoords calc1, CalCoords calc2)
{
	// Use compare function to perform equality check
	return (compareCalCoords(calc1, calc2) == -1);
}
int isGreaterCalCoords (CalCoords calc1, CalCoords calc2)
{
	// Use compare function to perform greater than check
	return (compareCalCoords(calc1, calc2) == 1);
}
int isLessCalCoords (CalCoords calc1, CalCoords calc2)
{
	// Use compare function to perform less than check
	return (compareCalCoords(calc1, calc2) == 0);
}
int isNotEqualCalCoords (CalCoords calc1, CalCoords calc2)
{
	// Use equality function
	return !isEqualCalCoords (calc1, calc2);
}
int isGreaterOrEqualCalCoords (CalCoords calc1, CalCoords calc2)
{
	// Use less than function
	return !isLessCalCoords (calc1, calc2);
}
int isLessOrEqualCalCoords (CalCoords calc1, CalCoords calc2)

{
	// Use greater than function
	return !isGreaterCalCoords (calc1, calc2);
}

// Date Coordinate comparison functions
int compareDateCoords_NC(DateCoords date1, DateCoords date2)
{
	/* Compare the elements of 2 year month day coordinates.
	Return 1 if date1 > date2
	0 if date1 < date2
	-1 if date1 == date2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	//  Compare the elements in descending order
	if (date1.gigayear == date2.gigayear)
	{
		// Go to the next element
		if (date1.year == date2.year)
		{
			// Go to the next element
			if (date1.month == date2.month)
			{
				// Go to the next element
				if (date1.dayOfMonth == date2.dayOfMonth)
				{
					// The values are equal
					return -1;
				}
				else
				{
					// The day of month elements are different
					return (date1.dayOfMonth > date2.dayOfMonth);
				}
			}
			else
			{
				// The month elements are different
				return (date1.month > date2.month);
			}
		}
		else
		{
			// The year elements are different
			return (date1.year > date2.year);
		}
	}
	else
	{
		// The gigayear elements are different
		return (date1.gigayear > date2.gigayear);
	}
}

int isEqualDateCoords_NC(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform equality check
	return (compareDateCoords_NC(date1, date2) == -1);
}
int isGreaterDateCoords_NC(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform greater than check
	return (compareDateCoords_NC(date1, date2) == 1);
}
int isLessDateCoords_NC(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform less than check
	return (compareDateCoords_NC(date1, date2) == 0);
}
int isNotEqualDateCoords_NC(DateCoords date1, DateCoords date2)
{
	// Use equality function
	return !isEqualDateCoords_NC(date1, date2);
}
int isGreaterOrEqualDateCoords_NC(DateCoords date1, DateCoords date2)
{
	// Use less than function
	return !isLessDateCoords_NC(date1, date2);
}
int isLessOrEqualDateCoords_NC(DateCoords date1, DateCoords date2)

{
	// Use greater than function
	return !isGreaterDateCoords_NC(date1, date2);
}

int compareDateCoords(DateCoords date1, DateCoords date2)
{
	/* Compare the elements of 2 year month day coordinates.
	If the calendars are not the same, convert both DateCoords
	to UTC Gregorian, then compare.
	Return 1 if date1 > date2
	0 if date1 < date2
	-1 if date1 == date2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	DateCoords y1, y2;

	// Initialize the values to be compared
	y1 = date1;
	y2 = date2;

	//  Check whether the calendars are the same
	if (y1.calendar != y2.calendar)
	{
		// Convert both to UTC Gregorian, if necessary
		if (y1.calendar != 0) y1 = convertToUTCGregorian(date1);
		if (y2.calendar != 0) y2 = convertToUTCGregorian(date2);
	}

	//  Compare the elements in descending order
	if (y1.gigayear == y2.gigayear)
	{
		// Go to the next element
		if (y1.year == y2.year)
		{
			// Go to the next element
			if (y1.month == y2.month)
			{
				// Go to the next element
				if (y1.dayOfMonth == y2.dayOfMonth)
				{
					// The values are equal
					return -1;
				}
				else
				{
					// The day of month elements are different
					return (y1.dayOfMonth > y2.dayOfMonth);
				}
			}
			else
			{
				// The month elements are different
				return (y1.month > y2.month);
			}
		}
		else
		{
			// The year elements are different
			return (y1.year > y2.year);
		}
	}
	else
	{
		// The gigayear elements are different
		return (y1.gigayear > y2.gigayear);
	}
}

int isEqualDateCoords(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform equality check
	return (compareDateCoords(date1, date2) == -1);
}
int isGreaterDateCoords(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform greater than check
	return (compareDateCoords(date1, date2) == 1);
}
int isLessDateCoords(DateCoords date1, DateCoords date2)
{
	// Use compare function to perform less than check
	return (compareDateCoords(date1, date2) == 0);
}
int isNotEqualDateCoords(DateCoords date1, DateCoords date2)
{
	// Use equality function
	return !isEqualDateCoords(date1, date2);
}
int isGreaterOrEqualDateCoords(DateCoords date1, DateCoords date2)
{
	// Use less than function
	return !isLessDateCoords(date1, date2);
}
int isLessOrEqualDateCoords(DateCoords date1, DateCoords date2)

{
	// Use greater than function
	return !isGreaterDateCoords(date1, date2);
}

int compareTimeCoords(TimeCoords time1, TimeCoords time2)
{
	/* Compare the elements of 2 time coordinates.
	Return 1 if time1 > time2
	0 if time1 < time2
	-1 if time1 == time2
	Adopt this unconventional approach to return values
	to eliminate extra assignments. */

	TimeCoords y1, y2;

	// Initialize the values to be compared
	y1 = time1;
	y2 = time2;


	//  Compare the elements in descending order
	if (y1.hour == y2.hour)
	{
		// Go to the next element
		if (y1.minute == y2.minute)
		{
			// Go to the next element
			if (y1.second == y2.second)
			{
				// Go to the next element
				if (y1.nanosecond == y2.nanosecond)
				{
					// Go to the next element
					if (y1.attosecond == y2.attosecond)
					{
						// The values are equal
						return -1;
					}
					else
					{
						// The attosecond elements are different
						return (y1.attosecond > y2.attosecond);
					}
				}
				else
				{
					// The nanosecond elements are different
					return (y1.nanosecond > y2.nanosecond);
				}
			}
			else
			{
				// The second elements are different
				return (y1.second > y2.second);
			}
		}
		else
		{
			// The minute elements are different
			return (y1.minute > y2.minute);
		}
	}
	else
	{
		// The hour elements are different
		return (y1.hour > y2.hour);
	}
}

int isEqualTimeCoords(TimeCoords time1, TimeCoords time2)
{
	// Use compare function to perform equality check
	return (compareTimeCoords(time1, time2) == -1);
}
int isGreaterTimeCoords(TimeCoords time1, TimeCoords time2)
{
	// Use compare function to perform greater than check
	return (compareTimeCoords(time1, time2) == 1);
}
int isLessTimeCoords(TimeCoords time1, TimeCoords time2)
{
	// Use compare function to perform less than check
	return (compareTimeCoords(time1, time2) == 0);
}
int isNotEqualTimeCoords(TimeCoords time1, TimeCoords time2)
{
	// Use equality function
	return !isEqualTimeCoords(time1, time2);
}
int isGreaterOrEqualTimeCoords(TimeCoords time1, TimeCoords time2)
{
	// Use less than function
	return !isLessTimeCoords(time1, time2);
}
int isLessOrEqualTimeCoords(TimeCoords time1, TimeCoords time2)

{
	// Use greater than function
	return !isGreaterTimeCoords(time1, time2);
}

DateCoords skipGregorianDays(uint8_t numberOfDays, DateCoords date1)
{
	/*	  This function is a helper function to determine the invalid date range created
	by the transition from Julian to Gregorian calendar.  For example, in the
	first transition to the Gregorian calendar, 10 days were skipped after 10/4/1582.
	As a result, the dates 10/5/1582 - 10/14/1582 do not exist.  */

	/* This function supports the addition of a small number of days,
		generally 10 - 13, up to a maximum of 28 days,
		which is the number of days skipped in the
		transition from the Julian to Gregorian calendar depending on
		the date the transition took place.  As such, accommodation is only
		made for 1 month of carry.  The days are assummed to be added
		to a Gregorian date.  */

	// date1 is assumed to represent a well formed Gregorian data

	// Initialize the return value
	DateCoords date2 = date1;
	//	Check for valid calendar specification
	if (date1.calendar != 0)
	{
		date2.gigayear = 0;
		date2.year = 0;
		date2.month = 0;
		date2.dayOfMonth = 0;
		date2.calendar = 0;
		date2.dateInit |= InvalidCalendar;
	}

	// Add days, carrying if necessary.

	// Determine the number of days in the month.  All calculations performed on Gregorian
	//	calendar basis, since we are assumed to be skipping days on the Gregorian calendar
	//	to find the first Gregorian date.

	/* This approach only is problematic if the transition day is 2/29/1700 J, 2/29/1800 J or
		2/29/1900 J.  These years are not leap years on the Gregorian calendar, so the next
		calculation will determine these months only have 28 days.  While this would seem to
		be a problem, it is not, as when days are added (e.g., 10 days), the carry calculation
		will produce the desired result. (This is also true had there been a transition on
		2/30/1712 J Sweden.) */

	uint8_t daysInMonth = 31;
	// April, June, September and November only have 30 days
	if (date2.month == 4 || date2.month == 6 || date2.month == 9 || date2.month == 11) daysInMonth = 30;
	// February 
	if (date2.month == 2) daysInMonth = 28 + numLeapDays(0, date2.year, 0);
	date2.dayOfMonth += numberOfDays + 1;
	if (date2.dayOfMonth > daysInMonth)
	{
		// Carry, assuming can't move past next month
		date2.dayOfMonth -= daysInMonth;
		date2.month++;
		// Check for end of year
		if (date2.month > 12)
		{
			date2.month = 1;
			date2.year++;
		}
	}
	return date2;
}

DateCoords lastJulianDate(uint32_t calendar)
{
	// Extract last Julian date from a calendar
	//  Does not require a call to isLeapYear, so can be called from isLeapYear

	/*
	The transition from the Julian to Gregorian calendar is
	assumed to occur at midnight in local wall time.  As such,
	when a valid wall time is translated to standard time or
	universal time, the day may be incremented or decremented
	to a day that would be invalid in wall time.

	When such a translation occurs (in offsetCalCoords), the
	calendar is adjusted, by adding 1e9, in the case of an
	increment and 2e9, in the case of a decrement.  These
	adjustments either move the last Julian date forward
	a day or the first Gregorian date back a day.

	For calendar == 0, the last Julian date is 10/4/1582.
	For calendar == 1, there is no first Gregorian date, so no last Julian date
	For calendar == 2, the last Julian date is 2/17/1753.
	For all other values of calendar, the first Gregorian date is determined
	by parsing the value of calendar, which is yyyymmdd to determine the
	last Julian date.

	*/

	DateCoords date1;
	uint8_t daysInMonth;
	uint32_t modcal;

	// Extract last Julian date if necessary

	if (calendar == 1)
	{
		// Julian calendar, so no calculation to be performed
		date1.gigayear = 99;
		date1.year = 9999999;
		date1.month = 0;
		date1.dayOfMonth = 0;
		date1.calendar = calendar;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 0)
	{
		// Return earliest last Julian date
		date1.gigayear = 0;
		date1.year = 1582;
		date1.month = 10;
		date1.dayOfMonth = 4;
		date1.calendar = calendar;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == e9)
	{
		// Return 1 day past the earliest last Julian date
		date1.gigayear = 0;
		date1.year = 1582;
		date1.month = 10;
		date1.dayOfMonth = 5;
		date1.calendar = calendar;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 2)
	{
		// Return last Julian date for Sweden
		date1.gigayear = 0;
		date1.year = 1753;
		date1.month = 2;
		date1.dayOfMonth = 17;
		date1.calendar = calendar;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == e9 + 2)
	{
		// Return 1 day past the last Julian date for Sweden 
		date1.gigayear = 0;
		date1.year = 1753;
		date1.month = 2;
		date1.dayOfMonth = 18;
		date1.calendar = calendar;
		date1.dateInit = 0;
		return date1;
	}
	//	Check for value within range
	if ((calendar % e9 < MinCalendar) || (calendar % e9 > MaxCalendar)) // 28-Feb-4100 is the latest transition date
	{
		// Value out of range
		date1.gigayear = 0;
		date1.year = 0;
		date1.month = 0;
		date1.dayOfMonth = 0;
		date1.calendar = calendar;
		date1.dateInit |= InvalidCalendar;
		return date1;
	}
	else
	{
		// Extraction is necessary
		modcal = calendar % e9;
		date1.gigayear = 0;
		date1.year = modcal / 10000;
		date1.month = (modcal - 10000 * date1.year) / 100;
		date1.dayOfMonth = modcal - 10000 * date1.year - 100 * date1.month;
		date1.calendar = calendar;
		date1.dateInit = 0;
		// Go forward 1 day if calendar > e9, but < 2*e9
		if (calendar > e9 && calendar < 2*e9)
		{
			// Move forward 1 day using a full calendar
			daysInMonth = 31;
			// April, June, September and November only have 30 days
			if (date1.month == 4 || date1.month == 6 || date1.month == 9 || date1.month == 11) daysInMonth = 30;
			// February 
			if (date1.month == 2) daysInMonth = 28 + numLeapDays(date1.gigayear, date1.year, calendar);
			//
			date1.dayOfMonth += 1;
			if (date1.dayOfMonth > daysInMonth)
			{
				date1.month++;
				date1.dayOfMonth = 1;
				//
				if (date1.month > 12)
				{
					date1.year++;
					date1.month = 1;
					// No need to check year carry to gigayear
				}
			}
		}

		// date1 is the last Julian date.
		return date1;
	}
}

uint8_t numDaysToSkip(uint32_t calendar)
{
	// Find the number of days to skip in the transition from Julian to Gregorian calendar

	/*
	The transition from the Julian to Gregorian calendar is
	assumed to occur at midnight in local wall time.  As such,
	when a valid wall time is translated to standard time or
	universal time, the day may be incremented or decremented
	to a day that would be invalid in wall time.

	When such a translation occurs (in offsetCalCoords), the
	calendar is adjusted, by adding 1e9, in the case of an
	increment and 2e9, in the case of a decrement.  These
	adjustments either move the last Julian date forward
	a day or the first Gregorian date back a day, either
	of which reduces the number of days skipped by 1.

	For calendar == 1, there are no skipped days, as there is no transition
	For calendar == 0, there are 10 skipped days, from 10/5/1582 through 10/14/1582
	For calendar == 2, there are 11 skipped days, from 2/18/1753 through 2/28/1753
	For all other values of calendar, the number of days to skip is determined
	by parsing the value of calendar, which is yyyymmdd to determine the
	last Julian date.

	Starting on 10/4/1582 J, Julian Day 2,299,160, the number of days to skip is 10
	so the next day is 11 days later on the Gregorian calendar.
	We could increment the number of days to skip on either 2/29/1700 or 3/1/1700,
	as 2/29/1700 is not a valid date on the Gregorian calendar.  Because of the way
	the firstGregorianDate algorithm has been implemented, we will increment on 3/1/1700.
	Starting on 3/1/1700 J, Julian Day 2,342,044, the number of days to skip is 11.
	so the next day is 12 days later on the Greg2orian calendar.
	Starting on 3/1/1800 J, Julian Day 2,378,568, the number of days to skip is 12.
	so the next day is 13 days later on the Gregorian calendar.
	Starting on 3/1/1900 J, Julian Day 2,415,093, the number of days to skip is 13.
	so the next day is 14 days later on the Gregorian calendar.

	Turkey was the last country to change in 1927, however we allow last Julian dates
	up to 2/28/4100.  This limit is set to avoid moving ahead more than 1 month.

	*/

//	static const DateCoords skip10 = { 0, 1582, 10, 4, 0, 0 };
	static const DateCoords skip11 = { 0, 1700, 03, 01, 0, 0 };

	DateCoords date1;
	uint8_t nDaysToSkip, nCenturies;
	uint32_t nYears;

	// Extract last Julian date if necessary

	if (calendar == 1)
	{
		// Julian calendar, so no calculation to be performed
		return 0;
	}
	else if (calendar == 0)
	{
		// Return 10
		return 10;
	}
	else if (calendar == e9 || calendar == 2*e9)
	{
		// Calendar adjusted due to translation from wall time
		return 9;
	}
	else if (calendar == 2)
	{
		// Return 11
		return 11;
	}
	else if (calendar == 2 + e9 || calendar == 2 + 2*e9)
	{
		// Calendar adjusted due to translation from wall time
		return 10;
	}
	//	Check for value within range
	else if ((calendar % e9 < MinCalendar) || (calendar % e9 > MaxCalendar)) // 2/28/4100 is latest allowed transition date
	{
		// Value out of range
		return 0;
	}
	else
	{
		// Extraction is necessary.  Use the unmodified calendar.
		date1 = lastJulianDate(calendar % e9);

		// date1 is the last Julian date.
		//   If the last Julian date is before 3/1/1700, skip 10 days
		if (isLessDateCoords_NC(date1, skip11)) nDaysToSkip = 10;
		else
		{
			// Determine the number of full centuries that will have elapsed between 3/1/1600 and the last Julian date
			nDaysToSkip = 10;
			nYears = date1.year - 1600;
			if (date1.month < 3) nYears--;
			nCenturies = nYears / 100;
			// Every 4 centuries adds 3 skipped days.  Each of the first 3 centuries in a 4 century period adds another
			//	skipped day
			nDaysToSkip += 3 * (nCenturies / 4) + nCenturies % 4;
		}
		// Decrease by 1, if necessary
		if (calendar > e9) nDaysToSkip--;
	}
	return nDaysToSkip;
}

uint32_t numDaysInYear(int8_t gigayear, int32_t year, uint32_t calendar)
{
	// Compute the number of days in a year, considering potential skipped
	//	days due to the transition from Julian to Gregorian calendar
	DateCoords transStart;
	uint8_t nDaysSkipped;

	if (calendar == 1 || gigayear != 0 || year < EarliestTransitionDate.year ||
		year > LatestTransitionDate.year)
	{
		// Julian calendar or outside of the transition period, so no skipped days
		return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar));
	}

	// If there is a transition from a Julian to a Gregorian calendar, there are 3
	//	possible cases:
	//  1. The transition occurs entirely within the year
	//	2. The transition occurs at the end of the previous year and extends into the year
	//	3. The transition occurs at the end of the year and extends into the next year
	//
	//	Determine the date the transition begins
	transStart = lastJulianDate(calendar);
	if (transStart.year < year - 1 || transStart.year > year)
	{
		// Transition does not affect the year
		return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar));
	}
	else
	{
		// The transition could affect the year
		//  Determine the number of days skipped
		nDaysSkipped = numDaysToSkip(calendar);
		if (transStart.year == year - 1 && (transStart.month != 12 ||
			transStart.dayOfMonth + nDaysSkipped <= 31))
		{
			// Skipped days are in the previous year
			return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar));
		}
		else if (transStart.year == year - 1)
		{
			// Transition begins in previous year and carries over into this year
			return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar))
				- (firstGregorianDate(calendar).dayOfMonth - 1);
		}
		else if (transStart.year == year && (transStart.month != 12 ||
			transStart.dayOfMonth + nDaysSkipped <= 31))
		{
			// Transition occurs entirely during the year
			return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar))
				- nDaysSkipped;
		}
		else
		{
			// Transition beings during the year and ends the next year
			return monthDayToDayOfYear(12, 31, numLeapDays(gigayear, year, calendar))
				- (31 - lastJulianDate(calendar).dayOfMonth);
		}
	}

}

DateCoords julianToUTCGregorian(DateCoords dateJulian)
{
	// Convert a Julian date to
	//	a UTC Gregorian date, which is a Gregorian date consistent with the
	//	earliest switch to the Gregorian calendar
	//
	DateCoords dateGreg = dateJulian;
	uint8_t jMonth, jDay, nCenturies, n4Years;
	uint32_t numDays, n400Years, nMegaYears;
	int32_t jYear, gYear, nFullYears;
	int8_t jGiga, gGiga;
	uint8_t * md;

	// End of the Julian 16th Century as a Julian date
	static const DateCoords dateEnd16thCentury = { 0, 1599, 12, 31, 1, 0 };

	// Assume dateJulian is a Julian date, even if the calendar != 1

	// The converted value must fall within the range of DateCoords.
	//	As such, Julian year must be limited to MaxJulianDate
	//
	dateGreg.calendar = 0;
	if (isGreaterDateCoords_NC(dateJulian, MaxJulianDate))
	{
		// Invalid Julian date.  Return a range error.
		dateGreg.gigayear = 0;
		dateGreg.year = 0;
		dateGreg.month = 0;
		dateGreg.dayOfMonth = 0;
		dateGreg.dateInit |= InvalidYear;
		return dateGreg; 
	}

	// Until Oct 4, 1582, the Gregorian date == the Julian date
	//	After that date, a number of days must be added on the
	//	Gregorian calendar.
	//
	if (isLessOrEqualDateCoords_NC(dateJulian, EarliestTransitionDate)) return dateGreg;
	else if (isLessOrEqualDateCoords_NC(dateJulian, dateEnd16thCentury))
	{
		//  There is a 10 day difference.  Use the skip function
		return skipGregorianDays(9, dateGreg);
	}
	else
	{
		// After the end of the Julian 16th century
		// Determine the number of elapsed days since 12/31/1599 Julian, which
		//	corresponds to 1/10/1600 Gregorian.
		//
		jGiga = dateJulian.gigayear;
		jYear = dateJulian.year;
		jMonth = dateJulian.month;
		jDay = dateJulian.dayOfMonth;
		// 
		// Begin with gigayear periods
		//
		// A Julian gigayear is equivalent to a Gregorian gigayear plus 7,500,000 days
		//	The number of extra days will be <= 750,000,000, so extra days will fit into an int
		gGiga = jGiga;
		numDays = 0;
		if (gGiga > 0)  numDays += gGiga * 7500000;
		//
		//  Under e9 years remains, so can compute the number of years
		nFullYears = jYear - 1600;
		//  Handle the situation where jYear < 1600 - can occur when jGiga > 0
		if (nFullYears < 0)
		{
			// Borrow from jGiga
			nFullYears += e9;
			gGiga--;
			numDays -= 7500000;
		}
		if (jMonth == 12 && jDay == 31) nFullYears++;
		//
		//	As there are more than uint32_t days in e9 years, cannot just
		//	calculate in terms of days.  First, consider million year
		//	periods.
		//
		nMegaYears = nFullYears / 1000000;
		nFullYears -= 1000000 * nMegaYears;
		//  Every million Julian years adds 3 * 1000000 / 400 = 7500 Gregorian days
		//	Set numDays to this incremental number of days
		//
		numDays += 7500 * nMegaYears;
		//
		//	The remaining time is under 1,000,000 years + 7,500,000 days, so can be computed in terms of days
		//	  even considering up to a 750,000,000 day contribution from the gigayears
		//
		// Every 4 Julian years contains 1461 days
		numDays += 1461 * (nFullYears / 4);
		// Determine number of full years remaining
		nFullYears = nFullYears % 4;
		// The first Julian year in a 4 year period is always a leap year
		if (nFullYears > 0) numDays += 366 + (nFullYears - 1) * 365;
		// Determine the number of days into the year
		numDays += monthDayToDayOfYear(jMonth, jDay, numLeapDays(0, jYear, 1));
		//
		//	Add the days on the Gregorian calendar
		//
		//	 Add 10 days to numDays, since we are starting on 1/10/1600 on the Gregorian calendar
		numDays += 10;
		//
		//	 Determine the number of 400 year periods
		//
		n400Years = numDays / daysIn4GregorianCenturies;
		//	 Starting date is 12/31/1599
		gYear = 1599 + nMegaYears * 1000000 + n400Years * 400;
		numDays -= daysIn4GregorianCenturies * n400Years;
		//
		//	 Determine the number of centuries
		//	  First century is a leap century
		if (numDays < daysInLeapCentury) nCenturies = 0;
		else
		{
			nCenturies = 1;
			numDays -= daysInLeapCentury;
			if (numDays >= daysInNonLeapCentury)
			{
				nCenturies += numDays / daysInNonLeapCentury;
				numDays -= (nCenturies - 1) * daysInNonLeapCentury;
			}
			gYear += nCenturies * 100;
		}
		//
		//	  Determine the number of 4 year periods
		//
		//		The first 4 year period of the first century is a leap period.
		//		  The first periods of all other centuries are not.
		//
		n4Years = 0;
		if (nCenturies == 0 && numDays >= 1461)
		{
			n4Years = 1;
			numDays -= 1461;
		}
		else if (nCenturies != 0 && numDays >= 1460)
		{
			n4Years = 1;
			numDays -= 1460;
		}
		//		All remaining 4 year periods have 1461 days
		n4Years += numDays / 1461;
		if (n4Years > 0) numDays -= (n4Years - 1) * 1461;
		gYear += 4 * n4Years;
		//
		//	Determine the number of full years
		//
		nFullYears = 0;
		//	First year of a 4 year period is a leap year, unless it is the
		//	first year of a non leapa century
		if ((nCenturies == 0 || n4Years > 0) && numDays >= 366)
		{
			nFullYears = 1;
			numDays -= 366;
		}
		else if (numDays >= 365)
		{
			nFullYears = 1;
			numDays -= 365;
		}
		//	All remaining years have 365 days
		nFullYears += numDays / 365;
		if (nFullYears > 0) numDays -= (nFullYears - 1)  * 365;
		gYear += nFullYears;
		//
		// Only a part of a year remains
		//
		//	If 1 day is left, it is 1/1 of the next year
		//
		if (numDays == 0)
		{
			// Check for carry
			if (gYear >= e9)
			{
				gYear -= e9;
				gGiga++;
			}
			dateGreg.gigayear = gGiga;
			dateGreg.year = gYear;
			dateGreg.month = 12;
			dateGreg.dayOfMonth = 31;
			dateGreg.dateInit = 0;
			return dateGreg;
		}
		else
		{
			gYear++;
			// Check for carry
			if (gYear >= e9)
			{
				gYear -= e9;
				gGiga++;
			}
			dateGreg.gigayear = gGiga;
			md = dayOfYearToMonthDay(numDays, numLeapDays(0, gYear, 0));
			dateGreg.year = gYear;
			dateGreg.month = md[0];
			dateGreg.dayOfMonth = md[1];
			dateGreg.dateInit = 0;
			return dateGreg;
		}
	}
}

DateCoords uTCGregorianToJulian(DateCoords dateUTCGreg)
{
	// Convert a UTC Gregorian date to a Julian date.
	//
	DateCoords dateJulian = dateUTCGreg;
	int daysInMonth;
	uint8_t gMonth, gDay, nCenturies;
	uint32_t numDays, nMegaYears, n4Years;
	int32_t jYear, gYear, nFullYears;
	int8_t jGiga, gGiga;
	uint8_t * md;

	// End of the Julian 16th Century as a Gregorian date
	static const DateCoords dateEnd16thCentury = { 0, 1600, 1, 10, 0, 0 };

	// Check that dateUTCGreg is a Gregorian date
	if (dateUTCGreg.calendar != 0)
	{
		// Not a Julian date
		dateJulian.gigayear = 0;
		dateJulian.year = 0;
		dateJulian.month = 0;
		dateJulian.dayOfMonth = 0;
		dateJulian.dateInit |= InvalidCalendar;
		return dateJulian;
	}

	// Until Oct 4, 1582, the Gregorian date == the Julian date
	//	After that date, a number of days must be subtracted on the
	//	Julian calendar.
	//
	dateJulian.calendar = 1;
	if (isLessOrEqualDateCoords_NC(dateUTCGreg, EarliestTransitionDate)) return dateJulian;
	else if (isLessOrEqualDateCoords_NC(dateUTCGreg, dateEnd16thCentury))
	{
		//  There is a 10 day difference.  Subtract 10 days on the Julian calendar
		dateJulian.dayOfMonth -= 10;
		if (dateJulian.dayOfMonth <= 0)
		{
			dateJulian.month--;
			if (dateJulian.month == 0)
			{
				dateJulian.year--;
				dateJulian.month = 12;
				dateJulian.dayOfMonth += 31;
				return dateJulian;
			}
			else
			{
				if (dateJulian.month == 4 || dateJulian.month == 6 || dateJulian.month == 9 || dateJulian.month == 11)
					daysInMonth = 30;
				else if (dateJulian.month == 2) daysInMonth = 28 + numLeapDays(0, dateJulian.year, 1);
				else daysInMonth = 31;
				dateJulian.dayOfMonth += daysInMonth;
				return dateJulian;
			}
		}
		return dateJulian;
	}
	else
	{
		// After the end of the Julian 16th century
		// Determine the number of elapsed days since 1/10/1600 Gregorian, which
		//	corresponds to 12/31/1599 Julian.
		//
		gGiga = dateUTCGreg.gigayear;
		gYear = dateUTCGreg.year;
		gMonth = dateUTCGreg.month;
		gDay = dateUTCGreg.dayOfMonth;
		// 
		// Begin with gigayear periods
		//
		//  Every billion Gregorian years subtracts 3 * 1,000,000,000 / 400 = 7,500,000 Julian days
		//	  from the comparable number of Julian gigayears
		//	To avoid having to count backwards, decrement nGigaYears, increment nMegaYears and add 
		//	 3 Gregorian megayears worth of days.  3 megayears is selected, since this value is greater
		//	 than the maximum number of subtracted days, which is approximately 750,000,000.
		//	 Gregorian megayears are selected so that the correct value for the adjustment to
		//	 numDays based on megayears is calculated below.
		// This results in at most 5 megayears worth of days
		//	 which is still well within the range of a uint32_t
		//
		jGiga = gGiga;
		nMegaYears = 0;
		numDays = 0;
		if (gGiga > 0)
		{
			// 1 is subtracted from gGiga, since Gregorian megayears are being used
			numDays += daysIn4GregorianCenturies * 2500 * 3 - 7500000 * (gGiga - 1);
			nMegaYears = 997;
			jGiga--;
		}
		//
		//  Under e9 years remains, so can compute the number of years
		//
		nFullYears = gYear - 1600;
		if (gMonth == 1 && gDay < 10) nFullYears--;
		// Borrow from megayears, if necessary 
		if (nFullYears < 0)
		{
			// This can only occur when gigaYear > 0, e.g. year 1,000,001,500.
			//	As such, nMegayears == 997, so we can borrow 1
			nFullYears += 1000000;
			nMegaYears--;
		}
		//
		//	As there are more than uint32_t days in e9 years, begin with million year
		//	periods.
		//
		nMegaYears += nFullYears / 1000000;
		//  Check for carry
		if (nMegaYears > 1000)
		{
			nMegaYears -= 1000;
			jGiga++;
			numDays -= 7500000;
		}
		nFullYears = nFullYears % 1000000;
		//  Every million Gregorian years subtracts 3 * 1000000 / 400 = 7500 Julian days
		//	  from the comparable number of Julian megayears
		//	To avoid having to count backwards, decrement nMegaYears and add a
		//	 Gregorian megayear worth of days.  This results in at most 2 megayears worth of days
		//	 which is still well within the range of a uint32_t
		//
		if (nMegaYears > 0)
		{
			// 1 is subtracted from nMegaYears, since Gregorian centuries are being used
			numDays += daysIn4GregorianCenturies  * 2500 - 7500 * (nMegaYears - 1);
			nMegaYears--;
		}
		//
		//	The remaining time is under 1,000,000 years - 7,500,000 days, so can be computed in terms of days
		//
		// Every 400 Gregorian years contains 146097 days
		numDays += daysIn4GregorianCenturies * (nFullYears / 400);
		// Determine number of full years remaining
		nFullYears = nFullYears % 400;
		// Consider the remaining centuries
		nCenturies = 0;
		if (nFullYears >= 100)
		{
			// There is at least 1 century, the first of which is a leap century
			nCenturies = 1;
			numDays += daysInLeapCentury;
			nFullYears -= 100;
			nCenturies += nFullYears / 100;
			numDays += daysInNonLeapCentury * (nCenturies - 1);
			nFullYears = nFullYears % 100;
		}
		// Less than a century remains
		//	Determine the number of 4 year periods
		n4Years = nFullYears / 4;
		nFullYears = nFullYears % 4;
		if (n4Years > 0)
		{
			// Every 4 year period, except those containing a century that is not a multiple of 4
			//	contains 1461 days
			numDays += 1461 * n4Years;
			// The first century of each 4 century period is a multiple of 4
			if (nCenturies > 0) numDays--;
		}
		// Less than 4 years remains
		//	The first year is a leap year, unless it is the first year of a century that is not a multiple of 4
		if (nFullYears > 0)
		{
			numDays += 366;
			if (nCenturies > 0 && n4Years == 0) numDays--;
			nFullYears--;
			numDays += nFullYears * 365;
		}
		// Less than a year remains
		if (gMonth == 1 && gDay < 10) numDays += gDay +
			monthDayToDayOfYear(12, 31, numLeapDays(0, gYear - 1, 0)) - 10;
		else numDays += monthDayToDayOfYear(gMonth, gDay, numLeapDays(0, gYear, 0)) - 10;
	}
	//
	//	Add the days on the Julian calendar
	//
	//	 Starting date is 12/31/1599
	//
	//	 Determine the number of 4 year periods
	//
	n4Years = numDays / 1461;
	numDays = numDays % 1461;
	//	 Determine the number of years
	//	   The first year is a leap year
	nFullYears = 0;
	if (numDays >= 366)
	{
		nFullYears = 1;
		numDays -= 366;
		nFullYears += numDays / 365;
		numDays = numDays % 365;
	}
	//
	// Only part of a year remains
	//
	//	If 1 day is left, it is 1/1 of the next year
	//
	jYear = 1599 + nMegaYears * 1000000 + n4Years * 4 + nFullYears;
	//
	if (numDays == 0)
	{
		dateJulian.gigayear = jGiga;
		dateJulian.year = jYear;
		dateJulian.month = 12;
		dateJulian.dayOfMonth = 31;
		dateJulian.calendar = 1;
		dateJulian.dateInit = 0;
		return dateJulian;
	}
	else
	{
		jYear++;
		md = dayOfYearToMonthDay(numDays, numLeapDays(0, jYear, 1));
		dateJulian.gigayear = jGiga;
		dateJulian.year = jYear;
		dateJulian.month = md[0];
		dateJulian.dayOfMonth = md[1];
		dateJulian.dateInit = 0;
		return dateJulian;
	}
}

DateCoords convertToUTCGregorian(DateCoords dateCal)
{
	// Convert a date on a specified Gregorian calendar to a UTC Gregorian date
	// Assume dateCal is a valid date, i.e., has passed checkDateCoordsElements.
	DateCoords dateUTCGreg, dateJulian;

	// Check if conversion is necessary
	if (dateCal.calendar == 0) return dateCal;

	// Special function for Julian dates
	if (dateCal.calendar == 1) return julianToUTCGregorian(dateCal);

	// If the date has been extended due to a translation from wall time to
	//	universal time, convert to Julian, then Gregorian
	//
	if (dateCal.calendar >= e9)
	{
		return julianToUTCGregorian(convertToJulian(dateCal));
	}

	// Up to the earliest transition date and after the lastest transition date
	//	all calendars are the same
	if (isLessOrEqualDateCoords_NC(dateCal, EarliestTransitionDate) ||
		isGreaterDateCoords_NC(dateCal, LatestTransitionDate))
	{
		dateUTCGreg = dateCal;
		dateUTCGreg.calendar = 0;
		return dateUTCGreg;
	}

	// Special handling for Swedish calendar
	if (dateCal.calendar == 2)
	{
		// Up to 2/28/1700, the date is Julian
		// From 3/1/1700 to 2/30/1712, use the Swedish convention
		//	The Swedish date is 1 day ahead of the Julian convention
		//	 E.g., 3/1/1700 Swedish is 2/29/1700 Julian
		//	       4/1/1700 Swedish is 3/31/1700 Julian
		//	       2/29/1712 Swedish is 2/28/1712 Julian
		//		   2/30/1712 Swedish is 2/29/1712 Julian
		//  Assume the date is a Julian date and go back 1 day
		//	  Works for 2/30/1712, as long as we allow 2/30 Julian
		//		during the calculation.
		// From 3/1/1712 to 2/17/1753, the date is Julian
		// From 3/1/1753 forward, the date is Gregorian
		if (isLessOrEqualDateCoords_NC(dateCal, feb281700)) return julianToUTCGregorian(dateCal);
		else if (isLessDateCoords_NC(dateCal, mar011712))
		{
			// Move back 1 day on the Julian calendar
			dateJulian = dateCal;
			dateJulian.calendar = 1;
			dateJulian.dayOfMonth--;
			if (dateJulian.dayOfMonth == 0)
			{
				dateJulian.month--;
				if (dateJulian.month == 0)
				{
					dateJulian.year--;
					dateJulian.month = 12;
					dateJulian.dayOfMonth = 31;
				}
				else
				{
					dateJulian.dayOfMonth = 31;
					if (dateJulian.month == 4 || dateJulian.month == 6 || dateJulian.month == 9
						|| dateJulian.month == 11) dateJulian.dayOfMonth = 30;
					else if (dateJulian.month == 2)
					{
						dateJulian.dayOfMonth = 28;
						if (numLeapDays(0, dateJulian.year, 1)) dateJulian.dayOfMonth = 29;
					}
				}
			}
			return julianToUTCGregorian(dateJulian);
		}
		else if (isLessDateCoords_NC(dateCal, mar011753)) return julianToUTCGregorian(dateCal);
		else
		{
			dateUTCGreg = dateCal;
			dateUTCGreg.calendar = 0;
			return dateUTCGreg;
		}
	}

	// At this point, the date is in between the earlist and latest transition date
	//	Up to and including the last Julian date, the date is Julian.  After the transition date, it is Gregorian
	//
	if (isLessOrEqualDateCoords_NC(dateCal, lastJulianDate(dateCal.calendar)))
	{
		dateJulian = dateCal;
		dateJulian.calendar = 1;
		return julianToUTCGregorian(dateJulian);
	}
	else
	{
		dateUTCGreg = dateCal;
		dateUTCGreg.calendar = 0;
	}
	return dateUTCGreg;
}

DateCoords convertToJulian(DateCoords dateCalIn)
{
	// Convert a date on a specified calendar to a Julian date
	// Assume dateCal is a valid date, i.e., has passed checkDateCoordsElements.
	DateCoords dateJulian, dateCal = dateCalIn;

	// If the calendar was extended due to a translation from wall time to
	//	universal time, convert to non-extended calendar
	//
	if (dateCal.calendar >= e9 && dateCal.calendar < 2 * e9)
	{
		// Calendar has been extended into the day before the first Gregorian date
		//	Adjust to the last Julian date on the non-extended calendar
		dateCal.calendar = dateCal.calendar % e9;
		dateCal = lastJulianDate(dateCal.calendar);
		dateCal.calendar = 1;
		return dateCal;
	}
	else if (dateCal.calendar >= 2 * e9)
	{
		// Calendar has been extended into the day after the last Julian date
		//	This is the correct Julian date.
		dateCal.calendar = 1;
		return dateCal;
	}

	// Check if conversion is necessary
	if (dateCal.calendar == 1) return dateCal;

	// Special function for Gregorian dates
	if (dateCal.calendar == 0) return uTCGregorianToJulian(dateCal);

	// Initialize dateJulian
	dateJulian = dateCal;
	dateJulian.calendar = 1;

	// Up to the earliest transition date and after the lastest transition date
	//	all calendars are the same

	if (isLessOrEqualDateCoords_NC(dateCal, EarliestTransitionDate) ||
		isGreaterDateCoords_NC(dateCal, LatestTransitionDate)) return dateJulian;

	// Special handling for Swedish calendar
	if (dateCal.calendar == 2)
	{
		// Up to 2/28/1700, the date is Julian
		// From 3/1/1700 to 2/30/1712, use the Swedish convention
		//	The Swedish date is 1 day ahead of the Julian convention
		//	 E.g., 3/1/1700 Swedish is 2/29/1700 Julian
		//	       4/1/1700 Swedish is 3/31/1700 Julian
		//	       2/29/1712 Swedish is 2/28/1712 Julian
		//		   2/30/1712 Swedish is 2/29/1712 Julian
		//  Assume the date is a Julian date and go back 1 day
		//	  Works for 2/30/1712, as long as we allow 2/30 Julian
		//		during the calculation.
		// From 3/1/1712 to 2/17/1753, the date is Julian
		// From 3/1/1753 forward, the date is Gregorian
		if (isLessOrEqualDateCoords_NC(dateCal, feb281700)) return dateJulian;
		else if (isLessDateCoords_NC(dateCal, mar011712))
		{
			// Move back 1 day on the Julian calendar
			dateJulian.dayOfMonth--;
			if (dateJulian.dayOfMonth == 0)
			{
				dateJulian.month--;
				if (dateJulian.month == 0)
				{
					dateJulian.year--;
					dateJulian.month = 12;
					dateJulian.dayOfMonth = 31;
				}
				else
				{
					dateJulian.dayOfMonth = 31;
					if (dateJulian.month == 4 || dateJulian.month == 6 || dateJulian.month == 9
						|| dateJulian.month == 11) dateJulian.dayOfMonth = 30;
					else if (dateJulian.month == 2)
					{
						dateJulian.dayOfMonth = 28;
						if (numLeapDays(0, dateJulian.year, 1)) dateJulian.dayOfMonth = 29;
					}
				}
			}
			return dateJulian;
		}
		else if (isLessDateCoords_NC(dateCal, mar011753)) return dateJulian;
		else return uTCGregorianToJulian(dateCal);
	}

	// At this point, the date is in between the earlist and latest transition date
	//	Up to and including the last Julian date, the date is Julian.  After the transition date, it is Gregorian
	//
	if (isLessOrEqualDateCoords_NC(dateCal, lastJulianDate(dateCal.calendar))) return dateJulian;
	else return uTCGregorianToJulian(dateCal);
}

DateCoords convertFromUTCGregorian(DateCoords dateUTCGreg, uint32_t calendar)
{
	// Convert a UTC Gregorian date to a date on a specified calendar
	//	The UTC Gregorian calendar has a transition from Julian to Gregorian dates from
	//	  4-Oct-1582 23:59:59.999999999 999999999 to 15-Oct-1582 00:00:00 in universal time.
	//	  As the transition generally occurs at the above hour in wall time, universal time
	//	  can be different from UTC Gregorian time for a few hours around the transition time.
	//	  For example, where the offset from GMT is +3:00, the transition jumps from
	//	  on 4-Oct-1582 20:59:59.999999999 999999999 to 14-Oct-1582 21:00:00 when considered
	//	  in universal time.  As such, the last 3 hours of 14-Oct-1582 must be considered valid.
	//
	//	When times in those 3 hours are converted to UTC Gregorian, the time stays the same, but
	//	  the date is changed to 4-Oct-1582.  This conversation results in a correct calculation
	//	  of the number of ticks.
	//

	DateCoords dateCal, dateJulian;
	int daysInMonth;

	// Assume dateUTCGreg is a valid UTC Gregorian date, i.e., has passed checkDateCoordsElements.
	//	Check that dateGreg.calendar is correct and that the conversion is to a valid calendar.
	//	Calendars >= e9 are not allowed, since we cannot convert to these calendars.  Dates on those
	//	calendars only result from the application of offsets to CalCoords.
	//
	if (dateUTCGreg.calendar != 0 || calendar >= e9)
	{
		// Not a UTC Gregorian date or trying to convert to an invalid calendar.  
		//	Conversion cannot be performed.
		dateCal.year = 0;
		dateCal.month = 0;
		dateCal.dayOfMonth = 0;
		dateCal.calendar = dateUTCGreg.calendar;
		dateCal.dateInit |= InvalidCalendar;
		return dateCal;
	}

	// Check if conversion is necessary
	if (calendar == 0) return dateUTCGreg;

	// Special function for Julian dates
	if (calendar == 1) return  uTCGregorianToJulian(dateUTCGreg);

	// Up to the earliest transition date and after the lastest transition date
	//	all calendars are the same
	if (isLessOrEqualDateCoords_NC(dateUTCGreg, EarliestTransitionDate) ||
		isGreaterOrEqualDateCoords_NC(dateUTCGreg, LatestFirstGregorianDate))
	{
		dateCal = dateUTCGreg;
		dateCal.calendar = calendar;
		return dateCal;
	}

	// Special handling for Swedish calendar
	if (calendar == 2)
	{
		// Up to 2/28/1700, the date is Julian
		// From 3/1/1700 to 2/30/1712, use the Swedish convention
		//	The Swedish date is 1 day ahead of the Julian convention
		//	 E.g., 3/1/1700 Swedish is 2/29/1700 Julian
		//	       4/1/1700 Swedish is 3/31/1700 Julian
		//	       2/29/1712 Swedish is 2/28/1712 Julian
		//		   2/30/1712 Swedish is 2/29/1712 Julian
		//  Assume the date is a Julian date and go back 1 day
		//	  Works for 2/30/1712, as long as we allow 2/30 Julian
		//		during the calculation.
		// From 3/1/1712 to 2/17/1753, the date is Julian
		// From 3/1/1753 forward, the date is Gregorian
		//
		// Convert to Julian date
		dateJulian = uTCGregorianToJulian(dateUTCGreg);
		dateJulian.calendar = 2;
		if (isLessOrEqualDateCoords_NC(dateJulian, feb281700)) return dateJulian;
		else if (isLessDateCoords_NC(dateJulian, mar011712))
		{
			// Move forward 1 day on the Swedish calendar
			dateCal = dateJulian;
			if (dateCal.month == 4 || dateCal.month == 6 || dateCal.month == 9 || dateCal.month == 11)
				daysInMonth = 30;
			else if (dateCal.month == 2) daysInMonth = 28 + numLeapDays(0, dateCal.year, 2);
			else daysInMonth = 31;
			dateCal.dayOfMonth++;
			if (dateCal.dayOfMonth > daysInMonth)
			{
				dateCal.month++;
				dateCal.dayOfMonth = 1;
				if (dateCal.month == 13)
				{
					dateCal.year++;
					dateCal.month = 1;
				}
			}
			return dateCal;
		}
		else if (isLessDateCoords_NC(dateJulian, mar011753)) return dateJulian;
		else
		{
			dateCal = dateUTCGreg;
			dateCal.calendar = 2;
			return dateCal;
		}
	}

	// At this point, the date is in between the earlist and latest transition date
	//	Up to 1 day before the first Gregorian date, the date to be returned is Julian.  
	//	After that date, it is Gregorian.
	//
	if (isLessDateCoords_NC(dateUTCGreg, firstGregorianDate(calendar))) return uTCGregorianToJulian(dateUTCGreg);
	else
	{
		dateCal = dateUTCGreg;
		dateCal.calendar = calendar;
	}
	return dateCal;
}

DateCoords convertToCalendar(DateCoords date, uint32_t calendar)
{
	// Convert a date on any calendar to a date on a specified calendar
	// Assume date is a valid date, i.e., has passed checkDateCoordsElements.

	DateCoords dateCal, dateUTCGreg;

	// Check validity of calendar
	if (calendar >= e9)
	{
		// Attempt to convert to an invalid calendar
		dateCal.year = 0;
		dateCal.month = 0;
		dateCal.dayOfMonth = 0;
		dateCal.calendar = calendar;
		dateCal.dateInit |= InvalidCalendar;
		return dateCal;
	}
	// Check if conversion is necessary
	if (calendar == date.calendar) return date;

	// Special function for Gregorian calendar
	if (calendar == 0) return convertToUTCGregorian(date);

	// Special function for Julian calendar
	if (calendar == 1) return  convertToJulian(date);

	// Up to the earliest transition date and after the lastest transition date
	//	all calendars are the same
	if (isLessOrEqualDateCoords_NC(date, EarliestTransitionDate) ||
		isGreaterOrEqualDateCoords_NC(date, LatestFirstGregorianDate))
	{
		dateCal = date;
		dateCal.calendar = calendar;
		return dateCal;
	}

	// At the point, we know the input date is beyond the earliest possible transition date,
	//	and before the latest first Gregorian date
	// Convert to a UTCGregorian date then to the specified calendar
	//
	dateUTCGreg = convertToUTCGregorian(date);
	return convertFromUTCGregorian(dateUTCGreg, calendar);
}

CalCoords convertCalToUTCGregorian(CalCoords cc1)
{
	// Convert a CalCoords to a CalCoords on the UTC Gregorian calendar
	CalCoords cc2 = cc1;

	cc2.date = convertToUTCGregorian(cc1.date);
	return cc2;
}

CalCoords convertCalToJulian(CalCoords cc1)
{
	// Convert a CalCoords to a CalCoords on the Julian calendar
	CalCoords cc2 = cc1;

	cc2.date = convertToJulian(cc1.date);
	return cc2;
}

CalCoords convertCalToCalendar(CalCoords cc1, uint32_t calendar)
{
	// Convert a CalCoords to a CalCoords on a specified calendar
	CalCoords cc2 = cc1;

	cc2.date = convertToCalendar(cc1.date, calendar);
	return cc2;
}

uint32_t dayOfYearFromDate(DateCoords date)
{
	// Extract the day of the year from a Date
	DateCoords transStart;
	uint8_t nDaysSkipped;
	uint32_t calendar;
	int8_t gigayear, month, dayOfMonth;
	int32_t year;

	gigayear = date.gigayear;
	year = date.year;
	month = date.month;
	dayOfMonth = date.dayOfMonth;
	calendar = date.calendar;

	if (calendar == 1 || gigayear != 0 || year < EarliestTransitionDate.year ||
		year > LatestTransitionDate.year)
	{
		// Julian calendar or outside of the transition period, so no skipped days
		return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar));
	}

	// If there is a transition from a Julian to a Gregorian calendar, there are 3
	//	possible cases:
	//  1. The transition occurs entirely within the year
	//	2. The transition occurs at the end of the previous year and extends into the year
	//	3. The transition occurs at the end of the year and extends into the next year
	//
	//	Determine the date the transition begins
	transStart = lastJulianDate(calendar);
	if (transStart.year < year - 1 || transStart.year > year)
	{
		// Transition does not affect the year
		return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar));
	}
	else
	{
		// The transition could affect the year
		//  Determine the number of days skipped
		nDaysSkipped = numDaysToSkip(calendar);
		if (transStart.year == year - 1 && (transStart.month != 12 ||
			transStart.dayOfMonth + nDaysSkipped <= 31))
		{
			// Skipped days are in the previous year
			return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar));
		}
		else if (transStart.year == year - 1)
		{
			// Transition begins in previous year and carries over into this year
			return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar))
				- (firstGregorianDate(calendar).dayOfMonth - 1);
		}
		else if (transStart.year == year && (transStart.month != 12 ||
			transStart.dayOfMonth + nDaysSkipped <= 31))
		{
			// Transition occurs entirely during the year
			if (month < transStart.month || (month == transStart.month
				&& dayOfMonth <= transStart.dayOfMonth))
			{
				// Transition occurs after the date
				return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar));
			}
			else
			{
				// Transition occurs before the date
				return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar))
					- nDaysSkipped;
			}
		}
		else
		{
			// Transition begins during the year and ends the next year
			//	Date must be before the transition
			return monthDayToDayOfYear(month, dayOfMonth, numLeapDays(gigayear, year, calendar));
		}
	}
}

DateCoords firstGregorianDate(uint32_t calendar)
{
	// Find first Gregorian date by adding days to the last Julian date

	/* 
		The transition from the Julian to Gregorian calendar is
		assumed to occur at midnight in local wall time.  As such,
		when a valid wall time is translated to standard time or
		universal time, the day may be incremented or decremented
		to a day that would be invalid in wall time.

		When such a translation occurs (in offsetCalCoords), the
		calendar is adjusted, by adding 1e9, in the case of an
		increment and 2e9, in the case of a decrement.  These
		adjustments either move the last Julian date forward
		a day or the first Gregorian date back a day.

		For calendar == 1, there is no first Gregorian date
		For calendar == 0, the first Gregorian date is 10/15/1582.
		For calendar == 2, the first Gregorian date is 03/01/1753.
		For all other values of calendar, the first Gregorian date is determined
			by parsing the value of calendar, which is yyyymmdd to determine the
			last Julian date, then adding the appropriate number of days.

			Starting on 10/4/1582 J, Julian Day 2,299,160, the number of days to skip is 10
			so the next day is 11 days later on the Gregorian calendar.
			Starting on 3/1/1700 J, Julian Day 2,342,043, the number of days to skip is 11.
			so the next day is 12 days later on the Gregorian calendar.
			Starting on 3/1/1800 J, Julian Day 2,378,568, the number of days to skip is 12.
			so the next day is 13 days later on the Gregorian calendar.
			Starting on 3/1/1900 J, Julian Day 2,415,093, the number of days to skip is 13.
			so the next day is 14 days later on the Gregorian calendar.

			Turkey was the last country to change in 1927, but we allow much later transitions.


	*/

	DateCoords date1, date2;
	uint8_t nDaysToSkip;

	// Extract last Julian date if necessary

	if (calendar == 1)
	{
		// Julian calendar, so no calculation to be performed
		date1.gigayear = 99;
		date1.year = 999999999;
		date1.month = 0;
		date1.dayOfMonth = 0;
		date1.calendar = 0;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 0)
	{
		// Return earliest Gregorian date
		date1.gigayear = 0;
		date1.year = 1582;
		date1.month = 10;
		date1.dayOfMonth = 15;
		date1.calendar = 0;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 2*e9)
	{
		// Return one day before the earliest Gregorian date
		date1.gigayear = 0;
		date1.year = 1582;
		date1.month = 10;
		date1.dayOfMonth = 14;
		date1.calendar = 0;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 2)
	{
		// Return first Gregorian date on the Swedish calendar
		date1.gigayear = 0;
		date1.year = 1753;
		date1.month = 3;
		date1.dayOfMonth = 1;
		date1.calendar = 0;
		date1.dateInit = 0;
		return date1;
	}
	else if (calendar == 2 + 2*e9)
	{
		// Return one day before the first Gregorian date on the Swedish calendar
		date1.gigayear = 0;
		date1.year = 1753;
		date1.month = 2;
		date1.dayOfMonth = 29;
		date1.calendar = 0;
		date1.dateInit = 0;
		return date1;
	}
	//	Check for value within range
	if ((calendar % e9 < MinCalendar) || (calendar % e9 > MaxCalendar)) // 28-Feb-4100 is latest transition date
	{
		// Value out of range
		date1.gigayear = 0;
		date1.year = 0;
		date1.month = 0;
		date1.dayOfMonth = 0;
		date1.calendar = 0;
		date1.dateInit |= InvalidCalendar;
		return date1;
	}
	else
	{
		// Extraction is necessary
		date1 = lastJulianDate(calendar);

		// date1 is the last Julian date.
		//	Determine the number of days to skip then calculate the first Gregorian date
		//	numDaysToSkip handles the decrease associated with calendar > e9

		nDaysToSkip = numDaysToSkip(calendar);

		date1.calendar = 0;
		date2 = skipGregorianDays(nDaysToSkip, date1);
		return date2;
	}
}

UTCDatetime createUTCDatetimeFromCalCoords(CalCoords cc, int8_t precision,
	int8_t uncertainty, uint8_t futureAdjust)
{
	// Create a UTCDatetime from CalCoords, assuming the CalCoords represent a Universal
	//	time on a specified calendar.
	//
	//
	//	Call createUTCDatetime
	//
	return createUTCDatetime(cc.date.gigayear, cc.date.year, cc.date.month, cc.date.dayOfMonth,
		cc.time.hour, cc.time.minute, cc.time.second, cc.time.nanosecond, cc.time.attosecond,
		precision, uncertainty, cc.date.calendar, futureAdjust);
}
