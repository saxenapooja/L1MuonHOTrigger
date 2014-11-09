#ifndef HODetId_H
#define HODetId_H

/** \class HIId
 *  DetUnit identifier for Hadron-Outer.
 *  
 *  \author Pooja Saxena
 */

#include <DataFormats/DetId/interface/DetId.h>
#include <iosfwd>


//class
class HOId : public DetId {
  
 private:
  int ieta_;
  int iphi_;
  
 public:
  /// Default constructor. 
  /// Fills the common part in the base and leaves 0 in all other fields
  HOId();
  

  /// Construct from a packed id. It is required that the packed id represents 
  ///valid HO DetId (proper Detector & SubDet fields), otherwise an exception is thrown.
  HOId(uint32_t id);
  HOId(DetId id);

  
  /* struct tileID { */
  /*   int eta; */
  /*   int phi; */
  /* }; */
  

  /// Construct from fully qualified identifier.
  HOId(int ieta_, int iphi_);

  signed eta() const;
  unsigned phi() const;
  int ring()   const;
  int sector() const;
  int trayId() const;
  int tileId() const;
  int bx() const;
  
  static const int maxEta = 15;
  static const int minEta = -15;
  static const int maxPhi = 72;
  static const int minPhi = 1;
  
  static const int minSectorId =     1; //0 for DT
  static const int maxSectorId =    12; // 14 for DT
  static const int minWheelId  =    -2;
  static const int maxWheelId  =     2;
  static const int maxTraysID  =     6;
  static const int minLayerId  =     1;
  static const int maxLayerId  =     2;
  

 protected:
  // Perform a consistency check of the id with a HO Id
  // It thorows an exception if this is not the case
  void checkHOId();
};


//std::ostream& operator<<( std::ostream& os, const HOId& id );
#endif

