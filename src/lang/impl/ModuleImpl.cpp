#include "ModuleImpl.h"
#include "../Exception.h"
#include "../BasicClasses.h"
#include "../Symbol.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

void ModuleImpl::loadImpl() {
    auto moduleClass = BasicClasses::ModuleClass;
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "<", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "<=", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "<=>", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "==", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "===", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, ">", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, ">=", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "alias_method", 2)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "ancestors", 0)
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "attr")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "attr_accessor")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "attr_reader")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "attr_writer")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "autoload", 2)
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "autoload?")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "class_variable_defined?", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "class_variable_get", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "class_variable_set", 2)
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "class_variables")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "const_defined?")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "const_get")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "const_missing", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "const_set", 2)
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "const_source_location")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "constants")
    DEFINE_INSTANCE_METHOD_P(moduleClass, ModuleImpl::less, "define_method")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "deprecate_constant")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "freeze", 0)
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "include")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "include?", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "included_modules", 0)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "instance_method", 1)
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "instance_methods")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "method_defined?")
    DEFINE_INSTANCE_METHOD_P(moduleClass, ModuleImpl::less, "module_eval")
    DEFINE_INSTANCE_METHOD_B(moduleClass, ModuleImpl::less, "module_exec")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "name", 0)
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "prepend")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "private_class_method")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "private_constant")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "private_instance_methods")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "private_method_defined?")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "protected_instance_methods")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "protected_method_defined?")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "public_class_method")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "public_constant")
    DEFINE_INSTANCE_METHOD_V(moduleClass, ModuleImpl::less, "public_instance_method")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "public_instance_methods")
    DEFINE_INSTANCE_METHOD_N(moduleClass, ModuleImpl::less, "public_method_defined?")
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "refinements", 0)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "remove_class_variable", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "remove_method", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "set_temporary_name", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "singleton_class?", 0)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "to_s", 0)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "undef_method", 1)
    DEFINE_INSTANCE_METHOD_C(moduleClass, ModuleImpl::less, "undefined_instance_methods", 0)
    ClassImpl::loadImpl();
}

void ModuleImpl::ClassImpl::loadImpl() {
    auto moduleClass = BasicClasses::ModuleClass;
    DEFINE_SINGLETON_METHOD_V(moduleClass, ClassImpl::constants, "constants")
    DEFINE_SINGLETON_METHOD_C(moduleClass, ClassImpl::nesting, "nesting", 0)
    DEFINE_SINGLETON_METHOD_P(moduleClass, ClassImpl::new_impl, "new")
    DEFINE_SINGLETON_METHOD_C(moduleClass, ClassImpl::used_modules, "used_modules", 1)
    DEFINE_SINGLETON_METHOD_C(moduleClass, ClassImpl::used_refinements, "used_refinements", 1)
}

Object *ModuleImpl::ClassImpl::constants(int argc, Object **argv) {
    if (argc == 0) {
        //todo find previous call stack frame and find self pointer within it
        //todo iterate over consts
    } else if (argc == 1) {
        //todo investigate method
    } else {
        THROW_ARGS_COUNT_EXCEPTION(argc, 0..1);
    }
}

Object *ModuleImpl::ClassImpl::nesting() {
    return nullptr;
}

Object *ModuleImpl::ClassImpl::new_impl() {
    return nullptr;
}

Object *ModuleImpl::ClassImpl::used_modules() {
    return nullptr;
}

Object *ModuleImpl::ClassImpl::used_refinements() {
    return nullptr;
}

Object *ModuleImpl::less(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::less_equal(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::comparison(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::equal(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::case_equal(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::greater(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::greater_equal(Object *rhs) {
    return nullptr;
}

Object *ModuleImpl::alias_method(Object *newName, Object *oldName) {
    return nullptr;
}

Object *ModuleImpl::ancestors() {
    return nullptr;
}

Object *ModuleImpl::attr(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::attr_accessor(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::attr_reader(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::attr_writer(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::autoload(Object *constant, Object *filename) {
    return nullptr;
}

Object *ModuleImpl::autoload_qm(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::class_variable_defined(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::class_variable_get(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::class_variable_set(Object *sym, Object *val) {
    return nullptr;
}

Object *ModuleImpl::class_variables(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::const_defined(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::const_get(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::const_missing(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::const_set(Object *sym, Object *val) {
    return nullptr;
}

Object *ModuleImpl::const_source_location(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::constants(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::define_method(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::deprecate_constant(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::freeze() {
    return nullptr;
}

Object *ModuleImpl::include(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::include_qm(Object *module) {
    return nullptr;
}

Object *ModuleImpl::included_modules() {
    return nullptr;
}

Object *ModuleImpl::instance_method(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::instance_methods(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::method_defined(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::module_eval(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::module_exec(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::name_impl() {
    return nullptr;
}

Object *ModuleImpl::prepend(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::private_class_method(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::private_constant(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::private_instance_methods(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::private_method_defined_qm(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::protected_instance_methods(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::protected_method_defined(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::public_class_method(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::public_constant(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::public_instance_method(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::public_instance_methods(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::public_method_defined_qm(int argc, Object **argv) {
    return nullptr;
}

Object *ModuleImpl::refinements() {
    return nullptr;
}

Object *ModuleImpl::remove_class_variable(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::remove_method(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::set_temporary_name(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::singleton_class_qm() {
    return nullptr;
}

Object *ModuleImpl::to_s() {
    return nullptr;
}

Object *ModuleImpl::undef_method(Object *sym) {
    return nullptr;
}

Object *ModuleImpl::undefined_instance_methods() {
    return nullptr;
}


} // UltraRuby
} // Lang
} // Impl