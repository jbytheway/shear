#ifndef SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP
#define SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP

#include <shear/lalr/detail/item_state.hpp>

namespace shear { namespace lalr { namespace detail {

class lookahead_dfa_state : public automata::dfa_state {
  public:
    typedef boost::shared_ptr<lookahead_dfa_state> ptr;

    template<typename Range>
    lookahead_dfa_state(const Range& states) :
      automata::dfa_state(states)
    {
      BOOST_FOREACH(automata::nfa::state::ptr state, states) {
        if (item_state::ptr is =
            boost::dynamic_pointer_cast<item_state>(state)) {
          item_states_.insert(is);
        }
      }
    }

    bool pump_lookahead() {
      bool altered = false;
			BOOST_FOREACH(item_state::ptr item_state, item_states_) {
				altered = item_state->pump_lookahead() || altered;
      }
			return altered;
    }

  private:
    std::set<item_state::ptr> item_states_;
};

}}}

#endif // SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP

