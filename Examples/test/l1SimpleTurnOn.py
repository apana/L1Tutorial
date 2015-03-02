import FWCore.ParameterSet.Config as cms

#############################################################

nevts=-100
histofile = "l1SimpleTurnOns_WToENu.root"
inputfile = "file:L1Emulator_Stage1_PP_plusRECO_WToENu.root"

#############################################################

process= cms.Process('L1SimpleAnalysis')

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag.connect = cms.string('frontier://FrontierProd/CMS_COND_31X_GLOBALTAG')
process.GlobalTag.globaltag = cms.string('MCRUN2_73_V7::All')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(nevts)
)

# use TFileService for output histograms
process.TFileService = cms.Service("TFileService",
                              fileName = cms.string(histofile)
                              )
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputfile)
)

luminosityDirectory = "startup"
useXmlFile="L1Menu_Collisions2015_25ns_v1_L1T_Scales_20101224_Imp0_0x102f.xml"
process.load('L1TriggerConfig.L1GtConfigProducers.l1GtTriggerMenuXml_cfi')
process.l1GtTriggerMenuXml.TriggerMenuLuminosity = luminosityDirectory
process.l1GtTriggerMenuXml.DefXmlFile = useXmlFile

process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')

######################################################################

l1extra="l1extraParticlesFromStage1"; ## L1Extra from L1Emulator
# l1extra="l1extraParticles"; ## L1Extra from original processing

inputGtDigis="simGtDigis::L1TEMULATION" ## from L1Emulator
# inputGtDigis="gtDigis"  ## GT from original processing

process.l1turnon = cms.EDAnalyzer("L1SimpleTurnOn",
                                  L1GtRecordInputTag  = cms.InputTag(inputGtDigis),
                                  PFJetAlgorithm      = cms.InputTag("ak4PFJetsCHS"),
                                  muons               = cms.InputTag("muons"),
                                  electrons           = cms.InputTag("gedGsfElectrons"),
                                  L1JetTriggerBitName = cms.string("L1_SingleS1Jet68"), ##  e.g. "L1_SingleJet36" , "L1_SingleJet68", "L1_SingleJet92"
                                  L1MuonTriggerBitName= cms.string("L1_SingleMu16"), ##  e.g. "L1_SingleMu16"  , "L1_SingleMu3", "L1_SingleMu7"
                                  L1EGTriggerBitName  = cms.string("L1_SingleEG20"), ##  e.g. "L1_SingleEG12"  , "L1_SingleMu5", "L1_SingleMu30"
                                  L1JetThresholdForL1Extra  =cms.double(68.),
                                  L1MuonThresholdForL1Extra =cms.double(16.),
                                  L1EGThresholdForL1Extra   =cms.double(20.),
                                  l1collections       = cms.InputTag(l1extra)
                               )
process.p1 = cms.Path(process.l1turnon)
