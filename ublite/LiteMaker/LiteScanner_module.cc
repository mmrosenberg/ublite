////////////////////////////////////////////////////////////////////////
// Class:       LiteScanner
// Module Type: analyzer
// File:        LiteScanner_module.cc
//
// Generated at Wed Oct 15 18:41:39 2014 by Kazuhiro Terao using artmod
// from cetpkgsupport v1_07_01.
////////////////////////////////////////////////////////////////////////

// ART includes
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// LArLite include
#include "DataFormat/storage_manager.h"
#include "DataFormat/potsummary.h"

// LArSoft includes
#include "ubcore/Geometry/UBOpChannelTypes.h"
#include "ubcore/Geometry/UBOpReadoutMap.h"
#include "ubobj/Trigger/ubdaqSoftwareTriggerData.h"
#include "ubobj/MuCS/MuCSData.h"
#include "ubobj/MuCS/MuCSRecoData.h"
#include "larsim/EventWeight/Base/MCEventWeight.h"

#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/CoreUtils/ServiceUtil.h" // lar::providerFrom<>()
#include "larcorealg/Geometry/geo_vectors_utils_TVector.h" // toTVector3()
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/OpDetWaveform.h"
#include "lardataobj/Simulation/SimPhotons.h"
#include "lardataobj/RawData/TriggerData.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/EndPoint2D.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/PCAxis.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/FlashMatch.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/Simulation/AuxDetSimChannel.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "lardataobj/OpticalDetectorData/FIFOChannel.h"
#include "lardataobj/OpticalDetectorData/OpticalTypes.h"
#include "lardataobj/MCBase/MCShower.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardata/Utilities/GeometryUtilities.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
//// New Data Product
#include "lardataobj/AnalysisBase/BackTrackerMatchingData.h"

#include "DataFormat/simphotons.h"
#include "DataFormat/chstatus.h"
#include "DataFormat/DataFormatException.h"
#include "ScannerAlgo.h"
#include "LLMetaMaker.h"
//#include "ScannerAlgo.template.h"

// std 
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// ROOT
#include <TTimeStamp.h>
#include <TString.h>

class LiteScanner;

