#ifndef QUARK_HELPERS_OPTIONAL
#define QUARK_HELPERS_OPTIONAL

#include <concepts>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <new>
#include <type_traits>
#include <utility>

namespace quark
{
 struct nullopt_t
 {
 } nullopt;

 template <class Type>
 class optional
  requires (!std :: is_reference_v<Type>) && (!std :: is_void_v<Type>) && std :: destructible<Type>
 {
  private:
  constexpr const static bool is_optional_of_bool = std :: is_same_v<std :: remove_cv_t<Type>, bool>;
  template <class OtherType>
  constexpr const static bool is_optional_lvalref_constructible;
  template <class OtherType>
  constexpr const static bool is_optional_rvalref_constructible;
  template <class OtherType>
  constexpr const static bool is_generic_concept_constructible;
  template <class OtherType>
  constexpr const static bool is_optional_v;
  template <class OtherType> struct is_optional : std :: false_type {};
  template <class OtherType> struct is_optional<optional<OtherType> > : std :: true_type {};
  constexpr static bool is_nothrow_copy_assignable() noexcept;
  constexpr static bool is_nothrow_move_assignable() noexcept;
  template <class OtherType>
  constexpr const static bool is_optional_lvalref_assignable;
  template <class OtherType>
  constexpr const static bool is_optional_rvalref_assignable;
  template <class OtherType>
  constexpr const static bool is_generic_concept_assignable;
  template <class Self, class Functor>
  constexpr static bool is_and_then_functor() noexcept;
  template <class Self, class Functor>
  constexpr static bool is_transform_functor() noexcept;
  template <class Self, class Functor>
  constexpr static bool is_or_else_functor() noexcept;
  public:
  // Nested types
  using vale_type = Type;
  using iterator = Type*;
  using const_iterator = const Type*;
  // Constructors
  constexpr optional() noexcept;
  constexpr optional(nullopt_t) noexcept;
  constexpr optional(const optional& other)
  noexcept(std :: is_nothrow_copy_constructible_v<Type>) requires std :: copy_constructible<Type>;
  constexpr optional(optional&& other)
  noexcept(std :: is_nothrow_move_constructible_v<Type>) requires std :: move_constructible<Type>;
  template <class OtherType>
  constexpr explicit(!std :: is_convertible_v<const OtherType&, Type>) optional(const optional<OtherType>& other)
  noexcept (std :: is_nothrow_constructible_v<Type, const OtherType&>) requires is_optional_lvalref_constructible<OtherType>;
  template <class OtherType>
  constexpr explicit(!std :: is_convertible_v<OtherType, Type>) optional(optional<OtherType>&& other)
  noexcept (std :: is_nothrow_constructible_v<Type, OtherType&&>) requires is_optional_rvalref_constructible<OtherType>;
  template <class ... Args>
  constexpr explicit optional(std :: in_place_t, Args&& ... args)
  noexcept(std :: is_nothrow_constructible_v<Type, Args&&...>) requires std :: constructible_from<Type, Args&&...>;
  template <class U, class ... Args>
  constexpr explicit optional(std :: in_place_t, std :: initializer_list<U> il, Args&& ... args)
  noexcept(std :: is_nothrow_constructible_v<Type, std :: initializer_list<U>&, Args&&...>)
  requires std :: constructible_from<Type, std :: initializer_list<U>&, Args&&...>;
  template <class OtherType = std :: remove_cv_t<Type> >
  constexpr explicit(!std :: is_convertible_v<OtherType, Type>) optional(OtherType&& value)
  noexcept(std :: is_nothrow_constructible_v<Type, OtherType>)
  requires is_generic_concept_constructible<OtherType>;  
  // Destructor
  constexpr ~optional() noexcept(std :: is_nothrow_destructible_v<Type>);
  // Assignment operators
  constexpr optional& operator = (nullopt_t) noexcept(std :: is_nothrow_destructible_v<Type>);
  constexpr optional& operator = (const optional& other) noexcept(is_nothrow_copy_assignable())
  requires (std :: is_copy_assignable_v<Type> || std :: is_copy_constructible_v<Type>);
  constexpr optional& operator = (optional&& other) noexcept(is_nothrow_move_assignable())
  requires (std :: is_move_assignable_v<Type> || std :: is_move_constructible);
  template <class OtherType>
  constexpr optional& operator = (const optional<OtherType>& other)
  noexcept (std :: is_nothrow_constructible_v<Type, const OtherType&> && is_nothrow_assignable_v<Type&, const OtherType&>)
  requires is_optional_lvalref_assignable<OtherType>;
  template <class OtherType>
  constexpr optional& operator = (optional<OtherType>&& other)
  noexcept (std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, OtherType> && is_nothrow_assignable_v<Type&, OtherType>)
  requires is_optional_rvalref_assignable<OtherType>;
  template <class OtherType = std :: remove_cv_t<Type> >
  constexpr optional& operator = (OtherType&& other)
  noexcept (std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, OtherType> && is_nothrow_assignable_v<Type&, OtherType>)
  requires is_generic_concept_assignable<OtherType>;
  // Iterators
  constexpr iterator begin() noexcept;
  constexpr const_iterator begin() const noexcept;
  constexpr iterator end() noexcept;
  constexpr const_iterator end() const noexcept;
  // Observers
  constexpr const Type* operator -> () const noexcept;
  constexpr Type* operator -> () noexcept;
  constexpr auto&& operator * (this auto&& self) noexcept;
  constexpr explicit operator bool () const noexcept;
  constexpr bool has_value() const noexcept;
  constexpr auto&& value(this auto&& self);
  template <class OtherType = std :: remove_cv_t<Type> >
  constexpr Type value_or(OtherType&& default_value) const &
  noexcept(std :: is_nothrow_copy_constructible_v<Type> && std :: is_nothrow_convertible_v<OtherType&&, Type>)
  requires std :: copy_constructible<Type> && std :: convertible_to<OtherType&&, Type>;
  template <class OtherType = std :: remove_cv_t<Type> >
  constexpr Type value_or(OtherType&& default_value) &&
  noexcept(std :: is_nothrow_move_constructible_v<Type> && std :: is_nothrow_convertible_v<OtherType&&, Type>)
  requires std :: move_constructible<Type> && std :: convertible_to<OtherType&&, Type>;
  // Monadic operations
  template <class Self, class Functor>
  constexpr auto and_then(this Self&& self, Functor&& func)
  noexcept (std :: is_nothrow_invocable_v<Functor, decltype(std :: forward_like<Self>(std :: declval<Type>()))>)
  requires is_and_then_functor<Self, Functor>();
  template <class Self, class Functor>
  constexpr auto transform(this Self&& self, Functor&& func)
  noexcept (std :: is_nothrow_invocable_v<Functor, decltype(std :: forward_like<Self>(std :: declval<Type>()))>)
  requires is_transform_functor<Self, Functor>();
  template <class Self, class Functor>
  constexpr auto or_else(this Self&& self, Functor&& func)
  noexcept (noexcept(std :: forward_like<Self>(std :: declval<Type>())) && std :: is_nothrow_invocable_v<Functor>)
  requires is_or_else_functor<Self, Functor>();
  // Modifiers
  constexpr void swap(optional& other)
  noexcept(std :: is_nothrow_destructible_v<Type> && is_nothrow_swapable_v<Type> && std :: is_nothrow_move_constructible_v<Type>)
  requires std :: is_swappable_v<Type> && std :: is_move_constructible_v<Type>;
  constexpr void reset() noexcept(std :: is_nothrow_destructible_v<Type>);
  template <class ... Args>
  constexpr Type& emplace(Args&& ... args)
  noexcept(std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, Args&&...>)
  requires std :: constructible_from<Type, Args&&...>;
  template <class OtherType, class ... Args>
  constexpr Type& emplace(std :: initializer_list<OtherType> ilist, Args&& ... args)
  noexcept(std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, std :: initializer_list<OtherType>&, Args&&...>)
  requires std :: constructible_from<Type, std :: initializer_list<OtherType>, Args&&...>;
  private:
  Type* stored;
  alignas(Type) std :: byte storage[sizeof(Type)];
 };

