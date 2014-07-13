function PrintTable(t, name, level, levelstr)
	local levelstr = levelstr or "    "
	local level = level or ""
	local nextlevel = level .. levelstr
	print(level .. "[" .. name .. "] = {")
	for key, value in pairs(t) do
		if type(value) == "table" then
			PrintTable(value, key, nextlevel, levelstr)
		else
			print(nextlevel .. key .. " = \"" .. tostring(value), "\"")
		end
	end
	print(level .. "}")
end

print("LuaCL environment test\n")

local numPlatforms = GetNumPlatforms();
print("Total platforms: ", numPlatforms)

for index = 1, numPlatforms do
	local cl_info = GetPlatformInfo(index)
	print("Dumping result ...")
	PrintTable(cl_info, "cl_info")

	print("Platform", index, "has device:", GetNumDevices(index))
end

print("Assertion on GetPlatformInfo ... ")
local cl_info = GetPlatformInfo(-1)
assert(cl_info == nil)
local cl_info = GetPlatformInfo(numPlatforms + 1)
assert(cl_info == nil)
print("Assertion passed")

print("\nFinished")
