#include <vector>
#include "lang/Object.h"
#include "lang/Heap.h"
#include "lang/BasicClasses.h"
#include "lang/Symbol.h"
#include "lang/Class.h"
#include "lang/String.h"

namespace UltraRuby {
namespace Lang {
Class *Color;

Symbol *Kernel_attr_accessor;
Symbol *red, *green, *blue, *alpha;

Object *Color_initialize(Object *self, std::vector<Object *> args) {
    self->callV(Symbol::get("set"), std::move(args));
}

Object *Color_self__load_lambda0(Object *self, Object *arg0) {

}

Object *Color_self__load(Class *self, Object *arg0) {
    Object *tmp0 = arg0->call1(Symbol::get("unpack"), String::get("D*"));
    Object *lambda0 = nullptr; //todo instance lambda
    Object *tmp1 = tmp0->call1(Symbol::get("map"), lambda0);
    return Color->call1(Symbol::get("new"), tmp1);
}

Object *Color_class_def(Class *self) {
//    self->callV(Kernel_attr_accessor, std::vector<Object *>{red, green, blue, alpha});
    self->defineInstanceMethod(Symbol::get("initialize"), 4, 4, reinterpret_cast<void *>(Color_initialize));
    self->defineSingletonMethod(Symbol::get("_load"), 1, 1, reinterpret_cast<void *>(Color_self__load));
    self->defineInstanceMethod(Symbol::get("_dump"), 1, 1, nullptr);
    self->defineInstanceMethod(Symbol::get("set"), -1, -1, nullptr);
    self->defineInstanceMethod(Symbol::get("gl_color_vf4"), 0, 0, nullptr);
    return self->defineInstanceMethod(Symbol::get("gl_color_vb4"), 0, 0, nullptr);
}

Object *Color_self_instance(Object *self);

Object *top_level_0(Object *self) {
    Class::defineClass(Symbol::get("Color"), &Color, UltraRuby::Lang::BasicClasses::ObjectClass,
                       Color_class_def);
    return self->defineClassInstance(Color_self_instance);
}
}
}