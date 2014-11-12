#ifndef L1Trigger_L1IntegratedMuonTrigger_MBLTCollection_h_
#define L1Trigger_L1IntegratedMuonTrigger_MBLTCollection_h_
// 
// Class: L1ITMu:: 
//
// Info: This track represents a DT(1 station) plus eventual RPC station(s)
//       based track seed, from which a full multi-station track can be
//       built.
//
// Author: 
//

#include <iostream>
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitiveFwd.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitive.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

namespace L1ITMu {
  
  class MBLTCollection {

  public:
    /// internal enum for subdetector stub identification
    enum subsystem_offset{ kDT, kRPCb, kCSC, kRPCf, kHO };

    /// structure for internal indexing
    struct primitiveAssociation {
      std::vector< size_t > rpcIn;
      std::vector< size_t > rpcOut;
      std::vector< size_t > hoIn;
    };

    enum bxMatch { NOMATCH, INMATCH, OUTMATCH, FULLMATCH };
    
    /// default constructor
    MBLTCollection() :_wheel(0),_sector(0),_station(0) {};

    /// construction out of DTChamberId: automatically extracts info
    MBLTCollection( const DTChamberId & dtId );
    ~MBLTCollection() {};

    /// selectively add Trigger Primitives to the MBLTCollection
    /// dt, rpc up layer and rpc down layer are stored in separated collections
    void addStub( const TriggerPrimitiveRef& stub );


    /// return a reference to the DT only segments
    const TriggerPrimitiveList & getDtSegments() const {
      return _dtAssociatedStubs;
    }

    /// rpc inner layer hits only
    const TriggerPrimitiveList & getRpcInner() const {
      return _rpcInAssociatedStubs;
    }

    /// rpc outer layer hits only
    const TriggerPrimitiveList & getRpcOuter() const {
      return _rpcOutAssociatedStubs;
    }

    /// ho hits only
    const TriggerPrimitiveList & getHOInner() const {
      return _hoInAssociatesStubs;
    }


    /// returns wheel
    inline int wheel() const { return _wheel; }

    /// returns sector
    inline int sector() const { return _sector; }

    /// returns station
    inline int station() const { return _station; }

    /// returns detId
    inline DTChamberId detId() const { return DTChamberId( _wheel, _station, _sector ); }

    /// rpc inner layer hits associated to a given dt station
    TriggerPrimitiveList getRpcInAssociatedStubs( size_t dtIndex ) const;

    /// rpc outer layer hits associated to a given dt station
    TriggerPrimitiveList getRpcOutAssociatedStubs( size_t dtIndex ) const;

    // ho hits associated to a given dt station
    TriggerPrimitiveList getHOAssociatedStubs( size_t dtIndex ) const;

    // build association map among dt and rpc primitives
    void associate( double , double, bool);

    // look for common rpc hists among 2 dt primitives
    bxMatch haveCommonRpc( size_t dt1, size_t dt2 ) const;

     /// rpc inner layer hits unassociated to a given dt station
     TriggerPrimitiveList getRpcInUnassociatedStubs() const;
 
     /// rpc outer layer hits unassociated to a given dt station
     TriggerPrimitiveList getRpcOutUnassociatedStubs() const;

     /// ho hits unassociated to a given dt station
     TriggerPrimitiveList getHOUnassociatedStubs() const;

     /// RPC unassociated clusters
     std::vector< std::pair< TriggerPrimitiveList, TriggerPrimitiveList > >
       getUnassociatedRpcClusters( double minRpcPhi ) const;
  
   private :
     /// RPC unassociated utility functions
     void
       getUnassociatedRpcClusters( const std::vector< size_t > & rpcUnass,
                                 const TriggerPrimitiveList & rpcList,
                                 double minRpcPhi,
                                 std::vector< std::vector <size_t> > & clusters ) const;
     //
     bool areCloseClusters( std::vector< size_t > & cluster1,
                          std::vector< size_t > & cluster2,
                          const TriggerPrimitiveList & rpcList1,
                          const TriggerPrimitiveList & rpcList2,
                          double minRpcPhi ) const;
     //
     size_t reduceRpcClusters( std::vector< std::vector <size_t> > & tmpClusters,
                             const TriggerPrimitiveList & rpcList,
                             double minRpcPhi ) const;

  private :
    /// dt segments
     //typedef std::vector<TriggerPrimitiveRef>     TriggerPrimitiveList
    TriggerPrimitiveList _dtAssociatedStubs;

    /// rpc inner layer hits
    TriggerPrimitiveList _rpcInAssociatedStubs;

    /// rpc outer layer hits
    TriggerPrimitiveList _rpcOutAssociatedStubs;

    //// HO inner hits
    TriggerPrimitiveList _hoInAssociatesStubs;

    /// space coordinates
    int _wheel, _sector, _station;

    // association map among dt and rpc primitives
    std::vector< primitiveAssociation > _dtMapAss;

    // association map among dt and rpc primitives
    primitiveAssociation _rpcMapUnass;

  };
}

#endif
