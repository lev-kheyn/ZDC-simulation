#ifndef SimG4CMSForwardZdcCherenkov_h
#define SimG4CMSForwardZdcCherenkov_h
///////////////////////////////////////////////////////////////////////////////

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimG4CMS/Calo/interface/CaloSD.h"
#include "G4VPhysicalVolume.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include <TH1F.h>
class ZdcCherenkov {
public:
  ZdcCherenkov(edm::ParameterSet const& p);
  virtual ~ZdcCherenkov();

  int computeNPE(const G4Step* step);

protected:

//  int computeNbOfPhotons();
//  G4ThreeVector setToLocal(const G4ThreeVector&, const G4VTouchable*) const;
  double pmtEfficiency(double);
};

#endif
