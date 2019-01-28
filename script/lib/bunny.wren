import "random" for Random
import "graphics" for Canvas

var RANDOM = Random.new()
var MAX_SPEED = 500
var GRAVITY = 981
var DAMPENING = 0.9
var MIN_X = 0
var MAX_X = Canvas.width
var MIN_Y = 0
var MAX_Y = Canvas.height

class Bunny {

    construct new(random) {
        _x = (MAX_X - MIN_X) / 2 // Spawn in the top-center part of the screen.
        _y = (MAX_Y - MIN_Y) / 8
        _vx = (RANDOM.float() * MAX_SPEED) - (MAX_SPEED / 2.0)
        _vy = (RANDOM.float() * MAX_SPEED) - (MAX_SPEED / 2.0)

        _random = random
    }

    update(deltaTime) {
        _x = _x + _vx * deltaTime
        _y = _y + _vy * deltaTime

        _vy = _vy + GRAVITY * deltaTime

        if (_x > MAX_X) {
            _vx = _vx * DAMPENING * -1.0
            _x = MAX_X
        } else if (_x < MIN_X) {
            _vx = _vx * DAMPENING * -1.0
            _x = MIN_X
        }

        if (_y > MAX_Y) {
            _vy = _vy * DAMPENING * -1.0
            _y = MAX_Y

            if (_vy.abs <= 500.0 && _random.float() <= 0.10) { // Higher bounce occasionally.
                _vy = _vy - ((_random.float() * 250.0) + 250.0)
            }
        } else if (_y < MIN_Y) {
            _vy = _vy * DAMPENING * -1.0
            _y = MIN_Y
        }
    }

    render() {
        var angle = (((_vx.abs > MAX_SPEED) ? MAX_SPEED : _vx.abs) / MAX_SPEED) * 45.0
        var rotation = _vx.sign * angle
        Canvas.sprite(0, 0, _x, _y, rotation)
        // Canvas.sprite(0, 0, _x, _y)
    }

}