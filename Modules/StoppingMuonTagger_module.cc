////////////////////////////////////////////////////////////////////////
// Class:       StoppingMuonTagger
// Plugin Type: producer (art v2_05_00)
// File:        StoppingMuonTagger_module.cc
//
// Generated at Fri Oct  6 15:55:20 2017 by Marco Del Tutto using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "canvas/Persistency/Common/FindManyP.h"

#include <memory>

// Data products include
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/OpHit.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "uboone/UBXSec/DataTypes/FlashMatch.h"
#include "uboone/UBXSec/DataTypes/MCGhost.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "uboone/UBXSec/DataTypes/TPCObject.h"

// LArSoft include
#include "uboone/UBFlashFinder/PECalib.h"
#include "larsim/MCCheater/BackTracker.h"

//Algorithms include
#include "uboone/UBXSec/Algorithms/FiducialVolume.h"
#include "uboone/UBXSec/Algorithms/StoppingMuonTaggerHelper.h"

// Root include
#include "TString.h"
#include "TTree.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"


class StoppingMuonTagger;


class StoppingMuonTagger : public art::EDProducer {
public:
  explicit StoppingMuonTagger(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  StoppingMuonTagger(StoppingMuonTagger const &) = delete;
  StoppingMuonTagger(StoppingMuonTagger &&) = delete;
  StoppingMuonTagger & operator = (StoppingMuonTagger const &) = delete;
  StoppingMuonTagger & operator = (StoppingMuonTagger &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;

private:

  //::art::ServiceHandle<cheat::BackTracker> bt;
  ::art::ServiceHandle<geo::Geometry> geo;
  //::art::ServiceHandle<detinfo::DetectorPropertiesService> det;
  detinfo::DetectorProperties const* fDetectorProperties;

  ::ubana::FiducialVolume _fiducial_volume;

  ::ubana::StoppingMuonTaggerHelper _helper;

  std::string _tpcobject_producer;


  TH1D * _h_nstopmu;
  TH1D * _h_stopmu_type;
};


StoppingMuonTagger::StoppingMuonTagger(fhicl::ParameterSet const & p) {


  _fiducial_volume.Configure(p.get<fhicl::ParameterSet>("FiducialVolumeSettings"),
                             geo->DetHalfHeight(),
                             2.*geo->DetHalfWidth(),
                             geo->DetLength());

  _fiducial_volume.PrintConfig();

  _tpcobject_producer             = p.get<std::string>("TPCObjectProducer", "TPCObjectMaker");

  fDetectorProperties = lar::providerFrom<detinfo::DetectorPropertiesService>(); 

  art::ServiceHandle<art::TFileService> fs;
  _h_nstopmu = fs->make<TH1D>("h_nstopmu", ";Stopping Muons Per Event;", 10, 0, 10);
  _h_stopmu_type = fs->make<TH1D>("h_stopmu_type", "Stopping Muon Chategories;;", 10, 0, 10);
}

void StoppingMuonTagger::produce(art::Event & e) {

  std::cout <<"[StoppingMuonTagger] Starts." << std::endl;

  // Get TPCObjects from the Event
  art::Handle<std::vector<ubana::TPCObject>> tpcobj_h;
  e.getByLabel(_tpcobject_producer, tpcobj_h);
  if (!tpcobj_h.isValid()) {
    std::cout << "[StoppingMuonTagger] Cannote locate ubana::TPCObject." << std::endl;
  }
  //art::FindManyP<ubana::FlashMatch> tpcobjToFlashMatchAssns(tpcobj_h, e, _neutrino_flash_match_producer);
  art::FindManyP<recob::Track>      tpcobjToTrackAssns(tpcobj_h, e, _tpcobject_producer);
  art::FindManyP<recob::PFParticle> tpcobjToPFPAssns(tpcobj_h, e, _tpcobject_producer);
  art::FindManyP<recob::Shower>     tpcobjToShowerAssns(tpcobj_h, e, _tpcobject_producer);
  //art::FindManyP<anab::CosmicTag>   tpcobjToCosmicTagAssns(tpcobj_h, e, _geocosmictag_producer);

  std::vector<art::Ptr<ubana::TPCObject>> tpcobj_v;
  art::fill_ptr_vector(tpcobj_v, tpcobj_h);

  /*
  // Collect tracks and map track->hit
  lar_pandora::TrackVector track_v;
  lar_pandora::TracksToHits tracks_to_hits;
  lar_pandora::LArPandoraHelper::CollectTracks(e, "pandoraNu::UBXSec", track_v, tracks_to_hits);

  // Collect showers and map shower->hit
  lar_pandora::ShowerVector shower_v;
  lar_pandora::ShowersToHits showers_to_hits; 
  lar_pandora::LArPandoraHelper::CollectShowers(e, "pandoraNu::UBXSec", shower_v, showers_to_hits); 
*/
  // Collect pfparticles and map pfp->spacepoint
  lar_pandora::PFParticleVector pfp_v;
  lar_pandora::PFParticlesToSpacePoints pfps_to_spacepoints;
  lar_pandora::LArPandoraHelper::CollectPFParticles(e, "pandoraNu::UBXSec", pfp_v, pfps_to_spacepoints);


  // Collect pfparticles and map pfp->cluster
  lar_pandora::PFParticlesToClusters pfps_to_clusters;
  lar_pandora::LArPandoraHelper::CollectPFParticles(e, "pandoraNu::UBXSec", pfp_v, pfps_to_clusters);

  // Collect clusters and map cluster->hit
  lar_pandora::ClusterVector cluster_v;
  lar_pandora::ClustersToHits clusters_to_hits;
  lar_pandora::LArPandoraHelper::CollectClusters(e, "pandoraNu::UBXSec", cluster_v, clusters_to_hits);

  // Collect hits
  art::Handle<std::vector<recob::Hit>> hit_h;
  e.getByLabel("pandoraCosmicHitRemoval", hit_h); 
  if(!hit_h.isValid()) {
    std::cout << "[StoppingMuonTagger] Cannote locate recob::Hit." << std::endl;
  }
  std::vector<art::Ptr<recob::Hit>> hit_v;
  art::fill_ptr_vector(hit_v, hit_h);
  for (auto h : hit_v){
    if (h->WireID().Wire == 2056) {
      std::cout << "Found hit on wire 2056, time is " << h->PeakTime() << std::endl;
    }
  }

  int n_stopmu = 0;

  for (size_t i = 0; i < tpcobj_v.size(); i++) {

    std::cout << "[StoppingMuonTagger] >>>>> TPCObject " << i << std::endl;

    art::Ptr<ubana::TPCObject> tpcobj = tpcobj_v.at(i);

    std::vector<art::Ptr<recob::Track>>      tracks  = tpcobjToTrackAssns.at(tpcobj.key());
    std::vector<art::Ptr<recob::PFParticle>> pfps    = tpcobjToPFPAssns.at(tpcobj.key());
    std::vector<art::Ptr<recob::Shower>>     showers = tpcobjToShowerAssns.at(tpcobj.key());

    if (!e.isRealData()) {

      if (tpcobj->GetOrigin() != ubana::TPCObjectOrigin::kCosmicRay) continue;

      if (tpcobj->GetOriginExtra() != ubana::TPCObjectOriginExtra::kStoppingMuon) continue;

      std::cout << "[StoppingMuonTagger] Found TPCObject representing a stopping cosmic muon." << std::endl;
      n_stopmu++;

      if (tracks.size() == 1 && showers.size() == 0)          // One track - bin 0
        _h_stopmu_type->Fill(0);
      else if (tracks.size() == 0 && showers.size() == 1)     // One shower - bin 1
        _h_stopmu_type->Fill(1);
      else if (tracks.size() == 1 && showers.size() == 1)     // One track One shower - bin 2
        _h_stopmu_type->Fill(2);
      else if (tracks.size() == 2 && showers.size() == 0)     // Two track Zero shower - bin 3
        _h_stopmu_type->Fill(3);
      else if (tracks.size() == 0 && showers.size() == 2)     // Zero track Two shower - bin 4
        _h_stopmu_type->Fill(4);
      else                                                    // Others
        _h_stopmu_type->Fill(5);

    }

    std::cout <<"[StoppingMuonTagger]\t Number of tracks for this TPCObject:  " << tracks.size()  << std::endl;
    std::cout <<"[StoppingMuonTagger]\t Number of showers for this TPCObject: " << showers.size() << std::endl;
    std::cout <<"[StoppingMuonTagger]\t Number of PFPs for this TPCObject:    " << pfps.size()    << std::endl;


    // Get all the hits ...
    std::vector<art::Ptr<recob::Hit>> hit_v;
    hit_v.clear();

    /*
    // ...from tracks
    for (auto t : tracks) {
      auto iter = tracks_to_hits.find(t);
      if (iter == tracks_to_hits.end()) {
        std::cout << "[StoppingMuonTagger] Track in TPCObject not found by pandora!?" << std::endl;
        throw std::exception();
      }
      hit_v.reserve(hit_v.size() + iter->second.size());
      hit_v.insert(hit_v.end(), iter->second.begin(), iter->second.end());     
    }

    // ...from showers
    for (auto s : showers) { 
      auto iter = showers_to_hits.find(s);
      if (iter == showers_to_hits.end()) { 
        std::cout << "[StoppingMuonTagger] Shower in TPCObject not found by pandora!?" << std::endl;
        throw std::exception();
      }
      hit_v.reserve(hit_v.size() + iter->second.size()); 
      hit_v.insert(hit_v.end(), iter->second.begin(), iter->second.end());
    }
    */

    // Get all the Hits SpacePoints from the PFPs
    std::vector<art::Ptr<recob::SpacePoint>> sp_v;
    sp_v.clear();
    for (auto p : pfps) {
      auto iter = pfps_to_spacepoints.find(p);
      if (iter == pfps_to_spacepoints.end()) { 
        continue;
      }
      sp_v.reserve(sp_v.size() + iter->second.size());
      sp_v.insert(sp_v.end(), iter->second.begin(), iter->second.end());

      // Find clusters first ...
      auto iter2 = pfps_to_clusters.find(p);
      if (iter2 == pfps_to_clusters.end()) { 
        continue;
      }

      // ... then find hits
      for (auto c : iter2->second) {
        auto iter3 = clusters_to_hits.find(c);
        if (iter3 == clusters_to_hits.end()) {
          std::cout << "[StoppingMuonTagger] Cluster in TPCObject not found by pandora !?" << std::endl;
          throw std::exception(); 
        }

        hit_v.reserve(hit_v.size() + iter3->second.size());
        hit_v.insert(hit_v.end(), iter3->second.begin(), iter3->second.end());
      } 
    }

    // Now get the highest point
    std::sort(sp_v.begin(), sp_v.end(),
              [](art::Ptr<recob::SpacePoint> a, art::Ptr<recob::SpacePoint> b) -> bool
              {
                const double *xyz_a = a->XYZ();
                const double *xyz_b = b->XYZ();
                return xyz_a[1] > xyz_b[1];
              });

    const double *highest_point = sp_v.at(0)->XYZ();
    //raw::ChannelID_t ch = geo->NearestChannel(highest_point, 2);
    int w = geo->NearestWire(highest_point, 2);
    size_t time = fDetectorProperties->ConvertXToTicks(highest_point[0], geo::PlaneID(0,0,2));
    std::cout << "[StoppingMuonTagger] Highest point: wire: " << w << ", time: " << time << std::endl;

    /*
    double time2cm = 0.1;
    double triggerOffset = 3200;
    double planeOrigin[3] = {0.0, -0.3, -0.6};
    double offset_collection = triggerOffset * time2cm - planeOrigin[2];
    size_t time = highest_point[0]/time2cm + offset_collection;  
    */

    std::cout << "[StoppingMuonTagger] Now create simple hit vector, size " << hit_v.size() << std::endl;

    std::cout << "MARCO: 1000 -> " << fDetectorProperties->ConvertTicksToX(1000, geo::PlaneID(0,0,2))<< std::endl;
    std::cout << "MARCO: 1001 -> " << fDetectorProperties->ConvertTicksToX(1001, geo::PlaneID(0,0,2))<< std::endl;
    std::cout << "MARCO: 1002 -> " << fDetectorProperties->ConvertTicksToX(1002, geo::PlaneID(0,0,2))<< std::endl;

    // Create SimpleHit vector
    ubana::SimpleHitVector shit_v;
    for (auto h : hit_v) {
      ubana::SimpleHit shit;
      shit.t = fDetectorProperties->ConvertTicksToX(h->PeakTime(), 
                                                       geo::PlaneID(0,0,2));
      shit.w = h->WireID().Wire * geo->WirePitch(geo::PlaneID(0,0,2));
      shit.plane = h->View();
      shit.integral = h->Integral();

      shit.time = h->PeakTime();
      shit.wire = h->WireID().Wire;

      //std::cout << "Emplacing hit with time " << shit.time << ", and wire " << shit.wire << ", on plane " << shit.plane << std::endl;
      shit_v.emplace_back(shit);
    }

    std::cout << "[StoppingMuonTagger] Simple hit vector size " << shit_v.size() << std::endl;

    // Emplace to the helper
    std::cout << "[StoppingMuonTagger] Now emplace hits" << std::endl;
    _helper.Emplace(shit_v);

    // Only collection plane hits
    std::cout << "[StoppingMuonTagger] Now filter hits by plane" << std::endl;
    _helper.FilterByPlane(2);

    
    // Set the start hit
    std::cout << "[StoppingMuonTagger] Now set start hit" << std::endl;
    _helper.SetStartHit(time, w, 2); 

    // Order hits
    std::cout << "[StoppingMuonTagger] Now order hits" << std::endl;
    _helper.OrderHits();

    // dQds
    std::cout << "[StoppingMuonTagger] Now calculate dqds" << std::endl; 
    _helper.CalculatedQds();    

    // dQds Slider
    std::cout << "[StoppingMuonTagger] Now perform window slider" << std::endl;
    _helper.PerformdQdsSlider();

    // Make a decision
    std::cout << "[StoppingMuonTagger] Now make a decision" << std::endl;
    bool result = _helper.MakeDecision();

    std::cout << "[StoppingMuonTagger] Is stoppin muon? " << (result ? "YES" : "NO") << std::endl;

  } // TPCObject loop

  _h_nstopmu->Fill(n_stopmu);

  std::cout <<"[StoppingMuonTagger] Ends." << std::endl;

}

DEFINE_ART_MODULE(StoppingMuonTagger)
