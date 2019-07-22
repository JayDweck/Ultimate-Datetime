# JuliaCon 2019 Demonstration
if (isdir("Z:"))
    include("Z:\\Ultimate Datetime Datatype\\Ultimate Datetime Git\\Ultimate-Datetime\\Julia Integration\\UltimateDT.jl")
elseif (isdir("C:"))
    include("C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\Ultimate Datetime\\Julia Integration\\UltimateDT.jl")
end
# Create a struct to contain code fragments
struct CodeFragment
    description::String
    expression::Expr
end
# Create a function to describe then demonstrate a code fragment
function demo_code(codeFragment::CodeFragment)
    if (codeFragment.description != "")
        printstyled(stdout, "\n *** ", codeFragment.description, " ***\n", bold=true, color=:light_green)
    end
    # The replace functions removes comment lines inserted by the system
    print(stdout, replace(string(codeFragment.expression), r"#.*#\n" => ""), "  ")
    if eval(codeFragment.expression) != nothing
        printstyled(stdout, eval(codeFragment.expression), color=:light_cyan)
    end
    readline() # Wait for a carraige return
    nothing
end
# Use the Unicode plotting package
using UnicodePlots
# Create a function to create the vector of vectors required to generate a boxplot
#   comparing two UncertainFloat64s.
function boxCompareUF(uf1::UncertainFloat64, uf2::UncertainFloat64)
    val1 = uf1.value - uf1.precision*uf1.uncertainty
    val2 = uf1.value - uf1.precision*uf1.uncertainty
    val3 = uf1.value
    val4 = uf1.value + uf1.precision*uf1.uncertainty
    val5 = uf1.value + uf1.precision*uf1.uncertainty
    val6 = uf2.value - uf2.precision*uf2.uncertainty
    val7 = uf2.value - uf2.precision*uf2.uncertainty
    val8 = uf2.value
    val9 = uf2.value + uf2.precision*uf2.uncertainty
    val10 = uf2.value + uf2.precision*uf2.uncertainty
    return [[val1,val2,val3,val4,val5],[val6,val7,val8,val9,val10]]
end
# Create an array with strings for all of the comparison operators
compops = ["==","!=","==′","!=′","==″","!=″","==‴","!=‴",">","<=″",
           ">′","<=′",">″","<=",">=","<″",">=′","<′",">=″","<"]
# Create an array with descriptions of the operators and print them in boxAndCompare
compopsDesc = ["Must equal", "May not equal", "Has same midpoint as", "Does not have same midpoint as",
               "May equal", "Cannot equal", "Has same range as", "Does not have same range as",
               "Must be greater than", "Can be less than or equal to", "Has larger midpoint than",
               "Has midpoint less than or equal to that of", "Can be greater than", "Must be less than or equal to",
               "Must be greater than or equal to", "Can be less than", "Has midpoint greater than or equal to that of",
               "Has smaller midpoint than", "Can be greater than or equal to", "Must be less than"
               ]
# Create a function to draw a box plot and display the results of the comparisons
# uf1 and uf2 need to be in the global scope so that the eval in boxAndCompare receives the proper values
uf1 = UncertainFloat64(1.,p=1,u=1)
uf2 = uf1
function boxAndCompare(uf1t::UncertainFloat64, uf2t::UncertainFloat64)
    # Need to pass the global variables to eval
    global uf1 = uf1t
    global uf2 = uf2t
    # When used in an expression to be evaluated, boxplot works if it is the last line in
    #  the expression.  Otherwise, must use show(boxplot).
    show(boxplot([string(uf1), string(uf2)], boxCompareUF(uf1,uf2), border=:none))
    print(stdout,"\n")
    for i = 1:20
        code = Meta.parse("uf1" * compops[i] * "uf2")
        print(stdout, "\n", rpad(string(code),11), "   ")
        printstyled(stdout, rpad(compopsDesc[i],45), "  ", color=:light_green)
        if (eval(code) == true)
            printstyled(stdout, eval(code), color=:light_cyan)
        else
            printstyled(stdout, eval(code), color=:light_red)
        end
    end
    return nothing
