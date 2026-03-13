--- 元表 ---
--- 本质上是对原表的一个功能扩展，通过setmetatable为table增加额外处理 ---

--- 算术运算符重载 ---
local meta = {
    __add = function(lf, rt)
        return { x = lf.a + rt.a, y = lf.b + rt.b }
    end,

    __mul = function(lf, rt)
        return { x = lf.a * rt.a, y = lf.b * rt.b }
    end
}


local v1 = {
    a = 1,
    b = 2
}

local v2 = {
    a = 3,
    b = 4
}

setmetatable(v1, meta)
setmetatable(v2, meta)

local t = v1 + v2
for k, v in pairs(t) do
    print(k, v)
end

--- __index ---

local v1 = {
    name = "hsf",
    age = 21
}

local v1_meta = {
    name = "hulalalei",
    age = 23,
    addr = "jiangsu"
}

setmetatable(v1, {__index = v1_meta})
print(v1.addr)

--- __call ---
local v1 = {
    name = "hsf"
}

setmetatable(v1, {__call = function()
        print("call the default function")
    end
})

v1()

--- __tostring ---
local v1 = {
    name = "hsf"
}

setmetatable(v1, {__tostring = function()
        return "overload the print function"
    end
})

print(v1)
