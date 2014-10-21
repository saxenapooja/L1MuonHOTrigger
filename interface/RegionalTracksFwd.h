#ifndef __L1ITMU_REGIONALTRACKSFWD_H__
#define __L1ITMU_REGIONALTRACKSFWD_H__

#include <vector>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/RefToBase.h"

class L1MuRegionalCand;
class L1MuDTTrackCand;
class RPCDigiL1Link;
namespace csc {
  class L1Track;
}

namespace L1ITMu {
  typedef std::vector<L1MuRegionalCand> RegionalCandCollection;
  typedef edm::RefToBase<L1MuRegionalCand> RegionalCandBaseRef;
  typedef edm::Ptr<L1MuRegionalCand> RegionalCandPtr;
  typedef edm::Ref<RegionalCandCollection> RegionalCandRef;

  typedef std::vector<L1MuDTTrackCand>  DTTrackCollection;
  typedef edm::Ptr<L1MuDTTrackCand> DTTrackPtr;
  typedef edm::Ref<DTTrackCollection> DTTrackRef;

  typedef std::vector<csc::L1Track>  CSCTrackCollection;
  typedef edm::Ptr<csc::L1Track> CSCTrackPtr;
  typedef edm::Ref<CSCTrackCollection> CSCTrackRef;

  typedef std::vector<RPCDigiL1Link> RPCL1LinkCollection;
  typedef edm::Ptr<RPCDigiL1Link> RPCL1LinkPtr;
  typedef edm::Ref<RPCL1LinkCollection> RPCL1LinkRef;
}

#endif
