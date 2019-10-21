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

#include "engine.h"

#include "config.h"
#include "configuration.h"
#include "log.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef __CAP_FPS__
  #include "platform.h"
  #if PLATFORM_ID == PLATFORM_LINUX
    #include <unistd.h>
  #elif PLATFORM_ID == PLATFORM_WINDOWS
    #include <windows.h>
  #endif
#endif
#ifdef DEBUG
  #define STB_LEAKCHECK_IMPLEMENTATION
  #include <stb/stb_leakcheck.h>
#endif

static bool update_statistics(Engine_Statistics_t *statistics, float elapsed) {
    static float history[FPS_AVERAGE_SAMPLES] = {};
    static int index = 0;
    static int samples = 0;
    static float sum = 0.0f;
    static int count = 0;

    sum -= history[index];
    sum += elapsed;
    history[index] = elapsed;
    index = (index + 1) % FPS_AVERAGE_SAMPLES;
    if (samples < FPS_AVERAGE_SAMPLES) {
        samples += 1;
        return false;
    }
    float fps = roundf((float)FPS_AVERAGE_SAMPLES / sum);
    statistics->fps = fps;
    if (count == 0) {
        statistics->history[statistics->index] = fps;
        statistics->index = (statistics->index + 1) % STATISTICS_LENGTH;
    }
    count = (count + 1) % FPS_STATISTICS_RESOLUTION;
    return true;
}

bool Engine_initialize(Engine_t *engine, const char *base_path)
{
    Log_initialize();
    Environment_initialize(&engine->environment, base_path);
    Configuration_initialize(&engine->configuration);

    bool result = Interpreter_initialize(&engine->interpreter, &engine->configuration, &engine->environment, &engine->display);
    if (!result) {
        Log_write(LOG_LEVELS_FATAL, "<ENGINE> can't initialize interpreter");
        return false;
    }

    Log_configure(engine->configuration.debug);

    Display_Configuration_t display_configuration = {
            .width = engine->configuration.width,
            .height = engine->configuration.height,
            .fullscreen = engine->configuration.fullscreen,
            .scale = engine->configuration.scale,
            .hide_cursor = engine->configuration.hide_cursor,
            .exit_key_enabled = engine->configuration.exit_key_enabled,
        };
    result = Display_initialize(&engine->display, &display_configuration, engine->configuration.title);
    if (!result) {
        Log_write(LOG_LEVELS_FATAL, "<ENGINE> can't initialize display");
        Interpreter_terminate(&engine->interpreter);
        return false;
    }

    engine->environment.timer_pool = &engine->interpreter.timer_pool; // HACK: inject the timer-pool pointer.

    result = Interpreter_init(&engine->interpreter);
    if (!result) {
        Log_write(LOG_LEVELS_FATAL, "<ENGINE> can't call init method");
        Interpreter_terminate(&engine->interpreter);
        Display_terminate(&engine->display);
        return false;
    }

    return true;
}

void Engine_terminate(Engine_t *engine)
{
    Interpreter_terminate(&engine->interpreter); // Terminate the interpreter to unlock all resources.
    Display_terminate(&engine->display);
    Environment_terminate(&engine->environment);
#if DEBUG
    stb_leakcheck_dumpmem();
#endif
}

void Engine_run(Engine_t *engine)
{
    const float delta_time = 1.0f / (float)engine->configuration.fps;
    const int skippable_frames = engine->configuration.skippable_frames;
    Log_write(LOG_LEVELS_INFO, "<ENGINE> now running, delta-time is %.3fs w/ %d skippable frames", delta_time, skippable_frames);

    Engine_Statistics_t statistics = (Engine_Statistics_t){
            .delta_time = delta_time,
        };

    float previous = (float)glfwGetTime();
    float lag = 0.0f;

    for (bool running = true; running && !engine->environment.quit && !Display_should_close(&engine->display); ) {
        float current = (float)glfwGetTime();
        float elapsed = current - previous;
        previous = current;

        if (engine->configuration.debug) {
            bool ready = update_statistics(&statistics, elapsed);
            engine->environment.fps = ready ? statistics.fps : 0.0f;
#ifdef __DEBUG_ENGINE_FPS__
            static size_t count = 0;
            if (++count == 250) {
                Log_write(LOG_LEVELS_INFO, "<ENGINE> currently running at %.0f FPS", engine->environment.fps);
                count = 0;
            }
#endif
        }

        Display_process_input(&engine->display);

        running = running && Interpreter_input(&engine->interpreter); // Lazy evaluate `running`, will avoid calls when error.

        lag += elapsed; // Count a maximum amount of skippable frames in order no to stall on slower machines.
        for (int frames = 0; (frames < skippable_frames) && (lag >= delta_time); ++frames) {
            engine->environment.time += delta_time;
            running = running && Interpreter_update(&engine->interpreter, delta_time);
            lag -= delta_time;
        }

        running = running && Interpreter_render(&engine->interpreter, lag / delta_time);

        Display_present(&engine->display);

#ifdef __CAP_FPS__
        float used = (float)glfwGetTime() - current;
        if (used < delta_time) {
            int millis = (int)((delta_time - used) * 1000.0f);
#if PLATFORM_ID == PLATFORM_LINUX
            usleep(millis * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#elif PLATFORM_ID == PLATFORM_LINUX
            Sleep(millis);
#else
            struct timespec ts;
            ts.tv_sec = millis / 1000;
            ts.tv_nsec = (millis % 1000) * 1000000;
            nanosleep(&ts, NULL);
#endif
        }
#endif
    }
}
