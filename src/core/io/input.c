/*
 * Copyright (c) 2019 Marco Lizza (marco.lizza@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "input.h"

#include <libs/log.h>

#define LOG_CONTEXT "input"

bool Input_initialize(Input_t *input, const Input_Configuration_t *configuration, GLFWwindow *window)
{
    // TODO: should perform a single "zeroing" call and the set the single fields?

    *input = (Input_t){
            .configuration = *configuration,
            .window = window,
            .time = 0
        };
    return true;
}

void Input_terminate(Input_t *input)
{
}

void Input_update(Input_t *input, float delta_time)
{
    input->time += delta_time;

    Input_Keyboard_t *keyboard = &input->keyboard;
    for (int i = Input_Keys_t_First; i <= Input_Keys_t_Last; ++i) {
        Input_Key_t *key = &keyboard->keys[i];

        if (!key->state.triggered) {
            continue;
        }

        key->state.pressed = false; // Clear the flags, will be eventually updated.
        key->state.released = false;

        key->time += delta_time;

        while (key->time >= key->period) {
            Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "#%d %.3fs", i, key->time);
            key->time -= key->period;

            key->state.down = !key->state.down;
            key->state.pressed = key->state.down;
            key->state.released = !key->state.down;
            Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "#%d %.3fs %d %d %d", i, key->time, key->state.down, key->state.pressed, key->state.released);
        }
    }

}

void Input_process(Input_t *input)
{
    static const int keys[Input_Keys_t_CountOf] = {
        GLFW_KEY_UP,
        GLFW_KEY_DOWN,
        GLFW_KEY_LEFT,
        GLFW_KEY_RIGHT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_Z,
        GLFW_KEY_S,
        GLFW_KEY_X,
        GLFW_KEY_D,
        GLFW_KEY_ENTER,
        GLFW_KEY_SPACE,
        GLFW_KEY_ESCAPE
    };
    static const int gamepad[Input_Keys_t_CountOf] = {
        GLFW_GAMEPAD_BUTTON_DPAD_UP,
        GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        GLFW_GAMEPAD_BUTTON_Y,
        GLFW_GAMEPAD_BUTTON_X,
        GLFW_GAMEPAD_BUTTON_B,
        GLFW_GAMEPAD_BUTTON_A,
        GLFW_GAMEPAD_BUTTON_BACK,
        GLFW_GAMEPAD_BUTTON_START,
        GLFW_GAMEPAD_BUTTON_GUIDE
    };
    static const int buttons[Input_Buttons_t_CountOf] = {
        GLFW_MOUSE_BUTTON_LEFT,
        GLFW_MOUSE_BUTTON_MIDDLE,
        GLFW_MOUSE_BUTTON_RIGHT
    };

    glfwPollEvents();

    Input_Keyboard_t *keyboard = &input->keyboard;
    for (int i = Input_Keys_t_First; i <= Input_Keys_t_Last; ++i) {
        Input_Key_t *key = &keyboard->keys[i];

        bool was_down = key->state.down;
        bool is_down = glfwGetKey(input->window, keys[i]) == GLFW_PRESS;

        if (!key->state.triggered) { // If not triggered use the current physical status.
            key->state.down = is_down;
            key->state.pressed = !was_down && is_down;
            key->state.released = was_down && !is_down;

            if (key->state.pressed && key->period > 0.0f) { // On press, track the trigger state and reset counter.
                key->state.triggered = true;
                key->time = 0.0f;
                Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "key #%d triggered, %.3fs %d %d %d", i, key->time, key->state.down, key->state.pressed, key->state.released);
            }
        } else
        if (!is_down) {
            key->state.down = false;
            key->state.pressed = false;
            key->state.released = was_down; // Track release is was previously down.

            key->state.triggered = false;
            Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "button #%d held for %.3fs %d %d %d", i, key->time, key->state.down, key->state.pressed, key->state.released);
        }
    }

    int result = glfwJoystickPresent(0);
    if (result == GLFW_TRUE) {
        GLFWgamepadstate state;
        int result = glfwGetGamepadState(0, &state);
        if (result == GLFW_TRUE) {
            for (int i = Input_Keys_t_First; i <= Input_Keys_t_Last; ++i) {
                Input_Key_t *key = &keyboard->keys[i];

                bool was_down = key->state.down;
                bool is_down = state.buttons[gamepad[i]] == GLFW_PRESS;

                if (!key->state.triggered) { // If not triggered use the current physical status.
                    key->state.down = is_down;
                    key->state.pressed = !was_down && is_down;
                    key->state.released = was_down && !is_down;

                    if (key->state.pressed && key->period > 0.0f) { // On press, track the trigger state and reset counter.
                        key->state.triggered = true;
                        key->time = 0.0f;
                        Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "key #%d triggered, %.3fs %d %d %d", i, key->time, key->state.down, key->state.pressed, key->state.released);
                    }
                } else
                if (!is_down) {
                    key->state.down = false;
                    key->state.pressed = false;
                    key->state.released = was_down; // Track release is was previously down.

                    key->state.triggered = false;
                    Log_write(LOG_LEVELS_TRACE, LOG_CONTEXT, "button #%d held for %.3fs %d %d %d", i, key->time, key->state.down, key->state.pressed, key->state.released);
                }
            }
        }
    }

    Input_Mouse_t *mouse = &input->mouse;
    for (int i = Input_Buttons_t_First; i <= Input_Buttons_t_Last; ++i) {
        Input_Button_State_t *button = &mouse->buttons[i];

        bool was_down = button->down;
        bool is_down = glfwGetMouseButton(input->window, buttons[i]) == GLFW_PRESS;

        button->down = is_down;
        button->pressed = !was_down && is_down;
        button->released = was_down && !is_down;
    }
    double x, y;
    glfwGetCursorPos(input->window, &x, &y);
    mouse->x = (float)x;
    mouse->y = (float)y;

    if (input->configuration.exit_key_enabled) {
        if (glfwGetKey(input->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(input->window, true);
        }
    }
}

void Input_auto_repeat(Input_t *input, Input_Keys_t id, float period)
{
    input->keyboard.keys[id] = (Input_Key_t){
            .period = period,
            .time = 0.0f
        };
    Log_write(LOG_LEVELS_DEBUG, LOG_CONTEXT, "auto-repeat set to %.3fs for button #%d", period, id);
}
