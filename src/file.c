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

#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

#define UNUSED(x)   (void)(x)

void file_resolve_path(char *resolved, const char *path)
{
    char *ptr = realpath(path, resolved);
    UNUSED(ptr);
    size_t length = strlen(resolved);
    if (resolved[length - 1] != '/') {
        strcat(resolved, FILE_PATH_SEPARATOR_SZ);
    }
}

char *file_load_as_string(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, mode);
    if (!file) {
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    size_t length = ftell(file);
    char *data = Memory_calloc(length + 1, sizeof(char)); // Add null terminator for the string.
    rewind(file);
    size_t read_bytes = fread(data, sizeof(char), length, file);
    fclose(file);
    if (read_bytes < length) {
        Memory_free(data);
        return NULL;
    }
    data[length] = '\0';
    return data;
}