class LiteScanner : public art::EDAnalyzer {
public:
  explicit LiteScanner(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  LiteScanner(LiteScanner const &) = delete;
  LiteScanner(LiteScanner &&) = delete;
  LiteScanner & operator = (LiteScanner const &) = delete;
  LiteScanner & operator = (LiteScanner &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;
  void beginJob() override;
  void endJob() override;
  void beginSubRun(const art::SubRun& sr) override;

private:

  /// Data scanning algorithm instance
  ::larlite::ScannerAlgo fAlg;

  /// Templated data scanner function
  template<class T> void ScanData(const art::Event& evt, const size_t name_index);

  /// Templated data scanner function
  template<class T> void ScanSimpleData(const art::Event& evt, const size_t name_index);

  /// Special function for SimPhotons
  void ScanSimPhotons(const art::Event& evt, const size_t name_index);

  /// Templated association scanner function
  template<class T> void SaveAssociationSource(const art::Event& evt);
  
  /// Templated association scanner
  template<class T> void ScanAssociation(const art::Event& evt, const size_t name_index);
 
  void FillChStatus(const art::Event& e, const std::string& name);

  /// storage_manager from larlite
  ::larlite::storage_manager _mgr;

  /// Boolean to switch on/off association storage
  bool fStoreAss;
  /// POTSummary producer label
  std::vector<std::string> fPOTSummaryLabel_v;
  /// Association producer label to be scanned (for those associations not made by product producers)
  std::vector<std::string> fAssProducer_v;
  /// Association producer label to be stored
  std::set<std::string> fAssLabelToSave_s;
  /// Boolean flag to automatically scan associations made by producers
  bool fScanAssByProducers;
  /// Boolean to enable unique file name
  std::string fOutFileName;
  /// Stream name
  std::string fStreamName;
  /// RawDigitproducer name (if needed) for ChStatus 
  std::string _chstatus_rawdigit_producer;
};


LiteScanner::LiteScanner(fhicl::ParameterSet const & p) 
  : EDAnalyzer(p)
  , fAlg()
 // More initializers here.
{
  fStreamName = p.get<std::string>("stream");

  //  fDataReadFlag.resize((size_t)(::larlite::data::kDATA_TYPE_MAX),std::map<std::string,
  fStoreAss = p.get<bool>("store_association");

  _chstatus_rawdigit_producer = p.get<std::string>("RawDigit4ChStatus","");

  fOutFileName = p.get<std::string>("out_filename","annonymous.root");
  if(p.get<bool>("unique_filename")) {
    TString tmp_fname(p.get<std::string>("out_filename","annonymous.root"));
    tmp_fname.ReplaceAll(".root","");
    TTimeStamp ts;
    fOutFileName = Form("%s_%08d_%06d_%06d.root",tmp_fname.Data(),ts.GetDate(),ts.GetTime(), (int)(ts.GetNanoSec()/1.e3));
  }
  _mgr.set_out_filename(fOutFileName);

  art::ServiceHandle<util::LLMetaMaker> metamaker;
  metamaker->addJson(fOutFileName,fStreamName);

  auto const data_pset = p.get<fhicl::ParameterSet>("DataLookUpMap");
  auto const ass_pset = p.get<fhicl::ParameterSet>("AssociationLookUpMap");
  for(size_t i = 0; i<(size_t)(::larlite::data::kDATA_TYPE_MAX); ++i) {

    std::vector<std::string> labels;
    switch((::larlite::data::DataType_t)i) {
    case ::larlite::data::kUndefined:
    case ::larlite::data::kEvent:
      break;
    default:

      labels = data_pset.get<std::vector<std::string> >(::larlite::data::kDATA_TREE_NAME[i].c_str(),labels);
      //std::cout<<::larlite::data::kDATA_TREE_NAME[i].c_str()<<" data product..."<<std::endl;
      for(auto const& label : labels) {
	//std::cout<<"  --"<<label.c_str()<<std::endl;
	fAlg.Register(label,(::larlite::data::DataType_t)i);
      }
      labels.clear();

      labels = ass_pset.get<std::vector<std::string> >(::larlite::data::kDATA_TREE_NAME[i].c_str(),labels);
      for(auto const& label : labels)
	fAlg.AssociationRegister(label,(::larlite::data::DataType_t)i);
    }
  }
  fPOTSummaryLabel_v = p.get<std::vector<std::string> >("pot_labels");

  fAssLabelToSave_s.clear();
  auto const& label_to_save_v = p.get<std::vector<std::string> >("AssLabelToSave",std::vector<std::string>());
  for(auto const& label : label_to_save_v) 
    fAssLabelToSave_s.insert(label);

  fAssProducer_v.clear();
  fAssProducer_v = p.get<std::vector<std::string> >("AssociationProducers",fAssProducer_v);
  fScanAssByProducers = p.get<bool>("ScanAssByProducers",true);
}
/*
template<> void LiteScanner::ScanAssociation<recob::Cluster>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::CosmicTag>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::EndPoint2D>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::SpacePoint>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::Track>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::Shower>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::Vertex>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::Calorimetry>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::ParticleID>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::Seed>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::PFParticle>(const art::Event& evt, const size_t name_index);
template<> void LiteScanner::ScanAssociation<recob::MCParticle>(const art::Event& evt, const size_t name_index);
*/
void LiteScanner::beginJob() {
  //_mgr.set_verbosity(larlite::msg::kDEBUG);
  //_mgr.set_out_filename("larlite.root");
  _mgr.set_io_mode(::larlite::storage_manager::kWRITE);
  _mgr.open();
}

void LiteScanner::endJob() {
  _mgr.close();
}

void LiteScanner::beginSubRun(const art::SubRun& sr)
{
  if(fPOTSummaryLabel_v.empty()) return;
  // POTSummary
  for(auto const& label : fPOTSummaryLabel_v) {
    auto lite_data = (::larlite::potsummary*)(_mgr.get_subrundata(::larlite::data::kPOTSummary,label));
    
    art::Handle< sumdata::POTSummary > potHandle;
    art::InputTag inputtag(label);
    sr.getByLabel(inputtag, potHandle);

    if(potHandle.isValid()) {
      lite_data->totpot     = potHandle->totpot;
      lite_data->totgoodpot = potHandle->totgoodpot;
      lite_data->totspills  = potHandle->totspills;
      lite_data->goodspills = potHandle->goodspills;
    }else{
      lite_data->totpot     = 0;
      lite_data->totgoodpot = 0;
      lite_data->totspills  = 0;
      lite_data->goodspills = 0;
    }
  }
}


void LiteScanner::analyze(art::Event const & e)
{
  fAlg.EventClear();
  _mgr.set_id(e.id().run(),
	      e.id().subRun(),
	      e.id().event());
  //auto const* ts = lar::providerFrom<detinfo::DetectorClocksService>();
  //ts->preProcessEvent(e);
  //
  /*
  std::cout<<" Run: " << _mgr.run_id() << " ... "
	   <<" SubRun: " << _mgr.subrun_id() << " ... "
	   <<" Event: " << _mgr.event_id() << std::endl;
  */
  //
  // Loop over data type to store association ptr map
  //
  SaveAssociationSource<simb::MCTruth>(e);
  SaveAssociationSource<simb::MCParticle>(e);
  //  SaveAssociationSource<anab::BackTrackerHitMatchingData>(e);
  SaveAssociationSource<recob::Hit>(e);
  SaveAssociationSource<recob::Cluster>(e);
  SaveAssociationSource<recob::EndPoint2D>(e);
  SaveAssociationSource<recob::SpacePoint>(e);
  SaveAssociationSource<recob::OpHit>(e);
  SaveAssociationSource<recob::OpFlash>(e);
  SaveAssociationSource<anab::CosmicTag>(e);
  SaveAssociationSource<recob::Track>(e);
  SaveAssociationSource<recob::Seed>(e);
  SaveAssociationSource<recob::Shower>(e);
  SaveAssociationSource<recob::Vertex>(e);
  SaveAssociationSource<recob::PFParticle>(e);
  SaveAssociationSource<anab::Calorimetry>(e);
  SaveAssociationSource<anab::ParticleID>(e);
  SaveAssociationSource<recob::PCAxis>(e);
  SaveAssociationSource<anab::FlashMatch>(e);
  //
  // Loop over data type to store data & locally art::Ptr
  //
  auto const& data_labels_v = fAlg.ModuleLabels();

  for(size_t i=0; i<data_labels_v.size(); ++i) {

    auto const& labels = data_labels_v[i];
    ::larlite::data::DataType_t lite_type = (::larlite::data::DataType_t)i;

    for(size_t j=0; j<labels.size(); ++j) {

      switch(lite_type) {
      case ::larlite::data::kGTruth:
	ScanData<simb::GTruth>(e,j); break;
      case ::larlite::data::kMCTruth:
	ScanData<simb::MCTruth>(e,j); break;
      case ::larlite::data::kMCFlux:
	ScanData<simb::MCFlux>(e,j); break;
      case ::larlite::data::kMCParticle:
	ScanData<simb::MCParticle>(e,j); break;
      case ::larlite::data::kSimPhotons:
	ScanSimPhotons(e,j); break;
      case ::larlite::data::kSimChannel:
	ScanData<sim::SimChannel>(e,j); break;
      case ::larlite::data::kAuxDetSimChannel:
	ScanData<sim::AuxDetSimChannel>(e,j); break;
      case ::larlite::data::kMCShower:
	ScanData<sim::MCShower>(e,j); break;
      case ::larlite::data::kMCTrack: 
	ScanData<sim::MCTrack>(e,j); break;

      case ::larlite::data::kRawDigit:
	ScanData<raw::RawDigit>(e,j); break;
      case ::larlite::data::kOpDetWaveform:
	ScanData<raw::OpDetWaveform>(e,j); break;
      case ::larlite::data::kTrigger:
	ScanData<raw::Trigger>(e,j); break;
      case ::larlite::data::kSWTrigger:
	ScanSimpleData<raw::ubdaqSoftwareTriggerData>(e,j); break;

      case ::larlite::data::kHit:
	ScanData<recob::Hit>(e,j); break;
      case ::larlite::data::kMCEventWeight:
 	ScanData<evwgh::MCEventWeight>(e,j); break;
      case ::larlite::data::kWire:
	ScanData<recob::Wire>(e,j); break;
      case ::larlite::data::kOpHit:
	ScanData<recob::OpHit>(e,j); break;
      case ::larlite::data::kOpFlash:
	ScanData<recob::OpFlash>(e,j); break;
      case ::larlite::data::kCluster:
	ScanData<recob::Cluster>(e,j); break;
      case ::larlite::data::kCosmicTag:
	ScanData<anab::CosmicTag>(e,j); break;
      case ::larlite::data::kEndPoint2D:
	ScanData<recob::EndPoint2D>(e,j); break;
      case ::larlite::data::kSpacePoint:
	ScanData<recob::SpacePoint>(e,j); break;
      case ::larlite::data::kSeed:
	ScanData<recob::Seed>(e,j); break;
      case ::larlite::data::kTrack:
	ScanData<recob::Track>(e,j); break;
      case ::larlite::data::kShower:
	ScanData<recob::Shower>(e,j); break;
      case ::larlite::data::kVertex:
	ScanData<recob::Vertex>(e,j); break;
      case ::larlite::data::kCalorimetry:
	ScanData<anab::Calorimetry>(e,j); break;
      case ::larlite::data::kParticleID:
	ScanData<anab::ParticleID>(e,j); break;
      case ::larlite::data::kPFParticle:
	ScanData<recob::PFParticle>(e,j); break;
      case ::larlite::data::kPCAxis:
	ScanData<recob::PCAxis>(e,j); break;
      case ::larlite::data::kFlashMatch:
	ScanData<anab::FlashMatch>(e,j); break;
      case ::larlite::data::kMuCSData:
	ScanData<MuCS::MuCSData>(e,j); break;
      case ::larlite::data::kMuCSReco:
	ScanData<MuCS::MuCSRecoData>(e,j); break;
	//case ::larlite::data::kPOTSummary:
	//break;
      case::larlite::data::kChStatus:
	FillChStatus(e,labels[j]); break;
      case ::larlite::data::kUndefined:
      case ::larlite::data::kEvent:
      default:
	continue;
      }
    }
  }

  auto const& ass_labels_v = fAlg.AssLabels();

  // Loop over data type to store association
  for(size_t i=0; fStoreAss && i<ass_labels_v.size(); ++i) {

    auto const& labels = ass_labels_v[i];
    ::larlite::data::DataType_t lite_type = (::larlite::data::DataType_t)i;

    for(size_t j=0; j<labels.size(); ++j) {

      auto const& label = labels[j];
      //std::cout << "This is the label I am scanning now: " << label << std::endl;
      ////////////////////////////////////////////////////////////////////////////////////////////
      if ( label == "gaushitTruthMatch" ){

        auto lite_ass = (::larlite::event_ass*)( _mgr.get_data(::larlite::data::kAssociation,"gaushitTruthMatch"));

        auto lite_hit = _mgr.get_data(::larlite::data::kHit,"gaushit");
        auto lite_mcpart = _mgr.get_data(::larlite::data::kMCParticle,"largeant");

        //std::cout<<"Now the real adventure begins... " <<std::endl ;
        auto const& hit_handle = e.getValidHandle<std::vector<recob::Hit>>("gaushit");
        auto const& hit_vec(*hit_handle);

        auto const& mcpart_handle= e.getValidHandle<std::vector<simb::MCParticle>>("largeant");
        auto const& mcpart_vec(*mcpart_handle);

        art::FindMany<simb::MCParticle,anab::BackTrackerHitMatchingData> particles_per_hit(hit_handle,e,"gaushitTruthMatch");

        std::vector<simb::MCParticle const*> particle_vec;
        std::vector<anab::BackTrackerHitMatchingData const*> match_v;

        ::larlite::AssSet_t hit_to_mcps_map ;
        hit_to_mcps_map.resize(hit_handle->size());
  
        for (auto& ass_unit : hit_to_mcps_map)
          ass_unit.reserve(100);
       
        //std::cout<<"Hit vec: "<<hit_vec.size()<<std::endl;
        for(int ii = 0; ii < int(hit_vec.size()) ; ii++){

          particle_vec.clear(); match_v.clear();
          double max_energy = -1 ;
          int best_match_id = -1 ;
        
          particles_per_hit.get(ii,particle_vec,match_v);

          for (size_t m = 0; m < match_v.size(); m++) {
            double this_energy = match_v[m]->energy;
            if (this_energy > max_energy) {
              best_match_id = m;
              max_energy = this_energy;
            }
          }
          
          if (best_match_id > -1) {

            int store_this_id = best_match_id ;
            for(int janky = 0; janky < int(mcpart_vec.size()); janky++ ){
              if ( mcpart_vec[janky].TrackId() == particle_vec[best_match_id]->TrackId() ){
                store_this_id = mcpart_vec[janky].TrackId() ;
                //store_this_id = janky ;
              }            
            }  
            try {
              hit_to_mcps_map[ii].emplace_back(store_this_id) ;
            } catch (cet::exception &e) {
              std::cout<<"CANT ADD!!! " <<std::endl ;
            }
          }
       }
       if (lite_ass)
         lite_ass->set_association(lite_hit->id(), lite_mcpart->id(), hit_to_mcps_map);
       }
        ///////////////////////////////

      if(!fAssLabelToSave_s.empty() && fAssLabelToSave_s.find(label) == fAssLabelToSave_s.end())
	continue;
      switch(lite_type) {
      case ::larlite::data::kCluster:
	ScanAssociation<recob::Cluster>(e,j); break;
      case ::larlite::data::kCosmicTag:
	ScanAssociation<anab::CosmicTag>(e,j); break;
      case ::larlite::data::kEndPoint2D:
	ScanAssociation<recob::EndPoint2D>(e,j); break;
      case ::larlite::data::kSpacePoint:
	ScanAssociation<recob::SpacePoint>(e,j); break;
      case ::larlite::data::kTrack:
	ScanAssociation<recob::Track>(e,j); break;
      case ::larlite::data::kShower:
	ScanAssociation<recob::Shower>(e,j); break;
      case ::larlite::data::kVertex:
	ScanAssociation<recob::Vertex>(e,j); break;
      case ::larlite::data::kCalorimetry:
	ScanAssociation<anab::Calorimetry>(e,j); break;
      case ::larlite::data::kParticleID:
	ScanAssociation<anab::ParticleID>(e,j); break;
      case ::larlite::data::kPFParticle:
	ScanAssociation<recob::PFParticle>(e,j); break;
      case ::larlite::data::kMCParticle:
	ScanAssociation<simb::MCParticle>(e,j); break;
      case ::larlite::data::kOpFlash:
	ScanAssociation<recob::OpFlash>(e,j); break;
	// Currently associations FROM the followings are not supported
      case ::larlite::data::kMuCSData:
      case ::larlite::data::kMuCSReco:
      case ::larlite::data::kMCTruth:
      case ::larlite::data::kOpHit: 
      case ::larlite::data::kSimChannel:
      case ::larlite::data::kAuxDetSimChannel:
      case ::larlite::data::kSimPhotons:
      case ::larlite::data::kMCShower:
      case ::larlite::data::kMCTrack:
      case ::larlite::data::kWire:
      case ::larlite::data::kHit:
      case ::larlite::data::kMCEventWeight:
	//ScanAssociation<recob::Hit>(e,j); break;
      case ::larlite::data::kUndefined:
      case ::larlite::data::kEvent:
      default:
	continue;
      }
    }
  }
  fAlg.EventClear();
  _mgr.next_event();
}

//-------------------------------------------------------------------------------------------------
// FillChStatus
//-------------------------------------------------------------------------------------------------
void LiteScanner::FillChStatus(const art::Event& e, const std::string& name)
{ 
  auto lite_chstatus = _mgr.get_data<larlite::event_chstatus>(name);
  auto const* geom = ::lar::providerFrom<geo::Geometry>();

  std::vector<bool> filled_ch( geom->Nchannels(), false );
  std::map<geo::PlaneID,std::vector<short> > status_m;

  // If specified check RawDigit pedestal value: if negative this channel is not used by wire (set status=>-2)
  if(!_chstatus_rawdigit_producer.empty()) {
    art::Handle<std::vector<raw::RawDigit> > digit_h;

    std::string label = _chstatus_rawdigit_producer;
    art::InputTag inputtag(label);
    e.getByLabel(inputtag, digit_h);
      
    for(auto const& digit : *digit_h) {
      auto const ch = digit.Channel();
      if(ch >= filled_ch.size()) throw ::larlite::DataFormatException("Found RawDigit > possible channel number!");
      if(digit.GetPedestal()<0.) {
	auto const wid =  geom->ChannelToWire(ch).front();
	auto iter = status_m.find(wid.planeID());
	if(iter != status_m.end())
	  (*iter).second[wid.Wire] = -2;
	else{
	  std::vector<short> status_v(geom->Nwires(wid.planeID()),5);
	  status_v[wid.Wire] = -2;
	  status_m.emplace(wid.planeID(),status_v);
	}
	filled_ch[ch] = true;
      }
    }
  }

  // Set database status                                                                                                                                
  const lariov::ChannelStatusProvider& chanFilt = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();
  for(size_t i=0; i < geom->Nchannels(); ++i) {
    if ( filled_ch[i] ) continue;
    auto const wid =  geom->ChannelToWire(i).front();
    short status = 0;
    if (!chanFilt.IsPresent(i)) status = -1;
    else status = (short)(chanFilt.Status(i));

    auto iter = status_m.find(wid.planeID());
    if(iter != status_m.end())
      (*iter).second[wid.Wire] = status;
    else{
      std::vector<short> status_v(geom->Nwires(wid.planeID()),5);
      status_v[wid.Wire] = status;
      status_m.emplace(wid.planeID(),status_v);
    }    
  }
  
  // store
  for(auto& plane_status : status_m) {
    auto const& pid = plane_status.first;
    auto& status_v = plane_status.second;
    ::larlite::geo::PlaneID lite_pid(pid.Cryostat, pid.TPC, pid.Plane);
    ::larlite::chstatus status;
    status.set_status(lite_pid,std::move(status_v));
    lite_chstatus->emplace_back(status);
  }
}

//-------------------------------------------------------------------------------------------------
// Scan
//-------------------------------------------------------------------------------------------------
template<class T> void LiteScanner::ScanData(const art::Event& evt, const size_t name_index)
{ 

  auto lite_id = fAlg.ProductID<T>(name_index);
  std::string label = lite_id.second;
  auto lite_data = _mgr.get_data((::larlite::data::DataType_t)lite_id.first,label);

  art::Handle<std::vector<T> > dh;
  // All cases except for optical
  if(lite_id.first == ::larlite::data::kOpDetWaveform) {
    art::ServiceHandle<geo::UBOpReadoutMap> ub_pmt_channel_map;

    art::InputTag inputtag(label);
    evt.getByLabel(inputtag, dh);

    if(dh.isValid()) fAlg.ScanData(dh,lite_data);

    for ( unsigned int cat=0; cat<(unsigned int)opdet::NumUBOpticalChannelCategories; cat++ ) {
      
      evt.getByLabel(label, opdet::UBOpChannelEnumName( (opdet::UBOpticalChannelCategory_t)cat ), dh);

      if(!dh.isValid()) continue;

      fAlg.ScanData(dh,lite_data);

    }
  }
  else{    
    art::InputTag inputtag(label);
    evt.getByLabel(inputtag, dh);
    
    if(!dh.isValid()) return;
    fAlg.ScanData(dh,lite_data);
  }
}

//-------------------------------------------------------------------------------------------------
// Scan
//-------------------------------------------------------------------------------------------------
template<class T> void LiteScanner::ScanSimpleData(const art::Event& evt, const size_t name_index)
{ 
  auto lite_id = fAlg.ProductID<T>(name_index);

  auto lite_data = _mgr.get_data((::larlite::data::DataType_t)lite_id.first,lite_id.second);
  std::string label=lite_id.second;
  //std::cout << label.substr()
  art::Handle<T> dh;
  art::InputTag inputtag(label);
  evt.getByLabel(inputtag,dh);
    
  if(!dh.isValid()) return;
  fAlg.ScanSimpleData(dh,lite_data);
}

//-------------------------------------------------------------------------------------------------
// Scan
//-------------------------------------------------------------------------------------------------
void LiteScanner::ScanSimPhotons(const art::Event& evt, const size_t name_index)
{ 
  auto lite_id = fAlg.ProductID<::sim::SimPhotonsCollection>(name_index);
  auto lite_data = (::larlite::event_simphotons*)(_mgr.get_data((::larlite::data::DataType_t)lite_id.first,lite_id.second));
  std::string label = lite_id.second;
  art::Handle< std::vector<sim::SimPhotons> > dh;

  art::InputTag inputtag(label);
  evt.getByLabel(inputtag,dh);
    
  if(!dh.isValid()) return;

  lite_data->reserve(dh->size());
  for(auto& simph : *dh) {
    
    ::larlite::simphotons lite_simph;
    ::larlite::onephoton  lite_photon;

    lite_simph.SetChannel(simph.OpChannel());
    lite_simph.reserve(simph.size());

    for(auto const& ph : simph) {

      lite_photon.SetInSD = ph.SetInSD;
      lite_photon.InitialPosition = geo::vect::toTVector3(ph.InitialPosition);
      lite_photon.FinalLocalPosition = geo::vect::toTVector3(ph.FinalLocalPosition);
      lite_photon.Time = ph.Time;
      lite_photon.Energy = ph.Energy;

      lite_simph.push_back(lite_photon);
    }
    lite_data->emplace_back(lite_simph);
  }
}


//-------------------------------------------------------------------------------------------------
// SaveAssociationSource
//-------------------------------------------------------------------------------------------------
template<class T> void LiteScanner::SaveAssociationSource(const art::Event& evt)
{

  auto lite_type = fAlg.LiteDataType<T>();
  auto const& ass_labels_v = fAlg.AssLabels();

  //std::cout<<"Ass labels size: "<<ass_labels_v.size()<<", "<<lite_type<<std::endl ;

  for(size_t i=0; i<ass_labels_v[lite_type].size(); ++i) {
    

    auto const& name = ass_labels_v[lite_type][i];
    
    //std::cout <<"ASS LABEL NAME !! "<< name << ", "<<lite_type<<std::endl;

    art::Handle<std::vector<T> > dh;
    art::InputTag inputtag(name);
    evt.getByLabel(inputtag,dh);
    
    if(!dh.isValid() || !(dh->size())) continue; 

    //std::cout<<"Now building association for "<<name<<std::endl ;
    for(size_t j=0; j<dh->size(); ++j) {
      //std::cout<<"j "<<j<<std::endl ;
      const art::Ptr<T> ptr(dh,j);

      size_t key1, key2;
      fAlg.ProducePtrMapKey(ptr,key1,key2);
      auto& ptr_map = fAlg.GetPtrMap<T>(key1,key2);

      ptr_map[ptr] = std::make_pair(j,i);
      //std::cout<<"Pointer map now adjusted "<<std::endl ;

    }

  }

} 

//-------------------------------------------------------------------------------------------------
// ScanAssociation
//-------------------------------------------------------------------------------------------------
template<class T> void LiteScanner::ScanAssociation(const art::Event& evt, const size_t name_index)
{ 
  //std::cout<<"Inside ScanAssociation.... "<<name_index<<std::endl ;
  auto lite_id = fAlg.AssProductID<T>(name_index);
  art::Handle<std::vector<T> > dh;
  std::string label = lite_id.second;

  //std::cout << "\nI am scanning for this assocaition : " << label << std::endl; 
  // Done adding this block in
  art::InputTag inputtag(label);
  evt.getByLabel(inputtag,dh);
        
  if(!dh.isValid()) return;

  //std::cout<<"Inspecting association for type " << lite_id.first << " by " << lite_id.second << std::endl;

  for(auto const& ass_producer : fAssProducer_v) {

    auto lite_ass = (::larlite::event_ass*)(_mgr.get_data(::larlite::data::kAssociation,ass_producer));
    /*
    std::cout << "LArLite product type: " << lite_id.first
	      << " ... label: " << lite_id.second
	      << " ... pointer: " << lite_ass << std::endl;
    */    
    switch(lite_id.first){
    case ::larlite::data::kUndefined:    break;
    case ::larlite::data::kEvent:        break;
    case ::larlite::data::kGTruth:       break;
    case ::larlite::data::kMCTruth:      break;
    case ::larlite::data::kMCParticle:
      fAlg.ScanAssociation<T, simb::MCTruth     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kMCFlux:        break;
    case ::larlite::data::kMCTrajectory:  break;
    case ::larlite::data::kMCNeutrino:    break;
    case ::larlite::data::kRawDigit:      break;
    case ::larlite::data::kOpDetWaveform: break;
    case ::larlite::data::kSimPhotons:    break;
    case ::larlite::data::kTrigger:       break;
    case ::larlite::data::kSWTrigger:     break;
    case ::larlite::data::kWire:          break;
    case ::larlite::data::kHit:           break;
    case ::larlite::data::kMuCSData:      break;
    case ::larlite::data::kMuCSReco:      break;
    case ::larlite::data::kCosmicTag:
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::PCAxis     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kOpHit:        break;
    case ::larlite::data::kOpFlash:
      fAlg.ScanAssociation<T, recob::OpHit      > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kCluster:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kSeed:         break;
    case ::larlite::data::kSpacePoint:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kTrack:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::CosmicTag   > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::ParticleID  > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::Calorimetry > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kShower:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kVertex:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::EndPoint2D > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kEndPoint2D:
      //fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kMCEventWeight: break;
    case ::larlite::data::kCalorimetry:
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kParticleID:
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kPFParticle:
      //fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Seed       > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::PCAxis     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::CosmicTag   > (evt,dh,lite_ass);
      break;
    default:
      break;
    }
  }

  if(fScanAssByProducers) {

    auto lite_ass = (::larlite::event_ass*)(_mgr.get_data(::larlite::data::kAssociation,lite_id.second));
    
    switch(lite_id.first){
    case ::larlite::data::kUndefined:    break;
    case ::larlite::data::kEvent:        break;
    case ::larlite::data::kGTruth:       break;
    case ::larlite::data::kMCTruth:      break;
    case ::larlite::data::kMCParticle:
      fAlg.ScanAssociation<T, simb::MCTruth     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kMCFlux:        break;
    case ::larlite::data::kMCTrajectory:  break;
    case ::larlite::data::kMCNeutrino:    break;
    case ::larlite::data::kRawDigit:      break;
    case ::larlite::data::kOpDetWaveform: break;
    case ::larlite::data::kSimPhotons:    break;
    case ::larlite::data::kTrigger:       break;
    case ::larlite::data::kWire:          break;
    case ::larlite::data::kHit:           break;
    case ::larlite::data::kMuCSData:      break;
    case ::larlite::data::kMuCSReco:      break;
    case ::larlite::data::kCosmicTag:
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::PCAxis     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kOpHit:        break;
    case ::larlite::data::kOpFlash:
      fAlg.ScanAssociation<T, recob::OpHit      > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kCluster:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kSeed:         break;
    case ::larlite::data::kSpacePoint:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kTrack:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::CosmicTag   > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::ParticleID  > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::Calorimetry > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kShower:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kVertex:
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::EndPoint2D > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kEndPoint2D:
      //fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kCalorimetry:
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kParticleID:
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      //fAlg.ScanAssociation<T, recob::Shower     > (evt,dh,lite_ass);
      break;
    case ::larlite::data::kPFParticle:
      //fAlg.ScanAssociation<T, recob::Hit        > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Cluster    > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::SpacePoint > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Track      > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Seed       > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::PCAxis     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, recob::Vertex     > (evt,dh,lite_ass);
      fAlg.ScanAssociation<T, anab::CosmicTag   > (evt,dh,lite_ass);
      break;
    default:
      break;
    }
  }
  //lite_ass->list_association();
}

DEFINE_ART_MODULE(LiteScanner)
