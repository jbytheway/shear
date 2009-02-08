#ifndef SHEAR__NODES__SEQUENCE_HPP
#define SHEAR__NODES__SEQUENCE_HPP

#include <shear/produced.hpp>
#include <shear/pointer.hpp>

namespace shear { namespace nodes {

template<typename T>
class sequence {
  public:
    typedef typename mpl::vector<
      produced<>,
      produced<pointer<sequence>, pointer<T> >
    >::type shear_productions;

    sequence() {}

    sequence(boost::shared_ptr<sequence> h, boost::shared_ptr<T> t) {
      swap(h, head_);
      assert(head_);
      swap(t, tail_);
      assert(tail_);
    }

    template<typename OutputIterator>
    OutputIterator copy(OutputIterator o) {
      if (head_) {
        o = head_->copy(o);
        *o = *tail_;
        ++o;
      }
      return o;
    }

    operator std::vector<T>() const {
      std::vector<T> result;
      if (head_) {
        head_->copy(std::back_inserter(result));
        result.push_back(*tail_);
      }
      return result;
    }
  private:
    boost::shared_ptr<sequence> head_;
    boost::shared_ptr<T> tail_;
};

}}

#endif // SHEAR__NODES__SEQUENCE_HPP

