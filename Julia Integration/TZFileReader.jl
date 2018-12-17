# Script to read IANA time zone files and create C header files defining the rules and time zones
module FileProcessor # Clears definitions
# The following is necessary to overload the operators for the new datatypes
using Base
using Nullables
using Dates
# Establish the directory containing the time zone files and the directory to store the
#  header file
# Open preferentially on the Z: drive, which will be the case when working at home
if isdir("Z:")
  TZFileDir = "Z:\\Ultimate Datetime Datatype\\TZ Database\\TZ Database\\"
  TZHeaderDir = "Z:\\Ultimate Datetime Datatype\\Ultimate Datetime\\LocalDatetimeDLL\\"
# Check for local directory on Windows machine
elseif isdir("C:")
  TZFileDir = "C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\TZ Database\\TZ Database\\"
  TZHeaderDir = "C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\Ultimate Datetime\\LocalDatetimeDLL\\"
# Check for OS / X
elseif isdir("/Volumes")
  TZFileDir = "/Volumes/Shared/Ultimate Datetime Datatype/TZ Database/TZ Database/"
  TZHeaderDir = "/Volumes/Shared/Ultimate Datetime Datatype/Ultimate Datetime/LocalDatetimeDLL/"
end
# Create a helper function to add a string to the end of a string array in a dictionary
function addString(nameDict::Dict{String,Array{String}}, key::AbstractString, incValue::AbstractString)
  if (haskey(nameDict, key))
    push!(nameDict[key], incValue)
  else
    nameDict[key] = [incValue]
  end
end
#
# Establish an array of TZ file names
TZFile = ["africa", "antarctica", "asia", "australasia", "europe", "northamerica", "southamerica"]
# Create a rule structure
struct TZRule
  name::String
  fromYear::UInt32 # First year rule is applicable
  toYear::UInt32 # Last year rule is applicable
  transMonth::UInt8 # Month the transition to or from daylight savings time takes place
  afterDayOfMonth::UInt8 # The day of the month the transition takes place, if week == 0,
                         #  or the day on which to begin counting if weekNumber > 0 &&
                         #  weekNumber < 6.
  weekNumber::UInt8      # 1 = first week of the month, 2 = second ..., 6 = last week of the month
  dayOfWeek::UInt8       # 0 = Sunday, 1 = Monday, etc.
  hourOfTransition::UInt8 # Hour of the day the transition is to take place
  minuteOfTransition::UInt8 # Minute of the day the transition is to take place
  secondOfTransition::UInt8 # Second of the day the transition is to take place
  hundredthOfTransition::UInt8 # Hundredth of a second of the day the transition is to take place
  basisOfTransition::UInt8 # 0 = universal time, 1 = local standard time, 2 = local wall time
  hoursSaved::UInt8 # Hours saved vs standard time after the transition
  minutesSaved::UInt8 # Minutes saved vs standard time after the transition
  abbrevChars::String # Characters used to modify the time zone abbreviation when the rule is in effect
end
# Create a time zone structure
struct TimeZone
  name::String
  # GMT Offset is the amount of time to add to UTC to get the standard time in this time zone
  isNegativeGMTOffset::UInt8 # 0 = positive offset, 1 = negative offset
  hoursOfGMTOffset::UInt8 # Number of hours of GMT offset
  minutesOfGMTOffset::UInt8 # Number of minutes of GMT offset
  secondsOfGMTOffset::UInt8 # Number of seconds of GMT offset
  hundredthsOfGMTOffset::UInt8 # Number of hundredths of a second of GMT Offset
  # A time zone can either have no daylight savings time rule, 1 daylight savings time rule, or
  #  a specified time offset.
  # If the rule filed is blank, use the offset.  When no rule is specified, set the offset to zero,
  #  to handle the second case, above.
  daylightSavingsTimeRule::String
  hoursSaved::UInt8 # Number of hours saved vs standard time after the transition
  minutesSaved::UInt8 # Number of minutes saved vs standard time after the transition
  # The following field contains the time zone abbreviation.  If it includes a slash,
  #  the field contains the abbreviations for both standard and daylight savings time.
  # If it includes %s, this is a place holder for the characters provided by the daylight
  #  savings time rules.
  abbrevChars::String
  # The until datetime indicates the date and time until which the time zone is effective,
  #  using the GMT offset and daylight savings time rules of the time zone.
  #  The default basis of this datetime is local wall time.
  # The basis can be modified to local standard time or to universal time.
  untilYear::UInt32 # Year of the datetime until which the time zone is effective.  No need to consider gigayear.
  untilMonth::UInt8 # Month of the datetime until which the time zone is effective.
  untilDayOfMonth::UInt8 # Day of the datetime until which the time zone is effective.
  untilHour::UInt8 # Hour of the datetime until which the time zone is effective.
  untilMinute::UInt8 # Minute of the datetime until which the time zone is effective.
  untilSecond::UInt8 # Second of the datetime until which the time zone is effective.
  untilHundredth::UInt8 # Hundredth of a second of the datetime until which the time zone is effecttive.
  untilBasis::UInt8 # 0 = universal time, 1 = local standard time, 2 = local wall time
