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

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <stdbool.h>

#define MAX_CONFIGURATION_TITLE_LENGTH      128

typedef struct _Configuration {
    char title[MAX_CONFIGURATION_TITLE_LENGTH];
    int width, height, depth;
    bool fullscreen;  // TODO: rename to "windowed"?
    bool autofit;
    int fps;
    int skippable_frames;
    bool hide_cursor;
    bool exit_key_enabled;
    bool debug;
    // TODO: key-remapping?
} Configuration_t;

extern void Configuration_initialize(Configuration_t *configuration);
extern void Configuration_load(Configuration_t *configuration, const char *pathfile);

#endif  /* __CONFIGURATION_H__ */