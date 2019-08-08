local Bunny = {}

Bunny.__index = Bunny

local MAX_SPEED = 500
local GRAVITY = 981
local DAMPENING = 0.9
local MIN_X = 0
local MAX_X = tofu.graphics.Canvas.width()
local MIN_Y = 0
local MAX_Y = tofu.graphics.Canvas.height()

function Bunny.new(bank)
  return setmetatable({
      x = (MAX_X - MIN_X) / 2, -- Spawn in the top-center part of the screen.
      y = (MAX_Y - MIN_Y) / 8,
      vx = (math.random() * MAX_SPEED) - (MAX_SPEED / 2.0),
      vy = (math.random() * MAX_SPEED) - (MAX_SPEED / 2.0),
      bank = bank
    }, Bunny)
end

function Bunny:update(delta_time)
  self.x = self.x + self.vx * delta_time
  self.y = self.y + self.vy * delta_time

  self.vy = self.vy + GRAVITY * delta_time

  if self.x > MAX_X then
    self.vx = self.vx * DAMPENING * -1.0
    self.x = MAX_X
  elseif self.x < MIN_X then
    self.vx = self.vx * DAMPENING * -1.0
    self.x = MIN_X
  end

  if self.y > MAX_Y then
    self.vy = self.vy * DAMPENING * -1.0
    self.y = MAX_Y

    if math.abs(self.vy) <= 400.0 and math.random() <= 0.10 then  -- Higher bounce occasionally.
      self.vy = self.vy - ((math.random() * 150.0) + 100.0)
    end
  elseif self.y < MIN_Y then
    self.vy = self.vy * DAMPENING * -1.0
    self.y = MIN_Y
  end
end

function Bunny:render()
  --var angle = (((_vx.abs > MAX_SPEED) ? MAX_SPEED : _vx.abs) / MAX_SPEED) * 60.0
  --var rotation = _vx.sign * angle
  --_bank.blit(0, _x, _y, rotation)
  --_bank.blit(0, _x, _y, -1.0, -1.0)
  --_bank.blit(0, _x, _y, _vx.sign, _vy.sign)
  self.bank:blit(0, self.x, self.y)
end

return Bunny