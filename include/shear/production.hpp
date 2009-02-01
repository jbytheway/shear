#ifndef SHEAR__PRODUCTION_HPP
#define SHEAR__PRODUCTION_HPP

#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>

#include <shear/compiletime/make_vector_excluding.hpp>

namespace shear {

namespace compiletime {
  struct no_symbol;
}

template<
  typename Source,
  BOOST_PP_ENUM_BINARY_PARAMS(
    SHEAR_PRODUCTION_LIMIT,
    typename Produced, = compiletime::no_symbol BOOST_PP_INTERCEPT
  )>
class production :
  public production_s<
    Source,
    typename compiletime::make_vector_excluding<
      compiletime::no_symbol,
      BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
    >::type
  >
{
  public:
    typedef production_s<
      Source,
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
class get_source<
    production<Source, BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)>
  > :
  public get_source<
    typename production<
      Source, BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
    >::base
  > {
};

}

#endif // SHEAR__PRODUCTION_HPP

