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
#include "AbstractMySWESolver_ADERDG.h"

#include "kernels/aderdg/generic/Kernels.h"

#include "MySWESolver_ADERDG.h" // Have to include a proper declaration. Cannot use forward declared classes in static_cast.

#include <stdio.h>

// We define the constructor of the actual solver here in order to regenerate it easily.
SWE::MySWESolver_ADERDG::MySWESolver_ADERDG(
  const double maximumMeshSize,
  const int maximumMeshDepth,
  const int haloCells,
  const int regularisedFineGridLevels,
  const exahype::solvers::Solver::TimeStepping timeStepping,
  const int DMPObservables
)
  :
  AbstractMySWESolver_ADERDG::AbstractMySWESolver_ADERDG(
      maximumMeshSize,
      maximumMeshDepth,
      haloCells,
      regularisedFineGridLevels,
      timeStepping,
      DMPObservables
) {
}


SWE::AbstractMySWESolver_ADERDG::AbstractMySWESolver_ADERDG(
    const double maximumMeshSize,
    const int maximumMeshDepth,
    const int haloCells,
    const int regularisedFineGridLevels,
    const exahype::solvers::Solver::TimeStepping timeStepping,
    const int DMPObservables
)
  :
  exahype::solvers::ADERDGSolver(
      "MySWESolver_ADERDG",
      NumberOfVariables,
      NumberOfParameters,
      NumberOfGlobalObservables,
      Order+1,
      maximumMeshSize,
      maximumMeshDepth,
      haloCells,
      regularisedFineGridLevels,
      timeStepping,
      DMPObservables
) {
}

void SWE::AbstractMySWESolver_ADERDG::constantsToString(std::ostream& os) {
  // This string is used in the --version output to identify compile time constants
  os << "SWE::AbstractMySWESolver_ADERDG("
     << "nVar=" << NumberOfVariables << ", "
     << "nParam=" << NumberOfParameters << ", "
     << "Order=" << Order
     << ")";
}

void SWE::AbstractMySWESolver_ADERDG::abortWithMsg(const char* const msg) {
  // verbosily fail even without assertions turned on
  puts(msg);
  abort();
}

int SWE::AbstractMySWESolver_ADERDG::fusedSpaceTimePredictorVolumeIntegral(double* lduh, double* lQhbnd, double* lGradQhbnd, double* lFhbnd, double* const luh, const tarch::la::Vector<DIMENSIONS, double>& cellCentre, const tarch::la::Vector<DIMENSIONS, double>& cellSize, const double t ,const double dt,const bool addVolumeIntegralResultToUpdate) {
  constexpr int spaceBasisSize     = (Order+1)*(Order+1);
  constexpr int spaceTimeBasisSize = spaceBasisSize*(Order+1);
  constexpr int sizeLQi   = (NumberOfVariables+NumberOfParameters)*spaceTimeBasisSize;
  constexpr int sizeRhs   = sizeLQi;
  constexpr int sizeLFi   = (DIMENSIONS + 1)*NumberOfVariables*spaceTimeBasisSize;
  constexpr int sizeGradQ = DIMENSIONS*NumberOfVariables*spaceBasisSize;
  
  constexpr int sizeLQhi = (NumberOfVariables+NumberOfParameters)*spaceBasisSize;
  constexpr int sizeLFhi = (DIMENSIONS + 1)*NumberOfVariables*spaceBasisSize;

  
  constexpr int totalSize = sizeLQi + sizeRhs + sizeLFi + sizeGradQ +
                            sizeLQhi + sizeLFhi;

  
  double* block = new double[totalSize];
  double* memory = block;
      
  double* lQi   = memory; memory+=sizeLQi;
  double* rhs   = memory; memory+=sizeRhs;
  double* lFi   = memory; memory+=sizeLFi;
  double* gradQ = memory; memory+=sizeGradQ;
  
  double* lQhi = memory; memory+=sizeLQhi;
  double* lFhi = memory; memory+=sizeLFhi;
 const int picardIterations = kernels::aderdg::generic::c::spaceTimePredictorNonlinear<false, true, false, true, false, MySWESolver_ADERDG>(*static_cast<MySWESolver_ADERDG*>(this), lQhbnd, lGradQhbnd, lFhbnd, lQi, rhs, lFi, gradQ, lQhi, lFhi, luh, cellCentre, tarch::la::invertEntries(cellSize), t, dt);
 
  if ( addVolumeIntegralResultToUpdate ) {
    kernels::aderdg::generic::c::volumeIntegralNonlinear<true, true, false, NumberOfVariables, Order+1>(lduh,lFhi,cellSize); 
  } else { // directly add the result to the solution
    double tempUpdate[NumberOfVariables*spaceBasisSize] = {0.0};
    kernels::aderdg::generic::c::volumeIntegralNonlinear<true, true, false, NumberOfVariables, Order+1>(tempUpdate,lFhi,cellSize);
    addUpdateToSolution(luh,luh,tempUpdate,dt);
  }

  delete[] block;
  return picardIterations;
}

