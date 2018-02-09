#ifndef NUMUCCEVENTSELECTION_CXX
#define NUMUCCEVENTSELECTION_CXX

#include "NuMuCCEventSelection.h"
#include <iostream>

namespace ubana {

  NuMuCCEventSelection::NuMuCCEventSelection() {
    _configured = false;
  }

  void NuMuCCEventSelection::Configure(fhicl::ParameterSet const& pset)
  {
    _deltax_cut_down          = pset.get< double > ( "DeltaXCutDown" );
    _deltax_cut_up            = pset.get< double > ( "DeltaXCutUp" );
    _deltaz_cut_down          = pset.get< double > ( "DeltaZCutDown" );
    _deltaz_cut_up            = pset.get< double > ( "DeltaZCutUp" );
    _flsmatch_score_cut       = pset.get< double > ( "FlsMatchScoreCut" );
    _vtxcheck_angle_cut_down  = pset.get< double > ( "VtxCheckAngleCutDown" );
    _vtxcheck_angle_cut_up    = pset.get< double > ( "VtxCheckAngleCutUp" );
    _mcs_length_cut           = pset.get< double > ( "MCSLengthCut" );
    _ntrack_cut               = pset.get< double > ( "NTrackCut" );
    _pe_cut                   = pset.get< double > ( "PECut" );
    _beamSpillStarts          = pset.get< double > ( "BeamSpillStarts" );
    _beamSpillEnds            = pset.get< double > ( "BeamSpillEnds" );

    _verbose                  = pset.get< bool > ( "Verbose" );

    _configured = true;
  }

  void NuMuCCEventSelection::PrintConfig() {

    std::cout << "--- NuMuCCEventSelection configuration:" << std::endl;
    std::cout << "---   _deltax_cut_down          = " << _deltax_cut_down << std::endl;
    std::cout << "---   _deltax_cut_up            = " << _deltax_cut_up << std::endl;
    std::cout << "---   _deltaz_cut_down          = " << _deltaz_cut_down << std::endl;
    std::cout << "---   _deltaz_cut_up            = " << _deltaz_cut_up << std::endl;
    std::cout << "---   _flsmatch_score_cut       = " << _flsmatch_score_cut << std::endl;
    std::cout << "---   _vtxcheck_angle_cut_down  = " << _vtxcheck_angle_cut_down << std::endl;
    std::cout << "---   _vtxcheck_angle_cut_up    = " << _vtxcheck_angle_cut_up << std::endl;
    std::cout << "---   _mcs_length_cut           = " << _mcs_length_cut << std::endl;
    std::cout << "---   _ntrack_cut               = " << _ntrack_cut << std::endl;
    std::cout << "---   _pe_cut                   = " << _pe_cut << std::endl;
    std::cout << "---   _beamSpillStarts          = " << _beamSpillStarts << std::endl;
    std::cout << "---   _beamSpillEnds            = " << _beamSpillEnds << std::endl;
    std::cout << "---   _verbose                  = " << _verbose << std::endl;
  }

  void NuMuCCEventSelection::SetEvent(UBXSecEvent* ubxsec_event) {

    _ubxsec_event = ubxsec_event;
    _event_is_set = true;

  }

