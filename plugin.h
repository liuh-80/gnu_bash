/* plugin.h - functions from plugin.c. */

/* Copyright (C) 1993-2015 Free Software Foundation, Inc.

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

#if !defined (_PLUGIN_H_)
#define _PLUGIN_H_

#include "stdc.h"

/* System-wide bash plugin configuration. */
#define SYS_BASH_PLUGIN "/etc/bash.plugin"

typedef enum { T_COMMAND } plugin_type_t;

typedef struct bash_plugin_conf
{
	const char *path;	/* path to binary */
	char *name;		/* Used to distinguish plugins */
} bash_plugin_conf_t;

/* Load all plugins */
extern void load_plugins __P((void));

/* Free all plugins */
extern void free_plugins __P((void));

/* Invoke plugins before shell execve */
extern void invoke_plugin_on_shell_execve __P((char *,char **));

#endif /* _PLUGIN_H_ */
