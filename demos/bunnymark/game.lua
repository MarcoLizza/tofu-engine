local System = require("tofu.core.System")
local Bank = require("tofu.graphics.Bank")
local Canvas = require("tofu.graphics.Canvas")
local Font = require("tofu.graphics.Font")
local Input = require("tofu.events.Input")
local Class = require("tofu.util.Class")

local LITTER_SIZE = 250
local MAX_BUNNIES = 32768

local Game = Class.define()

local function dump(t, spaces)
  spaces = spaces or ""
  for k, v in pairs(t) do
    print(spaces .. k .. " " .. type(v) .. " " .. tostring(v))
    if type(v) == "table" then
      if (k ~= "__index") then
        dump(v, spaces .. " ")
      end
    end
  end
end

function Game:__ctor()
--dump(Canvas)
  Canvas.palette("gameboy")
  Canvas.background(1)

  self.bunnies = {}
  self.bank = Bank.new("assets/sheet.png", 26, 37)
  self.font = Font.default()
  self.speed = 1.0
  self.running = true
end

function Game:input()
  if Input.is_key_pressed(Input.START) then
    local Bunny = require("lib.bunny") -- Lazily require the module only in this scope.
    for i = 1, LITTER_SIZE do
      table.insert(self.bunnies, Bunny.new(self.bank))
    end
    if #self.bunnies >= MAX_BUNNIES then
      System.quit()
    end
  elseif Input.is_key_pressed(Input.LEFT) then
    self.speed = self.speed * 0.5
  elseif Input.is_key_pressed(Input.RIGHT) then
    self.speed = self.speed * 2.0
  elseif Input.is_key_pressed(Input.DOWN) then
    self.speed = 1.0
  elseif Input.is_key_pressed(Input.SELECT) then
    self.bunnies = {}
  elseif Input.is_key_pressed(Input.Y) then
    self.running = not self.running
  end
end

function Game:update(delta_time)
  if not self.running then
    return
  end
  for _, bunny in pairs(self.bunnies) do
    bunny:update(delta_time * self.speed)
  end
end

function Game:render(ratio)
  for _, bunny in pairs(self.bunnies) do
    bunny:render()
  end
  self.font:write(string.format("FPS: %d", System.fps()), 0, 0, 0, 1.0, "left")
  self.font:write(string.format("#%d bunnies", #self.bunnies), Canvas.width(), 0, 3, 1.0, "right")
end

return Game