# JuliaCon 2019 Demonstration
include("C:\\Users\\jay\\Documents\\Ultimate Datetime Datatype\\Ultimate Datetime\\Julia Integration\\UltimateDT.jl")
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
push!(code,CodeFragment("Addition - add uncertainties, take minimum precision",:(uf1 + uf2)))
push!(code,CodeFragment("Subtraction - add uncertainties, take minimum precision",:(uf1 - uf2)))
push!(code,CodeFragment("Multiplication - add % uncertainties, take minimum digits precision",:(uf1 * uf2)))
push!(code,CodeFragment("Division - add % uncertainties, take minimum digits precision",:(uf1 / uf2)))
map(demo_code, code)
# **** Check rules for mult and div precisions.  Might be based on uncertainty. Show exact and zero special cases.
uf1 = UncertainFloat64(1005.25,p=.01,u=2)
uf2 = UncertainFloat64(4,p=1,u=0)
uf1*uf2
