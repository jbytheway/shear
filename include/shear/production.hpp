#ifndef SHEAR__PRODUCTION_HPP
#define SHEAR__PRODUCTION_HPP

#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>

#include <shear/detail/make_vector_excluding.hpp>

namespace shear {

namespace detail {
  struct no_symbol;
}

template<
  typename Source,
  BOOST_PP_ENUM_BINARY_PARAMS(
    SHEAR_PRODUCTION_LIMIT,
    typename Produced, = detail::no_symbol BOOST_PP_INTERCEPT
  )>
class production : 
  production_s<
    Source,
    typename detail::make_vector_excluding<
      detail::no_symbol,
      BOOST_PP_ENUM_PARAMS(SHEAR_PRODUCTION_LIMIT, Produced)
    >::type
  >
{
};

}

#endif // SHEAR__PRODUCTION_HPP

