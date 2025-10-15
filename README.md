#To produce simulations
cmsrel CMSSW_14_1_0
cd CMSSW_14_1_0/src
scram b disable-biglib
cmsenv
scram b clean
cmsenv
git cms-addpkg SimG4CMS/Forward

cd SimG4CMS/Forward/src 
cp -pr ZdcSD.cc ZdcSD.cc_orig
lnf ZdcSD.cc_new  ZdcSD.cc.
lnf ZdcShowerLibrary.cc_new ZdcShowerLibrary.cc
cp ZdcCherenkov.cc 

cd ../interface
cp -pr ZdcSD.h ZdcSD.h_orig
lnf ZdcSD.h_new ZdcSD.h
cp ZdcCherenkov.h 

cd ../test/python 
scramv1 b -j 8

1.For simulation:

a) For full simulation without use of library
cp ZdcRun3_full.py .
cp ZdcRun3_full_surf.py .

cmsenv
cmsRun ZdcRun3_full.py
cmsRun ZdcRun3_full_surf.py

b) For simulation with use of library
For starting from custom vertex
cp ZdcRun3_lib.py .
For starting at surface of ZDC (needed for tests and creation of shower library)
cp ZdcRun3_lib_surf.py .

2. For processing of root files produced in simulation:

cd ../plugins

For ZdcAnalyzer
cp -pr ZDCSimHitStudy.cc ZDCSimHitStudy.cc_orig
lnf ZDCSimHitStudy.cc_new ZDCSimHitStudy.cc

For library creation
cp /afs/cern.ch/user/k/kheyn/public/For_Hannah/./ZDCSimHitStudy.cc_forlib
lnf ZDCSimHitStudy.cc_forlib ZDCSimHitStudy.cc

scramv1 b -j 8

cd ../test/python
cp runZDCStudy_new_cfg.py
cmsRun runZDCStudy_new_cfg.py
3. For analyzis of produced in runZDCStudy root files
cp ZdcAnalyzer.C
root ZdcAnalyzer.C

=========================================================================
cp “file” implies copy from github.

Config file zdcRun3.py runs simulation with incident particles from the vertex.
Config file zdcRun3_surf.py runs simulation with incident particles pushed directly at the
surface of ZDC. It can be run both for neutrons (PartID=2112) and photons (PartID=22). The
initial x and y coordinates (in cm) at ZDC surface are set by parameters
"process.VtxSmeared.MeanX" and "process.VtxSmeared.MeanY". For such events, only plus side of ZDC
should be analyzed..

After the simulation step, the produced root file can be processed with use of config file
runZDCStudy_cfg.py. Plugin ZDCSimHitStudy.cc is applied, which produces tree to be used in
further analysis or for creation of shower library

One example of macro analyzing produced tree is in ZdcAnalyzer.C. It histograms distributions of
full number of photoelectrons in Em and HAD sections.

Simulation produces and analyzes number of photoelectrons. The "energy" variable contains just that number.

All above was done for release CMSSW_14_1_0. It could be done for newer release with imbedding of necessary modifications.


