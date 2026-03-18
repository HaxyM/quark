#ifndef QUARK_DYNAMIC_MEMORY_INTERFACE
#define QUARK_DYNAMIC_MEMORY_INTERFACE

#include <cstddef>
#include <utility> //CHECKME: Used for std :: exchange, check if freestanding implementation available (should be since C++23).

#include "helpers/optional.hpp"

namespace quark
{
 class dynamic_memory_interface
 {
  public:
  dynamic_memory_interface() noexcept;
  dynamic_memory_interface(const dynamic_memory_interface& other) = delete;
  dynamic_memory_interface(dynamic_memory_interface&& other) noexcept;
  dynamic_memory_interface& operator = (const dynamic_memory_interface& other) = delete;
  dynamic_memory_interface& operator = (dynamic_memory_interface&& other) noexcept;
  virtual ~dynamic_memory_interface() noexcept;
  virtual bool is_formated() const noexcept;
  virtual bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept;
  virtual void* allocate(std :: size_t size) const noexcept;
  virtual void* allocate(std :: size_t size, std :: align_val_t align) const noexcept;
  virtual bool deallocate(const void* ptr) const noexcept;
  virtual optional<bool> owns_pointer(const void* ptr) const noexcept;
  void* get_data() const noexcept;
  protected:
  void* data;
  static optional<std :: size_t> min_size_for(void* data, std :: size_t expected_size, std :: align_val_t expected_allign) noexcept;
 };
}
#endif
