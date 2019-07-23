# Implementation of Ultimate Datetime
# module UltimateDateTime
module UltimateDatetime # Clears definitions
# The following is necessary to overload the operators for the new datatypes
using Base
using Nullables
using Libdl
import Base: ==, !=, >, <, >=, <=, +, -, >>>, >>, <<
import Base.show
import Base.*
import Base.promote_rule  # Need to import this to add promotoe_rules
export ==′, !=′, ==″, !=″, ==‴, !=‴, >′, >″, >=′, >=″, <′, <″, <=′, <=″
export TAITicks, TAIRelTicks, UTCDatetime, TAIRelDatetime, UncertainFloat64, DateCoords
export UTCOffset, UTCDatetime, CalCoords, LocalCalCoordsDT
export asJulian, asGregorian, asCalendar
export showDatetime, translate
# Open the C libraries
# Open preferentially on the Z: drive, which will be the case when working at home
# UDTLib = Libdl.dlopen("W:\\Shared\\Users\\Jay\\Ultimate Datetime Datatype\\Ultimate Datetime\\x64\\Debug\\UltimateDatetimeDLL")
if isdir("Z:")
  UDTLib = Libdl.dlopen("Z:\\Ultimate Datetime Datatype\\Ultimate Datetime Git\\Ultimate-Datetime\\x64\\Debug\\UltimateDatetimeDLL")
  LocLib = Libdl.dlopen("Z:\\Ultimate Datetime Datatype\\Ultimate Datetime Git\\Ultimate-Datetime\\x64\\Debug\\LocalDatetimeDLL")
# Check for local directory on Windows machine
elseif isdir("C:")
  UDTLib = Libdl.dlopen("C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\Ultimate Datetime\\x64\\Debug\\UltimateDatetimeDLL")
  LocLib = Libdl.dlopen("C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\Ultimate Datetime\\x64\\Debug\\LocalDatetimeDLL")
# Check for OS / X
elseif isdir("/Volumes")
  UDTLib = Libdl.dlopen("/Volumes/Shared/Ultimate Datetime Datatype/Ultimate Datetime/Debug/libUltimateDatetimeDLL")
  LocLib = Libdl.dlopen("/Volumes/Shared/Ultimate Datetime Datatype/Ultimate Datetime/Debug/libLocalDatetimeDLL")
end
# Define datatypes
# Tick count from epoch either uses 2 @ 64-bit unsigned integers or 4 @ 32-bit unsigned integers
#   based on how UltimateDatetimeDLL was built
numTickBits = ccall(Libdl.dlsym(UDTLib,:getTickBits), Int, ())
maxTZNameLength = ccall(Libdl.dlsym(LocLib,:getMaxTZNameLength), Int, ())
if(numTickBits == 64)
    struct TAITicks
        seconds::UInt64
        attoseconds::UInt64
    end
elseif(numTickBits == 32)
    struct TAITicks
        gigaseconds::UInt32
        seconds::UInt32
        nanoseconds::UInt32
        attoseconds::UInt32
    end
else
    println("Error - unable to retrieve size of tick fields from UltimateDatetimeDLL")
end
# Constructor
function TAITicks(; gs = 0, s = 0, ns = 0, as = 0)
    return (ccall(Libdl.dlsym(UDTLib,:createTicks), TAITicks,
                (UInt32, UInt32, UInt32, UInt32), gs, s, ns, as))
end
function ==(x::TAITicks, y::TAITicks)
  return (ccall(Libdl.dlsym(UDTLib,:isEqualTicks), Bool,
              (TAITicks, TAITicks), x, y))
end
function !=(x::TAITicks, y::TAITicks)
    return !(x == y)
end
function >(x::TAITicks, y::TAITicks)
  return (ccall(Libdl.dlsym(UDTLib,:isGreaterTicks), Bool,
                  (TAITicks, TAITicks),x,y))
end
function <=(x::TAITicks, y::TAITicks)
    return !(x > y)
end
function <(x::TAITicks, y::TAITicks)
  return (ccall(Libdl.dlsym(UDTLib,:isLessTicks), Bool,
                  (TAITicks, TAITicks),x,y))
end
function >=(x::TAITicks, y::TAITicks)
    return !(x < y)
end
# Pretty print TAITicks
function show(io::IO, z::TAITicks)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringTAITicks), Cstring,
                    (TAITicks,),z)
    print(io, unsafe_string(buf))
end
# Relative ticks using 2 @ 64-bit unsigned integers or 4 @ 32-bit unsigned integers
#   Since the maximum value for attoseconds is 1e18 -1, save bits
#   by representing negative relative tick values by adding 1e18
#   to the number of attoseconds
if(numTickBits == 64)
    struct TAIRelTicks
        seconds::UInt64
        attoseconds::UInt64
    end
elseif(numTickBits == 32)
    struct TAIRelTicks
        gigaseconds::UInt32
        seconds::UInt32
        nanoseconds::UInt32
        attoseconds::UInt32
    end
else
    println("Error - unable to retrieve size of tick fields from UltimateDatetimeDLL")
end
# Constructor
function TAIRelTicks(; gs = 0, s = 0, ns = 0, as = 0, isneg = false)
    # Convert isneg to an integer
    isneg ? ineg = 1 : ineg = 0
    return (ccall(Libdl.dlsym(UDTLib,:createRelTicks), TAIRelTicks,
                (UInt32, UInt32, UInt32, UInt32, Int), gs, s, ns, as, ineg))
end
function ==(x::TAIRelTicks, y::TAIRelTicks)
  return (ccall(Libdl.dlsym(UDTLib,:isEqualRelTicks), Bool,
                  (TAIRelTicks, TAIRelTicks),x,y))
end
function !=(x::TAIRelTicks, y::TAIRelTicks)
    return !(x == y)
end
function >(x::TAIRelTicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterRelTicks), Bool,
                    (TAIRelTicks, TAIRelTicks),x,y))
end
function <=(x::TAIRelTicks, y::TAIRelTicks)
    return !(x > y)
end
function <(x::TAIRelTicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:isLessRelTicks), Bool,
                    (TAIRelTicks, TAIRelTicks),x,y))
end
function >=(x::TAIRelTicks, y::TAIRelTicks)
    return !(x < y)
end
# Negate TAIRelTicks
function -(x::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:negateRelTicks), TAIRelTicks,
                    (TAIRelTicks, ),x))
end
# Pretty print TAIRelTicks
function show(io::IO, z::TAIRelTicks)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringTAIRelTicks), Cstring,
                    (TAIRelTicks,),z)
    print(io, unsafe_string(buf))
end
# Difference between 2 ticks
function -(x::TAITicks, y::TAITicks)
    return (ccall(Libdl.dlsym(UDTLib,:diffTicks), TAIRelTicks,
                    (TAITicks, TAITicks),x,y))
end
# Add relative ticks to ticks
function +(x::TAITicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:addRelTicksToTicks), TAITicks,
                    (TAITicks, TAIRelTicks),x,y))
end
# Add ticks to relative ticks
function +(x::TAIRelTicks, y::TAITicks)
    return (y + x)
end
# Subtract relative ticks from ticks
function -(x::TAITicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:subtractRelTicksFromTicks), TAITicks,
                    (TAITicks, TAIRelTicks),x,y))
end
# Add relative ticks to relative ticks
function +(x::TAIRelTicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:addRelTicks), TAIRelTicks,
                    (TAIRelTicks, TAIRelTicks),x,y))
end
# Subtract relative ticks from relative ticks
function -(x::TAIRelTicks, y::TAIRelTicks)
    return (ccall(Libdl.dlsym(UDTLib,:subtractRelTicks), TAIRelTicks,
                    (TAIRelTicks, TAIRelTicks),x,y))
end
# A Datetime on the UTC calendar as described in the comment below
struct UTCDatetime
    dayOfMonth::UInt8
    month::UInt8 # 1 = Jan, 2 = Feb, ... 12 = Dec
    year::Int32  #= Positive years = CE, 0 = 1BCE, -1 = 2 BCE, ... -n = n+1 BCE
				      Used a 32-bit value, even though years can go from 13e9 BCE
					  to over 100e9 CE for 32-bit Ticks and 500e9 CE for 64-bit Ticks.
					  The reason for this is to avoid creating two data strcutures,
					  one that uses a 64-bit int for years and one that uses 2 32-bit
					  ints.=#

    gigayear::Int8 #= Number of billions of years.  Using an 8-bit int limits
							the value to 127 billion years. =#
    hour::UInt8 # Calendar hour
    minute::UInt8 # Calendar minute
    second::UInt8 #= Calendar second
                        nanosecond and attosecond have been removed from the utc data
                        structure, because they are large and quickly calculable from tai =#
    precision::Int8 #= 99=unspecified, 3=day, 2=hour, 1=minute, 0=second, -1=10^-1 second,
								-n=10^-n second =#
    uncertainty::Int8 #= As a multiplier of precision - 0 = unspecified
							For example, if precision=-3 and uncertainty=10, the time
							is known to ± 10 milliseconds. =#
    futureAdjust::UInt8 #= If the value is nonzero, the tick count or datetime elements
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
							indicating no further adjustments are required.   To avoid making
                            this data structure mutable, create a copy in these situations. =#
    tai::TAITicks # The tick count associated with this UTC datetime
    taiInit::UInt16 #= Bit field indicating status of initialization and error codes
						0000h indicates initialized without error.
						Other values indicate specification errors as per ccInit
						=#
end
# UTCDatetime constructors
function UTCDatetime(; gy = 0, y = 0, m = 1, d = 1, h = 0, min = 0, s = 0, ns = 0, as = 0,
                        dfrac = 0.0, splusfrac = 0.0, p = 99, u = 0, c = 0, f = 1)
# Call appropriate constructor based on arguments specified
    # Check for valid h, m, s, ns, as specification
    if (s != 0 || ns != 0 || as != 0)
        if (dfrac != 0.0 || splusfrac != 0.0)
            println("Over specification")
            return Nullable{UTCDatetime}()
        else
            utc = (ccall(Libdl.dlsym(UDTLib,:createUTCDatetime), UTCDatetime,
                    (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8, UInt32, UInt32, Int8, Int8, UInt8, UInt8),
                    gy, y, m, d, h, min, s, ns, as, p, u, c, f))
        end
    # Check for valid day fraction specification
    elseif  (dfrac != 0)
        if (splusfrac != 0.0 || h != 0 || min != 0)
            println("Over specification")
            return Nullable{UTCDatetime}()
        else
            utc = (ccall(Libdl.dlsym(UDTLib,:createUTCDatetimeFromDayFrac), UTCDatetime,
                    (Int8, Int32, UInt8, UInt8, Float64, Int8, Int8, UInt8, UInt8),
                    gy, y, m, d, dfrac, p, u, c, f))
        end
    # Check for valid second plus fraction specification
    elseif (splusfrac != 0.0)
        utc = (ccall(Libdl.dlsym(UDTLib,:createUTCDatetimeFromSecondFrac), UTCDatetime,
                (Int8, Int32, UInt8, UInt8, UInt8, UInt8, Float64, Int8, Int8, UInt8, UInt8),
                 gy, y, m, d, h, min, splusfrac, p, u, c, f))
    else
        # All calls will give equivalent results
        utc = (ccall(Libdl.dlsym(UDTLib,:createUTCDatetime), UTCDatetime,
                (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8, UInt32, UInt32, Int8, Int8, UInt8, UInt8),
                gy, y, m, d, h, min, s, ns, as, p, u, c, f))
    end
    # Check taiInit
    if utc.taiInit != 0
        reportUTCConstructionError(stdout, utc)
        return Nullable{UTCDatetime}()
    else
        return utc
    end
