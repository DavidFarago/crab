#include "../common.hpp"

using namespace std;
using namespace crab::analyzer;
using namespace crab::cfg_impl;
using namespace crab::domain_impl;

/* Example of how to build a CFG */
cfg_t prog (VariableFactory &vfac)  {

  // Definining program variables
  z_var i (vfac ["i"]);
  z_var x (vfac ["x"]);
  z_var y (vfac ["y"]);
  z_var z (vfac ["z"]);
  z_var w (vfac ["w"]);
  z_var s (vfac ["s"]);
  z_var t (vfac ["t"]);
  z_var nd1 (vfac ["nd1"]);
  z_var nd2 (vfac ["nd2"]);
  // entry and exit block
  cfg_t cfg ("entry","ret");
  // adding blocks
  basic_block_t& entry = cfg.insert ("entry");
  basic_block_t& bb1   = cfg.insert ("bb1");
  basic_block_t& bb1_t = cfg.insert ("bb1_t");
  basic_block_t& bb1_f = cfg.insert ("bb1_f");
  basic_block_t& bb2   = cfg.insert ("bb2");
  basic_block_t& ret   = cfg.insert ("ret");
  // adding control flow 
  entry >> bb1;
  bb1 >> bb1_t; bb1 >> bb1_f;
  bb1_t >> bb2; bb2 >> bb1; bb1_f >> ret;
  // adding statements
  entry.assign (i, 0);
  entry.assign (x, 5);
  entry.assign (y, 5);
  entry.assign (z, 3);
  entry.assign (w, 3);
  entry.assign (s, 0);
  entry.assign (t, 0);
  bb1_t.assume (i <= 99);
  bb1_f.assume (i >= 100);
  bb2.havoc(nd1.name());
  bb2.havoc(nd2.name());
  bb2.bitwise_and(x,x,nd1);
  bb2.bitwise_and(y,y,nd1);
  bb2.bitwise_or(z,z,nd1);
  bb2.bitwise_or(w,w,nd1);
  bb2.bitwise_xor(s,nd1,nd2);
  bb2.bitwise_xor(t,nd1,nd2);
  bb2.add(i, i, 1);

  return cfg;
}

/* Example of how to infer invariants from the above CFG */
int main (int argc, char** argv )
{
  VariableFactory vfac;
  cfg_t cfg = prog (vfac);
  cfg.simplify (); // this is optional
  cout << cfg << endl;
  const bool run_live = false;

  {
    NumFwdAnalyzer <cfg_t, interval_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    // Run fixpoint 
    interval_domain_t inv = interval_domain_t::top ();
    a.Run (inv);
    // Print invariants
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg) {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t, dbm_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    // Run fixpoint 
    dbm_domain_t inv = dbm_domain_t::top ();
    a.Run (inv);
    // Print invariants
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg) {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t, term_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    // Run fixpoint 
    term_domain_t inv = term_domain_t::top ();
    a.Run (inv);
    // Print invariants
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg) {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
    cout << "As linear constraints:\n" << endl;
    for (auto & b : cfg)
    {
      term_domain_t inv = a [b.label ()];
      term_domain_t::linear_constraint_system_t cst(inv.to_linear_constraint_system());
      std::cout << "  " << get_label_str (b.label ()) << "=" << cst << "\n";
    }
  }

  return 0;
}
