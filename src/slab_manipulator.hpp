#ifndef QARK_SLAB_MANIPULATOR
#define QARK_SLAB_MANIPULATOR

#include <cstddef>
#include <new>
#include <utility>

#include "dynamic_memory_interface.hpp"
#include "helpers/optional.hpp"

namespace quark
{
 class slab_manipulator : public dynamic_memory_interface
 {
  public:
  slab_manipulator() noexcept;
  slab_manipulator(const slab_manipulator& other) = delete;
  slab_manipulator(slab_manipulator&& other) noexcept;
  slab_manipulator& operator = (const slab_manipulator& other) = delete;
  slab_manipulator& operator = (slab_manipulator&& other) noexcept;
  ~slab_manipulator() noexcept;
  bool is_formated() const noexcept override;
  bool format_pool(void* mem_pool, std :: size_t pool_size) noexcept override;
  void* allocate(std :: size_t size) const noexcept override;
  void* allocate(std :: size_t size, std :: align_val_t align) const noexcept override;
  bool deallocate(const void* ptr) const noexcept override;
  optional<bool> owns_pointer(const void* ptr) const noexcept override;
  protected:
  struct chunk_header;
  chunk_header* get_first_chunk() const noexcept;
  static const chunk_header* get_next_chunk(const chunk_header& chunk) noexcept;
  static const chunk_header* get_prev_chunk(const chunk_header& chunk) noexcept;
  static void* get_data_ptr(const chunk_header& chunk) noexcept;
  static std :: size_t get_chunk_size(const chunk_header& chunk) noexcept;
  static bool is_chunk_free(const chunk_header& chunk) noexcept;
  chunk_header* find_chunk_by_data_ptr(const void* ptr) const noexcept;
  private:
  static optional<std :: size_t> min_size_for_chunk(void* data) noexcept;
 };
}
#endif