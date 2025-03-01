
----------------------------------------------------------------------------------
--                util辅助函数
----------------------------------------------------------------------------------
require "common"
local hotfix = require("hotfix")

local M = {}

--获取文件名
function M.getFileName(str)
    local idx = str:match(".+()%.%w+$")
    if(idx) then
        return str:sub(1, idx-1)
    else
        return str
    end
end

--获取扩展名
function M.getExtension(str)
    return str:match(".+%.(%w+)$")
end

--递归遍历目录
function M.GetDirFiles(rootPath, ext, t)
    for entry in lfs.dir(rootPath) do
        if entry~='.' and entry~='..' then
            local path = rootPath.."/"..entry
            local attr = lfs.attributes(path)
            assert(type(attr)=="table") --如果获取不到属性表则报错
            -- PrintTable(attr)
            if(attr.mode == "directory") then
                --print("Dir:",path)
				M.GetDirFiles(path, ext, t)
            elseif attr.mode=="file" then
                --print("File:",path)
				local postfix = M.getExtension(entry)
				if postfix == ext then 
					table.insert(t, path)
				end
            end
        end
    end
end

function M.GetReloadFileName(full_name)
	local filename = ""
	local len = string.len(full_name)
	local n1, n2 = string.find(full_name, "./")
	if n1 == 1 then --即./
		filename = string.sub(full_name, 3, len)
		filename = string.gsub(filename, "/", ".")
		filename = string.gsub(filename, "\\", ".")
		filename = M.getFileName(filename) --去掉扩展名
	elseif n1 == 2 then --即../
		filename = string.sub(full_name, 4, len)
		filename = string.gsub(filename, "/", ".")
		filename = string.gsub(filename, "\\", ".")
		filename = M.getFileName(filename) --去掉扩展名
	end
	return filename
end

--热更新所有代码
function M.HotUpdateAllCode()
	local t = {}
	M.GetDirFiles(".", "lua", t)
	for key, value in ipairs(t) do
		if value ~= "./main.lua" then
			local filename = M.GetReloadFileName(value)
			print("reload " .. filename)
			hotfix.hotfix_module(filename)
		end
	end
end

--热更新所有配置表
function M.HotUpdateAllTable()
	local t = {}
	M.GetDirFiles("../config", "lua", t)
	for key, value in ipairs(t) do
		local filename = M.GetReloadFileName(value)
		print("reload " .. filename)
		hotfix.hotfix_table(filename)
	end
end

--热更新指定代码
function M.HotUpdateCode()
	local file = io.open("hot_update_code.list", "r")
	io.input(file)
	
	while true do
		local line = io.read()
		if line == nil then
			break
		else
			print("reload " .. line)
			hotfix.hotfix_module(line)
		end
	end
	
	io.close(file)
end

--热更新指定配置表
function M.HotUpdateTable()
	local file = io.open("hot_update_table.list", "r")
	io.input(file)
	
	while true do
		local line = io.read()
		if line == nil then
			break
		else
			print("reload " .. line)
			hotfix.hotfix_table(line)
		end
	end
	
	io.close(file)
end


--字符串分隔方法
function M.Split(str, sep)
    local sep, fields = sep or ":", {}
    local pattern = string.format("([^%s]+)", sep)
    str:gsub(pattern, function (c) fields[#fields + 1] = c end)
    return fields
end

return M