 class bad_optional_access : public std :: exception
 {
  public:
  const char* what() const noexcept override
  {
   return "bad optional access";
  }
 };
}

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_optional_lvalref_constructible
= std :: is_constructible_v<Type, const OtherType&>
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, quark :: optional<OtherType>&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, const quark :: optional<OtherType>&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, quark :: optional<OtherType>&&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, const quark :: optional<OtherType>&&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<quark :: optional<OtherType>&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<const quark :: optional<OtherType>&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<quark :: optional<OtherType>&&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<const quark :: optional<OtherType>&&, Type>);

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_optional_rvalref_constructible
= std :: is_constructible_v<Type, OtherType>
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, quark :: optional<OtherType>&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, const quark :: optional<OtherType>&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, quark :: optional<OtherType>&&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_constructible_v<Type, const quark :: optional<OtherType>&&>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<quark :: optional<OtherType>&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<const quark :: optional<OtherType>&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<quark :: optional<OtherType>&&, Type>)
&& (qark :: optional <Type> :: is_optional_of_bool || !std :: is_convertible_v<const quark :: optional<OtherType>&&, Type>);

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_generic_concept_constructible
= std :: is_constructible_v<Type, OtherType>
&& !std :: is_same_v<std :: remove_cvref_t<OtherType>, std :: in_place_t>
&& !std :: is_same_v<std :: remove_cvref_t<OtherType>, quark :: optional<Type> >
&& !(std :: is_same_v<std :: remove_cv_t<OtherType>, bool> && qark :: optional <Type> :: is_optional_v<std :: remove_cvref_t<OtherType> >);

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_optional_v
= quark :: optional <Type> :: is_optional<OtherType> :: value;

