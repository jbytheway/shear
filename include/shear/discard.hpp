#ifndef SHEAR__DISCARD_HPP
#define SHEAR__DISCARD_HPP

#include <shear/argument_tags.hpp>
#include <shear/compiletime/tagged_symbol.hpp>

namespace shear {

template<typename Symbol>
struct discard {
};

namespace compiletime {

template<typename TaggedSymbol>
struct get_symbol<discard<TaggedSymbol> > {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol>
struct get_tag<discard<TaggedSymbol> > {
  typedef argument_tags::discard type;
};

}

}

#endif // SHEAR__DISCARD_HPP