end
# Create a function to do a simple comparison of two LocalCalCoordsDT
lcc1 = LocalCalCoordsDT(7, 23, 2019, "America/New_York", 2; cal=0, h=10)
lcc2 = lcc1
simplecomps = [:(lcc1 == lcc2), :(lcc1 != lcc2), :(lcc1 > lcc2), :(lcc1 <= lcc2), :(lcc1 < lcc2), :(lcc1 >= lcc2)]
function simpleCompare(lcc1t::LocalCalCoordsDT, lcc2t::LocalCalCoordsDT)
    # Need to pass the global variables to eval
    global lcc1 = lcc1t
    global lcc2 = lcc2t
    print(stdout,"\n")
    for i=1:6
        code = simplecomps[i]
        print(stdout, "\n", rpad(string(code),12), "   ")
        if (eval(code) == true)
            printstyled(stdout, eval(code), color=:light_cyan)
        else
            printstyled(stdout, eval(code), color=:light_red)
        end
    end
end
# Create an array to contain the code fragments
code = Array{CodeFragment,1}()
# Uncertain Floats
# Create an Uncertain Float with default precision and uncertainty
push!(code,CodeFragment("Uncertain float with default precision and uncertainty",:(UncertainFloat64(1005.25))))
# Show change in default precision and uncertainty with magnitude
push!(code,CodeFragment("Default precision and uncertainty change with magnitude",:(UncertainFloat64(5005.25))))
# Create 2 uncertain floats to demonstrate arithmetic functions
push!(code,CodeFragment("Create uf1 with specified precision and uncertainty",:(uf1 = UncertainFloat64(10,p=1,u=2))))
push!(code,CodeFragment("Create uf2 with specified precision and uncertainty",:(uf2 = UncertainFloat64(8,p=1,u=1))))
push!(code,CodeFragment("Addition - add absolute uncertainties, use minimum absolute precision",:(uf1 + uf2)))
push!(code,CodeFragment("Subtraction - add absolute uncertainties, use minimum absolute precision",:(uf1 - uf2)))
push!(code,CodeFragment("Create uf3 with specified precision and uncertainty",:(uf3 = UncertainFloat64(100,p=1,u=2))))
push!(code,CodeFragment("Multiplication - add fractional uncertainties, use minimum fractional precision",:(uf3 * uf2)))
push!(code,CodeFragment("Division - add fractional uncertainties, use minimum fractional precision",:(uf3 / uf2)))
# Exact values have a precision of 1 and uncertainy of 0 (i.e., as Integers)
push!(code,CodeFragment("Create uf1 with specified precision and uncertainty",:(uf1 = UncertainFloat64(1005.25,p=.01,u=2))))
push!(code,CodeFragment("Create uf2 as an exact value",:(uf2 = UncertainFloat64(4,p=1,u=0))))
push!(code,CodeFragment("Multiply the 2 values.  Uf1 is only contributor to p and u.",:(uf1 * uf2)))
# Compare with integber result
push!(code,CodeFragment("Multiply an uncertain value by an integer.  Uf1 is only contributor to p and u.",:(4 * uf1)))
# Zero values require special handling
push!(code,CodeFragment("Zero value with an uncertainty of 1.",:(uf1 = UncertainFloat64(0,p=1,u=1))))
push!(code,CodeFragment("Zero value with an uncertainty of 1 billion",:(uf2 = UncertainFloat64(0,p=1e9,u=1))))
push!(code,CodeFragment("Non-zero value with a 90% uncertainty",:(uf3 = UncertainFloat64(10.,p=1,u=9))))
push!(code,CodeFragment("Multiplication multiplies the uncertainty by the largest possible value of the other factor.",:(uf3 * uf1)))
push!(code,CodeFragment("Division increases the uncertainty by (1 + fractional uncertainty of the denominator).",:(uf2 / uf3)))
# Demonstrate range comparisons
push!(code,CodeFragment("Range based comparison operators with overlapping values",
    :(boxAndCompare(UncertainFloat64(8.,p=1,u=2),UncertainFloat64(9.,p=1,u=2)))))
push!(code,CodeFragment("Range based comparison operators with exact values",
    :(boxAndCompare(UncertainFloat64(8,p=1,u=0),UncertainFloat64(8,p=1,u=0)))))
push!(code,CodeFragment("Range based comparison operators with disjoint values",
    :(boxAndCompare(UncertainFloat64(5,p=1,u=2),UncertainFloat64(10,p=1,u=2)))))
push!(code,CodeFragment("Range based comparison operators with a single overlapping value",
    :(boxAndCompare(UncertainFloat64(5,p=1,u=2),UncertainFloat64(9,p=1,u=2)))))
push!(code,CodeFragment("Range based comparison operators with the same range",
    :(boxAndCompare(UncertainFloat64(9,p=1,u=2),UncertainFloat64(9,p=1,u=2)))))
