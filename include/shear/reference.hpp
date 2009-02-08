#ifndef SHEAR__REFERENCE_HPP
#define SHEAR__REFERENCE_HPP

#include <shear/argument_tags.hpp>
#include <shear/compiletime/tagged_symbol.hpp>

namespace shear {

template<typename Symbol>
struct reference {
};

namespace compiletime {

template<typename TaggedSymbol>
struct get_symbol<reference<TaggedSymbol> > {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol>
struct get_tag<reference<TaggedSymbol>, void> {
  typedef argument_tags::by_reference type;
};

}

}

#endif // SHEAR__REFERENCE_HPP

