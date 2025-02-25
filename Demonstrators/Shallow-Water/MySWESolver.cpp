// This file is generated by the ExaHyPE toolkit.
// Please do not modify - it will be overwritten by the next
// ExaHyPE toolkit call.
// 
// ========================
//   www.exahype.eu
// ========================
// ==============================================
// Please do not change the implementations below
// ==============================================
#include "MySWESolver.h"

#include "kernels/limiter/generic/Limiter.h"

SWE::MySWESolver::MySWESolver(
        const double maximumMeshSize,
        const int maximumMeshDepth,
        const int haloCells,
        const int regularisedFineGridLevels,
        const exahype::solvers::Solver::TimeStepping timeStepping,
        const int DMPObservables,
        const double DMPRelaxationParameter,
        const double DMPDifferenceScaling
) :
  exahype::solvers::LimitingADERDGSolver::LimitingADERDGSolver(
      "MySWESolver",
    new SWE::MySWESolver_ADERDG(
      maximumMeshSize,maximumMeshDepth,haloCells,regularisedFineGridLevels,timeStepping,DMPObservables),
    new SWE::MySWESolver_FV(
      maximumMeshSize, timeStepping),
    DMPRelaxationParameter,
    DMPDifferenceScaling) {}

void SWE::MySWESolver::projectOnFVLimiterSpace(const double* const luh, double* const lim) const {
  kernels::limiter::generic::c::projectOnFVLimiterSpace<Order+1,NumberOfVariables+NumberOfParameters,GhostLayerWidth>(luh, lim);
}

void SWE::MySWESolver::projectOnDGSpace(const double* const lim, double* const luh) const {
  kernels::limiter::generic::c::projectOnDGSpace<Order+1,NumberOfVariables+NumberOfParameters,GhostLayerWidth>(lim, luh);
}

bool SWE::MySWESolver::discreteMaximumPrincipleAndMinAndMaxSearch(const double* const luh, double* const boundaryMinPerVariables, double* const boundaryMaxPerVariables) {
  return kernels::limiter::generic::c::discreteMaximumPrincipleAndMinAndMaxSearch<AbstractMySWESolver_ADERDG, NumberOfDMPObservables, GhostLayerWidth>(luh, *static_cast<AbstractMySWESolver_ADERDG*>(_solver.get()), _DMPMaximumRelaxationParameter, _DMPDifferenceScaling, boundaryMinPerVariables, boundaryMaxPerVariables);
}

void SWE::MySWESolver::findCellLocalMinAndMax(const double* const luh, double* const localMinPerVariables, double* const localMaxPerVariable) {
  kernels::limiter::generic::c::findCellLocalMinAndMax<AbstractMySWESolver_ADERDG, NumberOfDMPObservables>(luh, *static_cast<AbstractMySWESolver_ADERDG*>(_solver.get()), localMinPerVariables, localMaxPerVariable);
}
void SWE::MySWESolver::findCellLocalLimiterMinAndMax(const double* const lim, double* const localMinPerObservable, double* const localMaxPerObservable) {
  kernels::limiter::generic::c::findCellLocalLimiterMinAndMax<AbstractMySWESolver_ADERDG, NumberOfDMPObservables, GhostLayerWidth>(lim, *static_cast<AbstractMySWESolver_ADERDG*>(_solver.get()), localMinPerObservable,localMaxPerObservable);
}