# Demonstrate that relative time is different from UTCDatetime
push!(code,CodeFragment("Create a UTCDatetime with default precision and uncertainty.",
    :(utc1 = UTCDatetime(y=2019,m=7,d=23,h=11,min=45))))
push!(code,CodeFragment("Create a 2nd UTCDatetime with default precision and uncertainty.",
    :(utc2 = UTCDatetime(y=2019,m=7,d=23,h=10,min=45))))
# UTC Datetime Formatting
push!(code,CodeFragment("Formatting is automatic based on precision.",
    :(for i = -18:15;
        utc = UTCDatetime(gy=99,y=999999999,m=12,d=31,h=23,min=59,s=59,ns=999999999,as=999999999,p=i,u=1);
        printstyled(stdout,"\n", utc,color=:light_cyan);
      end)))
push!(code,CodeFragment("Subtract the 2 UTCDatetimes to produce a relative datetime.",:(rel1 = utc1 - utc2)))
push!(code,CodeFragment("Relative datetimes can be positive or negative.",:(rel2 = utc2 - utc1)))
# Show allowed operations on relative datetimes
push!(code,CodeFragment("Create a UTCDatetime with nanosecond precision and uncertainty.",
    :(utc1 = UTCDatetime(y=2019,m=7,d=23,h=11,min=45,p=-9,u=1))))
push!(code,CodeFragment("Create a 2nd UTCDatetime with nanosecond precision and uncertainty.",
    :(utc2 = UTCDatetime(y=2019,m=7,d=23,h=10,min=45,p=-9,u=1))))
push!(code,CodeFragment("Subtract the 2 UTCDatetimes to produce a relative datetime.",:(rel1 = utc1 - utc2)))
push!(code,CodeFragment("Relative datetimes can be multiplied by integers and floating point numbers.",:(2*rel1 + 1.5*rel1)))
push!(code,CodeFragment("A relative datetime can be divided by another relative datetime.",:( x = rel1 / (2*rel1 + 1.5*rel1))))
push!(code,CodeFragment("The result of this division is an UncertianFloat64 (which is the original reason the type was created.)!", :(typeof(x))))
push!(code,CodeFragment("When the UncertainFloat64 is multiplied by the denominator, the precision is restored with the propogated uncertainty.",
    :(x * (2*rel1 + 1.5*rel1))))
# Demonstrate Julian to Gregorian transition
push!(code,CodeFragment("Create a UTCDatetime just before the earliest transtion to the Gregorian calendar.",
    :(utcj = UTCDatetime(y=1582,m=10,d=4,h=23,min=0,p=0,u=1))))
push!(code,CodeFragment("Attempt to create a UTCDatetime during the skipped period.",
    :(utcs = UTCDatetime(y=1582,m=10,d=5,h=1,min=0,p=0,u=1))))
push!(code,CodeFragment("Create a UTCDatetime just after the earliest transition to the Gregorian calendar.",
    :(utcg = UTCDatetime(y=1582,m=10,d=15,h=1,min=0,p=0,u=1))))
push!(code,CodeFragment("Very little time elapsed between those two dates!",
    :(utcg - utcj)))
# Demonstrate leap seconds
push!(code,CodeFragment("Leap seconds occur at midnight UTC, either on June 30 or December 31.",
    :(lsminus1 = UTCDatetime(y=2015,m=6,d=30,h=23,min=59,s=59))))
push!(code,CodeFragment("This is the actual leap second.  Notice the seconds field.",
    :(ls = UTCDatetime(y=2015,m=6,d=30,h=23,min=59,s=60))))
push!(code,CodeFragment("This is the next second.",
    :(lsplus1 = UTCDatetime(y=2015,m=7,d=1,h=0,min=0,s=0))))
push!(code,CodeFragment("The arithmetic works.",
    :(lsplus1 - lsminus1)))
push!(code,CodeFragment("Only minutes containing leap seconds have a 60th second.",
    :(ls = UTCDatetime(y=2015,m=6,d=30,h=23,min=58,s=60))))
# Explore time zones
push!(code,CodeFragment("Prior to the late 19th century, time zones were based on local mean time, a type of solar time.",
    :(lchi = LocalCalCoordsDT(5, 15, 1870, "America/Chicago", 2; cal=0, h=4))))
push!(code,CodeFragment("The offset from UTC (GMT, at the time) was based solely on the longitude of a location.",
    :(translate(lchi,f=0))))
