#ifndef SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP
#define SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP

#include <boost/shared_ptr.hpp>

#include <shear/compiletime/argument_tag.hpp>

namespace shear { namespace compiletime {

struct argument_type {
  template<typename Args>
  struct result;

  template<typename Symbol>
  struct result<
    argument_type(const boost::shared_ptr<Symbol>&, argument_by_reference&)> {
    typedef const Symbol& type;
  };

  template<typename Symbol>
  struct result<
    argument_type(boost::shared_ptr<Symbol>, argument_by_reference)> {
    typedef const Symbol& type;
  };

  template<typename Symbol>
  const Symbol& operator()(
      const boost::shared_ptr<Symbol>& s, argument_by_reference&
    ) const {
    return *s;
  }

  template<typename Symbol>
  struct result<
    argument_type(const boost::shared_ptr<Symbol>&, discard_argument&)> {
    typedef void type;
  };

  template<typename Symbol>
  struct result<
    argument_type(const boost::shared_ptr<Symbol>, discard_argument)> {
    typedef void type;
  };
};

}}

#endif // SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP
