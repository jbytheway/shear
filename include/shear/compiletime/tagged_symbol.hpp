#ifndef SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP
#define SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP

#include <shear/compiletime/argument_tag.hpp>
#include <shear/discard.hpp>

namespace shear { namespace compiletime {

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

#endif // SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP

