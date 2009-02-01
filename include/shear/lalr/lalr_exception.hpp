#ifndef SHEAR__LALR__LALR_EXCEPTION_HPP
#define SHEAR__LALR__LALR_EXCEPTION_HPP

#include <shear/lalr/conflict.hpp>

namespace shear { namespace lalr {

class lalr_exception : std::exception {
};

class lalr_conflict_exception : lalr_exception {
  public:
    lalr_conflict_exception(const std::vector<conflict>&) {}
};

}}

#endif // SHEAR__LALR__LALR_EXCEPTION_HPP

