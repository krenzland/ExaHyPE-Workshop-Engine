// This file is generated by the ExaHyPE toolkit.
// Please do not modify - it will be overwritten by the next
// ExaHyPE toolkit call.
// 
// ========================
//   www.exahype.eu
// ========================
#ifndef __AbstractMySWESolver_ADERDG_CLASS_HEADER__
#define __AbstractMySWESolver_ADERDG_CLASS_HEADER__

#include <ostream>
#include <algorithm>

#include "exahype/solvers/ADERDGSolver.h"

/**
 * We include Peano's assertion collection here.
 */
#include "tarch/Assertions.h"
#include "tarch/la/Vector.h"

namespace SWE {
  class MySWESolver_ADERDG;
  class AbstractMySWESolver_ADERDG;
}

class SWE::AbstractMySWESolver_ADERDG: public exahype::solvers::ADERDGSolver {
  public:
    static constexpr int Order                     = 3;
    static constexpr int NumberOfVariables         = 4;
    static constexpr int NumberOfParameters        = 0;
    static constexpr int NumberOfGlobalObservables = 0;
    static constexpr int NumberOfDMPObservables    = 0; // only of interest if this ADERDGSolver is a component of a LimitingADERDSolver 
    static constexpr int MaxPicardIterations       = Order+1;
    static constexpr bool UseMaxPicardIterations   = false;
    static constexpr double CFL                    = 0.9;

    // getters for the constexpr's ; TODO(Dominic): Who uses this?
    int    constexpr_getNumberOfVariables()  const { return NumberOfVariables; };
    int    constexpr_getNumberOfParameters() const { return NumberOfParameters; };
    int    constexpr_getOrder()              const { return Order; };
    double constexpr_getCFLNumber()          const { return CFL; };
    
    bool isLinear() const override { return false; }  
    bool isUseViscousFlux() const override { return false; }
    
    class VariableMetrics;
    class Variables;
    class ReadOnlyVariables;
    class Fluxes;
    class VariableShortcuts;
    class VariableMultiplicities;
    class VariableNames;
    
    AbstractMySWESolver_ADERDG(
      const double maximumMeshSize,
      const int maximumMeshDepth,
      const int haloCells,
      const int regularisedFineGridLevels,
      const exahype::solvers::Solver::TimeStepping timeStepping,
      const int DMPObservables
);
    
    int fusedSpaceTimePredictorVolumeIntegral(double* const lduh,double* const lQhbnd, double* lGradQhbnd, double* const lFhbnd,double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellCentre,const tarch::la::Vector<DIMENSIONS,double>& cellSize,const double t,const double dt,const bool addVolumeIntegralResultToUpdate) override;
    void addUpdateToSolution(double* const luh,const double* const luhOld,const double* const lduh,const double dt) override;
    void riemannSolver(double* const FL,double* const FR,const double* const QL,const double* const QR,const double t,const double dt, const tarch::la::Vector<DIMENSIONS, double>& cellSize,const int direction, bool isBoundaryFace, int faceIndex) override;
    void boundaryConditions(double* const fluxIn,const double* const stateIn,const double* const gradStateIn, const double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellCentre,const tarch::la::Vector<DIMENSIONS,double>& cellSize,const double t,const double dt,const int direction,const int orientation) override;
    void faceIntegral(double* const out,const double* const lFhbnd,const int direction, const int orientation,const tarch::la::Vector<DIMENSIONS-1,int>& subfaceIndex,const int levelDelta,const tarch::la::Vector<DIMENSIONS, double>& cellSize,const double dt,const bool addToUpdate) override;    
    double stableTimeStepSize(const double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellSize) override;
    void adjustSolution(double* const luh,const tarch::la::Vector<DIMENSIONS,double>& cellCentre,const tarch::la::Vector<DIMENSIONS,double>& cellSize,const double t,const double dt) override; 
    void faceUnknownsProlongation(double* const lQhbndFine,double* const lFhbndFine,const double* const lQhbndCoarse,const double* const lFhbndCoarse,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS-1,int>& subfaceIndex) override;
    void volumeUnknownsProlongation(double* const luhFine,const double* const luhCoarse,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS,int>& subcellIndex) override;
    void volumeUnknownsRestriction(double* const luhCoarse,const double* const luhFine,const int coarseGridLevel,const int fineGridLevel,const tarch::la::Vector<DIMENSIONS,int>& subcellIndex) override;

