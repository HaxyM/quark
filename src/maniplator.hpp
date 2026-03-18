#ifndef QUARK_MANIPLATOR
#define QUARK_MANIPLATOR

#include <type_traits>

#include "dynamic_memory_interface.hpp"

namespace quark
{
 template <class Manipulator>
 concept manipulator = std :: is_base_of_v<dynamic_memory_interface, Manipulator>
 && std :: is_convertible_v<const volatile Manipulator*, const volatile dynamic_memory_interface*>
 && std :: is_default_constructible_v<Manipulator>
 && (sizeof(Manipulator) == sizeof(dynamic_memory_interface))
 && (alignof(Manipulator) == alignof(dynamic_memory_interface));
}

#endif