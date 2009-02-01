#ifndef SHEAR__LALR_DETAIL__DFA_HPP
#define SHEAR__LALR_DETAIL__DFA_HPP

#include <boost/multi_array.hpp>

#include <shear/automata/dfa.hpp>
#include <shear/lalr/conflict.hpp>
#include <shear/lalr/detail/lookahead_dfa_state.hpp>

namespace shear { namespace lalr { namespace detail {

class dfa : public automata::dfa<lookahead_dfa_state> {
  public:
    dfa(const automata::nfa& nfa);

    template<typename Element>
    boost::multi_array<Element, 2>
    make_transition_array(size_t num_shifts, std::vector<conflict>&);
};

// TODO: if not header-only should not be inline
inline dfa::dfa(const automata::nfa& nfa) :
  automata::dfa<lookahead_dfa_state>(nfa)
{
  // Add lookahead

  // EOF lookahead should already have been added outside somewhere

  bool altered;
  do {
    altered = false;

    BOOST_FOREACH(lookahead_dfa_state::ptr state_set, states_) {
      altered = state_set->pump_lookahead() || altered;
    }
  } while(altered);
}

template<typename Element>
boost::multi_array<Element, 2>
dfa::make_transition_array(
    size_t num_shifts,
    std::vector<conflict>& conflicts
  ) {
  boost::multi_array<Element, 2> array(
      boost::extents[states_.size()][num_shifts]
    );

  typedef StateMap::index<index_tag>::type StatesByIndex;
  StatesByIndex& states_by_index = states_.get<index_tag>();

  StatesByIndex::iterator it = states_by_index.begin();
  for (size_t i=0; i<states_.size(); ++i, ++it) {
    lookahead_dfa_state& state = **it;
    assert(state.index() == i);
    state.make_transition_row(
        num_shifts, conflicts, array[i]
      );
  }
  return array;
}

}}}

#endif // SHEAR__LALR_DETAIL__DFA_HPP