    static void constantsToString(std::ostream& os);
    static void abortWithMsg(const char* const msg);
      
    /**
     * Default implementation. Please overwrite.
     *
     * See superclass for function's semantics.
     */
    bool isPhysicallyAdmissible(
        const double* const                         solution,
        const double* const                         observablesMin,
        const double* const                         observablesMax,
        const bool                                  wasTroubledInPreviousTimeStep,
        const tarch::la::Vector<DIMENSIONS,double>& cellCentre,
        const tarch::la::Vector<DIMENSIONS,double>& cellSize,
        const double                                timeStamp) const override { return true; }
    
    /**
     * Default implementation. Please overwrite.
     *
     * See superclass for function's semantics.
     */
    bool vetoDiscreteMaximumPrincipleDecision(
        const double* const                         solution,
        const double* const                         localObservablesMin,
        const double* const                         localObservablesMax,
        const bool                                  wasTroubledInPreviousTimeStep,
        const tarch::la::Vector<DIMENSIONS,double>& cellCentre,
        const tarch::la::Vector<DIMENSIONS,double>& cellSize,
        const double                                timeStamp) const override { return false; /* do not veto DMP*/ } 
    
    /**
     * Default implementation. Please overwrite.
     *
     * See superclass for function's semantics.
     */
    void mapDiscreteMaximumPrincipleObservables(double* const observables,const double* const Q) const override {
      if (NumberOfDMPObservables>0) {
        std::copy_n(Q,NumberOfDMPObservables,observables);
      }
    }
    
    /**
     * Default implementation. Please overwrite.
     *
     * See superclass for function's semantics.
     */
    double getDiscreteMaximumPrincipleRelaxationParameter(
        const double& specifiedRelaxationParameter,
        const int&    observable,
        const double& localMin,
        const double& localMax,
        const double& boundaryMinPerObservable,
        const double& previousMax) const override {
       return specifiedRelaxationParameter;
    }

    std::vector<double> mapGlobalObservables(const double* const Q, const tarch::la::Vector<DIMENSIONS, double> &cellSize) const override;
    std::vector<double> resetGlobalObservables() const override;
    void reduceGlobalObservables(
            std::vector<double>& reducedGlobalObservables,
            const std::vector<double>& curGlobalObservables) const override;
    
    /**
     * Default implementation. Please overwrite.
     *
     * See superclass exahype::solvers::Solver for function's semantics.
     */
    int getGeometricLoadBalancingWeight(
        const tarch::la::Vector<DIMENSIONS,double>& cellCentre,
        const tarch::la::Vector<DIMENSIONS,double>& cellSize) override {
      return 1;
    }
    
    /**
     * @defgroup User PDE kernels
     */
    ///@{
    /**
     * Compute the eigenvalues of the flux tensor per coordinate direction \p d.
     *
     * \param[in] Q  the conserved variables associated with a quadrature node
     *               as C array (already allocated).
     * \param[in] d  the column of the flux vector (d=0,1,...,DIMENSIONS).
     * \param[inout] lambda the eigenvalues as C array (already allocated).
     */
    virtual void eigenvalues(const double* const Q,const int direction,double* const lambda) = 0;

    /**
     * Impose boundary conditions at a point on a boundary face
     * within the time interval [t,t+dt].
     *
     * \param[in]    x         the physical coordinate on the face.
     * \param[in]    t         the start of the time interval.
     * \param[in]    dt        the width of the time interval.
     * \param[in]    faceIndex indexing of the face (0 -- {x[0]=xmin}, 1 -- {x[1]=xmax}, 2 -- {x[1]=ymin}, 3 -- {x[2]=ymax}, and so on,
     *                         where xmin,xmax,ymin,ymax are the bounds of the cell containing point x.
     * \param[in]    d         the coordinate direction the face normal is pointing to.
     * \param[in]    QIn       the conserved variables at point x from inside of the domain
     *                         and time-averaged (over [t,t+dt]) as C array (already allocated).
     * \param[in]    FIn       the normal fluxes at point x from inside of the domain
     *                         and time-averaged (over [t,t+dt]) as C array (already allocated).
     * \param[inout] QOut      the conserved variables at point x from outside of the domain
     *                         and time-averaged (over [t,t+dt]) as C array (already allocated).
     * \param[inout] FOut      the normal fluxes at point x from outside of the domain
     *                         and time-averaged (over [t,t+dt]) as C array (already allocated).
     */
  virtual void boundaryValues(const double* const x,const double t,const double dt,const int faceIndex,const int direction,const double * const fluxIn,const double* const stateIn,const double* const gradStateIn, double *fluxOut,double* stateOut) = 0;

