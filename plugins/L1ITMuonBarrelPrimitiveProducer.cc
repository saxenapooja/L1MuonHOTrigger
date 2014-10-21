// framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

// L1IT include files
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitive.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitiveFwd.h"

#include "L1Trigger/L1IntegratedMuonTrigger/interface/MBLTCollection.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/MBLTCollectionFwd.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhDigi.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThDigi.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/PrimitiveCombiner.h"

// user include files
#include "DataFormats/Math/interface/deltaPhi.h"

class L1ITMuonBarrelPrimitiveProducer : public edm::EDProducer {

public:
  L1ITMuonBarrelPrimitiveProducer(const edm::ParameterSet&);
  ~L1ITMuonBarrelPrimitiveProducer();
  virtual void produce(edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag _mbltCollectionInput;
  const L1ITMu::PrimitiveCombiner::resolutions _resol;
  const int _qualityRemappingMode;
  const int _useRpcBxForDtBelowQuality;
  edm::ESHandle<DTGeometry> _muonGeom;
};

std::ostream & operator<< (std::ostream & out, const L1ITMu::TriggerPrimitiveList & rpc )
{
  std::vector<L1ITMu::TriggerPrimitiveRef>::const_iterator it = rpc.begin();
  std::vector<L1ITMu::TriggerPrimitiveRef>::const_iterator end = rpc.end();
  for ( ; it != end ; ++it ) out << (*it)->getCMSGlobalPhi() << '\t';
  out << std::endl;
  return out;
}

L1ITMuonBarrelPrimitiveProducer::~L1ITMuonBarrelPrimitiveProducer()
{
}

L1ITMuonBarrelPrimitiveProducer::L1ITMuonBarrelPrimitiveProducer( const edm::ParameterSet& iConfig )
  : _mbltCollectionInput( iConfig.getParameter<edm::InputTag>("MBLTCollection") ),
    _resol( iConfig.getParameter<double>("xDtResol"), 
	    iConfig.getParameter<double>("xRpcResol"),
	    iConfig.getParameter<double>("phibDtCorrResol"),
	    iConfig.getParameter<double>("phibDtUnCorrResol")
	    ),
    _qualityRemappingMode( iConfig.getParameter<int>("qualityRemappingMode") ),
    _useRpcBxForDtBelowQuality( iConfig.getParameter<int>("useRpcBxForDtBelowQuality") )
 
{
  produces<L1MuDTChambPhContainer>();
  //produces<L1MuDTChambThContainer>();
  // produces<std::vector<L1MuDTChambPhDigi> >();
}


void L1ITMuonBarrelPrimitiveProducer::produce( edm::Event& iEvent, 
					      const edm::EventSetup& iSetup )
{
  iSetup.get<MuonGeometryRecord>().get(_muonGeom);

  std::auto_ptr<L1MuDTChambPhContainer> out(new L1MuDTChambPhContainer);
  std::vector<L1MuDTChambPhDigi> phiChambVector;
  
  edm::Handle<L1ITMu::MBLTContainer> mbltContainer;
  iEvent.getByLabel( _mbltCollectionInput, mbltContainer );

  L1ITMu::MBLTContainer::const_iterator st = mbltContainer->begin();
  L1ITMu::MBLTContainer::const_iterator stend = mbltContainer->end();

  L1MuDTChambPhContainer phiContainer;
  std::vector<L1MuDTChambPhDigi> phiVector;
  
  for ( ; st != stend; ++st ) {

    const L1ITMu::MBLTCollection & mbltStation = st->second;

    /// useful index
    int station = mbltStation.station();
    int wheel = mbltStation.wheel();
    int sector = mbltStation.sector();
    ///

    /// get dt to rpc associations
    size_t dtListSize = mbltStation.getDtSegments().size();
    std::vector<size_t> uncorrelated;
    std::vector<size_t> correlated;
    for ( size_t iDt = 0; iDt < dtListSize; ++iDt ) {
      const L1ITMu::TriggerPrimitiveRef & dt = mbltStation.getDtSegments().at(iDt);
      int dtquality = dt->getDTData().qualityCode;
      /// define new set of qualities
      /// skip for the moment uncorrelated
      // int qualityCode = -2;
      switch ( dtquality ) {
      case -1 : continue;/// -1 are theta
      case 0 : /* qualityCode = -2;*/ break;
      case 1 : /* qualityCode = -2;*/ break;
      case 2 : uncorrelated.push_back( iDt ); continue;
      case 3 : uncorrelated.push_back( iDt ); continue;
      case 4 : correlated.push_back( iDt ); continue; //qualityCode = 5; break;
      case 5 : correlated.push_back( iDt ); continue; //qualityCode = 5; break;
      case 6 : correlated.push_back( iDt ); continue; //qualityCode = 5; break;
      default : /* qualityCode = dtquality; */ break;
      }

      //L1MuDTChambPhDigi chamb( dt->getBX(), wheel, sector-1, station, dt->getDTData().radialAngle,
      //		       dt->getDTData().bendingAngle, qualityCode,
      //			       dt->getDTData().Ts2TagCode, dt->getDTData().BxCntCode );
      //phiChambVector.push_back( chamb );
    }

    // START OF BX ANALYSIS FOR CORRELATED TRIGGER
    size_t cSize = correlated.size(); 
    for ( size_t idxDt = 0; idxDt < cSize; ++idxDt ) {
      int bx=-999;
      int iDt = correlated.at(idxDt);
      if ( iDt < 0 ) continue;
      const L1ITMu::TriggerPrimitive & dt = *mbltStation.getDtSegments().at(iDt);
      L1ITMu::TriggerPrimitiveList rpcInMatch = mbltStation.getRpcInAssociatedStubs( iDt );
      L1ITMu::TriggerPrimitiveList rpcOutMatch = mbltStation.getRpcOutAssociatedStubs( iDt );
      size_t rpcInMatchSize = rpcInMatch.size();
      size_t rpcOutMatchSize = rpcOutMatch.size();
      if ( rpcInMatchSize && rpcOutMatchSize ) {
	const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	/// only the first is real...
	// LG try also to reassign BX to single H using RPC BX, e.g. do not ask for DT and RPC to have the same BX
	if (( dt.getBX() == rpcIn.getBX() && dt.getBX() == rpcOut.getBX() ) || (_qualityRemappingMode>1 && rpcIn.getBX()==rpcOut.getBX() && abs(dt.getBX()-rpcIn.getBX())<=1)) {
	  bx = rpcIn.getBX();
	}
      }else if (rpcInMatchSize){
	const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	if ( dt.getBX() == rpcIn.getBX() || (_qualityRemappingMode>1 && abs(dt.getBX()-rpcIn.getBX())<=1)) {
	  bx = rpcIn.getBX();
	}
      }
      else if (rpcOutMatchSize){
	const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	if ( dt.getBX() == rpcOut.getBX() || (_qualityRemappingMode>1 && abs(dt.getBX()-rpcOut.getBX())<=1)) {
	  bx = rpcOut.getBX();
	}
      }
      // add primitive here
      int newBx=dt.getBX();
      if (bx>-999 && dt.getDTData().qualityCode<_useRpcBxForDtBelowQuality){
	newBx=bx;
      }
      L1MuDTChambPhDigi chamb( newBx, wheel, sector-1, station, dt.getDTData().radialAngle,
			       dt.getDTData().bendingAngle, dt.getDTData().qualityCode,
			       dt.getDTData().Ts2TagCode, dt.getDTData().BxCntCode );
      phiChambVector.push_back( chamb );
    }
    // END OF BX ANALYSIS FOR CORRELATED TRIGGER

    size_t uncSize = uncorrelated.size(); 
    for ( size_t idxDt = 0; idxDt < uncSize; ++idxDt ) {

      int iDt = uncorrelated.at(idxDt);
      if ( iDt < 0 ) continue;
      const L1ITMu::TriggerPrimitive & dt = *mbltStation.getDtSegments().at(iDt);

      /// check if there is a pair of HI+HO at different bx
      int closest = -1;
      int closestIdx = -1;
      double minDeltaPhiDt = 9999999999;
      for ( size_t jdxDt = idxDt+1; jdxDt < uncSize; ++jdxDt ) {

	int jDt = uncorrelated.at(jdxDt);
	if ( jDt < 0 ) continue;

	const L1ITMu::TriggerPrimitiveRef & dtM = mbltStation.getDtSegments().at(jDt);
	if ( dt.getBX() == dtM->getBX() || dt.getDTData().qualityCode == dtM->getDTData().qualityCode )
	  continue;

	double deltaPhiDt = fabs( reco::deltaPhi( dt.getCMSGlobalPhi(), dtM->getCMSGlobalPhi() ) );
	if ( deltaPhiDt < minDeltaPhiDt ) {
	  closest = jDt;
	  closestIdx = jdxDt;
	  minDeltaPhiDt=deltaPhiDt;
	}
      }

      /// check if the pair shares the closest rpc hit
      L1ITMu::MBLTCollection::bxMatch match = L1ITMu::MBLTCollection::NOMATCH;
      if ( closest > 0 && minDeltaPhiDt < 0.05 ) {
      //if ( closest > 0 ) {
	match = mbltStation.haveCommonRpc( iDt, closest );
      }

      /// this is just a set of output variables for building L1ITMuDTChambPhDigi
      int qualityCode = dt.getDTData().qualityCode;
      int bx = -2;
      int radialAngle = 0;
      int bendingAngle = 0;
      L1ITMu::PrimitiveCombiner combiner( _resol, _muonGeom );
      /// association HI/HO provided by the tool
      combiner.addDt( dt );

      /// there is a pair HI+HO with a shared inner RPC hit
      if ( match != L1ITMu::MBLTCollection::NOMATCH ) {
	uncorrelated[closestIdx] = -1;

	/// association HI/HO provided by the tool
	combiner.addDt( *mbltStation.getDtSegments().at(closest) );

	/// redefine quality
	qualityCode = 4;
	L1ITMu::TriggerPrimitiveList rpcInMatch = mbltStation.getRpcInAssociatedStubs( iDt );
	L1ITMu::TriggerPrimitiveList rpcOutMatch = mbltStation.getRpcOutAssociatedStubs( iDt );

	/// there is a pair HI+HO with a shared inner RPC hit
	if ( match == L1ITMu::MBLTCollection::INMATCH ) {

	  const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	  combiner.addRpcIn( rpcIn );
	  bx = rpcIn.getBX();

	  /// there is a pair HI+HO with a shared outer RPC hit
	} else if ( match == L1ITMu::MBLTCollection::OUTMATCH ) {

	  const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	  combiner.addRpcOut( rpcOut );
	  bx = rpcOut.getBX();

	  /// there is a pair HI+HO with both shared inner and outer RPC hit
	} else if ( match == L1ITMu::MBLTCollection::FULLMATCH ) {

	  const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	  const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	  combiner.addRpcIn( rpcIn );
	  combiner.addRpcOut( rpcOut );
	  bx = rpcIn.getBX();
	}


      } else { /// there is no match

	L1ITMu::TriggerPrimitiveList rpcInMatch = mbltStation.getRpcInAssociatedStubs( iDt );
	L1ITMu::TriggerPrimitiveList rpcOutMatch = mbltStation.getRpcOutAssociatedStubs( iDt );
	size_t rpcInMatchSize = rpcInMatch.size();
	size_t rpcOutMatchSize = rpcOutMatch.size();

	/// the uncorrelated has possibly inner and outer confirmation
	if ( rpcInMatchSize && rpcOutMatchSize ) {
	  const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	  const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	  /// only the first is real...
	  // LG try also to reassign BX to single H using RPC BX, e.g. do not ask for DT and RPC to have the same BX
	  if (( dt.getBX() == rpcIn.getBX() && dt.getBX() == rpcOut.getBX() ) || (_qualityRemappingMode>1 && rpcIn.getBX()==rpcOut.getBX() && abs(dt.getBX()-rpcIn.getBX())<=1)) {
	    bx = rpcIn.getBX();
	    combiner.addRpcIn( rpcIn );
	    combiner.addRpcOut( rpcOut );
	  } else if ( dt.getBX() == rpcIn.getBX() ) {
	    bx = rpcIn.getBX();
	    combiner.addRpcIn( rpcIn );
	  } else if ( dt.getBX() == rpcOut.getBX() ) {
	    bx = rpcOut.getBX();
	    combiner.addRpcOut( rpcOut );
	  }

	/// the uncorrelated has a possible inner confirmation
	} else if ( rpcInMatchSize ) {
	  const L1ITMu::TriggerPrimitive & rpcIn = *rpcInMatch.front();
	  if ( dt.getBX() == rpcIn.getBX() || (_qualityRemappingMode>1 && abs(dt.getBX()-rpcIn.getBX())<=1)) {
	    bx = rpcIn.getBX();
	    combiner.addRpcIn( rpcIn );
	  }

	/// the uncorrelated has a possible outer confirmation
	} else if ( rpcOutMatchSize ) {
	  const L1ITMu::TriggerPrimitive & rpcOut = *rpcOutMatch.front();
	  if ( dt.getBX() == rpcOut.getBX()|| (_qualityRemappingMode>1  && abs(dt.getBX()-rpcOut.getBX())<=1)) {
	    bx = rpcOut.getBX();
	    combiner.addRpcOut( rpcOut );
	  }

	}
      }

      // match found, PrimitiveCombiner has the needed variables already calculated
      if ( combiner.isValid() ) {
	//std::cout<<"=== I am making a combination ==="<<std::endl;
	combiner.combine();
	radialAngle = combiner.radialAngle();
	bendingAngle = (combiner.bendingAngle()<-511 || combiner.bendingAngle()>511)?dt.getDTData().bendingAngle:combiner.bendingAngle();
      } else {
	// no match found, keep the primitive as it is
	bx = dt.getBX();
	radialAngle = dt.getDTData().radialAngle;
	bendingAngle = dt.getDTData().bendingAngle;
	//if (_qualityRemappingMode==0) 
	qualityCode = ( qualityCode == 2 ) ? 0 : 1;
      }

      L1MuDTChambPhDigi chamb( bx, wheel, sector-1, station, radialAngle,
			       bendingAngle, qualityCode,
			       dt.getDTData().Ts2TagCode, dt.getDTData().BxCntCode );
      phiChambVector.push_back( chamb );
      if (abs(bendingAngle)>511||1==1){
	//	std::cout<<"Got bending angle: "<<bendingAngle<<std::endl;
	//std::cout<<"Original DT primitive had bending angle: "<<dt.getDTData().bendingAngle<<std::endl;
	//std::cout<<"Original radial angle: "<<radialAngle<<std::endl;
	//std::cout<<"Quality: "<<qualityCode<<std::endl;
	//std::cout<<"Station: "<<station<<std::endl;
      }

    } /// end of the Uncorrelated loop
//     ////////////////////////////////////////////////////
//     /// loop over unassociated inner and outer RPC hits
//     const TriggerPrimitiveList & rpcInUnass = mbltStation.getRpcInUnassociatedStubs();
//     const TriggerPrimitiveList & rpcOutUnass = mbltStation.getRpcOutUnassociatedStubs();

//     size_t rpcInUSize = rpcInUnass.size();
//     size_t rpcOutUsize = rpcOutUnass.size();

//     for ( size_t in = 0; in < rpcInUSize; ++in ) {
//     for ( size_t out = 0; out < rpcOutUSize; ++out ) {

    const std::vector< std::pair< L1ITMu::TriggerPrimitiveList, L1ITMu::TriggerPrimitiveList > >
      rpcPairList = mbltStation.getUnassociatedRpcClusters( 0.05 );
    auto rpcPair = rpcPairList.cbegin();
    auto rpcPairEnd = rpcPairList.cend();
    for ( ; rpcPair != rpcPairEnd; ++ rpcPair ) {
      const L1ITMu::TriggerPrimitiveList & inRpc = rpcPair->first;
      const L1ITMu::TriggerPrimitiveList & outRpc = rpcPair->second;

      if ( inRpc.empty() && outRpc.empty() ) continue;

      L1ITMu::PrimitiveCombiner combiner( _resol, _muonGeom );
      size_t inSize = inRpc.size();
      size_t outSize = outRpc.size();
      int station = -1;
      int sector  = -1;
      int wheel = -5;
      double qualityCode = 0;

      if ( inSize ) {
	//std::cout<<"Producer working on IN&&!OUT"<<std::endl;
        size_t inPos = 0;
        double avPhiIn = 0;
        for ( size_t i = 0; i < inSize; ++i ) {
          double locPhi = inRpc.at(i)->getCMSGlobalPhi();
          avPhiIn += ( locPhi > 0 ? locPhi : 2*M_PI + locPhi );
        }
        avPhiIn /= inSize;
        double minDist = fabs( inRpc.at(0)->getCMSGlobalPhi() - avPhiIn );
        for ( size_t i = 1; i < inSize; ++i ) {
          double dist = fabs( inRpc.at(i)->getCMSGlobalPhi() - avPhiIn );
          if ( dist < minDist ) {
            inPos = i;
            minDist = dist;
          }
        }

        const L1ITMu::TriggerPrimitive & rpc = (*inRpc.at(inPos));
        station = rpc.detId<RPCDetId>().station();
        sector  = rpc.detId<RPCDetId>().sector();
        wheel = rpc.detId<RPCDetId>().ring();
        combiner.addRpcIn( rpc );


      }
      if ( outSize ) {
	//std::cout<<"Producer working on OUT&&!IN"<<std::endl;
        size_t outPos = 0;
        double avPhiOut = 0;
        for ( size_t i = 0; i < outSize; ++i ) {
          double locPhi = outRpc.at(i)->getCMSGlobalPhi();
          avPhiOut += ( locPhi > 0 ? locPhi : 2*M_PI + locPhi );
        }

        avPhiOut /= outSize;
        double minDist = fabs( outRpc.at(0)->getCMSGlobalPhi() - avPhiOut );
        for ( size_t i = 1; i < outSize; ++i ) {
          double dist = fabs( outRpc.at(i)->getCMSGlobalPhi() - avPhiOut );
          if ( dist < minDist ) {
            outPos = i;
            minDist = dist;
          }
        }
        const L1ITMu::TriggerPrimitive & rpc = (*outRpc.at(outPos));
        station = rpc.detId<RPCDetId>().station();
        sector  = rpc.detId<RPCDetId>().sector();
        wheel = rpc.detId<RPCDetId>().ring();
        combiner.addRpcOut( rpc );
      } 
	//else // {
//         //	std::cout<<"Producer working on IN&&OUT"<<std::endl;
//         size_t inPos = 0;
//         size_t outPos = 0;
//         double minDist = 9999;

//       for ( size_t i = 0; i < inSize; ++i ) {
//           for ( size_t j = 0; j < outSize; ++j ) {

//             double dist = fabs( inRpc.at(0)->getCMSGlobalPhi()
//                                 - outRpc.at(0)->getCMSGlobalPhi() );
//             if ( dist < minDist ) {
//               inPos = i;
//               outPos = j;
//               minDist = dist;
//             }
//           }
//         }
//         const L1ITMu::TriggerPrimitive & rpc_in = (*inRpc.at(inPos));

//         const L1ITMu::TriggerPrimitive & rpc_out = (*outRpc.at(outPos));
//         station = rpc_in.detId<RPCDetId>().station();
//         sector  = rpc_in.detId<RPCDetId>().sector();
//         wheel = rpc_in.detId<RPCDetId>().ring();
//         combiner.addRpcIn( rpc_in );
//         combiner.addRpcOut( rpc_out );
//         qualityCode = 1;
//       }

      if (inSize && outSize) qualityCode=1;
      combiner.combine();
      double radialAngle = combiner.radialAngle();
      double bendingAngle = combiner.bendingAngle();
      double bx = combiner.bx();
      double Ts2TagCode = 0;
      double BxCntCode = 0;

      L1MuDTChambPhDigi chamb( bx, wheel, sector-1, station, radialAngle,
                               bendingAngle, qualityCode,
                               Ts2TagCode, BxCntCode );
      phiChambVector.push_back( chamb );

      //std::cout << "IN: \n" << inRpc;
      //std::cout << "OUT: \n" << outRpc;
      //std::cout << "\n";

    }

  }




  out->setContainer( phiChambVector );
  /// fill event
  iEvent.put(out);

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1ITMuonBarrelPrimitiveProducer);

