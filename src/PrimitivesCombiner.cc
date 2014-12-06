#include "L1Trigger/L1IntegratedMuonTrigger/interface/PrimitiveCombiner.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitive.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/DTGeometry/interface/DTChamber.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "FWCore/Utilities/interface/Exception.h"


L1ITMu::PrimitiveCombiner::PrimitiveCombiner( const L1ITMu::PrimitiveCombiner::resolutions & res, edm::ESHandle<DTGeometry> & muonGeom )
  : _resol(res), _muonGeom(muonGeom),
    _bx(0), _radialAngle(0), _bendingAngle(0), _bendingResol(0),
    _dtHI(0), _dtHO(0), _rpcIn(0), _rpcOut(0)
{}


void L1ITMu::PrimitiveCombiner::addDt( const L1ITMu::TriggerPrimitive & dt )
{
  int qualityCode = dt.getDTData().qualityCode;
  switch ( qualityCode ) {
  case 2 : addDtHO( dt ); break;
  case 3 : addDtHI( dt ); break;
  default :
    throw cms::Exception("Invalid DT Quality") 
      << "This module can combine only HI to HO (quality2/3), provided : "
      << qualityCode << std::endl;
  }
}


void L1ITMu::PrimitiveCombiner::addDtHI( const L1ITMu::TriggerPrimitive & prim )
{
  if ( _dtHI) 
    throw cms::Exception("Invalid DT Quality") 
      << "DT primitive with quality HI already provided"
      << std::endl;
  _dtHI = &prim;
}


void L1ITMu::PrimitiveCombiner::addDtHO( const L1ITMu::TriggerPrimitive & prim )
{
  if ( _dtHO )
    throw cms::Exception("Invalid DT Quality") 
      << "DT primitive with quality HO already provided"
      << std::endl;
  _dtHO = &prim;
}


void L1ITMu::PrimitiveCombiner::addRpcIn( const L1ITMu::TriggerPrimitive & prim )
{
  _rpcIn = &prim;
}


void L1ITMu::PrimitiveCombiner::addRpcOut( const L1ITMu::TriggerPrimitive & prim )
{
  _rpcOut = &prim;
}


void L1ITMu::PrimitiveCombiner::addHO( const L1ITMu::TriggerPrimitive & prim )
{
  _hoIn = &prim;
}