end
# Read each of the TZ files and create the rule header file
# Declare the array to contain the rules
rulesArray = Array{String}(undef,3000,9)
nrules = 0
maxRNameLength = 0
maxAbbrevLength = 0
# The following regular expression will be used to identify candidate
#  fractional second GMT Offset specifications withing the comments in
#  the files.  The IANA software does not support fractional seconds, so
#  the values were rounded to whole seconds.
#
# We could manually edit the file to make the changes, but this would
#  have to be done every time we received updated files.
#
# Instead, we create an array of candidate fractional specifications
#  and substitute them if a matching (rounded) GMTOff is located
#
fracSecSpec = r"-?[0-9][0-9]?:[0-9][0-9]?:[0-9][0-9]?\.[0-9][0-9]?"
candidateFracSpec = Array{Int}(undef, 1000, 8)
numCandidates = 0
numMatches = 0
for file in TZFile
  infile = open(TZFileDir * file,"r")
  lines = readlines(infile)
  for line in lines
    # Search for fractional second specification on comment lines
    fracsec = match(fracSecSpec, line)
    if (length(line) > 1 && line[1] == '#' && fracsec != nothing)
      # Process as if it is a GMTOff field
      global numCandidates += 1
      # Check for a negative sign
      if (fracsec.match[1] == '-')
        isneg = 1
        gmtoff = fracsec.match[2:end]
      else
        isneg = 0
        gmtoff = fracsec.match
      end
      # Default all values to 0
      hour = 0
      minute = 0
      second = 0
      hundredth = 0
      # A '-' indicates 0
      if (gmtoff[1] != '-')
        x = split(gmtoff,":")
        hour = parse(Int,x[1])
        if (length(x) > 1)
          minute = parse(Int,x[2])
          if (length(x) == 3)
            # Split again to locate fractional seconds
            y = split(x[3],".")
            second = parse(Int,y[1])
            if (length(y) == 2)
              hundredth = parse(Int,y[2])
              if (hundredth < 10)
                hundredth *= 10
              end
            end
          end
        end
      end
      # Round the value to the next higher second.  Do this instead of
      #  actual rounding, since the rounding is not done consistently in the file
      rndhour = hour
      rndminute = minute
      rndsecond = second
      if (hundredth >= 0)  # Had been: if (hundredth >= 50)
        rndsecond += 1
        if (rndsecond == 60)
          rndsecond = 0
          rndminute += 1
          if (rndminute == 60)
            rndminute = 0
            rndhour += 1
          end
        end
      end
      # Store the values in the candidate fractional second GMTOFF array
      candidateFracSpec[numCandidates,1] = 0 # Number of times this entry has been matched
      candidateFracSpec[numCandidates,2] = rndhour
      candidateFracSpec[numCandidates,3] = rndminute
      candidateFracSpec[numCandidates,4] = rndsecond
      candidateFracSpec[numCandidates,5] = hour
      candidateFracSpec[numCandidates,6] = minute
      candidateFracSpec[numCandidates,7] = second
      candidateFracSpec[numCandidates,8] = hundredth
    end
    # The following line defends against Unicode characters in the string, which can generate StringIndexError
    if (length(line) > 4 && line[1:1] == "R" && line[2:4] == "ule")
      global nrules += 1
      nsubs = 0
      for sub in split(line)
        nsubs += 1
        # Check for maximum name length
        if (nsubs == 2)
          global maxRNameLength = max(maxRNameLength, length(sub))
        end
        # Check for maximum abberviation characters length
        if (nsubs == 10)
          global maxAbbrevLength = max(maxAbbrevLength, length(sub))
        end
        # Sometimes a comment and another word or phrase is added at the end of a rule
        if (sub == "#")
          break
        elseif (nsubs > 1)
          rulesArray[nrules, nsubs - 1] = sub
        end
      end
    end
  end
  close(infile)
end
print("Processed " * string(nrules) * " rule versions.\n")
print("Longest rule name: " * string(maxRNameLength) * " characters.\n")
print("Longest abberviation modifier: " * string(maxAbbrevLength) * " characters.\n")
print("Number of candidates for fractional second GMTOff specification: " *
          string(numCandidates) * "\n")
#
# Create an array of tzRuleVersions from the rulesArray
# Create a tzRules dictionary, which indexes into the tzRuleVersions array
#
tzRuleVersions = Array{TZRule}(undef,nrules)
tzRules = Dict{String,Array{String}}()
# Create an array of month abbreviations
months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
# Createt an array of day abbreviations
days = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
daysj = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]
# Process the rules array
maxSavedHours = 0
maxSavedMinutes = 0
maxTZ = ""
previousName = ""
for i = 1:nrules
  addString(tzRules,rulesArray[i,1], string(i-1))
  # **** FROM field ****
  # Check whether any of the from values are not integers between 1900 and 2100
  from = 0
  v = tryparse(Int,rulesArray[i,2])
  if (v == nothing)
    print("From field not an integer - " * rulesArray[i,2])
  else
    v = parse(Int,rulesArray[i,2])
    if (v < 1900 || v > 2100)
      print("From field out of range: " * string(v))
    else
      from = v
    end
  end
  # **** TO field ****
  # Check whether any of the to values are not integers between 1900 and 2100
  to = 0
  v = tryparse(Int,rulesArray[i,3])
  if (v == nothing)
    #Check for 'only'
    if (rulesArray[i,3] == "only")
      to = from
    elseif (rulesArray[i,3] == "max")
      to = 1000000000 # Will be translated to 99,999,999,999
    else
      print("To field is not valid - " * rulesArray[i,3] * "\n")
    end
  else
    v = parse(Int,rulesArray[i,3])
    if (v < 1900 || v > 2100)
      print("To field out of range: " * string(v) * "\n")
    else
      to = v
    end
  end
  # **** Ignore the TYPE field, as it is blank in all cases
  if (rulesArray[i,4] != "-")
    print(rulesArray[i] * "\n")
  end
  # **** IN field
  month = 0
  for j in eachindex(months)
    if (rulesArray[i,5] == months[j])
      month = j
      break
    end
  end
  # Check for invalid month
  if (month == 0)
    print("Invalid month - " * rulesArray[i,5] * "\n")
  end
  # **** ON field
  dom = 0
  week = 0
  day = 7
  v = tryparse(Int,rulesArray[i,6])
  if (v == nothing)
    if (occursin(">=",rulesArray[i,6]))
      # day>=dom specification
      for j in eachindex(days)
        if (rulesArray[i,6][1:3] == days[j])
          day = j - 1
          break
        end
      end
      # Check for invalid day
      if (day == 7)
        print("Invalid day - " * rulesArray[i,6][1:3] * "\n")
      end
      # Day of month begins in character 6
      dom = parse(Int, rulesArray[i,6][6:end])
      # Handle >= 1, 8, 15 and 22 as 1st, 2nd, 3rd, 4th, respectively to account for skipped days,
      #  even though it is unlikely there was a rule processed during a transition month
      if (dom % 7 == 1)
        week = div(dom,7) + 1
        dom = dom % 7
      end
    elseif (occursin("last",rulesArray[i,6]))
      # lastday specification
      week = 6
      for j in eachindex(days)
        if (rulesArray[i,6][5:7] == days[j])
          day = j - 1
          break
        end
      end
      # Check for invalid day
      if (day == 7)
        print("Invalid day - " * rulesArray[i,6][1:3] * "\n")
      end
    else
      print("Invalid day specification: " * rulesArray[i,6])
    end
  else
    # Day of month specification
    dom = parse(Int,rulesArray[i,6])
  end
  # **** AT field
  # Default to midnight
  hour = 0
  minute = 0
  second = 0
  hundredth = 0
  # Determin the AT time basis by checking the last character of the AT field
  basis = 2
  at = rulesArray[i,7]
  v = tryparse(Int,string(rulesArray[i,7][end]))
  if (v == nothing)
    if (rulesArray[i,7[end]] == 'u')
      basis = 0
    elseif (rulesArray[i,7][end] == 's')
      basis = 1
    else
      basis = 2
    end
    # Remove the last character for subsequent processing
    at = rulesArray[i,7][1:end-1]
  else
    at = rulesArray[i,7]
  end
  # A - indicates midnight
  if (at[1] != '-')
    x = split(at,":")
    hour = parse(Int,x[1])
    if (length(x) > 1)
      minute = parse(Int,x[2])
      if (length(x) == 3)
        # Split again to locate fractional seconds
        y = split(x[3],".")
        second = parse(Int,y[1])
        if (length(y) == 2)
          hundredth = parse(Int,y[2])
          if (hundredth < 10)
            hundredth *= 10
          end
        end
      end
    end
  end
  # **** SAVE field
  hourssaved = 0
  minutessaved = 0
  save = rulesArray[i,8]
  x = split(save,":")
  hourssaved = parse(Int,x[1])
  if (length(x) == 2)
    minutessaved = parse(Int,x[2])
  end
  if (60*hourssaved + minutessaved > 60*maxSavedHours + maxSavedMinutes)
    global maxSavedHours = hourssaved
    global maxSavedMinutes = minutessaved
    global maxTZ = rulesArray[i,1]
  end
  # **** LETTER field
  letters = (rulesArray[i,9] != "-" ? rulesArray[i,9] : "")
  # Create a TZRule
  tzRuleVersions[i] = TZRule(rulesArray[i,1], from, to, month, dom, week, day, hour, minute,
                  second, hundredth, basis, hourssaved, minutessaved, letters)
