/* mock_helper.c -- mock helper for bash plugin UT. */

void *dlopen(const char *filename, int flags)
{
}

int dlclose(void *handle)
{
}

void *dlsym(void *restrict handle, const char *restrict symbol)
{
}

char *dlerror(void)
{
}

char *get_string_value(const char * str)
{
}

int absolute_program (const char * str)
{
}