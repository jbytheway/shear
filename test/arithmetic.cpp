namespace sh = shear;

class X {};
class PLUS {};

class expression {
  public:
    expression(const X&) : is_x_(true), sub_() {}
    expression(const expression& e, const X&) :
      is_x_(false), sub_(new expression(e))
    {}

    bool is_x() const { return is_x_; }
    const expression& subexpression() const { assert(is_x_); return *sub_(); }
  private:
    bool is_x_;
    boost::scoped_ptr<expression> sub_;
};

int main()
{
  // For the type of the grammar we need to specify the root symbol and all the
  // tokens.  Even so, this type will be abstract (the concrete type also needs
  // to encoude all the productions)
  typedef sh::grammar_using<expression, X, PLUS>::type grammar_type;
  // Construct the grammar; following everyone else we use a BNF-alike in C++
  grammar_type& arithmetic_grammar = sh::make_grammar[
    sh::token<X>(),
    sh::token<PLUS>(),
    sh::symbol<expression>()
      = sh::ref<X>()
      = sh::ref<expression>() << sh::discard<PLUS>() << sh::ref<X>()
  ];
  // The parser type similarly depends on the root symbol and tokens, and is
  // similarly abstract, but obtainable from the grammar type so we don't need
  // to express all that stuff again.
  typedef sh::result_of::make_lalr_parser<grammar_type>::type parser_type;
  parser_type& arithmetic_parser =
    sh::make_lalr_parser(arithmetic_grammar);
  // Tokens passed in to the parser one by one
  arithmetic_parser.handle_token(X())
  arithmetic_parser.handle_token(PLUS())
  arithmetic_parser.handle_token(X())
  // And then we finalize to indicate the end of input, getting a parse object
  // encapsulating the result
  parser_type::parse_type parse = arithmetic_parser.finalize();
  // The parse object converted to bool yields success / failure
  assert(parse);
  // On success, we can get the root of the parse tree
  expression root = *parse.root();
  // Check it constructed the right things
  assert(!expression.is_x());
  assert(expression.subexpression().is_x());
}

