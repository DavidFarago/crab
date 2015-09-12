#include "../common.hpp"

using namespace std;
using namespace crab::analyzer;
using namespace crab::cfg_impl;
using namespace crab::domain_impl;

cfg_t prog (VariableFactory &vfac) 
{

  cfg_t cfg ("loop1_entry","ret");
  //cfg_t cfg ("loop1_entry");
  basic_block_t& loop1_entry = cfg.insert ("loop1_entry");
  basic_block_t& loop1_bb1   = cfg.insert ("loop1_bb1");
  basic_block_t& loop1_bb1_t = cfg.insert ("loop1_bb1_t");
  basic_block_t& loop1_bb1_f = cfg.insert ("loop1_bb1_f");
  basic_block_t& loop1_bb2   = cfg.insert ("loop1_bb2");
  basic_block_t& loop2_entry = cfg.insert ("loop2_entry");
  basic_block_t& loop2_bb1   = cfg.insert ("loop2_bb1");
  basic_block_t& loop2_bb1_t = cfg.insert ("loop2_bb1_t");
  basic_block_t& loop2_bb1_f = cfg.insert ("loop2_bb1_f");
  basic_block_t& loop2_bb2   = cfg.insert ("loop2_bb2");
  basic_block_t& ret         = cfg.insert ("ret");

  loop1_entry >> loop1_bb1;
  loop1_bb1 >> loop1_bb1_t; loop1_bb1 >> loop1_bb1_f;
  loop1_bb1_t >> loop1_bb2; loop1_bb2 >> loop1_bb1; loop1_bb1_f >> loop2_entry;

  loop2_entry >> loop2_bb1;
  loop2_bb1 >> loop2_bb1_t; loop2_bb1 >> loop2_bb1_f;
  loop2_bb1_t >> loop2_bb2; loop2_bb2 >> loop2_bb1; loop2_bb1_f >> ret;

  z_var i(vfac["i"]);
  z_var j(vfac["j"]);
  z_var k(vfac["k"]);

  loop1_entry.assign (i, 0);
  loop1_entry.assign (k, 30);
  loop1_bb1_t.assume (i <= 9);
  loop1_bb1_f.assume (i >= 10);
  loop1_bb2.add (i, i, 1);

  loop2_entry.assign (j, 0);
  loop2_bb1_t.assume (j <= 9);
  loop2_bb1_f.assume (j >= 10);
  loop2_bb2.add (j, j, 1);
  return cfg;
}


int main (int argc, char** argv )
{


  VariableFactory vfac;
  cfg_t cfg = prog (vfac);
  cfg.simplify ();
  cout << cfg << endl;

  const bool run_live = true;

  {
    NumFwdAnalyzer <cfg_t, interval_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    interval_domain_t inv = interval_domain_t::top ();
    a.Run (inv);
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg)
    {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t, dbm_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    dbm_domain_t inv = dbm_domain_t::top ();
    a.Run (inv);
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg)
    {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t, ric_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    ric_domain_t inv = ric_domain_t::top ();
    a.Run (inv);
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg)
    {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t, term_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    term_domain_t inv = term_domain_t::top ();
    a.Run (inv);
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg)
    {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  {
    NumFwdAnalyzer <cfg_t,sym_cst_intv_domain_t,VariableFactory>::type a (cfg,vfac,run_live);
    // Run fixpoint 
    sym_cst_intv_domain_t inv = sym_cst_intv_domain_t::top ();
    a.Run (inv);
    // Print invariants
    cout << "Invariants using " << inv.getDomainName () << "\n";
    for (auto &b : cfg) {
      auto inv = a [b.label ()];
      std::cout << get_label_str (b.label ()) << "=" << inv << "\n";
    }
  }

  return 0;
}
