#ifndef __L1ITMU_TRIGGERPRIMITIVEFWD_H__
#define __L1ITMU_TRIGGERPRIMITIVEFWD_H__

#include <vector>
#include <map>
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Ptr.h"

namespace L1ITMu{
  class TriggerPrimitive;

  typedef std::vector<TriggerPrimitive> TriggerPrimitiveCollection;
  
  typedef edm::Ref<TriggerPrimitiveCollection> TriggerPrimitiveRef;
  typedef std::vector<TriggerPrimitiveRef> TriggerPrimitiveList;
  typedef edm::Ptr<TriggerPrimitive> TriggerPrimitivePtr;
  typedef std::map<unsigned,TriggerPrimitiveList> TriggerPrimitiveStationMap;
}

#endif
