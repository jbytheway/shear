#ifndef SHEAR__COMPILETIME__GET_INTRINSIC_PRODUCTIONS_HPP
#define SHEAR__COMPILETIME__GET_INTRINSIC_PRODUCTIONS_HPP

#include <boost/utility/enable_if.hpp>

#include <shear/make_production.hpp>

namespace shear { namespace compiletime {

template<typename NonTerminal, typename ShearProductions = void>
struct get_intrinsic_productions {
  typedef mpl::vector0<> type;
};

template<typename NonTerminal>
struct get_intrinsic_productions<
  NonTerminal,
  typename boost::enable_if<
    typename boost::is_same<
      typename NonTerminal::shear_productions,
      typename NonTerminal::shear_productions
    >::type
  >::type
>{
  typedef typename mpl::fold<
      typename NonTerminal::shear_productions,
      mpl::vector0<>,
      mpl::push_back<mpl::_1, make_production<NonTerminal, mpl::_2> >
    >::type type;
};

}}

#endif // SHEAR__COMPILETIME__GET_INTRINSIC_PRODUCTIONS_HPP

