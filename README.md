# cpisync

This is the source code for set reconciliation from the CPISync project (http://nislab.bu.edu).
Current Version - 2.0.3

------------------------------


## References:

 The code, explanation, references, API, and a demo can be found on this
 web page.  If you use this work, please cite any relevant papers below.

The main theoretical bases for the approaches in this work are:
   * Y. Minsky, A. Trachtenberg, and R. Zippel,
    "Set Reconciliation with Nearly Optimal Communication Complexity",
    IEEE Transactions on Information Theory, 49:9.
    <http://ipsit.bu.edu/documents/ieee-it3-web.pdf>
    
   * Y. Minsky and A. Trachtenberg,
    "Scalable set reconciliation"
    40th Annual Allerton Conference on Communication, Control, and Computing, 2002.
    <http://ipsit.bu.edu/documents/BUTR2002-01.pdf>

Relevant applications and extensions can be found at:
   * D. Starobinski, A. Trachtenberg and S. Agarwal,
    "Efficient PDA synchronization"
    IEEE Transactions on Mobile Computing 2:1, pp. 40-51 (2003)
    <http://ipsit.bu.edu/documents/efficient_pda_web.pdf>
  
   * S. Agarwal, V. Chauhan and A. Trachtenberg,
    "Bandwidth efficient string reconciliation using puzzles"
    IEEE Transactions on Parallel and Distributed Systems 17:11,pp. 1217-1225 (2006)
    <http://ipsit.bu.edu/documents/puzzles_journal.pdf>

   *  M.G. Karpovsky, L.B. Levitin. and A. Trachtenberg,
    "Data verification and reconciliation with generalized error-control codes"
    IEEE Transactions on Information Theory 49:7, pp. 1788-1793 (2003).

   * More at <http://people.bu.edu/trachten>.

  Elements of the CPISync project code have been worked on, at various points, by:

  @ Ari Trachtenberg

  @ Sachin Agarwal 

  @ Paul Varghese

  @ Jiaxi Jin

  @ Jie Meng

  @ Alexander Smirnov

  @ [Eliezer Pearl](http://eliezer.tk) 
  
  @ Sean Brandenburg

------------------------------

## Compilation:

This project has the following dependencies:
   * [NTL](http://www.shoup.net/ntl/) - A library for doing Number Theory (>9.5) 
       - ptheads - may be required depending on how NTL is configured
       - gmp - may be required depending on how NTL is configured
   * [cppunit](http://cppunit.sourceforge.net/doc/cvs/index.html) - For testing
   * [cmake](https://cmake.org) - For building

------------------------------

## Installation:
 **MacOS & Linux** - Open the project in terminal and run the following commands:
  - `cmake`
  - `make`
  - `sudo make install`

 **Windows** - Not currently supported

------------------------------

## Use Instructions:
  -
  -
  -
  -
  -

------------------------------

Acknowledgments:  NSF

Ari Trachtenberg, trachten@bu.edu, Boston University