end
# Construct UTCDatetime from TAITicks
function UTCDatetime(tai::TAITicks; p = 99, u = 0, f = 1)
    utc = ccall(Libdl.dlsym(UDTLib, :deriveUTCDatetime), UTCDatetime,
            (TAITicks, Int8, Int8, Int8), tai, p, u, f)
    # Check taiInit
    if utc.taiInit != 0
        reportUTCConstructionError(stdout, utc)
        return Nullable{UTCDatetime}()
    else
        return utc
    end
end
# Pretty print UTCDatetime
function show(io::IO, z::UTCDatetime)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringUTCDatetime), Cstring,
            (UTCDatetime, Ptr{UInt8}, UInt32), z, sbuf, 0)
    print(io, beautify(unsafe_string(buf)))
end
# Print a UTCDatetime on a specified calendar basis
function showWithCalendar(z::UTCDatetime, c::UInt32; io = stdout)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringUTCDatetime), Cstring,
            (UTCDatetime, Ptr{UInt8}, UInt32), z, sbuf, c)
    print(io, beautify(unsafe_string(buf)))
end
# Check validity of UTCDatetime parameters
function checkDatetimeElements(; gy = 0, y = 0, m = 1, d = 1, h = 0, min = 0, s = 0, ns = 0, as = 0,
                                p = 99, u = 0, c = 0, f = 1)
    return (ccall(Libdl.dlsym(UDTLib,:checkDatetimeElements), UInt16,
      (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8, UInt32, UInt32, Int8, Int8, UInt8, UInt8),
      gy, y, m, d, h, min, s, ns, as, p, u, c, f))
end
# Report errors found during construction of a UTCDatetime
function reportUTCConstructionError(io::IO,utc::UTCDatetime)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(UDTLib, :reportUTCConstructionError), Cstring,
            (UInt16, Ptr{UInt8}, Int, Int), utc.taiInit, sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# Get nanoseconds and attoseconds from a UTCDatetime
#   Must be computed as they are not stored in the data structure to save space
function getNano(utc::UTCDatetime)
    return ccall(Libdl.dlsym(UDTLib, :getNanosecond), UInt32,
            (TAITicks,), utc.tai)
end
#
function getAtto(utc::UTCDatetime)
    return ccall(Libdl.dlsym(UDTLib, :getAttosecond), UInt32,
            (TAITicks,), utc.tai)
end
# UTCDatetime comparison functions
function ==(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mustEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function !=(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mayNotEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function ==′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isEqualMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function !=′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isNotEqualMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function ==″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:canEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function !=″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:cannotEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function ==‴(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isEqualRangeUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function !=‴(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isNotEqualRangeUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeGreaterUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:canBeGreaterUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeLessUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isLessMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:canBeLessUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >=(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeGreaterOrEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >=′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterOrEqualMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function >=″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mayBeGreaterOrEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <=(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeLessOrEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <=′(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isLessOrEqualMidpointUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
function <=″(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:mayBeLessOrEqualUTCDatetimes), Bool,
                    (UTCDatetime, UTCDatetime),x,y))
end
# Relative datetime based on TAI ticks
#  Gigaday, day, hour, minute, second representation of
#    relative ticks.  Each element is measured in
#    TAI space, using a standard number of SI seconds,
#    e.g., day is 86400 seconds, hour is 3600 seconds.
struct TAIRelDatetime
    gigadays::UInt32
    days::UInt32
    hours::UInt8
    minutes::UInt8
    seconds::UInt8
    precision::Int8 #= 99=unspecified, 3=day, 2=hour, 1=minute, 0=second, -1=10^-1 second,
								-n=10^-n second =#
    uncertainty::Int8 #= As a multiplier of precision - 0 = unspecified
							For example, if precision=-3 and uncertainty=10, the time
							is known to ± 10 milliseconds. =#
    relTicks::TAIRelTicks
    relInit::UInt8 #= Bit field indicating status of initialization and error codes
						00h indicates initialized without error.
						Other values indicate specification errors as follows, by bit number:
						1 Relative datetime greater than max,			 2 Days >= 1e9,
						3 Hours > 23,				                     4 Minutes > 59,
						5 Seconds > 59,						             6 Nano >= 1e9,
						7 Atto >= 1e9,									 8 Invalid Precision =#
end
# TAIRelDatetime constructors
function TAIRelDatetime(; gd = 0, d = 0, h = 0, m = 0, s = 0, ns = 0, as = 0,
                        dfrac = 0.0, splusfrac = 0.0, isneg=false, p = 99, u = 0)
# Call appropriate constructor based on arguments specified
    # Check for valid h, m, s, ns, as specification
    # Convert isneg to an integer
    isneg ? ineg = 1 : ineg = 0
    if (s != 0 || ns != 0 || as != 0)
        if (dfrac != 0.0 || splusfrac != 0.0)
            println("Over specification")
            return Nullable{TAIRelDatetime}()
        else
            rel = (ccall(Libdl.dlsym(UDTLib,:createTAIRelDatetime), TAIRelDatetime,
                    (UInt16, UInt32, UInt8, UInt8, UInt8, UInt32, UInt32, Int, Int8, Int8),
                    gd, d, h, m, s, ns, as, ineg, p, u))
        end
    # Check for valid day fraction specification
    elseif  (dfrac != 0)
        if (splusfrac != 0.0 || h != 0 || m != 0)
            println("Over specification")
            return Nullable{TAIRelDatetime}()
        else
            rel = (ccall(Libdl.dlsym(UDTLib,:createTAIRelDatetimeFromDayFrac), TAIRelDatetime,
                    (Float64, Int8, Int8), dfrac, p, u))
        end
    # Check for valid second plus fraction specification
    elseif (splusfrac != 0.0)
        rel = (ccall(Libdl.dlsym(UDTLib,:createTAIRelDatetimeFromSecondFrac), TAIRelDatetime,
        (Float64, Int8, Int8), splusfrac, p, u))
    else
        # All calls will give equivalent results
        rel = (ccall(Libdl.dlsym(UDTLib,:createTAIRelDatetime), TAIRelDatetime,
                (UInt16, UInt32, UInt8, UInt8, UInt8, UInt32, UInt32, Int, Int8, Int8),
                gd, d, h, m, s, ns, as, ineg, p, u))
    end
    # Check taiInit
    if rel.relInit != 0
        reportTAIRelConstructionError(stdout, rel)
        return Nullable{TAIRelDatetime}()
    else
        return rel
    end
end
# Report errors found during construction of a TAIRelDatetime
function reportTAIRelConstructionError(io::IO,rel::TAIRelDatetime)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(UDTLib, :reportTAIRelConstructionError), Cstring,
            (UInt8, Ptr{UInt8}, Int, Int), rel.relInit, sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# Pretty print TAIRelDatetime
function show(io::IO, z::TAIRelDatetime)
    sbuf = Vector{UInt8}(undef,100)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringTAIRelDatetime), Cstring,
            (TAIRelDatetime, Ptr{UInt8}), z, sbuf)
    print(io, unsafe_string(buf))
end
# Negate TAIRelDatetime
function -(x::TAIRelDatetime)
  return (ccall(Libdl.dlsym(UDTLib,:negateRelDatetime), TAIRelDatetime,
                  (TAIRelDatetime, ),x))
end
# TAIRelDatetime comparison functions
function ==(x::TAIRelDatetime, y::TAIRelDatetime)
  return (ccall(Libdl.dlsym(UDTLib,:isEqualRelDatetimes), Bool,
                  (TAIRelDatetime, TAIRelDatetime),x,y))
end
function !=(x::TAIRelDatetime, y::TAIRelDatetime)
    return !(x == y)
end
function >(x::TAIRelDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterRelDatetimes), Bool,
                    (TAIRelDatetime, TAIRelDatetime),x,y))
end
function <=(x::TAIRelDatetime, y::TAIRelDatetime)
    return !(x > y)
end
function <(x::TAIRelDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:isLessRelDatetimes), Bool,
                    (TAIRelDatetime, TAIRelDatetime),x,y))
end
function >=(x::TAIRelDatetime, y::TAIRelDatetime)
    return !(x < y)
end
# Add 2 TAIRelDatetimes
function +(x::TAIRelDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :addRelDatetimes), TAIRelDatetime,
                    (TAIRelDatetime, TAIRelDatetime),x,y))
end
# Subtract 2 TAIRelDatetimes
function -(x::TAIRelDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :subtractRelDatetimes), TAIRelDatetime,
                    (TAIRelDatetime, TAIRelDatetime),x,y))
end
# Multiply a TAIRelDatetime by a 32-bit integer
function *(x::Int, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :intMultRelDatetime), TAIRelDatetime,
                    (Int32, TAIRelDatetime),x,y))
end
# Multiply a 32-bit integer by a TAIRelDatetimes
function *(x::TAIRelDatetime, y::Int)
    return y*x
end
# Define a struct for a Float64 with precision and uncertainty
#  Must deriver from Number so the promotion and conversion functions
#   can facilitate mixed mode arithmetic involving Integers and Floats
struct UncertainFloat64 <: Number
    value::Float64
    precision::Float64 #= Precision is 1 part per precision.  Max is 9.0073x10^15 which is
                            the limit for a double precision. =#
    uncertainty::Float64 # Actual value, not a multiplier on precision
end
# Construct an UncertainFloat64
function UncertainFloat64(x::Number; p=0., u=0.)
    return ccall(Libdl.dlsym(UDTLib,:createUncertainDouble), UncertainFloat64,
                (Float64, Float64, Float64),x, p, u)
end
# Convert an AbstractFloat to an UncertainFloat64
convert(::Type{UncertainFloat64}, x::AbstractFloat) =
     UncertainFloat64(x)
promote_rule(::Type{UncertainFloat64}, ::Type{Float16}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Float32}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Float64}) = UncertainFloat64
convert(::Type{UncertainFloat64}, x::Integer) =
     UncertainFloat64(Float64(x))