void L1ITMu::PrimitiveCombiner::combine() 
{
  if ( !isValid() ) return;

  typedef L1ITMu::PrimitiveCombiner::results localResult;
  std::vector<localResult> localResults;
  _radialAngle = 0;

  // inner and outer DT
  if ( _dtHI && _dtHO ) {
    localResults.push_back( combineDt( _dtHI, _dtHO ) );
    _radialAngle = localResults.back().radialAngle;
  }
  
  // inner DT
  if ( ! ( _dtHI && _dtHO ) && _dtHI ) {
    if (!_dtHO) {
      localResults.push_back(dummyCombineDt(_dtHI));
      std::cout<<" initial dT radialAngle : " << _dtHI->getDTData().radialAngle << std::endl;
    }
    if ( _rpcIn ) {
      localResults.push_back( combineDtRpc( _dtHI, _rpcIn ) );
      std::cout<<"RPCIn randialAngle : "<< _radialAngle << std::endl;
      _radialAngle = _radialAngle ? _radialAngle :_dtHI->getDTData().radialAngle;
      std::cout<<"RPCIn after randialAngle : "<< _radialAngle << " dT radialAngle : " << _dtHI->getDTData().radialAngle << std::endl;
    }
    if ( _rpcOut ) {
      localResults.push_back( combineDtRpc( _dtHI, _rpcOut ) );
      _radialAngle = _radialAngle ? _radialAngle :_dtHI->getDTData().radialAngle;
    }
    if ( _hoIn ) {
      localResults.push_back( combineDtHo( _dtHI, _hoIn ) );
      _radialAngle = _radialAngle ? _radialAngle :_dtHI->getDTData().radialAngle;
    }
  }
  
  // outer DT
  if ( !( _dtHI && _dtHO ) && _dtHO ) {
    if (!_dtHI) localResults.push_back(dummyCombineDt(_dtHO));
    if ( _rpcIn ) {
      localResults.push_back( combineDtRpc( _dtHO, _rpcIn ) );
      _radialAngle = _radialAngle ? _radialAngle :_dtHO->getDTData().radialAngle;
    }
    if ( _rpcOut ) {
      localResults.push_back( combineDtRpc( _dtHO, _rpcOut ) );
      _radialAngle = _radialAngle ? _radialAngle :_dtHO->getDTData().radialAngle;
    }
    if ( _hoIn ) {
      localResults.push_back( combineDtHo( _dtHO, _hoIn ) );
      _radialAngle = _radialAngle ? _radialAngle :_dtHO->getDTData().radialAngle;
    }
  }
  
  // no DT
  if ( !_dtHI && !_dtHO ) {
    if ( _rpcIn && _rpcOut ) {
      results local = combineRpcRpc( _rpcIn, _rpcOut );
      localResults.push_back( local );
      //_radialAngle = local.radialAngle;
      if ( _hoIn ) {
	localResults.push_back( combineRpcHo( _rpcIn, _rpcOut,  _hoIn ) );
	_radialAngle = _radialAngle ? _radialAngle :radialAngleFromGlobalPhi( _rpcIn );
      }
    } else if ( _rpcIn ) {
      //std::cout<<"Entered RPC-IN-ONLY thing..."<<std::endl;
      if ( _hoIn ) {
        localResults.push_back( combineRpcHo( _rpcIn, _hoIn ) );
        _radialAngle = _radialAngle ? _radialAngle :radialAngleFromGlobalPhi( _rpcIn );
	_bendingAngle = -666;
      }
      return;
    } else if ( _rpcOut ) {
      if ( _hoIn ) {
        localResults.push_back( combineRpcHo( _rpcOut, _hoIn ) );
        _radialAngle = _radialAngle ? _radialAngle :radialAngleFromGlobalPhi( _rpcOut );
        _bendingAngle = -666;
      }
      return;
    }
  } // no DT
   
   double weightSum = 0;
   _bendingResol = 0;
   _bendingAngle = 0;
   
   //
   std::vector<localResult>::const_iterator it    = localResults.begin();
   std::vector<localResult>::const_iterator itend = localResults.end();
   int kcount=0;
   for ( ; it != itend; ++it ) {
     //weightSum += it->bendingResol;
     //_bendingAngle += it->bendingResol * it->bendingAngle;
     kcount++;
     //std::cout<<"combining result "<<kcount<<" with resolution "<<it->bendingResol<<std::endl;
     weightSum += 1.0/(( it->bendingResol)*(it->bendingResol));
     _bendingAngle += double(it->bendingAngle)/((it->bendingResol) * (it->bendingResol));
     _bendingResol += it->bendingResol * it->bendingResol;
   } 
   
   _bendingAngle /= weightSum;
   _bendingResol = sqrt( _bendingResol );
}



L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::dummyCombineDt( const L1ITMu::TriggerPrimitive * dt)
{	
  // i want to combine also the DT data alone
  results localResult;
  localResult.radialAngle  = dt->getDTData().radialAngle;
  //std::cout<<" HEY!!! I am adding a DT benfing as a combination result! "<<std::endl;
  localResult.bendingAngle = dt->getDTData().bendingAngle;
  localResult.bendingResol = _resol.phibDtUnCorr;
  return localResult;
}
 


