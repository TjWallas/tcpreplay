/* $Id: signal_handler.c,v 1.3 2003/08/31 01:12:38 aturner Exp $ */

/*
 * File: signal_handler.c 
 *
 * Author: Jeff Guttenfelder
 *         Nathan Monteleone
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Anzen Computing, Inc.
 * 4. Neither the name of Anzen Computing, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Description: 
 *     This file contains routines relating to signals. 
 *
 * Modifications:
 *      01/24/2003  Added suspend signal support.
 */

#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "signal_handler.h"
#include "timer.h"
#include "err.h"

struct timeval suspend_time;
static struct timeval suspend_start;
static struct timeval suspend_end;

/*
 * init_signal_handlers - 
 *     Initialize signal handlers to be used in tcpreplay.
 */
void init_signal_handlers()
{
    signal(SIGUSR1, suspend_handler);
    signal(SIGCONT, continue_handler);

    reset_suspend_time();
}

/*
 * reset_suspend_time -
 *     Reset time values for suspend signal.
 */
void reset_suspend_time()
{
    timerclear(&suspend_time);
    timerclear(&suspend_start);
    timerclear(&suspend_end);
}

/*
 * suspend signal handler -
 *     Signal handler for signal SIGUSR1. SIGSTOP cannot be 
 * caught, so SIGUSR1 is caught and it throws SIGSTOP.
 */
void suspend_handler( int signo )
{
    if (gettimeofday(&suspend_start, NULL) < 0)
        err(1, "gettimeofday");

    kill(getpid(), SIGSTOP);
}

/*
 * continue_handler -
 *     Signal handler for continue signal.
 */
void continue_handler( int signo )
{
    struct timeval suspend_delta;

    if (gettimeofday(&suspend_end, NULL) < 0)
        err(1, "gettimeofday");

    timersub(&suspend_end, &suspend_start, &suspend_delta);
    timeradd(&suspend_time, &suspend_delta, &suspend_time);
}
