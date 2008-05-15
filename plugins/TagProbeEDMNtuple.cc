// -*- C++ -*-
//
// Package:    TagProbeEDMNtuple
// Class:      TagProbeEDMNtuple
// 
/**\class TagProbeEDMNtuple TagProbeEDMNtuple.cc MuonAnalysis/TagProbeEDMNtuple/src/TagProbeEDMNtuple.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Nadia Adam
//         Created:  Mon May  5 08:47:29 CDT 2008
// $Id: TagProbeEDMNtuple.cc,v 1.4 2008/05/11 12:32:12 neadam Exp $
//
//


// system include files
#include <cmath>

// user include files
#include "MuonAnalysis/TagAndProbe/interface/TagProbeEDMNtuple.h"
#include "MuonAnalysis/TagAndProbe/interface/CandidateAssociation.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/HLTPathStatus.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronIsolationAssociation.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectronFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/RecoCandidate/interface/FitResult.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidateIsolation.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "Math/GenVector/VectorUtil.h"
#include "PhysicsTools/HepMCCandAlgos/interface/MCCandMatcher.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"

//
// constants, enums and typedefs
//
using namespace std; 
using namespace reco; 
using namespace edm;
using namespace l1extra;
using namespace trigger;

//
// static data member definitions
//

//
// constructors and destructor
//
TagProbeEDMNtuple::TagProbeEDMNtuple(const edm::ParameterSet& iConfig)
{
   cout << "Here in TagProbeEDMNtuple init!" << endl;
   
   candType_ = iConfig.getUntrackedParameter<string>("tagProbeType","Muon");
   candPDGId_ = 13;
   if( candType_ != "Muon" ) candPDGId_ = 11;

   // Get the id's of any MC particles to store.
   vector<int> defaultPIDs;
   mcParticles_ = iConfig.getUntrackedParameter< vector<int> >(
      "mcParticles",defaultPIDs);

   vector<int> defaultPPIDs;
   mcParents_ = iConfig.getUntrackedParameter< vector<int> >(
      "mcParents",defaultPPIDs);

   if( mcParents_.size() != mcParticles_.size() )
   {
      mcParents_.clear();
      for(unsigned int i=0; i<mcParticles_.size(); ++i )
      {
	 mcParents_.push_back(0);
      }
   }

   // ********** Reco Tracks ********** //
   vector< edm::InputTag > dTrackTags;
   dTrackTags.push_back( edm::InputTag("ctfWithMaterialTracks"));
   trackTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "trackTags",dTrackTags);
   // ********************************* //

   // ********** Tag-Probes ********** //
   vector< edm::InputTag > defaultTagProbeMapTags;
   tagProbeMapTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "tagProbeMapTags",defaultTagProbeMapTags);
   for( int i=0; i<(int)tagProbeMapTags_.size(); ++i )
      cout << tagProbeMapTags_[i] << endl;
   // ******************************** //

   // Make sure vector sizes are correct!
   int map_size = (int)tagProbeMapTags_.size();
   const edm::InputTag dBlankTag("Blank");

   // ********** Candidate collections ********** //
   const edm::InputTag dGenParticlesTag("genParticles");
   genParticlesTag_ = iConfig.getUntrackedParameter<edm::InputTag>(
      "genParticlesTag",dGenParticlesTag);

   vector< edm::InputTag > defaultTagCandTags;
   tagCandTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "tagCandTags",defaultTagCandTags);
   for( int i=0; i<(int)tagCandTags_.size(); ++i ) 
      cout << tagCandTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)tagCandTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)tagCandTags_.size()); ++i ) 
	 tagCandTags_.push_back(dBlankTag);
   } 

   vector< edm::InputTag > defaultAllProbeCandTags;
   allProbeCandTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "allProbeCandTags",defaultAllProbeCandTags);
   for( int i=0; i<(int)allProbeCandTags_.size(); ++i ) 
      cout << allProbeCandTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)allProbeCandTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)allProbeCandTags_.size()); ++i ) 
	 allProbeCandTags_.push_back(dBlankTag);
   } 

   vector< edm::InputTag > defaultPassProbeCandTags;
   passProbeCandTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "passProbeCandTags",defaultPassProbeCandTags);
   for( int i=0; i<(int)passProbeCandTags_.size(); ++i ) 
      cout << passProbeCandTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)passProbeCandTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)passProbeCandTags_.size()); ++i ) 
	 passProbeCandTags_.push_back(dBlankTag);
   } 
   // ******************************************* //

   // ********** Truth matching ********** //
   vector< edm::InputTag > defaultTagTruthMatchMapTags;
   tagTruthMatchMapTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "tagTruthMatchMapTags",defaultTagTruthMatchMapTags);
   for( int i=0; i<(int)tagTruthMatchMapTags_.size(); ++i ) 
      cout << tagTruthMatchMapTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)tagTruthMatchMapTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)tagTruthMatchMapTags_.size()); ++i ) 
	 tagTruthMatchMapTags_.push_back(dBlankTag);
   } 

   vector< edm::InputTag > defaultAllProbeTruthMatchMapTags;
   allProbeTruthMatchMapTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "allProbeTruthMatchMapTags",defaultAllProbeTruthMatchMapTags);
   for( int i=0; i<(int)allProbeTruthMatchMapTags_.size(); ++i ) 
      cout << allProbeTruthMatchMapTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)allProbeTruthMatchMapTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)allProbeTruthMatchMapTags_.size()); ++i ) 
	 allProbeTruthMatchMapTags_.push_back(dBlankTag);
   } 

   vector< edm::InputTag > defaultPassProbeTruthMatchMapTags;
   passProbeTruthMatchMapTags_ = iConfig.getUntrackedParameter< vector<edm::InputTag> >(
      "passProbeTruthMatchMapTags",defaultPassProbeTruthMatchMapTags);
   for( int i=0; i<(int)passProbeTruthMatchMapTags_.size(); ++i )
      cout << passProbeTruthMatchMapTags_[i] << endl;
   // Make sure the arrays won't cause a seg fault!
   if( (int)passProbeTruthMatchMapTags_.size() < map_size )
   {
      cout << "Warning: Number of TagProbe maps bigger than number of tag arrays!" << endl;
      for( int i=0; i<(map_size-(int)passProbeTruthMatchMapTags_.size()); ++i ) 
	 passProbeTruthMatchMapTags_.push_back(dBlankTag);
   } 
   // ************************************ //

   // **************** Trigger ******************* //
   const edm::InputTag dTriggerEventTag("triggerSummaryRAW");
   triggerEventTag_ = 
      iConfig.getUntrackedParameter<edm::InputTag>("triggerEventTag",dTriggerEventTag);

   const edm::InputTag dHLTL1Tag("SingleMuIsoLevel1Seed");
   hltL1Tag_ = iConfig.getUntrackedParameter<edm::InputTag>("hltL1Tag",dHLTL1Tag);

   const edm::InputTag dHLTTag("SingleMuIsoL3IsoFiltered");
   hltTag_ = iConfig.getUntrackedParameter<edm::InputTag>("hltTag",dHLTTag);

   delRMatchingCut_ = iConfig.getUntrackedParameter<double>("triggerDelRMatch",0.15);
   delPtRelMatchingCut_ = iConfig.getUntrackedParameter<double>("triggerDelPtRelMatch",0.15);
   // ******************************************** //

   // ******* Register the output products ******* //
   produces< int >( "run" ).setBranchAlias( "Run" );     /* Run number */
   produces< int >( "event" ).setBranchAlias( "Event" ); /* Event number */

   produces< int >( "nl1" ).setBranchAlias( "nL1" );     /* Number of L1 triggers. */
   produces< int >( "nhlt" ).setBranchAlias( "nHLT" );   /* Number of HLT triggers. */

   // If the user has requested information about specific MC particle
   // types we will store this in the event also
   for( int i=0; i<(int)mcParticles_.size(); ++i )
   {
      stringstream nstream;
      nstream << "MC" << mcParticles_[i];
      string prefix = nstream.str();

      string name = "n"+prefix;
      produces< int >(name.c_str()).setBranchAlias(name.c_str());

      name = prefix+"ppid"; /* Particle PDGId of parent. */
      produces< vector<int> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"pbc";  /* Particle barcode of parent. */
      produces< vector<int> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"pid";  /* Particle PDGId. */
      produces< vector<int> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"bc";   /* Particle barcode. */
      produces< vector<int> >(name.c_str()).setBranchAlias(name.c_str());

      name = prefix+"mass"; /* Particle mass. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"p";    /* Particle momentum. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"pt";   /* Particle transverse momentum. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"px";   /* Particle x-momentum. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"py";   /* Particle y-momentum. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"pz";   /* Particle z-momentum. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"e";    /* Particle energy. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"eta";  /* Particle psuedorapidity. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
      name = prefix+"phi";  /* Particle phi. */
      produces< vector<float> >(name.c_str()).setBranchAlias(name.c_str());
 
   }

   produces< int >( "nrTP" ).setBranchAlias( "nrTP" );                    /* Number of Tag-Probe entries */
   produces< std::vector<int> >( "TPtype" ).setBranchAlias( "TPtype" );   /* Type of eff measurement for these TP's */
   produces< std::vector<int> >( "TPtrue" ).setBranchAlias( "TPtrue" );   /* Is this a true Z candidate? (If MC) */
   produces< std::vector<int> >( "TPppass" ).setBranchAlias( "TPppass" ); /* Is this a passing probe? */ 
   produces< std::vector<int> >( "TPl1" ).setBranchAlias( "TPl1" );       /* Is the tag matched to a L1 trigger? */
   produces< std::vector<int> >( "TPhlt" ).setBranchAlias( "TPhlt" );     /* Is the tag matched to a HL trigger? */

   produces<std::vector<float> >( "TPmass" ).setBranchAlias( "TPmass" );  /* Tag-Probe invariant mass. */
   produces<std::vector<float> >( "TPp"    ).setBranchAlias( "TPp"    );  /* Tag-Probe momentum. */
   produces<std::vector<float> >( "TPpt"   ).setBranchAlias( "TPpt"   );  /* Tag-Probe transverse momentum. */
   produces<std::vector<float> >( "TPpx"   ).setBranchAlias( "TPpx"   );  /* Tag-Probe x-momentum. */
   produces<std::vector<float> >( "TPpy"   ).setBranchAlias( "TPpy"   );  /* Tag-Probe y-momentum. */
   produces<std::vector<float> >( "TPpz"   ).setBranchAlias( "TPpz"   );  /* Tag-Probe z-momentum. */
   produces<std::vector<float> >( "TPe"    ).setBranchAlias( "TPe"    );  /* Tag-Probe energy. */
   produces<std::vector<float> >( "TPet"   ).setBranchAlias( "TPet"   );  /* Tag-Probe transverse energy. */

   produces<std::vector<float> >( "TPTagp"   ).setBranchAlias( "TPTagp"   ); /* Tag momentum. */
   produces<std::vector<float> >( "TPTagpx"  ).setBranchAlias( "TPTagpx"  ); /* Tag x-momentum. */
   produces<std::vector<float> >( "TPTagpy"  ).setBranchAlias( "TPTagpy"  ); /* Tag y-momentum. */
   produces<std::vector<float> >( "TPTagpz"  ).setBranchAlias( "TPTagpz"  ); /* Tag z-momentum. */
   produces<std::vector<float> >( "TPTagpt"  ).setBranchAlias( "TPTagpt"  ); /* Tag transverse momentum. */
   produces<std::vector<float> >( "TPTage"   ).setBranchAlias( "TPTage"   ); /* Tag energy. */
   produces<std::vector<float> >( "TPTaget"  ).setBranchAlias( "TPTaget"  ); /* Tag transverse energy. */
   produces<std::vector<float> >( "TPTagq"   ).setBranchAlias( "TPTagq"   ); /* Tag charge. */
   produces<std::vector<float> >( "TPTageta" ).setBranchAlias( "TPTageta" ); /* Tag pseudorapidity. */
   produces<std::vector<float> >( "TPTagphi" ).setBranchAlias( "TPTagphi" ); /* Tag phi. */                 

   produces<std::vector<float> >( "TPProbep"   ).setBranchAlias( "TPProbep"   ); /* Probe momentum. */            
   produces<std::vector<float> >( "TPProbepx"  ).setBranchAlias( "TPProbepx"  ); /* Probe x-momentum. */          
   produces<std::vector<float> >( "TPProbepy"  ).setBranchAlias( "TPProbepy"  ); /* Probe y-momentum. */          
   produces<std::vector<float> >( "TPProbepz"  ).setBranchAlias( "TPProbepz"  ); /* Probe z-momentum. */          
   produces<std::vector<float> >( "TPProbept"  ).setBranchAlias( "TPProbept"  ); /* Probe transverse momentum. */ 
   produces<std::vector<float> >( "TPProbee"   ).setBranchAlias( "TPProbee"   ); /* Probe energy. */              
   produces<std::vector<float> >( "TPProbeet"  ).setBranchAlias( "TPProbeet"  ); /* Probe transverse energy. */   
   produces<std::vector<float> >( "TPProbeq"   ).setBranchAlias( "TPProbeq"   ); /* Probe charge. */              
   produces<std::vector<float> >( "TPProbeeta" ).setBranchAlias( "TPProbeeta" ); /* Probe pseudorapidity. */      
   produces<std::vector<float> >( "TPProbephi" ).setBranchAlias( "TPProbephi" ); /* Probe phi. */                 

   produces< int >( "nCnd" ).setBranchAlias( "nCnd" );                        /* Number of true daughter cands. */
   produces< std::vector<int> >( "Cndtype" ).setBranchAlias( "Cndtype" );     /* Type of eff measurement. */
   produces< std::vector<int> >( "Cndtag" ).setBranchAlias( "Cndtag" );       /* Is this cand matched to a tag? */
   produces< std::vector<int> >( "Cndaprobe" ).setBranchAlias( "Cndaprobe" ); /* Is this cand matched to any probe? */
   produces< std::vector<int> >( "Cndpprobe" ).setBranchAlias( "Cndpprobe" ); /* Is this cand matched to a passing probe? */
   produces< std::vector<int> >( "Cndmoid" ).setBranchAlias( "Cndmoid" );     /* PDGId of candidates mother. */
   produces< std::vector<int> >( "Cndgmid" ).setBranchAlias( "Cndgmid" );     /* PDGId of candidates grandmother. */

   produces<std::vector<float> >( "Cndp" ).setBranchAlias( "Cndp" );     /* Candidate momentum. */     
   produces<std::vector<float> >( "Cndpt" ).setBranchAlias( "Cndpt" );   /* Candidate transverse momentum. */
   produces<std::vector<float> >( "Cndpx" ).setBranchAlias( "Cndpx" );   /* Candidate x-momentum. */
   produces<std::vector<float> >( "Cndpy" ).setBranchAlias( "Cndpy" );   /* Candidate y-momentum. */
   produces<std::vector<float> >( "Cndpz" ).setBranchAlias( "Cndpz" );   /* Candidate z-momentum. */
   produces<std::vector<float> >( "Cnde" ).setBranchAlias( "Cnde" );     /* Candidate energy. */
   produces<std::vector<float> >( "Cndet" ).setBranchAlias( "Cndet" );   /* Candidate transverse energy. */
   produces<std::vector<float> >( "Cndq" ).setBranchAlias( "Cndq" );     /* Candidate charge. */
   produces<std::vector<float> >( "Cndeta" ).setBranchAlias( "Cndeta" ); /* Candidate pesudorapidity. */
   produces<std::vector<float> >( "Cndphi" ).setBranchAlias( "Cndphi" ); /* Candidate phi. */                   

   produces<std::vector<float> >( "Cndrp" ).setBranchAlias( "Cndrp" );    /* Candidate reco momentum. */            
   produces<std::vector<float> >( "Cndrpt" ).setBranchAlias( "Cndrpt" );  /* Candidate reco transverse momentum. */ 
   produces<std::vector<float> >( "Cndrpx" ).setBranchAlias( "Cndrpx" );  /* Candidate reco x-momentum. */          
   produces<std::vector<float> >( "Cndrpy" ).setBranchAlias( "Cndrpy" );  /* Candidate reco y-momentum. */          
   produces<std::vector<float> >( "Cndrpz" ).setBranchAlias( "Cndrpz" );  /* Candidate reco z-momentum. */          
   produces<std::vector<float> >( "Cndre" ).setBranchAlias( "Cndre" );	  /* Candidate reco energy. */              
   produces<std::vector<float> >( "Cndret" ).setBranchAlias( "Cndret" );  /* Candidate reco transverse energy. */   
   produces<std::vector<float> >( "Cndrq" ).setBranchAlias( "Cndrq" );	  /* Candidate reco charge. */              
   produces<std::vector<float> >( "Cndreta" ).setBranchAlias( "Cndreta" ); /* Candidate reco pesudorapidity. */
   produces<std::vector<float> >( "Cndrphi" ).setBranchAlias( "Cndrphi" ); /* Candidate reco phi. */           
}


