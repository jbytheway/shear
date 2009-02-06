#ifndef SHEAR__PRODUCED_S_HPP
#define SHEAR__PRODUCED_S_HPP

#include <shear/make_production.hpp>
#include <shear/production_s.hpp>

namespace shear {

template<typename ProducedSequence>
class produced_s {
  public:
    typedef produced_s type;
};

template<typename Source, typename ProducedSequence>
struct make_production<Source, produced_s<ProducedSequence> > {
  typedef production_s<Source, ProducedSequence> type;
};

}

#endif // SHEAR__PRODUCED_S_HPP

