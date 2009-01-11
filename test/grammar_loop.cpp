#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <shear/make_grammar.hpp>
#include <shear/production.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;
namespace px = boost::phoenix;

// Dummy classes for the grammar
namespace grammar_loop_symbols {
  class A {};
  class B {};
  class C {};
}

using namespace grammar_loop_symbols;

BOOST_AUTO_TEST_CASE(grammar_loop_test)
{
  {
    // This grammar has a trivial loop in
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<>::type,
      // productions
      mpl::vector<
        sh::production<A, A>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_loops(),
        sh::grammar_loop_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::grammar_loop_exception
      );
  }
  {
    // This grammar has a slightly less trivial loop in
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<>::type,
      // productions
      mpl::vector<
        sh::production<A, B>,
        sh::production<B, A>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_loops(),
        sh::grammar_loop_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::grammar_loop_exception
      );
  }
  {
    // This grammar has a loop in, but only because C produces empty
    typedef sh::make_grammar<
      // root symbol
      A,
      // tokens
      mpl::vector<>::type,
      // productions
      mpl::vector<
        sh::production<A, B>,
        sh::production<B, A, C>,
        sh::production<C>
      >::type
    >::type grammar_type;
    grammar_type arithmetic_grammar;
    BOOST_CHECK_THROW(
        arithmetic_grammar.check_for_loops(),
        sh::grammar_loop_exception
      );
    BOOST_CHECK_THROW(
        arithmetic_grammar.check(),
        sh::grammar_loop_exception
      );
  }
}

