//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ode/Function_Traits.hh
 * \author Kent Budge
 * \date   Wed Aug 18 10:31:24 2004
 * \brief  Definition of class template Function_Traits
 * \note   Copyright (C) 2016-2019 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//

#ifndef ode_Function_Traits_hh
#define ode_Function_Traits_hh

#include "ds++/config.h"
#include <complex>

namespace rtt_ode {

//===========================================================================//
/*!
 * \class Function_Traits
 * \brief Traits of Function category
 *
 * The Function category includes all types that behave like a function of a
 * single value that returns a value.  Examples include <code> double
 * (*)(double)</code>, <code> double (*)(int)</code>, or <code>
 * complex<double> (*)(double)</code>. Many functor classes are also included
 * in this category.
 *
 * The default implementation takes its traits from the Function type itself.
 * Specializations must be defined for built-in types.  There are
 * specializations in rtt_ode for <code>double (*)(double)</code> and
 * <code>complex<double> (*)(double)</code>.
 * 
 * \arg \a Function A function type.
 */
//===========================================================================//

template <typename Function> class DLL_PUBLIC_ode Function_Traits {
public:
  // The following traits must be defined for all Function types:

  //! Type returned by Function::operator()
  typedef typename Function::return_type return_type;
};

//---------------------------------------------------------------------------//
//! Traits for ordinary function mapping double to double
template <> class Function_Traits<double (*)(double)> {
public:
  typedef double return_type;
};

#ifdef draco_isAIX
//---------------------------------------------------------------------------//
/*! Traits for ordinary function mapping const double to double
 *
 * AIX considers top const qualifiers significant in function
 * signatures. Everyone else, including the ISO standard, disregards top const
 * qualifiers.
 */
template <> class Function_Traits<double (*)(double const)> {
public:
  typedef double return_type;
};
#endif

//---------------------------------------------------------------------------//
//! Traits for ordinary function mapping double to complex
template <> class Function_Traits<std::complex<double> (*)(double)> {
public:
  typedef std::complex<double> return_type;
};

} // end namespace rtt_ode

#endif // ode_Function_Traits_hh

//---------------------------------------------------------------------------//
// end of ode/Function_Traits.hh
//---------------------------------------------------------------------------//
