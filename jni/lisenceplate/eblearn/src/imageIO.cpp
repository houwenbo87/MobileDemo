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

// tell header that we are in the libidx scope
#define LIBIDX

#include <stdio.h>

#ifndef __WINDOWS__
#include <inttypes.h>
#endif

#include "imageIO.h"
#include "idxops.h"
#include "idx.h"
#include "stl.h"
#include "utils.h"

#include <opencv2/opencv.hpp>

namespace ebl {

idx<ubyte> image_read(const char *fname, idx<ubyte> *out_, cv::Mat *im)
{
    idx<ubyte> out;
    idx<ubyte> *pout = &out;

    idxdim dims(im->rows, im->cols, im->channels());
    int vmax = 255;

    if (!out_) // allocate buffer if out is empty
        out = idx<ubyte>(dims);
    else
        pout = out_;
    idx_checkorder1(*pout, 3); // allow only 3D buffers

    // resize out if necessary
    if (pout->get_idxdim() != dims)
        pout->resize(dims);

    // sizes
    size_t sz = (vmax == 65535) ? 2 : 1;

    if (pout->contiguousp())
    {
        if (sz == 2) // 16 bits per pixel
        {
        }
        else // 8 bits per pixel
        {
            memcpy((char *)pout->idx_ptr(), im->data, sz * dims.nelements());
        }
    }
    else
    {
        eblerror("pout not contiguous");
    }

    return *pout;
}

bool save_image(const std::string &fname, idx<ubyte> &in, const char *format)
{
    if (!strcmp(format, "mat"))   // save as idx
    {
        return save_matrix(in, fname);
    }
    idxdim dims = in.get_idxdim();
    cv::Mat im(dims.dim(0), dims.dim(1), dims.dim(2) == 1 ? CV_8UC1 : CV_8UC3);
    memcpy(im.data, (char *)in.idx_ptr(), dims.nelements());
    cv::imwrite(fname, im);

    return true;
}

} // end namespace ebl