template <class Type>
inline constexpr bool quark :: optional <Type> :: is_nothrow_copy_assignable() noexcept
{
 if constexpr (std :: is_copy_assignable_v<Type>)
 {
  return std :: is_nothrow_copy_assignable_v<Type> && std :: is_nothrow_destructible_v<Type>;
 }
 else if constexpr (std :: is_copy_constructible_v<Type>)
 {
  return std :: is_nothrow_copy_constructible_v<Type> && std :: is_nothrow_destructible_v<Type>;
 }
 else return {};
}

template <class Type>
inline constexpr bool quark :: optional <Type> :: is_nothrow_move_assignable() noexcept
{
 if constexpr (std :: is_move_assignable_v<Type>)
 {
  return std :: is_nothrow_move_assignable_v<Type> && std :: is_nothrow_destructible_v<Type>;
 }
 else if constexpr (std :: is_move_constructible_v<Type>)
 {
  return std :: is_nothrow_move_constructible_v<Type> && std :: is_nothrow_destructible_v<Type>;
 }
 else return {};
}

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_optional_lvalref_assignable
= std :: is_constructible_v<Type, const OtherType&> && std :: is_assignable_v<Type&, const OtherType&>
&& !std :: is_constructible_v<Type, quark :: optional<OtherType>&>
&& !std :: is_constructible_v<Type, const quark :: optional<OtherType>&>
&& !std :: is_constructible_v<Type, quark :: optional<OtherType>&&>
&& !std :: is_constructible_v<Type, const quark :: optional<OtherType>&&>
&& !std :: is_convertible_v<quark :: optional<OtherType>&, Type>
&& !std :: is_convertible_v<const quark :: optional<OtherType>&, Type>
&& !std :: is_convertible_v<quark :: optional<OtherType>&&, Type>
&& !std :: is_convertible_v<const quark :: optional<OtherType>&&, Type>
&& !std :: is_assignable_v<Type&, quark :: optional<OtherType>&>
&& !std :: is_assignable_v<Type&, const quark :: optional<OtherType>&>
&& !std :: is_assignable_v<Type&, quark :: optional<OtherType>&&>
&& !std :: is_assignable_v<Type&, const quark :: optional<OtherType>&&>;

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_optional_rvalref_assignable
= std :: is_constructible_v<Type, OtherType> && std :: is_assignable_v<Type&, OtherType>
&& !std :: is_constructible_v<Type, quark :: optional<OtherType>&>
&& !std :: is_constructible_v<Type, const quark :: optional<OtherType>&>
&& !std :: is_constructible_v<Type, quark :: optional<OtherType>&&>
&& !std :: is_constructible_v<Type, const quark :: optional<OtherType>&&>
&& !std :: is_convertible_v<quark :: optional<OtherType>&, Type>
&& !std :: is_convertible_v<const quark :: optional<OtherType>&, Type>
&& !std :: is_convertible_v<quark :: optional<OtherType>&&, Type>
&& !std :: is_convertible_v<const quark :: optional<OtherType>&&, Type>
&& !std :: is_assignable_v<Type&, quark :: optional<OtherType>&>
&& !std :: is_assignable_v<Type&, const quark :: optional<OtherType>&>
&& !std :: is_assignable_v<Type&, quark :: optional<OtherType>&&>
&& !std :: is_assignable_v<Type&, const quark :: optional<OtherType>&&>;

