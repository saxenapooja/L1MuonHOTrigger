# Auto generated configuration file
# using: 
# Revision: 1.381.2.7 
# Source: /local/reps/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: SingleMuFlatLogPt_100MeVto2TeV_cfi.py -s GEN,SIM,DIGI,L1 --conditions START53_V7A::All --eventcontent FEVTDEBUG --no_exec
import FWCore.ParameterSet.Config as cms
process = cms.Process('L1')

## Configuration parameters

# The GlobalTag 
# must be defined according to the release and the configuration you need
# this is release/geometry depended, what is now here has been tested to make the
# workflow work in CMSSW_6_2_12_patch1 using 2012 geomery and was not used for 
# big sample production, only technical workflow was tested
#globalTag = "START62_V1"
#globalTag = "START71_V8"
#globalTag = "PHYS14_25_V1::All"
globalTag = "MCRUN2_71_V1::All"
#globalTag = "POSTLS170_V3::All"   

# The eta range for GEN muon production
# the present DTTF goes up to |eta|<1.04, the BarrelTF will go roughly up to
# |eta|<0.9, here putting 1.05 as limit to include scattering of muons 
# before reaching the muon chambers
minEta = -1.26
maxEta =  1.26

# The phi range for GEN muon production
# presently set to study the performance of one single sector plus neighbours
minPhi = - 3.14159265359/6.
maxPhi =   3.14159265359/6.

# The pT range for GEN muon production
# presently set using limits of pt for muons to reach the barrel spectrometer
# and the present DTTF pT scale overflow
minPt = 3
maxPt = 140

# The sign of the muon
# -1 for mu- and +1 for mu+
muonCharge = +1

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')

# new_geometry added on  Nov 28, 2014
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2015_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
#process.load('Configuration.StandardSequences.GeometrySimDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic8TeVCollision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Extra_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Input source
process.source = cms.Source("EmptySource")
process.options = cms.untracked.PSet(

)


# suggested by Piet, added on Nov_28, 2014 
# source: https://github.com/pietverwilligen/MyCmsDriverCommands/blob/master/ConfigFileSnippets/RPC_Digitization_ReadLocalConditions.py 
from CondCore.DBCommon.CondDBSetup_cfi import *
process.noisesfromprep = cms.ESSource("PoolDBESSource",
                                      # connect = cms.string('sqlite_file:RPC_Phase2UpgradeStudies_mc.db'),                                                      
				      # connect = cms.string('sqlite_file:RPC_Eff2012_PhaseII_mc.db'),    
                                      # connect = cms.string('sqlite_file:RPC_Eff2012_256Strips_mc.db'),                                                    
                                      # connect = cms.string('sqlite_file:RPC_dataDrivenCondition_RPCEta2Upscope_mc.db'),   
				      connect = cms.string('sqlite_file:RPC_3108Rolls_BkgAtLumi1_14TeV_mc.db'),
                                      DBParameters = cms.PSet(
		messageLevel = cms.untracked.int32(0),
		authenticationPath = cms.untracked.string('.'),
		authenticationMethod = cms.untracked.uint32(1)
		),
                                      timetype = cms.string('runnumber'),
                                      toGet = cms.VPSet(cms.PSet(
			record = cms.string('RPCStripNoisesRcd'),
#			label = cms.untracked.string("noisesfromprep"),
			# tag = cms.string('RPC_Phase2UpgradeStudies_mc')
			# tag = cms.string('RPC_Eff2012_PhaseII_mc')
			# tag = cms.string('RPC_Eff2012_256Strips_mc') 
			# tag = cms.string('RPC_dataDrivenCondition_RPCEta2Upscope_mc')  
			tag = cms.string('RPC_3108Rolls_BkgAtLumi1_14TeV_mc')
			)
                                                        )
                                      )
process.es_prefer_noisesfromprep=cms.ESPrefer("PoolDBESSource", "noisesfromprep")

#process.clsfromprep = cms.ESSource("PoolDBESSource",
#    # connect = cms.string('sqlite_file:RPCClusterSize__upscope_mc_v1.db'),
#    connect = cms.string('sqlite_file:RPCClusterSize_PhaseII_mc.db'),
#    DBParameters = cms.PSet(
#        messageLevel = cms.untracked.int32(0),
#        authenticationPath = cms.untracked.string('.'),
#        authenticationMethod = cms.untracked.uint32(1)
#        ),
#    timetype = cms.string('runnumber'),
#    toGet = cms.VPSet(cms.PSet(
#        record = cms.string('RPCClusterSizeRcd'),
#        # tag = cms.string('RPCClusterSize__upscope_mc_v1')
#        tag = cms.string('RPCClusterSize_PhaseII_mc')
#        )
#    )
#)
#process.es_prefer_clsfromprep=cms.ESPrefer("PoolDBESSource" , "clsfromprep")


