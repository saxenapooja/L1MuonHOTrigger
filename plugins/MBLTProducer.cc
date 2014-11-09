// framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/MakerMacros.h"

// user include files
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitive.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitiveFwd.h"

#include "L1Trigger/L1IntegratedMuonTrigger/interface/MBLTCollection.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/MBLTCollectionFwd.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

// class declaration
class MBLTProducer : public edm::EDProducer {
public:
  MBLTProducer(const edm::ParameterSet&);
  ~MBLTProducer() {}

  void produce(edm::Event&, const edm::EventSetup&);  

private:
  edm::InputTag _trigPrimSrc;
  double        _maxDeltaPhi;
  bool          _addHO;
  int           _maxStationForHO;
};


//constructor
MBLTProducer::MBLTProducer( const edm::ParameterSet& iConfig )
  : _trigPrimSrc(iConfig.getParameter<edm::InputTag>("TriggerPrimitiveSrc")),
    _maxDeltaPhi(iConfig.getParameter<double>("MaxDeltaPhi")),
    _addHO(iConfig.getParameter<bool>("AddHO")),
    _maxStationForHO(iConfig.getParameter<int>("MaxStationForHO"))
{
  produces<L1ITMu::MBLTContainer>().setBranchAlias("MBLTContainer");
}


//produce
void MBLTProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  std::auto_ptr<L1ITMu::MBLTContainer>   out ( new L1ITMu::MBLTContainer );
  L1ITMu::MBLTContainer & tracksMap = *out;
  
  edm::Handle<L1ITMu::TriggerPrimitiveCollection> tps;
  iEvent.getByLabel(_trigPrimSrc, tps);

  auto tp    = tps->cbegin();
  auto tpbeg = tps->cbegin();
  auto tpend = tps->cend();

  // looping over
  for( ; tp != tpend; ++tp ) {

    DTChamberId key;
    L1ITMu::TriggerPrimitive::subsystem_type type = tp->subsystem();

    switch ( type ) {
      
    case L1ITMu::TriggerPrimitive::kDT :
      key = tp->detId<DTChamberId>();
      break;

    case L1ITMu::TriggerPrimitive::kRPC : {
      if ( tp->detId<RPCDetId>().region() ) continue; // endcap
      int station = tp->detId<RPCDetId>().station();
      int sector  = tp->detId<RPCDetId>().sector();
      int wheel   = tp->detId<RPCDetId>().ring();
      key = DTChamberId( wheel, station, sector );
      break;
    }
      
      if (_addHO && tp->detId<DTChamberId>().station() < _maxStationForHO) {
      case L1ITMu::TriggerPrimitive::kHO : {
	int sector  = tp->detId<HOId>().sector();
	int wheel   = tp->detId<HOId>().ring(); 
	int station = tp->detId<DTChamberId>().station();
	key = DTChamberId( wheel, station, sector );
	break;     
      }
      }

    default : continue;
    } // switch
    
    if ( out->find( key ) == out->end() ) 
      {
	tracksMap[key] = L1ITMu::MBLTCollection( key );
      }
    
    //typedef edm::Ref<TriggerPrimitiveCollection> TriggerPrimitiveRef;
    //typedef std::vector<TriggerPrimitive>        TriggerPrimitiveCollection;
    L1ITMu::TriggerPrimitiveRef  tpref(tps, tp - tpbeg);
    tracksMap[key].addStub(tpref);
    
  } // for( ; tp != tpend; ++tp )
  
  L1ITMu::MBLTContainer::iterator st    = out->begin();
  L1ITMu::MBLTContainer::iterator stend = out->end();
  for ( ; st != stend; ++st ) st->second.associate( _maxDeltaPhi );

  iEvent.put(out);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MBLTProducer);
