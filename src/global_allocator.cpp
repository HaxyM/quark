#include "dynamic_memory_interface.hpp"
#include "global_allocator.hpp"

#include <new>

alignas(quark :: dynamic_memory_interface)
	char allocator_buffer[sizeof(quark :: dynamic_memory_interface)];

alignas(bool) char is_maniplator_set_buffer[sizeof(bool)];

bool quark :: global_allocator :: init_globals() noexcept
{
 //None below should throw any exception.
 new (reinterpret_cast<void*>(allocator_buffer)) quarck :: dynamic_memory_interface();
 new (reinterpret_cast<void*>(is_maniplator_set_buffer)) bool(false);
 return true;
}

bool quark :: global_allocator :: is_formated() noexcept
{
 if (const auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return false;
 }
 else return current_allocator->is_formated();
}

bool quark :: global_allocator :: format_pool(void* mem_pool, std :: size_t pool_size) noexcept
{
 if (auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return false;
 }
 else return current_allocator->format_pool(mem_pool, pool_size);
}

void* quark :: global_allocator :: allocate(std :: size_t size) noexcept
{
 if (const auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return nullptr;
 }
 else return current_allocator->allocate(size);
}

void* quark :: global_allocator :: allocate(std :: size_t size,
		std :: align_val_t align) noexcept
{
 if (const auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return nullptr;
 }
 else return current_allocator->allocate(size, align);
}

bool quark :: global_allocator :: deallocate(const void* ptr) noexcept
{
 if (const auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return false;
 }
 else return current_allocator->deallocate(ptr);
}

quark :: optional<bool> quark :: global_allocator :: owns_pointer(const void* ptr) noexcept
{
 if (const auto* const current_allocator = get_allocator(); current_allocator == nullptr)
 {
  return {};
 }
 else return current_allocator->owns_pointer(ptr);
}

quark :: dynamic_memory_interface* quark :: global_allocator :: get_allocator() noexcept
{
 return std :: launder(reinterpret_cast<quark :: dynamic_memory_interface*>(allocator_buffer));
}

bool& quark :: global_allocator :: get_is_maniplator_set() noexcept
{
 return *std :: launder(reinterpret_cast<bool*>(is_maniplator_set_buffer));
}