  bool NuMuCCEventSelection::IsSelected(size_t & slice_index, std::map<std::string,bool> & failure_map) {
   
    if (_verbose) std::cout << "[NuMuCCEventSelection] Starts. " << std::endl;

    if (!_configured || !_event_is_set) {
      std::cout << "Call to NuMuCCEventSelection::IsSelected() without having done configuration or having set the UBXSecEvent" << std::endl;
      throw std::exception();
    }

    slice_index = -1;
    std::string reason = "no_failure";

    // ************ 
    // Flashes
    // ************

    if (_ubxsec_event->nbeamfls == 0) {
      reason = "no_beam_disc_flashes";
      failure_map["beam_disc_flashes"] = false;

      failure_map["beam_spill_flash"] = false;
      failure_map["flash_match"] = false;
      failure_map["flash_match_score"] = false;
      failure_map["flash_match_deltax_up"] = false;
      failure_map["flash_match_deltax_down"] = false;
      failure_map["flash_match_deltaz_up"] = false;
      failure_map["flash_match_deltaz_down"] = false;
      failure_map["fiducial_volume"] = false;
      failure_map["vertex_check_up"] = false;
      failure_map["vertex_check_down"] = false;
      failure_map["vertex_quality"] = false;
      failure_map["mcs_length_quality"] = false;
      failure_map["mip_consistency"] = false;
      failure_map["ntrack"] = false;
      failure_map["track_quality"] = false;
      failure_map["vertex_quality"] = false;

      if (_verbose) std::cout << "[NuMuCCEventSelection] Selection FAILED. No beam disc flashes." << std::endl;
      return false;

    } else {
      failure_map["beam_disc_flashes"] = true;
    }

    int flashInBeamSpill = -1;    
    double old_pe = -1;

    for (int fls = 0; fls < _ubxsec_event->nbeamfls; fls ++){

      if (_ubxsec_event->beamfls_time.at(fls) > _beamSpillStarts 
          && _ubxsec_event->beamfls_time.at(fls) < _beamSpillEnds
          && _ubxsec_event->beamfls_pe.at(fls) >= _pe_cut) {
        
        if (_ubxsec_event->beamfls_pe.at(fls) > old_pe) {
          flashInBeamSpill = fls;
          old_pe = _ubxsec_event->beamfls_pe.at(fls);
        }    
      }
    }
    
    if (flashInBeamSpill == -1) {
      reason = "no_beam_spill_flash";
      failure_map["beam_spill_flash"] = false;

      failure_map["flash_match"] = false;
      failure_map["flash_match_score"] = false;
      failure_map["flash_match_deltax_up"] = false;
      failure_map["flash_match_deltax_down"] = false;
      failure_map["flash_match_deltaz_up"] = false;
      failure_map["flash_match_deltaz_down"] = false;
      failure_map["fiducial_volume"] = false;
      failure_map["vertex_check_up"] = false;
      failure_map["vertex_check_down"] = false;
      failure_map["vertex_quality"] = false;
      failure_map["mcs_length_quality"] = false;
      failure_map["mip_consistency"] = false;
      failure_map["ntrack"] = false;
      failure_map["track_quality"] = false;
      failure_map["vertex_quality"] = false;

      if (_verbose) std::cout << "[NuMuCCEventSelection] Selection FAILED. No beam spill flash object." << std::endl;
      return false;

    } else {
      failure_map["beam_spill_flash"] = true;
    }
    
    // *******************************
    // Find slice with maximum score
    // *******************************
    double score_max = -1;
    int scl_ll_max = -1;
    for (int slc = 0; slc < _ubxsec_event->nslices; slc ++){
      
      if(_ubxsec_event->slc_flsmatch_score.at(slc) > score_max){
        scl_ll_max = slc;
        score_max = _ubxsec_event->slc_flsmatch_score.at(slc);
      }

    }
    
    if (scl_ll_max == -1) {
      reason = "fail_flash_match";
      failure_map["flash_match"] = false;

      failure_map["flash_match_score"] = false;
      failure_map["flash_match_deltax_up"] = false;
      failure_map["flash_match_deltax_down"] = false;
      failure_map["flash_match_deltaz_up"] = false;
      failure_map["flash_match_deltaz_down"] = false;
      failure_map["fiducial_volume"] = false;
      failure_map["vertex_check_up"] = false;
      failure_map["vertex_check_down"] = false;
      failure_map["vertex_quality"] = false;
      failure_map["mcs_length_quality"] = false;
      failure_map["mip_consistency"] = false;
      failure_map["ntrack"] = false;
      failure_map["track_quality"] = false;
      failure_map["vertex_quality"] = false;

      if (_verbose) std::cout << "[NuMuCCEventSelection] Selection FAILED. No flash-matched object." << std::endl;
      return false;

    } else {
      failure_map["flash_match"] = true;
    }
    
    if (score_max <= _flsmatch_score_cut) {
      reason = "fail_flash_match_score";
      failure_map["flash_match_score"] = false;
    } else {
      failure_map["flash_match_score"] = true;
    }

    // Delta X
    if(_ubxsec_event->slc_flsmatch_qllx.at(scl_ll_max) - _ubxsec_event->slc_flsmatch_tpcx.at(scl_ll_max) > _deltax_cut_up) {
      reason = "fail_flash_match_deltax_up";
      failure_map["flash_match_deltax_up"] = false;
    } else {
      failure_map["flash_match_deltax_up"] = true;
    }
    if(_ubxsec_event->slc_flsmatch_qllx.at(scl_ll_max) - _ubxsec_event->slc_flsmatch_tpcx.at(scl_ll_max) < _deltax_cut_down) {
      reason = "fail_flash_match_deltax_down";
      failure_map["flash_match_deltax_down"] = false;
    } else {
      failure_map["flash_match_deltax_down"] = true;
    }

    // DeltaZ
    if(_ubxsec_event->slc_flsmatch_hypoz.at(scl_ll_max) - _ubxsec_event->beamfls_z.at(flashInBeamSpill) > _deltaz_cut_up) {
      reason = "fail_flash_match_deltaz_up";
      failure_map["flash_match_deltaz_up"] = false;
    } else {
      failure_map["flash_match_deltaz_up"] = true;
    }
    if(_ubxsec_event->slc_flsmatch_hypoz.at(scl_ll_max) - _ubxsec_event->beamfls_z.at(flashInBeamSpill) < _deltaz_cut_down) {
      reason = "fail_flash_match_deltaz_down";
      failure_map["flash_match_deltaz_down"] = false;
    } else {
      failure_map["flash_match_deltaz_down"] = true;
    }

    // FV
    if(_ubxsec_event->slc_nuvtx_fv.at(scl_ll_max) == 0) {
      reason = "fail_fiducial_volume";
      failure_map["fiducial_volume"] = false;
    } else {
      failure_map["fiducial_volume"] = true;
    }

    // Vertex Check   
    if(_ubxsec_event->slc_vtxcheck_angle.at(scl_ll_max) > _vtxcheck_angle_cut_up) {
      reason = "fail_vertex_check_up";
      failure_map["vertex_check_up"] = false;
    } else {
      failure_map["vertex_check_up"] = true;
    }
    if(_ubxsec_event->slc_vtxcheck_angle.at(scl_ll_max) < _vtxcheck_angle_cut_down 
       && _ubxsec_event->slc_vtxcheck_angle.at(scl_ll_max) !=-9999 ) {
      reason = "fail_vertex_check_down";
      failure_map["vertex_check_down"] = false;
    } else {
      failure_map["vertex_check_down"] = true;
    }

    // N Track
    if(_ubxsec_event->slc_ntrack.at(scl_ll_max) < _ntrack_cut) {
      reason = "fail_ntrack";
      failure_map["ntrack"] = false;
    } else {
      failure_map["ntrack"] = true;
    }

    // Track Quality
    if(!_ubxsec_event->slc_passed_min_track_quality.at(scl_ll_max)) {
      reason = "fail_track quality";
      failure_map["track_quality"] = false;
    } else {
      failure_map["track_quality"] = true;
    }
    
    // Vertex Quality
    if(!_ubxsec_event->slc_passed_min_vertex_quality.at(scl_ll_max)) {
      reason = "fail_vertex_quality";
      failure_map["vertex_quality"] = false;
    } else {
      failure_map["vertex_quality"] = true;
    }

    // MCS-Length Quality Cut
    if(_ubxsec_event->slc_muoncandidate_contained.at(scl_ll_max) 
      && (_ubxsec_event->slc_muoncandidate_mom_mcs.at(scl_ll_max) 
        - _ubxsec_event->slc_muoncandidate_mom_range.at(scl_ll_max) > _mcs_length_cut)) {
      reason = "fail_mcs_length_quality";
      failure_map["mcs_length_quality"] = false;
    } else {
      failure_map["mcs_length_quality"] = true;
    }

    // MIP Consistency Cut
    if(!_ubxsec_event->slc_muoncandidate_mip_consistency.at(scl_ll_max)) {
      reason = "fail_mip_consistency";
      failure_map["mip_consistency"] = false;
    } else {
      failure_map["mip_consistency"] = true;
    }

    slice_index = scl_ll_max;

    for (auto iter : failure_map) {
      if (!iter.second) {
        if (_verbose) std::cout << "[NuMuCCEventSelection] Selection FAILED. Did not pass " << iter.first << " cut." << std::endl;
        return false;
      }
    }

    if (_verbose) std::cout << "[NuMuCCEventSelection] Selection PASSED. All cuts are satisfied." << std::endl;

    return true;

  }
}


#endif
