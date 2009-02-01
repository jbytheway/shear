#ifndef SHEAR__LALR__CONFLICT_HPP
#define SHEAR__LALR__CONFLICT_HPP

namespace shear { namespace lalr {

class conflict {
  public:
    conflict(
        bool /*shift*/,
        const std::vector<runtime::production::ptr>& /*reductions*/
      )
    {}
};

}}

#endif // SHEAR__LALR__CONFLICT_HPP

