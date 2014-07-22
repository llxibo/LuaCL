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
			file:write(tostring(value))
		end
		file:write(",\n")
	end
end

function dump_table( table, name )
	print(name .. " = {")
	write_table(table, "  ")
	print("}")
end
