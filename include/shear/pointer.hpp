#ifndef SHEAR__POINTER_HPP
#define SHEAR__POINTER_HPP

#include <shear/argument_tags.hpp>
#include <shear/compiletime/tagged_symbol.hpp>

namespace shear {

template<typename Symbol>
struct pointer {
};

namespace compiletime {

template<typename TaggedSymbol>
struct get_symbol<pointer<TaggedSymbol> > {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol>
struct get_tag<pointer<TaggedSymbol>, void> {
  typedef argument_tags::by_pointer type;
};

}

}

#endif // SHEAR__POINTER_HPP