promote_rule(::Type{UncertainFloat64}, ::Type{Int8}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Int16}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Int32}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Int64}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{Int128}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{UInt8}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{UInt16}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{UInt32}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{UInt64}) = UncertainFloat64
promote_rule(::Type{UncertainFloat64}, ::Type{UInt128}) = UncertainFloat64
# Pretty print an UncertainFloat64
function show(io::IO, z::UncertainFloat64)
    cbuf = ccall(Libdl.dlsym(UDTLib,:asStringUncertainDouble), Cstring,
                    (UncertainFloat64, ),z)
    print(io, beautify(unsafe_string(cbuf)))
    return
end
# Negate UncertainFloat64
function -(x::UncertainFloat64)
  return (ccall(Libdl.dlsym(UDTLib,:negateUncertainDouble), UncertainFloat64,
                  (UncertainFloat64, ),x))
end
# UncertainFloat64 comparison functions
function ==(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mustEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function !=(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mayNotEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function ==′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isEqualMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function !=′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isNotEqualMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function ==″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:canEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function !=″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:cannotEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function ==‴(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isEqualRangeUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function !=‴(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isNotEqualRangeUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeGreaterUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:canBeGreaterUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeLessUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isLessMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:canBeLessUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >=(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeGreaterOrEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >=′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterOrEqualMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function >=″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mayBeGreaterOrEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <=(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mustBeLessOrEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <=′(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:isLessOrEqualMidpointUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
function <=″(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib,:mayBeLessOrEqualUncertainDoubles), Bool,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
# Add 2 UncertainDoubles
function +(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib, :addUncertainDoubles), UncertainFloat64,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
# Subtract 2 UncertainDoubles
function -(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib, :subtractUncertainDoubles), UncertainFloat64,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
# Multiply 2 UncertainDoubles
function *(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib, :multiplyUncertainDoubles), UncertainFloat64,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
# Divide 2 UncertainDoubles
function Base.:/(x::UncertainFloat64, y::UncertainFloat64)
    return (ccall(Libdl.dlsym(UDTLib, :divideUncertainDoubles), UncertainFloat64,
                    (UncertainFloat64, UncertainFloat64),x,y))
end
# Multiply a TAIRelDatetime by an uncertain floating point value
function *(x::UncertainFloat64, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :doubleMultRelDatetime), TAIRelDatetime,
                    (UncertainFloat64, TAIRelDatetime),x,y))
end
# Multiply an uncertain floating point value by a TAIRelDatetime
function *(x::TAIRelDatetime, y::UncertainFloat64)
    return y * x
end
# Multiply a TAIRelDatetime by a floating point value
function *(x::Float64, y::TAIRelDatetime)
    # Use the uncertain multiplication function
    return UncertainFloat64(x) * y
end
# Multiply a floating point value by a TAIRelDatetime
function *(x::TAIRelDatetime, y::Float64)
    return y * x
end
# Divide a TAIRelDatetime by a TAIRelDatetime
function Base.:/(x::TAIRelDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :ratioRelDatetimes), UncertainFloat64,
                    (TAIRelDatetime, TAIRelDatetime),x,y))
end
# Difference between 2 UTCDatetimes
function -(x::UTCDatetime, y::UTCDatetime)
    return (ccall(Libdl.dlsym(UDTLib,:diffUTCDatetimes), TAIRelDatetime,
                    (UTCDatetime, UTCDatetime),x,y))
end
# Add TAIRelDatetime to UTCDatetime
#    Set futureAdjust to 1
function +(x::UTCDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :addRelToUTCDatetime), UTCDatetime,
                    (UTCDatetime, TAIRelDatetime, UInt8),x,y,1))
end
# Add UTCDatetime to TAIRelDatetime
function +(x::TAIRelDatetime, y::UTCDatetime)
    return (y + x)
end
# Subtract TAIRelDatetime from UTCDatetime
#    Set futureAdjust to 1
function -(x::UTCDatetime, y::TAIRelDatetime)
    return (ccall(Libdl.dlsym(UDTLib, :subtractRelFromUTCDatetime), UTCDatetime,
                    (UTCDatetime, TAIRelDatetime, UInt8),x,y,1))
end
# Define a struct for date coordinates
struct DateCoords
    gigayear:: Int8
    year:: Int32
    month:: UInt8
    dayOfMonth:: UInt8
    calendar:: UInt32 #= 0 indicates Gregorian calendar with earliest last Julian date
                               1 indicates Julian calendar, so no last Julian date
                               2 indicates Swedish calendar, so 2/17/1753 is last Julian date
                               > 10000000 indicates encoded last Julian date yyyymmdd =#
    dateInit:: UInt16 #= Bit field indicating status of initialization and error codes
                    0000b indicates initialized without error
                    Other values indicate specification errors as follows as per ccInit
                    =#
end
# DateCoords constructors
#   From month, day, year
function DateCoords(m::Number, d::Number, y::Number; cal=0, gy=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createDateCoords), DateCoords,
            (Int8, Int32, UInt8, UInt8, UInt32),gy, y, m, d, cal))
    # Check dateInit
    if y.dateInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{DateCoords}()
    else
        return y
    end
end
#   From year and day of year
function DateCoords(dayOfYear::Number, y::Number; cal=0, gy=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createDateCoordsFromDayOfYear), DateCoords,
            (Int8, Int32, UInt32, UInt32),gy, y, dayOfYear, cal))
    # Check dateInit
    if y.dateInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{DateCoords}()
    else
        return y
    end
end
#   From year and weekday rule
function DateCoords(y::Number, m::Number, w::Number, d::Number; cal=0, gy=0, after=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createDateCoordsFromWeekdayRule), DateCoords,
            (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt32),gy, y, m, after, w, d, cal))
    # Check dateInit
    if y.dateInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{DateCoords}()
    else
        return y
    end
end
# Pretty print DateCoords
function show(io::IO, z::DateCoords)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringDateCoords), Cstring,
            (DateCoords, Ptr{UInt8}), z, sbuf)
    print(io, unsafe_string(buf))
end
# Report errors found during construction of a DateCoords
function reportUTCConstructionError(io::IO,date::DateCoords)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(UDTLib, :reportUTCConstructionError), Cstring,
            (UInt16, Ptr{UInt8}, Int, Int), date.dateInit , sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# DateCoords comparison functions
function ==(x::DateCoords, y::DateCoords)
  return (ccall(Libdl.dlsym(UDTLib,:isEqualDateCoords), Bool,
                  (DateCoords, DateCoords),x,y))
end
function !=(x::DateCoords, y::DateCoords)
    return !(x == y)
end
function >(x::DateCoords, y::DateCoords)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterDateCoords), Bool,
                    (DateCoords, DateCoords),x,y))
end
function <=(x::DateCoords, y::DateCoords)
    return !(x > y)
end
function <(x::DateCoords, y::DateCoords)
    return (ccall(Libdl.dlsym(UDTLib,:isLessDateCoords), Bool,
                    (DateCoords, DateCoords),x,y))
end
function >=(x::DateCoords, y::DateCoords)
    return !(x < y)
end
# Convert DateCoords between calendars
#   Convert to a date on the UTC Gregorian calendar
function asGregorian(x::DateCoords)
    return (ccall(Libdl.dlsym(UDTLib,:convertToUTCGregorian), DateCoords,
                    (DateCoords, ),x))
end
 #  Convert to a date on the Julian calendar
 function asJulian(x::DateCoords)
    return (ccall(Libdl.dlsym(UDTLib,:convertToJulian), DateCoords,
                    (DateCoords, ),x))
end
 # Convert to a date on a specified calendar
 function asCalendar(x::DateCoords, y::Number)
    return (ccall(Libdl.dlsym(UDTLib,:convertToCalendar), DateCoords,
                    (DateCoords, UInt32),x,y))
end
# Extract day of year from DateCoords
function dayOfYear(x::DateCoords)
    return (ccall(Libdl.dlsym(UDTLib,:dayOfYearFromDate), Int,
                    (DateCoords, ),x))
end
# Define a struct for time coordinates.  Only used as part of CalCoords struct.
struct TimeCoords
    hour:: UInt8
    minute:: UInt8
    second:: UInt8
    nanosecond:: UInt32
    attosecond:: UInt32
    timeInit:: UInt16
end
# Define a struct for calendar coordinates
struct CalCoords
    date:: DateCoords
    time:: TimeCoords
    ccInit:: UInt16 #= Bit field indicating status of initialization and error codes
                        0000h indicates initialized without error.
                        Other values indicate specification errors as follows, by bit number:
                        1 Date before the Big Bang,					 2 GigaYear >= 100,
                        3 Invalid Year <= -1e9 || >= 1e9,			 4 Invalid month,
                        5 Invalid day of the month,					 6 Invalid calendar,
                        7 Hour > 23 or Min > 59 or Sec > 60,		 8 Nanosecond >= 1e9,
                        9 Attosecond >= 1e9,						10 Not a leap second, but second == 60,
                        11 Invalid precision,						12 Invalid futureAdjust,
                        13 Invalid timezone,						14 Invalid frame of reference,
                        15 Invalid dateime (skipped period),		16 Missing disambiguation
					 =#
end
# CalCoords constructors
#   From month, day, year
function CalCoords(m::Number, d::Number, y::Number; cal=0, gy=0, h=0,
    min=0, s=0, ns=0, as=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createCalCoords), CalCoords,
            (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8,
            UInt32, UInt32, UInt32),gy, y, m, d, h, min, s,
            ns, as, cal))
    # Check ccInit
    if y.ccInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{CalCoords}()
    else
        return y
    end
end
#   From year and day of year
function CalCoords(dayOfYear::Number, y::Number; cal=0, gy=0, h=0,
    min=0, s=0, ns=0, as=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createCalCoordsFromDayOfYear), CalCoords,
            (Int8, Int32, UInt32, UInt8, UInt8, UInt8, UInt32, UInt32, UInt32),
            gy, y, dayOfYear, h, min, s, ns, as, cal))
    # Check ccInit
    if y.ccInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{CalCoords}()
    else
        return y
    end
end
#   From year and weekday rule
function CalCoords(y::Number, m::Number, w::Number, d::Number; cal=0, gy=0, after=0,
    h=0, min=0, s=0, ns=0, as=0)
    y = (ccall(Libdl.dlsym(UDTLib,:createCalCoordsFromWeekdayRule), CalCoords,
            (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, UInt32,
            UInt32, UInt32),
            gy, y, m, after, w, d, h, min, s, ns, as, cal))
    # Check ccInit
    if y.ccInit != 0
        reportUTCConstructionError(stdout, y)
        return Nullable{CalCoords}()
    else
        return y
    end
end
# Pretty print CalCoords
function show(io::IO, z::CalCoords)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringCalCoords), Cstring,
            (CalCoords, Ptr{UInt8}), z, sbuf)
    print(io, unsafe_string(buf))
end
# Report errors found during construction of a CalCoords
function reportUTCConstructionError(io::IO,cc::CalCoords)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(UDTLib, :reportUTCConstructionError), Cstring,
            (UInt16, Ptr{UInt8}, Int, Int), cc.ccInit , sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# CalCoords comparison functions
