#ifndef SHEAR__MAKE_GRAMMAR_HAPP
#define SHEAR__MAKE_GRAMMAR_HAPP

#include <shear/grammar.hpp>

namespace shear {

template<
  typename RootSymbol,
  typename Tokens,
  typename Productions
>
struct make_grammar {
  typedef grammar<RootSymbol, Tokens, Productions> type;
};

}

#endif // SHEAR__MAKE_GRAMMAR_HAPP

