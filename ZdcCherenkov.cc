
#include "SimG4CMS/Forward/interface/ZdcCherenkov.h"
#include "SimG4CMS/Forward/interface/ForwardName.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "SimG4Core/Notification/interface/G4TrackToParticleID.h"
#include "SimG4Core/Notification/interface/TrackInformation.h"
//#include "SimG4CMS/Forward/interface/ZdcNumberingScheme.h"

#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"
//#include "G4Cerenkov.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/GlobalPhysicalConstants.h>
#include "Randomize.hh"
#include "G4Poisson.hh"
#include "G4TwoVector.hh"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

using namespace std;
using namespace edm;

//#define EDM_ML_DEBUG

ZdcCherenkov::ZdcCherenkov(edm::ParameterSet const& p){
}

//===================================================================================================

ZdcCherenkov:: ~ZdcCherenkov() {}

//===================================================================================================
 
int ZdcCherenkov::computeNPE(const G4Step* aStep) {

  const double alpha = 0.0073;
  const double ref_index = 1.459;
  const double EMAX = 4.79629 /*eV*/;				   // Maximum energy of PMT 
  const double EMIN = 1.75715 /*eV*/;				   // Minimum energy of PMT   
  const double fibreR = 0.35;
//  const double gain=0.3;
  const double aperture=0.33;
  const double HBARC = 6.582119514E-16 /*eV*s*/ * 2.99792458E8 /*m/s*/;  // hbar * c

  const double aperturetrapped=aperture/ref_index;

  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
  G4double beta = preStepPoint->GetBeta();
  double charge = preStepPoint->GetCharge();
  if (charge == 0.0)
    return 0.0;

  double step_length = aStep->GetStepLength() / 1000;  // Geant4 stepLength is in "mm"

  if (beta < (1 / ref_index) || step_length < 0.0001) { // no cherenkov
    return 0;
  }

  G4ThreeVector Pre = preStepPoint->GetPosition();
  G4ThreeVector Post = postStepPoint->GetPosition();

  const G4ThreeVector particleDirection_global = (Post - Pre) / (Post - Pre).mag();

    const G4TouchableHandle& theTouchable = preStepPoint->GetTouchableHandle();
    G4ThreeVector localPre = theTouchable->GetHistory()->GetTopTransform().TransformPoint(Pre);
    G4ThreeVector localPost = theTouchable->GetHistory()->GetTopTransform().TransformPoint(Post);

  // Calculate the unit direction vector in local (fiber) coordinates
  const G4ThreeVector particleDirection = (localPost - localPre) / (localPost - localPre).mag();

  double MeanNumberOfPhotons = (alpha * charge * charge * step_length) / HBARC * (EMAX - EMIN)  
  * (1.0 - 1.0 / (beta * beta * ref_index * ref_index));

  int nbOfPhotons = G4Poisson(MeanNumberOfPhotons) * aStep->GetTrack()->GetWeight();

    int npe = 0; 
                               
  if (nbOfPhotons <= 0) {
    return 0;
  } else {

    double theta_C = acos(1. / (beta * ref_index));
    double sinTheta = sin(theta_C);
    double cosTheta = cos(theta_C);

    double length = std::sqrt((localPost.x() - localPre.x()) * (localPost.x() - localPre.x()) +
                              (localPost.y() - localPre.y()) * (localPost.y() - localPre.y()));

    double yemit = std::sqrt(fibreR * fibreR - length * length / 4.);
    
//    printf(" length, yemit %7.2f %7.2f \n ",length, yemit);

//    double u_ph = 0, v_ph = 0, w_ph = 0;

  // --------------------------------------------------------------------------------------------------
    for (int i = 0; i < nbOfPhotons; i++) {
      double photonE = EMIN + G4UniformRand() * (EMAX - EMIN);       // uniform energy
      double lambda=1240 / photonE;
      double rr = G4UniformRand();
      
      if (rr > pmtEfficiency(lambda)) continue;
// -------
      double phi_C = 2 * M_PI * G4UniformRand();
      double cosPhi = cos(phi_C);
      double sinPhi = sin(phi_C);
       // Calculate momentum direction w.r.t primary particle (z-direction)
      double px = photonE * sinTheta * cosPhi;
      double py = photonE * sinTheta * sinPhi;
      double pz = photonE * cosTheta;
      G4ThreeVector photonMomentum(px, py, pz);

      // Rotate to the fiber reference frame
      photonMomentum.rotateUz(particleDirection);  // particleDirection is in fiber coordinates 
      // --------------

      double u_ph = photonMomentum.x()/photonE;
      double v_ph = photonMomentum.y()/photonE;
      double w_ph = photonMomentum.z()/photonE;

      double xemit = length * (G4UniformRand() - 0.5);
      double gam = atan2(yemit, xemit);
      double eps = atan2(v_ph, u_ph);
      double sinBeta = sin(gam - eps);
      double rho = std::sqrt(xemit * xemit + yemit*yemit);
      double sinEta = rho / fibreR * sinBeta;
      double cosEta = std::sqrt(1. - sinEta * sinEta);
      double sinPsi = std::sqrt(1. - w_ph * w_ph);
      double cosKsi = cosEta * sinPsi;

      if (cosKsi < aperturetrapped && w_ph > 0) npe += 1;
    }                
  }                  
  return npe;
}

double ZdcCherenkov::pmtEfficiency(double lambda) {
  // Hardcoded wavelength values (nm)
  const vector<double> LAMBDA_TAB{263.27, 265.98, 268.69, 271.39, 273.20, 275.90, 282.22, 282.22, 293.04,
				       308.38, 325.52, 346.26, 367.91, 392.27, 417.53, 440.98, 463.53, 484.28,
				       502.32, 516.75, 528.48, 539.30, 551.93, 564.56, 574.48, 584.41, 595.23,
				       606.96, 616.88, 625.00, 632.22, 637.63, 642.14, 647.55, 652.96, 656.57,
				       661.08, 666.49, 669.20, 673.71, 677.32, 680.93, 686.34, 692.65};

  // Hardcoded quantum efficiency values
  const vector<double> EFF_TAB{2.215,  2.860,  3.659,  4.724,  5.989,  7.734,  9.806,  9.806,  12.322,
				    15.068, 17.929, 20.570, 22.963, 24.050, 23.847, 22.798, 20.445, 18.003,
				    15.007, 12.282, 9.869,  7.858,  6.373,  5.121,  4.077,  3.276,  2.562,
				    2.077,  1.669,  1.305,  1.030,  0.805,  0.629,  0.492,  0.388,  0.303,
				    0.239,  0.187,  0.144,  0.113,  0.088,  0.069,  0.054,  0.043};
  //double efficiency = evaluateFunction(LAMBDA_TAB,EFF_TAB,lambda);

  // Using linear interpolation to calculate efficiency
  for (int i = 0; i < 44 - 1; i++) {
    if (lambda > LAMBDA_TAB[i] && lambda < LAMBDA_TAB[i + 1]) {
      double a = (EFF_TAB[i] - EFF_TAB[i + 1]) / (LAMBDA_TAB[i] - LAMBDA_TAB[i + 1]);
      double b = EFF_TAB[i] - a * LAMBDA_TAB[i];
      return (a * lambda + b) / 100.0;
    }
  }

  return 0;
}

