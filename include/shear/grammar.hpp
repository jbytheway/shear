#ifndef SHEAR__GRAMMAR_HPP
#define SHEAR__GRAMMAR_HPP

#include <map>
#include <set>
#include <queue>

#include <boost/mpl/for_each.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit/home/phoenix/core/reference.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/home/phoenix/object/new.hpp>
#include <boost/spirit/home/phoenix/stl/container.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <shear/grammar_exception.hpp>
#include <shear/compiletime/grammar_base.hpp>
#include <shear/runtime/non_terminal.hpp>
#include <shear/runtime/production.hpp>

namespace shear {

template<
  typename RootSymbol,
  typename Tokens,
  typename Productions
>
class grammar :
  public compiletime::grammar_base<RootSymbol, Tokens, Productions> {
  public:
    typedef size_t symbol_index_type;
    static const symbol_index_type num_symbols =
      mpl::size<typename grammar::symbol_index_vector>::type::value;

    grammar();

    void check();
    void check_for_loops();
    void check_for_non_productive_non_terminals();
    void check_for_non_reachable_non_terminals();
  private:
    typedef boost::multi_index_container<
      runtime::production::ptr,
      boost::multi_index::indexed_by<
        boost::multi_index::hashed_non_unique<
          BOOST_MULTI_INDEX_CONST_MEM_FUN(
              runtime::production,
              symbol_index_type,
              source_index
            )
        >
      >
    > ProductionMap;
    ProductionMap productions_r_;
    typedef boost::multi_index_container<
      runtime::non_terminal,
      boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
          BOOST_MULTI_INDEX_CONST_MEM_FUN(
              runtime::non_terminal,
              symbol_index_type,
              index
            )
        >
      >
    > NonTerminalMap;
    NonTerminalMap non_terminals_r_;
};

template<typename R, typename T, typename P>
grammar<R, T, P>::grammar()
{
  // Copy compile-time info to runtime versions
  mpl::for_each<
    typename grammar::productions
  >(px::insert(
        px::ref(productions_r_),
        px::construct<runtime::production::ptr>(
          px::new_<runtime::production>(
            arg1, typename grammar::symbol_index_map()
          )
        )
      ));
  mpl::for_each<
    typename grammar::non_terminals,
    mpl::at<typename grammar::symbol_index_map, mpl::_1>
  >(px::insert(
        px::ref(non_terminals_r_),
        px::construct<runtime::non_terminal>(arg1, px::cref(productions_r_))
      ));
  // Determine which symbols produce empty
  bool altered;
  do {
    altered = false;
    BOOST_FOREACH(const typename NonTerminalMap::value_type& nt,
        non_terminals_r_) {
      if (!nt.produces_empty()) {
        BOOST_FOREACH(const runtime::production::ptr& production,
            nt.productions()) {
          bool can_be_empty = true;
          BOOST_FOREACH(symbol_index_type i, production->produced()) {
            if (!non_terminals_r_.find(i)->produces_empty()) {
              can_be_empty = false;
              break;
            }
          }
          if (can_be_empty) {
            nt.set_produces_empty();
            altered = true;
            break;
          }
        }
      }
    }
  } while (altered);
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check()
{
  check_for_loops();
  check_for_non_productive_non_terminals();
  check_for_non_reachable_non_terminals();
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check_for_loops()
{
  BOOST_FOREACH(const runtime::non_terminal& checking_non_terminal,
      non_terminals_r_) {
    // Create a collection to store those non-terminals which this
    // one can produce
    std::set<symbol_index_type> non_terminals_produced;
    std::queue<symbol_index_type> non_terminals_to_process;
    non_terminals_to_process.push(checking_non_terminal.index());

    while (!non_terminals_to_process.empty()) {
      // Get next produced non-terminal
      const runtime::non_terminal& to_process =
        *non_terminals_r_.find(non_terminals_to_process.front());
      non_terminals_to_process.pop();
      if (non_terminals_produced.count(to_process.index())) {
        continue;
      }
      // Add it to the collection of those produced
      non_terminals_produced.insert(to_process.index());

      // Look for all rules capable of producing a single symbol
      BOOST_FOREACH(const runtime::production::ptr& production,
          to_process.productions())
      {
        size_t min_length = 0;
        const runtime::non_terminal* last_compulsory_symbol = NULL;

        BOOST_FOREACH(symbol_index_type symbol_index, production->produced()) {
          NonTerminalMap::iterator symbol = non_terminals_r_.find(symbol_index);
          if (symbol != non_terminals_r_.end() && !symbol->produces_empty())
          {
            last_compulsory_symbol = &*symbol;
            ++min_length;
            if (min_length > 1)
              break;
          }
        }

        switch (min_length)
        {
          case 0:
            // In this case any of the symbols in the list can be produced alone
            BOOST_FOREACH(symbol_index_type symbol_index,
                production->produced())
            {
              if (symbol_index == checking_non_terminal.index())
                throw grammar_loop_exception(production);
              if (non_terminals_r_.count(symbol_index))
                non_terminals_to_process.push(symbol_index);
            }
            break;
          case 1:
            // Check for a loop
            if (last_compulsory_symbol == &checking_non_terminal)
              throw grammar_loop_exception(production);
            // Enqueue this for further processing
            if (non_terminals_r_.count(last_compulsory_symbol->index()))
              non_terminals_to_process.push(last_compulsory_symbol->index());
            break;
          default:
            break;
        }
      }
    }
  }

}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check_for_non_productive_non_terminals()
{
  // TODO:
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check_for_non_reachable_non_terminals()
{
  // TODO:
}

}

#endif // SHEAR__GRAMMAR_HPP

