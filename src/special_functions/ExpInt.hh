//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   special_functions/ExpInt.hh
 * \author Paul Talbot
 * \date   Tue Jul 26 14:48:13 MDT 2011
 * \brief  Declare the ExpInt function templates.
 * \note   Copyright (C) 2016-2019 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//

#ifndef special_functions_ExpInt_hh
#define special_functions_ExpInt_hh

#include "ds++/config.h"

namespace rtt_sf {

//! Compute general exponential integral, order n, argument x \f$ E_n(x) \f$.
DLL_PUBLIC_special_functions double En(unsigned const n, double const x);

//! Compute exponential integral, argument x \f$ Ei(x) \f$.
DLL_PUBLIC_special_functions double Ei(double const x);
} // namespace rtt_sf

#endif //special_functions_ExpInt

//--------------------------------------------------------------------------//
// end of ExpInt.hh
//--------------------------------------------------------------------------//
