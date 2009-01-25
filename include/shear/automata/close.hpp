#ifndef SHEAR__AUTOMATA__CLOSE_HPP
#define SHEAR__AUTOMATA__CLOSE_HPP

#include <stack>

#include <shear/automata/nfa.hpp>

namespace shear { namespace automata {

// TODO: shouldn't be inline if no header-only
inline void close(std::set<nfa::state::ptr>& states) {
  std::vector<nfa::state::ptr> to_process(states.begin(), states.end());
  while (!to_process.empty()) {
    nfa::state::ptr next = to_process.back();
    to_process.pop_back();

    BOOST_FOREACH(nfa::transition::ptr transition, next->transitions()) {
      if (transition->is_epsilon_transition()) {
        if (states.insert(transition->destination()).second) {
          to_process.push_back(transition->destination());
        }
      }
    }
  }
}

}}

#endif // SHEAR__AUTOMATA__CLOSE_HPP

