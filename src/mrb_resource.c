/*
** mrb_resource.c - Resource class
**
** Copyright (c) SOGO Haraguchi 2016
**
** See Copyright Notice in LICENSE
*/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "mruby.h"
#include "mruby/array.h"

#include "mrb_resource.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

static mrb_value mrb_resource_getrusage(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb, "hi!!");
}

static mrb_value mrb_resource_getrlimit(mrb_state *mrb, mrb_value self) {

  struct rlimit rl;
  int rc = 0;
  mrb_value array = mrb_nil_value();

  mrb_int resource;
  mrb_get_args(mrb, "i", &resource);

  rc = getrlimit(resource, &rl);
  if (rc < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%S:%S\n", mrb_fixnum_value(errno),
               mrb_str_new_cstr(mrb, strerror(errno)));
  }

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_float_value(mrb, rl.rlim_cur));
  mrb_ary_push(mrb, array, mrb_float_value(mrb, rl.rlim_max));

  return array;
}

static mrb_value mrb_resource_setrlimit(mrb_state *mrb, mrb_value self) {

  struct rlimit rl;
  int rc = 0;

  mrb_value cur_limit, max_limit;
  mrb_int resource;
  mrb_int argc;

  argc = mrb_get_args(mrb, "if|f", &resource, &cur_limit, &max_limit);
  if (argc == 2) {
    max_limit = cur_limit;
  }

  rl.rlim_cur = (rlim_t)mrb_float(cur_limit);
  rl.rlim_max = (rlim_t)mrb_float(max_limit);

  rc = setrlimit(resource, &rl);
  if (rc < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%S:%S\n", mrb_fixnum_value(errno),
               mrb_str_new_cstr(mrb, strerror(errno)));
  }

  return mrb_nil_value();
}

void mrb_mruby_resource_gem_init(mrb_state *mrb) {

  struct RClass *resource;
  resource = mrb_define_module(mrb, "Resource");

#ifdef RLIMIT_AS
  mrb_define_const(mrb, resource, "RLIMIT_AS", mrb_fixnum_value(RLIMIT_AS));
#endif
#ifdef RLIMIT_CORE
  mrb_define_const(mrb, resource, "RLIMIT_CORE", mrb_fixnum_value(RLIMIT_CORE));
#endif
#ifdef RLIMIT_CPU
  mrb_define_const(mrb, resource, "RLIMIT_CPU", mrb_fixnum_value(RLIMIT_CPU));
#endif
#ifdef RLIMIT_DATA
  mrb_define_const(mrb, resource, "RLIMIT_DATA", mrb_fixnum_value(RLIMIT_DATA));
#endif
#ifdef RLIMIT_FSIZE
  mrb_define_const(mrb, resource, "RLIMIT_FSIZE",
                   mrb_fixnum_value(RLIMIT_FSIZE));
#endif
#ifdef RLIMIT_LOCKS
  mrb_define_const(mrb, resource, "RLIMIT_LOCKS",
                   mrb_fixnum_value(RLIMIT_LOCKS));
#endif
#ifdef RLIMIT_MEMLOCK
  mrb_define_const(mrb, resource, "RLIMIT_MEMLOCK",
                   mrb_fixnum_value(RLIMIT_MEMLOCK));
#endif
#ifdef RLIMIT_MSGQUEUE
  mrb_define_const(mrb, resource, "RLIMIT_MSGQUEUE",
                   mrb_fixnum_value(RLIMIT_MSGQUEUE));
#endif
#ifdef RLIMIT_NICE
  mrb_define_const(mrb, resource, "RLIMIT_NICE", mrb_fixnum_value(RLIMIT_NICE));
#endif
#ifdef RLIMIT_NOFILE
  mrb_define_const(mrb, resource, "RLIMIT_NOFILE",
                   mrb_fixnum_value(RLIMIT_NOFILE));
#endif
#ifdef RLIMIT_NPROC
  mrb_define_const(mrb, resource, "RLIMIT_NPROC",
                   mrb_fixnum_value(RLIMIT_NPROC));
#endif
#ifdef RLIMIT_RSS
  mrb_define_const(mrb, resource, "RLIMIT_RSS", mrb_fixnum_value(RLIMIT_RSS));
#endif
#ifdef RLIMIT_RTPRIO
  mrb_define_const(mrb, resource, "RLIMIT_RTPRIO",
                   mrb_fixnum_value(RLIMIT_RTPRIO));
#endif
#ifdef RLIMIT_RTTIME
  mrb_define_const(mrb, resource, "RLIMIT_RTTIME",
                   mrb_fixnum_value(RLIMIT_RTTIME));
#endif
#ifdef RLIMIT_SIGPENDING
  mrb_define_const(mrb, resource, "RLIMIT_SIGPENDING",
                   mrb_fixnum_value(RLIMIT_SIGPENDING));
#endif
#ifdef RLIMIT_STACK
  mrb_define_const(mrb, resource, "RLIMIT_STACK",
                   mrb_fixnum_value(RLIMIT_STACK));
#endif
#ifdef RLIM_INFINITY
  mrb_define_const(mrb, resource, "RLIM_INFINITY",
                   mrb_float_value(mrb, (mrb_float)RLIM_INFINITY));
#endif
#ifdef RLIM_SAVED_CUR
  mrb_define_const(mrb, resource, "RLIM_SAVED_CUR",
                   mrb_float_value(mrb, (mrb_float)RLIM_SAVED_CUR));
#endif
#ifdef RLIM_SAVED_MAX
  mrb_define_const(mrb, resource, "RLIM_SAVED_MAX",
                   mrb_float_value(mrb, (mrb_float)RLIM_SAVED_MAX));
#endif
#ifdef RUSAGE_SELF
  mrb_define_const(mrb, resource, "RUSAGE_SELF", mrb_fixnum_value(RUSAGE_SELF));
#endif
#ifdef RUSAGE_CHILDREN
  mrb_define_const(mrb, resource, "RUSAGE_CHILDREN",
                   mrb_fixnum_value(RUSAGE_CHILDREN));
#endif
#ifdef RUSAGE_BOTH
  mrb_define_const(mrb, resource, "RUSAGE_BOTH", mrb_fixnum_value(RUSAGE_BOTH));
#endif
#ifdef RUSAGE_THREAD
  mrb_define_const(mrb, resource, "RUSAGE_THREAD",
                   mrb_fixnum_value(RUSAGE_THREAD));
#endif

  mrb_define_module_function(mrb, resource, "getrusage", mrb_resource_getrusage,
                             MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, resource, "getrlimit", mrb_resource_getrlimit,
                             MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, resource, "setrlimit", mrb_resource_setrlimit,
                             MRB_ARGS_ARG(2, 1));
  DONE;
}

void mrb_mruby_resource_gem_final(mrb_state *mrb) {}