TagProbeEDMNtuple::~TagProbeEDMNtuple()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
TagProbeEDMNtuple::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   cout << "Here in muon efficiency producer ..." << endl;

   // Set the private event & setup pointers
   m_event = &iEvent;
   m_setup = &iSetup;

   // Fill the run and event number information
   fillRunEventInfo();

   // Fill event level trigger info
   fillTriggerInfo();

   // Fill MC Information
   fillMCInfo();

   // Fill Tag-Probe Info
   fillTagProbeInfo();

   // Fill Efficiency Info for true muons
   fillTrueEffInfo();

   return; 
}

// ------------ method called once each job just before starting event loop  ------------
void 
TagProbeEDMNtuple::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TagProbeEDMNtuple::endJob() {
}

// ****************************************************************** //
// ********************* Tree Fill Functions! *********************** //
// ****************************************************************** //


// ********************* Fill Run & Event Info *********************** //
void
TagProbeEDMNtuple::fillRunEventInfo()
{
   auto_ptr< int > run_( new int );
   auto_ptr< int > event_( new int );

   *run_ = m_event->id().run();
   *event_ = m_event->id().event();

   m_event->put( run_, "run" );
   m_event->put( event_, "event" );
}
// ****************************************************************** //

// ********************* Fill Trigger Info *********************** //
void
TagProbeEDMNtuple::fillTriggerInfo()
{
   auto_ptr<int> nl1_( new int );
   auto_ptr<int> nhlt_( new int );
   
   // Trigger Info
   Handle<TriggerEventWithRefs> trgEvent;
   try{ m_event->getByLabel(triggerEventTag_,trgEvent); } 
   catch (...) 
   { 
      LogWarning("TagAndProbe") << "Could not extract trigger event summary "
				<< "with tag " << triggerEventTag_;
   }
   
   *nl1_ = 0;
   *nhlt_ = 0;
   if( trgEvent.isValid() )
   {
      vector< L1MuonParticleRef > muonL1CandRefs;
      size_type l1index = trgEvent->filterIndex(hltL1Tag_.label());
      if( l1index < trgEvent->size() )
      {
	 trgEvent->getObjects( l1index, trigger::TriggerL1Mu, muonL1CandRefs );
	 *nl1_ = muonL1CandRefs.size(); 
      }

      vector< RecoChargedCandidateRef > muonCandRefs;
      size_type index = trgEvent->filterIndex(hltTag_.label());
      if( index < trgEvent->size() )
      {
	 trgEvent->getObjects( index, trigger::TriggerMuon, muonCandRefs );
	 *nhlt_ = muonCandRefs.size(); 
      }
   }

   m_event->put( nl1_, "nl1" );
   m_event->put( nhlt_, "nhlt" );
}
// ****************************************************************** //

