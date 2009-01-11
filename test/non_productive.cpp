#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <shear/make_grammar.hpp>
#include <shear/production.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;
namespace px = boost::phoenix;

// Dummy classes for the grammar
namespace non_productive_symbols {
  class A {};
  class B {};
  class C {};
  class X {};
}

using namespace non_productive_symbols;

BOOST_AUTO_TEST_CASE(non_productive_test)
{
  {
    // Here A is non-productive because it can only produce things involving
    // another A, never leading to tokens
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<X>::type,
      // productions
      mpl::vector<
        sh::production<A, A, X>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_non_productive_non_terminals(),
        sh::non_productive_non_terminals_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::non_productive_non_terminals_exception
      );
  }
  {
    // Here A is non-productive because it can only produce things involving
    // another B, which then leads back to A, never leading to tokens
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<X>::type,
      // productions
      mpl::vector<
        sh::production<A, B, X>,
        sh::production<B, A, X>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_non_productive_non_terminals(),
        sh::non_productive_non_terminals_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::non_productive_non_terminals_exception
      );
  }
}

