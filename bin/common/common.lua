----------------------------------------------------------------------------------
--                全局函数 (注: 不可以热更新，一般不需要改变)
----------------------------------------------------------------------------------
g_log_level = 1

function LOG_DEBUG(str, ...)
    if 0 < g_log_level  then
        return
    end

    octopus.write_log(string.format("[DEBUG] %s", string.format(str, ...)), 0)
end

function LOG_INFO(str, ...)
    if 1 < g_log_level  then
        return
    end

    octopus.write_log(string.format("[INFO] %s", string.format(str, ...)), 1)
end

function LOG_WARN(str, ...)
    if 2 < g_log_level  then
        return
    end

    octopus.write_log(string.format("[WARN] %s", string.format(str, ...)), 2)
end

function LOG_ERROR(str, ...)
    if 3 < g_log_level  then
        return
    end

    octopus.write_log(string.format("[ERROR] %s", string.format(str, ...)), 3)
end

function LOG_FATAL(str, ...)
    if 4 < g_log_level  then
        return
    end

	octopus.write_log(string.format("[FATAL] %s", string.format(str, ...)), 4)
end

function FormatNum(num)
	if num <= 0 then
		return 0
	else
		local t1, t2 = math.modf(num)
		---小数如果为0，则去掉
		if t2 > 0 then
			return num
		else
			return t1
		end
	end
end

function TableToString(tb, PrintMetatable)
    if type(tb) ~= "table" then
		return ("Sorry, it's not table, it is " .. type(tb) .. ":" .. tostring(tb))
    end

    local function _list_table(tb, table_list, level)
        local ret = ""
        local indent = string.rep(" ", level*4)

        for k, v in pairs(tb) do
            local quo = type(k) == "string" and "\"" or ""
            ret = ret .. indent .. "[" .. quo .. tostring(k) .. quo .. "] = "

            if type(v) == "table" then
                local t_name = table_list[v]
                if t_name then
                    ret = ret .. tostring(v) .. " -- > [\"" .. t_name .. "\"]\n"
                else
                    table_list[v] = tostring(k)
                    ret = ret .. "{\n"
                    ret = ret .. _list_table(v, table_list, level+1)
                    ret = ret .. indent .. "}\n"
                end
            elseif type(v) == "string" then
                ret = ret .. "\"" .. tostring(v) .. "\"\n"
            else
                ret = ret .. tostring(v) .. "\n"
            end
        end

		if PrintMetatable then
			local mt = getmetatable(tb)
			if mt then
				ret = ret .. "\n"
				local t_name = table_list[mt]
				ret = ret .. indent .. "<metatable> = "

				if t_name then
					ret = ret .. tostring(mt) .. " -- > [\"" .. t_name .. "\"]\n"
				else
					ret = ret .. "{\n"
					ret = ret .. _list_table(mt, table_list, level+1)
					ret = ret .. indent .. "}\n"
				end

			end
		end

        return ret
    end

    local ret = " {\n"
    local table_list = {}
    table_list[tb] = "root table"
    ret = ret .. _list_table(tb, table_list, 1)
    ret = ret .. "}"
    return ret
end

function dump(obj)
	LOG_INFO(TableToString(obj, false))
end

function PrintTraceInfo()
    local str = debug.traceback("", 2)
    LOG_ERROR(str)
    
    local startLevel = 2 --0表示getinfo本身, 1表示调用getinfo的函数(PrintTraceInfo), 2表示调用PrintLuaStack的函数,可以想象一个getinfo(0级)在顶的栈.
    local maxLevel = 10  --最大递归10层
    for level = startLevel, maxLevel do

        -- 打印堆栈每一层
        local info = debug.getinfo(level, "nSl")
        if info == nil then break end
        
        LOG_ERROR("--------------------------------------------------------------------")
        LOG_ERROR("[ line : %-4d] %-20s :: %s", info.currentline, info.name or "", info.source or "")

        -- 打印该层的参数与局部变量
        local index = 1 --1表示第一个参数或局部变量, 依次类推

        while true do

            local name, value = debug.getlocal( level, index )

            if name == nil or name == "(*temporary)" or name == "self" then break end
         
            if value ~= nil then
                LOG_ERROR("\t%s = %s", name, TableToString(value, false) )
            end

            index = index + 1

        end
    end
    
    LOG_INFO("-----------------------------------------------------------------------------------------------\n")
end

function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local newObject = {}
        lookup_table[object] = newObject
        for key, value in pairs(object) do
            newObject[_copy(key)] = _copy(value)
        end
        return setmetatable(newObject, getmetatable(object))
    end
    return _copy(object)
end

function table_len(t)
    local len=0
    for k, v in pairs(t) do
        len=len+1
    end
    return len
end

function table_empty(t)
    for k, v in pairs(t) do
        return false
    end
    return true
end