// ********************* Fill MC Info *********************** //
void TagProbeEDMNtuple::fillMCInfo()
{
   // Extract the MC information from the frame, if we are running on MC
   if( isMC_ )
   {      
      vector< Handle< HepMCProduct > > EvtHandles ;
      m_event->getManyByType( EvtHandles ) ;
   
      // Loop over all MC products
      for ( unsigned int i=0; (i<EvtHandles.size() && mcParticles_.size()>0); i++ )
      {
	 if( i>0 ) continue;

	 if ( EvtHandles[i].isValid() )
	 {
	    // Get the event
	    const HepMC::GenEvent* Evt = EvtHandles[i]->GetEvent() ;
   
	    // Loop over particles and extract any that the user has asked to
	    // be stored
	    for( int j=0; j<(int)mcParticles_.size(); ++j )
	    {
	       stringstream nstream;
	       nstream << "MC" << mcParticles_[j];
	       string prefix = nstream.str();

	       auto_ptr< int > nmc_( new int );

	       auto_ptr< vector<int> > mc_ppid_( new vector<int> );
	       auto_ptr< vector<int> > mc_pbc_( new vector<int> );
	       auto_ptr< vector<int> > mc_pid_( new vector<int> );
	       auto_ptr< vector<int> > mc_bc_( new vector<int> );

	       auto_ptr< vector<float> > mc_mass_( new vector<float> );
	       auto_ptr< vector<float> > mc_p_( new vector<float> );
	       auto_ptr< vector<float> > mc_pt_( new vector<float> );
	       auto_ptr< vector<float> > mc_px_( new vector<float> );
	       auto_ptr< vector<float> > mc_py_( new vector<float> );
	       auto_ptr< vector<float> > mc_pz_( new vector<float> );
	       auto_ptr< vector<float> > mc_e_( new vector<float> );
	       auto_ptr< vector<float> > mc_eta_( new vector<float> );
	       auto_ptr< vector<float> > mc_phi_( new vector<float> );

	       int nmc = 0;

	       HepMC::GenEvent::particle_const_iterator Part = Evt->particles_begin();
	       HepMC::GenEvent::particle_const_iterator PartEnd = Evt->particles_end();
	       for(; Part != PartEnd; Part++ )
	       {
		  int pdg = (*Part)->pdg_id();
	       
		  if( abs(pdg) == mcParticles_[j] )
		  {
		     HepMC::FourVector p4 = (*Part)->momentum();

		     int    pid = pdg;
		     int    barcode = (*Part)->barcode();
		     double p   = p4.mag();
		     double px = p4.px();
		     double py = p4.py();
		     double pz = p4.pz();
		     double pt = p4.perp();
		     double e  = p4.e();
		     double phi = p4.phi() ;
		     double eta = -log(tan(p4.theta()/2.));
		     double mass = e*e - p*p;
		     if( mass > 0 ) mass = sqrt(mass);
		     else           mass = -sqrt(fabs(mass));

		     // Get the parent barcode (in general there
		     // can be more than one parent, but for now
		     // we will just take the first).
		     int parent_pi = 0;
		     int parent_bc = 0;
		     HepMC::GenVertex *pvtx = (*Part)->production_vertex();
		     
		     // Loop over the particles in this vertex
		     if( pvtx != NULL )
		     {
			if( (*pvtx).particles_in_size() > 0 )
			{
			   HepMC::GenVertex::particles_in_const_iterator pIt = 
			      (*pvtx).particles_in_const_begin();
			   parent_pi = (*pIt)->pdg_id(); 
			   parent_bc = (*pIt)->barcode();
			}
		     }

		     int num_lep = 0;
		     if( abs(pdg) == 23 )
		     {
			HepMC::GenVertex *evtx = (*Part)->end_vertex();
			
			// Loop over the particles in this vertex
			if( evtx != NULL )
			{
			   if( (*evtx).particles_out_size() > 0 )
			   {
			      HepMC::GenVertex::particles_out_const_iterator pIt = 
				 (*evtx).particles_out_const_begin();
			      for( ; pIt != (*evtx).particles_out_const_end(); pIt++ )
			      {
				 if( abs((*pIt)->pdg_id()) >= 11 && 
				 abs((*pIt)->pdg_id()) <= 14 ) 
				 {
				    ++num_lep;
				 }
			      }
			   }
			}
		     }
		     if( abs(mcParticles_[j]) == 23 && num_lep == 0 ) continue;

		     // Check for the parents if required
		     if( mcParents_[j] != 0 && abs(parent_pi) != mcParents_[j] ) continue;

		     // set the tree values
		     mc_pbc_->push_back( parent_bc );
		     mc_ppid_->push_back( parent_pi );
		     mc_bc_->push_back( barcode );
		     mc_pid_->push_back( pid );
		     mc_p_->push_back( p );
		     mc_mass_->push_back( mass );
		     mc_px_->push_back( px );
		     mc_py_->push_back( py );
		     mc_pz_->push_back( pz );
		     mc_pt_->push_back( pt );
		     mc_e_->push_back( e );
		     mc_phi_->push_back( phi );
		     mc_eta_->push_back( eta );

		     nmc++;
		  }
	       }

	       // Insert these particles into the event!!
	       string name = "n"+prefix;
	       nmc_.reset( new int(nmc) );
	       m_event->put( nmc_, name.c_str());

	       name = prefix+"ppid";
	       m_event->put( mc_ppid_, name.c_str());
	       name = prefix+"pbc";
	       m_event->put( mc_pbc_, name.c_str());
	       name = prefix+"pid";
	       m_event->put( mc_pid_, name.c_str());
	       name = prefix+"bc";
	       m_event->put( mc_bc_, name.c_str());

	       name = prefix+"mass";
	       m_event->put( mc_mass_, name.c_str());
	       name = prefix+"p";
	       m_event->put( mc_p_, name.c_str());
	       name = prefix+"pt";
	       m_event->put( mc_pt_, name.c_str());
	       name = prefix+"px";
	       m_event->put( mc_px_, name.c_str());
	       name = prefix+"py";
	       m_event->put( mc_py_, name.c_str());
	       name = prefix+"pz";
	       m_event->put( mc_pz_, name.c_str());
	       name = prefix+"e";
	       m_event->put( mc_e_, name.c_str());
	       name = prefix+"eta";
	       m_event->put( mc_eta_, name.c_str());
	       name = prefix+"phi";
	       m_event->put( mc_phi_, name.c_str());
	    }
	 }
      }
   }
}
// ********************************************************** //