void SWE::AbstractMySWESolver_ADERDG::addUpdateToSolution(double* const luh,const double* const luhOld,const double* const lduh,const double dt) {
  kernels::aderdg::generic::c::solutionUpdate<MySWESolver_ADERDG>(*static_cast<MySWESolver_ADERDG*>(this),luh,luhOld,lduh,dt);
}

void SWE::AbstractMySWESolver_ADERDG::faceIntegral(double* const out/*may be solution or update vector*/,const double* const lFhbnd,const int direction, const int orientation,const tarch::la::Vector<DIMENSIONS-1,int>& subfaceIndex,const int levelDelta,const tarch::la::Vector<DIMENSIONS, double>& cellSize,const double dt,const bool addToUpdate) {
    
  constexpr int basisSize2 = (Order+1);
  constexpr int basisSize3 = basisSize2*(Order+1);
  constexpr int sizeUpdate = NumberOfVariables*basisSize3;

  if ( levelDelta > 0 ) {
    constexpr int sizeFlux = NumberOfVariables*basisSize2;
    double lFhbndCoarse[sizeFlux];
    kernels::aderdg::generic::c::faceUnknownsRestriction<NumberOfVariables,Order+1>(lFhbndCoarse,lFhbnd,subfaceIndex,levelDelta);
    tarch::la::Vector<DIMENSIONS,double> coarseGridCellSize = tarch::la::aPowI(levelDelta,3) * 1.0 * cellSize;
    
    kernels::aderdg::generic::c::faceIntegralNonlinear<NumberOfVariables, Order+1>(out,lFhbndCoarse,direction,orientation,coarseGridCellSize);
  } else {
    if ( addToUpdate ) {
      kernels::aderdg::generic::c::faceIntegralNonlinear<NumberOfVariables, Order+1>(out,lFhbnd,direction,orientation,cellSize);
    } else { // solution is only available in real cells, i.e. only here we can directly add the face integral result to the solution
      double lduh[sizeUpdate] = {0.0};                   // initialise with zeros
      kernels::aderdg::generic::c::faceIntegralNonlinear<NumberOfVariables, Order+1>(lduh,lFhbnd,direction,orientation,cellSize);
      addUpdateToSolution(out,out,lduh,dt);
    }
  }
  
}

void SWE::AbstractMySWESolver_ADERDG::riemannSolver(double* const FL,double* const FR,const double* const QL,const double* const QR,const double t,const double dt, const tarch::la::Vector<DIMENSIONS, double>& cellSize, const int direction, bool isBoundaryFace, int faceIndex) {
  assertion2(direction>=0,dt,direction);
  assertion2(direction<DIMENSIONS,dt,direction);
kernels::aderdg::generic::c::riemannSolverNonlinear<true,false, MySWESolver_ADERDG>(*static_cast<MySWESolver_ADERDG*>(this),FL,FR,QL,QR,t,dt,cellSize,direction);
}

void SWE::AbstractMySWESolver_ADERDG::boundaryConditions(double* const fluxIn,const double* const stateIn,const double* const gradStateIn, const double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellCentre,const tarch::la::Vector<DIMENSIONS,double>& cellSize,const double t,const double dt,const int direction,const int orientation) {
  constexpr int basisSize     = (Order+1);
  constexpr int sizeStateOut = (NumberOfVariables+NumberOfParameters)*basisSize;
  constexpr int sizeFluxOut  = NumberOfVariables*basisSize;
  
  constexpr int totalSize = sizeStateOut + sizeFluxOut;
  
  double* block = new double[totalSize];
  double* memory = block;

  double* stateOut = memory; memory+=sizeStateOut;
  double* fluxOut  = memory; memory+=sizeFluxOut;

  const int faceIndex = 2*direction+orientation;
    kernels::aderdg::generic::c::boundaryConditions<false, MySWESolver_ADERDG>(*static_cast<MySWESolver_ADERDG*>(this),fluxOut,stateOut,fluxIn,stateIn,gradStateIn,cellCentre,cellSize,t,dt,faceIndex,direction);
  if ( orientation==0 ) {
    double* FL = fluxOut; const double* const QL = stateOut;
    double* FR = fluxIn;  const double* const QR = stateIn;
  
    riemannSolver(FL,FR,QL,QR,t,dt,cellSize,direction,true,faceIndex);
  }
  else {
    double* FL = fluxIn;  const double* const QL = stateIn;
    double* FR = fluxOut; const double* const QR = stateOut;

    riemannSolver(FL,FR,QL,QR,t,dt,cellSize,direction,true,faceIndex);
  }
  delete[] block;
}

