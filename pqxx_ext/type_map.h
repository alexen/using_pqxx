//
// type_map.h
//
// Created on: Apr 18, 2018
//     Author: alexen
//

#pragma once

#include <boost/optional/optional.hpp>
#include <pqxx/strconv>


/// Специализации для любых типов, в том числе и пользовательских
///
/// @note Специализация требует наличия функций конвертации в std::string и обратно
/// в виде специализаций функций boost::lexical_cast<>()
///
/// Пример кода
/// @code
/// enum class UserType {
///     One,
///     Two,
///     Three
/// };
///
///
/// namespace pqxx {
///
/// PQXX_STRING_TRAITS_SPEC( UserType );
///
/// }
///
///
/// namespace boost {
///
/// template<> std::string lexical_cast< std::string, UserType >( const UserType& ut )
/// {
///     // ...преобразование UserType в std::string...
///     // (в случае ошибки выбрасываем исключение)
///     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
/// }
///
///
/// template<> UserType lexical_cast< UserType, std::string >( const std::string& ut )
/// {
///     // ...преобразование std::string в UserType...
///     // (в случае ошибки выбрасываем исключение)
///     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
/// }
///
/// }
/// @endcode
///

namespace pqxx {


/// Частичная специализация для маппинга nullable-типов в boost::optional<T>
template< typename T >
struct string_traits< boost::optional< T > >
{
     static constexpr const char* name() noexcept { return "boost::optional<T>"; }
     static constexpr bool has_null() noexcept { return true; }
     static bool is_null( const boost::optional< T >& val ) noexcept { return !val; }
     static boost::optional< T > null() { return boost::none; }
     static void from_string( const char* str, boost::optional< T >& out )
     {
          T val;
          string_traits< T >::from_string( str, val );
          out = val;
     }
     static std::string to_string( const boost::optional< T >& opt )
     {
          return string_traits< T >::to_string( *opt );
     }
};


/// Макрос для маппинга типов
#define PQXX_STRING_TRAITS_SPEC( T )                                                  \
     template<> struct string_traits< T >                                             \
     {                                                                                \
          static constexpr const char *name() noexcept { return #T; }                 \
          static constexpr bool has_null() noexcept { return false; }                 \
          static bool is_null( T ) { return false; }                                  \
          [[noreturn]] static T null() { internal::throw_null_conversion( name() ); } \
          static void from_string( const char* str, T &obj )                          \
          {                                                                           \
               obj = ::boost::lexical_cast< T, std::string >( str );                  \
          }                                                                           \
          static std::string to_string( const T& obj )                                \
          {                                                                           \
               return ::boost::lexical_cast< std::string >( obj );                    \
          }                                                                           \
     }


} // namespace pqxx
