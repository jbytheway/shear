#ifndef SHEAR__RUNTIME__NON_TERMINAL_HPP
#define SHEAR__RUNTIME__NON_TERMINAL_HPP

#include <vector>

#include <shear/runtime/production.hpp>

namespace shear { namespace runtime {

class non_terminal {
  public:
    template<
      typename Index,
      typename ProductionMap,
      typename SymbolIndexVector
    >
    non_terminal(Index, const ProductionMap& p, SymbolIndexVector) :
      index_(Index::value),
      name_(
        typeid(typename mpl::at<SymbolIndexVector, Index>::type::first).name()
      ),
      produces_empty_(false)
    {
      std::pair<
        typename ProductionMap::iterator,
        typename ProductionMap::iterator
      > range = p.equal_range(index_);
      productions_.assign(range.first, range.second);
    }

    size_t index() const { return index_; }
    const std::string& human_readable_name() const { return name_; }
    const std::vector<production::ptr> productions() const {
      return productions_;
    }
    bool produces_empty() const { return produces_empty_; }

    void set_produces_empty() const { produces_empty_ = true; }
  private:
    size_t index_;
    std::string name_;
    std::vector<production::ptr> productions_;
    // HACK: Mutable because of these things being stored const in a
    // multi_index_container in shear::grammar
    mutable bool produces_empty_;
};

inline bool operator<(const non_terminal& l, const non_terminal& r) {
  return l.index() < r.index();
}

}}

#endif // SHEAR__RUNTIME__NON_TERMINAL_HPP

