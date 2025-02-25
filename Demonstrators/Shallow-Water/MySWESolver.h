#ifndef __MySWESolver_CLASS_HEADER__
#define __MySWESolver_CLASS_HEADER__

// This file is generated by the ExaHyPE toolkit.
// Please do not modify - it will be overwritten by the next
// ExaHyPE toolkit call.
// 
// ========================
//   www.exahype.eu
// ========================

#include <string>

#include "exahype/solvers/LimitingADERDGSolver.h"
#include "MySWESolver_ADERDG.h"
#include "MySWESolver_FV.h"

namespace SWE{
  class MySWESolver;
}

class SWE::MySWESolver: public exahype::solvers::LimitingADERDGSolver {  
  public:
    static constexpr int NumberOfVariables         = SWE::AbstractMySWESolver_ADERDG::NumberOfVariables;
    static constexpr int NumberOfParameters        = SWE::AbstractMySWESolver_ADERDG::NumberOfParameters;
    static constexpr int Order                     = SWE::AbstractMySWESolver_ADERDG::Order;
    static constexpr int NumberOfGlobalObservables = SWE::AbstractMySWESolver_ADERDG::NumberOfGlobalObservables;
    static constexpr int NumberOfDMPObservables    = SWE::AbstractMySWESolver_ADERDG::NumberOfDMPObservables;
    static constexpr int GhostLayerWidth           = SWE::AbstractMySWESolver_FV::GhostLayerWidth;
      
    MySWESolver(
        const double maximumMeshSize,
        const int maximumMeshDepth,
        const int haloCells,
        const int regularisedFineGridLevels,
        const exahype::solvers::Solver::TimeStepping timeStepping,
        const int DMPObservables,
        const double DMPRelaxationParameter,
        const double DMPDifferenceScaling
);
    
    void projectOnFVLimiterSpace(const double* const luh, double* const lim) const override;
    void projectOnDGSpace(const double* const lim, double* const luh) const override;
    bool discreteMaximumPrincipleAndMinAndMaxSearch(const double* const luh, double* const boundaryMinPerVariables, double* const boundaryMaxPerVariables) override;
    void findCellLocalMinAndMax(const double* const luh, double* const localMinPerVariables, double* const localMaxPerVariable) override;
    void findCellLocalLimiterMinAndMax(const double* const lim, double* const localMinPerObservable, double* const localMaxPerObservable) override;
};

#endif // __MySWESolver_CLASS_HEADER__