function ==(x::CalCoords, y::CalCoords)
  return (ccall(Libdl.dlsym(UDTLib,:isEqualCalCoords), Bool,
                  (CalCoords, CalCoords),x,y))
end
function !=(x::CalCoords, y::CalCoords)
    return !(x == y)
end
function >(x::CalCoords, y::CalCoords)
    return (ccall(Libdl.dlsym(UDTLib,:isGreaterCalCoords), Bool,
                    (CalCoords, CalCoords),x,y))
end
function <=(x::CalCoords, y::CalCoords)
    return !(x > y)
end
function <(x::CalCoords, y::CalCoords)
    return (ccall(Libdl.dlsym(UDTLib,:isLessCalCoords), Bool,
                    (CalCoords, CalCoords),x,y))
end
function >=(x::CalCoords, y::CalCoords)
    return !(x < y)
end
# Convert CalCoords between calendars
#   Convert to a datetime on the UTC Gregorian calendar
function asGregorian(x::CalCoords)
    return (ccall(Libdl.dlsym(UDTLib,:convertCalToUTCGregorian), CalCoords,
                    (CalCoords, ),x))
end
 #  Convert to a datetime on the Julian calendar
 function asJulian(x::CalCoords)
    return (ccall(Libdl.dlsym(UDTLib,:convertCalToJulian), CalCoords,
                    (CalCoords, ),x))
end
 # Convert to a datetime on a specified calendar
 function asCalendar(x::CalCoords, y::Number)
    return (ccall(Libdl.dlsym(UDTLib,:convertCalToCalendar), CalCoords,
                    (CalCoords, UInt32),x,y))
end
# Define a struct for UncertainFloat64s
struct UTCOffset
	#= A UTCOffset is used to translate from local time coordinates to UTC.
		The elements are specified as calendar coordinates, instead of as
		a relative datetime, since the local time cannot be converted to
		ticks prior to the conversion to UTC =#
	isNegative:: UInt8 # 0 for positive, 1 for negative
	days:: UInt8  # allow offsets with 0 or 1 day.  Used for offsets >= 24 hours.
	hours:: UInt8 # hours in the offset.  If >=24 hours, set days to 1.
	minutes:: UInt8 # minutes in the offset
	seconds:: UInt8 # seconds in the offset.  Seconds only allowed prior to
					#	the advent of legal time zones
	hundredths:: UInt8 # hundredths of a second in the offset
    offsetType:: UInt8 # Offset type: 0 = u to s, 1 = s to w, 2 = u to w, 3 = w to w
					   #    Type 3 represents a jump from the end of one period to the beginning of the next
	uOffInit:: UInt8 #= Bit field indicating status of initialization and error codes
					 0000h indicates initialized without error.
					 Other values indicate specification errors as follows, by bit number:
					 1 Invalid isNegative,							 2 Days > 1,
					 3 Hours > 23,									 4 Minutes > 59,
					 5 Seconds > 59,								 6 Hundredths > 99
                     7 Type > 3,									 8 Invalid Operation - attempt to generate
																		an offset via an invalid operation
					 =#
end
# UTCOffset constructor
function UTCOffset(; isneg = 0, d = 0, h = 0, m = 0, s = 0, hnd = 0, typ = "u-s")
    y = (ccall(Libdl.dlsym(LocLib,:createUTCOffset), UTCOffset,
            (UInt8, UInt8, UInt8, UInt8, UInt8, UInt8, Cstring),isneg, d, h, m, s, hnd, typ))
    # Check dateInit
    if y.uOffInit != 0
        reportUTCOffsetConstructionError(stdout, y)
        return Nullable{UTCOffset}()
    else
        return y
    end
end
# Pretty print UTCOffset
function show(io::IO, z::UTCOffset)
    buf = ccall(Libdl.dlsym(LocLib,:asStringUTCOffset), Cstring,
            (UTCOffset,), z)
    print(io, unsafe_string(buf))
end
# Report errors found during construction of a UTCOffset
function reportUTCOffsetConstructionError(io::IO,off::UTCOffset)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(LocLib, :reportUTCOffsetConstructionError), Cstring,
            (UInt16, Ptr{UInt8}, Int, Int), off.uOffInit , sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# UTCOffset comparison functions
function ==(x::UTCOffset, y::UTCOffset)
  return (ccall(Libdl.dlsym(LocLib,:isEqualUTCOffsets), Bool,
                  (UTCOffset, UTCOffset),x,y))
end
function !=(x::UTCOffset, y::UTCOffset)
    return !(x == y)
end
function >(x::UTCOffset, y::UTCOffset)
    return (ccall(Libdl.dlsym(LocLib,:isGreaterUTCOffsets), Bool,
                    (UTCOffset, UTCOffset),x,y))
end
function <=(x::UTCOffset, y::UTCOffset)
    return !(x > y)
end
function <(x::UTCOffset, y::UTCOffset)
    return (ccall(Libdl.dlsym(LocLib,:isLessUTCOffsets), Bool,
                    (UTCOffset, UTCOffset),x,y))
end
function >=(x::UTCOffset, y::UTCOffset)
    return !(x < y)
end
# Negate UTCOffset
function -(x::UTCOffset)
  return (ccall(Libdl.dlsym(LocLib,:negateUTCOffset), UTCOffset,
                  (UTCOffset, ),x))
end
# Absolute value of a UTCOffset
function abs(x::UTCOffset)
  return (ccall(Libdl.dlsym(LocLib,:absValueUTCOffset), UTCOffset,
                  (UTCOffset, ),x))
end
# Add 2 UTCOffsets
function +(x::UTCOffset, y::UTCOffset)
    z = ccall(Libdl.dlsym(LocLib, :addUTCOffsets), UTCOffset,
                    (UTCOffset, UTCOffset),x,y)
    # Check for invalid operation
    if z.uOffInit != 0
        reportUTCOffsetConstructionError(stdout, z)
        return Nullable{UTCOffset}()
    else
        return z
    end
end
# Subtract 2 UTCOffsets
function -(x::UTCOffset, y::UTCOffset)
    z = ccall(Libdl.dlsym(LocLib, :subtractUTCOffsets), UTCOffset,
                    (UTCOffset, UTCOffset),x,y)
    # Check for invalid operation
    if z.uOffInit != 0
        reportUTCOffsetConstructionError(stdout, z)
        return Nullable{UTCOffset}()
    else
        return z
    end
end
# Offset a CalCoords by a UTCOffset
function +(x::CalCoords, y::UTCOffset)
    z = ccall(Libdl.dlsym(LocLib, :offsetCalCoords), CalCoords,
                    (CalCoords, UTCOffset),x,y)
    # Check for invalid operation
    if z.ccInit != 0
        reportUTCConstructionError(stdout, z)
        return Nullable{UTCOffset}()
    else
        return z
    end
end
# Replace ASCII characters with superscripts to express exponents
function superscript(z::String)
    asc = "-0123456789"
    sup = "⁻⁰¹²³⁴⁵⁶⁷⁸⁹"
    y = Array{String}(undef,12)
    y[1] = z
    ind = nextind(sup,0)
    for i = 1:length(asc)
        y[i+1] = replace(y[i], asc[i] => sup[ind])
        ind = nextind(sup, ind)
    end
    return y[12]
end
# Beautify string representations of dates and times by levering Unicode
function beautify(inputString::String)
    # The C routine formats exponents as 10^-15.
    # Julia supports Unicode, so reformat exponents as 10⁻¹⁵
    # Find the first exponent field
    # The C routine alwyas provides a minimum of 2 digits for the exponent
    #  Delete leading 0's
    #  First handle 10^0i
    tmpbuf = replace(inputString, "^0" => "^")
    #  Now handle 10^-0i
    buf = replace(tmpbuf, "^-0" => "^-")
    #  Now handle 10^0i
    ind = something(findnext(isequal('^'), buf, 1),0)
    if (ind != 0)
        # Extract the exponent string
        # The exponent is either followed by a blank or is at the end of the string
        expend = something(findnext(isequal(' '), buf, ind),0)
        if (expend == 0)
            expend = length(buf)
        end
        expstr = buf[ind+1:expend]
        # Create the replacement string
        repexp = superscript(expstr)
        # Replace the exponent string
        y = replace(buf, "^"*expstr => repexp, count=1)
        # Search for a second exponent field (i.e., for the uncertainty)
        ind2 = something(findnext(isequal('^'), buf, ind + 1),0)
        if (ind2 != 0)
            expend2 = length(buf) # Second exponent ends at the end of the string
            expstr2 = buf[ind2 + 1:expend2]
            repexp2 = superscript(expstr2)
            z = replace(y, "^"*expstr2 => repexp2)
            # The C routine precedes the uncertainty with +/-.
            # Julia supports Unicode, so replace with ±
            # This replacement must be done after the expoents are replaced, since if it is done
            #   before the superscripts, the logic for finding the exponents in the string will
            #   be flawed, since the unicode character ± occupies more than 1 byte.  For example,
            #   this means the last character is not located at len(buf).
            z = replace(z, "+/-" => "±")
            return z
        end
        # The C routine precedes the uncertainty with +/-.
        # Julia supports Unicode, so replace with ±
        y = replace(y, "+/-" => "±")
        return y
    end
    # The C routine precedes the uncertainty with +/-.
    # Julia supports Unicode, so replace with ±
    buf = replace(buf, "+/-" => "±")
    return buf
end
# Print datetime given the full range of elements
function showDatetime(m::Number, d::Number, y::Number; cal=0, gy=0, h=0,
    min=0, s=0, ns=0, as=0, f='w', p=99, u=0, tz="", bora=2)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(UDTLib,:asStringDatetime), Cstring,
            (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8,
            UInt32, UInt32, UInt32, UInt8, Int8, Int8, Cstring,
            UInt8, Ptr{UInt8}),
            gy, y, m, d, h, min, s, ns, as, cal, f, p, u, tz, bora, sbuf)
#    print(stdout, unsafe_string(buf))
    return unsafe_string(buf)
