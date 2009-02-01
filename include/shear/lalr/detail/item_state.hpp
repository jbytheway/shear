#ifndef SHEAR__LALR_DETAIL__ITEM_STATE_HPP
#define SHEAR__LALR_DETAIL__ITEM_STATE_HPP

#include <shear/automata/nfa.hpp>

namespace shear { namespace lalr { namespace detail {

class item_state : public automata::nfa::state {
  public:
    typedef boost::shared_ptr<item_state> ptr;

    // For the special production that does the outermost stuff
    item_state(size_t position) :
      penultimate_position_(position == 0),
      get_next_following_(false)
    {
    }

    // For the state at a given place in a production
    template<typename NonTerminals, typename FirstSets>
    item_state(
        const runtime::production::ptr& production,
        size_t position,
        const NonTerminals& non_terminals,
        const FirstSets& first_sets
      ) :
      production_(production),
      ultimate_position_(position == production->produced().size()),
      penultimate_position_(position == production->produced().size()-1),
      get_next_following_(false)
    {
      if (position < production->produced().size()-1) {
        typename NonTerminals::iterator nt =
          non_terminals.find(production->produced()[position]);
        get_next_following_ =
          nt != non_terminals.end() && nt->produces_empty();
      }
      for (size_t following_pos = position+1;
          following_pos < production->produced().size(); ++following_pos) {
        size_t symbol = production->produced()[following_pos];
        std::copy(
            first_sets[symbol].begin(), first_sets[symbol].end(),
            std::inserter(following_, following_.end())
          );
        typename NonTerminals::iterator nt = non_terminals.find(symbol);
        if (nt == non_terminals.end() || !nt->produces_empty()) {
          break;
        }
      }
    }

    template<typename Integer>
    void add_transition(const ptr& next, const Integer symbol) {
      assert(!progress_transition_);
      progress_transition_ =
        automata::nfa::state::add_transition(next, symbol);
    }

    void add_transition(const station::ptr& zoom_to) {
      assert(!zoom_in_transition_);
      zoom_in_transition_ = automata::nfa::state::add_transition(zoom_to);
    }

  	bool pump_lookahead()
  	{
  		bool altered = false;
  		
  		if (penultimate_position_) {
  			altered = add_following(lookahead_) || altered;
  		}
  		
  		if (progress_transition_) {
        ptr progress_to = boost::dynamic_pointer_cast<item_state>(
            progress_transition_->destination()
          );
        altered = progress_to->add_lookahead(lookahead_) || altered;

        if (get_next_following_) {
          altered = add_following(progress_to->following_) || altered;
        }

        if (zoom_in_transition_) {
          automata::nfa::state::ptr zoom_in_station =
            zoom_in_transition_->destination();

          BOOST_FOREACH(automata::nfa::transition::ptr zoomed_in_transition,
              zoom_in_station->transitions()) {
            ptr zoomed_in_state = boost::dynamic_pointer_cast<item_state>(
                zoomed_in_transition->destination()
                );
            altered = zoomed_in_state->add_lookahead(following_) || altered;
          }
        }
  		}
  		
  		return altered;
  	}

    bool add_lookahead(size_t i) {
      return lookahead_.insert(i).second;
    }

    template<typename OutputIterator>
    void get_reduction(OutputIterator o, size_t lookahead) {
      if (ultimate_position_ && lookahead_.count(lookahead) && production_) {
        *o++ = production_;
      }
    }
  private:
    template<typename Range>
    bool add_following(const Range& r) {
      size_t old_size = following_.size();
      std::copy(
          boost::begin(r), boost::end(r),
          std::inserter(following_, following_.end())
        );
      return following_.size() != old_size;
    }

    template<typename Range>
    bool add_lookahead(const Range& r) {
      size_t old_size = lookahead_.size();
      std::copy(
          boost::begin(r), boost::end(r),
          std::inserter(lookahead_, lookahead_.end())
        );
      return lookahead_.size() != old_size;
    }

    runtime::production::ptr production_;
    bool ultimate_position_;
    bool penultimate_position_;
    bool get_next_following_;
    std::set<size_t> lookahead_;
    std::set<size_t> following_;
    automata::nfa::transition::ptr progress_transition_;
    automata::nfa::transition::ptr zoom_in_transition_;
};

}}}

#endif // SHEAR__LALR_DETAIL__ITEM_STATE_HPP

