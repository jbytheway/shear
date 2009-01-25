#ifndef SHEAR__LALR_DETAIL__DFA_HPP
#define SHEAR__LALR_DETAIL__DFA_HPP

#include <shear/automata/dfa.hpp>
#include <shear/lalr/detail/lookahead_dfa_state.hpp>

namespace shear { namespace lalr { namespace detail {

class dfa : public automata::dfa<lookahead_dfa_state> {
  public:
    dfa(const automata::nfa& nfa);
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

}}}

#endif // SHEAR__LALR_DETAIL__DFA_HPP

