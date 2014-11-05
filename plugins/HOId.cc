/** \file
 *  See header file for a description of this class.
 *
 *  \author G. Cerminara - Pooja Saxena
 */

#include "L1Trigger/L1IntegratedMuonTrigger/interface/HOId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <ostream>

using namespace std;

HOId::HOId() : DetId(DetId::Hcal, HcalSubdetector::HcalOuter){}


HOId::HOId(uint32_t id) {
  checkHOId();               // Check this is a valid id for HO
}
HOId::HOId(DetId id) {
  checkHOId();               // Check this is a valid id for HO
}


HOId::HOId(int ieta, int iphi, double emin, double emax): iEta(ieta), iPhi(iphi), Emin(emin), Emax(emax) {
  checkHOId();

  // Check that arguments are within the range
  if (ieta      < maxEta     || ieta   > minEta ||
      iphi      < maxPhi     || iphi   > maxPhi) {
    throw cms::Exception("InvalidDetId") << "HOId ctor:" 
					 << " Invalid parameters: " 
					 << " ieta:"<< ieta
					 << " iphi:"<< iphi
					 << std::endl;
  }
}


void HOId::checkHOId() {
  if (det()!=DetId::Hcal || subdetId()!=HcalSubdetector::HcalOuter) {
    throw cms::Exception("InvalidDetId") << "HOId ctor:"
					 << " det: " << det()
					 << " subdet: " << subdetId()
					 << " is not a valid HO id";  
  }
}


int HOId::GetBX() const{
  int bx = 2;
  return bx;
}


int HOId::GetRing() const {
  int ring = 0;
  if(iEta < -10)
    ring = -2;
  if((iEta > -11) && (iEta < -4))
    ring = -1;
  if((iEta > -5) && (iEta < 5))
    ring = 0;
  if((iEta > 4) && (iEta < 11))
    ring = 1;
  if(iEta > 10)
    ring = 2;
  
  return ring;
}


int HOId::GetSector() const {
  int sector = 0;
  if (iPhi > 71 && iPhi < 5  ) sector =1;
  if (iPhi > 4  && iPhi < 11 ) sector =2;
  if (iPhi > 10 && iPhi < 17 ) sector =3;
  if (iPhi > 16 && iPhi < 23 ) sector =4;
  if (iPhi > 22 && iPhi < 29 ) sector =5;
  if (iPhi > 28 && iPhi < 35 ) sector =6;
  if (iPhi > 34 && iPhi < 41 ) sector =7;
  if (iPhi > 40 && iPhi < 47 ) sector =8;
  if (iPhi > 46 && iPhi < 53 ) sector =9;
  if (iPhi > 52 && iPhi < 59 ) sector =10;
  if (iPhi > 58 && iPhi < 65 ) sector =11;
  if (iPhi > 64 && iPhi < 71 ) sector =12;

  return sector;
}


int HOId::GetTrayID() const {
  int trayID = -1;

  if (iPhi%6 == 5 ) trayID = 0; //loweset tray
  if (iPhi%6 == 0 ) trayID = 1; //loweset+1
  if (iPhi%6 == 1 ) trayID = 2; //loweset+2
  if (iPhi%6 == 2 ) trayID = 3; //loweset+3
  if (iPhi%6 == 3 ) trayID = 4; //loweset+4
  if (iPhi%6 == 4 ) trayID = 5; //loweset+5

  return trayID;
}


int HOId::GetTileID() const {
  int tile = iEta * iPhi;
  // tileID tile;
  // tile.eta = iEta;
  // tile.phi = iPhi;
  return  tile;
}


// std::ostream& operator<<( std::ostream& os, const HOId & id ) {
//   //  os << " Wh:"<< id.wheel()
//   ///     << " Se:"<< id.sector()
//     os <<" ";
  
//   return os;
// }



