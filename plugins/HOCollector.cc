#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HORecHit.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h" //for bunch crossing
#include "L1Trigger/L1IntegratedMuonTrigger/interface/HOCollector.h"

using namespace L1ITMu;

HOCollector::HOCollector( const edm::ParameterSet& ps ): SubsystemCollector(ps) 
{
}


void HOCollector::extractPrimitives(const edm::Event& ev, const edm::EventSetup& es, std::vector<TriggerPrimitive>& out) const {

  edm::Handle<HODigiCollection> hoDigis;
  ev.getByLabel(_src, hoDigis);

  edm::ESHandle<CaloGeometry> caloGeo;
  es.get<CaloGeometryRecord>().get(caloGeo);
  
  auto bho_reco = hoDigis->begin();
  auto eho_reco = hoDigis->end();

  int ieta = caloGeo->getPosition(bho_reco->id()).eta();
  int iphi = caloGeo->getPosition(bho_reco->id()).phi();
  
  HOId id(ieta, iphi);
  int HObx = ev.eventAuxiliary().bunchCrossing();

  for(; bho_reco != eho_reco; ++bho_reco){
    out.push_back(TriggerPrimitive(id,
				   id.wheel(),
				   id.sector(),
				   id.eta(),
				   id.phi(),
				   id.trayId(),
				   id.tileId(),
   				   id.Emin(),
				   id.Emax(),
				   HObx));
  }
  
}

#include "L1Trigger/L1IntegratedMuonTrigger/interface/SubsystemCollectorFactory.h"
DEFINE_EDM_PLUGIN( SubsystemCollectorFactory, HOCollector, "HOCollector");
