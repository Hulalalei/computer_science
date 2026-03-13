--- math library module ---

local constant = 3.14


function add(lf, rt) 
    return lf + rt
end

--- local变量不对外抛出接口 ---
--- 除非显式导出 ---
local function mul(lf, rt) 
    return lf * rt
end

--- method 1 ---
-- return {
--     constant = constant,
--     mul = mul
-- }

--- method 2 ---
local mod = {}
mod.constant = constant
mod.add = add
mod.mul = mul
return mod
