#ifndef SHEAR__LALR__PARSER_FROM_GRAMMAR_HPP
#define SHEAR__LALR__PARSER_FROM_GRAMMAR_HPP

#include <boost/array.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>

#include <shear/compiletime/make_shared_ptr.hpp>
#include <shear/lalr/detail/station.hpp>
#include <shear/lalr/detail/item_state.hpp>
#include <shear/lalr/detail/dfa.hpp>
#include <shear/lalr/parser_base.hpp>
#include <shear/lalr/reduce.hpp>
#include <shear/lalr/lalr_exception.hpp>

namespace shear { namespace lalr {

template<typename Grammar>
class parser_from_grammar :
  public parser_base<
    typename Grammar::root_symbol,
    long,
    typename Grammar::symbol_index_type
  > {
  public:
    typedef Grammar grammar_type;
    typedef typename Grammar::symbol_index_type symbol_index_type;
    typedef long action_index_type;
    static const symbol_index_type num_symbols = Grammar::num_symbols;

    parser_from_grammar(const grammar_type&);

    virtual boost::tuple<action_index_type, any_symbol<symbol_index_type> >
    reduce(action_index_type state, action_index_type reduction);

    template<typename Token>
    void handle_token(Token t) {
      boost::shared_ptr<void> token_ptr(new Token(t));
      parser_base<
          typename Grammar::root_symbol,
          action_index_type,
          typename Grammar::symbol_index_type
        >::handle_token(any_symbol<symbol_index_type>(
              token_ptr,
              mpl::at<typename Grammar::symbol_index_map, Token>::type::value
            ));
    }
};

namespace detail {

template<size_t pos>
class extent_generator {
  public:
    template<typename Collection>
    inline boost::detail::multi_array::extent_gen<pos> operator()(
        const Collection& c
      ) const {
      return extent_generator<pos-1>()(c)[c[pos-1]];
    }
};

template<>
class extent_generator<0> {
  public:
    template<typename Collection>
    inline boost::detail::multi_array::extent_gen<0> operator()(
        const Collection&
      ) const {
      return boost::extents;
    }
};

}

template<typename Grammar>
parser_from_grammar<Grammar>::parser_from_grammar(const Grammar& grammar) :
  parser_base<
    typename Grammar::root_symbol, action_index_type, symbol_index_type
  >(
      1 /* start_state */, 0 /* error_code */, Grammar::root_index::value,
      Grammar::end_of_file_code
    )
{
  // First we compute the First sets of each Symbol
  boost::array<std::set<symbol_index_type>, num_symbols> first_sets;
  // For tokens, the first set consists of the token itself
  mpl::for_each<
    typename Grammar::tokens,
    mpl::at<typename Grammar::symbol_index_map, mpl::_1>
  >(px::insert(
        px::ref(first_sets)[arg1],
        arg1
      ));

  bool altered;

  do {
    altered = false;
    BOOST_FOREACH(const typename Grammar::NonTerminalMap::value_type& nt,
        grammar.non_terminals_r()) {
      std::set<symbol_index_type>& this_first_set = first_sets[nt.index()];
      size_t old_size = this_first_set.size();
      BOOST_FOREACH(const runtime::production::ptr& production,
          nt.productions()) {
        BOOST_FOREACH(symbol_index_type i, production->produced()) {
          std::copy(
              first_sets[i].begin(), first_sets[i].end(),
              std::inserter(this_first_set, this_first_set.end())
            );

          typename Grammar::NonTerminalMap::iterator produced_nt =
            grammar.non_terminals_r().find(i);
          if (produced_nt == grammar.non_terminals_r().end() ||
              !produced_nt->produces_empty())
            break;
        }
      }
      altered = altered || old_size < this_first_set.size();
    }
  } while (altered);

  //foreach (NonTerminal nonTerminal in mNonTerminals)
  //{
  //	System.Console.WriteLine("First set for "+nonTerminal.NiceName+" is:");
  //	foreach (Token token in nonTerminal.First)
  //		System.Console.Write(", "+token.NiceName);
  //	System.Console.WriteLine();
  //}

  // Next we build the NFA.  This consists of a bunch of ItemStates
  // and Stations.

  // We make the stations first.
  // We have one station per NonTerminal plus one extra
  // to be the start station
  std::map<symbol_index_type, detail::station::ptr> stations;

  BOOST_FOREACH(const typename Grammar::NonTerminalMap::value_type& nt,
      grammar.non_terminals_r()) {
    detail::station::ptr station(new detail::station(nt));
    stations[nt.index()] = station;
  }
  detail::station::ptr start_station(new detail::station());

  // Now we need ItemStates for all the production rules.
  BOOST_FOREACH(const typename Grammar::NonTerminalMap::value_type& nt,
      grammar.non_terminals_r()) {
    BOOST_FOREACH(const runtime::production::ptr& production,
        nt.productions()) {
      detail::item_state::ptr previous_item_state(
          new detail::item_state(
            production, 0, grammar.non_terminals_r(), first_sets
          )
        );
      detail::item_state::ptr next_item_state;
      stations[nt.index()]->add_transition(previous_item_state);

      for (size_t j=0; j<production->produced().size(); ++j)
      {
        next_item_state.reset(new detail::item_state(
              production, j+1, grammar.non_terminals_r(), first_sets
            ));
        symbol_index_type next_symbol = production->produced()[j];
        previous_item_state->add_transition(
            next_item_state, next_symbol
          );

        typename std::map<symbol_index_type, detail::station::ptr>::iterator
          next_station = stations.find(next_symbol);
        if (next_station != stations.end())
          previous_item_state->add_transition(next_station->second);
        previous_item_state = next_item_state;
      }
    }
  }

  // And we deal with the last, special station
  detail::item_state::ptr start_of_special_production(
      new detail::item_state(0)
    );
  detail::item_state::ptr end_of_special_production(
      new detail::item_state(1)
    );
  start_station->add_transition(start_of_special_production);
  start_of_special_production->add_transition(
      stations[Grammar::root_index::value]
    );
  start_of_special_production->add_transition(
      end_of_special_production, Grammar::root_index::value
    );

  // Add lookahead for the special production now
  start_of_special_production->add_lookahead(Grammar::end_of_file_code);
  end_of_special_production->add_lookahead(Grammar::end_of_file_code);

  automata::nfa nfa(start_station);
  // Now we apply the subset algorithm to convert to a DFA
  detail::dfa dfa(nfa);

  // And construct the tansition array
  std::vector<conflict> conflicts;
  boost::multi_array<action_index_type, 2> transition_array(
      dfa.make_transition_array<action_index_type>(
        Grammar::end_of_file_code+1, conflicts
      )
    );
  this->transition_array_.resize(
      detail::extent_generator<2>()(transition_array.shape())
    );
  this->transition_array_ = transition_array;

  if (!conflicts.empty()) {
    throw lalr_conflict_exception(conflicts);
  }
}

namespace detail {

template<size_t index>
struct fill_tuple_backwards {
  template<
    typename Tuple,
    typename Stack,
    typename Integer
  >
  void operator()(Tuple& tuple, Stack& stack, Integer& state)
  {
    //typedef typename compiletime::show_me<Tuple>::type foo;
    fusion::at<mpl::size_t<index-1> >(tuple).operator =( stack.top().symbol.
        template get_symbol<
          typename fusion::result_of::value_at<
            Tuple, mpl::size_t<index-1>
          >::type::value_type
        >());
    state = stack.top().state;
    stack.pop();
    fill_tuple_backwards<index-1>()(tuple, stack, state);
  }
};

template<>
struct fill_tuple_backwards<0> {
  template<
    typename Tuple,
    typename Stack,
    typename Integer
  >
  void operator()(Tuple&, Stack&, Integer&) {}
};

template<typename Grammar, long reduction, typename Stack, typename Integer>
inline typename boost::enable_if<
  typename mpl::less<
    mpl::long_<reduction>,
    typename mpl::size<typename Grammar::productions>::type
  >::type,
  boost::tuple<
    typename parser_from_grammar<Grammar>::action_index_type,
    any_symbol<typename Grammar::symbol_index_type>
  >
>::type
reduce_helper(Stack& stack, Integer state)
{
  // Get the type of the production
  typedef typename mpl::first<typename mpl::at<
    typename Grammar::production_index_vector, mpl::long_<reduction>
  >::type>::type production;
  // We extract the symbols in reverse order from the stack into a vector
  //typedef typename compiletime::show_me<production>::type foo;
  typedef typename production::produced produced;
  typedef typename fusion::result_of::as_vector<
    typename mpl::transform<
      produced,
      compiletime::make_shared_ptr<mpl::_1>
    >::type
  >::type argument_vector;
  argument_vector args;
  typedef typename fusion::result_of::size<argument_vector>::type vector_size;
  fill_tuple_backwards<vector_size::value>()(args, stack, state);
  any_symbol<typename Grammar::symbol_index_type> symbol =
    reduce<Grammar, production, typename Grammar::symbol_index_type>(args);
  return boost::make_tuple(state, symbol);
}

}

template<typename Grammar>
boost::tuple<
  typename parser_from_grammar<Grammar>::action_index_type,
  any_symbol<typename Grammar::symbol_index_type>
>
parser_from_grammar<Grammar>::reduce(
    action_index_type state,
    action_index_type reduction
  )
{
  // reduction counts from 1, not zero
  switch (reduction) {
#define SHEAR_LALR_PARSER_FROM_GRAMMAR_CASE(val) \
    case val:                                    \
      if (val <= mpl::size<typename Grammar::productions>::type::value) { \
        return detail::reduce_helper<Grammar, long(val-1)>(this->stack_, state); \
      } else {                                   \
        break;                                   \
      }
    SHEAR_LALR_PARSER_FROM_GRAMMAR_CASE(1)
    default:
      break;
  }
  abort();
}

}}

#endif // SHEAR__LALR__PARSER_FROM_GRAMMAR_HPP

