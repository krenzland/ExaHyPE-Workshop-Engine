// This file is generated by the ExaHyPE toolkit.
// Please do not modify - it will be overwritten by the next
// ExaHyPE toolkit call.
// 
// ========================
//   www.exahype.eu
// ========================
#include <sstream>
#include <ostream>
#include <memory>
#include <array>
#include <unistd.h> // POSIX pipes
#include <stdio.h>
#include <string>

#include "tarch/parallel/Node.h"

#include "exahype/plotters/Plotter.h"
#include "exahype/profilers/ProfilerFactory.h"
#include "exahype/solvers/Solver.h"
#include "exahype/solvers/SolverCoupling.h"
#include "kernels/KernelCalls.h"

#include "kernels/GaussLegendreQuadrature.h"
#include "kernels/GaussLobattoQuadrature.h"
#include "kernels/LimiterProjectionMatrices.h"
#include "kernels/DGMatrices.h"
#include "kernels/DGBasisFunctions.h"
#include "buildinfo.h"


// includes for solver 'MySWESolver'
#include "MySWESolver.h"
#include "ConservedWriter.h"


void kernels::registerSolvers(exahype::parser::Parser& parser) {
  // register solver 'MySWESolver' (type: 'Limiting-ADER-DG')
  {
    exahype::solvers::RegisteredSolvers.push_back(
      new SWE::MySWESolver(
          parser.getMaximumMeshSize(0),
          parser.getMaximumMeshDepth(0),
          parser.getHaloCells(0),
          parser.getRegularisedFineGridLevels(0),
          parser.getTimeStepping(0),
          parser.getDMPObservables(0),
          parser.getDMPRelaxationParameter(0),
          parser.getDMPDifferenceScaling(0)
      )
    );
    parser.checkSolverConsistency(0);
    
    // register plotter 'ConservedWriter'
    exahype::plotters::RegisteredPlotters.push_back( 
      new exahype::plotters::Plotter(0,0,parser, new SWE::ConservedWriter(
          *static_cast<SWE::MySWESolver*>(exahype::solvers::RegisteredSolvers[0])) 
      )
    );
  }
  std::set<int> orders;
  for (const auto p : exahype::solvers::RegisteredSolvers) {
    orders.insert(p->getNodesPerCoordinateAxis()-1);
  }
  kernels::initGaussLegendreNodesAndWeights(orders);
  kernels::initGaussLobattoNodesAndWeights(orders);
  kernels::initLimiterProjectionMatrices(orders);
  kernels::initDGMatrices(orders);
  kernels::initBasisFunctions(orders);
}

void kernels::toString(std::ostream& ostream) {
  /* Generated solver registration code by the toolkit */
  ostream << "inputFileName:  Demonstrators/Shallow-Water/ShallowWater.exahype\n";
  ostream << "projectName:    SWE\n";
  ostream << "Solver[0].type:             Limiting-ADER-DG\n";
  ostream << "Solver[0].class:            SWE::MySWESolver\n";
  ostream << "Solver[0].class[ADERDG]:    SWE::MySWESolver_ADERDG\n";
  ostream << "Solver[0].abstract[ADERDG]: ";
  SWE::MySWESolver_ADERDG::constantsToString(ostream);
  ostream << "Solver[0].class[FV]:        SWE::MySWESolver_FV\n";
  ostream << "Solver[0].abstract[FV]: ";
  SWE::MySWESolver_FV::constantsToString(ostream);
  ostream << "Solver[0].variables:           [ h : 1, hu : 1, hv : 1, b : 1 ]\n";
  ostream << "Solver[0].material_parameters: [ 0 ]\n";
  ostream << "Solver[0].global_observables:  [ 0 ]\n";
  ostream << "Solver[0].Plotter[0]: SWE::ConservedWriter(variables=6)\n";
 
// TODO not used yet
//  ostream << "\n";
//  ostream << "Kernel[0].hasConstants: true\n";
//  ostream << "Kernel[0].Plotter[0]: Euler::ErrorPlotter(variables=15)\n";
}

