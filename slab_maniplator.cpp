#include "slab_manipulator.hpp"

#include <cstdint>
#include <limits>
#include <utility> //CHECKME: Used for std :: exchange, check if freestanding implementation available (should be since C++23).

#include "helpers/optional.hpp"

//CHECKME: Add elements from <memory> header like std :: align, std :: destroy_at if possible.

 struct quark :: slab_manipulator :: chunk_header
 {
  chunk_header* next;
  chunk_header* prev;
  void* data;
  std :: size_t size;
  bool is_free;
  void* default_data() noexcept
 };

quark :: slab_manipulator :: slab_manipulator() noexcept
{
}

quark :: slab_manipulator :: slab_manipulator(slab_manipulator&& other) noexcept
: dynamic_memory_interface(std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {}))
{
}

quark :: slab_manipulator :: ~slab_manipulator() noexcept
{
 for (auto* current_chunk = get_first_chunk(); current_chunk != nullptr;)
 {
  auto* const next_chunk = current_chunk->next;
  current_chunk->~chunk_header();
  current_chunk = next_chunk;
 }
}

quark :: slab_manipulator& quark :: slab_manipulator :: operator = (slab_manipulator&& other) noexcept
{
 if (this != &other)
 {
  *static_cast<dynamic_memory_interface*>(this) = std :: exchange(*static_cast<dynamic_memory_interface*>(&other), {});
 }
 return *this;
}

bool quark :: slab_manipulator :: is_formated() const noexcept
{
 return (get_first_chunk() != nullptr);
}

bool quark :: slab_manipulator :: format_pool(void* mem_pool, std :: size_t pool_size) noexcept
{
 if (const auto min_size = min_size_for_chunk(mem_pool);
 min_size && (pool_size >= *min_size))
 {
  const auto shift = *min_size - sizeof(chunk_header);
  void* const aligned_address = reinterpret_cast<void*>(reinterpret_cast<char*>(mem_pool) + shift);
  const auto first_chunk = new (aligned_address) chunk_header;
  first_chunk->next = nullptr;
  first_chunk->prev = nullptr;
  first_chunk->data = first_chunk->default_data();
  first_chunk->size = pool_size - sizeof(chunk_header);
  first_chunk->is_free = true;
  data = first_chunk;
  return true;
 }
 return false;
}

void* quark :: slab_manipulator :: allocate(std :: size_t size) const noexcept
{
 return allocate(size, std :: align_val_t(alignof(std :: max_align_t)));
}

void* quark :: slab_manipulator :: allocate(std :: size_t size, std :: align_val_t align) const noexcept
{
 if (size > 0zu) //CHECKME: Is this solution valid with https://cplusplus.github.io/LWG/issue9? 
 {
  for (auto* current_chunk = get_first_chunk(); current_chunk != nullptr;)
  {
   if (const auto min_block_size = dynamic_memory_interface :: min_size_for(current_chunk->default_data(), size, align);
   min_block_size && current_chunk->is_free && (current_chunk->size >= *min_block_size))
   {
    void* const block_begin = current_chunk->default_data();
    const auto misalignment = *min_block_size - size;
    void* const data_begin = reinterpret_cast<void*>(reinterpret_cast<char*>(block_begin) + misalignment);
    void* const data_end = reinterpret_cast<void*>(reinterpret_cast<char*>(data_begin) + size);
    if (const auto min_size = min_size_for_chunk(data_end);
    min_size && (current_chunk->size > size + *min_size)) //Is chunk big enought to be split?
    {
     const auto chunk_shift = *min_size - sizeof(chunk_header);
     void* const new_chunk_address = reinterpret_cast<void*>(reinterpret_cast<char*>(data_begin) + size + chunk_shift);
     auto* const new_chunk = new (new_chunk_address) chunk_header;
     new_chunk->next = current_chunk->next;
     new_chunk->prev = current_chunk;
     new_chunk->data = data_begin;
     new_chunk->size = current_chunk->size - *min_block_size - *min_size;
     new_chunk->is_free = true;
     if (new_chunk->next != nullptr)
     {
      new_chunk->next->prev = new_chunk;
     }
     current_chunk->next = new_chunk;
     current_chunk->size = *min_block_size;
    }
    current_chunk->is_free = false;
    return data_begin;
   }
   current_chunk = current_chunk->next;
  }
 }
 return nullptr;
}

