#ifndef SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP
#define SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP

#include <shear/argument_tags.hpp>

namespace shear { namespace compiletime {

template<typename TaggedSymbol>
struct get_symbol {
  typedef TaggedSymbol type;
};

template<typename TaggedSymbol, typename Enabler = void>
struct get_tag {
  typedef argument_tags::by_reference type;
};

template<typename TaggedSymbol>
struct get_tag<
  TaggedSymbol,
  typename boost::enable_if<
    typename boost::is_same<
      typename TaggedSymbol::shear_default_argument_tag,
      typename TaggedSymbol::shear_default_argument_tag
    >::type
  >::type
> {
  typedef typename TaggedSymbol::shear_default_argument_tag type;
};

}}

#endif // SHEAR__COMPILETIME__TAGGED_SYMBOL_HPP

