#include "L1Tutorial/Examples/interface/L1SimpleTurnOn.h"

// Get the algorithm of the jet collections we will read from the .cfg file
// which defines the value of the strings CaloJetAlgorithm and GenJetAlgorithm.
L1SimpleTurnOn::L1SimpleTurnOn( const ParameterSet & cfg ) {
  cout << " Beginning L1SimpleTurnOn Job " << endl;
  l1GtRecordInputTag      = cfg.getParameter<InputTag>( "L1GtRecordInputTag" );
  PFJetAlgorithm          = cfg.getParameter<InputTag>( "PFJetAlgorithm"  );
  l1JetTriggerBitName     = cfg.getParameter<string> ("L1JetTriggerBitName");
  JetThreshold            = cfg.getParameter<double> ("L1JetThresholdForL1Extra");
  l1CollectionsTag_       = cfg.getParameter< InputTag > ("l1collections");

  errCnt=0;
  initL1=false;
}

void L1SimpleTurnOn::beginJob() {

  int nbins=50;
  Double_t xmin=0.,xmax=250.;

  evtCounter    =  fs->make<TH1F>("EventCounter","Event Counter",5,0.,5.);
  JetPtL        =  fs->make<TH1F>( "JetPt"         , "p_{T} of leading PFJets", nbins, xmin, xmax );
  JetPtL_Trig   =  fs->make<TH1F>( "JetPtL_Trig"   , "p_{T} of leading PFJets -- Triggered" , nbins, xmin, xmax );
  JetPtL_L1Extra=  fs->make<TH1F>( "JetPtL_L1Extra", "p_{T} of leading PFJets -- From L1Extra Emulation" , nbins, xmin, xmax );
}

void L1SimpleTurnOn::analyze( const Event& evt, const EventSetup& es ) {

  string errMsg("");
  doL1Jets=true; doPFJets=true;

  //Get the collections
  Handle<PFJetCollection> pfJets, pfJetsDummy;
  Handle<l1extra::L1JetParticleCollection> l1CenJets,l1ForJets,l1TauJets,l1jetsDummy;

  evt.getByLabel( PFJetAlgorithm,  pfJets );
  if (!pfJets.isValid())   { errMsg=errMsg + "  -- No PFJets";  pfJets   = pfJetsDummy  ; doPFJets   =false;}

  InputTag L1CenJetTag(edm::InputTag(l1CollectionsTag_.label(),"Central"));
  //InputTag L1CenJetTag(edm::InputTag(l1CollectionsTag_.label(),"cenJets"));
  evt.getByLabel(L1CenJetTag,l1CenJets);
  if (! l1CenJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1CenJetTag.label() ;
    l1CenJets = l1jetsDummy; doL1Jets=false;}

  InputTag L1ForJetTag(edm::InputTag(l1CollectionsTag_.label(),"Forward"));
  //InputTag L1ForJetTag(edm::InputTag(l1CollectionsTag_.label(),"forJets"));
  evt.getByLabel(L1ForJetTag,l1ForJets);
  if (! l1ForJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1ForJetTag.label() ;
    l1ForJets = l1jetsDummy; doL1Jets=false;}

  InputTag L1TauJetTag(edm::InputTag(l1CollectionsTag_.label(),"Tau"));
  //InputTag L1TauJetTag(edm::InputTag(l1CollectionsTag_.label(),"tauJets"));
  evt.getByLabel(L1TauJetTag,l1TauJets);
  if (! l1TauJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1TauJetTag.label() ;
    l1TauJets = l1jetsDummy; doL1Jets=false;}

  // Get L1 GT info
  bool gotL1=true;
  evtCounter->Fill(0.);

  //Get the L1GT collections
  edm::ESHandle<L1GtTriggerMenu> menuRcd;
  es.get<L1GtTriggerMenuRcd>().get(menuRcd) ;

  Handle<L1GlobalTriggerReadoutRecord>   gtRecord;
  evt.getByLabel(l1GtRecordInputTag, gtRecord);

  if (!gtRecord.isValid()) {

    cout << "\nL1GlobalTriggerReadoutRecord with \n  "
	 << l1GtRecordInputTag << "\nnot found"
      "\n  --> returning false by default!\n" << endl;

    gotL1=false;
    
  }


  if (gotL1) {
    evtCounter->Fill(1.);
    getL1Results(*gtRecord.product(), *menuRcd.product());
    if (doL1Jets) L1JetAnalysis(*pfJets,*l1CenJets,*l1ForJets,*l1TauJets,checkTriggerBit(l1JetTriggerBitName),JetThreshold);
  }

  if ((errMsg != "") && (errCnt < errMax())){
    errCnt=errCnt+1;
    errMsg=errMsg + ".";
    cout << "%L1SimpleTurnOn-Warning" << errMsg << endl;
    if (errCnt == errMax()){
      errMsg="%L1JetPLots-Warning -- Maximum error count reached -- No more messages will be printed.";
      cout << errMsg << endl;
    }
  }

}

