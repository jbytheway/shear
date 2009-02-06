#ifndef SHEAR__PRODUCED_HPP
#define SHEAR__PRODUCED_HPP

#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>

#include <shear/produced_s.hpp>
#include <shear/compiletime/make_vector_excluding.hpp>

namespace shear {

namespace compiletime {
  struct no_symbol;
}

template<BOOST_PP_ENUM_BINARY_PARAMS(
    SHEAR_PRODUCTION_LIMIT,
    typename Produced, = compiletime::no_symbol BOOST_PP_INTERCEPT
  )>
class produced :
  public produced_s<
   typename compiletime::make_vector_excluding<
      compiletime::no_symbol,
      BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
    >::type
  >
{
  public:
    typedef produced_s<
      typename compiletime::make_vector_excluding<
        compiletime::no_symbol,
        BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
      >::type
    > base;
};

template<
  typename Source,
  BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, typename Produced)
>
struct make_production<
  Source,
  produced<BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)>
> {
  typedef typename make_production<
      Source,
      typename produced<
        BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
      >::base
    >::type type;
};

}

#endif // SHEAR__PRODUCED_HPP

