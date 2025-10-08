#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *handle;
    void (*func)(void);
    char *error;


    handle = dlopen("../build/asgard_lib", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    func = dlsym(handle, "print_message");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "Ошибка поиска функции: %s\n", error);
        dlclose(handle);
        return 1;
    }

    func();

    dlclose(handle);
    return 0;
}