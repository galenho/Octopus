local M = {}

M.count = 0

function M.func()
    M.count = M.count + 1
	print(M.count)
end

return M