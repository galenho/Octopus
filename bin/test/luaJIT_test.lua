function test1()
	local s = [[aaaaaabbbbbbbcccccccccccddddddddddddeeeeeeeeeeeeefffffffffffffffffggggggggggggggaaaaaaaaaaabbbbbbbbbbbbbbccccccccccclllll]]
	for i=1,10000 do
    		for j=1,10000 do
        		string.find(s, "ll", 1, true)
    		end
	end
end

function test2()
	local loop_count = 1000

	local t = {}
	for i=1,100 do
    		t[i] = i
	end
	for i=1,loop_count do
    		for j=1,1000 do
       			for k,v in ipairs(t) do

        		end
    		end
	end
end

function test3()
	local loop_count = 1000

	local t = {}
	for i=1,100 do
    		t[i] = i
	end
	for i=1,loop_count do
    		for j=1,1000 do
       			for k,v in pairs(t) do

        		end
    		end
	end
end

local start_time = os.clock()
--test1()
--test2()
test3()
local end_time = os.clock()
local use_time = end_time - start_time
print(use_time)
