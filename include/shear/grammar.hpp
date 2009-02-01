#ifndef SHEAR__GRAMMAR_HPP
#define SHEAR__GRAMMAR_HPP

#include <map>
#include <set>
#include <queue>

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/has_key.hpp>
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
  BOOST_MPL_ASSERT((
        typename mpl::has_key<
          typename grammar::non_terminals, typename grammar::root
        >::type
      ));
  public:
    typedef size_t symbol_index_type;
    typedef RootSymbol root_symbol;
    static const symbol_index_type num_symbols =
      mpl::size<typename grammar::symbol_index_vector>::type::value;
    static const symbol_index_type end_of_file_code = num_symbols;

    typedef typename mpl::at<
        typename grammar::symbol_index_map,
        typename grammar::root
      >::type root_index;

    grammar();

    void check() throw(grammar_exception&);
    void check_for_loops() throw(grammar_loop_exception&);
    void check_for_non_productive_non_terminals()
      throw(non_productive_non_terminals_exception&);
    void check_for_non_reachable_non_terminals()
      throw(non_reachable_non_terminals_exception&);

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

    const NonTerminalMap& non_terminals_r() const { return non_terminals_r_; }

    void dump_productions(std::ostream&);
  private:
    ProductionMap productions_r_;
    NonTerminalMap non_terminals_r_;
};

template<typename R, typename T, typename P>
grammar<R, T, P>::grammar()
{
  // Copy compile-time info to runtime versions
  mpl::for_each<
    typename grammar::productions,
    mpl::at<typename grammar::production_index_map, mpl::_1>
  >(px::insert(
        px::ref(productions_r_),
        px::construct<runtime::production::ptr>(
          px::new_<runtime::production>(
            arg1, typename grammar::production_index_vector(),
            typename grammar::symbol_index_map()
          )
        )
      ));
  mpl::for_each<
    typename grammar::non_terminals,
    mpl::at<typename grammar::symbol_index_map, mpl::_1>
  >(px::insert(
        px::ref(non_terminals_r_),
        px::construct<runtime::non_terminal>(
          arg1, px::cref(productions_r_),
          typename grammar::symbol_index_vector()
        )
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
void grammar<R, T, P>::check() throw(grammar_exception&)
{
  check_for_loops();
  check_for_non_productive_non_terminals();
  check_for_non_reachable_non_terminals();
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check_for_loops() throw(grammar_loop_exception&)
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
          if (symbol == non_terminals_r_.end() || !symbol->produces_empty())
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
  throw(non_productive_non_terminals_exception&)
{
  // Create a collection of all productive non-terminals
  std::set<symbol_index_type> productive_non_terminals;

  // Repeatedly search through all non-terminals looking for new ones
  // to add to the collection
  size_t num_productive_non_terminals;
  do {
    num_productive_non_terminals = productive_non_terminals.size();

    BOOST_FOREACH(const runtime::non_terminal& non_terminal, non_terminals_r_)
    {
      if (productive_non_terminals.count(non_terminal.index()))
        continue;
      BOOST_FOREACH(const runtime::production::ptr& production,
          non_terminal.productions()) {
        bool productiveProduction = true;
        BOOST_FOREACH(symbol_index_type symbol_index, production->produced()) {
          if (non_terminals_r_.count(symbol_index) &&
              !productive_non_terminals.count(symbol_index)) {
            productiveProduction = false;
            break;
          }
        }
        if (productiveProduction) {
          productive_non_terminals.insert(non_terminal.index());
          break;
        }
      }
    }
  } while (productive_non_terminals.size() > num_productive_non_terminals);

  if (num_productive_non_terminals < non_terminals_r_.size()) {
    std::set<runtime::non_terminal> non_productive_non_terminals;

    BOOST_FOREACH(const runtime::non_terminal& non_terminal, non_terminals_r_)
      if (!productive_non_terminals.count(non_terminal.index()))
        non_productive_non_terminals.insert(non_terminal);

    throw non_productive_non_terminals_exception(non_productive_non_terminals);
  }
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::check_for_non_reachable_non_terminals()
  throw(non_reachable_non_terminals_exception&)
{
  // Create a collection of all reachable non-terminals
  std::set<symbol_index_type> reachable_non_terminals;
  reachable_non_terminals.insert(root_index::value);
  std::queue<symbol_index_type> non_terminals_to_process;
  non_terminals_to_process.push(root_index::value);

  // Process everything in the Queue until nothing is left
  while (!non_terminals_to_process.empty())
  {
    symbol_index_type to_process = non_terminals_to_process.front();
    non_terminals_to_process.pop();

    BOOST_FOREACH(const runtime::production::ptr& production,
          non_terminals_r_.find(to_process)->productions()) {
      BOOST_FOREACH(symbol_index_type symbol_index, production->produced()) {
        if (non_terminals_r_.count(symbol_index) &&
            !reachable_non_terminals.count(symbol_index)) {
          reachable_non_terminals.insert(symbol_index);
          non_terminals_to_process.push(symbol_index);
        }
      }
    }
  }

  if (reachable_non_terminals.size() < non_terminals_r_.size())
  {
    std::set<symbol_index_type> non_reachable_non_terminals;

    BOOST_FOREACH(const runtime::non_terminal& non_terminal, non_terminals_r_)
      if (!reachable_non_terminals.count(non_terminal.index()))
        non_reachable_non_terminals.insert(non_terminal.index());

    throw non_reachable_non_terminals_exception(non_reachable_non_terminals);
  }
}

template<typename R, typename T, typename P>
void grammar<R, T, P>::dump_productions(std::ostream& o) {
  BOOST_FOREACH(runtime::production::ptr production, productions_r_) {
    o << "production "<<production->index() << ":";
    BOOST_FOREACH(symbol_index_type symbol_index, production->produced()) {
      o << ' ' << symbol_index;
    }
    o << '\n';
  }
}

}

#endif // SHEAR__GRAMMAR_HPP

