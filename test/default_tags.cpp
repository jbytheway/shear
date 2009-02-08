#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <shear/make_grammar.hpp>
#include <shear/argument_tags.hpp>
#include <shear/reference.hpp>
#include <shear/production.hpp>
#include <shear/lalr/parser_from_grammar.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;

namespace default_tags_symbols {

class B {
  public:
    typedef sh::argument_tags::discard shear_default_argument_tag;
};
class C {
  public:
    typedef sh::argument_tags::by_pointer shear_default_argument_tag;
};
class A {
  public:
    A(boost::shared_ptr<C>, const B&) {}
};

}

using namespace default_tags_symbols;

BOOST_AUTO_TEST_CASE(default_tags)
{
  BOOST_MPL_ASSERT((
        boost::is_same<
          sh::compiletime::get_tag<B>::type,
          sh::argument_tags::discard
        >::type
      ));
  BOOST_MPL_ASSERT((
        boost::is_same<
          sh::compiletime::get_tag<C>::type,
          sh::argument_tags::by_pointer
        >::type
      ));
  typedef sh::make_grammar<
    // root symbol
    A,
    // tokens
    mpl::vector<B, C>::type,
    // productions
    mpl::vector<
      sh::production<A, B, C, sh::reference<B> >
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
  parser.handle_token(B());
  parser.handle_token(C());
  parser.handle_token(B());
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
  A root = *parse.root();
}

