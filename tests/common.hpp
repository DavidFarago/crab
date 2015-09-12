#ifndef __TESTS_COMMON__
#define __TESTS_COMMON__

#include <crab/cfg/Cfg.hpp>
#include <crab/cfg/VarFactory.hpp>
#include <crab/common/types.hpp>
#include <crab/analysis/FwdAnalyzer.hpp>
#include <crab/analysis/Pointer.hpp>
#include <crab/domains/linear_constraints.hpp> 
#include <crab/domains/intervals.hpp>                      
#include <crab/domains/intervals_congruences.hpp>                      
#include <crab/domains/dbm.hpp>                      
#include <crab/domains/array_graph.hpp>                      
#include <crab/domains/array_smashing.hpp>
#include <crab/domains/var_packing_naive_dbm.hpp>                      
#include <crab/domains/naive_dbm.hpp>   
#include <crab/domains/symbolic_constant_domain.hpp>   
#include <crab/cfg/CfgBgl.hpp> 

namespace crab {

  namespace cfg_impl {

    using namespace cfg;
    using namespace std;

    template<> inline std::string get_label_str(std::string e) 
    { return e; }

    // A variable factory based on strings
    typedef cfg::var_factory_impl::StrVariableFactory VariableFactory;
    typedef typename VariableFactory::varname_t varname_t;

    // CFG
    typedef variable< z_number, varname_t >      z_var;
    typedef std::string                          basic_block_label_t;
    typedef Cfg< basic_block_label_t, varname_t> cfg_t;
    typedef cfg_t::basic_block_t                 basic_block_t;
  }

  namespace domain_impl {
    using namespace crab::cfg_impl;
    using namespace crab::domains; 
    using namespace ikos;
    typedef linear_constraint_system<z_number, varname_t> z_lin_cst_sys_t;
    typedef interval<z_number> z_interval_t;
    typedef bound<z_number> z_bound_t;
    // Numerical domains
    typedef interval_domain< z_number, varname_t > interval_domain_t;
    typedef interval_congruence_domain< z_number, varname_t > ric_domain_t;
    typedef DBM<z_number, varname_t> dbm_domain_t;
    typedef anti_unif<term::TDomInfo<z_number, varname_t, interval_domain_t> >::anti_unif_t term_domain_t;
    typedef anti_unif<term::TDomInfo<z_number, varname_t, dbm_domain_t> >::anti_unif_t term_dbm_t;
    typedef naive_dbm <z_number, varname_t> naive_dbm_domain_t;
    typedef var_packing_naive_dbm <z_number, varname_t> cgs_dbm_domain_t;
    typedef num_sym_constant_domain < interval_domain_t, z_number, varname_t> sym_cst_intv_domain_t;
    // Array domains
    typedef array_graph_domain<dbm_domain_t,
                               z_number, varname_t,
                               interval_domain_t> array_graph_domain_t;
    typedef array_smashing <interval_domain_t, z_number, varname_t> array_smashing_t;
  } 

}

#endif 