end
# Define a struct to represent calendar coordinates for a local datetime
struct LocalCalCoordsDT
#
	# A local datetime expressed in terms of calendar coordinates without a tick count
	cc:: CalCoords #= The calendar coordinates - gigayear through attosecond, plus calendar.=#
	timezone:: NTuple{maxTZNameLength,UInt8} #= The name of a timezone from the IANA time zone database.
						   It is storage inefficient to use the name, but if it is
						   replaced by an index, care must be taken to not change
						   index values when updating the time zone database. =#
	frame:: UInt8 # Frame of reference.  0 = universal, 1 = standard and 2 = wall.
						#   Wall is daylight savings time when a DSTR is in effect
						#   and standard time when there is no effective DSTR.
	bOrA:: UInt8 #= Flag indicating whether a specified time is before a negative jump
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
				  there is a negative change in GMT Offset. =#
    ambigS:: UInt8 #= A flag indicating whether the LocalCalCoordsDT is an ambiguous region when
              		translated to a standard time.
              		Ambiguous regions can occur for standard times, when there is a negative
              		change in GMT offset, or for wall times, when there is negative jump in
              		GMT offset + S-W offset.  When a LocalCalCoordsDT is translated to either
              		a standard or wall frame of reference, a check must be made for ambiguous
              		regions and the appropriate value of bOrA assigned.  This check can be
              		complex, requiring much of the logic in computeOffsets.  To avoid these
              		calculations upon every translation, ambigS and ambigW are computed
              		in the computeOffsets call and saved within the LocalCalCoordsDT struct. =#
    ambigW:: UInt8 #= A flag indicating whether the LocalCalCoordsDT is an ambiguous region when
              						translated to a wall time. =#
    beforeFlag:: UInt8 #= For a LocalCalCoordsDT created in the universal frame of reference,
              			or for a LocalCalCoordsDT created in an unambiguous region for
              			a standard or wall frame of reference.
              			bOrA will be 0.  As such, there is no indication when the
              			LocalCalCoordsDT is translated to standard or wall frame of
              			reference, and there is ambiguity, whether bOrA is before
              			or after.  This flag provides that indication.
              			One could conceive of a pathological case where a before value
              			in standard frame of reference is actually an after value in
              			wall frame of reference.  This case does not exist in practice
              			and would likely cause problems in other sections of the algorithem.
              			Thus, we will assume that whenever there is ambiguity in both the
              			standard and wall frames of reference, both are before the jump
              			or both are after the jump.=#
	futureAdjust:: UInt8 #= Same concept as for UTC, except for time zone and/or rules.  Ie, if
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
						  to GMTOffset or DSTR, futureAdjust is set to 0. =#
    timezoneIndex:: UInt32 # The index of the time zone in the PeriodTimeZones array
    periodIndex:: UInt8 #= The index of the period in the PeriodTimeZones array.
                      			GMT Offset can be retrieved:
                      			PeriodTimeZones[timezoneIndex].periods[periodIndex].gMTOffset
                      			Could have stored GMTOffset, but that is 64 bits, as opposed to 40 bits =#
    sToWMinutes:: UInt8 #= The DST (aka S-W) offset based on the time zone and DSTR in minutes.
                      			All DST offsets are in even number of minutes, with the largest
                      			in the IANA files being 2 hours.  Could have stored sToWOffset, but
                      			that is 64 bits.
                      			S-W offset can be computed by calling:
                      				createUTCOffset(0, 0, sToWMinutes/60, sToWMinutes % 60, ....) =#
	lccInit:: UInt16  #= Bit field indicating status of initialization and error codes
						0000h indicates initialized without error.
						Other values indicate specification errors as per ccInit
					   =#
end
# LocalCalCoordsDT constructors
#   From month, day, year
function LocalCalCoordsDT(m::Number, d::Number, y::Number, tz::String, f::Number; cal=0, gy=0, h=0,
    min=0, s=0, ns=0, as=0, ba=0, fa=0)
    y = (ccall(Libdl.dlsym(LocLib,:createLocalCalCoordsDT), LocalCalCoordsDT,
            (Int8, Int32, UInt8, UInt8, UInt8, UInt8, UInt8,
            UInt32, UInt32, UInt32, Ptr{UInt8}, UInt8, UInt8, UInt8),gy, y, m, d, h, min, s,
            ns, as, cal, tz, f, ba, fa))
    # Check lccInit
    if y.lccInit != 0
        reportLCCConstructionError(stdout, y)
        return Nullable{LocalCalCoordsDT}()
    else
        return y
    end
end
# Pretty print LocalCalCoordsDT
function show(io::IO, z::LocalCalCoordsDT)
    sbuf = Vector{UInt8}(undef,130)
    buf = ccall(Libdl.dlsym(LocLib,:asStringLocalCalCoordsDT), Cstring,
            (LocalCalCoordsDT, Ptr{UInt8}), z, sbuf)
    print(io, beautify(unsafe_string(buf)))
end
# Report errors found during construction of a LocalCalCoordsDT
function reportLCCConstructionError(io::IO,lcc::LocalCalCoordsDT)
    sbuf = Vector{UInt8}(undef,800)
    buf = ccall(Libdl.dlsym(UDTLib, :reportUTCConstructionError), Cstring,
            (UInt16, Ptr{UInt8}, Int, Int), lcc.lccInit, sbuf, 800, 0)
    print(io, unsafe_string(buf),"\n")
end
# LocalCalCoordsDT comparison functions
function ==(x::LocalCalCoordsDT, y::LocalCalCoordsDT)
  return (ccall(Libdl.dlsym(LocLib,:isEqualLocalCalCoordsDT), Bool,
                  (LocalCalCoordsDT, LocalCalCoordsDT),x,y))
end
# Translate a LocalCalCoordsDT to universal frame of reference
function translateToUniversal(z::LocalCalCoordsDT)
#    tzIndex = Ref{Cuint}(0)
#    perIndex = Ref{Cuchar}(0)
    y = ccall(Libdl.dlsym(LocLib, :translateToUniversal), LocalCalCoordsDT,
#    (LocalCalCoordsDT, Ref{Cuint}, Ref{Cuchar}), z, tzIndex, perIndex)
    (LocalCalCoordsDT,), z)
#    print(tzIndex[], " ", perIndex[])
    # Check lccInit
    if y.lccInit != 0
        reportLCCConstructionError(stdout, y)
        return Nullable{LocalCalCoordsDT}()
    else
        return y
    end
end
# Translate a LocalCalCoordsDT from universal frame of reference
function translateFromUniversal(z::LocalCalCoordsDT, tz::String, f::Number)
#    tzIndex = Ref{Cuint}(0)
#    perIndex = Ref{Cuchar}(0)
    ctz = Cstring
    ctz = tz
    y = ccall(Libdl.dlsym(LocLib, :translateFromUniversal), LocalCalCoordsDT,
#    (LocalCalCoordsDT, Cstring, UInt8, Ref{Cuint}, Ref{Cuchar}), z, tz, f, tzIndex, perIndex)
    (LocalCalCoordsDT, Cstring, UInt8), z, tz, f)
#    print(tzIndex[], " ", perIndex[])
    # Check lccInit
    if y.lccInit != 0
        reportLCCConstructionError(stdout, y)
        return Nullable{LocalCalCoordsDT}()
    else
        return y
    end
end
# Translate a LocalCalCoordsDT to a specified time zone and frame of reference
function translate(z::LocalCalCoordsDT; tz="", f=3)
    # If either tz or f is defaulted, pass the value from z
    cBuf = Vector{Char}(undef,length(z.timezone))
    if tz == ""
        lenString = 1
        for i = 1:length(z.timezone)
            if z.timezone[i] == 0
                lenString = i - 1
                break
            end
            cBuf[i] = Char(z.timezone[i])
        end
        tz = String(cBuf)[1:lenString]
    end
    if f == 3
        f = z.frame
    end
    y = ccall(Libdl.dlsym(LocLib, :translateLocalCalCoordsDT), LocalCalCoordsDT,
    (LocalCalCoordsDT, Cstring, UInt8), z, tz, f)
    # Check lccInit
    if y.lccInit != 0
        reportLCCConstructionError(stdout, y)
        return Nullable{LocalCalCoordsDT}()
    else
        return y
    end
end
#
function julianUntil(cal::Number, tz::Number, f::Number)
    return ccall(Libdl.dlsym(LocLib, :julianUntilCoords), LocalCalCoordsDT,
    (UInt32, UInt32, UInt8), cal, tz, f)
end
#
function gregorianFrom(cal::Number, tz::Number, f::Number)
    return ccall(Libdl.dlsym(LocLib, :gregorianFromCoords), LocalCalCoordsDT,
    (UInt32, UInt32, UInt8), cal, tz, f)
end
end # End module UltimateDatetime
# Unit Tests
using .UltimateDatetime
using Test
relTAI1 = TAIRelTicks(s=3,as=4)
tAI1 = TAITicks(s=5,as=10)
tAI2 = TAITicks(s=5,as=10)
tAI3 = TAITicks(s=3,as=4)
relTAI2 = TAIRelTicks(s=3,as=4,isneg=true)
@testset "TAITicks formatting function" begin
    @test string(TAITicks(gs=15,s=12,ns=3,as=100)) == "15000000012.000000003000000100"
end
@testset "TAIRelTicks formatting function" begin
    @test string(TAIRelTicks(gs=15,s=12,ns=3,as=100,isneg=true)) == "-15000000012.000000003000000100"
end
@testset "TAITicks comparison functions" begin
    @test tAI1 == tAI2
    @test !(tAI1 == tAI3)
    @test !(tAI1 != tAI2)
    @test tAI1 != tAI3
    @test tAI1 > tAI3
    @test !(tAI3 > tAI1)
    @test !(tAI1 > tAI2)
end
@testset "TAIRelTicks comparison functions" begin
    @test (relTAI1 == -relTAI2)
end
utc1 = UTCDatetime(y=2016,m=6,d=30)
utc2 = UTCDatetime(y=2017,m=6,d=30,h=5,min=4,s=3,ns=2,as=1)
creation = UTCDatetime(gy=-13,y=-826151188,m=12,d=25,h=12)
endOfTime = UTCDatetime(gy=99,y=999999999,m=12,d=31,h=23,min=59,s=59,ns=999999999,as=999999999)
rel1 = utc2 - utc1
rel2 = utc1 - utc2
rel3 = utc1 - utc1
rel4 = TAIRelDatetime(m=10,p=0,u=3)
rel5 = TAIRelDatetime(m=10,p=1,u=2)
rel6 = TAIRelDatetime(h=10,p=2,u=3)
rel7 = TAIRelDatetime(h=0)
rel8 = TAIRelDatetime(h=0,p=0,u=2)
rel9 = TAIRelDatetime(h=0,p=2,u=1)
@testset "UTCDatetime and TAIRelDatetime comparisons" begin
    @test !(utc2 == utc1)
    @test utc2 != utc1
    @test utc2 > utc1
    @test utc2 >= utc1
    @test !(utc2 < utc1)
    @test !(utc2 <= utc1)
    @test rel4 == rel5
    @test !(rel4 != rel5)
    @test rel1 == -rel2
    @test rel1 > rel4
    @test rel1 >= rel4
    @test !(rel1 < rel4)
    @test !(rel1 <= rel4)
