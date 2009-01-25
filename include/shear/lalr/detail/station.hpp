#ifndef SHEAR__LALR_DETAIL__STATION_HPP
#define SHEAR__LALR_DETAIL__STATION_HPP

#include <shear/automata/nfa.hpp>

namespace shear { namespace lalr { namespace detail {

class station : public automata::nfa::state {
  public:
    typedef boost::shared_ptr<station> ptr;

    station() {}
    station(const runtime::non_terminal&) {}
};

}}}

#endif // SHEAR__LALR_DETAIL__STATION_HPP