if muonCharge > 0 :
	chargeTag='Plus'
else :
        chargeTag='Minus'

configTag = 'SingleMu' + chargeTag + '_FlatPt_' + str(minPt) + 'to' + str(maxPt) \
    + '_eta' + str(minEta) + 'to' + str(maxEta) + '_phi' + str(int(minPhi*100)/100.) \
    + 'to' + str(int(maxPhi*100)/100.)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
	version = cms.untracked.string('$Revision: 1.4 $'),
	annotation = cms.untracked.string(configTag),
	name = cms.untracked.string('PyReleaseValidation')
	)


# Output definition
process.FEVTDEBUGoutput = cms.OutputModule("PoolOutputModule",
  splitLevel = cms.untracked.int32(0),
  eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
  outputCommands = process.FEVTDEBUGEventContent.outputCommands,
  fileName = cms.untracked.string(configTag+'_GEN_SIM_DIGI_L1.root'),
  dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
  ),
  SelectEvents = cms.untracked.PSet(
       SelectEvents = cms.vstring('generation_step')
  )
)

# Additional output definition

# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
process.mix.digitizers =  cms.PSet(process.theDigitizersValid)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, globalTag+'::All', '')

# http://cmslxr.fnal.gov/lxr/source/Configuration/AlCa/python/autoCond.py?view=markup
from Configuration.AlCa.autoCond import autoCond
#process.GlobalTag.globaltag = autoCond['run2_design'] #PRE_LS172_V15::All

process.generator = cms.EDProducer("FlatRandomPtGunProducer",
  PGunParameters = cms.PSet(
        MinPt  = cms.double(minPt),
	MaxPt  = cms.double(maxPt),
        PartID = cms.vint32(-13 * muonCharge),        
        MaxPhi = cms.double(maxPhi),
	MinPhi = cms.double(minPhi),
	MaxEta = cms.double(maxEta),
        MinEta = cms.double(minEta)        
	),
       Verbosity = cms.untracked.int32(0),
       psethack = cms.string('single mu pt ' + str(minPt) + 'to' + str(maxPt)),
       AddAntiParticle = cms.bool(False),  #need *single* muons dammit
       firstRun = cms.untracked.uint32(1)
)


# Path and EndPath definitions
process.generation_step   = cms.Path(process.pgen)
process.simulation_step   = cms.Path(process.psim)
process.digitisation_step = cms.Path(process.pdigi)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step     = cms.Path(process.DigiToRaw)
process.raw2digi_step     = cms.Path(process.RawToDigi)
process.L1extra_step      = cms.Path(process.L1Extra)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGoutput_step = cms.EndPath(process.FEVTDEBUGoutput)


#outCommands = cms.untracked.vstring('drop *')
#outCommands.append('keep *_simMuonDTDigis_*_*')
#outCommands.append('keep *_simMuonRPCDigis_*_*')
#outCommands.append('keep *_simMuonCSCDigis_*_*')
#outCommands.append('keep *_genParticles_*_*')
#outCommands.append('keep *_simCsctfTrackDigis_*_*')
#outCommands.append('keep *_simDttfDigis_*_*')
#outCommands.append('keep *_simGmtDigis_*_*')
#outCommands.append('keep *_simRpcTriggerDigis_*_*')
#outCommands.append('keep *_simDtTriggerPrimitiveDigis_*_*')
#outCommands.append('keep *_simCscTriggerPrimitiveDigis_*_*')
#outCommands.append('keep *_L1ITMuTriggerPrimitives_*_*')
#outCommands.append('keep *_MBLTProducer_*_*')
#outCommands.append('keep *_MBTracksProducer_*_*')
#outCommands.append('keep *_L1ITMuonBarrelPrimitiveProducer_*_*')
#outCommands.append('keep *_*Converter_*_*')
#outCommands.append('keep *_*Matcher_*_*')
#
#process.FEVTDEBUGoutput.outputCommands = outCommands
#
# Schedule definition
process.schedule = cms.Schedule(process.generation_step,
				process.genfiltersummary_step,
				process.simulation_step,
				process.digitisation_step,
				process.L1simulation_step,
				process.digi2raw_step,
				process.raw2digi_step,
				process.L1extra_step,
				process.endjob_step,
				process.FEVTDEBUGoutput_step
				)

# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 