// ********************* Fill Tag-Probe Info *********************** //
void
TagProbeEDMNtuple::fillTagProbeInfo()
{
   auto_ptr<int> nrtp_( new int );
   auto_ptr< vector<int> > tp_type_( new vector<int> );
   auto_ptr< vector<int> > tp_true_( new vector<int> );
   auto_ptr< vector<int> > tp_ppass_( new vector<int> );
   auto_ptr< vector<int> > tp_l1_( new vector<int> );  
   auto_ptr< vector<int> > tp_hlt_( new vector<int> ); 

   auto_ptr< vector<float> > tp_mass_( new vector<float> );
   auto_ptr< vector<float> > tp_p_( new vector<float> );  
   auto_ptr< vector<float> > tp_pt_( new vector<float> ); 
   auto_ptr< vector<float> > tp_px_( new vector<float> ); 
   auto_ptr< vector<float> > tp_py_( new vector<float> ); 
   auto_ptr< vector<float> > tp_pz_( new vector<float> ); 
   auto_ptr< vector<float> > tp_e_( new vector<float> );  
   auto_ptr< vector<float> > tp_et_( new vector<float> ); 

   auto_ptr< vector<float> > tp_tag_p_( new vector<float> );   
   auto_ptr< vector<float> > tp_tag_px_( new vector<float> );  
   auto_ptr< vector<float> > tp_tag_py_( new vector<float> );  
   auto_ptr< vector<float> > tp_tag_pz_( new vector<float> );  
   auto_ptr< vector<float> > tp_tag_pt_( new vector<float> );  
   auto_ptr< vector<float> > tp_tag_e_( new vector<float> );   
   auto_ptr< vector<float> > tp_tag_et_( new vector<float> );  
   auto_ptr< vector<float> > tp_tag_q_( new vector<float> );   
   auto_ptr< vector<float> > tp_tag_eta_( new vector<float> ); 
   auto_ptr< vector<float> > tp_tag_phi_( new vector<float> ); 

   auto_ptr< vector<float> > tp_probe_p_( new vector<float> );   
   auto_ptr< vector<float> > tp_probe_px_( new vector<float> );  
   auto_ptr< vector<float> > tp_probe_py_( new vector<float> );  
   auto_ptr< vector<float> > tp_probe_pz_( new vector<float> );  
   auto_ptr< vector<float> > tp_probe_pt_( new vector<float> );  
   auto_ptr< vector<float> > tp_probe_e_( new vector<float> );   
   auto_ptr< vector<float> > tp_probe_et_( new vector<float> );  
   auto_ptr< vector<float> > tp_probe_q_( new vector<float> );   
   auto_ptr< vector<float> > tp_probe_eta_( new vector<float> ); 
   auto_ptr< vector<float> > tp_probe_phi_( new vector<float> ); 

   // Trigger Info
   Handle<TriggerEventWithRefs> trgEvent;
   try{ m_event->getByLabel(triggerEventTag_,trgEvent); } 
   catch (...) 
   { 
      LogWarning("Z") << "Could not extract trigger event summary "
		      << "with tag " << triggerEventTag_;
   }

   // Fill some information about the tag & probe collections
   int nrtp = 0;
   for( int itype=0; itype<(int)tagProbeMapTags_.size(); ++itype )
   {
      // Tag-Probes
      Handle<CandViewCandViewAssociation> tagprobes;
      try{ m_event->getByLabel(tagProbeMapTags_[itype],tagprobes); }
      catch(...)
      { 
	 LogWarning("Z") << "Could not extract tag-probe map with input tag " 
			 << tagProbeMapTags_[itype];
      }

      // Tag MC Truth Match Maps
      Handle<GenParticleMatch> tagmatch;
      try{ m_event->getByLabel(tagTruthMatchMapTags_[itype],tagmatch); }
      catch(...)
      { 
	 LogWarning("Z") << "Could not extract muon tag match map "
			 << "with input tag " << tagTruthMatchMapTags_[itype];
      }

      // Truth match for probe
      Handle<GenParticleMatch> allprobematch;
      try{ m_event->getByLabel(allProbeTruthMatchMapTags_[itype],allprobematch); }
      catch(...)
      { 
	 LogWarning("Z") << "Could not extract muon allprobe match map "
			 << "with input tag " << allProbeTruthMatchMapTags_[itype];
      }

      // Tag Candidates
      Handle<CandidateView> tagmuons;
      try{ m_event->getByLabel(tagCandTags_[itype],tagmuons); }
      catch(...)
      { 
	 LogWarning("Z") << "Could not extract tag muon cands with input tag " 
			 << tagCandTags_[itype];
      }

      // Passing Probe Candidates
      Handle<CandidateView> passprobemuons;
      try{ m_event->getByLabel(passProbeCandTags_[itype],passprobemuons); }
      catch(...)
      { 
	 LogWarning("Z") << "Could not extract tag muon cands with input tag " 
			 << passProbeCandTags_[itype];
      }

      if( tagprobes.isValid() )
      {
	 CandViewCandViewAssociation::const_iterator tpItr = tagprobes->begin();
	 for( ; tpItr != tagprobes->end(); ++tpItr )
	 {
	    const CandidateBaseRef &tag = tpItr->key;
	    vector< pair<CandidateBaseRef,double> > vprobes = (*tagprobes)[tag];

	    // If there are two probes with the tag continue
	    if( vprobes.size() > 1 ) continue;

	    math::XYZTLorentzVector tpP4 = tag->p4() + (vprobes[0].first)->p4();

	    // Is this Tag-Probe pair from a true Z?
	    // See if both the daughters are matched.
	    int tptrue = 0;
 	    bool tagFromZ   = CandFromZ((*tagmatch)[tag]);
 	    bool probeFromZ = CandFromZ((*allprobematch)[vprobes[0].first]);

	    // If both tag and probe are from Z .. set to true
	    if( tagFromZ && probeFromZ ) tptrue = 1;
	    cout << "Is true Z? " << tptrue << endl;

	    // Is this probe in the set that pass the efficiency criteria?
	    int ppass = ProbePassProbeOverlap(vprobes[0].first,passprobemuons);

	    // Did this tag cause a L1 and/or HLT trigger?
	    bool l1Trigger = false;
	    bool hltTrigger = false;
	    if( trgEvent.isValid() )
	    {
	       vector< L1MuonParticleRef > muonL1CandRefs;
	       size_type l1index = trgEvent->filterIndex(hltL1Tag_.label());
	       if( l1index < trgEvent->size() )
	       {
		  trgEvent->getObjects( l1index, trigger::TriggerL1Mu, muonL1CandRefs );
		  int npart = muonL1CandRefs.size();
		  for(int ipart = 0; ipart != npart; ++ipart)
		  { 
		     const L1MuonParticle* l1mu = (muonL1CandRefs[ipart]).get();
		     l1Trigger = MatchObjects( (const Candidate*)l1mu, tag, false );
		     if( l1Trigger ) break;
		  }
	       }

	       vector< RecoChargedCandidateRef > muonCandRefs;
	       size_type index = trgEvent->filterIndex(hltTag_.label());
	       if( index < trgEvent->size() )
	       {
		  trgEvent->getObjects( index, trigger::TriggerMuon, muonCandRefs );
		  int npart = muonCandRefs.size();
		  for(int ipart = 0; ipart != npart; ++ipart)
		  { 
		     const RecoChargedCandidate* muon = (muonCandRefs[ipart]).get();
		     hltTrigger = MatchObjects( (const Candidate*)muon, tag, false );
		     if( hltTrigger ) break;
		  }
	       }
	    }

	    double mass = tpP4.M();
	    double px   = tpP4.Px();
	    double py   = tpP4.Py();
	    double pz   = tpP4.Pz();
	    double pt   = tpP4.Pt();
	    double p    = tpP4.P();
	    double e    = tpP4.E();
	    double et   = tpP4.Et();

	    tp_type_->push_back(   itype );
	    tp_true_->push_back(   tptrue );
	    tp_ppass_->push_back(  ppass );
	    tp_l1_->push_back(  l1Trigger );
	    tp_hlt_->push_back(  hltTrigger );
	    tp_p_->push_back(   p );
	    tp_px_->push_back(  px );
	    tp_py_->push_back(  py );
	    tp_pz_->push_back(  pz );
	    tp_pt_->push_back(  pt );
	    tp_e_->push_back(   e );
	    tp_et_->push_back(  et );
	    tp_mass_->push_back(  mass );

	    double dp   = tag->p();
	    double dpx  = tag->px();
	    double dpy  = tag->py();
	    double dpz  = tag->pz();
	    double dpt  = tag->pt();
	    double de   = tag->energy();
	    double det  = tag->et();
	    double dq   = tag->charge();
	    double deta = tag->eta();
	    double dphi = tag->phi();

	    tp_tag_p_->push_back(    dp );
	    tp_tag_px_->push_back(   dpx );
	    tp_tag_py_->push_back(   dpy );
	    tp_tag_pz_->push_back(   dpz );
	    tp_tag_pt_->push_back(   dpt );
	    tp_tag_e_->push_back(    de );
	    tp_tag_et_->push_back(   det );
	    tp_tag_q_->push_back(    dq );
	    tp_tag_eta_->push_back(  deta );
	    tp_tag_phi_->push_back(  dphi );

	    dp   = (vprobes[0].first)->p();
	    dpx  = (vprobes[0].first)->px();
	    dpy  = (vprobes[0].first)->py();
	    dpz  = (vprobes[0].first)->pz();
	    dpt  = (vprobes[0].first)->pt();
	    de   = (vprobes[0].first)->energy();
	    det  = (vprobes[0].first)->et();
	    dq   = (vprobes[0].first)->charge();
	    deta = (vprobes[0].first)->eta();
	    dphi = (vprobes[0].first)->phi();

	    tp_probe_p_->push_back(    dp );
	    tp_probe_px_->push_back(   dpx );
	    tp_probe_py_->push_back(   dpy );
	    tp_probe_pz_->push_back(   dpz );
	    tp_probe_pt_->push_back(   dpt );
	    tp_probe_e_->push_back(    de );
	    tp_probe_et_->push_back(   det );
	    tp_probe_q_->push_back(    dq );
	    tp_probe_eta_->push_back(  deta );
	    tp_probe_phi_->push_back(  dphi );

	    ++nrtp;
	 }
      }
   }
   nrtp_.reset( new int(nrtp) );

   m_event->put( nrtp_, "nrTP" );

   m_event->put( tp_type_, "TPtype" );
   m_event->put( tp_true_, "TPtrue" );
   m_event->put( tp_ppass_, "TPppass" );
   m_event->put( tp_l1_, "TPl1" );  
   m_event->put( tp_hlt_, "TPhlt" ); 

   m_event->put( tp_mass_, "TPmass" );
   m_event->put( tp_p_, "TPp" );  
   m_event->put( tp_pt_, "TPpt" ); 
   m_event->put( tp_px_, "TPpx" ); 
   m_event->put( tp_py_, "TPpy" ); 
   m_event->put( tp_pz_, "TPpz" ); 
   m_event->put( tp_e_, "TPe" );  
   m_event->put( tp_et_, "TPet" ); 

   m_event->put( tp_tag_p_, "TPTagp" );   
   m_event->put( tp_tag_px_, "TPTagpx" );  
   m_event->put( tp_tag_py_, "TPTagpy" );  
   m_event->put( tp_tag_pz_, "TPTagpz" );  
   m_event->put( tp_tag_pt_, "TPTagpt" );  
   m_event->put( tp_tag_e_, "TPTage" );   
   m_event->put( tp_tag_et_, "TPTaget" );  
   m_event->put( tp_tag_q_, "TPTagq" );   
   m_event->put( tp_tag_eta_, "TPTageta" ); 
   m_event->put( tp_tag_phi_, "TPTagphi" ); 

   m_event->put( tp_probe_p_, "TPProbep" );   
   m_event->put( tp_probe_px_, "TPProbepx" );  
   m_event->put( tp_probe_py_, "TPProbepy" );  
   m_event->put( tp_probe_pz_, "TPProbepz" );  
   m_event->put( tp_probe_pt_, "TPProbept" );  
   m_event->put( tp_probe_e_, "TPProbee" );   
   m_event->put( tp_probe_et_, "TPProbeet" );  
   m_event->put( tp_probe_q_, "TPProbeq" );   
   m_event->put( tp_probe_eta_, "TPProbeeta" ); 
   m_event->put( tp_probe_phi_, "TPProbephi" ); 

}
// ******************************************************************* //


