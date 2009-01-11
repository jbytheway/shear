#ifndef SHEAR__GRAMMAR_HAPP
#define SHEAR__GRAMMAR_HAPP

#include <shear/core.hpp>
#include <shear/detail/grammar_base.hpp>

namespace shear {

template<
  typename RootSymbol,
  typename Tokens,
  typename Productions
>
class grammar : public detail::grammar_base<RootSymbol, Tokens, Productions> {
  public:
};

}

#endif // SHEAR__GRAMMAR_HAPP

