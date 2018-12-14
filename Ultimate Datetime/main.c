//#include "../UltimateDatetimeDLL/UltimateDatetime.h"
#include "../LocalDatetimeDLL/LocalDatetime.h"
#include "private.h"
#include <time.h>
int main(void)
{
	// Test suite

	FILE *ifp, *ofp, *tzp;
	char *readmode = "r";
	char *writemode = "w";
	char inputFilename[] = "datetimetest.input";
	char outputFilename[] = "datetimetest.output";
	char tzPeriodsFilename[] = "timezoneperiods.output";
	char *not[] = {" not ", " "};
	uint32_t val3, val4, val5, val6, val7, val8;
	uint32_t val12, val14, val15, val18, val19, val20;
	uint32_t val21, val22, val23, val27, val29, val30;
	int val1, val2, val9, val16, val17, val10, val24, val25, val11, val26;
	char functionName[35];
	char stringResult[130];
	char description[200];
	char passOrFail[500];
	char readableString1[55], readableString2[55], readableString3[55];
	char stringUTCDatetime1[130], stringUTCDatetime2[130];
	char tickElemString[85], tickElemString2[85], tickElemString3[85];
	int lenUTCError = 800, lenTAIRelError = 800;
	char createUTCError[800], createTAIRelError[800];
	char tz1[31], tz2[31];
	int intResult1, intResult2, intResult3, intResult4;
	int iter, funcode;
	int numFunctions = 54;
	int testCount = 0;
	DateCoords date1;
	const char *ordinals[] =
	{ "first", "second", "third", "fourth", "fifth" };
	const char *daysOfTheWeek[] =
	{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	const char *functions[] = 
	 {"isEqualTicks64", "isEqualTicks32", "isEqualTicks", "isGreaterTicks64",
	  "isGreaterTicks32", "isGreaterTicks", "isLessTicks64", "isLessTicks32",
	  "isLessTicks", "isNotEqualTicks", "isGreaterOrEqualTicks",
	  "isLessOrEqualTicks", "diffTicks64", "diffTicks32", "diffTicks",
	  "dayOfYearToMonthDay", "monthDayToDayOfYear", "addRelTicksToTicks64",
	  "addRelTicksToTicks32", "addRelTicksToTicks", "negateRelTicks64", "negateRelTicks32",
	  "negateRelTicks", "subtractRelTicksFromTicks64", "subtractRelTicksFromTicks32",
	  "subtractRelTicksFromTicks","calcTicks", "asStringUTCDatetime",
	  "cumLeapSecondsUnadj", "diffUTCDatetimes", "deriveUTCDatetime",
	  "adjustRelTicks", "unAdjustRelTicks", "dayOfTheWeek", "isLeapSecondDay",
	  "createUTCDatetimeFromDayFrac","createUTCDatetimeFromSecondFrac",
	  "createTAIRelDatetimeFromDayFrac","createTAIRelDatetimeFromSecondFrac",
	  "deriveTAIRelDatetime", "addRelTicks64", "addRelTicks32",
	  "addRelTicks", "subtractRelTicks64", "subtractRelTicks32",
	  "subtractRelTicks", "intMultTAIRelTicks", "intMultRelDatetime",
	  "addRelDatetimes", "subtractRelDatetimes", "ratioRelDatetimes",
	  "doubleMultRelDatetime", "dayOfMonthFromWeekdayRule", "firstGregorianDate"};
	TAITicks64 t164, t264;
	TAITicks32 t132, t232;
	TAITicks t1, t2;
	TAIRelTicks64 r164, r264, r364;
	TAIRelTicks32 r132, r232, r332;
	TAIRelTicks r1, r2, r3;
	UTCDatetime u1, u2;
	char mode;
	uint8_t * md;
	uint32_t day;
	int  cal;
	int jd;
	TAIRelDatetime re1, re2, re3;
	UncertainDouble rr;
	int period, isNegative;
	uint16_t retcode;
	const int adjustment[3] = {15, 24, 25};
	double dayFrac, daysPlusFrac, secondsPlusFrac, secondFrac, denom, dmult;
    int failTest, numFails;

    time_t time1;
	timezone_t tzone1, tzone2;
	struct tm lt1, *lt2, lt3;

	tzone1 = tzalloc("America/New_York");
	tzone2 = tzalloc("UTC");
	lt1.tm_year = 70;
	lt1.tm_mon = 0;
	lt1.tm_mday = 1;
	lt1.tm_hour = 1;
	lt1.tm_min = 0;
	lt1.tm_sec = 0;
	lt1.tm_isdst = -1;

	time1 = mktime_z(tzone1, &lt1);
	lt2 = localtime_rz(tzone2, &time1, &lt3);

	uint32_t tzindex, tzfields, tzfields1, tzfields2;
	uint8_t tzper, bora, fa, frame, hund;

	tzfields = 0;
	tzfields = setTimeZone(tzfields, 302);
	tzfields = setTZPeriod(tzfields, 85);
	tzfields = setFrame(tzfields, 2);
	tzfields = setAmbigS(tzfields, 1);
	tzfields = setAmbigW(tzfields, 0);
	tzfields = setBeforeFlag(tzfields, 1);
	tzfields = setFutureAdjust(tzfields, 3);
	tzfields = setFracOffset(tzfields, 56);

	tzindex = getTimeZone(tzfields);
	tzper = getTZPeriod(tzfields);
	bora = getBOrA(tzfields);
	fa = getFutureAdjust(tzfields);
	frame = getFrame(tzfields);
	hund = getFracOffset(tzfields);

	tzfields = 0;
	tzfields = setPacked32BitValue(tzfields, 9, 0, 302);
	tzfields = setPacked8BitValue(tzfields, 7, 9, 85);
	tzfields = setPacked8BitValue(tzfields, 2, 16, 2);
	tzfields = setPacked8BitValue(tzfields, 2, 18, 3);
	tzfields = setPacked8BitValue(tzfields, 2, 20, 1);
	tzfields = setPacked8BitValue(tzfields, 7, 22, 56);

	tzindex = getPacked32BitValue(tzfields, 9, 0);
	tzper = getPacked8BitValue(tzfields, 7, 9);
	bora = getPacked8BitValue(tzfields, 2, 16);
	fa = getPacked8BitValue(tzfields, 2, 18);
	frame = getPacked8BitValue(tzfields, 2, 20);
	hund = getPacked8BitValue(tzfields, 7, 22); 

	TimeZoneFields tzf1 = unpackTimeZoneFieldsTZIndex(tzfields);
	TimeZoneFields tzf2 = unpackTimeZoneFields(tzfields); 

	tzfields1 = packTimeZoneFieldsTZIndex(tzf1.timeZoneIndex, tzf1.period, tzf1.frame,
		tzf1.ambigS, tzf1.ambigW, tzf1.beforeFlag, tzf1.futureAdjust, tzf1.fracOffset);
	tzfields2 = packTimeZoneFields(tzf2.timezone, tzf1.period, tzf1.frame,
		tzf1.ambigS, tzf1.ambigW, tzf1.beforeFlag, tzf1.futureAdjust, tzf1.fracOffset);

	// Open the time zone periods output file
	fopen_s(&tzp, tzPeriodsFilename, writemode);

	if (tzp == NULL)
	{
		printf("Can't open time zone period output file %s!\n", tzPeriodsFilename);
		return (1);
	}

	outputTimeZonePeriods(tzp);

	// Open the input file
	fopen_s(&ifp, inputFilename, readmode);

	if (ifp == NULL)
	{
		printf("Can't open input file %s!\n", inputFilename);
		return (1);
	}

	// Open the output file
	fopen_s(&ofp, outputFilename, writemode);

	if (ofp == NULL)
	{
		printf("Can't open output file %s!\n", outputFilename);
		return (1);
	}
	//double val, truncval;
	//val = 123456789.1234567;
	//truncval = trunc(val / 0.0001 + 0.5) * 0.0001;
	//printf("%17.7f", truncval);
	//getchar();
	// Test largest exact value
	//double vm = doublePrecision - 1.;
	//double vm2 = doublePrecision;
	//double vm3 = doublePrecision + 1.;
	//double vm4 = doublePrecision + 2;
	//double vm5 = 1. + minFracUncertainty;
	//printf("Maximum exact value - 1: %17.15e\n", vm);
	//printf("Maximum exact value    : %17.15e\n", vm2);
	//printf("Maximum exact value + 1: %17.15e\n", vm3);
	//printf("Maximum exact value + 2: %17.15e\n", vm4);
	//printf("1 + Min Frac Uncertainty: %17.15e\n", vm5);

	//// Test number of significant decimal digits
	//double x13, x14, x15, x16, x17;
	//double i = 2.999, j = 1., k = 1.;
	//for (j = 0.; j <= 9.; j++)
	//{
	//	x13 = i + j*0.0001 + k*1.e-12;
	//	x14 = i + j*0.0001 + k*1.e-13;
	//	x15 = i + j*0.0001 + k*1.e-14;
	//	x16 = i + j*0.0001 + k*1.e-15;
	//	x17 = i + j*0.0001 + k*1.e-16;
		//printf("Output 17 digits after the decimal point.\n");
		//printf("                       1.12345678901234567\n");
		//printf("13 significant digits: %19.17e\n", x13);
		//printf("14 significant digits: %19.17e\n", x14);
		//printf("15 significant digits: %19.17e\n", x15);
		//printf("16 significant digits: %19.17e\n", x16);
		//printf("17 significant digits: %19.17e\n", x17);
		//printf("Output 16 digits after the decimal point.\n");
		//printf("                       1.12345678901234567\n");
		//printf("13 significant digits: %18.16e\n", x13);
		//printf("14 significant digits: %18.16e\n", x14);
		//printf("15 significant digits: %18.16e\n", x15);
		//printf("16 significant digits: %18.16e\n", x16);
		//printf("17 significant digits: %18.16e\n", x17);
		//printf("Output 15 digits after the decimal point.\n");
		//printf("                       1.12345678901234567\n");
		//printf("13 significant digits: %17.15e\n", x13);
		//printf("14 significant digits: %17.15e\n", x14);
		//printf("15 significant digits: %17.15e\n", x15);
		//printf("16 significant digits: %17.15e\n", x16);
		//printf("17 significant digits: %17.15e\n", x17);
		//printf("Output 14 digits after the decimal point.\n");
		//printf("                       1.12345678901234567\n");
		//printf("13 significant digits: %16.14e\n", x13);
		//printf("14 significant digits: %16.14e\n", x14);
		//printf("15 significant digits: %16.14e\n", x15);
		//printf("16 significant digits: %16.14e\n", x16);
		//printf("17 significant digits: %16.14e\n", x17);
	//}
	// Test the output of values close to the reciprocal of the maximum precision
	//	to develop a test for 17 vs 16 digits
	//printf("%17.0f\n", 9999999999999998.e-3/1.e-3);
	//printf("%17.0f\n", 9999999999999999.e-3 / 1.e-3);
	//printf("%17.0f\n", 10000000000000000.e-3 / 1.e-3);
	//getchar();

	// Test the accuracy and precision of the power and log10 functions on integral powers of10
//	double values[201];
//	values[0] = 1.e-100;
//	values[1] = 1.e-99;
//	values[2] = 1.e-98;
//	values[3] = 1.e-97;
//	values[4] = 1.e-96;
//	values[5] = 1.e-95;
//	values[6] = 1.e-94;
//	values[7] = 1.e-93;
//	values[8] = 1.e-92;
//	values[9] = 1.e-91;
//	values[10] = 1.e-90;
//	values[11] = 1.e-89;
//	values[12] = 1.e-88;
//	values[13] = 1.e-87;
//	values[14] = 1.e-86;
//	values[15] = 1.e-85;
//	values[16] = 1.e-84;
//	values[17] = 1.e-83;
//	values[18] = 1.e-82;
//	values[19] = 1.e-81;
//	values[20] = 1.e-80;
//	values[21] = 1.e-79;
//	values[22] = 1.e-78;
//	values[23] = 1.e-77;
//	values[24] = 1.e-76;
//	values[25] = 1.e-75;
//	values[26] = 1.e-74;
//	values[27] = 1.e-73;
//	values[28] = 1.e-72;
//	values[29] = 1.e-71;
//	values[30] = 1.e-70;
//	values[31] = 1.e-69;
//	values[32] = 1.e-68;
//	values[33] = 1.e-67;
//	values[34] = 1.e-66;
//	values[35] = 1.e-65;
//	values[36] = 1.e-64;
//	values[37] = 1.e-63;
//	values[38] = 1.e-62;
//	values[39] = 1.e-61;
//	values[40] = 1.e-60;
//	values[41] = 1.e-59;
//	values[42] = 1.e-58;
//	values[43] = 1.e-57;
//	values[44] = 1.e-56;
//	values[45] = 1.e-55;
//	values[46] = 1.e-54;
//	values[47] = 1.e-53;
//	values[48] = 1.e-52;
//	values[49] = 1.e-51;
//	values[50] = 1.e-50;
//	values[51] = 1.e-49;
//	values[52] = 1.e-48;
//	values[53] = 1.e-47;
//	values[54] = 1.e-46;
//	values[55] = 1.e-45;
//	values[56] = 1.e-44;
//	values[57] = 1.e-43;
//	values[58] = 1.e-42;
//	values[59] = 1.e-41;
//	values[60] = 1.e-40;
//	values[61] = 1.e-39;
//	values[62] = 1.e-38;
//	values[63] = 1.e-37;
//	values[64] = 1.e-36;
//	values[65] = 1.e-35;
//	values[66] = 1.e-34;
//	values[67] = 1.e-33;
//	values[68] = 1.e-32;
//	values[69] = 1.e-31;
//	values[70] = 1.e-30;
//	values[71] = 1.e-29;
//	values[72] = 1.e-28;
//	values[73] = 1.e-27;
//	values[74] = 1.e-26;
//	values[75] = 1.e-25;
//	values[76] = 1.e-24;
//	values[77] = 1.e-23;
//	values[78] = 1.e-22;
//	values[79] = 1.e-21;
//	values[80] = 1.e-20;
//	values[81] = 1.e-19;
//	values[82] = 1.e-18;
//	values[83] = 1.e-17;
//	values[84] = 1.e-16;
//	values[85] = 1.e-15;
//	values[86] = 1.e-14;
//	values[87] = 1.e-13;
//	values[88] = 1.e-12;
//	values[89] = 1.e-11;
//	values[90] = 1.e-10;
//	values[91] = 1.e-9;
//	values[92] = 1.e-8;
//	values[93] = 1.e-7;
//	values[94] = 1.e-6;
//	values[95] = 1.e-5;
//	values[96] = 1.e-4;
//	values[97] = 1.e-3;
//	values[98] = 1.e-2;
//	values[99] = 1.e-1;
//	values[100] = 1.;
//	values[101] = 1.e1;
//	values[102] = 1.e2;
//	values[103] = 1.e3;
//	values[104] = 1.e4;
//	values[105] = 1.e5;
//	values[106] = 1.e6;
//	values[107] = 1.e7;
//	values[108] = 1.e8;
//	values[109] = 1.e9;
//	values[110] = 1.e10;
//	values[111] = 1.e11;
//	values[112] = 1.e12;
//	values[113] = 1.e13;
//	values[114] = 1.e14;
//	values[115] = 1.e15;
//	values[116] = 1.e16;
//	values[117] = 1.e17;
//	values[118] = 1.e18;
//	values[119] = 1.e19;
//	values[120] = 1.e20;
//	values[121] = 1.e21;
//	values[122] = 1.e22;
//	values[123] = 1.e23;
//	values[124] = 1.e24;
//	values[125] = 1.e25;
//	values[126] = 1.e26;
//	values[127] = 1.e27;
//	values[128] = 1.e28;
//	values[129] = 1.e29;
//	values[130] = 1.e30;
//	values[131] = 1.e31;
//	values[132] = 1.e32;
//	values[133] = 1.e33;
//	values[134] = 1.e34;
//	values[135] = 1.e35;
//	values[136] = 1.e36;
//	values[137] = 1.e37;
//	values[138] = 1.e38;
//	values[139] = 1.e39;
//	values[140] = 1.e40;
//	values[141] = 1.e41;
//	values[142] = 1.e42;
//	values[143] = 1.e43;
//	values[144] = 1.e44;
//	values[145] = 1.e45;
//	values[146] = 1.e46;
//	values[147] = 1.e47;
//	values[148] = 1.e48;
//	values[149] = 1.e49;
//	values[150] = 1.e50;
//	values[151] = 1.e51;
//	values[152] = 1.e52;
//	values[153] = 1.e53;
//	values[154] = 1.e54;
//	values[155] = 1.e55;
//	values[156] = 1.e56;
//	values[157] = 1.e57;
//	values[158] = 1.e58;
//	values[159] = 1.e59;
//	values[160] = 1.e60;
//	values[161] = 1.e61;
//	values[162] = 1.e62;
//	values[163] = 1.e63;
//	values[164] = 1.e64;
//	values[165] = 1.e65;
//	values[166] = 1.e66;
//	values[167] = 1.e67;
//	values[168] = 1.e68;
//	values[169] = 1.e69;
//	values[170] = 1.e70;
//	values[171] = 1.e71;
//	values[172] = 1.e72;
//	values[173] = 1.e73;
//	values[174] = 1.e74;
//	values[175] = 1.e75;
//	values[176] = 1.e76;
//	values[177] = 1.e77;
//	values[178] = 1.e78;
//	values[179] = 1.e79;
//	values[180] = 1.e80;
//	values[181] = 1.e81;
//	values[182] = 1.e82;
//	values[183] = 1.e83;
//	values[184] = 1.e84;
//	values[185] = 1.e85;
//	values[186] = 1.e86;
//	values[187] = 1.e87;
//	values[188] = 1.e88;
//	values[189] = 1.e89;
//	values[190] = 1.e90;
//	values[191] = 1.e91;
//	values[192] = 1.e92;
//	values[193] = 1.e93;
//	values[194] = 1.e94;
//	values[195] = 1.e95;
//	values[196] = 1.e96;
//	values[197] = 1.e97;
//	values[198] = 1.e98;
//	values[199] = 1.e99;
//	values[200] = 1.e100;
//	double calcvalue;
//	int ndig, digerror;
//	for (int indx = 0; indx <= 200; indx++)
//	{
//		calcvalue = pow(10., log10(values[indx]));
////		ndig = (int)floor(log10(calcvalue));
//		ndig = (int)floor(log10(values[indx]));
//		digerror = (indx - 100 - ndig);
//		fprintf(ofp, "%18.16e  %18.16e %d %d \n", values[indx], calcvalue, ndig, digerror);
//	}


	/* Read the input file and run the appropriate test.  The following is the largest example - 
			take the difference between two date times and return a result as a formatted relative tick string
			and / or the four elements of a 32-bit relative tick.
		val1 - LocalDate1.gigayear
		val2 - LocalDate1.year
		val3 - LocalDate1.month
		val4 - LocalDate1.dayOfMonth
		val5 - LocalDate1.hour
		val6 - LocalDate1.minute
		val7 - LocalDate1.second
		val8 - LocalDate1.nanosecond
		val9 - LocalDate1.attosecond
		val10 - LocalDate1.precision
		val11 - LocalDate1.uncertainty
		val12 - LocalDate1.calendar
		val13 - LocalDate1.timeZone
		val14 - LocalDate1.sOrD
		val15 - LocalDate1.futureAdjust
		val16 - LocalDate2.gigayear
		val17 - LocalDate2.year
		val18 - LocalDate2.month
		val19 - LocalDate2.dayOfMonth
		val20 - LocalDate2.hour
		val21 - LocalDate2.minute
		val22 - LocalDate2.second
		val23 - LocalDate2.nanosecond
		val24 - LocalDate2.attosecond
		val25 - LocalDate2.precision
		val26 - LocalDate2.uncertainty
		val27 - LocalDate2.calendar
		val28 - LocalDate2.timeZone
		val29 - LocalDate2.sOrD
		val30 - LocalDate2.futureAdjust
		functionName - diffLocalTime
		stringResult - Formatted relative tick
		intResult1 - RelativeTick.gigaseconds
		intResult2 - RelativeTick.seconds
		intResult3 - RelativeTick.nanoseconds
		intResult4 - RelativeTick.attoseconds
		*/

#if (defined (WIN32) || defined (_WIN32) || defined (__WIN32)) && !defined (__CYGWIN__)
    // Windows
    while (fscanf_s(ifp, "%d %d %u %u %u %u %u %u %d %d %u %u %s %u %u %d %d %u %u %u %u %u %u %d %d %u %u %s %u %u %s %s %d %d %d %d %s",
                        &val1,  &val2, &val3, &val4, &val5,  &val6, &val7, &val8, &val9, &val10,
                        &val11,  &val12, tz1, (int)sizeof(tz1), &val14, &val15,  &val16, &val17, &val18, &val19, &val20,
                        &val21,  &val22, &val23, &val24, &val25,  &val26, &val27, tz2, (int)sizeof(tz2), &val29, &val30,
                        functionName, (int)sizeof(functionName), stringResult, (int)sizeof(stringResult),
                        &intResult1, &intResult2, &intResult3, &intResult4, description, (int)sizeof(description))
                        == 37)
#else
    // Linux or OS X
    
    while (fscanf(ifp, "%d %d %u %u %u %u %u %u %d %d %u %u %s %u %u %d %d %u %u %u %u %u %u %d %d %u %u %s %u %u %s %s %d %d %d %d %s",
                        &val1,  &val2, &val3, &val4, &val5,  &val6, &val7, &val8, &val9, &val10,
                        &val11,  &val12, tz1, &val14, &val15,  &val16, &val17, &val18, &val19, &val20,
                        &val21,  &val22, &val23, &val24, &val25,  &val26, &val27, tz2, &val29, &val30,
                        functionName, stringResult,
                        &intResult1, &intResult2, &intResult3, &intResult4, description)
                        == 37)
#endif
	{
		// Find the function code
		funcode = -1;
		for (iter = 0; iter < numFunctions; iter++)
		{
			if (strcmp(functions[iter], functionName) == 0)
			{
				funcode = iter;
				break;
			}
		}
		testCount++;
		printf("Test %d: %s\n", testCount, functionName);
		if (funcode >= 0)
		{
			// Execute the appropriate function and check the result
			if (testCount > 1) fprintf(ofp, "\n");
			// Determine whether the test is supposed to fail
			failTest = (strcmp(stringResult, "Fail") == 0);
			switch (funcode)
			{
			case 0: // isEqualTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				t264 = createTicks64(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isEqualTicks64(t164, t264) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks64(t164), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks64(t264), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 1: // isEqualTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				t232 = createTicks32(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isEqualTicks32(t132, t232) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks32(t132), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks32(t232), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 2: // isEqualTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isEqualTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 3: // isGreaterTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				t264 = createTicks64(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isGreaterTicks64(t164, t264) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks64(t164), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks64(t264), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 4: // isGreaterTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				t232 = createTicks32(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isGreaterTicks32(t132, t232) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks32(t132), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks32(t232), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 5: // isGreaterTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isGreaterTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 6: // isLessTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				t264 = createTicks64(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isLessTicks64(t164, t264) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks64(t164), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks64(t264), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 7: // isLessTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				t232 = createTicks32(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isLessTicks32(t132, t232) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks32(t132), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks32(t232), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 8: // isLessTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isLessTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 9: // isNotEqualTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isNotEqualTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 10: // isGreaterOrEqualTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isGreaterOrEqualTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 11: // isLessOrEqualTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Determine the mode - i.e., was the test supposed to return true or false
				mode = intResult1 ? ' ' : '!';
				// Run the function and check the result
				(isLessOrEqualTicks(t1, t2) == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %c%s %s", readableNumberString(asStringTAITicks(t1), readableString1), mode,
					functionName, readableNumberString(asStringTAITicks(t2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 12: // diffTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				t264 = createTicks64(val5, val6, val7, val8);
				// Run the function and check the result
				r164 = diffTicks64(t164, t264);
				(strcmp(asStringTAIRelTicks64(r164), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks64(t164), readableString1), functionName,
					readableNumberString(asStringTAITicks64(t264), readableString2),
					readableNumberString(asStringTAIRelTicks64(r164), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 13: // diffTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				t232 = createTicks32(val5, val6, val7, val8);
				// Run the function and check the result
				r132 = diffTicks32(t132, t232);
				(strcmp(asStringTAIRelTicks32(r132), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks32(t132), readableString1), functionName,
					readableNumberString(asStringTAITicks32(t232), readableString2),
					readableNumberString(asStringTAIRelTicks32(r132), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 14: // diffTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				t2 = createTicks(val5, val6, val7, val8);
				// Run the function and check the result
				r1 = diffTicks(t1, t2);
				(strcmp(asStringTAIRelTicks(r1), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks(t1), readableString1), functionName,
					readableNumberString(asStringTAITicks(t2), readableString2),
					readableNumberString(asStringTAIRelTicks(r1), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 15: // dayOfYearToMonthDay
				md = dayOfYearToMonthDay((uint32_t)val1, (uint32_t)val2);
				((md[0] == intResult1) && (md[1] == intResult2) != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				fprintf(ofp, "Day %d", val1);
				(val2 ? fprintf(ofp, " in a leap year ") : fprintf(ofp, " "));
				fprintf(ofp, "= %d-%d", md[0], md[1]);
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 16: // monthDayToDayOfYear
				day = monthDayToDayOfYear((uint32_t)val1, val2, val3);
				(day == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				fprintf(ofp, "%d-%d", val1, val2);
				(val3 ? fprintf(ofp, " in a leap year ") : fprintf(ofp, " "));
				fprintf(ofp, "is day %d", day);
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 17: // addRelTicksToTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				r164 = createRelTicks64(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t264 = addRelTicksToTicks64(t164, r164);
				(strcmp(asStringTAITicks64(t264), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks64(t164), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks64(r164), readableString2),
					readableNumberString(asStringTAITicks64(t264), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 18: // addRelTicksToTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				r132 = createRelTicks32(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t232 = addRelTicksToTicks32(t132, r132);
				(strcmp(asStringTAITicks32(t232), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks32(t132), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks32(r132), readableString2),
					readableNumberString(asStringTAITicks32(t232), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 19: // addRelTicksToTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				r1 = createRelTicks(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t2 = addRelTicksToTicks(t1, r1);
				(strcmp(asStringTAITicks(t2), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks(t1), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks(r1), readableString2),
					readableNumberString(asStringTAITicks(t2), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 20: // negateRelTicks64
				// Create the value
				r164 = createRelTicks64(val1, val2, val3, val4, val9);
				// Run the function and check the result
				r264 = negateRelTicks64(r164);
				(strcmp(asStringTAIRelTicks64(r264), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s = %s", readableNumberString(asStringTAIRelTicks64(r164), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks64(r264), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 21: // negateRelTicks32
				// Create the value
				r132 = createRelTicks32(val1, val2, val3, val4, val9);
				// Run the function and check the result
				r232 = negateRelTicks32(r132);
				(strcmp(asStringTAIRelTicks32(r232), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s = %s", readableNumberString(asStringTAIRelTicks32(r132), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks32(r232), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 22: // negateRelTicks
				// Create the value
				r1 = createRelTicks(val1, val2, val3, val4, val9);
				// Run the function and check the result
				r2 = negateRelTicks(r1);
				(strcmp(asStringTAIRelTicks(r2), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the two values using 2 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s = %s", readableNumberString(asStringTAIRelTicks(r1), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks(r2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 23: // subtractRelTicksFromTicks64
				// Create the two values
				t164 = createTicks64(val1, val2, val3, val4);
				r164 = createRelTicks64(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t264 = subtractRelTicksFromTicks64(t164, r164);
				(strcmp(asStringTAITicks64(t264), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks64(t164), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks64(r164), readableString2),
					readableNumberString(asStringTAITicks64(t264), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 24: // subtractRelTicksFromTicks32
				// Create the two values
				t132 = createTicks32(val1, val2, val3, val4);
				r132 = createRelTicks32(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t232 = subtractRelTicksFromTicks32(t132, r132);
				(strcmp(asStringTAITicks32(t232), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks32(t132), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks32(r132), readableString2),
					readableNumberString(asStringTAITicks32(t232), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 25: // subtractRelTicksFromTicks
				// Create the two values
				t1 = createTicks(val1, val2, val3, val4);
				r1 = createRelTicks(val5, val6, val7, val8, val9);
				// Run the function and check the result
				t2 = subtractRelTicksFromTicks(t1, r1);
				(strcmp(asStringTAITicks(t2), stringResult) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values using 3 calls to fprintf to avoid overwriting of readableNumberString return value
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAITicks(t1), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks(r1), readableString2),
					readableNumberString(asStringTAITicks(t2), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 26: // calcTicks
				// Create the tick value
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				u1 = createUTCDatetime(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, (uint8_t)val7,
					val8, val9, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				else
				{
#ifdef Ticks64
					jd = (int)((u1.tai.seconds - 436320000000000000) / 86400);
#else
					jd = (int)(((uint64_t)u1.tai.gigaseconds * e9 + (uint64_t)u1.tai.seconds - 436320000000000000) / 86400);
#endif
					(jd == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = Julian day: %d", asStringUTCDatetime(u1, stringUTCDatetime1, val12), jd);
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 27: // asStringUTCDatetime
				// Create the tick value
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				u1 = createUTCDatetime(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, (uint8_t)val7,
					val8, val9, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				else
				{
					(strcmp(asStringUTCDatetime(u1, stringUTCDatetime1, val12), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s", asStringUTCDatetime(u1, stringUTCDatetime1, val12));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 28: // cumLeapSecondsUnadj
				// Create the UTCDatetime
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				u1 = createUTCDatetime(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, (uint8_t)val7,
					val8, val9, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				else
				{
					(cumLeapSecondsAdj(u1.tai) == intResult1) != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED ");
					fprintf(ofp, "%s has %d leap seconds", asStringUTCDatetime(u1, stringUTCDatetime1, val12), cumLeapSecondsAdj(u1.tai));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 29: // diffUTCDatetimes
				// Create the UTCDatetimes
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				u1 = createUTCDatetime(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, (uint8_t)val7,
					val8, val9, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}

				if (val24 >= e9)
				{
					val16 = -(int8_t)val16;
					val17 = -val17;
					val24 -= e9;
				}
				u2 = createUTCDatetime(val16, val17, (uint8_t)val18, (uint8_t)val19,
					(uint8_t)val20, (uint8_t)val21, (uint8_t)val22,
					val23, val24, val25, val26, val27, val30);
				// Check for specificaiton error
				if (u2.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u2.taiInit, createUTCError, lenUTCError, failTest));
				}
				if ((u1.taiInit == 0) && (u2.taiInit == 0))
				{
					re1 = diffUTCDatetimes(u1, u2);
					(strcmp(asStringTAIRelDatetime(re1, tickElemString), underscoreToBlank(stringResult)) == 0
						!= failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s - %s = %s", asStringUTCDatetime(u1, stringUTCDatetime1, val12),
						asStringUTCDatetime(u2, stringUTCDatetime2, val27), asStringTAIRelDatetime(re1, tickElemString));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 30: // deriveUTCDatetime
				// Create the UTCDatetime
				// Determine the sign of the year from the value of val9
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				u1 = createUTCDatetime(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, (uint8_t)val7,
					val8, val9, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				// Derive the UTCDatetime
				u2 = deriveUTCDatetime(u1.tai, val10, val11, val15);

				if ((u1.taiInit == 0) && (u2.taiInit == 0))
				{
					(strcmp(asStringUTCDatetime(u1, stringUTCDatetime1, val12), asStringUTCDatetime(u2, stringUTCDatetime2, val12)) == 0
						!= failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringUTCDatetime(u1, stringUTCDatetime1, val12),
						asStringUTCDatetime(u2, stringUTCDatetime2, val12));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 31: // adjustRelTicks
				r1 = createRelTicks(0, val1, val2, val3, 0);
				r2 = adjustRelTicks(r1, val4);
				(((getSeconds(r2) == intResult1) && (getNanoseconds(r2) == intResult2)) != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				fprintf(ofp, "%s -> ", asStringTAIRelTicks(r1));
				fprintf(ofp, "%s", asStringTAIRelTicks(r2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 32: // unAdjustRelTicks
				r1 = createRelTicks(0, val1, val2, val3, 0);
				period = val4;
				r2 = adjustRelTicks(r1, adjustment[period]);
				r3 = unAdjustRelTicks(r2, period);
				(((getSeconds(r1) == getSeconds(r3)) && (getNanoseconds(r1) == getNanoseconds(r3))) != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				fprintf(ofp, "%s -> ", asStringTAIRelTicks(r1));
				fprintf(ofp, "%s -> ", asStringTAIRelTicks(r2));
				fprintf(ofp, "%s", asStringTAIRelTicks(r3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 33: // dayOfTheWeek
				// Determine the sign of the year from the value of val9
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				// Create a DateCoords
				date1.gigayear = val1;
				date1.year = val2;
				date1.month = (uint8_t)val3;
				date1.dayOfMonth = (uint8_t)val4;
				date1.calendar = val5;
				retcode = dayOfTheWeek(date1);
				// Check for specificaiton error
				if (retcode >= 8)
				{
					// DateCoords specification error
					fprintf(ofp, "%s", reportUTCConstructionError(retcode - 8, createUTCError, lenUTCError, failTest));
				}
				else
				{
					(retcode == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%02d-%s-%s is a %s.", val4, months[val3], sprintfYear(val1, val2),
						daysOfTheWeek[retcode]);
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 34: // isLeapSecondDay
				// Determine the sign of the year from the value of val9
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				retcode = isLeapSecondDay(val1, val2, (uint8_t)val3, (uint8_t)val4);
				(retcode == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				fprintf(ofp, "%02d-%s-%s is%sa leap second day.", val4, months[val3], sprintfYear(val1, val2),
					not[retcode]);
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 35: // createUTCDatetimeFromDayFrac
				// Create UTCDatetime
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				// Check for leap second day
				denom = 86400.;
				if (isLeapSecondDay(val1, val2, val3, val4)) denom = 86401.;
				dayFrac = ((double)(val5 * 3600 + val6 * 60 + val7) + (double)(val8) / 1e9 + (double)(val9) / 1e18) / denom;
				u1 = createUTCDatetimeFromDayFrac(val1, val2, (uint8_t)val3, (uint8_t)val4,
					dayFrac, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				else
				{
					// Create u2 from the integer elements and check for match
					u2 = createUTCDatetime(val1, val2, val3, val4, val5, val6, val7, val8, val9, val10,
						val11, val12, val15);
					(strcmp(asStringUTCDatetime(u1, stringUTCDatetime1, val12), asStringUTCDatetime(u2, stringUTCDatetime2, val12)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringUTCDatetime(u1, stringUTCDatetime1, val12), asStringUTCDatetime(u2, stringUTCDatetime2, val12));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 36: // createUTCDatetimeFromSecondFrac
				// Create UTCDatetime
				// Determine the sign of the year from the value of intResult1
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
					val9 -= e9;
				}
				secondFrac = (double)(val7)+(double)(val8) / 1e9 + (double)(val9) / 1e18;
				u1 = createUTCDatetimeFromSecondFrac(val1, val2, (uint8_t)val3, (uint8_t)val4,
					(uint8_t)val5, (uint8_t)val6, secondFrac, val10, val11, val12, val15);
				// Check for specificaiton error
				if (u1.taiInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportUTCConstructionError(u1.taiInit, createUTCError, lenUTCError, failTest));
				}
				else
				{
					// Create u2 from the integer elements and check for match
					u2 = createUTCDatetime(val1, val2, val3, val4, val5, val6, val7, val8, val9, val10,
						val11, val12, val15);
					(strcmp(asStringUTCDatetime(u1, stringUTCDatetime1, val12), asStringUTCDatetime(u2, stringUTCDatetime2, val12)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringUTCDatetime(u1, stringUTCDatetime1, val12), asStringUTCDatetime(u2, stringUTCDatetime2, val12));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 37: // createTAIRelDatetimeFromDayFrac
				// Create TAIRelDatetime
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				daysPlusFrac = (double)(val1)*1e9 + (double)val2 + (double)(val5) / 24. + (double)(val6) / 1440. + (double)(val7) / 86400. +
					(double)(val8) / 86400. / 1e9 + (double)(val9) / 86400. / 1e18;
				if (isNegative) daysPlusFrac = -daysPlusFrac;
				re1 = createTAIRelDatetimeFromDayFrac(daysPlusFrac, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}
				else
				{
					// Create re2 from the integer elements and check for match
					re2 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, re1.precision, val11);
					(strcmp(asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 38: // createTAIRelDatetimeFromSecondFrac
				// Create TAIRelDatetime
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				secondsPlusFrac = (double)(val1)*1e9*86400. + (double)(val2)*86400. + (double)(val5)*3600. + (double)(val6)*60. + (double)(val7)+
					(double)(val8) / 1e9 + (double)(val9) / 1e18;
				if (isNegative) secondsPlusFrac = -secondsPlusFrac;
				re1 = createTAIRelDatetimeFromSecondFrac(secondsPlusFrac, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// UTCDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}
				else
				{
					// Create re2 from the integer elements and check for match
					re2 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, re1.precision, val11);
					(strcmp(asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 39: // deriveTAIRelDatetime
				// Create the TAIRelDatetime
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}
				// Derive the TAIRelDatetime
				re2 = deriveTAIRelDatetime(re1.relTicks, val10, val11);

				// Check for specificaiton error
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createUTCError, lenUTCError, failTest));
				}
				if ((re1.relInit == 0) && (re2.relInit == 0))
				{
					(strcmp(asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 40: // addRelTicks64
				// Create the two values
				r164 = createRelTicks64(val1, val2, val3, val4, val9);
				r264 = createRelTicks64(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r364 = addRelTicks64(r164, r264);
				(strcmp(readableNumberString(asStringTAIRelTicks64(r364), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks64(r164), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks64(r264), readableString2),
					readableNumberString(asStringTAIRelTicks64(r364), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 41: // addRelTicks32
				// Create the two values
				r132 = createRelTicks32(val1, val2, val3, val4, val9);
				r232 = createRelTicks32(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r332 = addRelTicks32(r132, r232);
				(strcmp(readableNumberString(asStringTAIRelTicks32(r332), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks32(r132), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks32(r232), readableString2),
					readableNumberString(asStringTAIRelTicks32(r332), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 42: // addRelTicks
				// Create the two values
				r1 = createRelTicks(val1, val2, val3, val4, val9);
				r2 = createRelTicks(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r3 = addRelTicks(r1, r2);
				(strcmp(readableNumberString(asStringTAIRelTicks(r3), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks(r1), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks(r2), readableString2),
					readableNumberString(asStringTAIRelTicks(r3), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 43: // subtractRelTicks64
				// Create the two values
				r164 = createRelTicks64(val1, val2, val3, val4, val9);
				r264 = createRelTicks64(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r364 = subtractRelTicks64(r164, r264);
				(strcmp(readableNumberString(asStringTAIRelTicks64(r364), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks64(r164), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks64(r264), readableString2),
					readableNumberString(asStringTAIRelTicks64(r364), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 44: // subtractRelTicks32
				// Create the two values
				r132 = createRelTicks32(val1, val2, val3, val4, val9);
				r232 = createRelTicks32(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r332 = subtractRelTicks32(r132, r232);
				(strcmp(readableNumberString(asStringTAIRelTicks32(r332), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks32(r132), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks32(r232), readableString2),
					readableNumberString(asStringTAIRelTicks32(r332), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 45: // subtractRelTicks
				// Create the two values
				r1 = createRelTicks(val1, val2, val3, val4, val9);
				r2 = createRelTicks(val5, val6, val7, val8, val10);
				// Run the function and check the result
				r3 = subtractRelTicks(r1, r2);
				(strcmp(readableNumberString(asStringTAIRelTicks(r3), readableString3), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%s %s %s = %s", readableNumberString(asStringTAIRelTicks(r1), readableString1), functionName,
					readableNumberString(asStringTAIRelTicks(r2), readableString2),
					readableNumberString(asStringTAIRelTicks(r3), readableString3));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 46: // intMultTAIRelTicks
				// Create the relative tick value
				r1 = createRelTicks(val1, val2, val3, val4, val9);
				// Perform the multiplication
				r2 = intMultTAIRelTicks(val5, r1);
				(strcmp(readableNumberString(asStringTAIRelTicks(r2), readableString1), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
				// Print the three values
				fprintf(ofp, "%d x %s = %s", val5, readableNumberString(asStringTAIRelTicks(r1), readableString1),
					readableNumberString(asStringTAIRelTicks(r2), readableString2));
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 47: // intMultRelDatetime
				// Create the TAIRelDatetime
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}
				// Perform the multiplication
				re2 = intMultRelDatetime(val3, re1);
				// Check for overflow
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createTAIRelError, lenTAIRelError, failTest));
				}

				if (re1.relInit == 0 && re2.relInit == 0)
				{
					(strcmp(asStringTAIRelDatetime(re2, tickElemString), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%d x %s = %s", val3, asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 48: // addRelDatetimes
				// Create the TAIRelDatetimes
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}

				// Determine the sign from the value of val9
				isNegative = val24 >= e9;
				if (val24 >= e9) val24 -= e9;
				re2 = createTAIRelDatetime(val16, val17, val20, val21, val22, val23, val24, isNegative, val25, val26);
				// Check for specificaiton error
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createUTCError, lenUTCError, failTest));
				}

				if ((re1.relInit == 0) && (re2.relInit == 0))
				{
					re3 = addRelDatetimes(re1, re2);
					(strcmp(asStringTAIRelDatetime(re3, tickElemString), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s + %s = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2),
						asStringTAIRelDatetime(re3, tickElemString3));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 49: // subtractRelDatetimes
				// Create the TAIRelDatetimes
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}

				// Determine the sign from the value of val9
				isNegative = val24 >= e9;
				if (val24 >= e9) val24 -= e9;
				re2 = createTAIRelDatetime(val16, val17, val20, val21, val22, val23, val24, isNegative, val25, val26);
				// Check for specificaiton error
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createUTCError, lenUTCError, failTest));
				}

				if ((re1.relInit == 0) && (re2.relInit == 0))
				{
					re3 = subtractRelDatetimes(re1, re2);
					(strcmp(asStringTAIRelDatetime(re3, tickElemString), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s + %s = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2),
						asStringTAIRelDatetime(re3, tickElemString3));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 50: // ratioRelDatetimes
				// Create the TAIRelDatetimes
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}

				// Determine the sign from the value of val9
				isNegative = val24 >= e9;
				if (val24 >= e9) val24 -= e9;
				re2 = createTAIRelDatetime(val16, val17, val20, val21, val22, val23, val24, isNegative, val25, val26);
				// Check for specificaiton error
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createUTCError, lenUTCError, failTest));
				}

				if ((re1.relInit == 0) && (re2.relInit == 0))
				{
					// Calculate the ratio
					rr = ratioRelDatetimes(re1, re2);
					// Check the division by multiplying
					re3 = doubleMultRelDatetime(rr, re2);
					(strcmp(asStringTAIRelDatetime(re3, tickElemString), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%s / %s * %g = %s", asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2),
						rr.value, asStringTAIRelDatetime(re3, tickElemString3));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 51: // doubleMultRelDatetime
				// Create the TAIRelDatetime
				// Determine the sign from the value of val9
				isNegative = val9 >= e9;
				if (val9 >= e9) val9 -= e9;
				re1 = createTAIRelDatetime(val1, val2, val5, val6, val7, val8, val9, isNegative, val10, val11);
				// Check for specificaiton error
				if (re1.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re1.relInit, createTAIRelError, lenTAIRelError, failTest));
				}
				// Perform the multiplication
				dmult = (double)val3 + (double)(val4) / 1.e9;
				rr.value = dmult;
				rr.precision = dmult/9e15;
				rr.uncertainty = 1;
				re2 = doubleMultRelDatetime(rr, re1);
				// Check for overflow
				if (re2.relInit != 0)
				{
					// TAIRelDatetime specification error
					fprintf(ofp, "%s", reportTAIRelConstructionError(re2.relInit, createTAIRelError, lenTAIRelError, failTest));
				}

				if (re1.relInit == 0 && re2.relInit == 0)
				{
					(strcmp(asStringTAIRelDatetime(re2, tickElemString), underscoreToBlank(stringResult)) == 0 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "%g x %s = %s", dmult, asStringTAIRelDatetime(re1, tickElemString), asStringTAIRelDatetime(re2, tickElemString2));
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 52: // dayOfMonthFromWeekdayRule
					 // Determine the sign of the year from the value of val9
				/*
					val1: |gigayears|
					val2: |years|
					val3: month - 1 to 12
					val4: afterDayOfMonth - 1 to 30
					val5: weekNumber - 1 to 6
					val6: dayOfTheWeek - 0 to 6
					val7: calendar
					val9: flag indicating the sign of the gigayear and year > 1e9 == negative
				*/
				if (val9 >= e9)
				{
					val1 = -(int8_t)val1;
					val2 = -val2;
				}
				retcode = dayOfMonthFromWeekdayRule(val1, val2, (uint8_t)val3, (uint8_t)val4, (uint8_t)val5,
					(uint8_t)val6, val7);
				// Check for specification error
				if (retcode == 0 && !failTest)
				{
					// Input specification error
					fprintf(ofp, "FAILED Invalid input specification");
				}
				else
				{
					(retcode == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					// First print for Last week
					if (val5 == 6)
					{
						fprintf(ofp, "The last %s of %s-%s is %d.", daysOfTheWeek[val6], months[val3],
							sprintfYear(val1, val2), retcode);
					}
					else
					{
						if (val4 == 0)
						{
							fprintf(ofp, "The %s %s of %s-%s is %d.", ordinals[val5 - 1], daysOfTheWeek[val6],
								months[val3], sprintfYear(val1, val2), retcode);
						}
						else
						{
							fprintf(ofp, "The %s %s of %s-%s after %d is %d.", ordinals[val5 - 1], daysOfTheWeek[val6],
								months[val3], sprintfYear(val1, val2), val4, retcode);

						}
					}
				}
				if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
				break;
			case 53: //firstGregorianDate
					 // Determine the sign of the year from the value of val9
					 /*
					 val7: calendar - 0, 1 or encoded last Julian date
					 intResult1: the first Gregorian date using calendar convention
					 */
				date1 = firstGregorianDate(val1);
				// Check for specification error
				if (((date1.month == 0) || (date1.dayOfMonth == 0)) && !failTest)
				{
					// Input specification error
					fprintf(ofp, "FAILED Invalid input specification");
				}
				else
				{
					cal = date1.year * 10000 + date1.month * 100 + date1.dayOfMonth;
					(cal == intResult1 != failTest ? fprintf(ofp, "PASSED ") : fprintf(ofp, "FAILED "));
					fprintf(ofp, "When the last Julian date is %d, the first Gregorian date is %02d/%02d/%d.",
						val1, date1.month, date1.dayOfMonth, date1.year);
					if (strcmp(description, "NA") != 0) fprintf(ofp, " [%s]", underscoreToBlank(description));
					break;
				}
			}
		}
		else
		{
			fprintf(ofp, "Function %s not found!", functionName);
		}
	}

	// Close the files
	fclose(ifp);
	fclose(ofp);
	fclose(tzp);

	// Reopen the output file and search for failures
	fopen_s(&ofp, outputFilename, readmode);
	numFails = 0;
	testCount = 0;
	printf("\n");
	while (fgets(passOrFail,500,ofp) != NULL)
	{
		testCount++;
		if (strncmp(passOrFail, "FAILED",6) == 0)
		{
			numFails++;
			printf("Test %d failed.\n", testCount);
		}
	}
	printf("\nTotal number of failures: %d", numFails);
	fclose(ofp);

	/***** Introduce the notion of a calendar day, hour and minute.  To move forward and backwards calendar days requires a calender.
			We could also define tick days as 86400 seconds. We also can have TAI day, hour and minute.  We can differentiate move
			forward as a calendar concept vs add time as a TAI concept*******/
 	getchar();
	return 0;

}
