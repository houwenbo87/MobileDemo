/***************************************************************************
 *   Copyright (C) 2012 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef DETECTION_THREAD_HPP_
#define DETECTION_THREAD_HPP_

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>

#include "libeblearn.h"
#include "libeblearntools.h"

namespace ebl {
// detection thread ////////////////////////////////////////////////////////////

template<typename T>
detection_thread<T>::
detection_thread(configuration &conf_, t_chans tc)
    :conf(conf_), frame(120, 160, 1), bfed(false),
    frame_name(""), frame_id(0), outdir(""), total_saved(0), color_space(tc),
    silent(false), frame_loaded(false), pdetect(NULL)
{
    theparam = NULL;
    ans = NULL;
    net = NULL;
    silent = conf.exists_true("silent");
    outdir = get_output_directory(conf);
    std::cout << "Saving outputs to " << outdir << std::endl;
}

template<typename T>
detection_thread<T>::~detection_thread()
{
}

template<typename T>
void detection_thread<T>::initDetectionThread()
{
    // load network and weights in a forward-only parameter
    theparam = new parameter<T>;
    theparam->set_forward_only();

    idx<ubyte> classes(1, 1);
    load_matrix<ubyte>(classes, conf.get_cstring("classes"));
    std::vector<std::string> sclasses = ubyteidx_to_stringvector(classes);

    ans = create_answer<T, T, T>(conf, classes.dim(0));

    uint noutputs = ans->get_nfeatures();
    intg thick = -1;
    net = create_network<T>(*theparam, conf, thick, noutputs, "arch");

    if (conf.exists("weights"))
    {
        // concatenate weights if multiple ones
        std::vector<std::string> w =
            string_to_stringvector(conf.get_string("weights"));
        theparam->load_x(w);
        // permute weights by blocks
        if (conf.exists("weights_permutation"))
        {
            std::string sblocks = conf.get_string("weights_blocks");
            std::string spermut = conf.get_string("weights_permutation");
            std::vector<intg> blocks = string_to_intgvector(sblocks.c_str());
            std::vector<uint> permut = string_to_uintvector(spermut.c_str());
            theparam->permute_x(blocks, permut);
        }
    }

    DEBUGMEM_PRETTY("before detection");

    // detector
    detector<T> *detect = new detector<T>(*net, sclasses, ans, NULL, NULL);// , mout, merr);
    init_detector(detect, conf, outdir, silent);
    // keep pointer to detector
    pdetect = detect;
}

template<typename T>
void detection_thread<T>::destroyDetectionThread()
{
    // free variables
    if (theparam)
        delete theparam;
    if (net)
        delete net;
    if (ans)
        delete ans;
}

template<typename T>
void detection_thread<T>::execute(cv::Mat& im)
{
    // timing variables
    timer toverall;
    toverall.start();

    if (!frame_loaded)
    {
        uframe = load_image<ubyte>("", &im);
        if (!silent)
            std::cout << "loaded image " << frame_fullname << std::endl;
    }
    if (!silent)
        std::cout << "processing " << frame_name << std::endl;
    // check frame is correctly allocated, if not, allocate.
    if (frame.order() != uframe.order())
        frame = idx<T>(uframe.get_idxdim());
    else if (frame.get_idxdim() != uframe.get_idxdim())
        frame.resize(uframe.get_idxdim());
    // copy frame
    idx_copy(uframe, frame);

    // run detector
    if (!silent)
        std::cout << "starting processing of frame " << frame_name << std::endl;

    bboxes &bb = pdetect->fprop(frame, frame_name.c_str(), frame_id);
    bbs = bb;; // make a copy of bounding boxes

    if (!silent)
        std::cout << "processing done for frame " << frame_name << std::endl;

    total_saved = pdetect->get_total_saved();
    if (!silent)
    {
        std::cout << bbs.pretty_short(pdetect->get_labels());
    }
    DEBUGMEM_PRETTY("after detection");

    if (!silent)
        std::cout << "detection finished. Execution time: " << toverall.elapsed() << std::endl;
}

// thread communication ////////////////////////////////////////////////////////

template<typename T>
void detection_thread<T>::init_detector(detector<T> *detect,
                                        configuration &conf,
                                        std::string &odir, bool silent)
{
    // multi-scaling parameters
    double maxs = conf.try_get_double("max_scale", 2.0);
    double mins = conf.try_get_double("min_scale", 1.0);
    t_scaling scaling_type =
        (t_scaling)conf.try_get_uint("scaling_type", SCALES_STEP);
    double scaling = conf.try_get_double("scaling", 1.4);
    std::vector<midxdim> scales;

    switch (scaling_type)
    {
    case MANUAL:
        if (!conf.exists("scales"))
            eblerror("expected \"scales\" variable to be defined in manual mode");
        scales = string_to_midxdimvector(conf.get_cstring("scales"));
        detect->set_resolutions(scales);
        break;
    case ORIGINAL:
        detect->set_scaling_original();
        break;
    case SCALES_STEP:
        detect->set_resolutions(scaling, maxs, mins);
        break;
    case SCALES_STEP_UP:
        detect->set_resolutions(scaling, maxs, mins);
        detect->set_scaling_type(scaling_type);
        break;
    default:
        detect->set_scaling_type(scaling_type);
    }

    // remove pads from target scales if requested
    if (conf.exists_true("scaling_remove_pad"))
        detect->set_scaling_rpad(true);
    // optimize memory usage by using only 2 buffers for entire flow
    state<T> input(1, 1, 1), output(1, 1, 1);
    if (!conf.exists_false("mem_optimization"))
        detect->set_mem_optimization(input, output, true);
    // TODO: always keep inputs, otherwise detection doesnt work. fix this.
    //                             conf.exists_true("save_detections") ||
    //                             (display && !mindisplay));
    // zero padding
    float hzpad = conf.try_get_float("hzpad", 0);
    float wzpad = conf.try_get_float("wzpad", 0);
    detect->set_zpads(hzpad, wzpad);
    if (conf.exists("input_min")) // limit inputs size
        detect->set_min_resolution(conf.get_uint("input_min"));
    if (conf.exists("input_max")) // limit inputs size
        detect->set_max_resolution(conf.get_uint("input_max"));
    if (silent)
        detect->set_silent();
    if (conf.exists_bool("save_detections"))
    {
        std::string detdir = odir;
        detdir += "detections";
        uint nsave = conf.try_get_uint("save_max_per_frame", 0);
        bool diverse = conf.exists_true("save_diverse");
        detdir = detect->set_save(detdir, nsave, diverse);
    }
    detect->set_scaler_mode(conf.exists_true("scaler_mode"));
    if (conf.exists("bbox_decision"))
        detect->set_bbox_decision(conf.get_uint("bbox_decision"));
    if (conf.exists("bbox_scalings"))
    {
        mfidxdim scalings =
            string_to_fidxdimvector(conf.get_cstring("bbox_scalings"));
        detect->set_bbox_scalings(scalings);
    }

    // nms configuration //////////////////////////////////////////////////////
    t_nms nms_type = (t_nms)conf.try_get_uint("nms", 0);
    float pre_threshold = conf.try_get_float("pre_threshold", 0.0);
    float post_threshold = conf.try_get_float("post_threshold", 0.0);
    float pre_hfact = conf.try_get_float("pre_hfact", 1.0);
    float pre_wfact = conf.try_get_float("pre_wfact", 1.0);
    float post_hfact = conf.try_get_float("post_hfact", 1.0);
    float post_wfact = conf.try_get_float("post_wfact", 1.0);
    float woverh = conf.try_get_float("woverh", 1.0);
    float max_overlap = conf.try_get_float("max_overlap", 0.0);
    float max_hcenter_dist = conf.try_get_float("max_hcenter_dist", 0.0);
    float max_wcenter_dist = conf.try_get_float("max_wcenter_dist", 0.0);
    float vote_max_overlap = conf.try_get_float("vote_max_overlap", 0.0);
    float vote_mhd = conf.try_get_float("vote_max_hcenter_dist", 0.0);
    float vote_mwd = conf.try_get_float("vote_max_wcenter_dist", 0.0);
    detect->set_nms(nms_type, pre_threshold, post_threshold, pre_hfact,
                   pre_wfact, post_hfact, post_wfact, woverh, max_overlap,
                   max_hcenter_dist, max_wcenter_dist, vote_max_overlap,
                   vote_mhd, vote_mwd);
    if (conf.exists("raw_thresholds"))
    {
        std::string srt = conf.get_string("raw_thresholds");
        std::vector<float> rt = string_to_floatvector(srt.c_str());
        detect->set_raw_thresholds(rt);
    }
    if (conf.exists("outputs_threshold"))
        detect->set_outputs_threshold((T)conf.get_double("outputs_threshold"), (T)conf.try_get_double("outputs_threshold_val", -1));
    ///////////////////////////////////////////////////////////////////////////
    if (conf.exists("netdims"))
    {
        idxdim d = string_to_idxdim(conf.get_string("netdims"));
        detect->set_netdim(d);
    }
    if (conf.exists("smoothing"))
    {
        idxdim ker;
        if (conf.exists("smoothing_kernel"))
            ker = string_to_idxdim(conf.get_string("smoothing_kernel"));
        detect->set_smoothing(conf.get_uint("smoothing"),
                             conf.try_get_double("smoothing_sigma", 1),
                             &ker,
                             conf.try_get_double("smoothing_sigma_scale", 1));
    }
    if (conf.exists("background_name"))
        detect->set_bgclass(conf.get_cstring("background_name"));
    if (conf.exists_true("bbox_ignore_outsiders"))
        detect->set_ignore_outsiders();
    if (conf.exists("corners_inference"))
        detect->set_corners_inference(conf.get_uint("corners_inference"));
    if (conf.exists("input_gain"))
        detect->set_input_gain(conf.get_double("input_gain"));
    if (conf.exists_true("dump_outputs"))
    {
        std::string fname;
        fname << odir << "/dump/detect_out";
        detect->set_outputs_dumping(fname.c_str());
    }
}

template<typename T>
bool detection_thread<T>::get_data(bboxes &bboxes2, uint &total_saved_, 
                                   idx<ubyte> *frame2, std::string *frame_name_,
                                   uint *id, svector<midx<T> > *samples, bboxes *bbsamples)
{
    // clear bboxes
    bboxes2.clear();
    bboxes2.push_back_new(bbs);
    bbs.clear();

    // set total of boxes saved
    total_saved_ = total_saved;

    if (frame2)
    {
        // check frame is correctly allocated, if not, allocate.
        if (frame2->order() != uframe.order())
            frame2 = new idx<ubyte>(uframe.get_idxdim());
        else if (frame2->get_idxdim() != uframe.get_idxdim())
            frame2->resize(uframe.get_idxdim());
        // copy frame
        idx_copy(uframe, *frame2);
    }
    // set frame name
    if (frame_name_)
    {
        *frame_name_ = frame_name;
    }
    // set frame id
    if (id)
    {
        *id = frame_id;
    }
    // overwrite samples
    if (samples)
    {
        samples->clear();
        samples->push_back_new(returned_samples);
        returned_samples.clear();
    }
    if (bbsamples)
    {
        bbsamples->clear();
        bbsamples->push_back_new(returned_samples_bboxes);
        returned_samples_bboxes.clear();
    }

    // confirm that we copied data.
    return true;
}

template<typename T>
bool detection_thread<T>::set_data(idx<ubyte> &frame2,
                                   std::string &fullname,
                                   std::string &name, uint id)
{
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != uframe.order())
        uframe = idx<ubyte>(frame2.get_idxdim());
    else if (frame2.get_idxdim() != uframe.get_idxdim())
        uframe.resize(frame2.get_idxdim());
    idx_copy(frame2, uframe); // copy frame
    frame_loaded = true;      // frame is loaded
    frame_fullname = fullname;
    frame_name = name;  // copy name
    frame_id   = id;  // copy frame_id
    bfed       = true;  // data has been fed at least once

    return true;  // confirm that we copied data.
}

template<typename T>
bool detection_thread<T>::set_data(std::string &fullname,
                                   std::string &name, uint id)
{
    // load image
    frame_fullname = fullname;
    frame_name = name;        // copy name
    frame_id = id;            // copy frame_id
    frame_loaded = false;     // let it be loaded later
    bfed       = true;  // data has been fed at least once

    return true;
}

template<typename T>
bool detection_thread<T>::fed()
{
    return bfed;
}

template<typename T>
std::string detection_thread<T>::
get_output_directory(configuration &conf)
{
    std::string s;

    if (conf.exists("output_dir")) s << conf.get_string("output_dir");
    else s << conf.get_string("current_dir");
    if (conf.exists("detections_dir"))
        s << conf.get_string("detections_dir");
    else
    {
        s << "/detections";
        if (conf.exists_true("nms")) s << "_" << tstamp();
    }
    s << "/";
    mkdir_full(s);
    return s;
}

// protected methods /////////////////////////////////////////////////////////

template<typename T>
void detection_thread<T>::clear_bboxes()
{
    // clear bboxes
    bbs.clear();
}

template<typename T>
void detection_thread<T>::copy_bootstrapping(svector<midx<T> > &samples,
                                             bboxes &           bb)
{
    returned_samples = samples;
    returned_samples_bboxes = bb;
    std::cout << "samples: " << samples << std::endl;
    std::cout << "boxes: " << bb << std::endl;
}

} // end namespace ebl

#endif /* DETECTION_THREAD_HPP_ */