void kernels::finalise() {
  std::set<int> orders;
  for (const auto p : exahype::solvers::RegisteredSolvers) {
    orders.insert(p->getNodesPerCoordinateAxis()-1);
  }
  kernels::freeGaussLegendreNodesAndWeights(orders);
  kernels::freeGaussLobattoNodesAndWeights(orders);
  kernels::freeLimiterProjectionMatrices(orders);
  kernels::freeDGMatrices(orders);
  kernels::freeBasisFunctions(orders);

  for (auto solver : exahype::solvers::RegisteredSolvers) {
    delete solver;
  }
  exahype::solvers::RegisteredSolvers.clear();

  for (auto plotter : exahype::plotters::RegisteredPlotters) {
    delete plotter;
  }
  exahype::plotters::RegisteredPlotters.clear();
  for (auto coupling : exahype::solvers::RegisteredSolverCouplings) {
    delete coupling;
  }
  exahype::solvers::RegisteredSolverCouplings.clear();
}

std::string kernels::readSpecificationFileToJSON(const std::string& filename) {
  static tarch::logging::Log _log("kernels");
  // Call the ExaHyPE Python Toolkit and grab stdout. stderr instead passes.
  // Things where this could be improved:
  //  - Harmonize logging of subcommand into Tarch's logging
  //  - Pipe the specfile into the external parser instead of passing a filename
  constexpr int buffer_size = 512;
  
  // These variables are generated by the toolkit itself and should make it
  // safe to call a similar python environment as the toolkit has found it from
  // anywhere on the same computer/cluster. If you encounter problems, you
  // can change them here (note that KernelCalls.cpp is overwritten by every
  // toolkit run.
  std::string command = "/home/anne/Desktop/Workshop/exahype-engine/Toolkit/toolkit.sh --format=any --validate-only " + filename + " 2>&1";
  // Note that we pass stderr to stdout so in case of problems, 
  
  if ( tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logInfo("readSpecificationFileToJSON()", "Invoking external command: " << command);
  }

  std::array<char, buffer_size> buffer;
  std::string json;
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    logError("readSpecificationFileToJSON()", "Could not call external command with popen: " << strerror(errno) << " --- The command I tried to invoke  was: " << command);
    throw std::runtime_error("Could not call ExaHyPE toolkit.");
  }

  while (!feof(pipe))
    if (fgets(buffer.data(), buffer_size, pipe) != nullptr)
        json += buffer.data();

  // Check exist code of 
  int exit_status = pclose(pipe)/256; // the proper way is to use the WEXITSTATUS macro, but some Intel compilers on some clusters refuse to compile it.
  if(exit_status != 0) {
     logError("readSpecificationFileToJSON()", "The toolkit call from ExaHyPE returned a non-zero exit value ("<< exit_status <<"). That means it certainly did not produce proper JSON. The output is given below.");
     std::cerr << "Output from toolkit:\n" << json << "\n";
     throw std::runtime_error("Could not properly call toolkit from ExaHyPE.");
  }

  return json;
}

