#ifndef SHEAR__AUTOMATA__DFA_HPP
#define SHEAR__AUTOMATA__DFA_HPP

#include <boost/spirit/home/phoenix/bind.hpp>
#include <boost/spirit/home/phoenix/operator/logical.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <shear/automata/nfa.hpp>
#include <shear/automata/close.hpp>

namespace shear { namespace automata {

class dfa_transition {
  public:
    typedef boost::shared_ptr<dfa_transition> ptr;
};

class dfa_state {
  public:
    typedef boost::shared_ptr<dfa_state> ptr;

    dfa_state(const std::set<nfa::state::ptr>& n) : nfa_states_(n) {}
    virtual ~dfa_state() = 0;

    const std::set<nfa::state::ptr>& nfa_states() const { return nfa_states_; }

    void add_transition(const size_t match, const ptr& destination) {
      assert(!transitions_.count(match));
      transitions_[match] = destination;
    }
  private:
    std::set<nfa::state::ptr> nfa_states_;
    std::map<size_t, ptr> transitions_;
};

template<typename State>
class dfa {
  public:
    typedef State state_type;

    dfa(const nfa&);
  protected:
    boost::shared_ptr<state_type> empty_state_;
    boost::shared_ptr<state_type> start_state_;
    
    typedef boost::multi_index_container<
      boost::shared_ptr<state_type>,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
          BOOST_MULTI_INDEX_CONST_MEM_FUN(
              dfa_state,
              const std::set<nfa::state::ptr>&,
              nfa_states
            )
        >
      >
    > StateMap;
    StateMap states_;
};

inline dfa_state::~dfa_state() {}

// TODO: if not header-only should not be inline
template<typename State>
inline dfa<State>::dfa(const nfa& n)
{
  {
    std::set<nfa::state::ptr> nfa_states;
    empty_state_.reset(new state_type(nfa_states));
    states_.insert(empty_state_);

    nfa_states.insert(n.start_state());
    close(nfa_states);
    start_state_.reset(new state_type(nfa_states));
    states_.insert(start_state_);
  }

  std::stack<dfa_state::ptr> to_process;
  to_process.push(start_state_);

  while (!to_process.empty()) {
    dfa_state::ptr next = to_process.top();
    to_process.pop();

    // extract all the transitions leading from this state (i.e. not epsilon
    // transitions)
    std::vector<nfa::transition::ptr> transitions;
    BOOST_FOREACH(nfa::state::ptr nfa_state, next->nfa_states()) {
      std::remove_copy_if(
          nfa_state->transitions().begin(), nfa_state->transitions().end(),
          std::back_inserter(transitions),
          px::bind(&nfa::transition::is_epsilon_transition, *arg1)
        );
    }
    // Now get all the values on which these transitions match
    std::set<size_t> matched;
    BOOST_FOREACH(nfa::transition::ptr transition, transitions) {
      size_t match;
      transition->get_match(match);
      matched.insert(match);
    }
    // And now for each such value ...
    BOOST_FOREACH(size_t potential_match, matched) {
      // ... determine the set of states transitioned to
      std::set<nfa::state::ptr> destination_states;
      BOOST_FOREACH(nfa::transition::ptr transition, transitions) {
        if (transition->matches(potential_match)) {
          destination_states.insert(transition->destination());
        }
      }
      // close it
      close(destination_states);
      // check if we already have it
      typename StateMap::iterator existing = states_.find(destination_states);

      boost::shared_ptr<state_type> new_state;
      if (existing == states_.end()) {
        // We didn't already have it
        new_state.reset(new state_type(destination_states));
        to_process.push(new_state);
        states_.insert(new_state);
      } else {
        new_state = *existing;
      }
      next->add_transition(potential_match, new_state);
    }
  }
}

}}

#endif // SHEAR__AUTOMATA__DFA_HPP

