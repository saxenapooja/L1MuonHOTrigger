#ifndef __L1ITMU_HOCOLLECTOR_H__
#define __L1ITMU_HOCOLLECTOR_H__

// 
// Class: L1ITMu::HOCollector
//
// Info: Processes HO digis into ITMu trigger primitives. 
//       Positional information is not assigned here.
//
// Author: P. Saxena (DESY)
//

#include <vector>
#include "L1Trigger/L1IntegratedMuonTrigger/interface/SubsystemCollector.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/Plane3D.h"
#include "HadronOuter/HO/interface/HOId.h"

using namespace ROOT::Math;

typedef ROOT::Math::XYZVector XYZPoint;

namespace L1ITMu {

  //class
  class HOCollector: public SubsystemCollector {

  public:
    HOCollector(const edm::ParameterSet&);
    ~HOCollector() {}

    //fucntions
    virtual void extractPrimitives(const edm::Event&, const edm::EventSetup&, 
				   std::vector<TriggerPrimitive>&) const;
    int GetRing(int &ieta) const;
    int GetSector(int &ieta, int &iphi) const;
    int GetTray(int &ieta, int &iphi) const;
    int GetTile(int &ieta, int &iphi) const;

    
    //variables
  public:
    static const int maxEta = 15;
    static const int minEta = -15;
    static const int maxPhi = 72;
    static const int minPhi = 1;
    static const int NTrays = 6;
    static const int NLayers = 2;
    //   XYZPoint HO_pos_low[maxEta - minEta + 1][maxPhi][NumberOfLayers];
  };
}

#endif
