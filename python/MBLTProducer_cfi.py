import FWCore.ParameterSet.Config as cms

MBLTProducer = cms.EDProducer(
    'MBLTProducer',
    TriggerPrimitiveSrc = cms.InputTag('L1ITMuTriggerPrimitives'),
    MaxDeltaPhi         = cms.double( 0.05 ),
    MaxDeltaPhiForHO    = cms.double( 0.5 ),
    AddHO               = cms.bool(False),
    MaxStationForHO     = cms.int32(3)
    )
