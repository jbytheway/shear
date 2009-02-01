#ifndef SHEAR__LALAR__PARSER_BASE_HPP
#define SHEAR__LALAR__PARSER_BASE_HPP

#include <boost/multi_array.hpp>

#include <shear/lalr/parse.hpp>
#include <shear/lalr/any_symbol.hpp>
#include <shear/syntax_error.hpp>

namespace shear { namespace lalr {

template<
  typename RootSymbol,
  typename ActionIndexType,
  typename SymbolIndexType
>
class parser_base {
  public:
    typedef ActionIndexType action_index_type;
    typedef SymbolIndexType symbol_index_type;
    typedef RootSymbol root_symbol;
    typedef parse<root_symbol> parse_type;

    parser_base(
        action_index_type start_state,
        action_index_type error_code,
        symbol_index_type root_index,
        symbol_index_type end_of_file_code
      ) :
      metastate_(ms_parsing),
      state_(start_state),
      error_code_(error_code),
      root_index_(root_index),
      end_of_file_code_(end_of_file_code)
    {}

    void handle_error(const any_symbol<symbol_index_type>& token);

    void handle_token(const any_symbol<symbol_index_type>& token);
    parse_type finalize();

    void dump_transition_array(std::ostream&) const;
  protected:
    virtual boost::tuple<action_index_type, any_symbol<symbol_index_type> >
    reduce(action_index_type state, action_index_type redution) = 0;

    std::vector<std::string> symbol_names_;

    boost::multi_array<action_index_type, 2> transition_array_;

    struct stack_entry {
      stack_entry(
          action_index_type st,
          const any_symbol<symbol_index_type>& sy
        ) :
        state(st),
        symbol(sy)
      {}

      action_index_type state;
      any_symbol<symbol_index_type> symbol;
    };

    std::stack<stack_entry> stack_;
  private:
    enum { ms_parsing, ms_error } metastate_;
    action_index_type state_;
    const action_index_type error_code_;
    const symbol_index_type root_index_;
    const symbol_index_type end_of_file_code_;
    std::vector<syntax_error> syntax_errors_;
};

template<
  typename RootSymbol,
  typename ActionIndexType,
  typename SymbolIndexType
>
void parser_base<RootSymbol, ActionIndexType, SymbolIndexType>::handle_error(
    const any_symbol<symbol_index_type>& erroneous_symbol
  )
{
  location l = erroneous_symbol.start();
  std::string message;
  if (!symbol_names_.empty()) {
    throw std::logic_error("not implemented");
  } else {
    message = "syntax error";
  }
  syntax_errors_.push_back(syntax_error(l, message));
}

template<
  typename RootSymbol,
  typename ActionIndexType,
  typename SymbolIndexType
>
void parser_base<RootSymbol, ActionIndexType, SymbolIndexType>::handle_token(
    const any_symbol<symbol_index_type>& token
  )
{
  if (metastate_ == ms_error)
    return;

  any_symbol<symbol_index_type> next_symbol = token;
  bool token_queued = false;

  while (true) {
    // Find out from the transition array what it is we're
    // supposed to do with this symbol
    action_index_type action = transition_array_[state_][next_symbol.index()];

    // If it's an error, then handle it
    if (action == error_code_) {
      handle_error(next_symbol);
      metastate_ = ms_error;
      return;
    }
    // Otherwise, it's either a shift or a reduce
    if (action >= 0) {
      // We shift and move to specified state
      stack_.push(stack_entry(state_, next_symbol));
      state_ = action;
      if (token_queued) {
        token_queued = false;
        next_symbol = token;
      } else {
        break;
      }
    } else {
      // We perform a reduction
      boost::tie(state_, next_symbol) = reduce(state_, -action);
      token_queued = true;
    }
  }
}

template<
  typename RootSymbol,
  typename ActionIndexType,
  typename SymbolIndexType
>
parse<RootSymbol>
parser_base<RootSymbol, ActionIndexType, SymbolIndexType>::finalize()
{
  if (metastate_ == ms_error)
    return parse_type(syntax_errors_);

  any_symbol<symbol_index_type> next_symbol(
      boost::shared_ptr<void>(), end_of_file_code_
    );

  while (stack_.size() != 1 || stack_.top().symbol.index() != root_index_) {
    // Find out from the transition array what it is we're
    // supposed to do with this symbol
    action_index_type action = transition_array_[state_][next_symbol.index()];

    // If it's an error, then handle it
    if (action == error_code_)
    {
      handle_error(next_symbol);
      metastate_ = ms_error;
      return parse_type(syntax_errors_);
    }
    // Otherwise, it's either a shift or a reduce
    if (action >= 0) {
      // We shift and move to specified state
      stack_.push(stack_entry(state_, next_symbol));
      state_ = action;
      next_symbol.reset(boost::shared_ptr<void>(), end_of_file_code_);
    } else {
      // We perform a reduction
      boost::tie(state_, next_symbol) = reduce(state_, -action);
    }
  }

  boost::shared_ptr<root_symbol> root =
    stack_.top().symbol.template get_symbol<root_symbol>();
  stack_.pop();

  return parse_type(syntax_errors_, root);
}

template<typename R, typename A, typename S>
void parser_base<R, A, S>::dump_transition_array(std::ostream& o) const
{
  o << "{\n";
  for (size_t i=0; i<transition_array_.shape()[0]; ++i) {
    o << " [" << i << "] {";
    for (size_t j=0; j<transition_array_.shape()[1]; ++j) {
      o << " " << transition_array_[i][j] << ",";
    }
    o << "},\n";
  }
  o << "}\n";
}

}}

#endif // SHEAR__LALAR__PARSER_BASE_HPP