L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineDt( const L1ITMu::TriggerPrimitive * dt1,
				      const L1ITMu::TriggerPrimitive * dt2 )
{
  const DTChamber* chamb1 = _muonGeom->chamber( dt1->detId<DTChamberId>() );
  LocalPoint point1       = chamb1->toLocal( dt1->getCMSGlobalPoint() );

  const DTChamber* chamb2 = _muonGeom->chamber( dt2->detId<DTChamberId>() );
  LocalPoint point2       = chamb2->toLocal( dt2->getCMSGlobalPoint() );

  results localResult;
  localResult.radialAngle = 0.5 * ( dt1->getDTData().radialAngle + dt2->getDTData().radialAngle );
  
  // positive chambers
  if ((dt1->getDTData().wheel > 0) || ((dt1->getDTData().wheel == 0) && 
				       !(dt1->getDTData().sector==0 || dt1->getDTData().sector==3 || dt1->getDTData().sector==4 || 
					 dt1->getDTData().sector==7 || dt1->getDTData().sector==8 || dt1->getDTData().sector==11))) 
    localResult.bendingAngle = ( atan(phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() ) - (localResult.radialAngle / 4096.0) ) ) * 512;
  else
    // negative chambers
    localResult.bendingAngle = ( atan(-phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() ) - (localResult.radialAngle / 4096.0) ) ) * 512;
  
  localResult.bendingResol = phiBCombinedResol( _resol.xDt, _resol.xDt, point1.z(), point2.z() );
  
  //std::cout<<" == === COMBINING DT-DT === == "<<std::endl;
  //std::cout << "dt-dt radial : " << dt1->getDTData().radialAngle << " * " << dt2->getDTData().radialAngle << " = " << localResult.radialAngle << '\n';
  //std::cout << " " << point1.x() << " " << point1.z() << " " << dt1->getDTData().qualityCode << '\n';
  //std::cout << " " << point2.x() << " " << point2.z() << " " << dt2->getDTData().qualityCode << '\n';
  //std::cout << "dt-dt bending : " << dt1->getDTData().bendingAngle << " * " << dt2->getDTData().bendingAngle << " = "
  //	    << localResult.bendingAngle << '\n';
  //std::cout<<" --- this was sector "<<dt1->getDTData().sector<<" and wheel "<<dt1->getDTData().wheel<<std::endl;
  
  return localResult;
}


L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineDtHo( const L1ITMu::TriggerPrimitive * dt,
					const L1ITMu::TriggerPrimitive * ho )
{
  results localResult;
  localResult.radialAngle = dt->getDTData().radialAngle;

  //point-I
  const DTChamber* chamb1 = _muonGeom->chamber( dt->detId<DTChamberId>() );
  LocalPoint point1 = chamb1->toLocal( dt->getCMSGlobalPoint() );  

  //point-II
  // const DTChamber* chamb1 = _muonGeom->chamber( dt1->detId<DTChamberId>() );
  // int station = rpc->detId<RPCDetId>().station();
  int sector  = ho->detId<HOId>().sector();
  int wheel   = ho->detId<HOId>().wheel();
  int station = 1; //combirning only with one station

  const DTChamber* chamb2 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point2 = chamb2->toLocal( ho->getCMSGlobalPoint() );
  
  if ((dt->getDTData().wheel > 0) ||
      ((dt->getDTData().wheel == 0) && !(dt->getDTData().sector==0 || dt->getDTData().sector==3 || dt->getDTData().sector==4 || 
					 dt->getDTData().sector==7 || dt->getDTData().sector==8 || dt->getDTData().sector==11))) 
    // positive wheels
    localResult.bendingAngle = (atan( phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() )-(localResult.radialAngle/ 4096.0))) * 512;
  else
    // negative wheels
    localResult.bendingAngle = (atan( -phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() )-(localResult.radialAngle/ 4096.0))) * 512;
  
  localResult.bendingResol = phiBCombinedResol( _resol.xDt, point1.z() );
  
  return localResult;
}

