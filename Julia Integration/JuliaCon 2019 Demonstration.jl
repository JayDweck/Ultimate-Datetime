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
    printstyled(stdout, " *** ", codeFragment.description, " ***\n", bold=true, color=:light_green)
    print(stdout, string(codeFragment.expression), "  ")
    printstyled(stdout, eval(codeFragment.expression), "\n", color=:light_cyan)
    readline() # Wait for a carraige return
    nothing
end
# wait_for_key(prompt) = (print(stdout, prompt); readline(); nothing)
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
# **** Comparisons ***
# Demonstrate that relative time is different from UTCDatetime
push!(code,CodeFragment("Create a UTCDatetime with default precision and uncertainty.",:(utc1 = UTCDatetime(y=2019,m=7,d=23,h=11,min=45))))
push!(code,CodeFragment("Create a 2nd UTCDatetime with default precision and uncertainty.",:(utc2 = UTCDatetime(y=2019,m=7,d=23,h=10,min=45))))
push!(code,CodeFragment("Subtract the 2 UTCDatetimes to produce a relative datetime.",:(rel1 = utc1 - utc2)))
push!(code,CodeFragment("Relative datetimes can be positive or negative.",:(rel2 = utc2 - utc1)))
# *** Leap seconds ***
map(demo_code, code)
nothing
