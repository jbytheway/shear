#ifndef SHEAR__COMPILETIME__MAKE_VECTOR_EXCLUDING_HPP
#define SHEAR__COMPILETIME__MAKE_VECTOR_EXCLUDING_HPP

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/equal_to.hpp>

#include <shear/core.hpp>

namespace shear { namespace compiletime {

class missing;

template<
  typename TExclude,
  BOOST_PP_ENUM_BINARY_PARAMS(
    SHEAR_VECTOR_LIMIT,
    typename T, = compiletime::missing BOOST_PP_INTERCEPT
  )>
class make_vector_excluding {
  private:
    typedef mpl::vector<
        BOOST_PP_ENUM_PARAMS(SHEAR_VECTOR_LIMIT, T)
      > vector_with_junk;
  public:
    // Truncate it to exclude the missing and excluded entries
    typedef typename mpl::copy_if<
        vector_with_junk,
        mpl::not_<mpl::or_<
          boost::is_same<mpl::_1, compiletime::missing>,
          boost::is_same<mpl::_1, TExclude>
        > >
      >::type type;
};

}}

#endif // SHEAR__COMPILETIME__MAKE_VECTOR_EXCLUDING_HPP

