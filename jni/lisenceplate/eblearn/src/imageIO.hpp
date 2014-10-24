/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef IMAGEIO_HPP_
#define IMAGEIO_HPP_

#include <math.h>
#include <stdlib.h>

#ifndef __NOSTL__
#include <fstream>
#include <cstdlib>
#include <cstdio>
#endif

#ifndef __WINDOWS__
#include <unistd.h>
#endif

#ifdef __MAGICKPP__
#include <Magick++.h>
#endif

#include <errno.h>
#include "config.h"
#include "idxIO.h"
#include "stl.h"

namespace ebl {
////////////////////////////////////////////////////////////////
// I/O: helper functions

template<class T>
idx<T> image_read(const char *fname, idx<T> *out_, cv::Mat *im)
{
    idx<ubyte> tmp;

    // read pnm image
    tmp = image_read(fname, NULL, im);

    idxdim dims(tmp);
    idx<T> out;
    idx<T> *pout = &out;
    // allocate if not allocated
    if (!out_)
        out = idx<T>(dims);
    else
        pout = out_;
    // resize if necessary
    if (pout->get_idxdim() != dims)
        pout->resize(dims);
    // copy/cast
    idx_copy(tmp, *pout);
    return *pout;
}

////////////////////////////////////////////////////////////////
// I/O: loading

template<class T>
void load_image(const char *fname, idx<T> &out)
{
    // first try if the image is a mat file
    try {
        if (out.order() > 3 || out.order() < 2)
            eblerror("image has to be 2D or 3D");
        load_matrix<T>(out, fname);
        // channels are likely in dim 0 if size 1 or 3
        if (((out.dim(0) == 1) || (out.dim(0) == 3)) && (out.order() == 3))
            out = out.shift_dim(0, 2);
        return;
    } catch (eblexception &e) {
        //e = e;
        // not a mat file, try regular image
    }
    image_read(fname, &out);
}

template<class T>
void load_image(const std::string &fname, idx<T> &out)
{
    return load_image(fname.c_str(), out);
}

template<class T>
idx<T> load_image(const char *fname, cv::Mat *im)
{
    //// first try if the image is a mat file
    //try {
    //    idx<T> m = load_matrix<T>(fname);
    //    if (m.order() > 3 || m.order() < 2)
    //        eblerror("image has to be 2D or 3D");
    //    // channels are likely in dim 0 if size 1 or 3
    //    if (((m.dim(0) == 1) || (m.dim(0) == 3)) && (m.order() == 3))
    //        m = m.shift_dim(0, 2);
    //    return m;
    //} catch (eblexception &) {
    //    //e = e; // not a mat file, try regular image
    //}
    return image_read<T>(fname, NULL, im);
}

} // end namespace ebl

#endif /* IMAGE_HPP_ */
