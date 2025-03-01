o = o or {}


-- 注释
--print("Hello World")

--[[
print("Hello World")
print("Hello World")
--]]



-- (1) 全局变量
print("(1)--------------")

b=10
print(b)

--如果你想删除一个全局变量，只需要将变量赋值为nil。
b = nil
print(b)

local b = 5         -- 局部变量

-- (2) 值的类型
print("(2)--------------")

print(type("Hello world"))      --> string
print(type(10.4*3))             --> number
print(type(print))              --> function
print(type(type))               --> function
print(type(true))               --> boolean
print(type(nil))                --> nil
print(type(type(X)))            --> string
print(type(a))					--> nil

print("a" .. 'b')

print(157 .. 428)


-- (3) Table (表，Key不是顺序的)
print("(3)--------------")

tab1 = { key1 = "val1", key2 = "val2", "val3" }
for k, v in pairs(tab1) do
    print(k .. " - " .. v)
end

print("--------")

tab1.key1 = nil --删除一个元素
for k, v in pairs(tab1) do
    print(k .. " - " .. v)
end

print("--------")

a = {}
a["key"] = "value"
key = 10
a[key] = 22
a[key] = a[key] + 11
for k, v in pairs(a) do
    print(k .. " : " .. v)
end


-- (4) 字符串
print("(4)--------------")

string1 = "this is string1"

-- 用 2 个方括号 "[[]]" 来表示"一块"字符串。
html = [[
<html>
<head></head>
<body>
    <a href="http://www.runoob.com/">菜鸟教程</a>
</body>
</html>
]]
print(html)

--取字符串长度
len = "www.runoob.com"
print(#len)  
print(#"www.runoob.com")

-- (5) 函数
print("(5)--------------")

function factorial(n)
    if n == 0 then
        return 1
    else
        return n * factorial(n - 1)
    end
end
print(factorial(5))


function testFun(tab,fun)
        for k ,v in pairs(tab) do
                print(fun(k,v));
        end
end

tab={key1="val1",key2="val2"};

testFun(tab,
function(key,val)--匿名函数
        return key.."="..val;
end
);



-- (6) 流程控制
print("(6)--------------")

--lua 中没有 continue 语句有点不习惯。可以使用类似下面这种方法实现 continue 语句：

for i = 10, 1, -1 do
	repeat
        if i == 5 then
            print("continue code here")
            break
        end
        print(i, "loop code here")
	until true --直到true退出
end

-- prints even numbers in [|1,10|]
for i=1,10 do
    if i % 2 == 1 then goto continue end
    print(i)
    ::continue::
 end

print("--------")
if 5 ~= 6 then
	print("5不等于6")
else
	
end


-- (7) 运算符
print("(7)--------------")
a = true
b = true

if ( a and b )
then
   print("a and b - 条件为 true" )
end

if ( a or b )
then
   print("a or b - 条件为 true" )
end

print("--------")

-- 修改 a 和 b 的值
a = false
b = true

if ( a and b )
then
   print("a and b - 条件为 true" )
else
   print("a and b - 条件为 false" )
end

if ( not( a and b) )
then
   print("not( a and b) - 条件为 true" )
else
   print("not( a and b) - 条件为 false" )
end

print("--------")

a = "Hello "
b = "World"

print("连接字符串 a 和 b ", a..b )

print("b 字符串长度 ",#b )

print("字符串 Test 长度 ",#"Test" )

print("菜鸟教程网址长度 ",#"www.runoob.com" )


-- (8) 元表(Metatable)
print("(8)--------------")
other = { foo = 3 }
t = setmetatable({}, { __index = other })
print(t.foo)



mytable = setmetatable({key1 = "value1"}, {
  __index = function(mytable, key)
    if key == "key2" then
      return "metatablevalue"
    else
      return nil
    end
  end
})

print(mytable.key1,mytable.key2)


mytable = setmetatable({ 10, 20, 30 }, {
  __tostring = function(mytable)
    sum = 0
    for k, v in pairs(mytable) do
                sum = sum + v
        end
    return "表所有元素的和为 " .. sum
  end
})
print(mytable)


-- (9)继承

local Animal = {
    legs = 4,
  }
  
  function Animal:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
  end
  
  function Animal:run()
    print("Animal is running")
  end
  
  -- 子类
  local Dog = Animal:new()
  
  function Dog:bark()
    print("Woof! Woof!")
  end

  function Dog:run()
    print("Dog is running")
  end

  local myDog = Dog:new()
  

  myDog:run()
  

  myDog:bark()
  
  -- 输出结果：
  -- Animal is running
  -- Woof! Woof!


-- (10)  数据结构
print("(9)--------------")

-- (11) 协程 coroutine
print("(10)--------------")

co = coroutine.create(
    function(i)
        print(i);
    end
)
 
coroutine.resume(co, 1)   -- 1
print(coroutine.status(co))  -- dead
 
print("----------")
 
co = coroutine.wrap(
    function(i)
        print(i);
    end
)
 
co(1)
 
print("----------")
 
co2 = coroutine.create(
    function()
        for i=1,10 do
            print(i)
            if i == 3 then
                print(coroutine.status(co2))  --running
                print(coroutine.running()) --thread:XXXXXX
            end
            coroutine.yield()
        end
    end
)
 
coroutine.resume(co2) --1
coroutine.resume(co2) --2
coroutine.resume(co2) --3
 
print(coroutine.status(co2))   -- suspended
print(coroutine.running())
 
print("----------")