--[[
    说明：覆盖蓝图事件时，只需要在返回的table中声明 Receive{EventName}

    例如：
    function M:ReceiveBeginPlay()
    end

    除了蓝图事件可以覆盖，也可以直接声明 {FunctionName} 来覆盖Function。
    如果需要调用被覆盖的蓝图Function，可以通过 self.Overridden.{FunctionName}(self, ...) 来访问

    例如：
    function M:SayHi(name)
        self.Overridden.SayHi(self, name)
    end

    注意：这里不可以写成 self.Overridden:SayHi(name)
]] --

--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_SeedLuaBattleComponent_C
local Screen = require "Screen"

local M = UnLua.Class()

-- function M:Initialize(Initializer)
-- end

-- 注意，重写了BeginPlay时， 蓝图的BeginPlay不会被调用
-- 以下代码执行顺序：
-- 当C++ 中调用了Super::BeginPlay()时, 会执行这部分函数
-- 1. 蓝图中的BeginPlay
-- 2. Lua中的BeginPlay
function M:ReceiveBeginPlay()
    -- 调用了蓝图中的BeginPlay
    self.Overridden.ReceiveBeginPlay(self)
    -- Lua部分的BeginPlay
    local msg = self:SayHi("陌生人")
    Screen.Print(msg)
    -- 测试注册Delegate 通过
    self.OnTest:Add(self, M.OnTestImpl)
end

function M:OnTestImpl(InString)
    Screen.Print(InString)
end

--  重写UFUNCTION(BlueprintImplementableEvent) SayHi
function M:SayHi(name)
    local origin = self.Overridden.SayHi(self, name)
    return origin .. "\n\n" ..
            [[现在我们已经相互熟悉了，这是来自Lua的问候。
    
            —— 本示例来自 "SeedLuaBattle.lua"
        ]]
end

function M:ReceiveEndPlay()
    self.OnTest:Remove(self, M.OnTestImpl)
end

-- function M:ReceiveTick(DeltaSeconds)
-- end

return M
