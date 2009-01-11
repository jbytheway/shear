#ifndef SHEAR__RUNTIME__NON_TERMINAL_HPP
#define SHEAR__RUNTIME__NON_TERMINAL_HPP

#include <vector>

#include <shear/runtime/production.hpp>

namespace shear { namespace runtime {

class non_terminal {
  public:
    template<typename ProductionsMap>
    non_terminal(size_t i, const ProductionsMap& p) :
      index_(i), produces_empty_(false)
    {
      std::pair<
        typename ProductionsMap::iterator,
        typename ProductionsMap::iterator
      > range = p.equal_range(i);
      productions_.assign(range.first, range.second);
    }

    size_t index() const { return index_; }
    const std::vector<production::ptr> productions() const {
      return productions_;
    }
    bool produces_empty() const { return produces_empty_; }

    void set_produces_empty() const { produces_empty_ = true; }
  private:
    size_t index_;
    std::vector<production::ptr> productions_;
    // HACK: Mutable because of these things being stored const in a
    // multi_index_container in shear::grammar
    mutable bool produces_empty_;
};

}}

#endif // SHEAR__RUNTIME__NON_TERMINAL_HPP

