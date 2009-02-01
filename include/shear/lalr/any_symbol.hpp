#ifndef SHEAR__LALR__ANY_SYMBOL_HPP
#define SHEAR__LALR__ANY_SYMBOL_HPP

#include <boost/type_traits/is_void.hpp>
#include <boost/mpl/assert.hpp>

#include <shear/core.hpp>

namespace shear { namespace lalr {

template<typename SymbolIndexType>
class any_symbol {
  public:
    typedef SymbolIndexType symbol_index_type;

    any_symbol(const any_symbol& copy) :
      erased_symbol_(copy.erased_symbol_->clone()),
      index_(copy.index_)
    {}

    template<typename Symbol>
    any_symbol(boost::shared_ptr<Symbol> symbol, symbol_index_type index) :
      erased_symbol_(new type_eraser<Symbol>(symbol)),
      index_(index)
    {
      // TODO: extract location information where possible
    }

    any_symbol& operator=(any_symbol copy) {
      swap(*this, copy);
      return *this;
    }

    symbol_index_type index() const { return index_; }

    template<typename Symbol>
    void reset(boost::shared_ptr<Symbol> symbol, symbol_index_type index) {
      erased_symbol_.reset(new type_eraser<Symbol>(symbol));
      index_ = index;
    }

    template<typename Symbol>
    boost::shared_ptr<Symbol> get_symbol() const {
      return dynamic_cast<type_eraser<Symbol>*>(erased_symbol_.get())->p;
    }

    const location& start() const { return start_; }

    friend inline void swap(any_symbol& l, any_symbol& r) {
      using std::swap;
      swap(l.erased_symbol_, r.erased_symbol_);
      swap(l.index_, r.index_);
    }
  private:
    struct type_erased {
      public:
        virtual ~type_erased() = 0;
        virtual type_erased* clone() = 0;
      protected:
        type_erased() {}
        type_erased(const type_erased&) {}
        type_erased& operator=(const type_erased&) {}
    };

    template<typename T>
    struct type_eraser : public type_erased {
      type_eraser(const boost::shared_ptr<T>& pt) : p(pt) {}
      virtual type_erased* clone() {
        return new type_eraser(*this);
      }
      boost::shared_ptr<T> p;
    };

    boost::scoped_ptr<type_erased> erased_symbol_;
    symbol_index_type index_;
    location start_;
};

template<typename S>
inline any_symbol<S>::type_erased::~type_erased() {}

}}

#endif // SHEAR__LALR__ANY_SYMBOL_HPP

