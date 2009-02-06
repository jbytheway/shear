#ifndef SHEAR__MAKE_PRODUCTION_HPP
#define SHEAR__MAKE_PRODUCTION_HPP

#include <shear/core.hpp>

namespace shear {

template<typename Source, typename Produced>
struct make_production {
  BOOST_STATIC_ASSERT((sizeof(Produced) == 0));
};

}

#endif // SHEAR__MAKE_PRODUCTION_HPP

