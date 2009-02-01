#ifndef SHEAR__CORE_HPP
#define SHEAR__CORE_HPP

#include <boost/spirit/home/phoenix/core/argument.hpp>

#ifndef SHEAR_LIMIT
#define SHEAR_LIMIT 10
#endif

#ifndef SHEAR_VECTOR_LIMIT
#define SHEAR_VECTOR_LIMIT SHEAR_LIMIT
#endif

#ifndef SHEAR_PRODUCTION_LIMIT
#define SHEAR_PRODUCTION_LIMIT SHEAR_LIMIT
#endif

namespace boost {
  namespace mpl {}
  namespace fusion {}
}

namespace shear {
  namespace mpl = boost::mpl;
  namespace fusion = boost::fusion;
  namespace px = boost::phoenix;

  using px::arg_names::arg1;
  using px::arg_names::arg2;
  using px::arg_names::arg3;
}

#endif // SHEAR__CORE_HPP

