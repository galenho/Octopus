o = o or {}


-- ע��
--print("Hello World")

--[[
print("Hello World")
print("Hello World")
--]]



-- (1) ȫ�ֱ���
print("(1)--------------")

b=10
print(b)

--�������ɾ��һ��ȫ�ֱ�����ֻ��Ҫ��������ֵΪnil��
b = nil
print(b)

local b = 5         -- �ֲ�����

-- (2) ֵ������
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


-- (3) Table (��Key����˳���)
print("(3)--------------")

tab1 = { key1 = "val1", key2 = "val2", "val3" }
for k, v in pairs(tab1) do
    print(k .. " - " .. v)
end

print("--------")

tab1.key1 = nil --ɾ��һ��Ԫ��
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


-- (4) �ַ���
print("(4)--------------")

string1 = "this is string1"

-- �� 2 �������� "[[]]" ����ʾ"һ��"�ַ�����
html = [[
<html>
<head></head>
<body>
    <a href="http://www.runoob.com/">����̳�</a>
</body>
</html>
]]
print(html)

--ȡ�ַ�������
len = "www.runoob.com"
print(#len)  
print(#"www.runoob.com")

-- (5) ����
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
function(key,val)--��������
        return key.."="..val;
end
);



-- (6) ���̿���
print("(6)--------------")

--lua ��û�� continue ����е㲻ϰ�ߡ�����ʹ�������������ַ���ʵ�� continue ��䣺

for i = 10, 1, -1 do
	repeat
        if i == 5 then
            print("continue code here")
            break
        end
        print(i, "loop code here")
	until true --ֱ��true�˳�
end

-- prints even numbers in [|1,10|]
for i=1,10 do
    if i % 2 == 1 then goto continue end
    print(i)
    ::continue::
 end

print("--------")
if 5 ~= 6 then
	print("5������6")
else
	
end


-- (7) �����
print("(7)--------------")
a = true
b = true

if ( a and b )
then
   print("a and b - ����Ϊ true" )
end

if ( a or b )
then
   print("a or b - ����Ϊ true" )
end

print("--------")

-- �޸� a �� b ��ֵ
a = false
b = true

if ( a and b )
then
   print("a and b - ����Ϊ true" )
else
   print("a and b - ����Ϊ false" )
end

if ( not( a and b) )
then
   print("not( a and b) - ����Ϊ true" )
else
   print("not( a and b) - ����Ϊ false" )
end

print("--------")

a = "Hello "
b = "World"

print("�����ַ��� a �� b ", a..b )

print("b �ַ������� ",#b )

print("�ַ��� Test ���� ",#"Test" )

print("����̳���ַ���� ",#"www.runoob.com" )


-- (8) Ԫ��(Metatable)
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
    return "������Ԫ�صĺ�Ϊ " .. sum
  end
})
print(mytable)


-- (9)�̳�

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
  
  -- ����
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
  
  -- ��������
  -- Animal is running
  -- Woof! Woof!


-- (10)  ���ݽṹ
print("(9)--------------")

-- (11) Э�� coroutine
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