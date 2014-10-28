L1IntegratedHOMuonTrigger
=======================

Code for implementing and studying a L1 trigger that uses trigger primitive information from all muon subsystems and Hadron-outer calorimeter of CMS.

Validated for:

* CMSSW_7_1_X


To build:

* scram p CMSSW CMSSW_7_1_7
* cd CMSSW_7_1_7/src/
* cmsenv
* git clone https://github.com/saxenapooja/L1MuonHOTrigger.git L1Trigger/L1IntegratedMuonTrigger/
* scram b -j9


To run:

* cd L1Trigger/L1IntegratedMuonTrigger/test
* cmsRun run_L1ITMuonBarrelLocalTrigger_cfg.py

Info Page:

* https://twiki.cern.ch/twiki/bin/viewauth/CMS/DtDttfTriggerUpgradeStudies#Technical_Aspects_Data_Samples_a