template <class Type>
template <class OtherType>
inline constexpr const bool quark :: optional <Type> :: is_generic_concept_assignable
= std :: is_constructible_v<Type, OtherType> && std :: is_assignable_v<Type&, OtherType>
&& !std :: is_same_v<std :: remove_cvref_t<OtherType>, quark :: optional<Type> >
&& !(std :: is_scalar_v<Type> && std :: is_same_v<std :: decay_t<OtherType>, Type>);

template <class Type>
template <class Self, class Functor>
inline constexpr bool quark :: optional <Type> :: is_and_then_functor() noexcept
{
 using arg_type = decltype(std :: forward_like<Self>(std :: declval<Type>()));
 if constexpr (!std :: is_invocable_v<Functor, arg_type>)
 {
  return false;
 }
 else
 { 
  using return_type = std :: invoke_result_t<Functor, arg_type>;
  return quark :: optional <Type> :: is_optional<std :: remove_cvref_t<return_type> > :: value;
 }
}

template <class Type>
template <class Self, class Functor>
inline constexpr bool quark :: optional <Type> :: is_transform_functor() noexcept
{
 using arg_type = decltype(std :: forward_like<Self>(std :: declval<Type>()));
 if constexpr (!std :: is_invocable_v<Functor, arg_type>)
 {
  return false;
 }
 else
 { 
  using return_type = std :: invoke_result_t<Functor, arg_type>;
  return !(std :: is_same_v<std :: remove_cvref_t<return_type> >, std :: in_place_t>
  || std :: is_same_v<std :: remove_cvref_t<return_type> >, quark :: nullopt_t>);
 }
}

template <class Type>
template <class Self, class Functor>
inline constexpr bool quark :: optional <Type> :: is_or_else_functor() noexcept
{
 if constexpr (!std :: is_invocable_v<Functor>)
 {
  return false;
 }
 else
 {
  using return_type = std :: rmove_cvref_t<std :: invoke_result_t<Functor> >;
  return std :: is_same_v<quark :: optional<Type>, return_type>;
 }
}

// Constructors
template <class Type> constexpr inline quark :: optional <Type> :: optional() noexcept
: stored(nullptr)
{
}

template <class Type> constexpr inline quark :: optional <Type> :: optional(quark :: nullopt_t) noexcept
: stored(nullptr)
{
}

template <class Type> constexpr inline quark :: optional <Type> :: optional(const optional& other)
noexcept(std :: is_nothrow_copy_constructible_v<Type>)
requires std :: copy_constructible<Type>
{
 stored = ((other.stored != nullptr) ? new (storage) Type(*(other.stored)) : nullptr);
}

