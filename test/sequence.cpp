#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <shear/make_grammar.hpp>
#include <shear/production.hpp>
#include <shear/nodes/sequence.hpp>
#include <shear/lalr/parser_from_grammar.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;

namespace list_symbols {

class X {};

class x_list {
  public:
    x_list(const std::vector<X>& v) : members_(v) {}
    const std::vector<X> members() const { return members_; }
  private:
    std::vector<X> members_;
};

}

using namespace list_symbols;

BOOST_AUTO_TEST_CASE(list_grammar)
{
  typedef sh::make_grammar<
    // root symbol
    x_list,
    // tokens
    mpl::vector<X>::type,
    // productions
    mpl::vector<
      sh::production<x_list, sh::nodes::sequence<X> >::type
    >::type
  >::type Grammar;
  // Construct the grammar
  Grammar grammar;
  try {
    grammar.check();
  } catch (sh::grammar_exception& e) {
    BOOST_ERROR(std::string("grammar exception: ") + e.what());
  }
  std::cout << "productions:\n";
  grammar.dump_productions(std::cout);
  // The parser type is similarly horrible, but obtainable from the grammar
  // type so we don't need to express all that stuff again.
  typedef sh::lalr::parser_from_grammar<Grammar> Parser;
  Parser parser(grammar, true);
  std::cout << "transition array:\n";
  parser.dump_transition_array(std::cout);
  // Tokens passed in to the parser one by one
  parser.handle_token(X());
  parser.handle_token(X());
  // And then we finalize to indicate the end of input, getting a parse object
  // encapsulating the result
  Parser::parse_type parse = parser.finalize();
  // The parse object converted to bool yields success / failure
  if (!parse) {
    std::copy(
        parse.syntax_errors().begin(), parse.syntax_errors().end(),
        std::ostream_iterator<sh::syntax_error>(std::cerr, "\n")
      );
    BOOST_ERROR("parse suffered syntax error");
  }
  // On success, we can get the root of the parse tree
  x_list root = *parse.root();
  // Check it constructed the right things
  BOOST_CHECK_EQUAL(root.members().size(), 2);
}

