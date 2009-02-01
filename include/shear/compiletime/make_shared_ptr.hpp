#ifndef SHEAR__COMPILETIME__MAKE_SHARED_PTR_HPP
#define SHEAR__COMPILETIME__MAKE_SHARED_PTR_HPP

namespace shear { namespace compiletime {

template<typename T>
struct make_shared_ptr {
  typedef boost::shared_ptr<T> type;
};

}}

#endif // SHEAR__COMPILETIME__MAKE_SHARED_PTR_HPP

