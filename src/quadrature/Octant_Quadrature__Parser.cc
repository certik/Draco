//----------------------------------*-C++-*----------------------------------------------//
/*!
 * \file   quadrature/Octant_Quadrature.cc
 * \author Kelly Thompson
 * \date   Wed Sep  1 10:19:52 2004
 * \brief  
 * \note   Copyright (C) 2016-2019 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------------------//
// $Id: Octant_Quadrature.cc 6718 2012-08-30 20:03:01Z warsa $
//---------------------------------------------------------------------------------------//

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "Octant_Quadrature.hh"

#include "parser/utilities.hh"

namespace rtt_quadrature {
using namespace rtt_parser;

//---------------------------------------------------------------------------------------//
/*!
 * Used in conjuction with child parse routines for common features
 */

/*static*/
void Octant_Quadrature::parse(Token_Stream &tokens, bool &has_axis_assignments,
                              unsigned &mu_axis, unsigned &eta_axis) {
  Token token = tokens.shift();

  has_axis_assignments = false;
  if (token.text() == "axis assignments") {
    has_axis_assignments = true;

    token = tokens.shift();
    tokens.check_syntax(token.text() == "mu", "expected mu");
    mu_axis = parse_unsigned_integer(tokens);
    tokens.check_semantics(mu_axis < 3, "mu axis must be 0, 1, or 2");

    token = tokens.shift();
    tokens.check_syntax(token.text() == "eta", "expected eta");
    eta_axis = parse_unsigned_integer(tokens);
    tokens.check_semantics(eta_axis < 3 && eta_axis != mu_axis,
                           "eta axis must be 0, 1, or 2 and differ from mu");
    token = tokens.shift();
  }

  tokens.check_syntax(token.type() == END, "missing end?");
}

} // end namespace rtt_quadrature

//---------------------------------------------------------------------------------------//
// end of Octant_Quadrature.cc
//---------------------------------------------------------------------------------------//
