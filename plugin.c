/* plugin.c -- Bash plugin support. */

/* Copyright (C) 1987-2016 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#if !defined (__GNUC__) && !defined (HAVE_ALLOCA_H) && defined (_AIX)
  #pragma alloca
#endif /* _AIX && RISC6000 && !__GNUC__ */

#include <stdio.h>
#include <dlfcn.h>
#include "chartypes.h"
#include "bashtypes.h"
#if !defined (_MINIX) && defined (HAVE_SYS_FILE_H)
#  include <sys/file.h>
#endif
#include "filecntl.h"
#include "posixstat.h"
#include <signal.h>
#if defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "posixtime.h"

#if defined (HAVE_SYS_RESOURCE_H) && !defined (RLIMTYPE)
#  include <sys/resource.h>
#endif

#if defined (HAVE_SYS_TIMES_H) && defined (HAVE_TIMES)
#  include <sys/times.h>
#endif

#include <errno.h>

#if !defined (errno)
extern int errno;
#endif

#define NEED_FPURGE_DECL
#define NEED_SH_SETLINEBUF_DECL

#include "bashansi.h"
#include "bashintl.h"

#include "memalloc.h"
#include "shell.h"
#include <y.tab.h>    /* use <...> so we pick it up from the build directory */
#include "error.h"
#include "flags.h"
#include "builtins.h"
#include "hashlib.h"
#include "jobs.h"
#include "execute_cmd.h"
#include "findcmd.h"
#include "redir.h"
#include "trap.h"
#include "pathexp.h"
#include "hashcmd.h"

#if defined (BASH_PLUGIN)
#include "plugin.h"
#endif /* BASH_PLUGIN */

#if defined (COND_COMMAND)
#  include "test.h"
#endif

#include "builtins/common.h"
#include "builtins/builtext.h"    /* list of builtins */

#include "builtins/getopt.h"

#include <glob/strmatch.h>
#include <tilde/tilde.h>

#if defined (BUFFERED_INPUT)
#  include "input.h"
#endif

#if defined (ALIAS)
#  include "alias.h"
#endif

#if defined (HISTORY)
#  include "bashhist.h"
#endif

#if defined (HAVE_MBSTR_H) && defined (HAVE_MBSCHR)
#  include <mbstr.h>        /* mbschr */
#endif

/* plugin configration file */
const char *plugin_config_file = "/etc/bash_plugins.conf";

/* plugin on_shell_execve function handle type */
typedef int on_shell_execve_t (char *user, int shell_level, char *cmd, char **argv);

/* plugin plugin_init function handle type */
typedef int plugin_init_t ();

/* plugin plugin_uninit function handle type */
typedef int plugin_uninit_t ();

/* plugin on_shell_execve function name */
static const char *on_shell_execve_function_name = "on_shell_execve";

/* plugin plugin_init function name */
static const char *plugin_init_function_name = "plugin_init";

/* plugin plugin_uninit function name */
static const char *plugin_uninit_function_name = "plugin_uninit";

/* Plugin list node. */
typedef struct plugin_node {
    
    /* Next plugin pointer. */
  struct plugin_node *next;
  
    /* Plugin library handle. */
  void *plugin_handle;
  
    /* Plugin on_shell_execve function handle. */
  on_shell_execve_t *on_shell_execve;
  
    /* Plugin plugin_init function handle. */
  plugin_init_t *plugin_init;
  
    /* Plugin plugin_uninit function handle. */
  plugin_uninit_t *plugin_uninit;
} PLUGIN_NODE;

/* plugin handle for test */
static PLUGIN_NODE *global_plugin_list = NULL;

/* Load plugin by plugin path */
void
append_plugin(
        void *plugin_handle,
        on_shell_execve_t *on_shell_execve,
        plugin_init_t *plugin_init,
        plugin_uninit_t *plugin_uninit)
{
    /* Create and initialize new plugin */
    PLUGIN_NODE *new_plugin_node = (PLUGIN_NODE*)malloc(sizeof(PLUGIN_NODE));
    new_plugin_node->next = NULL;
    new_plugin_node->plugin_handle = plugin_handle;
    new_plugin_node->on_shell_execve = on_shell_execve;
    new_plugin_node->plugin_init = plugin_init;
    new_plugin_node->plugin_uninit = plugin_uninit;
    
    /* Walk to last plugin */
    PLUGIN_NODE **current_plugin_node = &global_plugin_list;
    while (*current_plugin_node != NULL) {
        current_plugin_node = &((*current_plugin_node)->next);
    }
    
    /* append new plugin to tail node */
    *current_plugin_node = new_plugin_node;
}