template <class Type> constexpr inline quark :: optional <Type> :: optional(optional&& other)
noexcept(std :: is_nothrow_move_constructible_v<Type>)
requires std :: move_constructible<Type>
{
 stored = ((other.stored != nullptr) ? new (storage) Type(std :: move(*(other.stored))) : nullptr);
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional(const optional<OtherType>& other)
noexcept (std :: is_nothrow_constructible_v<Type, const OtherType&>)
requires is_optional_lvalref_constrctible<OtherType>
{
 stored = ((other.stored != nullptr) ? new (storage) Type(*(other.sored)) : nullptr);
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional(optional<OtherType>&& other)
noexcept (std :: is_nothrow_constructible_v<Type, OtherType&&>)
requires is_optional_rvalref_constructible<OtherType>
{
 stored = ((other.stored != nullptr)
 ? new (storage) Type(std :: forward<decltype(*std :: forward<OtherType>(other.stored))>(*std :: forward<OtherType>(other.stored)))
 : nullptr);
}

template <class Type>
template <class ... Args>
inline constexpr quark :: optional <Type> :: optional(std :: in_place_t, Args&& ... args)
noexcept(std :: is_nothrow_constructible_v<Type, Args&&...>)
requires std :: constructible_from<Type, Args&&...>
{
 stored = new (storage) Type(std :: forward<Args>(args)...);
}

template <class Type>
template <class U, class ... Args>
inline constexpr quark :: optional <Type> :: optional(std :: in_place_t, std :: initializer_list<U> il, Args&& ... args)
noexcept(std :: is_nothrow_constructible_v<Type, std :: initializer_list<U>&, Args&&...>)
requires std :: constructible_from<Type, std :: initializer_list<U>&, Args&&...>
{
 stored = new (storage) Type(il, std :: forward<Args>(args)...);
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional(OtherType&& value)
noexcept(std :: is_nothrow_constructible_v<Type, OtherType>)
requires is_generic_concept_constructible<OtherType>
{
 stored = new (storage) Type(std :: forward<OtherType>(value));
}

// Destructor
template <class Type> inline constexpr quark :: optional <Type> :: ~optional() noexcept(std :: is_nothrow_destructible_v<Type>)
{
 if constexpr (!std :: is_trivially_destructible_v<Type>)
 {
  reset();
 }
}

// Assignment operators
template <class Type>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (quark :: nullopt_t)
noexcept(std :: is_nothrow_destructible_v<Type>)
{
 reset();
 return *this;
}

template <class Type>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (const optional& other)
noexcept(is_nothrow_copy_assignable())
requires (std :: is_copy_assignable_v<Type> || std :: is_copy_constructible_v<Type>)
{//FIXME: Fix assigns!
 if (this != &other)
 {
  if (other.stored == nullptr)
  {
   reset();
  }
  else if constexpr (std :: is_copy_assignable_v<Type>)
  {
   *stored = *(other.stored);
  }
  else if constexpr (std :: is_copy_constructible_v<Type>)
  {
   reset();
   stored = new (storage) Type(*(other.stored));
  }
  else
  {
   static_assert(std :: is_copy_assignable_v<Type> || std :: is_copy_constructible_v<Type>,
   "Type must be copy assignable or copy constructible");
  }
 }
 return *this;
}

template <class Type>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (optional&& other)
noexcept(is_nothrow_copy_assignable())
{//FIXME: Fix assigns!
 if (this != &other)
 {
  if (other.stored == nullptr)
  {
   reset();
  }
  else if constexpr (std :: is_move_assignable_v<Type>)
  {
   *stored = std :: move(*(other.stored));
  }
  else if constexpr (std :: is_move_constructible_v<Type>)
  {
   reset();
   stored = new (storage) Type(std :: move(*(other.stored)));
  }
  else
  {
   static_assert(std :: is_move_assignable_v<Type> || std :: is_move_constructible_v<Type>,
   "Type must be move assignable or move constructible");
  }
 }
 return *this;
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (const quark :: optional<OtherType>& other)
noexcept (std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, const OtherType&> && is_nothrow_assignable_v<Type&, const OtherType&>)
requires is_optional_lvalref_assignable<OtherType>
{
 if (other.stored == nullptr)
 {
  reset();
 }
 else if (stored != nullptr)
 {
  *stored = *(other.stored);
 }
 else
 {
  stored = new (storage) Type(*(other.stored));
 }
 return *this;
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (quark :: optional<OtherType>&& other)
noexcept (std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, OtherType> && is_nothrow_assignable_v<Type&, OtherType>)
requires is_optional_rvalref_assignable<OtherType>
{
 if (other.stored == nullptr)
 {
  reset();
 }
 else if (stored != nullptr)
 {
  *stored = std :: forward<decltype(*std :: forward<OtherType>(other.stored))>(*std :: forward<OtherType>(other.stored));
 }
 else
 {
  stored = new (storage) Type(std :: forward<decltype(*std :: forward<OtherType>(other.stored))>(*std :: forward<OtherType>(other.stored)));
 }
 return *this;
}

template <class Type>
template <class OtherType>
inline constexpr quark :: optional <Type> :: optional& quark :: optional <Type> :: operator = (OtherType&& other)
noexcept (std :: is_nothrow_constructible_v<Type, OtherType> && is_nothrow_assignable_v<Type&, OtherType>)
requires is_generic_concept_assignable<OtherType>
{
 if (stored != nullptr)
 {
  *stored = std :: forward<OtherType>(other);
 }
 else
 {
  stored = new (storage) Type(std :: forward<OtherType>(other));
 }
 return *this;
}

// Iterators
template <class Type>
inline constexpr typename quark :: optional <Type> :: iterator quark :: optional <Type> :: begin() noexcept
{
 return stored;
}

template <class Type>
inline constexpr typename quark :: optional <Type> :: const_iterator quark :: optional <Type> :: begin() const noexcept
{
 return stored;
}

template <class Type>
inline constexpr typename quark :: optional <Type> :: iterator quark :: optional <Type> :: end() noexcept
{
 return (stored == nullptr) ? nullptr : (stored + 1zu);
}

template <class Type>
inline constexpr typename quark :: optional <Type> :: const_iterator quark :: optional <Type> :: end() const noexcept
{
 return (stored == nullptr) ? nullptr : (stored + 1zu);
}

// Observers
template <class Type>
inline constexpr const Type* quark :: optional <Type> :: operator -> () const noexcept
{
 return stored;
}

template <class Type>
inline constexpr Type* quark :: optional <Type> :: operator -> () noexcept
{
 return stored;
}

template <class Type>
inline constexpr auto&& quark :: optional <Type> :: operator * (this auto&& self) noexcept
{
 return std :: forward_like<decltype(self)>(*(self.stored));
}

template <class Type>
inline constexpr quark :: optional <Type> :: operator bool () const noexcept
{
 return (stored != nullptr);
}

template <class Type>
inline constexpr bool quark :: optional <Type> :: has_value() const noexcept
{
 return (stored != nullptr);
}

template <class Type>
inline constexpr auto&& quark :: optional <Type> :: value(this auto&& self)
{
 if (self.stored == nullptr)
 {
  throw quark :: bad_optional_access();
 }
 return std :: forward_like<decltype(self)>(*(self.stored));
}

template <class Type>
template <class OtherType>
inline constexpr Type quark :: optional <Type> :: value_or(OtherType&& default_value) const &
noexcept(std :: is_nothrow_copy_constructible_v<Type> && std :: is_nothrow_convertible_v<OtherType&&, Type>)
requires std :: copy_constructible<Type> && std :: convertible_to<OtherType&&, Type>
{
 if (stored != nullptr)
 {
  return *stored;
 }
 else
 {
  return static_cast<Type>(std :: forward<OtherType>(default_value));
 }
}

template <class Type>
template <class OtherType>
inline constexpr Type quark :: optional <Type> :: value_or(OtherType&& default_value) &&
noexcept(std :: is_nothrow_move_constructible_v<Type> && std :: is_nothrow_convertible_v<OtherType&&, Type>)
requires std :: move_constructible<Type> && std :: convertible_to<OtherType&&, Type>
{
 if (stored != nullptr)
 {
  return std :: move(*stored);
 }
 else
 {
  return static_cast<Type>(std :: forward<OtherType>(default_value));
 }
}

template <class Type>
template <class Self, class Functor>
inline constexpr auto quark :: optional <Type> :: and_then(this Self&& self, Functor&& func)
noexcept (std :: is_nothrow_invocable_v<Functor, decltype(std :: forward_like<Self>(std :: declval<Type>()))>)
requires quark :: optional <Type> :: is_and_then_functor<Self, Functor>()
{
 if (stored != nullptr)
 {
  return std :: invoke(std :: forward<Functor>(func), *std :: forward_like<Self>(stored));
 }
 else
 {
  using arg_type = decltype(std :: forward_like<Self>(std :: declval<Type>()));
  using return_type = std :: remove_cvref_t<std :: invoke_result_t<Functor, arg_type> >;
  return return_type{quark :: nullopt};
 }
}

template <class Type>
template <class Self, class Functor>
inline constexpr auto quark :: optional <Type> :: transform(this Self&& self, Functor&& func)
noexcept (std :: is_nothrow_invocable_v<Functor, decltype(std :: forward_like<Self>(std :: declval<Type>()))>)
requires quark :: optional <Type> :: is_transform_functor<Self, Functor>()
{
 using arg_type = decltype(std :: forward_like<Self>(std :: declval<Type>()));
 using return_type = std :: remove_cvref_t<std :: invoke_result_t<Functor, arg_type> >;
 if (stored != nullptr)
 {
  return quark :: optional<return_type>(std :: in_place,
    std :: invoke(std :: forward<Functor>(func), *std :: forward_like<Self>(stored)));
 }
 else
 {
  return quark :: optional<return_type>(quark :: nullopt);
 }
}

template <class Type>
template <class Self, class Functor>
inline constexpr auto quark :: optional <Type> :: or_else(this Self&& self, Functor&& func)
noexcept (noexcept(std :: forward_like<Self>(std :: declval<Type>())) && std :: is_nothrow_invocable_v<Functor>)
requires quark :: optional <Type> :: is_or_else_functor<Self, Functor>()
{
 if (stored != nullptr)
 {
  return std :: forward<Self>(self);
 }
 else
 {
  return std :: invoke(std :: forward<Functor>(func));
 }
}

template <class Type>
inline constexpr void quark :: optional <Type> :: swap(optional& other)
noexcept(std :: is_nothrow_destructible_v<Type> && is_nothrow_swapable_v<Type> && std :: is_nothrow_move_constructible_v<Type>)
requires std :: is_swappable_v<Type> && std :: is_move_constructible_v<Type>
{
 if (stored != nullptr && other.stored != nullptr)
 {
  using std :: swap;
  swap(*stored, *(other.stored));
 }
 else if (stored != nullptr && other.stored == nullptr)
 {
  other.stored = new (other.storage) Type(std :: move(*stored));
  reset();
 }
 else if (stored == nullptr && other.stored != nullptr)
 {
  stored = new (storage) Type(std :: move(*(other.stored)));
  other.reset();
 }
}

template <class Type>
inline constexpr void quark :: optional <Type> :: reset() noexcept(std :: is_nothrow_destructible_v<Type>)
{
 if (stored != nullptr)
 {
  if constexpr (!std :: is_trivially_destructible_v<Type>)
  {
   stored->~Type();
  }
  stored = nullptr;
 }
}

template <class Type>
template <class ... Args>
inline constexpr Type& quark :: optional <Type> :: emplace(Args&& ... args)
noexcept(std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, Args&&...>)
requires std :: constructible_from<Type, Args&&...>
{
 reset();
 stored = new (storage) Type(std :: forward<Args>(args)...);
 return *stored;
}

template <class Type>
template <class OtherType, class ... Args>
inline constexpr Type& quark :: optional <Type> :: emplace(std :: initializer_list<OtherType> ilist, Args&& ... args)
noexcept(std :: is_nothrow_destructible_v<Type> && std :: is_nothrow_constructible_v<Type, std :: initializer_list<OtherType>&, Args&&...>)
requires std :: constructible_from<Type, std :: initializer_list<OtherType>, Args&&...>
{
 reset();
 stored = new (storage) Type(ilist, std :: forward<Args>(args)...);
 return *stored;
}
#endif
