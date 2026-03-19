#ifndef QUARK_GLOBAL_ALLOCATOR
#define QUARK_GLOBAL_ALLOCATOR

#include "dynamic_memory_interface.hpp"
#include "helpers/optional.hpp"
#include "maniplator.hpp"

#include <concepts>
#include <cstddef>
#include <new>

namespace quark
{
 class global_allocator
 {
  public:
  static bool init_globals() noexcept;
  static bool is_formated() noexcept;
  static bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept;
  static void* allocate(std :: size_t size) noexcept;
  static void* allocate(std :: size_t size, std :: align_val_t align) noexcept;
  static bool deallocate(const void* ptr) noexcept;
  static optional<bool> owns_pointer(const void* ptr) noexcept;
  template <maniplator Manipulator> bool transmogrify_allocator() noexcept;
  private:
  static dynamic_memory_interface* get_allocator() noexcept;
  static bool& get_is_maniplator_set() noexcept;
 };
}

template <quark :: manipulator Manipulator>
bool quark :: global_allocator :: transmogrify_allocator() noexcept
{
 if (auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return false;
 }
 else
 {
  if (current_allocator->is_formated())
  {
   return false;
  }
  else
  {
   if constexpr (std :: is_nothrow_default_constructible_v<manipulator>)
   {
    current_allocator->~dynamic_memory_interace();
    new (current_allocator) Manipulator();
    get_is_maniplator_set() = true;
    return true;
   }
   else if constexpr (std :: is_move_constructible_v<Manipulator> &&
		   std :: is_nothrow_move_constructible_v<manipulator>)
   {
    try
    {
     Manipulator manipulator;
     current_allocator->~dynamic_memory_interace();
     new (current_allocator) Manipulator(std :: move(manipulator));
    }
    catch (...)
    {
     return false;
    }
    get_is_maniplator_set() = true;
    return true;
   }
   else return false;
  }
 }
#endif