/* Load plugin by plugin path */
void
try_load_plugin_by_path(const char *plugin_path)
{
    /* Plugin handle */
    void *plugin_handle;
    if ( (plugin_handle = dlopen(plugin_path, RTLD_LAZY)) == NULL) {
#ifdef DEBUG
        itrace("Plugin: can't load plugin %s: %s\n", plugin_path, dlerror());
#endif
        return;
    }

    /* Check if plugin support shell execve method */
    on_shell_execve_t* plugin_on_shell_execve_handle = dlsym(plugin_handle, on_shell_execve_function_name);
    if (dlerror() != NULL) {
        dlclose(plugin_handle);

#ifdef DEBUG
        itrace("Plugin: can't find on_shell_execve function %s: %s\n", plugin_path, dlerror());
#endif
        return;
    }
    

    /* Check if plugin support un-initialization method */
    plugin_uninit_t* plugin_uninit_handle = dlsym(plugin_handle, plugin_uninit_function_name);
    if (dlerror() != NULL) {
        dlclose(plugin_handle);

#ifdef DEBUG
        itrace("Plugin: can't find plugin_uninit function %s: %s\n", plugin_path, dlerror());
#endif
        return;
    }

    /* Check if plugin support initialization method */
    plugin_init_t* plugin_init_handle = dlsym(plugin_handle, plugin_init_function_name);
    if (dlerror() != NULL) {
        dlclose(plugin_handle);

#ifdef DEBUG
        itrace("Plugin: can't find plugin_init function %s: %s\n", plugin_path, dlerror());
#endif
        return;
    }
    else {
        /* Initialize plugin */
        plugin_init_handle();
    }

    /* Add plugin to plugin list */
    append_plugin(plugin_handle,
                    plugin_on_shell_execve_handle,
                    plugin_init_handle,
                    plugin_uninit_handle);
    
#ifdef DEBUG
    itrace("Plugin: plugin %s loaded\n", plugin_path);
#endif
}

/* Load plugin by config file */
void
load_plugin_by_config(const char *config_filename)
{
    FILE *config_file;
    char buffer[256];

    config_file = fopen(config_filename, "r");
    if(config_file == NULL) {
#ifdef DEBUG
            itrace("Plugin: can't open plugin config file %s: %s\n", config_filename, strerror(errno));
#endif
        return;
    }

    while(fgets(buffer, sizeof buffer, config_file)) {
        if(*buffer == '#' || isspace(*buffer)) {
            /* ignore comments or white space. */
            continue; 
        }
        
        /* read to first whitespace. */
        strtok(buffer, " \t\n\r\f"); 

        if(!strncmp(buffer, "plugin=", 7)) {
            /* read plugin path. */
            char* plugin_path = strtok(buffer+7, " \t\n\r\f"); 
#ifdef DEBUG
            itrace("Plugin: load plugin: %s\n", plugin_path);
#endif
            try_load_plugin_by_path(plugin_path);
        }
#ifdef DEBUG
        else {
            /* output debug message. */
            itrace("Plugin: unrecognized parameter: %s\n", buffer);
        }
#endif
    }

    fclose(config_file);
}

/* Free loaded plugins */
void
free_loaded_plugins()
{
    if ( global_plugin_list == NULL) {
        return;
    }
    
    /* Walk to last plugin */
    PLUGIN_NODE **current_plugin_node = &global_plugin_list;
    while (*current_plugin_node != NULL) {
        
        /* Unload plugin */
        (*current_plugin_node)->plugin_uninit();
        dlclose((*current_plugin_node)->plugin_handle);
        
        /* Continue with next pligin */
        current_plugin_node = &((*current_plugin_node)->next);
    }
}

/* Invoke loaded plugins */
int
invoke_loaded_plugins (user, shell_level, cmd, argv)
     char *user;
     int shell_level;
     char *cmd;
     char **argv;
{
    if (global_plugin_list == NULL) {
        return;
    }

    /* Walk to last plugin */
    PLUGIN_NODE **current_plugin_node = &global_plugin_list;
    while (*current_plugin_node != NULL) {

        /* Call plugin method */
        int plugin_error_code = (*current_plugin_node)->on_shell_execve(user, shell_level, cmd, argv);
        if (plugin_error_code != 0) {
#ifdef DEBUG
            itrace("Plugin: on_execve return error: %d\n", plugin_error_code);
#endif
            /* Exit when plugin failed */
            return plugin_error_code;
        }
        
        /* Continue with next pligin */
        current_plugin_node = &((*current_plugin_node)->next);
    }
    
    return 0;
}

/* Load all plugins。 */
void
load_plugins ()
{
    // plugin_config_file
    load_plugin_by_config("/home/joe/poc/gnu_bash/bash_plugins.conf");
}

/* Free all plugins */
void
free_plugins ()
{
    free_loaded_plugins();
}

/* Invoke plugins before shell execve */
int
invoke_plugin_on_shell_execve (user, cmd, argv)
     char *user;
     char *cmd;
     char **argv;
{
    const char* shell_level_str = get_string_value ("SHLVL");
    const int shell_level = atoi (shell_level_str);
    return invoke_loaded_plugins(user, shell_level, cmd, argv);
}