// ********************* Fill True Efficiency Info *********************** //
void
TagProbeEDMNtuple::fillTrueEffInfo()
{
   auto_ptr<int> ncnd_( new int );
   auto_ptr< vector<int> > cnd_type_( new vector<int> );   
   auto_ptr< vector<int> > cnd_tag_( new vector<int> );    
   auto_ptr< vector<int> > cnd_aprobe_( new vector<int> ); 
   auto_ptr< vector<int> > cnd_pprobe_( new vector<int> ); 
   auto_ptr< vector<int> > cnd_moid_( new vector<int> );   
   auto_ptr< vector<int> > cnd_gmid_( new vector<int> );   

   auto_ptr< vector<float> > cnd_p_( new vector<float> );   
   auto_ptr< vector<float> > cnd_pt_( new vector<float> );  
   auto_ptr< vector<float> > cnd_px_( new vector<float> );  
   auto_ptr< vector<float> > cnd_py_( new vector<float> );  
   auto_ptr< vector<float> > cnd_pz_( new vector<float> );  
   auto_ptr< vector<float> > cnd_e_( new vector<float> );   
   auto_ptr< vector<float> > cnd_et_( new vector<float> );  
   auto_ptr< vector<float> > cnd_q_( new vector<float> );   
   auto_ptr< vector<float> > cnd_eta_( new vector<float> ); 
   auto_ptr< vector<float> > cnd_phi_( new vector<float> ); 

   auto_ptr< vector<float> > cnd_rp_( new vector<float> );   
   auto_ptr< vector<float> > cnd_rpt_( new vector<float> );  
   auto_ptr< vector<float> > cnd_rpx_( new vector<float> );  
   auto_ptr< vector<float> > cnd_rpy_( new vector<float> );  
   auto_ptr< vector<float> > cnd_rpz_( new vector<float> );  
   auto_ptr< vector<float> > cnd_re_( new vector<float> );   
   auto_ptr< vector<float> > cnd_ret_( new vector<float> );  
   auto_ptr< vector<float> > cnd_rq_( new vector<float> );   
   auto_ptr< vector<float> > cnd_reta_( new vector<float> ); 
   auto_ptr< vector<float> > cnd_rphi_( new vector<float> ); 

   // Should change this to get the eff info for all types of tag-probe!!
   Handle<GenParticleCollection> genparticles;
   try{ m_event->getByLabel(genParticlesTag_,genparticles); }
   catch(...)
   { 
      LogWarning("Z") << "Could not extract gen particles with input tag " 
			<< genParticlesTag_;
   }

   int ncnd = 0;
   // Fill some information about the muon efficiency
   if( genparticles.isValid() )
   {
      for( int itype=0; itype<(int)tagProbeMapTags_.size(); ++itype )
      {
	 Handle<GenParticleMatch> tagmatch;
	 try{ m_event->getByLabel(tagTruthMatchMapTags_[itype],tagmatch); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract muon match map "
			    << "with input tag " << tagTruthMatchMapTags_[itype];
	 }

	 Handle<CandidateView> tags;
	 try{ m_event->getByLabel(tagCandTags_[itype],tags); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract tag candidates "
			    << "with input tag " << tagCandTags_[itype];
	 }

	 Handle<CandidateView> aprobes;
	 try{ m_event->getByLabel(allProbeCandTags_[itype],aprobes); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract probe candidates "
			    << "with input tag " << allProbeCandTags_[itype];
	 }

	 Handle<CandidateView> pprobes;
	 try{ m_event->getByLabel(passProbeCandTags_[itype],pprobes); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract passing probe candidates "
			    << "with input tag " << passProbeCandTags_[itype];
	 }

	 Handle<GenParticleMatch> apmatch;
	 try{ m_event->getByLabel(allProbeTruthMatchMapTags_[itype],apmatch); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract all probe match map "
			    << "with input tag " << allProbeTruthMatchMapTags_[itype];
	 }

	 Handle<GenParticleMatch> ppmatch;
	 try{ m_event->getByLabel(passProbeTruthMatchMapTags_[itype],ppmatch); }
	 catch(...)
	 { 
	    LogWarning("Z") << "Could not extract pass probe match map "
			    << "with input tag " << passProbeTruthMatchMapTags_[itype];
	 }
   
	 for( unsigned int i=0; i<genparticles->size(); i++ )
	 {
	    int pdg_id = (*genparticles)[i].pdgId();

	    // If this is not a muon keep going!
	    if( abs( pdg_id ) != candPDGId_ ) continue;

	    int moid  = -1;
	    int gmoid = -1;
	    const Candidate *mcand = (*genparticles)[i].mother();
	    if( mcand != 0 )
	    {
	       moid = mcand->pdgId();
	       if( mcand->pdgId() == pdg_id )
	       {
		  if( mcand->mother() != 0 )
		  {
		     const Candidate *gcand = mcand->mother();
		     gmoid = gcand->pdgId();
		  }
	       }
	    }

	    int ftag = 0;
	    int fapb = 0;
	    int fppb = 0;

	    double p   = (*genparticles)[i].p();
	    double px = (*genparticles)[i].px();
	    double py = (*genparticles)[i].py();
	    double pz = (*genparticles)[i].pz();
	    double pt = (*genparticles)[i].pt();
	    double e  = (*genparticles)[i].energy();
	    double et  = (*genparticles)[i].et();
	    double q  = (*genparticles)[i].charge();
	    double phi = (*genparticles)[i].phi();
	    double eta = (*genparticles)[i].eta();

	    double rp   = 0;
	    double rpx  = 0;
	    double rpy  = 0;
	    double rpz  = 0; 
	    double rpt  = 0; 
	    double re   = 0;
	    double ret  = 0;
	    double rq   = 0;
	    double rphi = 0;
	    double reta = 0;

	    GenParticleRef mcRef( genparticles, (size_t)i );
	    if( tagmatch.isValid() )
	    {
	       // Loop over the tag muons
	       CandidateView::const_iterator f = tags->begin();
	       for( ; f != tags->end(); ++f )
	       {
		  unsigned int index = f - tags->begin();
		  CandidateBaseRef tagRef = tags->refAt(index);
		  GenParticleRef mcMatchRef = (*tagmatch)[tagRef];

		  if( &(*mcRef)==&(*mcMatchRef) ) 
		  {
		     ftag = 1;
		     const Candidate *cnd = tagRef.get();
		     rp   = cnd->p();
		     rpx  = cnd->px();
		     rpy  = cnd->py();
		     rpz  = cnd->pz(); 
		     rpt  = cnd->pt(); 
		     re   = cnd->energy();
		     ret  = cnd->et();
		     rq   = cnd->charge();
		     rphi = cnd->phi();
		     reta = cnd->eta();
		  }
	       }
	    }
	    if( apmatch.isValid() )
	    {
	       // Loop over the tag muons
	       CandidateView::const_iterator f = aprobes->begin();
	       for( ; f != aprobes->end(); ++f )
	       {
		  unsigned int index = f - aprobes->begin();
		  CandidateBaseRef tagRef = aprobes->refAt(index);
		  GenParticleRef mcMatchRef = (*apmatch)[tagRef];

		  if( &(*mcRef)==&(*mcMatchRef) ) 
		  {
		     fapb = 1;
		     if( ftag == 0 )
		     {
			const Candidate *cnd = tagRef.get();
			rp   = cnd->p();
			rpx  = cnd->px();
			rpy  = cnd->py();
			rpz  = cnd->pz(); 
			rpt  = cnd->pt(); 
			re   = cnd->energy();
			ret  = cnd->et();
			rq   = cnd->charge();
			rphi = cnd->phi();
			reta = cnd->eta();
		     }
		  }
	       }
	    }
	    if( ppmatch.isValid() )
	    {
	       // Loop over the tag muons
	       CandidateView::const_iterator f = pprobes->begin();
	       for( ; f != pprobes->end(); ++f )
	       {
		  unsigned int index = f - pprobes->begin();
		  CandidateBaseRef tagRef = pprobes->refAt(index);
		  GenParticleRef mcMatchRef = (*ppmatch)[tagRef];

		  if( &(*mcRef)==&(*mcMatchRef) ) 
		  {
		     fppb = 1;
		     if( ftag == 0 && fapb == 0 )
		     {
			const Candidate *cnd = tagRef.get();
			rp   = cnd->p();
			rpx  = cnd->px();
			rpy  = cnd->py();
			rpz  = cnd->pz(); 
			rpt  = cnd->pt(); 
			re   = cnd->energy();
			ret  = cnd->et();
			rq   = cnd->charge();
			rphi = cnd->phi();
			reta = cnd->eta();
		     }
		  }
	       }
	    }

	    cnd_type_->push_back( itype );

	    cnd_tag_->push_back(    ftag );
	    cnd_aprobe_->push_back( fapb );
	    cnd_pprobe_->push_back( fppb );
	    cnd_moid_->push_back(   moid );
	    cnd_gmid_->push_back(   gmoid );

	    cnd_p_->push_back(   p );
	    cnd_px_->push_back(  px );
	    cnd_py_->push_back(  py );
	    cnd_pz_->push_back(  pz );
	    cnd_pt_->push_back(  pt );
	    cnd_e_->push_back(   e );
	    cnd_et_->push_back(  et );
	    cnd_q_->push_back(   q );
	    cnd_phi_->push_back(  phi );
	    cnd_eta_->push_back(  eta );

	    cnd_rp_->push_back(   rp );
	    cnd_rpx_->push_back(  rpx );
	    cnd_rpy_->push_back(  rpy );
	    cnd_rpz_->push_back(  rpz );
	    cnd_rpt_->push_back(  rpt );
	    cnd_re_->push_back(   re );
	    cnd_ret_->push_back(  ret );
	    cnd_rq_->push_back(   rq );
	    cnd_rphi_->push_back(  rphi );
	    cnd_reta_->push_back(  reta );

	    ncnd++;
	 }
      }
   }
   ncnd_.reset( new int(ncnd) );

   m_event->put( ncnd_, "nCnd" );

   m_event->put( cnd_type_, "Cndtype" );
   m_event->put( cnd_tag_, "Cndtag" );
   m_event->put( cnd_aprobe_, "Cndaprobe" );
   m_event->put( cnd_pprobe_, "Cndpprobe" );  
   m_event->put( cnd_moid_, "Cndmoid" ); 
   m_event->put( cnd_gmid_, "Cndgmid" ); 

   m_event->put( cnd_p_, "Cndp" );  
   m_event->put( cnd_pt_, "Cndpt" ); 
   m_event->put( cnd_px_, "Cndpx" ); 
   m_event->put( cnd_py_, "Cndpy" ); 
   m_event->put( cnd_pz_, "Cndpz" ); 
   m_event->put( cnd_e_, "Cnde" );  
   m_event->put( cnd_et_, "Cndet" ); 
   m_event->put( cnd_q_, "Cndq" );  
   m_event->put( cnd_eta_, "Cndeta" ); 
   m_event->put( cnd_phi_, "Cndphi" ); 

   m_event->put( cnd_rp_, "Cndrp" );  
   m_event->put( cnd_rpt_, "Cndrpt" ); 
   m_event->put( cnd_rpx_, "Cndrpx" ); 
   m_event->put( cnd_rpy_, "Cndrpy" ); 
   m_event->put( cnd_rpz_, "Cndrpz" ); 
   m_event->put( cnd_re_, "Cndre" );  
   m_event->put( cnd_ret_, "Cndret" ); 
   m_event->put( cnd_rq_, "Cndrq" );  
   m_event->put( cnd_reta_, "Cndreta" ); 
   m_event->put( cnd_rphi_, "Cndrphi" ); 
}
// *********************************************************************** //

