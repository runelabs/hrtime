/*
   Copyright 2010 Rune Karlsen <runelabs@gmail.com>. All rights reserved.
   Released under the "Simplified BSD License".

   Redistribution and use in source and binary forms, with or without modification, are
   permitted provided that the following conditions are met:

      1. Redistributions of source code must retain the above copyright notice, this list of
         conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright notice, this list
         of conditions and the following disclaimer in the documentation and/or other materials
         provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY RUNE KARLSEN ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RUNE KARLSEN OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   The views and conclusions contained in the software and documentation are those of the
   authors and should not be interpreted as representing official policies, either expressed
   or implied, of Rune Karlsen <runelabs@gmail.com>.
*/

#include "hrtime.h"

#include <cstring>

using namespace v8;
using namespace node;

extern "C"

void init(Handle<Object> target) {

  hrtime_v8::Initialize(target);

}

namespace hrtime_v8 {


static timespec init_time;
static clockid_t init_clock = CLOCK_MONOTONIC;
static char rns[32] = "";

void Initialize(Handle<Object> target) {

  if (clock_gettime(init_clock, &init_time) != 0 ) {
    init_clock = CLOCK_REALTIME;
    clock_gettime(init_clock, &init_time);
  }

  Local<FunctionTemplate> t1 = FunctionTemplate::New(Time);
  target->Set(String::NewSymbol("time"), t1->GetFunction());

  Local<FunctionTemplate> t2 = FunctionTemplate::New(Uptime);
  target->Set(String::NewSymbol("uptime"), t2->GetFunction());

  Local<FunctionTemplate> t3 = FunctionTemplate::New(Cputime);
  target->Set(String::NewSymbol("cputime"), t3->GetFunction() );

  Local<FunctionTemplate> t4 = FunctionTemplate::New(Initime);
  target->Set(String::NewSymbol("initime"), t4->GetFunction() );

  Local<FunctionTemplate> t5 = FunctionTemplate::New(Runtime);
  target->Set(String::NewSymbol("runtime"), t5->GetFunction() );

  Local<FunctionTemplate> t6 = FunctionTemplate::New(Resolution);
  target->Set(String::NewSymbol("resolution"), t6->GetFunction() );

}

/*
 * NAME: time()
 * RETURNS: An ASCII string representing system realtime clock high resolution time in number of nanoseconds.
 *   Note that daylight savings time (DST) and other system clock adjustments will influence this time result.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: If a boolean value of 'true' is passed, then only the nanosecond part of the time will be returned.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_gettime.html
 */
Handle<Value> Time(const Arguments &args) {

  clockid_t clockid = CLOCK_REALTIME;
  timespec new_time;
  char ns[32] = "";
  encoding encode = ASCII;

  if (clock_gettime(clockid, &new_time) != 0) return Encode(ns, 0, encode);

  if (args.Length() > 0) {
    if (args[0]->BooleanValue() == true) {
      sprintf(ns, "%lu", new_time.tv_nsec);
      return Encode(ns, strlen(ns), encode);
    }
  }

  sprintf(ns, "%lu%.9lu", new_time.tv_sec, new_time.tv_nsec);
  return Encode(ns, strlen(ns), encode);

}


/*
 * NAME: uptime()
 * RETURNS: An ASCII string representing the high resolution time in number of nanoseconds that has passed since the last boot time or time epoch.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: NONE.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_gettime.html
 */
Handle<Value> Uptime(const Arguments &args) {

  clockid_t clockid = CLOCK_MONOTONIC;
  timespec new_time;
  char ns[32] = "";
  encoding encode = ASCII;

  if (clock_gettime(clockid, &new_time) != 0) return Encode(ns, 0, encode);

  sprintf(ns, "%lu%.9lu", new_time.tv_sec, new_time.tv_nsec);
  return Encode(ns, strlen(ns), encode);

}


/*
 * NAME: cputime()
 * RETURNS: An ASCII string representing the high resolution time in number of nanoseconds of the accumulated time the cpu has spent working on the process.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: NONE.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_gettime.html
 */
Handle<Value> Cputime(const Arguments &args) {

  clockid_t clockid = CLOCK_PROCESS_CPUTIME_ID;
  timespec new_time;
  char ns[32] = "";
  encoding encode = ASCII;

  if (clock_gettime(clockid, &new_time) != 0) return Encode(ns, 0, encode);

  if (new_time.tv_sec > 0) {
    sprintf(ns, "%lu%.9lu", new_time.tv_sec, new_time.tv_nsec);
  }
  else {
    sprintf(ns, "%lu", new_time.tv_nsec);
  }
  return Encode(ns, strlen(ns), encode);

}


/*
 * NAME: initime()
 * RETURNS: An ASCII string representing the high resolution time in number of nanoseconds of the time when the module was initialized.
 *   Note that the time defaults to the same clock as uptime(), but will use the realtime clock if the former is unavailable.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: NONE.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_gettime.html
 */
Handle<Value> Initime(const Arguments &args) {

  char ns[32] = "";
  encoding encode = ASCII;

  if (init_time.tv_sec <= 0 || init_time.tv_nsec <= 0) return Encode(ns, 0, encode);

  sprintf(ns, "%lu%.9lu", init_time.tv_sec, init_time.tv_nsec);
  return Encode(ns, strlen(ns), encode);

}


/*
 * NAME: runtime()
 * RETURNS: An ASCII string representing the high resolution time in number of nanoseconds of the time passed since the module was initialized.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: NONE.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_gettime.html
 */
Handle<Value> Runtime(const Arguments &args) {

  clockid_t clockid = init_clock;
  timespec new_time;
  timespec run_time;
  char ns[32] = "";
  encoding encode = ASCII;

  if (clock_gettime(clockid, &new_time) != 0) return Encode(ns, 0, encode);

  run_time.tv_sec = new_time.tv_sec - init_time.tv_sec;
  if (new_time.tv_nsec < init_time.tv_nsec) {
    --run_time.tv_sec;
    run_time.tv_nsec = (1000000000 + new_time.tv_nsec) - init_time.tv_nsec;
  }
  else {
    run_time.tv_nsec = new_time.tv_nsec - init_time.tv_nsec;
  }

  if (run_time.tv_sec > 0) {
    sprintf(ns, "%lu%.9lu", run_time.tv_sec, run_time.tv_nsec);
  }
  else {
    sprintf(ns, "%lu", run_time.tv_nsec);
  }
  return Encode(ns, strlen(ns), encode);

}


/*
 * NAME: resolution()
 * RETURNS: An ASCII string representing the resolution of the realtime clock in number of nanoseconds.
 * ERRORS: Returns an empty string ("") on any error that occured during the request.
 * PARAMETERS: NONE.
 * SEE: http://www.opengroup.org/onlinepubs/009695399/functions/clock_getres.html
 */
Handle<Value> Resolution(const Arguments &args) {

  clockid_t clockid = CLOCK_REALTIME;
  timespec new_res;
  char ns[16] = "";
  encoding encode = ASCII;

  if (clock_getres(clockid, &new_res) != 0) return Encode(ns, 0, encode);

  sprintf(ns, "%lu", new_res.tv_nsec);
  return Encode(ns, strlen(ns), encode);

}



}