end
# Create another rule versions array, which has all of the versions of a rule contiguous
tzRuleVersions2 = Array{TZRule}(undef,nrules)
v2ind = 0
for key in keys(tzRules)
    val = tzRules[key]
    for j = 1:length(val)
        vind = parse(Int, val[j]) + 1
        global v2ind += 1
        tzRuleVersions2[v2ind] = tzRuleVersions[vind]
    end
end
# Reconstruct tzRules
tzRules = Dict{String,Array{String}}()
for i = 1:nrules
    addString(tzRules, tzRuleVersions2[i].name, string(i-1))
end
# Create a dictionary containing the rule sets
#   Contains the indices of the versions of a rule that are applicable for a time range
struct AppRuleVersionSet
    startYear:: UInt32
    endYear:: UInt32
    # Assumes up to 4 rule versions can be applicable at any time
    versionIndexArray:: Array{UInt32}
end
# Create a helper function to add a set of rule versions to the end of a rule version set array in a dictionary
function addRuleSet(ruleSetDict::Dict{String,Array{AppRuleVersionSet}}, key::AbstractString, incValue::AppRuleVersionSet)
  if (haskey(ruleSetDict, key))
    push!(ruleSetDict[key], incValue)
  else
    ruleSetDict[key] = [incValue]
  end
end
#
ruleVersionSet = Dict{String,Array{AppRuleVersionSet}}()
# Loop through the rule versions deriving the applicable rule sets
#  tzRules contains the indices of the rules
for key in keys(tzRules)
    firstInd =  parse(Int,tzRules[key][1]) + 1
    lastInd = parse(Int,tzRules[key][end]) + 1
    firstYear = 1000000000
    lastYear = 0
    # Find the earliest starting year and the latest end year for any rule version
    for i = firstInd:lastInd
        if (tzRuleVersions2[i].fromYear < firstYear)
            firstYear = tzRuleVersions2[i].fromYear
        end
        if (tzRuleVersions2[i].toYear > lastYear && tzRuleVersions2[i].toYear != 1000000000)
            lastYear = tzRuleVersions2[i].toYear
        end
    end
    # Now find all of the rule versions that start in the same year
    indexArray = Array{Int}(undef,0)
    for i = firstInd:lastInd
        if (tzRuleVersions2[i].fromYear == firstYear)
            # Change from 1 based array to 0 based Array
            push!(indexArray,i-1)
        end
    end
    # Loop through all of the years from the first to the last year looking for
    #   changes in applicable rules
    begYear = firstYear
    for i = firstYear+1:lastYear+1
        upperLocated = false
        deletedIndices = Array{Int}(undef,0)
        # Check for rule versions beyond their upper bound of applicability
        for j=1:length(indexArray)
            if (i > tzRuleVersions2[indexArray[j] + 1].toYear)
                # The rule version is no longer applicable
                if (!upperLocated)
                    #   The upper bound of the applicable rule set has been found
                    versionSet = AppRuleVersionSet(begYear, i-1, indexArray)
  #                  print(key, " D ", begYear, " ", i-1, " ", indexArray, "\n")
                    addRuleSet(ruleVersionSet,key,versionSet)
                    upperLocated = true
                    begYear = i
                end
                # Mark the rule versions for deletion
                push!(deletedIndices,j)
            end
        end
        # Delete the elements from the array in reverse order
        if (length(deletedIndices) > 0)
            for k = 1:length(deletedIndices)
                deleteat!(indexArray,deletedIndices[end+1-k])
            end
        end
        # Check for rule versions that have become applicable
        for j = firstInd:lastInd
            if (tzRuleVersions2[j].fromYear == i)
                # A new rule version has become applicable
                if (!upperLocated)
                    #   The upper bound of the applicable rule set has been found
                    #   Check whether the rule set is empty.  This can occur when there
                    #     is a gap - i.e., years with rules, followed by years with no
                    #     rules, followed by years with rules.  See Finland
                    if (length(indexArray) > 0)
                        versionSet = AppRuleVersionSet(begYear, i-1, indexArray)
  #                      print(key, " A ", begYear, " ", i-1, " ", indexArray, "\n")
                        addRuleSet(ruleVersionSet,key,versionSet)
                    end
                    begYear = i
                    upperLocated = true
                end
                # Add the rule version to the set
                # Change from 1 based array to 0 based array
                push!(indexArray,j-1)
            end
        end
    end
    # If any rules remain, they are perpetual
    if (length(indexArray) > 0)
        versionSet = AppRuleVersionSet(begYear, 1000000000, indexArray)
        addRuleSet(ruleVersionSet,key,versionSet)
