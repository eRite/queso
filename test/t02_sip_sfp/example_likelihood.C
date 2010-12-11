//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
// 
// QUESO - a library to support the Quantification of Uncertainty
// for Estimation, Simulation and Optimization
//
// Copyright (C) 2008,2009,2010 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor, 
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-
// 
// $Id$
//
//--------------------------------------------------------------------------

#include <example_likelihood.h>

double likelihoodRoutine(
  const uqGslVectorClass& paramValues,
  const uqGslVectorClass* paramDirection,
  const void*             functionDataPtr,
  uqGslVectorClass*       gradVector,
  uqGslMatrixClass*       hessianMatrix,
  uqGslVectorClass*       hessianEffect)
{
  // Logic just to avoid warnings from INTEL compiler
  const uqGslVectorClass* aux1 = paramDirection;
  if (aux1) {};
  aux1 = gradVector;
  aux1 = hessianEffect;
  uqGslMatrixClass* aux2 = hessianMatrix;
  if (aux2) {};

  // Just checking: the user, at the application level, expects
  // vector 'paramValues' to have size 2.
  UQ_FATAL_TEST_MACRO(paramValues.sizeGlobal() != 2,
                      UQ_UNAVAILABLE_RANK,
                      "likelihoodRoutine()",
                      "paramValues vector does not have size 2");

  // Actual code
  //
  // This code exemplifies multiple Metropolis-Hastings solvers, each calling
  // this likelihood routine.
  //
  // In this simple example, only node 0 in each subenvironment does the job
  // even though there might be more than one node per subenvironment.
  //
  // In a more realistic situation, if the user is asking for multiple nodes per
  // subenvironment, then the model code in the qoi and likelihood routines
  // might really demand more than one node.
  //
  // Here we use 'env.subRank()' only. A realistic application might want to use
  // 'env.subComm()' or 'env.subComm().Comm()'
  double result = 0.;
  const uqBaseEnvironmentClass& env = paramValues.env();
  if (env.subRank() == 0) {
    const uqGslVectorClass& meanVector =
      *((likelihoodRoutine_DataType *) functionDataPtr)->meanVector;
    const uqGslMatrixClass& covMatrix  =
      *((likelihoodRoutine_DataType *) functionDataPtr)->covMatrix;

    uqGslVectorClass diffVec(paramValues - meanVector);

    result = scalarProduct(diffVec, covMatrix.invertMultiply(diffVec));
  }
  else {
    // Do nothing;
  }

#ifdef QUESO_EXPECTS_LN_LIKELIHOOD_INSTEAD_OF_MINUS_2_LN
  return -.5*result;
#else
  return result;
#endif
}