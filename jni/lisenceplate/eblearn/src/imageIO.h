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

#ifndef IMAGEIO_H_
#define IMAGEIO_H_

#include <opencv2/opencv.hpp>

#include "libidx.h"

namespace ebl {
////////////////////////////////////////////////////////////////
// loading

//! Load the image in 'fname' into out.
//! First we check if the file is a matrix, if yes we load it and return it.
//! This throws string exceptions upon errors.
template<class T>
void load_image(const char *fname, idx<T> &out);

//! Load the image in 'fname' into out.
//! First we check if the file is a matrix, if yes we load it and return it.
//! This throws string exceptions upon errors.
template<class T>
void load_image(const std::string &fname, idx<T> &out);

//! Load the image in 'fname' and return it.
//! First we check if the file is a matrix, if yes we load it and return it.
//! This throws string exceptions upon errors.
template<class T>
idx<T> load_image(const char *fname, cv::Mat *im = NULL);

////////////////////////////////////////////////////////////////
// saving

// format is the desired image format. e.g.: "JPG", "PNG", etc.
bool save_image(const std::string &fname, idx<ubyte> &in,
                const char *format);

////////////////////////////////////////////////////////////////
// helper functions

//! read an image from a file into
//! an idx3 of ubytes (RGB or RGBA). <f> must
//! be a valid file descriptor (C pointer).
//! <out> is appropriately resized if required.
//! The last dimension is left unchanged but must be
//! at least 3. Appropriate conversion is performed.
//! extra color components (beyond the first 3) are left
//! untouched.
EXPORT idx<ubyte> image_read(const char *fname, idx<ubyte> *out = NULL, cv::Mat *im = NULL);

//  EXPORT idx<ubyte> pnm_read2(istream &in, idx<ubyte> *out = NULL);

//! read any kind of image
//! See above for description, as it is the same function used,
//! after a conversion to PPM
template<class T>
idx<T> image_read(const char *fname, idx<T> *out = NULL, cv::Mat *im = NULL);
} // end namespace ebl

#include "imageIO.hpp"

#endif /* IMAGEIO_H_ */