double SWE::AbstractMySWESolver_ADERDG::stableTimeStepSize(const double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellSize) {
  double d = kernels::aderdg::generic::c::stableTimeStepSize<MySWESolver_ADERDG,false>(*static_cast<MySWESolver_ADERDG*>(this),luh,cellSize);
  return d;
}
void SWE::AbstractMySWESolver_ADERDG::adjustSolution(double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellCentre,const tarch::la::Vector<DIMENSIONS,double>& cellSize,double t,double dt) {
  kernels::aderdg::generic::c::solutionAdjustment<MySWESolver_ADERDG>(*static_cast<MySWESolver_ADERDG*>(this),luh,cellCentre,cellSize,t,dt);
}

void SWE::AbstractMySWESolver_ADERDG::faceUnknownsProlongation(double* const lQhbndFine,double* const lFhbndFine,const double* const lQhbndCoarse,const double* const lFhbndCoarse,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS-1,int>& subfaceIndex) {
  kernels::aderdg::generic::c::faceUnknownsProlongation<NumberOfVariables,NumberOfParameters,Order+1>(lQhbndFine,lFhbndFine,lQhbndCoarse,lFhbndCoarse,coarseGridLevel,fineGridLevel,subfaceIndex);
}

void SWE::AbstractMySWESolver_ADERDG::volumeUnknownsProlongation(double* const luhFine,const double* const luhCoarse,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS,int>& subcellIndex) {
  kernels::aderdg::generic::c::volumeUnknownsProlongation<NumberOfVariables,NumberOfParameters,Order+1>(luhFine,luhCoarse,coarseGridLevel,fineGridLevel,subcellIndex);
}

void SWE::AbstractMySWESolver_ADERDG::volumeUnknownsRestriction(double* const luhCoarse,const double* const luhFine,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS,int>& subcellIndex) {
  kernels::aderdg::generic::c::volumeUnknownsRestriction<NumberOfVariables,NumberOfParameters,Order+1>(luhCoarse,luhFine,coarseGridLevel,fineGridLevel,subcellIndex);
}

void SWE::AbstractMySWESolver_ADERDG::flux(const double* const Q,double** const F) {
      abortWithMsg("flux: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

void SWE::AbstractMySWESolver_ADERDG::viscousFlux(const double* const Q,const double* const gradQ, double** const F) {
      abortWithMsg("viscous flux: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

void SWE::AbstractMySWESolver_ADERDG::viscousEigenvalues(const double* const Q,const int direction,double* const lambda) {
      abortWithMsg("viscous eigenvalues: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

void SWE::AbstractMySWESolver_ADERDG::algebraicSource(const tarch::la::Vector<DIMENSIONS, double>& x, double t, const double *const Q, double *S) {
      abortWithMsg("algebraicSource: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

    void SWE::AbstractMySWESolver_ADERDG::nonConservativeProduct(const double* const Q,const double* const gradQ,double* const BgradQ) {
      abortWithMsg("nonConservativeProduct: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

void SWE::AbstractMySWESolver_ADERDG::multiplyMaterialParameterMatrix(const double* const Q, double** const rhs) {
      abortWithMsg("multiplyMaterialParameterMatrix: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

void SWE::AbstractMySWESolver_ADERDG::pointSource(const double* const Q,const double* const x,const double t,const double dt, double* const forceVector,int n) {
      abortWithMsg("pointSource: If this operation is entered, you have activated the corresponding guard. Then you have to re-implement this routine, too." );
}

std::vector<double> SWE::AbstractMySWESolver_ADERDG::mapGlobalObservables(const double* const Q, const tarch::la::Vector<DIMENSIONS, double> &cellSize) const {
  return {};
}

std::vector<double> SWE::AbstractMySWESolver_ADERDG::resetGlobalObservables() const {
  return {};
}

void SWE::AbstractMySWESolver_ADERDG::reduceGlobalObservables(
	std::vector<double>& reducedGlobalObservables,
	const std::vector<double>& curGlobalObservables) const {
  // NOP
}



/**
 * Fallback implementations of joined functions. Users can either safely ignore this
 * or overwrite with their own implementations.
 **/
#include "kernels/fusedMethods.cpph"