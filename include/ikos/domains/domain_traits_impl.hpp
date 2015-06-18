/*******************************************************************************
 * Default implementations for non-standard abstract opperations
 *
 * We separate domain_traits.hpp from domain_traits_impl.hpp so that
 * we do not create cyclic dependencies when including headers. In
 * this way, abstract domains can also call domain_traits operations.
 *
 ******************************************************************************/


#ifndef IKOS_DOMAINS_TRAITS_IMPL_HPP
#define IKOS_DOMAINS_TRAITS_IMPL_HPP

#include <ikos/domains/domain_traits.hpp>

namespace ikos {

namespace domain_traits {

template <typename AbsNumDomain >
void normalize(AbsNumDomain& inv) {
}

template <typename AbsDomain, typename VariableName >
void expand (AbsDomain& inv, VariableName x, VariableName new_x) {
  // make a new copy of x but losing precision if relational domain
  inv.set (new_x , inv [x]);
}

template <typename AbsDomain, typename VariableName >
void array_init (AbsDomain& inv, VariableName arr) {
}

template <typename AbsDomain, typename VariableName >
void array_load (AbsDomain& inv, VariableName lhs, 
                 VariableName arr, VariableName idx) {
}

template <typename AbsDomain, typename VariableName >
void array_store (AbsDomain& inv, VariableName arr, 
                  VariableName idx, typename AbsDomain::linear_expression_t val,
                  bool is_singleton) {
}

}
} // end namespace ikos

#endif 