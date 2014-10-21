#ifndef __L1ITMU_INTERNALTRACK_H__
#define __L1ITMU_INTERNALTRACK_H__
// 
// Class: L1ITMu::InternalTrack
//
// Info: This class represents (one of the) internal tracks processed
//       by L1ITMu before sending off to the GT or whatever comes after.
//       As such, they are similar to the L1MuRegionalCands in terms of
//       their meaning.
//       To exploit that we also allow these tracks to be built out of
//       old style regional cands so we can perform studies using them
//       in the new framework. So that debugging is easier in the case
//       of the new track not matching it's parent, a reference to the
//       parent track is stored.
//
// Author: L. Gray (FNAL)
//

#include <iostream>

#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitiveFwd.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/TriggerPrimitive.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "L1Trigger/L1IntegratedMuonTrigger/interface/RegionalTracksFwd.h"
#include "DataFormats/Common/interface/RefToBase.h"

class L1MuDTTrackCand;

namespace csc {
  class L1Track;
}

namespace L1ITMu{
  class InternalTrack : public L1MuRegionalCand {   
  public:
    enum subsystem_offset{ kDT, kRPCb, kCSC, kRPCf };
    InternalTrack():_endcap(0),_wheel(0),_sector(0),_type(5),_mode(0) {}
    ~InternalTrack() {}
    
    InternalTrack(const L1MuDTTrackCand&);
    InternalTrack(const csc::L1Track&);
    InternalTrack(const L1MuRegionalCand&,
		  const RPCL1LinkRef&); // for RPCs
    
    void setType(unsigned type) { _type = type; }
    unsigned type_idx() const;

    // return the persistent pointer to the parent of this internal track
    // may be null if this has no parent
    RegionalCandBaseRef parent() const { return _parent; }
    void setParent(const RegionalCandBaseRef& parent)
       { _parent = parent; }

    RPCL1LinkRef parentRPCLink() const { return _parentlink; }

    void addStub(const TriggerPrimitiveRef& stub) ;
         
    const TriggerPrimitiveStationMap& getStubs() const 
      { return _associatedStubs; }

    unsigned long mode()     const { return (_mode & 0xffff); }
    unsigned long dtMode()   const { return (_mode & 0xf<<4*kDT )>>4*kDT; }
    unsigned long cscMode()  const { return (_mode & 0xf<<4*kCSC)>>4*kCSC; }
    unsigned long rpcbMode() const { return (_mode & 0xf<<4*kRPCb)>>4*kRPCb; }
    unsigned long rpcfMode() const { return (_mode & 0xf<<4*kRPCf)>>4*kRPCf; }

    void print(std::ostream&) const;

  private:
    TriggerPrimitiveStationMap _associatedStubs;
    int _endcap, _wheel, _sector;
    unsigned _type;
    // this represents the mode considering all available muon detector types
    // 0 DT 4 bits | RPCb 4 bits | CSC 4 bits | RPC f 4 bits
    // using an unsigned long since we may want to add GEMs later
    // so cscMode() will return only the CSC part of the tracks contributing
    // to a CSC track (if this track was built from one)
    unsigned long _mode; 
    //pointer to parent, if this was created from a CSC/DT/RPC track
    RegionalCandBaseRef _parent;
    //pointer to RPC-L1 link, if an rpc track
    RPCL1LinkRef _parentlink;
  };
}

#endif
