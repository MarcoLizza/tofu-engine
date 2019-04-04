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

#ifndef __MODULES_COLLECTIONS_H__
#define __MODULES_COLLECTIONS_H__

#include <wren/wren.h>

extern const char collections_wren[];

extern void collections_grid_allocate(WrenVM* vm);
extern void collections_grid_finalize(void* data);

extern void collections_grid_width(WrenVM *vm);
extern void collections_grid_height(WrenVM *vm);
extern void collections_grid_fill(WrenVM *vm);
extern void collections_grid_row(WrenVM *vm);
extern void collections_grid_peek(WrenVM *vm);
extern void collections_grid_poke(WrenVM *vm);

#endif  /* __MODULES_COLLECTIONS_H__ */