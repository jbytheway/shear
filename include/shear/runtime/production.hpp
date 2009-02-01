#ifndef SHEAR__RUNTIME__PRODUCTION_HPP
#define SHEAR__RUNTIME__PRODUCTION_HPP

#include <boost/shared_ptr.hpp>

#include <shear/core.hpp>

namespace shear { namespace runtime {

class production {
  public:
    typedef boost::shared_ptr<production> ptr;

    template<
      typename Index,
      typename ProductionIndexVector,
      typename SymbolIndexMap
    >
    production(Index, ProductionIndexVector, SymbolIndexMap) :
      index_(Index::value)
    {
      typedef typename mpl::first<
        typename mpl::at<ProductionIndexVector, Index>::type
      >::type Production;
      source_index_ =
          mpl::at<SymbolIndexMap, typename Production::source>::type::value;
      mpl::for_each<
        typename Production::produced,
        mpl::at<SymbolIndexMap, mpl::_1>
      >(px::push_back(px::ref(produced_), arg1));
    }

    size_t index() const { return index_; }
    size_t source_index() const { return source_index_; }
    const std::vector<size_t>& produced() const { return produced_; }
  private:
    size_t index_;
    size_t source_index_;
    std::vector<size_t> produced_;
};

}}

#endif // SHEAR__RUNTIME__PRODUCTION_HPP

