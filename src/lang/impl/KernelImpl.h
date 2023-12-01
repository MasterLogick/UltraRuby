#ifndef ULTRA_RUBY_KERNELIMPL_H
#define ULTRA_RUBY_KERNELIMPL_H

#include "../Class.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

class KernelImpl : private Object {
public:
    static void loadImpl();

private:
    Object *array(Object *obj);

    Object *bigDecimal(int argc, Object **argv);

    Object *complex(int argc, Object **argv);

    Object *float_impl(int argc, Object **argv);

    Object *hash(Object *obj);

    Object *integer(int argc, Object **argv);

    Object *pathname(Object *obj);

    Object *rational(int argc, Object **argv);

    Object *string_impl(int argc, Object **argv);

    Object *callee();

    Object *dir();

    Object *method();

    Object *subshell();

    Object *abort(int argc, Object **argv);

    Object *at_exit();

    Object *autoload(Object *constant, Object *filename);

    Object *autoload_qm(int argc, Object **argv);

    Object *binding();

    Object *block_given();

    Object *callcc();

    Object *caller(int argc, Object **argv);

    Object *caller_locations(int argc, Object **argv);

    Object *catch_impl(int argc, Object **argv);

    Object *chomp(int argc, Object **argv);

    Object *chop();

    Object *class_impl();

    Object *clone(int argc, Object **argv);

    Object *eval(int argc, Object **argv);

    Object *exec(int argc, Object **argv);

    Object *exit(int argc, Object **argv);

    Object *exit_immediate(int argc, Object **argv);

    Object *fork();

    Object *frozen();

    Object *gets(int argc, Object **argv);

    Object *global_variables();

    Object *gsub(int argc, Object **argv);

    Object *iterator();

    Object *lambda();

    Object *load(int argc, Object **argv);

    Object *local_variables();

    Object *loop();

    Object *open(int argc, Object **argv);

    Object *p(int argc, Object **argv);

    Object *print(int argc, Object **argv);

    Object *printf(int argc, Object **argv);

    Object *proc();

    Object *putc(Object *val);

    Object *puts(int argc, Object **argv);

    Object *raise(int argc, Object **argv);

    Object *rand(int argc, Object **argv);

    Object *readline(int argc, Object **argv);

    Object *readlines(int argc, Object **argv);

    Object *require_relative(Object *str);

    Object *select(int argc, Object **argv);

    Object *set_trace_func(Object *proc);

    Object *sleep(int argc, Object **argv);

    Object *spawn(int argc, Object **argv);

    Object *sprintf(int argc, Object **argv);

    Object *srand(int argc, Object **argv);

    Object *sub(int argc, Object **argv);

    Object *syscall(int argc, Object **argv);

    Object *system(int argc, Object **argv);

    Object *test(int argc, Object **argv);

    Object *then();

    Object *throw_impl(int argc, Object **argv);

    Object *trace_var(int argc, Object **argv);

    Object *trap(int argc, Object **argv);

    Object *untrace_var(int argc, Object **argv);

    Object *warn(int argc, Object **argv);

    Object *JSON(int argc, Object **argv);

    Object *gem(int argc, Object **argv);

    Object *j(int argc, Object **argv);

    Object *jj(int argc, Object **argv);

    Object *pp(int argc, Object **argv);

    Object *require(Object *str);

    Object *y(int argc, Object **argv);
};

} // UltraRuby
} // Lang
} // Impl

#endif //ULTRA_RUBY_KERNELIMPL_H
