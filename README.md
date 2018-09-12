 ELFELLI README
----------------

[![Build Status](https://travis-ci.org/cypheon/elfelli.svg?branch=master)](https://travis-ci.org/cypheon/elfelli)

This is Elfelli, a tool to calculate and visualize electric flux lines
around charged bodies.
To run Elfelli, gtkmm, at least version 2.8, needs to be installed. 


 COMPILING
-----------

To compile Elfelli, you need SCons. In many distributions you will also
need the development package of gtkmm (in Debian this is 'libgtkmm-2.4-dev').

To install the prerequisites, the following command should suffice:

    sudo apt install build-essential scons libgtkmm-2.4-dev


Compiling itself is very simple:

    scons


 INSTALLATION
--------------

To install Elfelli, simply type:

    scons install

By default, it is installed in '/usr/local'. If you want to change that
prefix, alter the install command to:

    scons install prefix=/install/prefix


 BUGS
------

If you find any, please report them at the GitHub project page:
https://github.com/cypheon/elfelli/issues
