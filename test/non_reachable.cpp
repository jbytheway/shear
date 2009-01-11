#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <shear/make_grammar.hpp>
#include <shear/production.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;
namespace px = boost::phoenix;

// Dummy classes for the grammar
namespace non_reachable_symbols {
  class A {};
  class B {};
  class C {};
  class X {};
}

using namespace non_reachable_symbols;

BOOST_AUTO_TEST_CASE(non_reachable_test)
{
  {
    // Here B is non-reachable
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<X>::type,
      // productions
      mpl::vector<
        sh::production<A, X>,
        sh::production<B, X>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_non_reachable_non_terminals(),
        sh::non_reachable_non_terminals_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::non_reachable_non_terminals_exception
      );
  }
  {
    // Here B is non-reachable
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<X>::type,
      // productions
      mpl::vector<
        sh::production<A, X>,
        sh::production<B, X>,
        sh::production<B, A>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_non_reachable_non_terminals(),
        sh::non_reachable_non_terminals_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::non_reachable_non_terminals_exception
      );
  }
}

