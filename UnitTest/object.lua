local GLOBAL_NAME = "UnitTest"
local objectRegistry = {}
local _G = _G

local _M = setmetatable({}, {
	__index = function (t, key)
		if _G[key] then
			return _G[key]
		end
		assert(objectRegistry[key].env)
		return objectRegistry[key].env
	end
})

_G[GLOBAL_NAME] = _M

-- setfenv(1, _M)

local metatableName
local registryName
local tostringName

local origAssert = _G["assert"]
local function assert(exp, msg, ...)
	if not exp then
		error((msg or "Assertion failed!"):format(...), 2)
	end
end

function _M.NewTest(objType, metatableName, registryName, tostringName)
	assert(objType)
	assert(metatableName)
	assert(registryName)
	assert(tostringName)

	local env = setmetatable({}, {__index = _G})
	objectRegistry[objType] = {
		metatableName = metatableName,
		registryName = registryName,
		tostringName = tostringName,
		env = env
	}
	-- setfenv(2, env)
	return env
end

function _M.GetObjectType(objType)
	local reg = objectRegistry[objType]
	assert(reg, string.format("GetObjectType failed: invalid objType %s", tostring(objType)))
	metatableName = reg.metatableName
	registryName = reg.registryName
	tostringName = reg.tostringName
end

-- Test objects from luacl_object template
function _M.AssertObject(objType, object)
	_M.GetObjectType(objType)
	local registry = GetRegistry()
	assert(registry, "Cannot find system registry.")
	local objectReg = registry[registryName]
	assert(type(objectReg) == "table")
	assert(getmetatable(objectReg).__mode == "kv")
	local objectMeta = registry[metatableName]
	assert(type(objectMeta) == "table")
	assert(type(objectMeta.__index) == "table")

	assert(type(object) == "userdata")
	assert(tostring(object):find(tostringName .. ":%s"))
end

function _M.AssertRegEmpty(objType)
	collectgarbage()
	_M.GetObjectType(objType)
	local objectReg = GetRegistry()[registryName]
	assert(not next(objectReg), "Registry not empty")
end

function _M.AssertRegMatch(objType, t)
	collectgarbage()
	_M.GetObjectType(objType)
	local objectReg = GetRegistry()[registryName]
	for key, value in pairs(objectReg) do
		local addrKey = tostring(key):match("^userdata: (.+)$")
		assert(addrKey, "Illegal key in registry")
		local addrValue = tostring(value):match(tostringName .. ": (.+)$")
		assert(addrValue, "Illegal value in registry")
		assert(addrKey == addrValue, "Key and value address mismatch")
	end
	return _M.MatchTableValue(objectReg, t)
end

function _M.AssertInfoTable(info, keys)
	for key, expectedType in pairs(keys) do
		assert(type(info[key]) == expectedType, "Info table type mismatch: expecting %s, got %s: %s", expectedType, type(info[key]), info[key])
	end
end

function _M.MatchTableValue(tbl1, tbl2)
	assert(type(tbl1) == "table", "arg1 is not a table")
	assert(type(tbl2) == "table", "arg2 is not a table")
	local set1, set2 = {}, {}
	for key, value in pairs(tbl1) do
		set1[value] = key
	end
	for key, value in pairs(tbl2) do
		set2[value] = key
		assert(set1[value], "Table mismatch: key '%s' not exist in table1", tostring(value))
	end
	for key, value in pairs(tbl1) do
		assert(set2[value], "Table mismatch: key '%s' not exist in table2", tostring(value))
	end
end

function _M.IsLightUserdata(value)
	return type(value) == "number" and tostring(value) == ""
end
