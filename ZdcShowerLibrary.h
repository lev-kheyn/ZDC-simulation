#ifndef SimG4CMS_ZdcShowerLibrary_h
#define SimG4CMS_ZdcShowerLibrary_h 1
///////////////////////////////////////////////////////////////////////////////
// File: ZdcShowerLibrary.h
// Description: Gets information from a shower library
// E. Garcia June 2008
///////////////////////////////////////////////////////////////////////////////

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/ForwardGeometry/interface/ZdcHardcodeGeometryData.h"

#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "DetectorDescription/Core/interface/DDsvalues.h"
#include "DataFormats/HcalDetId/interface/HcalZDCDetId.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include <TH1F.h>
#include "G4ThreeVector.hh"

#include <string>
#include <memory>

class G4Step;
class ZdcShowerLibrary {
public:
  //Constructor and Destructor
  ZdcShowerLibrary(const std::string& name, edm::ParameterSet const& p);
  ~ZdcShowerLibrary() = default;

  struct Hit {
    Hit() {}
    int detID;
    int energy;
    int time;
  };

//  std::vector<Hit> getHits(const G4Step* aStep, bool& ok);
  std::vector<Hit>& getHits(const G4Step* aStep, bool& ok);

protected:
  void getRecord(int, int, double);
  void interpolate(int, double, double, double);
//  void extrapolate(int, double, double, double);

  TBranch *emBranch, *hadBranch;
  bool verbose;
  G4int emPDG, epPDG, gammaPDG;
  G4int pi0PDG, etaPDG, nuePDG, numuPDG, nutauPDG;
  G4int anuePDG, anumuPDG, anutauPDG, geantinoPDG;
  int npe;
  std::vector<ZdcShowerLibrary::Hit> hits;    // From simulated showers
  std::vector<ZdcShowerLibrary::Hit> hitt;    // From simulated showers
  std::vector<ZdcShowerLibrary::Hit> hitsel;  // Selected ZdcShowerLibrary hits
  int nbinx, nbiny, nmomg, nmomn, ntotx, ntoty, evtPerShg, evtPerShn, totEvents;
//  float xbin, ybin, pmomg, pmomn;
  std::vector<float> xbin, ybin, pmomg, pmomn, lpmomg, lpmomn;
};
#endif
