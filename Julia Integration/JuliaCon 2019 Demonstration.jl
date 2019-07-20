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
# Create an array to contain the code fragments
code = Array{CodeFragment,1}()
# Demonstrate Julian to Gregorian transition
push!(code,CodeFragment("Create a UTCDatetime just before the earliest transtion to the Gregorian calendar.",
    :(utcj = UTCDatetime(y=1582,m=10,d=4,h=23,min=0,p=0,u=1))))
push!(code,CodeFragment("Attempt to create a UTCDatetime during the skipped period.",
    :(utcs = UTCDatetime(y=1582,m=10,d=5,h=1,min=0,p=0,u=1))))
push!(code,CodeFragment("Create a UTCDatetime just after the earliest transition to the Gregorian calendar.",
    :(utcg = UTCDatetime(y=1582,m=10,d=15,h=1,min=0,p=0,u=1))))
push!(code,CodeFragment("Very little time elapsed between those two dates!",
    :(utcg - utcj)))
push!(code,CodeFragment("The Julian calendar continued on - the last country to adopt the Gregorian calendar was Greence in 1923.",
    :(utcj2 = UTCDatetime(y=1582,m=10,d=5,h=1,min=0,p=0,u=1,c=1))))
push!(code,CodeFragment("For every Gregorian datetime, there is a corresponding Julian datetime.",
    :(utcg == utcj2)))
push!(code,CodeFragment("Sweden has its own, bizarre calendar.",
    :(UTCDatetime(y=1712,m=2,d=30,h=1,min=0,p=0,u=1,c=2))))
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
# *** Leap seconds ***
# UTC Datetime Formatting
push!(code,CodeFragment("Formatting is automatic based on precision.",
    :(for i = -18:15;
        utc = UTCDatetime(gy=99,y=999999999,m=12,d=31,h=23,min=59,s=59,ns=999999999,as=999999999,p=i,u=1);
        printstyled(stdout,"\n", utc,color=:light_cyan);
      end)))
map(demo_code, code)
nothing