const char* kernels::compiledSpecfile() {
  /* This is a hexdump of the specfile which was used to create this registration file.     */
  /* Run ExaHyPE with --help to learn how to view it's contents and/or run ExaHyPE with it. */
  static const char ret[] =
  {
    0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61, 0x72, 0x63, 0x68, 0x69, 0x74,
    0x65, 0x63, 0x74, 0x75, 0x72, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x6e, 0x6f, 0x61,
    0x72, 0x63, 0x68, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x63, 0x6f,
    0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x5f, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x22,
    0x3a, 0x20, 0x22, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x63, 0x6f,
    0x6d, 0x70, 0x75, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x61, 0x6c, 0x5f, 0x64,
    0x6f, 0x6d, 0x61, 0x69, 0x6e, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x69, 0x6d, 0x65, 0x6e, 0x73, 0x69,
    0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x32, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x22, 0x65, 0x6e, 0x64, 0x5f, 0x74, 0x69, 0x6d, 0x65, 0x22,
    0x3a, 0x20, 0x31, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6d, 0x61, 0x78, 0x5f, 0x6d, 0x65, 0x73, 0x68, 0x5f, 0x73,
    0x65, 0x74, 0x75, 0x70, 0x5f, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f,
    0x6e, 0x73, 0x22, 0x3a, 0x20, 0x34, 0x30, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x22,
    0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x2e, 0x30, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x6e, 0x65, 0x5f, 0x74, 0x68, 0x69, 0x72,
    0x64, 0x5f, 0x6f, 0x66, 0x5f, 0x63, 0x65, 0x6c, 0x6c, 0x73, 0x5f, 0x6f, 0x75,
    0x74, 0x73, 0x69, 0x64, 0x65, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x75,
    0x74, 0x73, 0x69, 0x64, 0x65, 0x5f, 0x63, 0x65, 0x6c, 0x6c, 0x73, 0x22, 0x3a,
    0x20, 0x32, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22,
    0x6f, 0x75, 0x74, 0x73, 0x69, 0x64, 0x65, 0x5f, 0x63, 0x65, 0x6c, 0x6c, 0x73,
    0x5f, 0x6c, 0x65, 0x66, 0x74, 0x22, 0x3a, 0x20, 0x31, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x77, 0x69, 0x64, 0x74, 0x68, 0x22,
    0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x31, 0x30, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x31, 0x30, 0x2e, 0x30, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5d, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6c, 0x69, 0x6e, 0x6b,
    0x65, 0x72, 0x5f, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x22, 0x3a, 0x20, 0x22, 0x22,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x70, 0x74, 0x69, 0x6d, 0x69,
    0x73, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x69, 0x73, 0x61, 0x62, 0x6c,
    0x65, 0x5f, 0x6d, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x65, 0x78,
    0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x5f, 0x69, 0x6e, 0x5f, 0x62, 0x61, 0x74,
    0x63, 0x68, 0x65, 0x64, 0x5f, 0x74, 0x69, 0x6d, 0x65, 0x5f, 0x73, 0x74, 0x65,
    0x70, 0x73, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x69, 0x73, 0x61, 0x62,
    0x6c, 0x65, 0x5f, 0x76, 0x65, 0x72, 0x74, 0x65, 0x78, 0x5f, 0x65, 0x78, 0x63,
    0x68, 0x61, 0x6e, 0x67, 0x65, 0x5f, 0x69, 0x6e, 0x5f, 0x74, 0x69, 0x6d, 0x65,
    0x5f, 0x73, 0x74, 0x65, 0x70, 0x73, 0x22, 0x3a, 0x20, 0x74, 0x72, 0x75, 0x65,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x6f,
    0x75, 0x62, 0x6c, 0x65, 0x5f, 0x63, 0x6f, 0x6d, 0x70, 0x72, 0x65, 0x73, 0x73,
    0x69, 0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x66, 0x75, 0x73, 0x65, 0x5f, 0x61,
    0x6c, 0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x69, 0x63, 0x5f, 0x73, 0x74,
    0x65, 0x70, 0x73, 0x22, 0x3a, 0x20, 0x22, 0x6e, 0x6f, 0x6e, 0x65, 0x22, 0x2c,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x66, 0x75, 0x73,
    0x65, 0x5f, 0x61, 0x6c, 0x67, 0x6f, 0x72, 0x69, 0x74, 0x68, 0x6d, 0x69, 0x63,
    0x5f, 0x73, 0x74, 0x65, 0x70, 0x73, 0x5f, 0x64, 0x69, 0x66, 0x66, 0x75, 0x73,
    0x69, 0x6f, 0x6e, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f, 0x72, 0x22, 0x3a, 0x20,
    0x30, 0x2e, 0x39, 0x39, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x66, 0x75, 0x73, 0x65, 0x5f, 0x61, 0x6c, 0x67, 0x6f, 0x72, 0x69,
    0x74, 0x68, 0x6d, 0x69, 0x63, 0x5f, 0x73, 0x74, 0x65, 0x70, 0x73, 0x5f, 0x72,
    0x65, 0x72, 0x75, 0x6e, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f, 0x72, 0x22, 0x3a,
    0x20, 0x30, 0x2e, 0x39, 0x39, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x6e, 0x67, 0x22, 0x3a,
    0x20, 0x22, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x22, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x65, 0x73, 0x68, 0x5f,
    0x72, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x22, 0x3a, 0x20,
    0x22, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x22, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x70, 0x61, 0x77, 0x6e, 0x5f,
    0x61, 0x6d, 0x72, 0x5f, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e,
    0x64, 0x5f, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x73, 0x22, 0x3a, 0x20, 0x66,
    0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x73, 0x70, 0x61, 0x77, 0x6e, 0x5f, 0x64, 0x6f, 0x75, 0x62, 0x6c,
    0x65, 0x5f, 0x63, 0x6f, 0x6d, 0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x6f, 0x6e,
    0x5f, 0x61, 0x73, 0x5f, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e,
    0x64, 0x5f, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x22, 0x3a, 0x20, 0x66, 0x61,
    0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x73, 0x70, 0x61, 0x77, 0x6e, 0x5f, 0x6e, 0x65, 0x69, 0x67, 0x68, 0x62,
    0x6f, 0x75, 0x72, 0x5f, 0x6d, 0x65, 0x72, 0x67, 0x65, 0x5f, 0x61, 0x73, 0x5f,
    0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73,
    0x65, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73,
    0x70, 0x61, 0x77, 0x6e, 0x5f, 0x70, 0x72, 0x65, 0x64, 0x69, 0x63, 0x74, 0x6f,
    0x72, 0x5f, 0x61, 0x73, 0x5f, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75,
    0x6e, 0x64, 0x5f, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x22, 0x3a, 0x20, 0x66,
    0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x73, 0x70, 0x61, 0x77, 0x6e, 0x5f, 0x70, 0x72, 0x6f, 0x6c, 0x6f,
    0x6e, 0x67, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x61, 0x73, 0x5f, 0x62, 0x61,
    0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x74, 0x68, 0x72, 0x65,
    0x61, 0x64, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x70, 0x61, 0x77, 0x6e,
    0x5f, 0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x5f, 0x61, 0x73, 0x5f, 0x62, 0x61,
    0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x74, 0x68, 0x72, 0x65,
    0x61, 0x64, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x74, 0x69, 0x6d, 0x65, 0x5f,
    0x73, 0x74, 0x65, 0x70, 0x5f, 0x62, 0x61, 0x74, 0x63, 0x68, 0x5f, 0x66, 0x61,
    0x63, 0x74, 0x6f, 0x72, 0x22, 0x3a, 0x20, 0x30, 0x2e, 0x30, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x61, 0x74,
    0x68, 0x73, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x65, 0x78, 0x61, 0x68, 0x79, 0x70, 0x65, 0x5f, 0x70, 0x61,
    0x74, 0x68, 0x22, 0x3a, 0x20, 0x22, 0x2e, 0x2f, 0x45, 0x78, 0x61, 0x48, 0x79,
    0x50, 0x45, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x6c, 0x6f, 0x67, 0x5f, 0x66, 0x69, 0x6c, 0x65, 0x22, 0x3a, 0x20, 0x22,
    0x6d, 0x79, 0x6c, 0x6f, 0x67, 0x66, 0x69, 0x6c, 0x65, 0x2e, 0x6c, 0x6f, 0x67,
    0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f,
    0x75, 0x74, 0x70, 0x75, 0x74, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f,
    0x72, 0x79, 0x22, 0x3a, 0x20, 0x22, 0x2e, 0x2f, 0x44, 0x65, 0x6d, 0x6f, 0x6e,
    0x73, 0x74, 0x72, 0x61, 0x74, 0x6f, 0x72, 0x73, 0x2f, 0x53, 0x68, 0x61, 0x6c,
    0x6c, 0x6f, 0x77, 0x2d, 0x57, 0x61, 0x74, 0x65, 0x72, 0x22, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x65, 0x61, 0x6e, 0x6f,
    0x5f, 0x6b, 0x65, 0x72, 0x6e, 0x65, 0x6c, 0x5f, 0x70, 0x61, 0x74, 0x68, 0x22,
    0x3a, 0x20, 0x22, 0x2e, 0x2f, 0x50, 0x65, 0x61, 0x6e, 0x6f, 0x22, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x6c, 0x6f, 0x74,
    0x74, 0x65, 0x72, 0x5f, 0x73, 0x75, 0x62, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74,
    0x6f, 0x72, 0x79, 0x22, 0x3a, 0x20, 0x22, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x72, 0x6f, 0x6a, 0x65,
    0x63, 0x74, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x53, 0x57,
    0x45, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x68, 0x61, 0x72,
    0x65, 0x64, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x22, 0x3a, 0x20, 0x7b,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61, 0x75, 0x74,
    0x6f, 0x74, 0x75, 0x6e, 0x69, 0x6e, 0x67, 0x5f, 0x73, 0x74, 0x72, 0x61, 0x74,
    0x65, 0x67, 0x79, 0x22, 0x3a, 0x20, 0x22, 0x64, 0x75, 0x6d, 0x6d, 0x79, 0x22,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x62, 0x61,
    0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x6a, 0x6f, 0x62, 0x5f,
    0x63, 0x6f, 0x6e, 0x73, 0x75, 0x6d, 0x65, 0x72, 0x73, 0x22, 0x3a, 0x20, 0x38,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x62, 0x61,
    0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x6a, 0x6f, 0x62, 0x5f,
    0x70, 0x72, 0x6f, 0x63, 0x65, 0x73, 0x73, 0x69, 0x6e, 0x67, 0x22, 0x3a, 0x20,
    0x22, 0x6a, 0x6f, 0x62, 0x5f, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x22, 0x2c,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x63, 0x6f, 0x72,
    0x65, 0x73, 0x22, 0x3a, 0x20, 0x38, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x22, 0x69, 0x6e, 0x76, 0x61, 0x73, 0x69, 0x6f, 0x6e, 0x5f,
    0x73, 0x74, 0x72, 0x61, 0x74, 0x65, 0x67, 0x79, 0x22, 0x3a, 0x20, 0x22, 0x6e,
    0x6f, 0x5f, 0x69, 0x6e, 0x76, 0x61, 0x64, 0x65, 0x22, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6a, 0x6f, 0x62, 0x5f, 0x73, 0x79,
    0x73, 0x74, 0x65, 0x6d, 0x5f, 0x77, 0x61, 0x69, 0x74, 0x5f, 0x62, 0x65, 0x68,
    0x61, 0x76, 0x69, 0x6f, 0x75, 0x72, 0x22, 0x3a, 0x20, 0x22, 0x70, 0x72, 0x6f,
    0x63, 0x65, 0x73, 0x73, 0x5f, 0x61, 0x6e, 0x79, 0x5f, 0x6a, 0x6f, 0x62, 0x73,
    0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d,
    0x61, 0x6e, 0x75, 0x61, 0x6c, 0x5f, 0x70, 0x69, 0x6e, 0x6e, 0x69, 0x6e, 0x67,
    0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x61, 0x78, 0x5f, 0x62, 0x61, 0x63,
    0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x6a, 0x6f, 0x62, 0x73, 0x5f,
    0x69, 0x6e, 0x5f, 0x6f, 0x6e, 0x65, 0x5f, 0x72, 0x75, 0x73, 0x68, 0x22, 0x3a,
    0x20, 0x32, 0x31, 0x34, 0x37, 0x34, 0x38, 0x33, 0x36, 0x34, 0x37, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x69, 0x6e, 0x5f,
    0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x5f, 0x6a, 0x6f,
    0x62, 0x73, 0x5f, 0x69, 0x6e, 0x5f, 0x6f, 0x6e, 0x65, 0x5f, 0x72, 0x75, 0x73,
    0x68, 0x22, 0x3a, 0x20, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x70, 0x72, 0x6f, 0x70, 0x65, 0x72, 0x74, 0x69, 0x65, 0x73,
    0x5f, 0x66, 0x69, 0x6c, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x73, 0x68, 0x61, 0x72,
    0x65, 0x64, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x2e, 0x70, 0x72, 0x6f, 0x70,
    0x65, 0x72, 0x74, 0x69, 0x65, 0x73, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x6f, 0x6c, 0x76, 0x65, 0x72,
    0x73, 0x22, 0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x61, 0x64, 0x65, 0x72, 0x64, 0x67, 0x5f, 0x6b, 0x65, 0x72,
    0x6e, 0x65, 0x6c, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61,
    0x64, 0x6a, 0x75, 0x73, 0x74, 0x5f, 0x73, 0x6f, 0x6c, 0x75, 0x74, 0x69, 0x6f,
    0x6e, 0x22, 0x3a, 0x20, 0x22, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x77, 0x69, 0x73,
    0x65, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61, 0x6c, 0x6c, 0x6f, 0x63,
    0x61, 0x74, 0x65, 0x5f, 0x74, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x61, 0x72, 0x79,
    0x5f, 0x61, 0x72, 0x72, 0x61, 0x79, 0x73, 0x22, 0x3a, 0x20, 0x22, 0x68, 0x65,
    0x61, 0x70, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x62, 0x61, 0x73, 0x69,
    0x73, 0x22, 0x3a, 0x20, 0x22, 0x4c, 0x65, 0x67, 0x65, 0x6e, 0x64, 0x72, 0x65,
    0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d,
    0x65, 0x6e, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x22, 0x67,
    0x65, 0x6e, 0x65, 0x72, 0x69, 0x63, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22,
    0x6c, 0x61, 0x6e, 0x67, 0x75, 0x61, 0x67, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x43,
    0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x6f, 0x6e, 0x6c, 0x69, 0x6e,
    0x65, 0x61, 0x72, 0x22, 0x3a, 0x20, 0x74, 0x72, 0x75, 0x65, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6f, 0x70, 0x74, 0x69, 0x6d, 0x69, 0x73, 0x65, 0x64, 0x5f,
    0x6b, 0x65, 0x72, 0x6e, 0x65, 0x6c, 0x5f, 0x64, 0x65, 0x62, 0x75, 0x67, 0x67,
    0x69, 0x6e, 0x67, 0x22, 0x3a, 0x20, 0x5b, 0x5d, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x6f, 0x70, 0x74, 0x69, 0x6d, 0x69, 0x73, 0x65, 0x64, 0x5f, 0x74, 0x65,
    0x72, 0x6d, 0x73, 0x22, 0x3a, 0x20, 0x5b, 0x5d, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x73, 0x70, 0x61, 0x63, 0x65, 0x5f, 0x74, 0x69, 0x6d, 0x65, 0x5f, 0x70,
    0x72, 0x65, 0x64, 0x69, 0x63, 0x74, 0x6f, 0x72, 0x22, 0x3a, 0x20, 0x7b, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x66, 0x69, 0x78, 0x5f, 0x70,
    0x69, 0x63, 0x61, 0x72, 0x64, 0x5f, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x69,
    0x6f, 0x6e, 0x73, 0x22, 0x3a, 0x20, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x70, 0x6c, 0x69, 0x74,
    0x5f, 0x63, 0x6b, 0x22, 0x3a, 0x20, 0x22, 0x64, 0x69, 0x73, 0x61, 0x62, 0x6c,
    0x65, 0x64, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x74, 0x65, 0x72, 0x6d, 0x73, 0x22, 0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x66, 0x6c, 0x75, 0x78, 0x22, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x63, 0x70, 0x22, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x5d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x66, 0x76, 0x5f, 0x6b, 0x65, 0x72,
    0x6e, 0x65, 0x6c, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61,
    0x64, 0x6a, 0x75, 0x73, 0x74, 0x5f, 0x73, 0x6f, 0x6c, 0x75, 0x74, 0x69, 0x6f,
    0x6e, 0x22, 0x3a, 0x20, 0x22, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x77, 0x69, 0x73,
    0x65, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x61, 0x6c, 0x6c, 0x6f, 0x63,
    0x61, 0x74, 0x65, 0x5f, 0x74, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x61, 0x72, 0x79,
    0x5f, 0x61, 0x72, 0x72, 0x61, 0x79, 0x73, 0x22, 0x3a, 0x20, 0x22, 0x73, 0x74,
    0x61, 0x63, 0x6b, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x69, 0x6d, 0x70,
    0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x22, 0x3a,
    0x20, 0x22, 0x67, 0x65, 0x6e, 0x65, 0x72, 0x69, 0x63, 0x22, 0x2c, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6c, 0x61, 0x6e, 0x67, 0x75, 0x61, 0x67, 0x65, 0x22, 0x3a,
    0x20, 0x22, 0x43, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x63, 0x68,
    0x65, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x67, 0x6f, 0x64, 0x75, 0x6e, 0x6f,
    0x76, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x73, 0x6c, 0x6f, 0x70, 0x65,
    0x5f, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x65, 0x72, 0x22, 0x3a, 0x20, 0x22, 0x6d,
    0x69, 0x6e, 0x6d, 0x6f, 0x64, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x74,
    0x65, 0x72, 0x6d, 0x73, 0x22, 0x3a, 0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x22, 0x66, 0x6c, 0x75, 0x78, 0x22, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x63, 0x70, 0x22, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x5d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x22, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x65, 0x72, 0x22,
    0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x6d, 0x70, 0x5f, 0x64,
    0x69, 0x66, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x63, 0x65, 0x5f, 0x73, 0x63, 0x61,
    0x6c, 0x69, 0x6e, 0x67, 0x22, 0x3a, 0x20, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x6d,
    0x70, 0x5f, 0x6f, 0x62, 0x73, 0x65, 0x72, 0x76, 0x61, 0x62, 0x6c, 0x65, 0x73,
    0x22, 0x3a, 0x20, 0x30, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x64, 0x6d, 0x70,
    0x5f, 0x72, 0x65, 0x6c, 0x61, 0x78, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x5f, 0x70,
    0x61, 0x72, 0x61, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x22, 0x3a, 0x20, 0x31, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2e, 0x30, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x61, 0x74,
    0x69, 0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x22, 0x67, 0x65, 0x6e, 0x65, 0x72, 0x69,
    0x63, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x61, 0x78, 0x69, 0x6d, 0x75, 0x6d, 0x5f,
    0x6d, 0x65, 0x73, 0x68, 0x5f, 0x64, 0x65, 0x70, 0x74, 0x68, 0x22, 0x3a, 0x20,
    0x32, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6d, 0x61, 0x78, 0x69, 0x6d, 0x75, 0x6d, 0x5f, 0x6d, 0x65,
    0x73, 0x68, 0x5f, 0x73, 0x69, 0x7a, 0x65, 0x22, 0x3a, 0x20, 0x30, 0x2e, 0x35,
    0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x4d, 0x79, 0x53,
    0x57, 0x45, 0x53, 0x6f, 0x6c, 0x76, 0x65, 0x72, 0x22, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x72,
    0x64, 0x65, 0x72, 0x22, 0x3a, 0x20, 0x33, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x61, 0x72, 0x61,
    0x6d, 0x65, 0x74, 0x65, 0x72, 0x73, 0x22, 0x3a, 0x20, 0x7b, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x65, 0x70, 0x73, 0x69, 0x6c, 0x6f, 0x6e, 0x22, 0x3a, 0x20, 0x30,
    0x2e, 0x30, 0x30, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x67, 0x72, 0x61,
    0x76, 0x22, 0x3a, 0x20, 0x39, 0x2e, 0x38, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x73, 0x63, 0x65, 0x6e, 0x61, 0x72, 0x69, 0x6f, 0x22, 0x3a, 0x20, 0x34,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x70, 0x6c, 0x6f, 0x74, 0x74, 0x65, 0x72, 0x73, 0x22, 0x3a,
    0x20, 0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x43, 0x6f,
    0x6e, 0x73, 0x65, 0x72, 0x76, 0x65, 0x64, 0x57, 0x72, 0x69, 0x74, 0x65, 0x72,
    0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6f, 0x75,
    0x74, 0x70, 0x75, 0x74, 0x22, 0x3a, 0x20, 0x22, 0x2e, 0x2f, 0x63, 0x6f, 0x6e,
    0x73, 0x65, 0x72, 0x76, 0x65, 0x64, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x22, 0x72, 0x65, 0x70, 0x65, 0x61, 0x74, 0x22, 0x3a, 0x20,
    0x30, 0x2e, 0x30, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x74, 0x69, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x74, 0x79, 0x70, 0x65, 0x22,
    0x3a, 0x20, 0x22, 0x76, 0x74, 0x75, 0x3a, 0x3a, 0x43, 0x61, 0x72, 0x74, 0x65,
    0x73, 0x69, 0x61, 0x6e, 0x3a, 0x3a, 0x63, 0x65, 0x6c, 0x6c, 0x73, 0x3a, 0x3a,
    0x6c, 0x69, 0x6d, 0x69, 0x74, 0x65, 0x64, 0x3a, 0x3a, 0x61, 0x73, 0x63, 0x69,
    0x69, 0x22, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x76,
    0x61, 0x72, 0x69, 0x61, 0x62, 0x6c, 0x65, 0x73, 0x22, 0x3a, 0x20, 0x36, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x5d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x5f,
    0x73, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x73, 0x22, 0x3a, 0x20, 0x30, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22,
    0x74, 0x69, 0x6d, 0x65, 0x5f, 0x73, 0x74, 0x65, 0x70, 0x70, 0x69, 0x6e, 0x67,
    0x22, 0x3a, 0x20, 0x22, 0x67, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x22, 0x2c, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22,
    0x74, 0x79, 0x70, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x4c, 0x69, 0x6d, 0x69, 0x74,
    0x69, 0x6e, 0x67, 0x2d, 0x41, 0x44, 0x45, 0x52, 0x2d, 0x44, 0x47, 0x22, 0x2c,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x22, 0x76, 0x61, 0x72, 0x69, 0x61, 0x62, 0x6c, 0x65, 0x73, 0x22, 0x3a, 0x20,
    0x5b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x6c, 0x69, 0x63, 0x69, 0x74,
    0x79, 0x22, 0x3a, 0x20, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x68, 0x22, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x6c,
    0x69, 0x63, 0x69, 0x74, 0x79, 0x22, 0x3a, 0x20, 0x31, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20,
    0x22, 0x68, 0x75, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x2c, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6d, 0x75,
    0x6c, 0x74, 0x69, 0x70, 0x6c, 0x69, 0x63, 0x69, 0x74, 0x79, 0x22, 0x3a, 0x20,
    0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x6e, 0x61,
    0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x68, 0x76, 0x22, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x7d, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x6c, 0x69, 0x63, 0x69,
    0x74, 0x79, 0x22, 0x3a, 0x20, 0x31, 0x2c, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0x3a, 0x20, 0x22, 0x62, 0x22,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x5d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x5d, 0x0a, 0x7d, 0x00
  };
  return ret;
}