void L1SimpleTurnOn::L1JetAnalysis(const reco::PFJetCollection& pfJets,
				   const l1extra::L1JetParticleCollection& l1CenJets,
				   const l1extra::L1JetParticleCollection& l1ForJets,
				   const l1extra::L1JetParticleCollection& l1TauJets,
				   const bool trigFired,
				   const double threshold
				   ) {

  // cout << "% Number of pfjets: " << pfJets.size() << endl;


  // try to emulate the jet bits from the L1Extra objects
  double maxL1Cen=0., maxL1For=0., maxL1Tau=0.;
  if (l1CenJets.size()>0) maxL1Cen=l1CenJets.at(0).pt();
  if (l1ForJets.size()>0) maxL1For=l1ForJets.at(0).pt();
  if (l1TauJets.size()>0) maxL1Tau=l1TauJets.at(0).pt();

  double maxL1=maxL1Cen;
  if (maxL1For>maxL1) maxL1=maxL1For;
  if (maxL1Tau>maxL1) maxL1=maxL1Tau;


  bool l1ExtraFired = false;
  if (maxL1>=threshold) l1ExtraFired=true;
  //############################################################


  if (doPFJets && pfJets.size()>0){
    PFJetCollection::const_iterator pf = pfJets.begin();
    if (checkPFJetID(pf)){
      JetPtL->Fill( pf->pt() );
      if (trigFired)    JetPtL_Trig ->Fill( pf->pt() );
      if (l1ExtraFired) JetPtL_L1Extra ->Fill( pf->pt() );
    }
  }

}

void L1SimpleTurnOn::endJob() {

}

bool L1SimpleTurnOn::checkTriggerBit(const string l1AlgoName){

    // get results for particular L1 algorithm
    bool l1Fired=false;

    trig_iter=l1TriggerDecision.find(l1AlgoName);
    if (trig_iter==l1TriggerDecision.end()){
      cout << "Could not find trigger path with name: " << l1AlgoName << endl;
    }else{
      l1Fired=trig_iter->second;
    }
    return l1Fired;
}

void L1SimpleTurnOn::getL1Results(const L1GlobalTriggerReadoutRecord& gtRecord,
			  const L1GtTriggerMenu& menu
			) {

  // this will get the decision word *before* masking disabled bits
  const DecisionWord dWord = gtRecord.decisionWord();  
  numberTriggerBits= dWord.size();
  if (numberTriggerBits > nL1BitsMax) {
    cout << "number of Trigger Bits exceed max allowed in trigger array -- truncating" << "\n";
    numberTriggerBits = nL1BitsMax;
  }

  const TechnicalTriggerWord technicalTriggerWordBeforeMask = gtRecord.technicalTriggerWord();
  numberTechnicalTriggerBits = technicalTriggerWordBeforeMask.size();
  if (numberTechnicalTriggerBits > nL1BitsMax) {
    cout << "number of Technical Trigger Bits exceed max allowed in trigger array -- truncating" << "\n";
    numberTechnicalTriggerBits = nL1BitsMax;
  }


  if ( !initL1){
    initL1=true;
    cout << "\n  Number of Algorithm Trigger bits " << numberTriggerBits << "\n\n";
    cout << "\tBit \t L1 Algorithm " << endl;

    // get L1 menu from event setup
    for (CItAlgo algo = menu.gtAlgorithmMap().begin(); algo!=menu.gtAlgorithmMap().end(); ++algo) {
      cout << "Name: " << (algo->second).algoName() << " Alias: " << (algo->second).algoAlias() << endl;
      int itrig = (algo->second).algoBitNumber();
      algoBitToName[itrig] = TString( (algo->second).algoName() );
    }

    // technical trigger bits
    cout << "\n  Number of Technical Trigger bits " << numberTechnicalTriggerBits << "\n\n";
    for (CItAlgo techTrig = menu.gtTechnicalTriggerMap().begin(); techTrig != menu.gtTechnicalTriggerMap().end(); ++techTrig) {
      int itrig = (techTrig->second).algoBitNumber();
      techBitToName[itrig] = TString( (techTrig->second).algoName() );
      cout << "tech bit " << itrig << ": " << techBitToName[itrig] << " " << endl;
      
    } // end of if
  }

  //Fill maps with trigger bit info
  for (unsigned int iBit = 0; iBit < numberTriggerBits; ++iBit) {  

    bool accept = dWord[iBit];

      // fill the trigger map
    typedef std::map<string,bool>::value_type valType;
    trig_iter=l1TriggerDecision.find(algoBitToName[iBit]);
    if (trig_iter==l1TriggerDecision.end())
      l1TriggerDecision.insert(valType(algoBitToName[iBit],accept));
    else
      trig_iter->second=accept;
    }

  //now technical triggerbits
  for (unsigned int iBit = 0; iBit < numberTechnicalTriggerBits; ++iBit) {  

    bool accept = technicalTriggerWordBeforeMask[iBit];

      // fill the trigger map
    typedef std::map<string,bool>::value_type valType;
    trig_iter=l1TechTriggerDecision.find(techBitToName[iBit]);
    if (trig_iter==l1TechTriggerDecision.end())
      l1TechTriggerDecision.insert(valType(techBitToName[iBit],accept));
    else
      trig_iter->second=accept;
    }
}
 
bool L1SimpleTurnOn::checkPFJetID(const std::vector<reco::PFJet>::const_iterator jet){

  // loose id for PFJets

  bool jid=false;

  double chf   = jet->chargedHadronEnergyFraction();
  double nhf   = (jet->neutralHadronEnergy() + jet->HFHadronEnergy())/jet->energy();
  double phf   = jet->photonEnergyFraction();
  double elf   = jet->electronEnergyFraction();
  double chm   = jet->chargedHadronMultiplicity();
  //int nhm   = jet->neutralHadronMultiplicity();
  //int phm   = jet->photonMultiplicity();
  //int elm   = jet->electronMultiplicity();
  int npr   = jet->chargedMultiplicity() + jet->neutralMultiplicity();

  jid  = (npr>1 && phf<0.99 && nhf<0.99 && ((fabs(jet->eta())<=2.4 && elf<0.99 && chf>0 && chm>0) || fabs(jet->eta())>2.4)) ;

  return jid;

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1SimpleTurnOn);
