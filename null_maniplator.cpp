#include "null_maniplator.hpp"

#include <utility> //CHECKME: Used for std :: exchange, check if freestanding implementation available (should be since C++23).

#include "helpers/optional.hpp"

quark :: null_manipulator :: null_manipulator() noexcept
{
}

quark :: null_manipulator :: null_manipulator(null_manipulator&& other) noexcept
: dynamic_memory_interface(std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {}))
{
}

quark :: null_manipulator :: ~null_manipulator() noexcept
{
}

quark :: null_manipulator& quark :: null_manipulator :: operator = (null_manipulator&& other) noexcept
{
 if (this != &other)
 {
  *static_cast<dynamic_memory_interface*>(this) = std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {});
 }
 return *this;
}

bool quark :: null_manipulator :: is_formated() const noexcept
{
 return data != nullptr;
}

bool quark :: null_manipulator :: format_pool(void* mem_pool, std :: size_t pool_size) noexcept
{
 if (mem_pool != nullptr)
 {
  data = mem_pool;
  return true;
 }
 return false;
}

void* quark :: null_manipulator :: allocate(std :: size_t) const noexcept
{
 return nullptr;
}

void* quark :: null_manipulator :: allocate(std :: size_t, std :: align_val_t) const noexcept
{
 return nullptr;
}

bool quark :: null_manipulator :: deallocate(const void* ptr) const noexcept
{
 return (ptr == nullptr);
}

quark :: optional<bool> quark :: null_manipulator :: owns_pointer(const void* ptr) const noexcept
{
 return {std :: in_place, (ptr == nullptr)};
}