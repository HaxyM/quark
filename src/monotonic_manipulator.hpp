#ifndef QARK_MONOTONIC_MANIPULATOR
#define QARK_MONOTONIC_MANIPULATOR

#include <cstddef>
#include <new>
#include <utility>

#include "dynamic_memory_interface.hpp"

namespace quark
{
 class monotonic_manipulator : public dynamic_memory_interface
 {
  public:
  monotonic_manipulator() noexcept;
  monotonic_manipulator(const monotonic_manipulator& other) = delete;
  monotonic_manipulator(monotonic_manipulator&& other) noexcept;
  monotonic_manipulator& operator = (const monotonic_manipulator& other) = delete;
  monotonic_manipulator& operator = (monotonic_manipulator&& other) noexcept;
  ~monotonic_manipulator() noexcept;
  bool is_formated() const noexcept override;
  bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept override;
  void* allocate(std :: size_t size) const noexcept override;
  void* allocate(std :: size_t size, std :: align_val_t align) const noexcept override;
  bool deallocate(const void* ptr) const noexcept override;
  optional<bool> owns_pointer(const void* ptr) const noexcept override;
  protected:
  struct header;
  header* get_header() const noexcept;
  optional<std :: size_t> get_used_size() const noexcept;
  optional<std :: size_t> get_total_size() const noexcept;
  optional<std :: size_t> get_free_size() const noexcept;
  void* get_free_pool_begin() const noexcept;
  private:
  static optional<std :: size_t> min_size_for_header(void* data) const noexcept;
 };
}
#endif