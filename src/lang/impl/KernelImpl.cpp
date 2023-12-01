#include "KernelImpl.h"
#include "../BasicClasses.h"
#include "../Symbol.h"
#include "../Exception.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

void KernelImpl::loadImpl() {
    auto *kernelModule = BasicClasses::KernelModule;
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::array, "Array", 1)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::bigDecimal, "BigDecimal")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::complex, "Complex")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::float_impl, "Float")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::hash, "Hash", 1)
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::integer, "Integer")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::pathname, "Pathname", 1)
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::rational, "Rational")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::string_impl, "String", 1)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::callee, "__callee__", 0)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::dir, "__dir__", 0)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::method, "__method__", 0)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::subshell, "`", 1)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::abort, "abort")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::at_exit, "at_exit")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::autoload, "autoload", 2)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::autoload_qm, "autoload?")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::binding, "binding", 0)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::block_given, "block_given?", 0)
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::callcc, "callcc")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::caller, "caller")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::caller_locations, "caller_locations")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::catch_impl, "catch")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::chomp, "chomp")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::chop, "chop", 0)
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::class_impl, "class", 0)
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::clone, "clone")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::eval, "eval")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::exec, "exec")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::exit, "exit")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::exit_immediate, "exit!")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::fork, "fork")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::frozen, "frozen?", 0)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::gets, "gets")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::global_variables, "global_variables", 0)
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::gsub, "gsub")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::iterator, "iterator?", 0)
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::iterator, "lambda")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::load, "load")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::local_variables, "local_variables", 0)
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::loop, "loop")
    DEFINE_INSTANCE_METHOD_B(kernelModule, KernelImpl::open, "open")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::p, "p")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::print, "print")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::printf, "printf")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::proc, "proc")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::puts, "puts")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::raise, "raise")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::rand, "rand")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::readline, "readline")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::readlines, "readlines")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::require_relative, "require_relative", 1)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::select, "select")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::set_trace_func, "set_trace_func", 1)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::sleep, "sleep")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::spawn, "spawn")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::sprintf, "sprintf")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::srand, "srand")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::sub, "sub")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::syscall, "syscall")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::system, "system")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::test, "test")
    DEFINE_INSTANCE_METHOD_P(kernelModule, KernelImpl::then, "then")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::throw_impl, "throw")
    DEFINE_INSTANCE_METHOD_B(kernelModule, KernelImpl::trace_var, "trace_var")
    DEFINE_INSTANCE_METHOD_B(kernelModule, KernelImpl::trap, "trap")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::untrace_var, "untrace_var")
    DEFINE_INSTANCE_METHOD_N(kernelModule, KernelImpl::warn, "warn")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::JSON, "JSON")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::gem, "gem")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::j, "j")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::jj, "jj")
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::pp, "pp")
    DEFINE_INSTANCE_METHOD_C(kernelModule, KernelImpl::require, "require", 1)
    DEFINE_INSTANCE_METHOD_V(kernelModule, KernelImpl::y, "y")
}

Object *KernelImpl::array(Object *obj) {
    throw Exception(nullptr);
}

Object *KernelImpl::bigDecimal(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::complex(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::float_impl(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::hash(Object *obj) {
    throw Exception(nullptr);
}

Object *KernelImpl::integer(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::pathname(Object *obj) {
    throw Exception(nullptr);
}

Object *KernelImpl::rational(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::string_impl(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::callee() {
    throw Exception(nullptr);
}

Object *KernelImpl::dir() {
    throw Exception(nullptr);
}

Object *KernelImpl::method() {
    throw Exception(nullptr);
}

Object *KernelImpl::subshell() {
    throw Exception(nullptr);
}

Object *KernelImpl::abort(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::at_exit() {
    throw Exception(nullptr);
}

Object *KernelImpl::autoload(Object *constant, Object *filename) {
    throw Exception(nullptr);
}

Object *KernelImpl::autoload_qm(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::binding() {
    throw Exception(nullptr);
}

Object *KernelImpl::block_given() {
    throw Exception(nullptr);
}

Object *KernelImpl::callcc() {
    throw Exception(nullptr);
}

Object *KernelImpl::caller(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::caller_locations(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::catch_impl(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::chomp(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::chop() {
    throw Exception(nullptr);
}

Object *KernelImpl::class_impl() {
    throw Exception(nullptr);
}

Object *KernelImpl::clone(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::eval(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::exec(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::exit(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::exit_immediate(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::fork() {
    throw Exception(nullptr);
}

Object *KernelImpl::frozen() {
    throw Exception(nullptr);
}

Object *KernelImpl::gets(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::global_variables() {
    throw Exception(nullptr);
}

Object *KernelImpl::gsub(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::iterator() {
    throw Exception(nullptr);
}

Object *KernelImpl::lambda() {
    throw Exception(nullptr);
}

Object *KernelImpl::load(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::local_variables() {
    throw Exception(nullptr);
}

Object *KernelImpl::loop() {
    throw Exception(nullptr);
}

Object *KernelImpl::open(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::p(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::print(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::printf(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::proc() {
    throw Exception(nullptr);
}

Object *KernelImpl::putc(Object *val) {
    throw Exception(nullptr);
}

Object *KernelImpl::puts(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::raise(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::rand(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::readline(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::readlines(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::require_relative(Object *str) {
    throw Exception(nullptr);
}

Object *KernelImpl::select(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::set_trace_func(Object *proc) {
    throw Exception(nullptr);
}

Object *KernelImpl::sleep(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::spawn(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::sprintf(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::srand(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::sub(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::syscall(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::system(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::test(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::then() {
    throw Exception(nullptr);
}

Object *KernelImpl::throw_impl(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::trace_var(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::trap(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::untrace_var(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::warn(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::JSON(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::gem(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::j(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::jj(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::pp(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *KernelImpl::require(Object *str) {
    throw Exception(nullptr);
}

Object *KernelImpl::y(int argc, Object **argv) {
    throw Exception(nullptr);
}
} // UltraRuby
} // Lang
} // Impl