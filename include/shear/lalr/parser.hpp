#ifndef SHEAR__LALR__PARSER_HPP
#define SHEAR__LALR__PARSER_HPP

#include <boost/array.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>

#include <shear/lalr/detail/station.hpp>
#include <shear/lalr/detail/item_state.hpp>
#include <shear/lalr/detail/dfa.hpp>
#include <shear/lalr/parse.hpp>

namespace shear { namespace lalr {

template<typename Grammar>
class parser {
  public:
    typedef Grammar grammar_type;
    typedef typename Grammar::symbol_index_type symbol_index_type;
    static const symbol_index_type num_symbols = Grammar::num_symbols;
    typedef parse<typename Grammar::root_symbol> parse_type;

    parser(const grammar_type&);

    template<typename Token>
    void handle_token(Token);

    parse_type finalize();
};

template<typename Grammar>
parser<Grammar>::parser(const Grammar& grammar)
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
  // Now we apply the subset algorithm
  detail::dfa dfa(nfa);
}

}}

#endif //SHEAR__LALR__PARSER_HPP

