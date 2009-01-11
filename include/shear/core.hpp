#ifndef SHEAR__CORE_HPP
#define SHEAR__CORE_HPP

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
}

namespace shear {
  namespace mpl = boost::mpl;
}

#endif // SHEAR__CORE_HPP

