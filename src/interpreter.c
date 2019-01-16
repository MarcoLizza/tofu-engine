#include "interpreter.h"

#include "log.h"
#include "utils.h"

#include <raylib/raylib.h>

#include <limits.h>
#include <string.h>

static void* reallocate_function(void *ptr, size_t size)
{
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

static const char *resolve_module_function(WrenVM *vm, const char *importer, const char *name)
{
    char *resolved = reallocate_function(NULL, PATH_MAX);
    if (strcmp(importer, "main") == 0) { // For the main module, use the base-path.
        Interpreter_Config_t *configuration = (Interpreter_Config_t *)wrenGetUserData(vm);
        strcpy(resolved, configuration->base_path);
    } else { // Otherwise, desume the path as relative to the importer.
        char *separator = strrchr(importer, '/');
        size_t length = separator - importer + 1; // Include the path-separator itself!
        strncpy(resolved, importer, length);
    }
    strcat(resolved, name);
    strcat(resolved, ".wren");

    Log_write(LOG_LEVELS_DEBUG, "loading module '%s' for importer '%s'", name, importer);
    return resolved;
}

static void write_function(WrenVM *vm, const char *text)
{
    Log_write(LOG_LEVELS_OTHER, text);
}

static char *load_module_function(WrenVM *vm, const char *name)
{
    Log_write(LOG_LEVELS_DEBUG, "loading module '%s'", name);
    return load_file_as_string(name, "rt");
}

bool Interpreter_initialize(Interpreter_t *interpreter, Interpreter_Config_t *configuration)
{
    memcpy(&interpreter->configuration, configuration, sizeof(Interpreter_Config_t));

    char module_filename[PATH_MAX];
    strcpy(module_filename, configuration->base_path);
    strcat(module_filename, "tofu.wren");

    WrenConfiguration config; 
    wrenInitConfiguration(&config);
    config.reallocateFn = reallocate_function;
    config.writeFn = write_function;
    config.resolveModuleFn = resolve_module_function;
    config.loadModuleFn = load_module_function;

    interpreter->vm = wrenNewVM(&config);
    if (!interpreter->vm) {
        Log_write(LOG_LEVELS_ERROR, "Can't initialize Wren's VM!");
        return false;
    }

    wrenSetUserData(interpreter->vm, &interpreter->configuration);

    char *source = load_file_as_string(module_filename, "rt");
    if (!source) {
        Log_write(LOG_LEVELS_ERROR, "Can't read main module '%s'!", module_filename);
        wrenFreeVM(interpreter->vm);
        return false;
    }

    WrenInterpretResult result = wrenInterpret(interpreter->vm, "main", source);
    if (result != WREN_RESULT_SUCCESS) {
        Log_write(LOG_LEVELS_ERROR, "Can't interpret main module!");
        wrenFreeVM(interpreter->vm);
        return false;
    }

    wrenEnsureSlots(interpreter->vm, 1); 
    wrenGetVariable(interpreter->vm, "main", "Tofu", 0); 
    interpreter->handles[RECEIVER] = wrenGetSlotHandle(interpreter->vm, 0);

    interpreter->handles[INITIALIZE] = wrenMakeCallHandle(interpreter->vm, "initialize()");
    interpreter->handles[HANDLE] = wrenMakeCallHandle(interpreter->vm, "handle(_)");
    interpreter->handles[UPDATE] = wrenMakeCallHandle(interpreter->vm, "update(_)");
    interpreter->handles[RENDER] = wrenMakeCallHandle(interpreter->vm, "render(_)");
    interpreter->handles[TERMINATE] = wrenMakeCallHandle(interpreter->vm, "terminate()");

    wrenEnsureSlots(interpreter->vm, 1); 
    wrenSetSlotHandle(interpreter->vm, 0, interpreter->handles[RECEIVER]);
    wrenCall(interpreter->vm, interpreter->handles[INITIALIZE]);

    return true;
}

void Interpreter_handle(Interpreter_t *interpreter)
{
    wrenEnsureSlots(interpreter->vm, 1); 
    wrenSetSlotHandle(interpreter->vm, 0, interpreter->handles[RECEIVER]);
    wrenCall(interpreter->vm, interpreter->handles[HANDLE]);
}

void Interpreter_update(Interpreter_t *interpreter, const double delta_time)
{
    wrenEnsureSlots(interpreter->vm, 2); 
    wrenSetSlotHandle(interpreter->vm, 0, interpreter->handles[RECEIVER]);
    wrenSetSlotDouble(interpreter->vm, 1, delta_time);
    wrenCall(interpreter->vm, interpreter->handles[UPDATE]);
}

void Interpreter_render(Interpreter_t *interpreter, const double ratio)
{
    wrenEnsureSlots(interpreter->vm, 1); 
    wrenSetSlotHandle(interpreter->vm, 0, interpreter->handles[RECEIVER]);
    wrenCall(interpreter->vm, interpreter->handles[RENDER]);
}

void Interpreter_terminate(Interpreter_t *interpreter)
{
    wrenEnsureSlots(interpreter->vm, 1); 
    wrenSetSlotHandle(interpreter->vm, 0, interpreter->handles[RECEIVER]);
    wrenCall(interpreter->vm, interpreter->handles[TERMINATE]);

    for (int i = 0; i < Handles_t_CountOf; ++i) {
        WrenHandle *handle = interpreter->handles[i];
        wrenReleaseHandle(interpreter->vm, handle);
    }

    wrenFreeVM(interpreter->vm);
}