#ifndef SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP
#define SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP

#include <boost/shared_ptr.hpp>

#include <shear/argument_tags.hpp>

namespace shear { namespace compiletime {

struct argument_type {
  template<typename Args>
  struct result;

  template<typename Symbol>
  struct result<
    argument_type(
        const boost::shared_ptr<Symbol>&,
        argument_tags::by_reference&
      )> {
    typedef const Symbol& type;
  };

  template<typename Symbol>
  struct result<
    argument_type(boost::shared_ptr<Symbol>, argument_tags::by_reference)> {
    typedef const Symbol& type;
  };

  template<typename Symbol>
  const Symbol& operator()(
      const boost::shared_ptr<Symbol>& s, argument_tags::by_reference&
    ) const {
    return *s;
  }

  template<typename Symbol>
  struct result<
    argument_type(
        const boost::shared_ptr<Symbol>&,
        argument_tags::by_pointer&
      )> {
    typedef const boost::shared_ptr<Symbol>& type;
  };

  template<typename Symbol>
  struct result<
    argument_type(boost::shared_ptr<Symbol>, argument_tags::by_pointer)> {
    typedef const boost::shared_ptr<Symbol>& type;
  };

  template<typename Symbol>
  const boost::shared_ptr<Symbol>& operator()(
      const boost::shared_ptr<Symbol>& s, argument_tags::by_pointer&
    ) const {
    return s;
  }

  template<typename Symbol>
  struct result<
    argument_type(const boost::shared_ptr<Symbol>&, argument_tags::discard&)> {
    typedef void type;
  };

  template<typename Symbol>
  struct result<
    argument_type(const boost::shared_ptr<Symbol>, argument_tags::discard)> {
    typedef void type;
  };
};

}}

#endif // SHEAR__COMPILETIME__ARGUMENT_TYPE_HPP

