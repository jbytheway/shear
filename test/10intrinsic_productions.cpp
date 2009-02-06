#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/mpl/vector.hpp>

#include <shear/compiletime/get_intrinsic_productions.hpp>
#include <shear/produced.hpp>
#include <shear/nodes/sequence.hpp>

namespace sh = shear;
namespace mpl = boost::mpl;

namespace intrinsic_symbols {

class NoIntrinsic {
  // Nothing
};

class OneIntrinsic {
  public:
    typedef mpl::vector<
      sh::produced<>
    >::type shear_productions;
};

}

using namespace intrinsic_symbols;

BOOST_AUTO_TEST_CASE(get_intrinsic)
{
  typedef sh::compiletime::get_intrinsic_productions<NoIntrinsic>::type S0;
  BOOST_MPL_ASSERT((mpl::empty<S0>::type));
  typedef sh::compiletime::get_intrinsic_productions<OneIntrinsic>::type S1;
  BOOST_MPL_ASSERT_RELATION(mpl::size<S1>::type::value,==,1);
  BOOST_MPL_ASSERT((
        boost::is_same<
          sh::make_production<OneIntrinsic, sh::produced<> >::type,
          sh::production_s<OneIntrinsic, mpl::vector0<> >
        >::type
      ));
  BOOST_MPL_ASSERT((
        boost::is_same<
          mpl::front<S1>::type,
          sh::production_s<OneIntrinsic, mpl::vector0<> >
        >::type
      ));
  typedef sh::compiletime::get_intrinsic_productions<
    sh::nodes::sequence<NoIntrinsic>
  >::type S2;
  BOOST_MPL_ASSERT_RELATION(mpl::size<S2>::type::value,==,2);
}

