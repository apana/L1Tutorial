import FWCore.ParameterSet.Config as cms

process= cms.Process('L1TEMULATION')

######## User options ############################################## 

nevts=-100

OutputFile= "L1Emulator_Stage1_PP_plusRECO.root"

# GLOBALTAG = 'MCRUN2_73_V7'
# GLOBALTAG = 'PRE_LS172_V15::ALL'
# inputfile=["file:/uscmst1b_scratch/lpc1/lpctrig/apana/L1Upgrade/RelValTTBar_PU_PRE_LS172_V15_IdTestFullSim-v1_327A4C34-8571-E411-A632-68B59972BFCE.root"]
inputfile=[
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_1_1_nZA.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_2_1_bq4.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_3_1_Z1v.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_4_1_KTB.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_5_1_gFl.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_6_1_dv4.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_7_1_eBe.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_8_1_ATz.root",
    "file:/eos/uscms/store/user/lpctrig/apana/HATS/L1Emulator_plusRECO_9_1_teD.root"
           ]
# inputfile=["/store/relval/CMSSW_7_3_0/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PU25ns_MCRUN2_73_V7-v1/00000/0496C828-4981-E411-8142-0025905A60B8.root"]
# inputfile=["file:0496C828-4981-E411-8142-0025905A60B8_justRAW.root"]

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

## process.raw2digi_step= cms.Path(process.RawToDigi+process.dttfDigis)
process.raw2digi_step= cms.Path(process.RawToDigi)
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

## update some collection names
process.cscReEmulTriggerPrimitiveDigis.CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
process.cscReEmulTriggerPrimitiveDigis.CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")

####
import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
process.l1extraParticlesFromStage1 = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
process.l1extraParticlesFromStage1.muonSource = cms.InputTag( "gmtReEmulDigis" )
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
process.l1GtTrigReport.L1GtRecordInputTag = "simGtDigis"
process.l1GtTrigReport.PrintVerbosity = 1

#### run the paths and output

process.L1ExtraAndReport = cms.Path(process.l1extraParticlesFromStage1 + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring('drop *',
                                           'keep L1GlobalTrigger*_*_*_*',
                                           'keep l1extra*_*_*_L1TEMULATION',
                                           'keep *BXVector_*_*_L1TEMULATION',
                                           # 'keep *_*_*_L1TEMULATION',
                                           'keep recoPFJets_ak4PFJetsCHS__RECO',
                                           'keep recoMuons_muons__RECO',
                                           'keep recoGsfElectrons_gedGsfElectrons__RECO'
                                           ),                               
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )


process.L1simulation_step.remove(process.simRpcTechTrigDigis)
process.RawToDigi.remove(process.caloStage1Digis)
process.RawToDigi.remove(process.caloStage1LegacyFormatDigis)

process.schedule = cms.Schedule( process.raw2digi_step,
                                 # process.muonTP,
                                 process.reRunHCALTP,
                                 process.L1simulation_step,
                                 process.L1ExtraAndReport,
                                 process.o
                                 )
