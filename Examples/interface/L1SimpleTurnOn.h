#ifndef MyL1_L1SimpleTurnOn_h
#define MyL1_L1SimpleTurnOn_h
/* \class L1SimpleTurnOn
 *
 * \author Leonard Apanasevich
 *
 * \version 1
 *
 */
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>
#include <TFile.h>
#include <TCanvas.h>
#include <cmath>
#include <map>

using namespace edm;
using namespace reco;
using namespace std;

class L1SimpleTurnOn : public edm::EDAnalyzer {
public:
  L1SimpleTurnOn( const edm::ParameterSet & );

private:
  void beginJob();
  void analyze( const edm::Event& , const edm::EventSetup& );
  void endJob();

  void L1JetAnalysis(const PFJetCollection&,
		     const l1extra::L1JetParticleCollection&,
		     const l1extra::L1JetParticleCollection&,
		     const l1extra::L1JetParticleCollection&,
		     const bool trigFired,
		     const double threshold
		     );



  // input parameters
  edm::InputTag PFJetAlgorithm, l1CollectionsTag_,l1GtRecordInputTag;
  string l1JetTriggerBitName;
  double JetThreshold;

  // histograms
  edm::Service<TFileService> fs;
  TH1F *evtCounter;
  TH1F *JetPtL, *JetPtL_Trig, *JetPtL_L1Extra;

  int errCnt;
  bool initL1;
  const int errMax(){return 100;}

  bool checkPFJetID(const std::vector<reco::PFJet>::const_iterator);
  void fillHist(const TString& histName, const Double_t& value, const Double_t& wt=1.0);
  void fill2DHist(const TString& histName, const Double_t& x,const Double_t& y,const Double_t& wt=1.0);
  void getL1Results(const L1GlobalTriggerReadoutRecord&,
		    const L1GtTriggerMenu&
		  );
  bool checkTriggerBit(const string Name);
  bool doPFJets;
  bool doL1Jets;


  // variables for storing results from running the GT menu
  unsigned int numberTriggerBits;
  unsigned int numberTechnicalTriggerBits;

  static const size_t nL1BitsMax=128;
  string algoBitToName[nL1BitsMax];
  string techBitToName[nL1BitsMax];

  // use the map function to access trigger information
  std::map <string,bool> l1TriggerDecision,l1TechTriggerDecision;
  std::map<string,bool>::iterator trig_iter;

};

#endif