#        print(key, " L ", begYear, " ", 1000000000, " ", indexArray, "\n")
    end
end
# Useful AppRuleVersionSet utilities
function getIndexArrayLength(x::AppRuleVersionSet)
    return length(x.versionIndexArray)
end
function combo(x::Array{AppRuleVersionSet})
    return maximum(map(getIndexArrayLength,x))
end
function getRuleVersionEffectiveMonth(x::UInt32)
    return tzRuleVersions2[x+1].transMonth
end
maxRuleVersions =  maximum(map(length,values(tzRules)))
numRules = length(keys(tzRules))
numRuleVersionSets = reduce(+,map(length,values(ruleVersionSet)))
maxRuleVersionSets = maximum(map(length,values(ruleVersionSet)))
maxRuleVersionsPerSet = maximum(map(combo,values(ruleVersionSet)))
print("Number of rules: ", string(numRules), "\n")
print("Maximum number of rule versions for a rule: ", string(maxRuleVersions),"\n")
print("Number of rule version sets: ", string(numRuleVersionSets),"\n")
print("Maximum number of rule version sets for a rule: ", string(maxRuleVersionSets),"\n")
print("Maximum number of rule versions in a set: ", string(maxRuleVersionsPerSet),"\n")
print("Maximum saved time (", maxTZ, "): ", string(maxSavedHours),
                              ":", string(maxSavedMinutes), "\n")
#
# Create the rules.h file
#
outfile = open(TZHeaderDir * "rules.h","w")
# Write the definition of the TZRule struct
structDef =
"#include <stdint.h>

#define maxRuleNameLength ** //  Maximum rule name length observed in IANA time zone files is %%
#define numRuleVersions xxxx // Number of rule versions in IANA time zone files
#define numRules yyyy // Number of rules in IANA time zone files
#define numRuleVersionSets zzzz // Number of rule version sets - derived from rule versions array
#define maxRuleVersionsPerSet qqqq // Maximum number of rule versions in a set
                                   //  derived from rule versions array
#define maxPeriodSegments rrrr // Maximum number of segments into which a year can be divided,
                               //  where each segment has a different s-w offset
                               //  maxRuleVersionsPerSet + 5

typedef struct
{
    // A time zone rule, created from the IANA timezone files
    char name[maxRuleNameLength]; // Rule name
    uint32_t fromYear; // First year rule is applicable.  No gigayears required as all
                       //  time zone date are from 1900 through 2100.
    uint32_t toYear;   //  Last year rule is applicable.
    uint8_t transMonth; // Month during which transiton to or from daylight savings time occurs
             						/* A rule is used to specify the day of the month during which the transitoin to
               							daylight savings time occurs.  The form of this rule is, for example,
               							the third Sunday on or after the 4th day of the month, or, generically,
               							the jth k day on or after the ith of the month, where i is afterDayOfMonth,
               							j is weekNumber and k is dayOfWeek. */
    uint8_t afterDayOfMonth; /* The day of the month the transition takes place, if weekNumber is 0,
                   								or the day on which to begin counting if weekNumber > 0.
                   								For example, if the third day of the month is a Sunday, and the
                   								rule is the first Sunday on or after the third of the month,
                   								the transition would occur on the third.  If it were the second
                   								Monday on or after the third of the month, it would occur on the
                   								11th.  */
    uint8_t weekNumber; // 1 = first week of the month, 2 = second week of the month..., 6 = last week of the month
    uint8_t dayOfWeek; // 0 = Sunday, 1 = Monday, etc.
    uint8_t hourOfTransition; // Hour of the day the transition is to take place
    uint8_t minuteOfTransition; // Minute of the day the transition is to take place
    uint8_t secondOfTransition; // Second of the day the transition is to take place
    uint8_t hundredthOfTransition; // Hundredth of a second of the day the transition is to take place
    uint8_t frameOfTransition; // 0 = universal time, 1 = local standard time, 2 = local wall time
    // Saved time is applied to the offset in the time zone applying the rule
    uint8_t hoursSaved; // Hours saved vs standard time after the transition
    uint8_t minutesSaved; // Minutes saved vs standard time after the transition
    char abbrevChars[^]; // Characters used to modify the time zone abbreviation when the rule is in effect, e.g., D or S
             						 //  maximum length observed in IANA time zone files is @
} TZRule;

typedef struct
{
  // A time zone rule set, containing the rule versions applicable for a range
  //  of years for a rule
  char ruleName[maxRuleNameLength]; // Rule name
  uint32_t fromYear; // First year the rule set is applicable
  uint32_t toYear; // Last year the rule set is applicable
  uint8_t numRuleVers; // Number of rule versions in the set
  uint32_t ruleVersionIndices[maxRuleVersionsPerSet]; // Indices of the rule versions in the set
} TZRuleSet;

