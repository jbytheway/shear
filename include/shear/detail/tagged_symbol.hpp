#ifndef SHEAR__DETAIL__TAGGED_SYMBOL_HPP
#define SHEAR__DETAIL__TAGGED_SYMBOL_HPP

#include <shear/detail/argument_tag.hpp>
#include <shear/discard.hpp>

namespace shear { namespace detail {

template<typename TaggedSymbol>
struct get_symbol {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol>
struct get_symbol<discard<TaggedSymbol> > {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol>
struct get_tag {
  typedef argument_by_reference type;
};

template<typename TaggedSymbol>
struct get_tag<discard<TaggedSymbol> > {
  typedef discard_argument type;
};

}}

#endif // SHEAR__DETAIL__TAGGED_SYMBOL_HPP

