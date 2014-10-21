import FWCore.ParameterSet.Config as cms

from L1Trigger.DTTrackFinder.dttfDigis_cfi import dttfDigis as tmpDttfDigis

DTBunchCrossingCleanerCfg = cms.PSet(
    bxWindowSize = cms.int32(1), #look one BX ahead and behind     
    )

L1ITMuTriggerPrimitives = cms.EDProducer(
    'L1ITMuTriggerPrimitiveProducer',
    DT   = cms.PSet( collectorType = cms.string('DTCollector'),
                     src = cms.InputTag('simDtTriggerPrimitiveDigis'),
                     src2 = cms.InputTag('simDtTriggerPrimitiveDigis'),
                     BX_min = cms.int32(tmpDttfDigis.BX_min.value()),
                     BX_max = cms.int32(tmpDttfDigis.BX_max.value()),
                     runBunchCrossingCleaner = cms.bool(False),
                     bxCleanerCfg = DTBunchCrossingCleanerCfg ),
    
    RPC = cms.PSet( collectorType = cms.string('RPCCollector'),
                    src = cms.InputTag('simMuonRPCDigis'),
                    src2 = cms.InputTag('') ),
    
    CSC  = cms.PSet( collectorType = cms.string('CSCCollector'), 
                     src = cms.InputTag('simCscTriggerPrimitiveDigis',
                                        'MPCSORTED') ,
                     src2 = cms.InputTag('','') )
    )