end
@testset "TAIRelDatetime ratios and multiplication by uncertain float" begin
end
uCons = Array{UTCDatetime}(undef,34)
uConsNeg = Array{UTCDatetime}(undef,34)
uConsNeg2 = Array{UTCDatetime}(undef,34)
uConsUpperBound = Array{UTCDatetime}(undef,34)
uConsLowerBound = Array{UTCDatetime}(undef,34)
for ind = 1:34
    uCons[ind] =
        UTCDatetime(gy=46,y=123456789,m=10,d=30,h=5,min=14,s=20,ns=123456789,as=123456789,p=ind-19,u=2)
    uConsNeg[ind] =
        UTCDatetime(gy=-12,y=-999999999,m=10,d=30,h=5,min=14,s=20,ns=123456789,as=123456789,p=ind-19,u=2)
    uConsNeg2[ind] =
        UTCDatetime(gy=-12,y=-999999998,m=10,d=30,h=5,min=14,s=20,ns=123456789,as=123456789,p=ind-19,u=2)
    uConsUpperBound[ind] =
        UTCDatetime(gy=99,y=999999999,m=12,d=31,h=23,min=59,s=59,ns=999999999,as=999999999,p=ind-19,u=2)
    uConsLowerBound[ind] =
        UTCDatetime(gy=-13,y=-826151188,m=12,d=25,h=12,min=0,s=0,ns=0,as=0,p=ind-19,u=2)
end
@testset "UTCDatetime construction with truncation" begin
    @test string(uCons[1].tai) == "1891835860690257297.123456789123456789"
    @test string(uCons[2].tai) == "1891835860690257297.123456789123456780"
    @test string(uCons[3].tai) == "1891835860690257297.123456789123456700"
    @test string(uCons[4].tai) == "1891835860690257297.123456789123456000"
    @test string(uCons[5].tai) == "1891835860690257297.123456789123450000"
    @test string(uCons[6].tai) == "1891835860690257297.123456789123400000"
    @test string(uCons[7].tai) == "1891835860690257297.123456789123000000"
    @test string(uCons[8].tai) == "1891835860690257297.123456789120000000"
    @test string(uCons[9].tai) == "1891835860690257297.123456789100000000"
    @test string(uCons[10].tai) == "1891835860690257297.123456789000000000"
    @test string(uCons[11].tai) == "1891835860690257297.123456780000000000"
    @test string(uCons[12].tai) == "1891835860690257297.123456700000000000"
    @test string(uCons[13].tai) == "1891835860690257297.123456000000000000"
    @test string(uCons[14].tai) == "1891835860690257297.123450000000000000"
    @test string(uCons[15].tai) == "1891835860690257297.123400000000000000"
    @test string(uCons[16].tai) == "1891835860690257297.123000000000000000"
    @test string(uCons[17].tai) == "1891835860690257297.120000000000000000"
    @test string(uCons[18].tai) == "1891835860690257297.100000000000000000"
    @test string(uCons[19].tai) == "1891835860690257297.000000000000000000"
    @test string(uCons[20].tai) == "1891835860690257277.000000000000000000"
    @test string(uCons[21].tai) == "1891835860690256437.000000000000000000"
    @test string(uCons[22].tai) == "1891835860690238437.000000000000000000"
    @test string(uCons[23].tai) == "1891835860687732837.000000000000000000"
    @test uCons[24].month == 1
    @test uCons[25].year == 123456790
    @test uCons[26].year == 123456800
    @test uCons[27].year == 123457000
    @test uCons[28].year == 123460000
    @test uCons[29].year == 123500000
    @test uCons[30].year == 123000000
    @test uCons[31].year == 120000000
    @test uCons[32].year == 100000000
    @test uCons[33].year == 0
    @test uCons[33].gigayear == 46
    @test uCons[34].gigayear == 50
    @test string(uConsNeg[1].tai) == "26071348757102060.123456789123456789"
    @test string(uConsNeg[2].tai) == "26071348757102060.123456789123456780"
    @test string(uConsNeg[3].tai) == "26071348757102060.123456789123456700"
    @test string(uConsNeg[4].tai) == "26071348757102060.123456789123456000"
    @test string(uConsNeg[5].tai) == "26071348757102060.123456789123450000"
    @test string(uConsNeg[6].tai) == "26071348757102060.123456789123400000"
    @test string(uConsNeg[7].tai) == "26071348757102060.123456789123000000"
    @test string(uConsNeg[8].tai) == "26071348757102060.123456789120000000"
    @test string(uConsNeg[9].tai) == "26071348757102060.123456789100000000"
    @test string(uConsNeg[10].tai) == "26071348757102060.123456789000000000"
    @test string(uConsNeg[11].tai) == "26071348757102060.123456780000000000"
    @test string(uConsNeg[12].tai) == "26071348757102060.123456700000000000"
    @test string(uConsNeg[13].tai) == "26071348757102060.123456000000000000"
    @test string(uConsNeg[14].tai) == "26071348757102060.123450000000000000"
    @test string(uConsNeg[15].tai) == "26071348757102060.123400000000000000"
    @test string(uConsNeg[16].tai) == "26071348757102060.123000000000000000"
    @test string(uConsNeg[17].tai) == "26071348757102060.120000000000000000"
    @test string(uConsNeg[18].tai) == "26071348757102060.100000000000000000"
    @test string(uConsNeg[19].tai) == "26071348757102060.000000000000000000"
    @test string(uConsNeg[20].tai) == "26071348757102040.000000000000000000"
    @test string(uConsNeg[21].tai) == "26071348757101200.000000000000000000"
    @test string(uConsNeg[22].tai) == "26071348757083200.000000000000000000"
    @test string(uConsNeg[23].tai) == "26071348754577600.000000000000000000"
    @test uConsNeg[24].month == 1
    @test uConsNeg[25].year == -999999999
    @test uConsNeg[33].year == -999999999
    @test uConsNeg[33].gigayear == -12
    @test uConsNeg[34].gigayear == -9
    @test string(uConsNeg[26]) == "13,000,000,000 BCE ± 200 years UTC"
    @test uConsNeg2[24].year == -999999998
    @test uConsNeg2[25].year == -999999999
    @test string(uConsNeg2[24]) == "12,999,999,999 BCE ± 2 years UTC"
    @test string(uConsNeg2[25]) == "13,000,000,000 BCE ± 20 years UTC"
    @test string(uConsLowerBound[1].tai) == "0.000000000000000000"
    @test string(uConsLowerBound[10].tai) == "0.000000000000000000"
    @test string(uConsLowerBound[20].tai) == "0.000000000000000000"
    @test string(uConsLowerBound[21]) == "25-Dec-13,826,151,189 BCE 12h ± 2 hours UTC"
    @test string(uConsLowerBound[22]) == "26-Dec-13,826,151,189 BCE ± 2 days UTC"
    @test string(uConsLowerBound[23]) == "Jan-13,826,151,188 BCE ± 2 months UTC"
    @test 1 - uConsLowerBound[24].year == 826151188
    @test string(uConsLowerBound[24]) == "13,826,151,188 BCE ± 2 years UTC"
    @test 1 - uConsLowerBound[25].year == 826151180
    @test string(uConsLowerBound[25]) == "13,826,151,180 BCE ± 20 years UTC"
    @test 1 - uConsLowerBound[26].year == 826151100
    @test string(uConsLowerBound[26]) == "13,826,151,100 BCE ± 200 years UTC"
    @test 1 - uConsLowerBound[27].year == 826151000
    @test string(uConsLowerBound[27]) == "13,826,151,000 BCE ± 2 kiloyears UTC"
    @test 1 - uConsLowerBound[28].year == 826150000
    @test string(uConsLowerBound[28]) == "13,826,150,000 BCE ± 20 kiloyears UTC"
    @test 1 - uConsLowerBound[29].year == 826100000
    @test string(uConsLowerBound[29]) == "13,826,100,000 BCE ± 200 kiloyears UTC"
    @test 1 - uConsLowerBound[30].year == 826000000
    @test string(uConsLowerBound[30]) == "13,826,000,000 BCE ± 2 megayears UTC"
    @test 1 - uConsLowerBound[31].year == 820000000
    @test string(uConsLowerBound[31]) == "13,820,000,000 BCE ± 20 megayears UTC"
    @test 1 - uConsLowerBound[32].year == 800000000
    @test string(uConsLowerBound[32]) == "13,800,000,000 BCE ± 200 megayears UTC"
    @test 1 - uConsLowerBound[33].year == 1000000000
    @test string(uConsLowerBound[33]) == "13,000,000,000 BCE ± 2 gigayears UTC"
    @test 1 - uConsLowerBound[34].year == 1000000000
    @test string(uConsLowerBound[34]) == "10,000,000,000 BCE ± 20 gigayears UTC"
    @test uConsUpperBound[24].year == 999999999
    @test string(uConsUpperBound[24]) == "99,999,999,999 ± 2 years UTC"
    @test uConsUpperBound[25].year == 999999990
    @test string(uConsUpperBound[25]) == "99,999,999,990 ± 20 years UTC"
    @test uConsUpperBound[26].year == 999999900
    @test string(uConsUpperBound[26]) == "99,999,999,900 ± 200 years UTC"
    @test uConsUpperBound[27].year == 999999000
    @test string(uConsUpperBound[27]) == "99,999,999,000 ± 2 kiloyears UTC"
    @test uConsUpperBound[28].year == 999990000
    @test string(uConsUpperBound[28]) == "99,999,990,000 ± 20 kiloyears UTC"
    @test uConsUpperBound[29].year == 999900000
    @test string(uConsUpperBound[29]) == "99,999,900,000 ± 200 kiloyears UTC"
    @test uConsUpperBound[30].year == 999000000
    @test string(uConsUpperBound[30]) == "99,999,000,000 ± 2 megayears UTC"
    @test uConsUpperBound[31].year == 990000000
    @test string(uConsUpperBound[31]) == "99,990,000,000 ± 20 megayears UTC"
    @test uConsUpperBound[32].year == 900000000
    @test string(uConsUpperBound[32]) == "99,900,000,000 ± 200 megayears UTC"
    @test uConsUpperBound[33].year == 0
    @test string(uConsUpperBound[33]) == "99,000,000,000 ± 2 gigayears UTC"
    @test uConsUpperBound[34].year == 0
    @test string(uConsUpperBound[34]) == "90,000,000,000 ± 20 gigayears UTC"
end
@testset "UTCDatetime / TAIRelDatetime arithmetic" begin
    @test rel1 + rel2 == TAIRelDatetime(d=0)
    @test rel1 == TAIRelDatetime(d=365,h=5,m=4,s=4,ns=2,as=1)  # Test addition of the 12-31-2016 leap second
    @test utc2 + rel4 ==‴ UTCDatetime(y=2017,m=6,d=30,h=5,min=14,s=3,ns=2,as=1,p=0,u=3)
    @test utc2 + rel6 ==‴ UTCDatetime(y=2017,m=6,d=30,h=15,min=4,s=3,ns=2,as=1,p=2,u=3)