L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineDtRpc( const L1ITMu::TriggerPrimitive * dt,
					 const L1ITMu::TriggerPrimitive * rpc )
{
  results localResult;
  localResult.radialAngle = dt->getDTData().radialAngle;

  //point-I
  const DTChamber* chamb1 = _muonGeom->chamber( dt->detId<DTChamberId>() );
  LocalPoint point1 = chamb1->toLocal( dt->getCMSGlobalPoint() );  

  //point-II
  int station = rpc->detId<RPCDetId>().station();
  int sector  = rpc->detId<RPCDetId>().sector();
  int wheel   = rpc->detId<RPCDetId>().ring();
  const DTChamber* chamb2 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point2 = chamb2->toLocal( rpc->getCMSGlobalPoint() );
  
  if ((dt->getDTData().wheel > 0) ||
      ((dt->getDTData().wheel == 0) && !(dt->getDTData().sector==0 || dt->getDTData().sector==3 || dt->getDTData().sector==4 || 
					 dt->getDTData().sector==7 || dt->getDTData().sector==8 || dt->getDTData().sector==11))) 
    // positive wheels
    localResult.bendingAngle = (atan( phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() )-(localResult.radialAngle/ 4096.0))) * 512;
  else
    // negative wheels
    localResult.bendingAngle = (atan( -phiBCombined( point1.x(), point1.z(), point2.x(), point2.z() )-(localResult.radialAngle/ 4096.0))) * 512;
  
  localResult.bendingResol = phiBCombinedResol( _resol.xDt, _resol.xRpc, point1.z(), point2.z() );
  
  return localResult;
}
 
 
// dt+rpc solo bending, phi dt
// dt+dt bending, media della posizione, direzione in base alla differenza dei due punti
// cancellare seconda traccia (bx diverso)

int L1ITMu::PrimitiveCombiner::radialAngleFromGlobalPhi( const L1ITMu::TriggerPrimitive * rpc )
{
  int radialAngle  = 0;
  int radialAngle2 = 0;
  int sector       = rpc->detId<RPCDetId>().sector();
  float phiGlobal  = rpc->getCMSGlobalPhi();

  // int wheel = rpc->detId<RPCDetId>().ring();
  // from phiGlobal to radialAngle of the primitive in sector sec in [1..12]
  if ( sector == 1) radialAngle = int( phiGlobal*4096 );
  else {
    if ( phiGlobal >= 0) radialAngle = int( (phiGlobal- (sector-1) * Geom::pi()/6 ) * 4096 );
    else radialAngle = int( (phiGlobal + (13-sector) * Geom::pi()/6) * 4096 ); 
  }

  //if ( ( wheel>0 ) ||
  //     ( ( wheel==0 ) &&
  //      ( sector==0 || sector==3 || sector==4 || sector==7 || sector==8 || sector==11 ) ) )
  radialAngle2 = radialAngle;
  //else
  // radialAngle2 = -radialAngle;
  return radialAngle2;
}


L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineRpcRpc( const L1ITMu::TriggerPrimitive * rpc1,
                                          const L1ITMu::TriggerPrimitive * rpc2 )

{  
  int station = rpc1->detId<RPCDetId>().station();
  int sector  = rpc1->detId<RPCDetId>().sector();
  int wheel = rpc1->detId<RPCDetId>().ring();
  const DTChamber* chamb1 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point1 = chamb1->toLocal( rpc1->getCMSGlobalPoint() );


  station = rpc2->detId<RPCDetId>().station();
  sector  = rpc2->detId<RPCDetId>().sector();
  wheel = rpc2->detId<RPCDetId>().ring();
  const DTChamber* chamb2 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point2 = chamb2->toLocal( rpc2->getCMSGlobalPoint() );


  results localResult;
  localResult.radialAngle = 0.5*(radialAngleFromGlobalPhi( rpc1 )+radialAngleFromGlobalPhi( rpc2 )) ;

  if ( ( wheel>0 ) ||
       ( ( wheel==0 ) &&
         !( sector==0 || sector==3 || sector==4 || sector==7 || sector==8 || sector==11 ) ) )
    localResult.bendingAngle = ( atan ( phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() ) 
                                        - ( localResult.radialAngle / 4096.0 )
                                        ) ) * 512;
    else
    localResult.bendingAngle = ( atan (-phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() )
                                       - ( localResult.radialAngle/4096.0 )
                                       ) ) * 512;
  localResult.bendingResol = phiBCombinedResol( _resol.xRpc, _resol.xRpc, point1.z(), point2.z());


  return localResult;

}



