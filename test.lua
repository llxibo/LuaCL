function write_table( table, indent )
	local file = io.stdout
	local indent = indent or ""
	for key, value in pairs(table) do
		file:write(indent)
		if type(key) == "number" then
			file:write("[", key, "]")
		elseif type(key) == "string" then
			file:write("[", string.format("%q", key), "]")
		end
		file:write(" = ")
		if type(value) == "number" then
			file:write(value)
		elseif type(value) == "string" then
			file:write(string.format("%q", value))
		elseif type(value) == "table" then
			file:write("{\n")
			write_table(value, indent .. "\t")
			file:write(indent)
			file:write("}")
		elseif type(value) == "boolean" then
			file:write(tostring(value))
		else
			file:write("nil")
		end
		file:write(",\n")
	end
end

function dump_table( table, name )
	print(name .. " = {")
	write_table(table, "  ")
	print("}")
end

print("LuaCL environment test\n")

local start1 = os.clock()
local numPlatforms = GetNumPlatforms();
local start2 = os.clock()
print("First call GetNumPlatforms:", start2 - start1)
local numPlatforms = GetNumPlatforms();
local start3 = os.clock()
print("Second call GetNumPlatforms:", start3 - start2)
print("Total platforms: ", numPlatforms)

for platform = 1, numPlatforms do
	local cl_info = GetPlatformInfo(platform)
	print("Dumping result ...")
	dump_table(cl_info, "cl_info")

	print("Platform", platform, "has device:", GetNumDevices(platform))

	for device = 1, GetNumDevices(platform) do
		print("Device", device)
		local deviceInfo = GetDeviceInfo(platform, device)
		dump_table(deviceInfo, "deviceInfo")
	end
end

print("Asserting invalid args GetPlatformInfo ... ")
local cl_info = GetPlatformInfo(-1)
assert(cl_info == nil)
local cl_info = GetPlatformInfo(numPlatforms + 1)
assert(cl_info == nil)
local ret = pcall(GetPlatformInfo, "test")
assert(not ret)
local cl_info = GetPlatformInfo(0.9)
assert(cl_info == nil)
print("Assertion passed.")

print("Asserting invalid args GetDeviceInfo ... ")
local deviceInfo = GetDeviceInfo(0, 1)
assert(deviceInfo == nil)
if numPlatforms > 1 then
	local numDevices = GetNumDevices(1)
	local deviceInfo = GetDeviceInfo(1, numDevices + 1)
	assert(deviceInfo == nil)
end
print("Assertion passed.")

print("\nFinished")
