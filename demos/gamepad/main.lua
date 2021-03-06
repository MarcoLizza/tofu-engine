local Bank = require("tofu.graphics").Bank
local Canvas = require("tofu.graphics").Canvas
local Font = require("tofu.graphics").Font
local Input = require("tofu.events").Input
local Class = require("tofu.util").Class
local System = require("tofu.core").System

local Main = Class.define()

local IDS = {
    Input.A, Input.B, Input.X, Input.Y,
    Input.LB, Input.RB,
    Input.UP, Input.DOWN, Input.LEFT, Input.RIGHT,
    Input.SELECT, Input.START
  }

local INDICES = {
    0, 1, 2, 3,
    4, 5,
    12, 13, 14, 15,
    24, 25
  }

function Main:__ctor()
  Canvas.palette("pico-8")

  Input.auto_repeat(Input.X, 0.25)
  Input.auto_repeat(Input.Y, 0.5)
  Input.cursor_area(0, 0, Canvas.width(), Canvas.height())

  self.bank = Bank.new("assets/sheet.png", 12, 12)
  self.font = Font.default(0, 15)
  self.down = {}
  self.scale = {}
end

function Main:input()
  for _, id in ipairs(IDS) do
    self.down[id] = Input.is_down(id)
    if Input.is_pressed(id) then
      self.scale[id] = 3.0
    end
  end
end

function Main:update(delta_time)
  for _, id in ipairs(IDS) do
    if self.scale[id] and self.scale[id] > 1.0 then
      self.scale[id] = math.max(1.0, self.scale[id] - delta_time * 12.0)
    end
  end
end

local function draw_stick(cx, cy, radius, _, _, angle, magnitude, pressed)
  local dx, dy = math.floor(math.cos(angle) * magnitude * radius + 0.5),
                 math.floor(math.sin(angle) * magnitude * radius + 0.5)
--  local dx, dy = x * radius, y * radius
  if pressed then
    Canvas.circle("fill", cx, cy, radius, 2)
  end
  Canvas.circle("line", cx, cy, radius, 1)
  Canvas.line(cx, cy, cx + dx, cy + dy, 3)
end

local function draw_trigger(cx, cy, radius, magnitude)
  if magnitude > 0.0 then
    Canvas.circle("fill", cx, cy, magnitude * radius, 2)
  end
  Canvas.circle("line", cx, cy, radius, 1)
end

function Main:render(_)
  local t = System.time()

  Canvas.clear()

  local cw, ch = self.bank:cell_width(), self.bank:cell_height()

  local x, y = (Canvas.width() - #IDS * cw) * 0.5, (Canvas.height() - ch) * 0.5
  for index, id in ipairs(IDS) do
    local dy = math.sin(t * 2.5 + x * 0.5) * ch
    if self.down[id] then
      dy = 0
    end
    local ox, oy = 0, 0
    local s = 1.0
    if self.scale[id] then
      s = self.scale[id]
      ox = (cw * s - cw) * 0.5
      oy = (ch * s - ch) * 0.5
    end
    self.bank:blit(INDICES[index], x - ox, y - oy + dy, s, s)
    x = x + cw
  end

  local h = Canvas.height() * 0.5
  local lx, ly, la, lm = Input.stick("left") -- TODO: use constants.
  local rx, ry, ra, rm = Input.stick("right")
  draw_stick(24, h - 12, 8, lx, ly, la, lm, Input.is_down(Input.LT))
  draw_stick(232, h - 12, 8, rx, ry, ra, rm, Input.is_down(Input.RT))
  local tl, tr = Input.triggers()
  draw_trigger(24, h + 12, 8, tl)
  draw_trigger(232, h + 12, 8, tr)

  local cx, cy = Input.cursor()
  Canvas.line(cx - 3, cy, cx - 1, cy, 2)
  Canvas.line(cx + 1, cy, cx + 3, cy, 2)
  Canvas.line(cx, cy - 3, cx, cy - 1, 2)
  Canvas.line(cx, cy + 1, cx, cy + 3, 2)

  self.font:write(string.format("FPS: %d", System.fps()), 0, 0, "left")
  self.font:write(string.format("X:%.2f Y:%.2f A:%.2f M:%.2f", lx, ly, la, lm), Canvas.width(), 0, "right")
end

return Main
