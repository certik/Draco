#-----------------------------*-cmake-*----------------------------------------#
# file   parser/test/CMakeLists.txt
# author Kelly Thompson <kgt@lanl.gov>
# date   2012 Aug 1
# brief  Generate build project files for parser/test.
# note   Copyright (C) 2016-2019, Triad National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
project( parser_test CXX )

# ---------------------------------------------------------------------------- #
# Source files
# ---------------------------------------------------------------------------- #

file( GLOB test_sources *.cc )

# ---------------------------------------------------------------------------- #
# Build Unit tests
# ---------------------------------------------------------------------------- #

# These tests are scalar only
set( scalar_test_sources ${PROJECT_SOURCE_DIR}/tstParse_Table.cc )
list( REMOVE_ITEM test_sources ${scalar_test_sources} )
add_scalar_tests( SOURCES "${scalar_test_sources}" DEPS "Lib_parser")

#------------------------------------------------------------------------------#
# End parser/test/CMakeLists.txt
#------------------------------------------------------------------------------#