bool quark :: slab_manipulator :: deallocate(const void* ptr) const noexcept
{
 if (auto* const chunk = find_chunk_by_data_ptr(ptr); (chunk != nullptr) && (!chunk->is_free))
 {
  chunk->is_free = true;
  chunk->data = chunk->default_data();
  if ((chunk->next != nullptr) && chunk->next->is_free) //Merge with next chunk
  {
   auto* const next_chunk = chunk->next;
   chunk->size += sizeof(chunk_header) + next_chunk->size;
   chunk->next = next_chunk->next;
   if (chunk->next != nullptr)
   {
    chunk->next->prev = chunk;
   }
   next_chunk->~chunk_header();
  }
  if ((chunk->prev != nullptr) && chunk->prev->is_free) //Merge with previous chunk
  {
   auto* const prev_chunk = chunk->prev;
   prev_chunk->size += sizeof(chunk_header) + chunk->size;
   prev_chunk->next = chunk->next;
   if (prev_chunk->next != nullptr)
   {
    prev_chunk->next->prev = prev_chunk;
   }
   chunk->~chunk_header();
  }
  return true;
 }
 return false;
}

quark :: optional<bool> quark :: slab_manipulator :: owns_pointer(const void* ptr) const noexcept
{
 //return (find_chunk_by_data_ptr(ptr) != nullptr); //???
 if (const auto* const first_chunk = get_first_chunk(); (first_chunk != nullptr) || (ptr == nullptr))
 {
  const auto* const pool_begin = reinterpret_cast<const void*>(reinterpret_cast<const char*>(first_chunk) + sizeof(chunk_header));
  auto* current_chunk = first_chunk;
  for  (; current_chunk->next != nullptr; current_chunk = current_chunk->next);
  const auto* const pool_end = reinterpret_cast<const void*>(reinterpret_cast<const char*>(current_chunk) + sizeof(chunk_header) + current_chunk->size);  
  return {std :: in_place, (ptr >= pool_begin) && (ptr < pool_end)};
 }
 return quark :: nullopt;
}

quark :: slab_manipulator :: chunk_header* quark :: slab_manipulator :: get_first_chunk() const noexcept
{
 if (data != nullptr)
 {
  return std :: launder(reinterpret_cast<chunk_header*>(data));
 }  
 return nullptr;
}

const quark :: slab_manipulator :: chunk_header* quark :: slab_manipulator :: get_next_chunk(const chunk_header& chunk) noexcept
{
 return chunk.next;
}

const quark :: slab_manipulator :: chunk_header* quark :: slab_manipulator :: get_prev_chunk(const chunk_header& chunk) noexcept
{
 return chunk.prev;
}

void* quark :: slab_manipulator :: get_data_ptr(const chunk_header& chunk) noexcept
{
 return chunk.data;
}

std :: size_t quark :: slab_manipulator :: get_chunk_size(const chunk_header& chunk) noexcept
{
 return chunk.size;
}

bool quark :: slab_manipulator :: is_chunk_free(const chunk_header& chunk) noexcept
{
 return chunk.is_free;
}

quark :: slab_manipulator :: chunk_header* quark :: slab_manipulator :: find_chunk_by_data_ptr(const void* ptr) const noexcept
{
 if (ptr != nullptr)
 {
  for (auto* current_chunk = get_first_chunk(); current_chunk != nullptr; current_chunk = current_chunk->next)
  {
   auto* const data_begin = reinterpret_cast<char*>(current_chunk) + sizeof(chunk_header);
   auto* const data_end = data_begin + current_chunk->size;
   if (
    (reinterpret_cast<const char*>(ptr) >= data_begin) &&
    (reinterpret_cast<const char*>(ptr) < data_end)
   )
   {
    return current_chunk;
   }
  }
 }
 return nullptr;
}

inline quark :: optional<std :: size_t> quark :: slab_manipulator :: min_size_for_chunk(void* data) noexcept
{
 return dynamic_memory_interface :: min_size_for(data, sizeof(chunk_header), static_cast<std :: align_val_t>(alignof(chunk_header)));
}

inline void* quark :: slab_manipulator :: chunk_header :: default_data() noexcept
{
 return reinterpret_cast<void*>(reinterpret_cast<char*>(this) + sizeof(chunk_header));
}