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
  static bool is_formated() noexcept;
  static bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept;
  static void* allocate(std :: size_t size) noexcept;
  static void* allocate(std :: size_t size, std :: align_val_t align) noexcept;
  static bool deallocate(const void* ptr) noexcept;
  static optional<bool> owns_pointer(const void* ptr) noexcept;
  template <maniplator Manipulator> bool transmogrify_allocator() noexcept;
  private:
  static dynamic_memory_interface* get_allocator() noexcept;
  static bool is_maniplator_set;
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
  if constexpr (std :: is_nothrow_default_constructible_v<Manipulator>)
  {
   if (current_allocator->is_formated())
   {
    return false;
   }
   else
   {
    crrent_allocator->~dynamic_memory_interace();
    new (current_allocator) Manipulator();
    is_maniplator_set = true;
    return true;
   }
  }
  Manipulator manipulator;
  if (current_allocator->get_data() == manipulator.get_data())
  {
   return true;
  }
 }
#endif
