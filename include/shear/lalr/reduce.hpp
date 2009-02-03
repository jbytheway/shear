#if !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

#ifndef SHEAR__LALR__REDUCE_HPP
#define SHEAR__LALR__REDUCE_HPP

#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/algorithm/transformation/remove.hpp>

#include <shear/compiletime/argument_type.hpp>
#include <shear/lalr/any_symbol.hpp>

namespace shear { namespace lalr {

namespace detail {

template<typename T, typename ArgVec>
typename boost::enable_if<
  typename mpl::equal_to<
    typename mpl::size<ArgVec>::type,
    mpl::size_t<0>
  >::type,
  T*
>::type new_expand_vector(const ArgVec&) {
  return new T();
}

template<typename T, typename ArgVec>
typename boost::enable_if<
  typename mpl::equal_to<
    typename mpl::size<ArgVec>::type,
    mpl::size_t<1>
  >::type,
  T*
>::type new_expand_vector(const ArgVec& args) {
  return new T(fusion::at<mpl::size_t<0> >(args));
}

#define BOOST_PP_ITERATION_PARAMS_1 \
  (3, (2, SHEAR_PRODUCTION_LIMIT, "shear/lalr/reduce.hpp"))
#include BOOST_PP_ITERATE()

} // namespace detail

template<
  typename Grammar,
  typename Production,
  typename SymbolIndexType,
  typename Symbols
>
any_symbol<SymbolIndexType> reduce(const Symbols& symbols)
{
  typedef typename Production::argument_tags tags;
  typedef typename fusion::result_of::as_vector<tags>::type tags_vector_type;
  tags_vector_type tags_vector;
  typedef typename fusion::result_of::transform<
    const Symbols, tags_vector_type, compiletime::argument_type
  >::type transformed_symbols_type;
  transformed_symbols_type transformed_symbols(
      symbols, tags_vector, compiletime::argument_type()
    );
  typedef typename fusion::result_of::remove<
    transformed_symbols_type,
    void
  >::type filtered_symbols_type;
  filtered_symbols_type filtered_symbols(transformed_symbols);
  typedef typename Production::source result_type;
  boost::shared_ptr<result_type> result(
      detail::new_expand_vector<result_type>(
        fusion::as_vector(filtered_symbols)
      )
    );
  return any_symbol<SymbolIndexType>(
      result,
      mpl::at<typename Grammar::symbol_index_map, result_type>::type::value
    );
}

}}

#endif // SHEAR__LALR__REDUCE_HPP

#else // iterating

template<typename T, typename ArgVec>
typename boost::enable_if<
  typename mpl::equal_to<
    typename mpl::size<ArgVec>::type,
    mpl::size_t<BOOST_PP_ITERATION()>
  >::type,
  T*
>::type new_expand_vector(const ArgVec& args) {
  return new T(
#define BOOST_PP_LOCAL_MACRO(index) \
      fusion::at<mpl::size_t<index> >(args),
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_PP_SUB(BOOST_PP_ITERATION(), 2))
#include BOOST_PP_LOCAL_ITERATE()
      fusion::at<mpl::size_t<BOOST_PP_ITERATION()-1> >(args)
    );
}

#endif

