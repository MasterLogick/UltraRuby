#ifndef ULTRA_RUBY_MODULEIMPL_H
#define ULTRA_RUBY_MODULEIMPL_H

#include "../Class.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

class ModuleImpl : private Object {
public:
    static void loadImpl();

private:
    class ClassImpl : private Class {
    public:
        static void loadImpl();

    private:
        Object *constants(int argc, Object **argv);

        Object *nesting();

        Object *new_impl();

        Object *used_modules();

        Object *used_refinements();
    };

    Object *less(Object *rhs);

    Object *less_equal(Object *rhs);

    Object *comparison(Object *rhs);

    Object *equal(Object *rhs);

    Object *case_equal(Object *rhs);

    Object *greater(Object *rhs);

    Object *greater_equal(Object *rhs);

    Object *alias_method(Object *newName, Object *oldName);

    Object *ancestors();

    Object *attr(int argc, Object **argv);

    Object *attr_accessor(int argc, Object **argv);

    Object *attr_reader(int argc, Object **argv);

    Object *attr_writer(int argc, Object **argv);

    Object *autoload(Object *constant, Object *filename);

    Object *autoload_qm(int argc, Object **argv);

    Object *class_variable_defined(Object *sym);

    Object *class_variable_get(Object *sym);

    Object *class_variable_set(Object *sym, Object *val);

    Object *class_variables(int argc, Object **argv);

    Object *const_defined(int argc, Object **argv);

    Object *const_get(int argc, Object **argv);

    Object *const_missing(Object *sym);

    Object *const_set(Object *sym, Object *val);

    Object *const_source_location(int argc, Object **argv);

    Object *constants(int argc, Object **argv);

    Object *define_method(int argc, Object **argv);

    Object *deprecate_constant(int argc, Object **argv);

    Object *freeze();

    Object *include(int argc, Object **argv);

    Object *include_qm(Object *module);

    Object *included_modules();

    Object *instance_method(Object *sym);

    Object *instance_methods(int argc, Object **argv);

    Object *method_defined(int argc, Object **argv);

    Object *module_eval(int argc, Object **argv);

    Object *module_exec(int argc, Object **argv);

    Object *name_impl();

    Object *prepend(int argc, Object **argv);

    Object *private_class_method(int argc, Object **argv);

    Object *private_constant(int argc, Object **argv);

    Object *private_instance_methods(int argc, Object **argv);

    Object *private_method_defined_qm(int argc, Object **argv);

    Object *protected_instance_methods(int argc, Object **argv);

    Object *protected_method_defined(int argc, Object **argv);

    Object *public_class_method(int argc, Object **argv);

    Object *public_constant(int argc, Object **argv);

    Object *public_instance_method(Object *sym);

    Object *public_instance_methods(int argc, Object **argv);

    Object *public_method_defined_qm(int argc, Object **argv);

    Object *refinements();

    Object *remove_class_variable(Object *sym);

    Object *remove_method(Object *sym);

    Object *set_temporary_name(Object *sym);

    Object *singleton_class_qm();

    Object *to_s();

    Object *undef_method(Object *sym);

    Object *undefined_instance_methods();
};
} // UltraRuby
} // Lang
} // Impl

#endif //ULTRA_RUBY_MODULEIMPL_H
