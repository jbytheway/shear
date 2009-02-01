#ifndef SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP
#define SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP

#include <shear/lalr/detail/item_state.hpp>

namespace shear { namespace lalr { namespace detail {

class lookahead_dfa_state : public automata::dfa_state {
  public:
    typedef boost::shared_ptr<lookahead_dfa_state> ptr;

    template<typename Range>
    lookahead_dfa_state(size_t index, const Range& states) :
      automata::dfa_state(index, states)
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

    template<typename Row>
    void make_transition_row(
        size_t num_shifts,
        std::vector<conflict>&,
        Row
      );
  private:
    std::set<item_state::ptr> item_states_;
};

template<typename Row>
void lookahead_dfa_state::make_transition_row(
    size_t num_shifts,
    std::vector<conflict>& conflicts,
    Row row
  )
{
  for (size_t symbol_index=0; symbol_index<num_shifts; ++symbol_index)
  {
    // Seek out any applicable reductions
    std::vector<runtime::production::ptr> reductions;
    BOOST_FOREACH(item_state::ptr item_state, item_states_)
      item_state->get_reduction(std::back_inserter(reductions), symbol_index);
    // Now get the transition and see if it provides a shift
    const std::map<size_t, automata::dfa_state::ptr>& transitions =
      this->transitions();
    std::map<size_t, automata::dfa_state::ptr>::const_iterator transition =
      transitions.find(symbol_index);

    if (transition != transitions.end() && !reductions.empty())
    {
      // We have a shift/reduce conflict
#if 0 // No attempt at resolution yet
      // First we attempt to resolve it by checking for
      // associativity/precedence
      for (int i=0; i<reductions.Count; )
      {
        /*System.Console.WriteLine("[LAStateSet.WriteTransitionsToStream] i=" + i);*/
        Production reduction = reductions[i];
        int lookaheadPrecedence = precedence[nextSymbolIndex];

        if (lookaheadPrecedence > 0)
        {
          bool shiftSuggested = false;
          bool reduceSuggested = false;
          bool treatAsError = false;
          Symbol shiftSuggestingSymbol = null;
          Symbol reduceSuggestingSymbol = null;

          for (int j=0; j<reduction.Length; j++)
          {
            Symbol symbol = reduction[j];
            int symbolPrecedence;

            try
            {
              symbolPrecedence = precedence[symbol.Index];
            }
            catch (IndexOutOfRangeException)
            {
              throw new Exception("Index "+symbol.Index+" out of range in precedence array (symbol "+symbol.Name+").");
            }

            if (symbolPrecedence > 0)
            {
              if (symbolPrecedence > lookaheadPrecedence)
              {
                reduceSuggested = true;
                reduceSuggestingSymbol = symbol;
              }
              else if (symbolPrecedence < lookaheadPrecedence)
              {
                shiftSuggested = true;
                shiftSuggestingSymbol = symbol;
              }
              else
              {
                Associativity lookaheadAssociativity = associativity[nextSymbolIndex];
                if (associativity[symbol.Index] != lookaheadAssociativity)
                  throw new ConflictingAssociativityException(symbol.Name, getSymbolNameDelegate(nextSymbolIndex));
                switch (lookaheadAssociativity)
                {
                  case Associativity.Left:
                    reduceSuggested = true;
                    reduceSuggestingSymbol = symbol;
                    break;
                  case Associativity.Right:
                    shiftSuggested = true;
                    shiftSuggestingSymbol = symbol;
                    break;
                  case Associativity.Nonassoc:
                    treatAsError = true;
                    break;
                  default:
                    throw new UnrecognizedAssociativityException(lookaheadAssociativity);
                }
              }
            }
          }

          // Now we look at the results of this investigation into
          // associativity:
          if (shiftSuggested && reduceSuggested)
          {
            throw new ConflictingPrecedenceException(getSymbolNameDelegate(nextSymbolIndex), reduction, shiftSuggestingSymbol.Name, reduceSuggestingSymbol.Name);
          }
          if (treatAsError)
          {
            reductions.RemoveAt(i);
            transition = 0;
            continue;
          }
          if (shiftSuggested)
          {
            reductions.RemoveAt(i);
            continue;
          }
          if (reduceSuggested)
          {
            transition = 0;
          }
        }
        ++i;
      }
#endif
    }

    // Add conflict warning
    if (reductions.size() + ( transition != transitions.end() ? 1 : 0 ) > 1)
    {
      conflicts.push_back(conflict(
            transition != transitions.end(), reductions
          ));
    }

    // If we have nothing to do, put an error
    if (transition == transitions.end() && reductions.empty())
    {
      row[symbol_index] = 0;
      continue;
    }

    // If no reductions, put a shift
    if (reductions.empty())
    {
      row[symbol_index] = transition->second->index();
      continue;
    }

    // If no shift, put reduction
    if (transition == transitions.end() && reductions.size() == 1)
    {
      row[symbol_index] = -reductions[0]->index()-1;
      continue;
    }

    // We have a shift-reduce conflict, so store error code (but note that in
    // this case the array shouldn't actually be used for anything)
    row[symbol_index] = 0;
  }
}

}}}

#endif // SHEAR__LALR_DETAIL__LOOKAHEAD_DFA_STATE_HPP

