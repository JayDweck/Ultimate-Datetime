y = Dict{String,Array{String}}()
y["first"] = ["abc", "def"]
print(y["first"], "\n")
y["first"] = vcat(y["first"], ["ghi"])
print(y["first"], "\n")
push!(y["first"], "Ronn")
print(y["first"], "\n")