end
u1 = UncertainFloat64(0.;p=0.1,u=3.) # Zero with specified Precision and Uncertainty
u2 = UncertainFloat64(0.;p=0.02,u=3.) # Zero with specified Precision and Uncertainty
u3 = UncertainFloat64(0.;p=0.,u=0.)  # Zero with unspecified Precision and Uncertainty
u4 = UncertainFloat64(0.;p=1.e40,u=5.)  # Zero with large specified Precision and Uncertainty
u5 = UncertainFloat64(0.;p=1.e40,u=100.)  # Zero with large specified Precision and Uncertainty > 10
u6 = UncertainFloat64(0.;p=0.01,u=0.) # Zero with specified Precision, but unspecied Uncertainty
u7 = UncertainFloat64(1500.;p=-3.1, u=-2.4) # Negative precision and uncertainty
u8 = UncertainFloat64(-99999.;p=100.,u=1.) # Negative value with specified precision and uncertainty.  Requires value rounding.
u9 = UncertainFloat64(-99999.;p=1.,u=1.) # Negative value with specified precision and uncertainty
u10 = UncertainFloat64(-30.;p=100.,u=1.)  #Negative value with magnitude of precision > magnitude of value
u11 = UncertainFloat64(30.;p=10.,u=20.) # Magnitude of precision becomes greater than value after adjutment due to large uncertainty
u12 = UncertainFloat64(54.;p=.01, u=.01) # Uncertainty too small so adjusted
u13 = UncertainFloat64(1234567.0123456789) # 17 signficant digits specified
u14 = UncertainFloat64(0.12345678901234567) # 17 significant digits specified, value < 1
u15 = UncertainFloat64(12345678901234567.) # 17 significant digits specified, value > 1e16
u16 = UncertainFloat64(9007199254740992.;p=1.,u=0.) # Maximum guaranteed exact value
u17 = UncertainFloat64(9007199254740993.;p=1.,u=0.) # Maximum guaranteed exact value + 1, but represented as maximum exact value
u18 = UncertainFloat64(9007199254740994.;p=1.,u=0.) # Maximum guaranteed exact value + 2
u19 = UncertainFloat64(9999999999999999.;p=1.,u=0.) # Representation is 10000000000000000.
u20 = UncertainFloat64(1234567.0123456789; p=1.e7,u=1) # 17 significant digits, precision > value
u21 = UncertainFloat64(1234567.0123456789; p=1.e6,u=1)
u22 = UncertainFloat64(1234567.0123456789; p=1.e5,u=1)
u23 = UncertainFloat64(1234567.0123456789; p=1.e4,u=1)
u24 = UncertainFloat64(1234567.0123456789; p=1.e3,u=1)
u25 = UncertainFloat64(1234567.0123456789; p=1.e2,u=1)
u26 = UncertainFloat64(1234567.0123456789; p=1.e1,u=1)
u27 = UncertainFloat64(1234567.0123456789; p=1.e0,u=1)
u28 = UncertainFloat64(1234567.0123456789; p=1.e-1,u=1)
u29 = UncertainFloat64(1234567.0123456789; p=1.e-2,u=1)
u30 = UncertainFloat64(1234567.0123456789; p=1.e-3,u=1)
u31 = UncertainFloat64(1234567.0123456789; p=1.e-4,u=1)
u32 = UncertainFloat64(1234567.0123456789; p=1.e-5,u=1)
u33 = UncertainFloat64(1234567.0123456789; p=1.e-6,u=1)
u34 = UncertainFloat64(1234567.0123456789; p=1.e-7,u=1)
u35 = UncertainFloat64(1234567.0123456789; p=1.e-8,u=1)
u36 = UncertainFloat64(1234567.0123456789; p=1.e-9,u=1)
u37 = UncertainFloat64(1234567.0123456789; p=1.e-10,u=1) # 17 significant digits, precision constrained by minimum
u38 = UncertainFloat64(-1234567.0123456789; p=1.e-11,u=1) # 17 significant digits, precision constrained by minimum
u39 = UncertainFloat64(-0.12345678901234567; p=1.,u=2) # 17 significant digits, precision > value
u40 = UncertainFloat64(-0.12345678901234567; p=1.e-1,u=2)
u41 = UncertainFloat64(-0.12345678901234567; p=1.e-4,u=2)
u42 = UncertainFloat64(-0.12345678901234567; p=1.e-8,u=2)
u43 = UncertainFloat64(-0.12345678901234567; p=1.e-17,u=2)
u44 = UncertainFloat64(-0.12345678901234567)
u45 = UncertainFloat64(945.) # Exact value
u46 = UncertainFloat64(-945.) # Exact value
u47 = UncertainFloat64(945.1)
u48 = UncertainFloat64(-945.1)
u49 = UncertainFloat64(451.1)
u50 = UncertainFloat64(2000000000000000.) # Exact values
@testset "UncertainFloat64 validity checks and associated adjustments" begin
    @test string(u1) == "0.0 ± 0.3"
    @test string(u2) == "0.00 ± 0.06"
    @test string(u3) == "0"
    @test string(u4) == "0 x 10⁴⁰ ± 5 x 10⁴⁰"
    @test string(u5) == "0 x 10⁴² ± 1 x 10⁴²"
    @test string(u6) == "0.00 ± 0.005"
    @test string(u7) == "1,500.000 000 000 000 ± 0.000 000 000 000 3"
    @test string(u8) == "-100,000 ± 100"
    @test string(u9) == "-99,999 ± 1"
    @test string(u10) == "0 x 10² ± 100"
    @test string(u11) == "0 x 10² ± 200"
    @test string(u12) == "54.00 ± 0.005"
    @test string(u13) == "1,234,567.012 345 679 ± 0.000 000 000 2"
    @test string(u14) == "1.234 567 890 123 457 x 10⁻¹ ± 2 x 10⁻¹⁷"
    @test string(u15) == "1.234 567 890 123 457 x 10¹⁶ ± 2"
    @test string(u16) == "9,007,199,254,740,992"
    @test string(u17) == "9,007,199,254,740,992"
    @test string(u18) == "9,007,199,254,740,994 ± 0.5"
    @test string(u19) == "1.000 000 000 000 000 x 10¹⁶ ± 2"
    @test string(u20) == "0 x 10⁷ ± 10,000,000"
    @test string(u21) == "1,000,000 ± 1,000,000"
    @test string(u22) == "1,200,000 ± 100,000"
    @test string(u23) == "1,230,000 ± 10,000"
    @test string(u24) == "1,235,000 ± 1,000"
    @test string(u25) == "1,234,600 ± 100"
    @test string(u26) == "1,234,570 ± 10"
    @test string(u27) == "1,234,567 ± 1"
    @test string(u28) == "1,234,567.0 ± 0.1"
    @test string(u29) == "1,234,567.01 ± 0.01"
    @test string(u30) == "1,234,567.012 ± 0.001"
    @test string(u31) == "1,234,567.012 3 ± 0.000 1"
    @test string(u32) == "1,234,567.012 35 ± 0.000 01"
    @test string(u33) == "1,234,567.012 346 ± 0.000 001"
    @test string(u34) == "1,234,567.012 345 7 ± 0.000 000 1"
    @test string(u35) == "1,234,567.012 345 68 ± 0.000 000 01"
    @test string(u36) == "1,234,567.012 345 679 ± 0.000 000 001"
    @test string(u37) == "1,234,567.012 345 679 ± 0.000 000 000 2"
    @test string(u38) == "-1,234,567.012 345 679 ± 0.000 000 000 2"
    @test string(u39) == "0 ± 2"
    @test string(u40) == "-0.1 ± 0.2"
    @test string(u41) == "-0.123 5 ± 0.000 2"
    @test string(u42) == "-0.123 456 79 ± 0.000 000 02"
    @test string(u43) == "-1.234 567 890 123 457 x 10⁻¹ ± 2 x 10⁻¹⁷"
    @test string(u44) == "-1.234 567 890 123 457 x 10⁻¹ ± 2 x 10⁻¹⁷"
    @test string(u45) == "945"
    @test string(u46) == "-945"
    @test string(u47) == "945.100 000 000 000 0 ± 0.000 000 000 000 2"
    @test string(u48) == "-945.100 000 000 000 0 ± 0.000 000 000 000 2"
    @test string(u49) == "451.100 000 000 000 0 ± 0.000 000 000 000 09"
end
@testset "UncertainFloat64 comparison functions" begin
    @test !(u43 == u44) # Midpoint values are equal, uncertainties are not zero, so false
    @test u43 != u44 # May not be equal
    @test u43 ==′ u44  # Midpoint values are equal
    @test !(u43 !=′ u44) # Midpoint values are equal, so false
    @test u43 ==″ u44 # Can be equal
    @test !(u43 !=″ u44) # Cannot be equal - false
    @test u43 ==‴ u44 # Midpoint valnes and uncertaities are equal
    @test !(u43 !=‴ u44) # Midpoint values and uncertainties are equal, so false
    @test !(u43 > u44) # Must be greater - false
    @test u43 <=″ u44 # May be less than or equal
    @test !(u43 >′ u44) # Has greater midpoint - false
    @test u43 <=′ u44 # Has less than or equal midoint
    @test u43 >″ u44 # Can be greater or equal
    @test !(u43 <= u44) # Is always less than or equal - false
    @test !(u43 >= u44) # Is always greater than or equal - false
    @test u43 <″ u44 # Can be less than or equal
    @test u43 >=′ u44 # Midpoint is greater than or equal
    @test !(u43 <′ u44) # Midpoint is less - false
    @test u43 >=″ u44 # Can be greater than or equal
    @test !(u43 < u44) # Must be less - false
end
@testset "UncertainFloat64 arithmetic" begin
# Multiply 2 exact numbers where the product is in range
    @test string(u45*u46) == "-893,025"
# Multiply 2 exact numbers where the product is not in range
    @test string(u50*u46) == "-1.890 000 000 000 000 x 10¹⁸ ± 400"
# Multiply an exact number by an uncertain number
    @test string(u22*u45) == "1,130,000,000 ± 90,000,000"
