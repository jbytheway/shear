#ifndef SHEAR__LOCATION_HPP
#define SHEAR__LOCATION_HPP

#include <ostream>

#include <shear/core.hpp>

namespace shear {

class location {
};

inline std::ostream& operator<<(std::ostream& o, const location&) {
  return o;
}

}

#endif // SHEAR__LOCATION_HPP

