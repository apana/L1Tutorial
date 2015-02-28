import FWCore.ParameterSet.Config as cms

process= cms.Process('L1TEMULATION')

######## User options ############################################## 

nevts=-100

OutputFile= "L1Emulator_Stage1_PP.root"

# GLOBALTAG = 'MCRUN2_73_V7'
# GLOBALTAG = 'PRE_LS172_V15::ALL'
inputfile=["file:/uscmst1b_scratch/lpc1/lpctrig/apana/L1Upgrade/RelValTTBar_PU_PRE_LS172_V15_IdTestFullSim-v1_327A4C34-8571-E411-A632-68B59972BFCE.root"]

## rawDataLabel="source"  ## DATA
rawDataLabel="rawDataCollector"  ## MC

############# import of standard configurations ####################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
# process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

# process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_CONDITIONS'
# process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
# process.GlobalTag.globaltag = GLOBALTAG


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG')
process.GlobalTag.globaltag = cms.string('MCRUN2_73_V7::All')

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.MessageLogger.categories.append('L1GtTrigReport')

### Input source ###################################################

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputfile)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(nevts)
)

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/SimL1Emulator_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration/StandardSequences/DigiToRaw_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')

process.raw2digi_step= cms.Path(process.RawToDigi+process.dttfDigis)
process.L1simulation_step = cms.Path(process.SimL1Emulator)

from L1Trigger.L1TCommon.customsPostLS1 import customiseSimL1EmulatorForPostLS1
process=customiseSimL1EmulatorForPostLS1(process)

## 
from L1Trigger.L1TCalorimeter.L1TCaloStage1_customForHLT import customiseL1EmulatorFromRaw
##import L1Trigger.L1TCalorimeter.L1TCaloStage1_customForHLT
process=customiseL1EmulatorFromRaw(process)

#####  Redo RCT Digis
process.simRctDigis.ecalDigis = cms.VInputTag( cms.InputTag( 'ecalDigis:EcalTriggerPrimitives' ) )
process.simRctDigis.hcalDigis = cms.VInputTag( cms.InputTag( 'simHcalTriggerPrimitiveDigis' ) )

# HCAL TP hack
process.load("L1Trigger.L1TCalorimeter.L1TRerunHCALTP_FromRaw_cff")
process.reRunHCALTP=cms.Path(process.L1TRerunHCALTP_FromRAW)

### include latest RCT calibrations from UCT
process.load("L1Trigger.L1TCalorimeter.caloStage1RCTLuts_cff")

process.dttfDigis = cms.EDProducer("DTTFFEDReader",
    verbose = cms.untracked.bool(False),
    DTTF_FED_Source = cms.InputTag("rawDataCollector")
)
process.simDttfDigis.DTDigi_Source = cms.InputTag("dttfDigis")
process.cscReEmulTriggerPrimitiveDigis.CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
process.cscReEmulTriggerPrimitiveDigis.CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")

### Global Trigger Emulator
import L1Trigger.GlobalTrigger.gtDigis_cfi
process.gtDigisFromStage1 = L1Trigger.GlobalTrigger.gtDigis_cfi.gtDigis.clone()
process.gtDigisFromStage1.GmtInputTag = 'simGmtDigis'
process.gtDigisFromStage1.GctInputTag = 'simCaloStage1LegacyFormatDigis'
process.gtDigisFromStage1.TechnicalTriggersInputTags = cms.VInputTag(cms.InputTag('simBscDigis'), 
                                                       cms.InputTag('simRpcTechTrigDigis'))

####
import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
process.l1extraParticlesFromStage1 = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
process.l1extraParticlesFromStage1.muonSource = cms.InputTag( "simGmtDigis" )
process.l1extraParticlesFromStage1.isolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','isoEm' )
process.l1extraParticlesFromStage1.nonIsolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','nonIsoEm' )
process.l1extraParticlesFromStage1.centralJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','cenJets' )
process.l1extraParticlesFromStage1.forwardJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','forJets' )
process.l1extraParticlesFromStage1.tauJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','tauJets' )
process.l1extraParticlesFromStage1.isoTauJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','isoTauJets' )
process.l1extraParticlesFromStage1.etTotalSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromStage1.etHadSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromStage1.etMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromStage1.htMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromStage1.hfRingEtSumsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromStage1.hfRingBitCountsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ) 


### Trigger report ################################################################

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.l1GtTrigReport.L1GtRecordInputTag = "gtDigisFromStage1"
process.l1GtTrigReport.PrintVerbosity = 1

#### run the paths and output

process.L1ExtraAndGT = cms.Path(process.l1extraParticlesFromStage1 + process.gtDigisFromStage1 + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring('drop *',
                                           'keep L1GlobalTrigger*_*_*_*',
                                           'keep l1extra*_*_*_L1TEMULATION',
                                           'keep *BXVector_*_*_L1TEMULATION',

                                           ),                               
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )


process.L1simulation_step.remove(process.simRpcTechTrigDigis)
process.RawToDigi.remove(process.caloStage1Digis)
process.RawToDigi.remove(process.caloStage1LegacyFormatDigis)

process.schedule = cms.Schedule( process.raw2digi_step,
                                 process.reRunHCALTP,
                                 process.L1simulation_step,
                                 process.L1ExtraAndGT,
                                 process.o
                                 )