# Multiply 2, positive non-zero uncertain numbers
# Multiply 2, negative non-zero uncertain numbers
# Multiply a positive, non-zero uncertain number by a negative, non-zero uncertain number
# Multiply a zero uncertain number by a positive, non-zero uncertain number
# Multiply a positive, non-zero uncertain number by a zero uncertain number
# Multiply a zero uncertain number by a negative, non-zero uncertain number
# Multiply a negative, non-zero uncertain number by a zero uncertain number
# Multiply 2 zero uncertain numbers
# Divide 2 numbers where the denominator is zero
# Divide 2 exact numbers where the result is an exact number
# Divide 2 exact numbers where the result is not an exact number
# Divide an exact number by an uncertain number
# Divide an uncertain zero  by an exact number
# Divide an uncertain non-zero number by an exact number
# Divide an uncertain number by an uncertain number
# Divide an uncertain zero by an uncertain number
end
@testset "DateCoords calendar conversions" begin
    # Last UTC Gregorian date, UTC time zone
    @test DateCoords(12,31,999999999;gy=99) == asGregorian(asJulian(DateCoords(12,31,999999999;gy=99)))
    # Last UTC Gregorian date, any time zone
    @test DateCoords(1,1,0;gy=100) == asGregorian(asJulian(DateCoords(1,1,0;gy=100)))
    # BCE period, gigayear == 0, year > -1600, after January 10th
    @test DateCoords(4,15,-1520) == asGregorian(asJulian(DateCoords(4,15,-1520)))
    # BCE period, gigayear == 0, year > -1600, before January 10th
    @test DateCoords(1,5,-1520) == asGregorian(asJulian(DateCoords(1,5,-1520)))
    # BCE period, gigayear < 0, year < -1600, after January 10th
    @test DateCoords(4,15,-3000000; gy=-4) == asGregorian(asJulian(DateCoords(4,15,-3000000; gy=-4)))
    # BCE period, gigayear < 0, year < -1600, before January 10th
    @test DateCoords(1,1,-3000000; gy=-4) == asGregorian(asJulian(DateCoords(1,1,-3000000; gy=-4)))
    # CE Period, gigayear == 0, year < 1600, after January 10th
    @test DateCoords(4,15,1321) == asGregorian(asJulian(DateCoords(4,15,1321)))
    # CE Period, gigayear == 0, year < 1600, before January 10th
    @test DateCoords(1,5,1321) == asGregorian(asJulian(DateCoords(1,5,1321)))
    # CE Period, gigayear > 0, year > 1600, after January 10th
    @test DateCoords(4,15,3000000; gy=92) == asGregorian(asJulian(DateCoords(4,15,3000000; gy=92)))
    # CE Period, gigayear > 0, year > 1600, before January 10th
    @test DateCoords(1,1,3000000; gy=92) == asGregorian(asJulian(DateCoords(1,1,3000000; gy=92)))
    # From Swedish to Julian and back during the Swedish period
    @test DateCoords(9,10,1710; cal=2) == asCalendar(asCalendar(DateCoords(9,10,1710; cal=2), 1), 2)
    # From Gregorian to Julian and back during the transition period
    @test DateCoords(1,1,1912) == asCalendar(asCalendar(DateCoords(1,1,1912), 1), 0)
    # From a calendar where a date is a Julian date to one where it is a Gregorian date and back
    @test DateCoords(5,10,3013;cal=30200908) == asCalendar(asCalendar(DateCoords(5,10,3013;cal=30200908),
     19240308), 30200908)
    # From a calendar where a date is a Gregorian date to one where it is a Julian date and back
    @test DateCoords(1,1,1927;cal=19242802) == asCalendar(asCalendar(DateCoords(1,1,1927;cal=19242802),
      20140301), 19242802)
end
uoffus1 = UTCOffset()
uoffsw2 = UTCOffset(d=1,typ="s-w")
uoffuw3 = UTCOffset(d=1,h=10,typ="u-w")
uoffnp14 = UTCOffset(d=1,h=8,m=47,typ="sp1")
uoffsu5 = UTCOffset(d=1,h=8,m=42,s=35,typ="s-u")
uoffws6 = UTCOffset(d=1,h=8,m=14,s=22,hnd=15,typ="w-s")
uoffwu7 = UTCOffset(isneg=1,typ="w-u")
uoffus8 = UTCOffset(isneg=1,d=1,typ="u-s")
uoffsw9 = UTCOffset(isneg=1,h=4,m=2,typ="s-w")
uoffuw10 = UTCOffset(isneg=1,s=3,hnd=55,typ="u-w")
uoffus11 = UTCOffset(h=1,m=30,s=40,hnd=60,typ="u-s")
uoffsw12 = UTCOffset(h=23,m=35,s=30,hnd=50,typ="s-w")
uoffuw13 = UTCOffset(d=1,h=1,m=6,s=11,hnd=10,typ="u-w")
uoffnm114 = UTCOffset(h=3,hnd=4,typ="sm1")
uoffnp115 = UTCOffset(isneg=1,h=3,hnd=4,typ="sp1")
uoffsu16 = UTCOffset(isneg=1,h=2,typ="s-u")
uoffnp117 = UTCOffset(isneg=1,m=29,s=19,hnd=40,typ="sp1")
uoffwu18 = UTCOffset(isneg=1,h=1,m=2,typ="w-u")
uoffnp119 = UTCOffset(isneg=1,h=1,m=2,s=3,hnd=55,typ="wp1")
uoffnp120 = UTCOffset(isneg=1,m=24,s=29,hnd=50,typ="sp1")
@testset "UTCOffset formatting" begin
    @test string(uoffus1) == "00:00 u-s"
    @test string(uoffsw2) == "1d 00:00 s-w"
    @test string(uoffuw3) == "1d 10:00 u-w"
    @test string(uoffnp14) == "1d 08:47 sp1"
    @test string(uoffsu5) == "1d 08:42:35 s-u"
    @test string(uoffws6) == "1d 08:14:22.15 w-s"
    @test string(uoffwu7) == "00:00 w-u"
    @test string(uoffus8) == "-1d 00:00 u-s"
    @test string(uoffsw9) == "-04:02 s-w"
    @test string(uoffuw10) == "-00:00:03.55 u-w"
    @test string(uoffnm114) == "03:00:00.04 sm1"
end
@testset "UTCOffset comparisons and operations" begin
    @test uoffuw13 == uoffus11 + uoffsw12
    @test uoffuw13 == uoffsw12 + uoffus11
    @test uoffuw13 - uoffus11 == uoffsw12
    @test uoffuw13 - uoffsw12 == uoffus11
    @test uoffuw3 > uoffsw2
    @test uoffsw2 < uoffuw3
    @test uoffuw3 >= uoffsw2
    @test uoffsw2 <= uoffuw3
    @test string(uoffsw2 + uoffsw2) == "Nullable{UTCOffset}()"
    @test uoffnm114 == -uoffnp115
    @test uoffnp115 == -uoffnm114
    @test uoffus11 + uoffsu16 == uoffnp117
    @test uoffsu16 + uoffus11 == uoffnp117
    @test uoffuw10 + uoffwu18 == uoffnp119
    @test uoffwu18 + uoffuw10 == uoffnp119
    @test uoffsw12 - uoffsw2 == uoffnp120
    @test uoffsw2 - uoffsw12 == -uoffnp120
end
@testset "CalCoords formatting" begin
    @test string(CalCoords(3,15,2018)) == "15-Mar-2018 00:00:00.000000000 000000000 UTC"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 UTC"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 UTC"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 UTC"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,cal=18540302)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 UTC"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,cal=20540302)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 Universal Julian"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,cal=20540302)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 Universal Julian"
    @test string(CalCoords(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,cal=20540302)) ==
    "15-Mar-2018 05:04:03.123456789 987654321 Universal Julian"
    @test string(CalCoords(3,15,1708,h=5,min=4,s=3,ns=123456789,as=987654321,cal=2)) ==
    "15-Mar-1708 05:04:03.123456789 987654321 Universal Swedish"
    @test string(CalCoords(3,15,1708,h=5,min=4,s=3,ns=123456789,as=987654321,cal=2)) ==
    "15-Mar-1708 05:04:03.123456789 987654321 Universal Swedish"
end
@testset "showDatetime formatting" begin
    @test showDatetime(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,f=1,
    cal=20540302,tz="America/New York",bora = 0) ==
    "15-Mar-2018 05:04:03.123456789 987654321 America/New York Standard Julian"
    @test showDatetime(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,f=1,
    cal=20540302,tz="America/New York",bora = 1) ==
    "15-Mar-2018 05:04:03.123456789 987654321 America/New York Standard (-) Julian"
    @test showDatetime(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,f=1,
    cal=20540302,tz="America/New York",bora = 2) ==
    "15-Mar-2018 05:04:03.123456789 987654321 America/New York Standard (+) Julian"
    @test showDatetime(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,f=0,
    cal=20540302,tz="America/New York",bora = 2) ==
    "15-Mar-2018 05:04:03.123456789 987654321 Universal Julian"
    @test showDatetime(3,15,2018,h=5,min=4,s=3,ns=123456789,as=987654321,f=0,
    cal=20540302,tz="America/New York",bora = 1) ==
    "15-Mar-2018 05:04:03.123456789 987654321 Universal Julian"
end
#LocalCalCoordsDT contruction, formatting and translation
# Wall time during daylight savings time during the last period of a time zone,
#   extra digit provided for ns, unnecessary ba specification
lccdt1 = LocalCalCoordsDT(5,29,2018,"America/New_York",2,h=5,min=10,s=20,ns=253000000,ba=2)
# Universal time within the first leap second
lccdt2 = LocalCalCoordsDT(6,30,1972,"America/New_York",0,h=23,min=59,s=60,ns=500000000)
# Wall time within the first leap second
lccdt3 = LocalCalCoordsDT(6,30,1972,"America/New_York",2,h=19,min=59,s=60,ns=500000000)
# Standard time within the first leap second
lccdt4 = LocalCalCoordsDT(6,30,1972,"America/New_York",1,h=18,min=59,s=60,ns=500000000)
# Wall time that is ambiguous in wall frame of reference, but not standard frame of reference
lccdt5 = LocalCalCoordsDT(11,4,2018,"America/Boise",2,h=1,min=15,ba='b')
# Standard tiem that is ambiguous in standard frame of reference, but not wall frame of reference
lccdt6 = LocalCalCoordsDT(3,31,1991,"Europe/Zaporozhye",2,h=2,min=15)
lccdt6a = LocalCalCoordsDT(3,31,1991,"Europe/Zaporozhye",1,h=1,min=15,ba='b')
# Wall time that is ambiguous in wall and standard frames of reference
lccdt7 = LocalCalCoordsDT(8,24,1941,"Europe/Zaporozhye",2,h=22,min=30,ba='a')
@testset "LocalCalCoordsDT translation" begin
    @test string(lccdt1) == "29-May-2018 05:10:20.25 America/New_York"
    # Translate wall time to universal time
    @test string(translate(lccdt1;f=0)) ==
        "29-May-2018 09:10:20.25 UTC"
    # Translate wall time to standard time
    @test string(translate(lccdt1;f=1)) ==
        "29-May-2018 04:10:20.25 America/New_York Standard"
    @test string(lccdt5) == "04-Nov-2018 01:15:00.00 America/Boise (-)"
    # Translate ambiguous wall time to unambiguous standard time
    @test string(translate(lccdt5;f=1)) ==
        "04-Nov-2018 00:15:00.00 America/Boise Standard"
    # Translate unambiguous wall time to ambiguous standard time
    @test string(translate(lccdt6;f=1)) ==
        "31-Mar-1991 01:15:00.00 Europe/Zaporozhye Standard (+)"
    # Translate ambiguous standard time to unambiguous wall time
    @test string(translate(lccdt6a,f=2)) ==
        "31-Mar-1991 01:15:00.00 Europe/Zaporozhye"
    # Translate ambiguous wall time to ambiguous standard time
    @test string(translate(lccdt7;f=1)) ==
        "24-Aug-1941 22:30:00.00 Europe/Zaporozhye Standard (+)"
end
