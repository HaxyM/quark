#ifndef QUARK_NULL_MANIPULATOR
#define QUARK_NULL_MANIPULATOR

#include "dynamic_memory_interface.hpp"
#include "helpers/optional.hpp"

#include <cstddef>
#include <new>
#include <utility>

namespace quark
{
 class null_manipulator : public dynamic_memory_interface
 {
  public:
  null_manipulator() noexcept;
  null_manipulator(const null_manipulator& other) = delete;
  null_manipulator(null_manipulator&& other) noexcept;
  null_manipulator& operator = (const null_manipulator& other) = delete;
  null_manipulator& operator = (null_manipulator&& other) noexcept;
  ~null_manipulator() noexcept;
  bool is_formated() const noexcept override;
  bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept override;
  void* allocate(std :: size_t size) const noexcept override;
  void* allocate(std :: size_t size, std :: align_val_t align) const noexcept override;
  bool deallocate(const void* ptr) const noexcept override;
  optional<bool> owns_pointer(const void* ptr) const noexcept override;
 };
}

#endif