static const TZRule TZRuleVersions[numRuleVersions] =
{\n"
# Replace the calculated values in structDef
structDef2 = replace(
              replace(
                replace(
                  replace(
                    replace(
                      replace(
                        replace(
                          replace(
                            replace(structDef,"**" => string(maxRNameLength + 1)),
                                "%%" => string(maxRNameLength)),
                                  "^" => string(maxAbbrevLength + 1)),
                                    "@" => string(maxAbbrevLength)),
                                      "xxxx" => string(nrules)),
                                       "yyyy" => string(numRules)),
                                        "zzzz" => string(numRuleVersionSets)),
                                         "qqqq" => string(maxRuleVersionsPerSet)),
                                          "rrrr" => string(maxRuleVersionsPerSet + 5))
# Write the beginning of the file
write(outfile, structDef2)
# Write each rule version
earliestFromYear = 1000000000
latestToYear = 0
for rule in tzRuleVersions2
  write(outfile,"    \u0022",rule.name,"\u0022, ", string(rule.fromYear), ", ",
                string(rule.toYear), ", ", string(rule.transMonth), ", ",
                string(rule.afterDayOfMonth), ", ", string(rule.weekNumber), ", ",
                string(rule.dayOfWeek), ", ", string(rule.hourOfTransition), ", ",
                string(rule.minuteOfTransition), ", ", string(rule.secondOfTransition), ", ",
                string(rule.hundredthOfTransition), ", ", string(rule.basisOfTransition), ", ",
                string(rule.hoursSaved), ", ",
                string(rule.minutesSaved), ", \u0022", rule.abbrevChars, "\u0022,",
                "\n")
  # Check for a situation where the wall time of the transition could be before
  #  the fromYear or after the toYear.  This can occur when the transition occurs on
  #  January 1st or December 31st and the basis is not wall.
  #
  global earliestFromYear = rule.fromYear < earliestFromYear ? rule.fromYear : earliestFromYear
  global latestToYear = (rule.toYear > latestToYear && rule.toYear != 1000000000) ?
   rule.toYear : latestToYear
  if (((rule.transMonth == 12 || rule.transMonth == 1) && rule.basisOfTransition !=2
      && rule.dayOfWeek != 7) || # In Dec or Jan with a dayOfWeek specification
      (rule.transMonth == 12 && rule.afterDayOfMonth == 31 && rule.basisOfTransition != 2) ||
      (rule.transMonth == 1 && rule.afterDayOfMonth == 1 && rule.basisOfTransition != 2))
      print("** Potential transition issue: ", rule.name, " Month: ", rule.transMonth,
      " Day: ", rule.afterDayOfMonth, " Basis: ", rule.basisOfTransition,
      " Day of week: ", rule.dayOfWeek, "\n")
  end
end
# Write the end of the array definition
write(outfile, "};")
print("Earliest rule 'from year': ", earliestFromYear, "\n")
print("Latest rule 'to year': ", latestToYear, "\n")
# Check for out of sequence rule versions
for key in keys(tzRules)
  val = tzRules[key]
  if (length(val) > 1)
    for i = 2:length(val)
      if (parse(Int, val[i]) != parse(Int, val[i-1]) + 1)
        print("Out of sequence rule version - Rule: ", key, " Index: ", string(i),"\n")
      end
    end
  end
end
# write the rule name array
structDef =
"\n
static const char TZRules[numRules][maxRuleNameLength] =
{\n"
write(outfile,structDef)
# Write each rule name
for key in keys(tzRules)
  write(outfile,"    \u022", key, "\u022,\n")
end
write(outfile, "};")
# Write the rule index array
structDef =
"\n
static const uint32_t TZRuleIndices[numRules][2] =
{\n"
write(outfile,structDef)
# Write each rule index
for key in keys(tzRules)
  write(outfile,"    ", tzRules[key][1], ", ", tzRules[key][end], ",\n")
end
write(outfile, "};")
# Write the rule set array
structDef =
"\n
static const TZRuleSet TZRuleVersionSets[numRuleVersionSets] =
{\n"
write(outfile,structDef)
# Write each rule set in the order of the rules array, sorted by
#   the month each rule version becomes effective
for key in keys(tzRules)
  versionSets = ruleVersionSet[key]
  for set in versionSets
    # Create the output array
    effectiveMonths = map(getRuleVersionEffectiveMonth, set.versionIndexArray)
    p = sortperm(effectiveMonths)
    # Check whether any of the versions have the same effective month
    for i = 2:length(p)
      if (p[i] == p[i-1])
        print("***** Duplicate effective month! Rule = ", key, " ", set.startYear,
          " - ", set.endYear, "\n")
      end
    end
    outArray = set.versionIndexArray[p]
    # Lengthen the index array to simplify output
    while (length(outArray) < maxRuleVersionsPerSet)
      push!(outArray, 0)
    end
    write(outfile,"    \u022", key, "\u022, ", string(set.startYear), ", ",
      string(set.endYear), ", ", string(length(set.versionIndexArray)), ", ")
    for i = 1:length(outArray)
      write(outfile, string(outArray[i]), ", ")
    end
    write(outfile, "\n")
  end
end
write(outfile, "};")
# Write the rule set index array in the same order as the rule name array
structDef =
"\n
static const uint32_t TZRuleSetIndices[numRules][2] =
{\n"
write(outfile,structDef)
firstind = 0
for key in keys(tzRules)
  lastind = firstind + length(ruleVersionSet[key]) - 1
  write(outfile,"    ",  string(firstind), ", ", string(lastind), ",\n")
  global firstind = lastind + 1
end
write(outfile, "};")
close(outfile)
# Reread each of the TZ files and create the timezone header file
# Declare the array to contain the time zones
tzArray = Array{String}(undef,3000,8)
ntz = 0
ntznorep = 0
maxNameLength = 0
maxAbbrevLength = 0
maxAbbrev = ""
#  Create a dictionary with the names for each time zone.
#      Initialie it in this section and finish in links secondOfTransition
TZNames = Dict{String,Array{String}}()
for file in TZFile
  infile = open(TZFileDir * file,"r")
  lines = readlines(infile)
  withinTZ = false
  for line in lines
    # If not already within a time zone, look for the first line of a time zone
    if (!withinTZ)
        if (length(line) > 4 && line[1:1] == "Z" && line[1:4] == "Zone")
            # This is the first line of a time zone specification
            global ntz += 1
            global ntznorep += 1
            withinTZ = true
            nsubs = 0
            # Blank elements 5 through 8, so they are not undefined
            tzArray[ntz,5:8] .= ""
            for sub in split(line)
                nsubs += 1
                # Check for maximum name length
                if (nsubs == 2)
                    global maxNameLength = max(maxNameLength, length(sub))
                    addString(TZNames,sub,sub)
                end
                # Check for maximum abberviation characters length
                if (nsubs == 5 && length(sub) > maxAbbrevLength)
                    global maxAbbrevLength = length(sub)
                    global maxAbbrev = sub
                end
                # Sometimes a comment and another word or phrase is added at the end of a time zone specification
                if (sub == "#")
                    break
                elseif (nsubs > 1)
                    tzArray[ntz, nsubs - 1] = sub
                end
            end
            # Check for 1 line time zone specification
            if (nsubs == 5)
                # No UNTIL field
                withinTZ = false
            end
        end
    else
        # Within a time zone.  Search for continuation lines
        # Every continuation line must have at least GMTOFF, RULES and FORMAT fields
        # Every time zone must end with a blank UNTIL field
        if (length(line) > 0 && line[1] != '#' && occursin(":", line))
            # It is a good assumption that a non-comment line with a colon is a time zone continuation lines
            # Consider a continuation line to be a new time zone with the same name
            ntz += 1
            nsubs = 0
            tzArray[ntz, 1] = tzArray[ntz - 1, 1]
            # Blank elements 5 through 8, so they are not undefined
            tzArray[ntz,5:8] .= ""
            for sub in split(line)
                nsubs += 1
                # Check for maximum abberviation characters length
                if (nsubs == 3 && length(sub) > maxAbbrevLength)
                    global maxAbbrevLength = length(sub)
                    global maxAbbrev = sub
                end
                # Sometimes a comment and another word or phrase is added at the end of a time zone specification
                if (sub == "#")
                    nsubs -= 1
                    break
                else
                    tzArray[ntz, nsubs + 1] = sub
                end
            end
            # Check end of time zone specification
            if (nsubs == 3)
                # No UNTIL field
                withinTZ = false
            end
        end
    end
  end
  close(infile)
end
print("Processed " * string(ntznorep) * " time zones.\n")
print("Processed " * string(ntz) * " time zone versions.\n")
print("Longest time zone name: " * string(maxNameLength) * " characters.\n")
print("Longest time zone abberviation (" * maxAbbrev * "): " * string(maxAbbrevLength) * " characters.\n")
#
# Create an array of TimeZones from the tzArray
#
timezones = Array{TimeZone}(undef, ntz)
#  Create a dictionary of time zone version indices.
TZIndices = Dict{String,Array{String}}()
# Process the time zone array
maxGMTHours = 0
maxGMTMinutes = 0
maxNegGMTHours = 0
maxNegGMTMinutes = 0
maxTZ = ""
maxNegTZ = ""
for i = 1:ntz
  # Populate the index dictionary
  addString(TZIndices,tzArray[i,1],string(i-1))
  # **** GMTOFF field ****
  # Check for a negative sign
  if (tzArray[i,2][1] == '-')
    isneg = 1
    gmtoff = tzArray[i,2][2:end]
  else
    isneg = 0
    gmtoff = tzArray[i,2]
  end
  # Default all values to 0
  hour = 0
  minute = 0
  second = 0
  hundredth = 0
  # A '-' indicates 0
  if (gmtoff[1] != '-')
    x = split(gmtoff,":")
    hour = parse(Int,x[1])
    if (length(x) > 1)
      minute = parse(Int,x[2])
      if (isneg == 0 && 60*hour + minute > 60*maxGMTHours + maxGMTMinutes)
        global maxGMTHours = hour
        global maxGMTMinutes = minute
        global maxTZ = tzArray[i,1]
      elseif (isneg == 1  && 60*hour + minute > 60*maxNegGMTHours + maxNegGMTMinutes)
        global maxNegGMTHours = hour
        global maxNegGMTMinutes = minute
        global maxNegTZ = tzArray[i,1]
      end
      if (length(x) == 3)
        # Split again to locate fractional seconds
        y = split(x[3],".")
        second = parse(Int,y[1])
        if (length(y) == 2)
          hundredth = parse(Int,y[2])
          if (hundredth < 10)
            hundredth *= 10
          end
        end
      end
    end
  end
  # Try to match the offset to one of the candidates fractional second
  #   GMT offset specifications.  Rounding isn't always done consistently
  #   in the files, so check both rounded and truncated values.
  for icand = 1:numCandidates
    if ((candidateFracSpec[icand,2] == hour && candidateFracSpec[icand,3] == minute &&
          candidateFracSpec[icand,4] == second) || (candidateFracSpec[icand,5] == hour &&
          candidateFracSpec[icand,6] == minute && candidateFracSpec[icand,7] == second))
      # Found a match
      candidateFracSpec[icand,1] += 1
      global numMatches += 1
      # Set the values to the more precise fractional second specification
      hour = candidateFracSpec[icand,5]
      minute = candidateFracSpec[icand,6]
      second = candidateFracSpec[icand,7]
      hundredth = candidateFracSpec[icand,8]
      break
    end
  end
  # **** RULES field ****
  # Default the hours and minuts saved to zero
  hourssaved = 0
  minutessaved = 0
  rule = tzArray[i,3]
  if (rule == "-")
    dstr = ""
  elseif (occursin(":",rule))
    x = split(rule,":")
    hourssaved = parse(Int,x[1])
    if (length(x) == 2)
      minutessaved = parse(Int,x[2])
    end
    dstr = ""
  else
    # Reference to a rule name
    dstr = rule
  end
  # **** FORMAT field ****
  format = (tzArray[i,4] != "-" ? tzArray[i,4] : "")
  # **** UNTIL field ****
  # Default all fields, except for year, to earliest possible value
  untilMonth = 1
  untilDayOfMonth = 1
  untilHour = 0
  untilMinute = 0
  untilSecond = 0
  untilHundredth = 0
  untilBasis = 2
  # If the field is blank, the time zone is effective until max time
  if (tzArray[i,5] == "")
    untilYear = 1000000000
  else
    # At least a year is specified
    untilYear = parse(Int,tzArray[i,5])
    # Next field, if specified, is the month
    if (tzArray[i,6] != "")
      for j in eachindex(months)
        if (tzArray[i,6] == months[j])
          untilMonth = j
          break
        end
      end
    end
    # Next field, if specified, is the day of the month
    if (tzArray[i,7] != "")
      # Sometimes, this field contains a 'lastddd' specification
      if (occursin("last",tzArray[i,7]))
        # Use date functions to resolve to an integer
        # Extract the day of the week
        for j in eachindex(daysj)
          if (tzArray[i,7][5:7] == daysj[j])
            # Find the day of the month of the last day of the until year and month
            untilDayOfMonth = Dates.dayofmonth(Dates.tolast(Dates.Date(untilYear,
                                untilMonth, 1), j))
            break
          end
        end
        # Sometimes, this field contains a 'ddd>=1' specification
      elseif (occursin(">=1",tzArray[i,7]))
        # Use date functions to resolve to an integer
        # Extract the day of the week
        for j in eachindex(daysj)
          if (tzArray[i,7][1:3] == daysj[j])
            # Find the day of the month of the last ddd of the until year and month
            untilDayOfMonth = Dates.dayofmonth(Dates.tofirst(Dates.Date(untilYear,
                                untilMonth, 1), j))
            break
          end
        end
      else
        untilDayOfMonth = parse(Int,tzArray[i,7])
      end
    end
    # Next field, if specified, contains the time and basis
    # Determin the UNTIL time basis by checking the last character of the AT field
    if (tzArray[i,8] != "")
      v = tryparse(Int,string(tzArray[i,8][end]))
      if (v == nothing)
        if (tzArray[i,8][end] == 'u')
          untilBasis = 0
        elseif (tzArray[i,8][end] == 's')
          untilBasis = 1
        else
          untilBasis = 2
        end
        # Remove the last character for subsequent processing
        untilTime = tzArray[i,8][1:end-1]
      else
        untilTime = tzArray[i,8]
      end
      # A - indicates midnight
      if (untilTime[1] != '-')
        x = split(untilTime,":")
        untilHour = parse(Int,x[1])
        if (length(x) > 1)
          untilMinute = parse(Int,x[2])
          if (length(x) == 3)
            # Split again to locate fractional seconds
            y = split(x[3],".")
            untilSecond = parse(Int,y[1])
            if (length(y) == 2)
              untilHundredth = parse(Int,y[2])
              if (untilHundredth < 10)
                untilHundredth *= 10
              end
            end
          end
        end
      end
    end
  end
  # Create a TimeZone
  timezones[i] = TimeZone(tzArray[i,1], isneg, hour, minute, second, hundredth, dstr,
                  hourssaved, minutessaved, format, untilYear, untilMonth,
                  untilDayOfMonth, untilHour, untilMinute, untilSecond,
                  untilHundredth, untilBasis)
end
function getUntilYear(x::TimeZone)
    return x.untilYear
end
print("Earliest until year: ", minimum(map(getUntilYear, timezones)), "\n")
print("Number of candidate fractional second GMTOff specifications matched: ", string(numMatches)," \n")
print("Maximum positive GMT Offset (", maxTZ, "): ", string(maxGMTHours),
                              ":", string(maxGMTMinutes), "\n")
print("Maximum negative GMT Offset = (", maxNegTZ, "): -", string(maxNegGMTHours), ":", string(maxNegGMTMinutes), "\n")
#
# Process Links
#
# Reread the files
nlinks = 0
maxLinkLength = 0
for file in TZFile
  infile = open(TZFileDir * file,"r")
  lines = readlines(infile)
  for line in lines
      # Search for lines starting with Link
      if (length(line) > 4)
          subs = split(line)
          if (subs[1] == "Link")
            # Found a link
              global nlinks += 1
              addString(TZNames,subs[2],subs[3])
              global maxLinkLength = max(maxLinkLength, length(subs[3]))
          end
      end
  end
  close(infile)
end
maxLinks =  maximum(map(length,values(TZNames))) - 1
print("Processed " * string(nlinks) * " links.\n")
print("Longest link name: " * string(maxLinkLength) * " characters.\n")
# Find the greatest number of links for a time zone
print("Most links for a time zone: " * string(maxLinks),"\n")
#
# Create the timezones.h file
#`
outfile = open(TZHeaderDir * "timezones.h","w")
# Write the definition of the TimeZone struct
structDef =
"#include <stdint.h>

#define maxTZNameLength cccc // Maximum time zone / link name length observed in IANA time zone files is qq
#define numTimeZones yyyy // Number of time zones in IANA time zone files
#define numTimeZoneVersions xxxx // Number of time zone versions in IANA time zone files
#define maxNamesPerTimeZone bbbb // Maximum number of names for a time zone observed in IANA time zone files

typedef struct
{
    // A time zone version, created from the IANA time zone files
    //  Every continuation line in the time zone file is another time zone version
    char name[maxTZNameLength]; // Time zone name
    // GMT Offset is the amount of time to add to UTC to get the standard time in this time zone
    uint8_t isNegativeGMTOffset;  // 0 = positive offset, 1 = negative offset
    uint8_t hoursOfGMTOffset;  // Number of hours of GMT offset
    uint8_t minutesOfGMTOffset;  // Number of minutes of GMT offset
    uint8_t secondsOfGMTOffset;  // Number of seconds of GMT offset
    uint8_t hundredthsOfGMTOffset;  // Number of hundredths of a second of GMT Offset
    // A time zone can either have no daylight savings time rule, 1 daylight savings time rule, or
    //  a specified time offset.
    // If the rule field is blank, use the offset.  When no rule is specified, set the offset to zero,
    //  to handle the second case, above.
    char daylightSavingsTimeRule[maxRuleNameLength];
    uint8_t hoursSaved;  // Number of hours saved vs standard time after the transition
    uint8_t minutesSaved;  // Number of minutes saved vs standard time after the transition
    // The following field contains the time zone abbreviation.  If it includes a slash,
    //  the field contains the abbreviations for both standard and daylight savings time.
    // If it includes %s, this is a place holder for the characters provided by the daylight
    //  savings time rules.
    char abbrevChars[zzz]; // Maximum length observed in IANA time zone files is @
    // The until datetime indicates the date and time until which the time zone is effective,
    //  using the GMT offset and daylight savings time rules of the time zone.
    //  The default basis of this datetime is local wall time.`
    // The basis can be modified to local standard time or to universal time.
    uint32_t untilYear;  // Year of the datetime until which the time zone is effective.  No need to consider gigayear.
    uint8_t untilMonth;  // Month of the datetime until which the time zone is effective.
    uint8_t untilDayOfMonth;  // Day of the datetime until which the time zone is effective.
    uint8_t untilHour;  // Hour of the datetime until which the time zone is effective.
    uint8_t untilMinute;  // Minute of the datetime until which the time zone is effective.
    uint8_t untilSecond;  // Second of the datetime until which the time zone is effective.
    uint8_t untilHundredth;  // Hundredth of a second of the datetime until which the time zone is effecttive.
    uint8_t untilFrame; // 0 = universal time, 1 = local standard time, 2 = local wall time
} TimeZone;

static const TimeZone TimeZoneVersions[numTimeZoneVersions] =
{\n"
# Replace the calculated values in structDef
structDef2 = replace(
              replace(
                replace(
                  replace(
                    replace(structDef,"**" => string(maxNameLength + 1)),
                              "qq" => string(max(maxNameLength, maxLinkLength))),
                                "zzz" => string(maxAbbrevLength + 1)),
                                  "@" => string(maxAbbrevLength)),
                                    "xxxx" => string(ntz))
structDef3 = replace(
               replace(
                  replace(structDef2,"cccc" => string(max(maxNameLength+1, maxLinkLength+1))),
                                      "bbbb" => string(maxLinks+1)),
                                        "yyyy" => string(ntznorep))
# Write the beginning of the file
write(outfile, structDef3)
# Write each time zone
for tz in timezones
   write(outfile,"    \u0022",tz.name,"\u0022, ", string(tz.isNegativeGMTOffset), ", ",
                string(tz.hoursOfGMTOffset), ", ", string(tz.minutesOfGMTOffset), ", ",
                string(tz.secondsOfGMTOffset), ", ", string(tz.hundredthsOfGMTOffset), ", ",
                "\u0022", tz.daylightSavingsTimeRule, "\u0022, ", string(tz.hoursSaved), ", ",
                string(tz.minutesSaved), ", \u0022", tz.abbrevChars, "\u0022, ",
                string(tz.untilYear), ", ", string(tz.untilMonth), ", ",
                string(tz.untilDayOfMonth), ", ", string(tz.untilHour), ", ",
                string(tz.untilMinute), ", ", string(tz.untilSecond), ", ",
                string(tz.untilHundredth), ", ", string(tz.untilBasis), ",\n")
    # Check for potential transition issues.  Specifically, where the Until datetime is
    #  either on 12/31 or 1/1 and the bais is not wall time.  In these cases, the until time
    #  could move to the next or previous year when converted to wall time, complicating the
    #  matching with the year of the rule set
    if (tz.untilBasis != 2 && tz.daylightSavingsTimeRule != "" &&
        ((tz.untilMonth == 12 && tz.untilDayOfMonth == 31) ||
        (tz.untilMonth == 1 && tz.untilDayOfMonth == 1)))
        print("*** Potential transition issue in time zone ", tz.name, " - month: ",
        tz.untilMonth, " day: ", tz.untilDayOfMonth, "\n")
    end
end
# Write thte end of the array
write(outfile, "};")
# Create a TZNames array in the timezone.h file
structDef =
"

static const char TimeZones[numTimeZones][maxNamesPerTimeZone][maxTZNameLength] =
{\n"
write(outfile, structDef)
# Write each link
k = collect(keys(TZNames))
for i in eachindex(k)
    v = TZNames[k[i]]
    lenThisLine = 0
    write(outfile,  "    \u0022", k[i], "\u0022, ")
    lenThisLine += 8 + length(k[i])
    for j in eachindex(v)
      if (j > 1)
        write(outfile, "\u0022", v[j], "\u0022, ")
        lenThisLine += 4 + length(v[j])
        # Limit line length
        if (lenThisLine > 100)
            write(outfile, "\n        ")
            lenThisLine = 8
        end
      end
    end
    if length(v) < maxLinks + 1
        if (lenThisLine + 4 * (maxLinks + 1 - length(v)) > 110)
            write(outfile, "\n        ")
        end
        for m = length(v) + 1 :  maxLinks + 1
            write(outfile, "\u0022\u0022, ")
        end
    end
    write(outfile, "\n")
end
# Write thte end of the array
write(outfile, "};")
structDef =
"

static const uint32_t TimeZoneIndices[numTimeZones][2] =
{\n"
write(outfile, structDef)
# Write each time zone index
for key in keys(TZIndices)
  write(outfile,"    ", TZIndices[key][1], ", ", TZIndices[key][end], ",\n")
  # Check whether a dstr has been specified for the first version of a time zone
  firstVer = parse(Int, TZIndices[key][1])
  if (timezones[firstVer+1].daylightSavingsTimeRule != "")
    print("***** First version of time zone ", key, " has a dstr.\n")
  end
end
write(outfile, "};")
close(outfile)
end # Ends module FileProcessor
