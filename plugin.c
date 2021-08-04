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
#include <y.tab.h>	/* use <...> so we pick it up from the build directory */
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
#include "tacacsauth.h"

#if defined (BASH_PLUGIN)
#include "plugin.h"
#endif /* BASH_PLUGIN */

#if defined (COND_COMMAND)
#  include "test.h"
#endif

#include "builtins/common.h"
#include "builtins/builtext.h"	/* list of builtins */

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
#  include <mbstr.h>		/* mbschr */
#endif


/* Load all plugins。 */
int
load_plugins ()
{
    internal_warning ("load_plugins\n");
}

/* Free all plugins */
int
free_plugins ()
{
    internal_warning ("free_plugins\n");
}

/* Invoke plugins before shell execve */
int
invoke_plugin_on_shell_execve (cmd, argv)
     char *cmd;
     char **argv;
{
    internal_warning ("invoke_plugin_on_shell_execve\n");
}