L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineRpcHo( const L1ITMu::TriggerPrimitive * rpc1,
					 const L1ITMu::TriggerPrimitive * rpc2,
                                          const L1ITMu::TriggerPrimitive * ho )

{  
  int station = rpc1->detId<RPCDetId>().station();
  int sector  = rpc1->detId<RPCDetId>().sector();
  int wheel = rpc1->detId<RPCDetId>().ring();
  const DTChamber* chamb1 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point1 = chamb1->toLocal( rpc1->getCMSGlobalPoint() );


  station = rpc2->detId<RPCDetId>().station();
  sector  = rpc2->detId<RPCDetId>().sector();
  wheel = rpc2->detId<RPCDetId>().ring();
  const DTChamber* chamb2 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point2 = chamb2->toLocal( rpc2->getCMSGlobalPoint() );


  results localResult;
  localResult.radialAngle = 0.5*(radialAngleFromGlobalPhi( rpc1 )+radialAngleFromGlobalPhi( rpc2 )) ;

  if ( ( wheel>0 ) ||
       ( ( wheel==0 ) &&
         !( sector==0 || sector==3 || sector==4 || sector==7 || sector==8 || sector==11 ) ) )
    localResult.bendingAngle = ( atan ( phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() ) 
                                        - ( localResult.radialAngle / 4096.0 )
                                        ) ) * 512;
    else
    localResult.bendingAngle = ( atan (-phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() )
                                       - ( localResult.radialAngle/4096.0 )
                                       ) ) * 512;
  localResult.bendingResol = phiBCombinedResol( _resol.xRpc, _resol.xRpc, point1.z(), point2.z());


  return localResult;

}



L1ITMu::PrimitiveCombiner::results
L1ITMu::PrimitiveCombiner::combineRpcHo( const L1ITMu::TriggerPrimitive * rpc,
                                          const L1ITMu::TriggerPrimitive * ho )

{  
  int station = rpc->detId<RPCDetId>().station();
  int sector  = rpc->detId<RPCDetId>().sector();
  int wheel = rpc->detId<RPCDetId>().ring();
  const DTChamber* chamb1 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point1 = chamb1->toLocal( rpc->getCMSGlobalPoint() );

  //point-II
  sector  = ho->detId<HOId>().sector();
  wheel   = ho->detId<HOId>().wheel();
  station = 1; //combirning only with one station

  const DTChamber* chamb2 = _muonGeom->chamber( DTChamberId( wheel, station, sector ) );
  LocalPoint point2 = chamb2->toLocal( ho->getCMSGlobalPoint() );


  results localResult;
  localResult.radialAngle = radialAngleFromGlobalPhi( rpc ) ;// 0.5*(radialAngleFromGlobalPhi( rpc1 )+radialAngleFromGlobalPhi( rpc2 )) ;

  if ( ( wheel>0 ) ||
       ( ( wheel==0 ) &&
         !( sector==0 || sector==3 || sector==4 || sector==7 || sector==8 || sector==11 ) ) )
    localResult.bendingAngle = ( atan ( phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() ) 
                                        - ( localResult.radialAngle / 4096.0 )
                                        ) ) * 512;
    else
    localResult.bendingAngle = ( atan (-phiBCombined( point1.x(),
                                                      point1.z(),
                                                      point2.x(),
                                                      point2.z() )
                                       - ( localResult.radialAngle/4096.0 )
                                       ) ) * 512;
  localResult.bendingResol = phiBCombinedResol( _resol.xRpc, _resol.xRpc, point1.z(), point2.z());

  return localResult;
}
