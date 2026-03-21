#include "dynamic_memory_interface.hpp"

#include <cstdint>
#include <limits>
#include <utility> //CHECKME: Used for std :: exchange, check if freestanding implementation available (should be since C++23).

#include "helpers/optional.hpp"

quark :: dynamic_memory_interface :: dynamic_memory_interface() noexcept : data(nullptr)
{
}

quark :: dynamic_memory_interface :: dynamic_memory_interface(dynamic_memory_interface&& other) noexcept
: data(std :: exchange(other.data, nullptr))
{
}

quark :: dynamic_memory_interface :: ~dynamic_memory_interface() noexcept
{
}

quark :: dynamic_memory_interface& quark :: dynamic_memory_interface :: operator = (dynamic_memory_interface&& other) noexcept
{
 if (this != &other)
 {
  data = std :: exchange(other.data, nullptr);
 }
 return *this;
}

bool quark :: dynamic_memory_interface :: is_formated() const noexcept
{
 return (data != nullptr);
}

bool quark :: dynamic_memory_interface :: format_pool(void* mem_pool, std :: size_t pool_size) noexcept
{
 return false;
}

void* quark :: dynamic_memory_interface :: allocate(std :: size_t size) const noexcept
{
 return nullptr;
}

void* quark :: dynamic_memory_interface :: allocate(std :: size_t size, std :: align_val_t align) const noexcept
{
 return nullptr;
}

bool quark :: dynamic_memory_interface :: deallocate(const void* ptr) const noexcept
{
 return false;
}

quark :: optional<bool> quark :: dynamic_memory_interface :: owns_pointer(const void* ptr) const noexcept
{
 return quark :: optional<bool>{std :: in_place, false};
}

void* quark :: dynamic_memory_interface :: get_data() const noexcept
{
 return data;
}

quark :: optional<std :: size_t>
quark :: dynamic_memory_interface :: min_size_for(void* data, std :: size_t expected_size, std :: align_val_t expected_allign) noexcept
{
 if (expected_allign == std :: align_val_t(1)) //No alignment required
 {
  return quark :: optional<std :: size_t>{std :: in_place, expected_size};
 }  
 else if ((data != nullptr) && (expected_size > 0zu)) //Alignment required 
 {
  const auto data_address = reinterpret_cast<std :: uintptr_t>(data);
  const auto align = static_cast<std :: size_t>(expected_allign);
  const auto misalignment = data_address % align;
  const auto adjustment = (misalignment == 0zu) ? 0zu : static_cast<std :: size_t>(align - misalignment);
  if (expected_size <= (std :: numeric_limits <std :: size_t> :: max() - adjustment))
  {
   return quark :: optional<std :: size_t>{std :: in_place, expected_size + adjustment};
  }
 }
 return quark :: nullopt;
}
