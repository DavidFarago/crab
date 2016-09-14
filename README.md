# Crab: A Language-Agnostic Engine for Static Analysis #

<img src="http://i.imgur.com/IDKhq5h.png" alt="crab logo" width=280 height=200 />

# Description #

Crab allows to perform static analysis of programs based on
[Abstract Interpretation](https://en.wikipedia.org/wiki/Abstract_interpretation).

Crab does not analyze directly a mainstream programming language such as
C, C++, or Java but instead it analyzes a simplified
Control-Flow-Graph (CFG) based language which is
language-independent. This can allow Crab analyzing different
programming languages assuming a translator to the CFG-based language
is available.

Crab has been designed to have two kind of users:

1.  Analysis/verification tools that want to compute invariants using
    abstract interpretation.

2.  Researchers on abstract interpretation who would like to
    experiment with new abstract domains and fixpoint iterators.

In spite of its simple design, Crab can scale with large real programs
and its CFG-based language is rich enough to represent programs with
loops, functions, pointers, etc.

The foundations of Crab is based on a collection of abstract domains
and fixpoint iterators built on the top of
[Ikos](http://ti.arc.nasa.gov/opensource/ikos/) (Inference Kernel for
Open Static Analyzers) developed by NASA Ames Research Center.

# Crab architecture #

![Crab Architecture](https://github.com/caballa/crab/blob/crab_language/Crab_arch.jpg?raw=true "Crab Architecture")

# Installation and Usage #

Crab is written in C++ and uses heavily the Boost library. The main
requirements are:

- C++ compiler supporting c++11
- Boost and GMP

To include Crab in your application you just need to include the
corresponding C++ header files located at the `include` directory and
make sure that you link your application with the Crab libraries
(`lib` directory). This repository contains a `CMakeLists.txt` that
you can adapt for your own needs.

The `tests` directory contains many examples of how to build CFGs and
compute invariants using different abstract domains. To install all
the tests via `CMake` type:

	mkdir build && cd build
    cmake -DENABLE_TESTS=ON -DCMAKE_INSTALL_PREFIX=run ../
    cmake --build . --target install 

and, for instance, to execute the test `tests/simple/test1.cc` type:

    cd run/tests/domains && ./test1

The Boxes and Apron domains require third-party libraries. To avoid
the burden to users who are not interested in those domains, the
installation of the libraries is optional.

If you want to use the BOXES domain then add `-DUSE_LDD=ON` option.

If you want to use the Apron library domains then add `-DUSE_APRON=ON` option.

# Example using C++ API#

Assume we want to perform static analysis on the following C-like
program:

```c
    int i,x,y;
	i=0;
	x=1;
	y=0;
	while (i < 100) {
		x=x+y;
		y=y+1;
		i=i+1;
	}	 
``` 

This is the C++ code to build the corresponding Crab CFG and run the
analysis using the Zones domain:

```c++
    // CFG-based language
    #include <crab/cfg/cfg.hpp>
    // Variable factory	
    #include <crab/cfg/var_factory.hpp>
    // Forward analyzer	
    #include <crab/analysis/fwd_analyzer.hpp>

    // linear expressions and constraints
    #include <crab/domains/linear_constraints.hpp>
    // Zones domain
    #include <crab/domains/split_dbm.hpp>

    typedef SplitDBM<z_number, varname_t> zones_domain_t;
    typedef num_fwd_analyzer<cfg_ref_t,zones_domain_t,str_variable_factory>::type analyzer_t;

    int main (int argc, char**argv) {
       // Declare variables i,x, and y
       str_variable_factory vfac;	
       z_var i (vfac ["i"]);
       z_var x (vfac ["x"]);
       z_var y (vfac ["y"]);
       // Create an empty CFG marking "entry" and "exit" are the labels
       // for the entry and exit blocks.
       cfg_t cfg ("entry","ret");
       // Add blocks
       basic_block_t& entry = cfg.insert ("entry");
       basic_block_t& bb1   = cfg.insert ("bb1");
       basic_block_t& bb1_t = cfg.insert ("bb1_t");
       basic_block_t& bb1_f = cfg.insert ("bb1_f");
       basic_block_t& bb2   = cfg.insert ("bb2");
       basic_block_t& ret   = cfg.insert ("ret");
       // Add control flow 
       entry >> bb1; bb1 >> bb1_t; bb1 >> bb1_f;
       bb1_t >> bb2; bb2 >> bb1; bb1_f >> ret;
       // Add statements
       entry.assign(i, 0);
       entry.assign(x, 1);
       entry.assign(y, 0);
       bb1_t.assume(i <= 99);
       bb1_f.assume(i >= 100);
       bb2.add(x,x,y);
       bb2.add(y,y,1);
       bb2.add(i,i,1);

       // Build an analyzer and run the zones domain
       analyzer_t a(cfg,vfac,...);
       a.Run(zones_domain_t::top());
       cout << "Invariants using " << zones_domain_t::getDomainName() << "\n";
	
       // Scan all CFG basic blocks and print the invariants that hold
       // at their entries
       for (auto &b : cfg) {
         auto inv = a[b.label()];
         cout << get_label_str(b.label()) << "=" << inv << "\n";
       }
	   return 0;
    }
```

The Crab output of this program, showing the invariants that hold at
the entry of each basic block, should be something like this:

    Invariants using SplitDBM
	
	entry={}
	bb1={i -> [0, 100], x -> [1, +oo], y -> [0, 100], y-i<=0, y-x<=0, i-x<=0, i-y<=0}
    bb1_t={i -> [0, 100], x -> [1, +oo], y -> [0, 100], y-i<=0, y-x<=0, i-x<=0, i-y<=0}
    bb1_f={i -> [0, 100], x -> [1, +oo], y -> [0, 100], y-i<=0, y-x<=0, i-x<=0, i-y<=0}
    bb2={i -> [0, 99], x -> [1, +oo], y -> [0, 99], y-i<=0, y-x<=0, i-x<=0, i-y<=0}
	ret={i -> [100, 100], x -> [100, +oo], y -> [100, 100], y-i<=0, y-x<=0, i-x<=0, i-y<=0}

# Example reading the CFG from a file#

[Temporary hack]: add manually  into `PYTHONPATH` the path to `tools/crabParser.py`.

Then, type `crab filename` where filename looks like:

    abs_domain : zones;
    decl : [x:int; y:int;];
    blocks {
      bb1[x:=0; y:=0;]
      bb2[]
      bb3[assume (1*x)<10;x := x + 1; y := y + 1;]
      bb4[assume (1*x)>=10;]
    }
    edges {bb1 ->bb2; bb2 -> bb3; bb3-> bb2; bb2->bb4;}

TODO:

- describe `filename` although it is quite self-explanatory.
- describe grammar


# Integrating Crab in other verification tools #

Check these projects:

- [Crab-Llvm](https://github.com/seahorn/crab-llvm) is a static
analyzer that infers invariants from LLVM-based languages using Crab.

- [SeaHorn](https://github.com/seahorn) is a verification framework
that uses Crab-Llvm to supply invariants to the back-end solvers.

# Licensing #

Crab is currently under a licensing process. Meanwhile, Crab cannot be
publicly distributed.

Ikos is distributed under NASA Open Source Agreement (NOSA)
Version 1.3 or later. See [Ikos_LICENSE.pdf](Ikos_LICENSE.pdf) for
details.

# Publications #

- "An Abstract Domain of Uninterpreted Functions" [(PDF)](http://www.clip.dia.fi.upm.es/~jorge/docs/terms-vmcai16.pdf). VMCAI'16.

- "Exploiting Sparsity in Difference-Bounds Matrices" [(PDF)](http://www.clip.dia.fi.upm.es/~jorge/docs/zones-SAS16.pdf). SAS'16.