// ***************** Are Candidates from a Z? ******************** //
bool TagProbeEDMNtuple::CandFromZ( GenParticleRef mcRef )
{
   bool isFromZ = false;

   if( mcRef.isNonnull() )
   {
      //cout << "Id " << mcMatchRef->pdgId() << endl;
      int moid = -99;
      int gmoid = -99;
      const Candidate *mcand = mcRef->mother();
      if( mcand != 0 )
      {
	 moid = mcand->pdgId();
	 if( mcand->pdgId() == mcRef->pdgId() )
	 {
	    if( mcand->mother() != 0 )
	    {
	       const Candidate *gcand = mcand->mother();
	       gmoid = gcand->pdgId();
	    }
	 }
      }
      if( moid == 23 || gmoid == 23 ) isFromZ = true;
   }

   return isFromZ;
}
// *************************************************************** //

// ***************** Does this probe pass? ******************** //
int TagProbeEDMNtuple::ProbePassProbeOverlap( const CandidateBaseRef& probe, 
					      Handle<CandidateView>& passprobes )
{
   int ppass = 0;
   if( passprobes.isValid() )
   {
      for( int ipp=0; ipp<(int)passprobes->size(); ++ipp )
      {
	 bool isOverlap = MatchObjects(&((*passprobes)[ipp]),probe);
	 if( isOverlap ) ppass = 1;
      } 
   }

   return ppass;
}
// ************************************************************ //


// ***************** Trigger object matching ******************** //
bool TagProbeEDMNtuple::MatchObjects( const Candidate *hltObj, 
				      const reco::CandidateBaseRef& tagObj,
				      bool exact )
{
   double tEta = tagObj->eta();
   double tPhi = tagObj->phi();
   double tPt  = tagObj->pt();
   double hEta = hltObj->eta();
   double hPhi = hltObj->phi();
   double hPt  = hltObj->pt();

   double dRval = deltaR(tEta, tPhi, hEta, hPhi);
   double dPtRel = 999.0;
   if( tPt > 0.0 ) dPtRel = fabs( hPt - tPt )/tPt;

   // If we are comparing two objects for which the candidates should
   // be exactly the same, cut hard. Otherwise take cuts from user.
   if( exact ) return ( dRval < 1e-3 && dPtRel < 1e-3 );
   else        return ( dRval < delRMatchingCut_ && dPtRel < delPtRelMatchingCut_ );
}
// ************************************************************** //

//define this as a plug-in
DEFINE_FWK_MODULE(TagProbeEDMNtuple);