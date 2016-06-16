/*
** mrb_resource.c - Resource class
**
** Copyright (c) SOGO Haraguchi 2016
**
** See Copyright Notice in LICENSE
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "mruby.h"
#include "mruby/array.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/hash.h"

#include "mrb_resource.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

#define SET_MRB_KEY_RUSAGE_MEMBER1(hash, rusage, member)                                                               \
  mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, #member)),                                             \
               mrb_float_value(mrb, (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / 1e6));

#define SET_MRB_KEY_RUSAGE_MEMBER2(hash, rusage, member)                                                               \
  mrb_hash_set(mrb, hash, mrb_symbol_value(mrb_intern_cstr(mrb, #member)), mrb_float_value(mrb, rusage.member));

typedef struct mrb_resource_ctx {
  struct rusage *ru;
} mrb_resource_ctx;

static const struct mrb_data_type mrb_resource_ctx_type = {
    "mrb_resource_ctx_type", mrb_free,
};

static void mrb_resource_rusage_inner(mrb_state *mrb, struct rusage *ru, mrb_int who)
{
  int ret = getrusage(who, ru);

  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%S:%S\n", mrb_fixnum_value(errno), mrb_str_new_cstr(mrb, strerror(errno)));
  }
}

static mrb_value mrb_resource_getrusage_init(mrb_state *mrb, mrb_value self)
{
  struct rusage ru;
  mrb_int who;
  mrb_resource_ctx *ctx;

  mrb_get_args(mrb, "i", &who);
  mrb_resource_rusage_inner(mrb, &ru, who);

  ctx = (mrb_resource_ctx *)DATA_PTR(self);
  if (ctx) {
    mrb_free(mrb, ctx);
  }

  DATA_TYPE(self) = &mrb_resource_ctx_type;
  ctx = (mrb_resource_ctx *)mrb_malloc(mrb, sizeof(mrb_resource_ctx));
  ctx->ru = &ru;

  DATA_PTR(self) = ctx;

  return self;
}

static mrb_value mrb_resource_getrusage_ru_utime(mrb_state *mrb, mrb_value self)
{
  mrb_resource_ctx *ctx = (mrb_resource_ctx *)DATA_PTR(self);

  return mrb_float_value(mrb, (double)ctx->ru->ru_utime.tv_sec + (double)ctx->ru->ru_utime.tv_usec / 1e6);
}

static mrb_value mrb_resource_getrusage(mrb_state *mrb, mrb_value self)
{

  struct rusage ru;
  mrb_value r;

  mrb_int who;
  mrb_get_args(mrb, "i", &who);

  mrb_resource_rusage_inner(mrb, &ru, who);

  r = mrb_hash_new(mrb);
  SET_MRB_KEY_RUSAGE_MEMBER1(r, ru, ru_utime);
  SET_MRB_KEY_RUSAGE_MEMBER1(r, ru, ru_stime);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_maxrss);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_ixrss);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_idrss);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_isrss);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_minflt);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_majflt);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_nswap);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_inblock);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_oublock);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_msgsnd);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_msgrcv);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_nsignals);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_nvcsw);
  SET_MRB_KEY_RUSAGE_MEMBER2(r, ru, ru_nivcsw);

  return r;
}

static mrb_value mrb_resource_getrlimit(mrb_state *mrb, mrb_value self)
{

  struct rlimit rl;
  int rc = 0;
  mrb_value array = mrb_nil_value();

  mrb_int resource;
  mrb_get_args(mrb, "i", &resource);

  rc = getrlimit(resource, &rl);
  if (rc < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%S:%S\n", mrb_fixnum_value(errno), mrb_str_new_cstr(mrb, strerror(errno)));
  }

  array = mrb_ary_new(mrb);
  mrb_ary_push(mrb, array, mrb_float_value(mrb, rl.rlim_cur));
  mrb_ary_push(mrb, array, mrb_float_value(mrb, rl.rlim_max));

  return array;
}

static mrb_value mrb_resource_setrlimit(mrb_state *mrb, mrb_value self)
{

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
    mrb_raisef(mrb, E_RUNTIME_ERROR, "%S:%S\n", mrb_fixnum_value(errno), mrb_str_new_cstr(mrb, strerror(errno)));
  }

  return mrb_nil_value();
}

void mrb_mruby_resource_gem_init(mrb_state *mrb)
{

  struct RClass *resource, *rusage;
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
  mrb_define_const(mrb, resource, "RLIMIT_FSIZE", mrb_fixnum_value(RLIMIT_FSIZE));
#endif
#ifdef RLIMIT_LOCKS
  mrb_define_const(mrb, resource, "RLIMIT_LOCKS", mrb_fixnum_value(RLIMIT_LOCKS));
#endif
#ifdef RLIMIT_MEMLOCK
  mrb_define_const(mrb, resource, "RLIMIT_MEMLOCK", mrb_fixnum_value(RLIMIT_MEMLOCK));
#endif
#ifdef RLIMIT_MSGQUEUE
  mrb_define_const(mrb, resource, "RLIMIT_MSGQUEUE", mrb_fixnum_value(RLIMIT_MSGQUEUE));
#endif
#ifdef RLIMIT_NICE
  mrb_define_const(mrb, resource, "RLIMIT_NICE", mrb_fixnum_value(RLIMIT_NICE));
#endif
#ifdef RLIMIT_NOFILE
  mrb_define_const(mrb, resource, "RLIMIT_NOFILE", mrb_fixnum_value(RLIMIT_NOFILE));
#endif
#ifdef RLIMIT_NPROC
  mrb_define_const(mrb, resource, "RLIMIT_NPROC", mrb_fixnum_value(RLIMIT_NPROC));
#endif
#ifdef RLIMIT_RSS
  mrb_define_const(mrb, resource, "RLIMIT_RSS", mrb_fixnum_value(RLIMIT_RSS));
#endif
#ifdef RLIMIT_RTPRIO
  mrb_define_const(mrb, resource, "RLIMIT_RTPRIO", mrb_fixnum_value(RLIMIT_RTPRIO));
#endif
#ifdef RLIMIT_RTTIME
  mrb_define_const(mrb, resource, "RLIMIT_RTTIME", mrb_fixnum_value(RLIMIT_RTTIME));
#endif
#ifdef RLIMIT_SIGPENDING
  mrb_define_const(mrb, resource, "RLIMIT_SIGPENDING", mrb_fixnum_value(RLIMIT_SIGPENDING));
#endif
#ifdef RLIMIT_STACK
  mrb_define_const(mrb, resource, "RLIMIT_STACK", mrb_fixnum_value(RLIMIT_STACK));
#endif
#ifdef RLIM_INFINITY
  mrb_define_const(mrb, resource, "RLIM_INFINITY", mrb_float_value(mrb, (mrb_float)RLIM_INFINITY));
#endif
#ifdef RLIM_SAVED_CUR
  mrb_define_const(mrb, resource, "RLIM_SAVED_CUR", mrb_float_value(mrb, (mrb_float)RLIM_SAVED_CUR));
#endif
#ifdef RLIM_SAVED_MAX
  mrb_define_const(mrb, resource, "RLIM_SAVED_MAX", mrb_float_value(mrb, (mrb_float)RLIM_SAVED_MAX));
#endif
#ifdef RUSAGE_SELF
  mrb_define_const(mrb, resource, "RUSAGE_SELF", mrb_fixnum_value(RUSAGE_SELF));
#endif
#ifdef RUSAGE_CHILDREN
  mrb_define_const(mrb, resource, "RUSAGE_CHILDREN", mrb_fixnum_value(RUSAGE_CHILDREN));
#endif
#ifdef RUSAGE_BOTH
  mrb_define_const(mrb, resource, "RUSAGE_BOTH", mrb_fixnum_value(RUSAGE_BOTH));
#endif
#ifdef RUSAGE_THREAD
  mrb_define_const(mrb, resource, "RUSAGE_THREAD", mrb_fixnum_value(RUSAGE_THREAD));
#endif

  mrb_define_module_function(mrb, resource, "getrusage", mrb_resource_getrusage, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, resource, "getrlimit", mrb_resource_getrlimit, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, resource, "setrlimit", mrb_resource_setrlimit, MRB_ARGS_ARG(2, 1));

  rusage = mrb_define_class_under(mrb, resource, "Rusage", mrb->object_class);
  mrb_define_method(mrb, rusage, "initialize", mrb_resource_getrusage_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rusage, "ru_utime", mrb_resource_getrusage_ru_utime, MRB_ARGS_NONE());
  DONE;
}

void mrb_mruby_resource_gem_final(mrb_state *mrb)
{
}