    /**
     * Adjust the conserved variables and parameters (together: Q) at a given time t at the (quadrature) point x.
     *
     * \note Use this function and ::useAdjustSolution to set initial conditions.
     *
     * \param[in]    x         the physical coordinate on the face.
     * \param[in]    t         the start of the time interval.
     * \param[in]    dt        the width of the time interval.
     * \param[inout] Q         the conserved variables (and parameters) associated with a quadrature point
     *                         as C array (already allocated).
     */
    virtual void adjustPointSolution(const double* const x,const double t,const double dt,double* const Q) {}
        
    /**
     * @TODO LR : document
     */
    virtual void multiplyMaterialParameterMatrix(const double* const Q, double** const rhs);
    
    /**
     * Compute a pointSource contribution.
     * 
     * @TODO(Leo): Please document
     */
    virtual void pointSource(const double* const Q,const double* const x,const double t,const double dt, double* const forceVector,int n);

    /**
     * Compute the Algebraic Sourceterms.
     * 
     * You may want to overwrite this with your PDE Source (algebraic RHS contributions).
     * However, in all schemes we have so far, the source-type contributions are
     * collected with non-conservative contributions into a fusedSource, see the
     * fusedSource method. From the kernels given with ExaHyPE, only the fusedSource
     * is called and there is a default implementation for the fusedSource calling
     * again seperately the nonConservativeProduct function and the algebraicSource
     * function.
     *
     * \param[in]    x         the physical coordinate
     * \param[in]    t         the start of the time interval.
     * \param[in]    Q the conserved variables (and parameters) associated with a quadrature point
     *                 as C array (already allocated).
     * \param[inout] S the source point as C array (already allocated).
     */
    virtual void algebraicSource(const tarch::la::Vector<DIMENSIONS, double>& x, double t, const double *const Q, double *S);
    
    /**
     * Compute the nonconservative term $B(Q) \nabla Q$.
     * 
     * This function shall return a vector BgradQ which holds the result
     * of the full term. To do so, it gets the vector Q and the matrix
     * gradQ which holds the derivative of Q in each spatial direction.
     * Currently, the gradQ is a continous storage and users can use the
     * kernels::icellSize2 class in order to compute the positions inside gradQ.
     *
     * @TODO: Check if the following is still right:
     * 
     * !!! Warning: BgradQ is a vector of size NumberOfVariables if you
     * use the ADER-DG kernels for nonlinear PDEs. If you use
     * the kernels for linear PDEs, it is a tensor with dimensions
     * Dim x NumberOfVariables.
     * 
     * \param[in]   Q   the vector of unknowns at the given position
     * \param[in]   gradQ   the gradients of the vector of unknowns,
     *                  stored in a linearized array.
     * \param[inout]  The vector BgradQ (extends nVar), already allocated. 
     *
     */

    virtual void nonConservativeProduct(const double* const Q,const double* const gradQ,double* const BgradQ);
    
    /**
     * Compute the conserved flux.
     * 
     * \param[in]  Q the conserved variabels (and parameters) associated with a
     *               quadrature point as C array.
     * \param[inout] F a C array with shape [nDim][nVars]. That is, this is an C list
     *               holding pointers to actual lists. Thus, the storage may be noncontinous.
     *               In any case, the storage has already been allocated.
     */
    virtual void flux(const double* const Q,double** const F);
    
    /**
     * Compute the flux with diffusive component.
     * 
     * \param[in]  Q the conserved variabels (and parameters) associated with a
     *               quadrature point as C array.
     *             gradQ the gradient of the conserved variabels (and parameters) associated
     *                   with a quadrature point as C array.
     * \param[inout] F a C array with shape [nDim][nVars]. That is, this is an C list
     *               holding pointers to actual lists. Thus, the storage may be noncontinous.
     *               In any case, the storage has already been allocated.
     */
    virtual void viscousFlux(const double* const Q, const double* const gradQ, double** const F);
    virtual void viscousEigenvalues(const double* const Q,const int direction,double* const lambda);
    ///@}
    
    //override the size of unused data storage to -1 to not allocate it
    int getTempPointForceSourcesSize() const {return -1;} //pointSource not required
    
};

#endif // __AbstractMySWESolver_ADERDG_CLASS_HEADER__