//
// tools_test.cpp
//
// Created on: Apr 22, 2018
//     Author: alexen
//

#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>
#include <boost/lexical_cast/bad_lexical_cast.hpp>

#include <dao/tools.h>


enum class CustomUserTypes {
     First,
     Second,
     Third
};


namespace boost {


template<>
std::string lexical_cast< std::string, CustomUserTypes >( const CustomUserTypes& t )
{
     switch( t )
     {
          case CustomUserTypes::First:  return "THE FIRST";
          case CustomUserTypes::Second: return "THE SECOND";
          case CustomUserTypes::Third:  return "THE THIRD";
          default:                      ;
     }
     BOOST_THROW_EXCEPTION( boost::bad_lexical_cast{} );
}


} // namespace boost



std::ostream& operator<<( std::ostream& ostr, const CustomUserTypes t )
{
     ostr << boost::lexical_cast< std::string >( t );
     return ostr;
}


TEST( MakeSeparatedList, IntegersWithoutQuoting )
{
     static constexpr auto integers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
     static constexpr auto expectedList = "1, 2, 3, 4, 5, 6, 7, 8, 9";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          integers.begin(),
          integers.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          integers.begin(),
          integers.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, str );
}


TEST( MakeSeparatedList, StringsWithoutQuoting )
{
     static constexpr auto strings = {
          "Richard", "Of", "York", "Gave", "Battle", "In", "Vain"
     };
     static constexpr auto expectedList =
          "Richard, Of, York, Gave, Battle, In, Vain";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          strings.begin(),
          strings.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          strings.begin(),
          strings.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, str );
}


TEST( MakeSeparatedList, CustomUserTypesWithoutQuoting )
{
     static constexpr auto types = {
          CustomUserTypes::First,
          CustomUserTypes::Third,
          CustomUserTypes::Second,
          CustomUserTypes::First,
          CustomUserTypes::Second
     };
     static constexpr auto expectedList =
          "THE FIRST, THE THIRD, THE SECOND, THE FIRST, THE SECOND";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          types.begin(),
          types.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          types.begin(),
          types.end(),
          []( auto param )
          {
               return param;
          }
          );

     ASSERT_EQ( expectedList, str );
}


template< typename T >
std::string customQuoter( const T& t )
{
     return '`' + boost::lexical_cast< std::string >( t ) + '`';
}


TEST( MakeSeparatedList, IntegersWithCustomQuoting )
{
     static constexpr auto integers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
     static constexpr auto expectedList = "`1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          integers.begin(),
          integers.end(),
          customQuoter< int >
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          integers.begin(),
          integers.end(),
          customQuoter< int >
          );

     ASSERT_EQ( expectedList, str );
}


TEST( MakeSeparatedList, StringsWithCustomQuoting )
{
     static constexpr auto strings = {
          "Richard", "Of", "York", "Gave", "Battle", "In", "Vain"
     };
     static constexpr auto expectedList =
          "`Richard`, `Of`, `York`, `Gave`, `Battle`, `In`, `Vain`";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          strings.begin(),
          strings.end(),
          customQuoter< const char* >
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          strings.begin(),
          strings.end(),
          customQuoter< const char* >
          );

     ASSERT_EQ( expectedList, str );
}


TEST( MakeSeparatedList, CustomUserTypesWithCustomQuoting )
{
     static constexpr auto types = {
          CustomUserTypes::First,
          CustomUserTypes::Third,
          CustomUserTypes::Second,
          CustomUserTypes::First,
          CustomUserTypes::Second
     };
     static constexpr auto expectedList =
          "`THE FIRST`, `THE THIRD`, `THE SECOND`, `THE FIRST`, `THE SECOND`";

     std::ostringstream ostr;

     dao::tools::separatedList(
          ostr,
          types.begin(),
          types.end(),
          customQuoter< CustomUserTypes >
          );

     ASSERT_EQ( expectedList, ostr.str() );

     const auto& str = dao::tools::separatedList(
          types.begin(),
          types.end(),
          customQuoter< CustomUserTypes >
          );

     ASSERT_EQ( expectedList, str );
}
