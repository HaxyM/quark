#include "monotonic_manipulator.hpp"

#include <cstdckdint> //CHECKME: For std :: ckd_add, check if freestanding implementation available.
#include <cstdint>
#include <new>
#include <limits>
#include <utility> //CHECKME: Used for std :: exchange, check if freestanding implementation available (should be since C++23).

#include "helpers/optional.hpp"

struct quark :: monotonic_manipulator :: header
{
 std :: size_t free_size;
 void* free_pool_begin;
};

quark :: monotonic_manipulator :: monotonic_manipulator() noexcept
{
}

quark :: monotonic_manipulator :: monotonic_manipulator(monotonic_manipulator&& other) noexcept
: dynamic_memory_interface(std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {}))
{
}

quark :: monotonic_manipulator :: ~monotonic_manipulator() noexcept
{
 if (get_header() != nullptr)
 {
  get_header()->~header();
 }
}

quark :: monotonic_manipulator& quark :: monotonic_manipulator :: operator = (monotonic_manipulator&& other) noexcept
{
 if (this != &other)
 {
  *static_cast<dynamic_memory_interface*>(this) = std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {});
 }
 return *this;
}

bool quark :: monotonic_manipulator :: is_formated() const noexcept
{
 return (get_header() != nullptr);
}

bool quark :: monotonic_manipulator :: format_pool(void* mem_pool, std :: size_t pool_size) noexcept
{
 if (const auto min_size = min_size_for_header(mem_pool);
 min_size && (pool_size >= *min_size))
 {
  const auto shift = *min_size - sizeof(header);
  void* const aligned_address = reinterpret_cast<void*>(reinterpret_cast<char*>(mem_pool) + shift);
  const auto new_header = new (aligned_address) header;
  new_header->free_size = pool_size - *min_size;
  new_header->free_pool_begin = reinterpret_cast<void*>(reinterpret_cast<char*>(aligned_address) + sizeof(header));
  data = new_header;
  return true;
 }
 return false;
}

void* quark :: monotonic_manipulator :: allocate(std :: size_t size) const noexcept
{
 return allocate(size, std :: align_val_t{alignof(std :: max_align_t)});
}

void* quark :: monotonic_manipulator :: allocate(std :: size_t size, std :: align_val_t align) const noexcept
{
 if (auto* const pool_header = get_header(); pool_header == nullptr)
 {
  if (const auto min_size = dynamic_memory_interface :: min_size_for(get_free_pool_begin(), size, align);
  min_size && (get_free_size() >= *min_size))
  {
   const auto shift = *min_size - size;
   void* const allocated_ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(get_free_pool_begin()) + shift);
   pool_header->free_pool_begin = reinterpret_cast<void*>(reinterpret_cast<char*>(pool_header->free_pool_begin) + *min_size);
   pool_header->free_size -= *min_size;
   return allocated_ptr;
  }
 }
 return nullptr;
}

bool quark :: monotonic_manipulator :: deallocate(const void* ptr) const noexcept
{
 if (const auto owns = owns_pointer(ptr); !owns) [[unlikely]]
 {
  return false;
 }
 else return *owns;
}

quark :: optional<bool> quark :: monotonic_manipulator :: owns_pointer(const void* ptr) const noexcept
{
 if (const auto total_size = get_total_size(); !total_size || (total_size == 0zu)) [[unlikely]]
 {
  return quark :: nullopt;
 }
 else
 {
  const void* const pool_begin = reinterpret_cast<const void*>(reinterpret_cast<const char*>(get_header()) + sizeof(header));
  const void* const pool_end = reinterpret_cast<const void*>(reinterpret_cast<const char*>(pool_begin) + total_size);
  return {std :: in_place, (ptr >= pool_begin) && (ptr < pool_end)};
 }
}

quark :: monotonic_manipulator :: header* quark :: monotonic_manipulator :: get_header() const noexcept
{
 if (data != nullptr)
 {
  return std :: launder(reinterpret_cast<header*>(data));
 }  
 return nullptr;
}

quark :: optional<std :: size_t> quark :: monotonic_manipulator :: get_used_size() const noexcept
{
 if (const auto* const pool_header = get_header(); pool_header != nullptr)
 {
  const auto pool_begin = reinterpret_cast<std :: uintptr_t>(reinterpret_cast<const char*>(pool_header) + sizeof(header));
  const auto pool_end = reinterpret_cast<std :: uintptr_t>(reinterpret_cast<const char*>(pool_header->free_pool_begin) + pool_header->free_size);
  if (const auto used_size = (pool_end > pool_begin) ? (pool_end - pool_begin) : (pool_begin - pool_end);
  std :: in_range<std :: size_t>(used_size)) [[likely]]
  {
   return {std :: in_place, static_cast<std :: size_t>(used_size)};
  }
  return quark :: nullopt;
 }
 return quark :: nullopt;
}

quark :: optional<std :: size_t> quark :: monotonic_manipulator :: get_total_size() const noexcept
{
 if (const auto [used_size, used_ok] = get_used_size(); !used_ok) [[unlikely]]
 {
  return quark :: nullopt;
 }
 else if (const auto [free_size, free_ok] = get_free_size(); !free_ok) [[unlikely]]
 {
  return quark :: nullopt;
 }
 else if (std :: size_t total_size; std :: ckd_add(&total_size, used_size, free_size)) [[unlikely]]
 {
  return quark :: nullopt;
 }
 else return {std :: in_place, total_size};
}

quark :: optional<std :: size_t> quark :: monotonic_manipulator :: get_free_size() const noexcept
{
 if (const auto* const pool_header = get_header(); pool_header != nullptr)
 {
  return {std :: in_place, pool_header->free_size};
 }
 return quark :: nullopt;
}

inline quark :: optional<std :: size_t> quark :: slab_manipulator :: min_size_for_header(void* data) noexcept
{
 return dynamic_memory_interface :: min_size_for(data, sizeof(chunk_header), static_cast<std :: align_val_t>(alignof(chunk_header)));
}