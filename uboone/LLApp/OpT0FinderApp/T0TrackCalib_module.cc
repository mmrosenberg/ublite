////////////////////////////////////////////////////////////////////////
// Class:       T0TrackCalib
// Module Type: analyzer
// File:        T0TrackCalib_module.cc
//
// Generated at Wed Sep 14 09:24:08 2016 by Kazuhiro Terao using artmod
// from cetpkgsupport v1_10_02.
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Common/FindManyP.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/CryostatGeo.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcorealg/Geometry/OpDetGeo.h"
#include "uboone/Geometry/UBOpReadoutMap.h"

#include <memory>
#include <set>
#include "uboone/LLBasicTool/GeoAlgo/GeoTrajectory.h"
#include "uboone/LLSelectionTool/OpT0Finder/Base/FlashMatchManager.h"
#include "uboone/LLSelectionTool/OpT0Finder/Algorithms/LightPath.h"
#include "TString.h"
#include "TTree.h"

class T0TrackCalib;

class T0TrackCalib : public art::EDAnalyzer {
public:
  explicit T0TrackCalib(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  T0TrackCalib(T0TrackCalib const &) = delete;
  T0TrackCalib(T0TrackCalib &&) = delete;
  T0TrackCalib & operator = (T0TrackCalib const &) = delete;
  T0TrackCalib & operator = (T0TrackCalib &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;


private:

  ::flashana::FlashMatchManager _mgr;
  
  std::vector<flashana::FlashMatch_t> _result;
  
  // Configurable params
  std::string _config_file;
  std::string _track_producer;
  std::string _opflash_producer_beam;
  std::string _opflash_producer_cosmic;
  double _flash_trange_start;
  double _flash_trange_end;
  size_t _num_tracks;
  std::vector<double> _gain_correction;

  TTree* _tree1;
  int _run, _subrun, _event, _matchid;
  int _tpcid, _flashid;
  double _tpc_xmin, _qll_xmin;
  double _t0, _score;
  double _hypo_pe, _flash_pe;

  TTree* _tree2;
  std::vector<double> _flash_spec;
  std::vector<double> _hypo_spec;

};


T0TrackCalib::T0TrackCalib(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{
  _track_producer          = p.get<std::string>("TrackProducer");
  _opflash_producer_beam   = p.get<std::string>("BeamOpFlashProducer");
  _opflash_producer_cosmic = p.get<std::string>("CosmicOpFlashProducer");
  _flash_trange_start      = p.get<double>("FlashVetoTimeStart");
  _flash_trange_end        = p.get<double>("FlashVetoTimeEnd");
  _gain_correction         = p.get<std::vector<double> >("GainCorrection");

  ::art::ServiceHandle<geo::Geometry> geo;
  ::art::ServiceHandle<geo::UBOpReadoutMap> ub_geo;
  
  if(geo->NOpDets() != _gain_correction.size()) {
    std::cout << "GainCorrection array size is " << _gain_correction.size() << " != # OpDet " << geo->NOpDets() << std::endl;
    throw std::exception();
  }
  
  _mgr.Configure(p.get<flashana::Config_t>("FlashMatchConfig"));

  _flash_spec.resize(geo->NOpDets(),0.);
  _hypo_spec.resize(geo->NOpDets(),0.);

  art::ServiceHandle<art::TFileService> fs;

  _tree1 = fs->make<TTree>("tree","");
  _tree1->Branch("run",&_run,"run/I");
  _tree1->Branch("subrun",&_subrun,"subrun/I");
  _tree1->Branch("event",&_event,"event/I");
  _tree1->Branch("matchid",&_matchid,"matchid/I");
  _tree1->Branch("tpcid",&_tpcid,"tpcid/I");
  _tree1->Branch("flashid",&_flashid,"flashid/I");
  _tree1->Branch("tpc_xmin",&_tpc_xmin,"tpc_xmin/D");
  _tree1->Branch("qll_xmin",&_qll_xmin,"qll_xmin/D");
  _tree1->Branch("t0",&_t0,"t0/D");
  _tree1->Branch("score",&_score,"score/D");
  _tree1->Branch("hypo_pe",&_hypo_pe,"hypo_pe/D");
  _tree1->Branch("flash_pe",&_flash_pe,"flash_pe/D");


  _tree2 = fs->make<TTree>("spectree","");
  _tree2->Branch("run",&_run,"run/I");
  _tree2->Branch("subrun",&_subrun,"subrun/I");
  _tree2->Branch("event",&_event,"event/I");
  _tree2->Branch("matchid",&_matchid,"matchid/I");
  _tree2->Branch("flash_spec","std::vector<double>",&_flash_spec);
  _tree2->Branch("hypo_spec","std::vector<double>",&_hypo_spec);

}

void T0TrackCalib::analyze(art::Event const & e)
{
  // _mgr.PrintConfig();

  std::cout << "********* T0TrackCalib::analyze starts" << std::endl;

  art::ServiceHandle<geo::Geometry> geo;
  art::ServiceHandle<geo::UBOpReadoutMap> ub_geo;

  art::Handle<std::vector<anab::T0> > t0_h;
  e.getByLabel(Form("T0TrackTagger%s",_track_producer.c_str()),t0_h);
  if(!t0_h.isValid()) {
    std::cout << "T0 product not found..." << std::endl;
    throw std::exception();
  }
  if(t0_h->empty()) {
    std::cout << "t0 is empty. Exit now." << std::endl;
    return;
  }

  std::set<art::Ptr<recob::OpFlash> > cosmic_flash_s;
  std::set<art::Ptr<recob::OpFlash> > beam_flash_s;
  art::Handle<std::vector<recob::OpFlash> > beam_flash_h;
  art::Handle<std::vector<recob::OpFlash> > cosmic_flash_h;
  e.getByLabel(_opflash_producer_beam,beam_flash_h);
  e.getByLabel(_opflash_producer_cosmic,cosmic_flash_h);
  if(beam_flash_h.isValid()) {
    for(size_t i=0; i<beam_flash_h->size(); ++i) {
      art::Ptr<recob::OpFlash> flash_ptr(beam_flash_h,i);
      beam_flash_s.insert(flash_ptr);
    }
  }
  if(cosmic_flash_h.isValid()) {
    for(size_t i=0; i<cosmic_flash_h->size(); ++i) {
      art::Ptr<recob::OpFlash> flash_ptr(cosmic_flash_h,i);
      cosmic_flash_s.insert(flash_ptr);
    }
  }

  if(cosmic_flash_s.empty() && beam_flash_s.empty()) {
    std::cout << "No OpFlash found..." << std::endl;
    return;
  }
 
  art::FindManyP<recob::Track>   track_ptr_coll_v(t0_h, e, Form("T0TrackTagger%s",_track_producer.c_str())); 
  art::FindManyP<recob::OpFlash> flash_ptr_coll_v(t0_h, e, Form("T0TrackTagger%s",_track_producer.c_str()));

  // See if there is an interesting flash
  // Loop over all the flashes in the event
  for(size_t i=0; i<flash_ptr_coll_v.size(); ++i) {

    if(flash_ptr_coll_v.at(i).size()>1 || track_ptr_coll_v.at(i).size()>1) { 
      std::cout << "More than 1 association found!" << std::endl;
      throw std::exception();
    }
    art::Ptr<recob::OpFlash> flash_ptr = flash_ptr_coll_v.at(i).at(0);
    if(flash_ptr->Time() < _flash_trange_start || _flash_trange_end < flash_ptr->Time()) {
      std::cout << "Ignoring flash: time not in range " << std::endl;
      continue;
    }

    _mgr.Reset();
    _result.clear();

    // Fill flash info
    bool beam_flash = (beam_flash_s.find(flash_ptr) != beam_flash_s.end());
    if(!beam_flash && cosmic_flash_s.find(flash_ptr) == cosmic_flash_s.end()) {
      std::cout << "Unregistered OpFlash found (associated OpFlash match neither of beam/cosmic opflash producer you specified" << std::endl;
      throw std::exception();
    }
    ::flashana::Flash_t f;
    f.x = f.x_err = 0;
    f.y = flash_ptr->YCenter();
    f.z = flash_ptr->ZCenter();
    f.y_err = flash_ptr->YWidth();
    f.z_err = flash_ptr->ZWidth();
    f.pe_v.resize(geo->NOpDets());
    f.pe_err_v.resize(geo->NOpDets());
    for (unsigned int i = 0; i < f.pe_v.size(); i++) {
      unsigned int opdet = geo->OpDetFromOpChannel(i);
      if(beam_flash) {
	f.pe_v[opdet] = flash_ptr->PE(i) / _gain_correction[i];
	f.pe_err_v[opdet] = sqrt(flash_ptr->PE(i) / _gain_correction[i]);
      }else{
	f.pe_v[opdet] = flash_ptr->PE(i) / _gain_correction[i] / 0.424;
	f.pe_err_v[opdet] = sqrt(flash_ptr->PE(i) / _gain_correction[i]) / 0.424;
      }
    }

    f.time = flash_ptr->Time();
    f.idx = 0;
    _mgr.Emplace(std::move(f));

    // Fill tpc info
    const art::Ptr<recob::Track>   track_ptr = track_ptr_coll_v.at(i).at(0);
    ::geoalgo::Trajectory geotrj;
    geotrj.reserve(track_ptr->NumberTrajectoryPoints() - 1);
    for (size_t i = 0; i < track_ptr->NumberTrajectoryPoints(); ++i) {
      ::geoalgo::Vector pt(track_ptr->LocationAtPoint(i));
      geotrj.emplace_back(std::move(pt));
    }
    auto qcluster = ((flashana::LightPath*)(_mgr.GetCustomAlgo("LightPath")))->FlashHypothesis(geotrj);
    _mgr.Emplace(std::move(qcluster));


    _result = _mgr.Match();

    if(_result.empty()) {
      //std::cout << "no match found " << std::endl;
      continue;
    }
    
    _run    = e.id().run();
    _subrun = e.id().subRun();
    _event  = e.id().event();
    _matchid = 0;

    auto const& match = _result[_matchid];
    _tpcid    = match.tpc_id;
    _flashid  = match.flash_id;
    _score    = match.score;
    _qll_xmin = match.tpc_point.x;

    _tpc_xmin = 1.e4;
    for(auto const& pt : _mgr.QClusterArray()[_tpcid])
      
      if(pt.x < _tpc_xmin) _tpc_xmin = pt.x;
    
    auto const& flash = _mgr.FlashArray()[_flashid];

    _t0 = flash.time;

    if(_hypo_spec.size() != match.hypothesis.size()) {
      std::cout << "Hypothesis size mismatch!" << std::endl;
      throw std::exception();
    }

    for(size_t pmt=0; pmt<_hypo_spec.size(); ++pmt) _hypo_spec[pmt]  = match.hypothesis[pmt];
    for(size_t pmt=0; pmt<_hypo_spec.size(); ++pmt) _flash_spec[pmt] = flash.pe_v[pmt];

    _flash_pe = 0.;
    _hypo_pe  = 0.;
    for(auto const& v : _hypo_spec) _hypo_pe += v;
    for(auto const& v : _flash_spec) _flash_pe += v;

    _tree1->Fill();
    _tree2->Fill();
  }
  
  return;
}

DEFINE_ART_MODULE(T0TrackCalib)