push!(code,CodeFragment("When this Chicago time is translated to a New York time, the difference is 54:34.4, not 1 hour!",
    :(lny = translate(lchi,tz = "America/New_York"))))
push!(code,CodeFragment("In the 20th century, we get the expected result.",
    :(lchi = LocalCalCoordsDT(5, 15, 1950, "America/Chicago", 2; cal=0, h=4))))
push!(code,CodeFragment("When this Chicago time is translated to a New York time, the difference is 1 hour.",
    :(lny = translate(lchi,tz = "America/New_York"))))
push!(code,CodeFragment("Daylight savings time began during World War I.  The Chicago time above is during daylight savings.",
    :(lny = translate(lchi,f=1))))
push!(code,CodeFragment("The transition to daylight savings time in 1950 took place on April 30th, the last Sunday in the month.",
    :(lchi = LocalCalCoordsDT(4, 30, 1950, "America/Chicago", 2; h=1, min=59))))
push!(code,CodeFragment("1:59 wall time is equivalent to 1:59 standard time.", :(translate(lchi,f=1))))
push!(code,CodeFragment("2:01 wall time does not exist!",
    :(lchi = LocalCalCoordsDT(4, 30, 1950, "America/Chicago", 2; h=2, min=1))))
push!(code,CodeFragment("2:01 standard time does exist...",
    :(lchi = LocalCalCoordsDT(4, 30, 1950, "America/Chicago", 1; h=2, min=1))))
push!(code,CodeFragment("... and is equivalent to 3:01 wall time.",:(translate(lchi,f=2))))
push!(code,CodeFragment("The transition back to standard time in 1950 took place on September 24th, the last Sunday in the month.",
    :(lchi = LocalCalCoordsDT(9, 24, 1950, "America/Chicago", 2; h=0, min=59))))
push!(code,CodeFragment("0:59 wall time is equivalent to 23:59 standard time.", :(translate(lchi,f=1))))
push!(code,CodeFragment("1:01 wall time is ambiguous, as it occurs twice.",
    :(lchi = LocalCalCoordsDT(9, 24, 1950, "America/Chicago", 2; h=1, min=1))))
push!(code,CodeFragment("1:01 standard time is not ambiguous...",
    :(lchi = LocalCalCoordsDT(9, 24, 1950, "America/Chicago", 1; h=1, min=1))))
push!(code,CodeFragment("... and is equivalent to 1:01 wall time, after the transition.",:(translate(lchi,f=2))))
push!(code,CodeFragment("Wall times must be disambiguated during ambiguous periods.",
    :(lchi = LocalCalCoordsDT(9, 24, 1950, "America/Chicago", 2; h=1, min=1, ba=1))))
push!(code,CodeFragment("1:01 before the transition is equivalent to 00:01 standard time.", :(translate(lchi,f=1))))
push!(code,CodeFragment("Daylight savings rules have changed over time.  Prior to 2007, March 15th was standard time in New York.",
    :(lny = LocalCalCoordsDT(3, 15, 2006, "America/New_York", 2; h=12))))
push!(code,CodeFragment("ie, standard and wall times were the same.", :(translate(lny,f=1))))
push!(code,CodeFragment("Beginning in 2007, daylight savings time starts the second Sunday in March, not the first Sunday in April.",
    :(lny = LocalCalCoordsDT(3, 15, 2007, "America/New_York", 2; h=12))))
push!(code,CodeFragment("Standard time is 1 hour less than wall time.", :(translate(lny,f=1))))
# Revisit calendars
push!(code,CodeFragment("Construct a Gregorian date in the proleptic America/New_York time zone.",
    :(lccg = LocalCalCoordsDT(10, 15, 1582, "America/New_York", 2; cal=0, h=10))))
push!(code,CodeFragment("The Julian calendar continued on - the last country to adopt the Gregorian calendar was Greence in 1923.",
    :(lccj = LocalCalCoordsDT(10, 5, 1582, "America/New_York", 2; cal=1, h=10))))
push!(code,CodeFragment("For every Gregorian datetime, there is a corresponding Julian datetime.",
    :(lccg == lccj)))
push!(code,CodeFragment("Simple comparisons work.", :(simpleCompare(lccg, lccj))))
push!(code,CodeFragment("Construct a later Gregorian date.",
    :(lccg2 = LocalCalCoordsDT(10, 16, 1582, "America/New_York", 2; cal=0, h=10))))
push!(code,CodeFragment("Compare.", :(simpleCompare(lccg2, lccj))))
#***** Show Swedish calendar
map(demo_code, code)
nothing
