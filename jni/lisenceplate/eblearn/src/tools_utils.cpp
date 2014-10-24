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

#include "tools_utils.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <errno.h>
#include "libidx.h"

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#endif

#ifdef __WINDOWS__
#include <windows.h>
#endif

using namespace std;

namespace ebl {
// directory utilities /////////////////////////////////////////////////////////

files_list *find_files(const std::string &dir, const char *,
                       files_list *fl_, bool, bool,
                       bool)
{
    files_list *fl = fl_;

    // allocate fl if null
    if (!fl)
        fl = new files_list();

#ifdef __WINDOWS__
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    std::string dir0 = dir;
    std::string str = "\\";
    std::size_t found = dir0.find(str);
    if (found != std::string::npos)
        dir0.replace(dir0.find(str), str.length(), "/");
    string path0 = dir0;
    dir0 += "*.*";
    hFind = FindFirstFile(dir0.c_str(), &FindFileData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return fl;
    }
    BOOL bRet = FindNextFile(hFind, &FindFileData);
    while (bRet)
    {
        bRet = FindNextFile(hFind, &FindFileData);
        string fn = FindFileData.cFileName;
        fl->push_back(pair<std::string, std::string>(path0, fn));
    }
#endif

    return fl;
}

std::list<std::string> *find_fullfiles(const std::string &dir, const char *pattern,
                                       std::list<std::string> *fl_, bool sorted, bool recursive,
                                       bool randomize, bool finddirs, bool fullpattern)
{
    std::list<std::string> *fl = fl_;

    eblerror("not implemented");

    (void)dir;
    (void)pattern;
    (void)sorted;
    (void)recursive;
    (void)randomize;
    (void)finddirs;
    (void)fullpattern;

    return fl;
}

std::string increment_filename(const char *fullname, const uint stride)
{
    std::vector<std::string> path_vector = string_to_stringvector(fullname, '/');
    std::string filename = path_vector[path_vector.size() - 1];
    //split filename into name and extension
    std::string filename_head = noext_name(filename.c_str());
    //since file can contain . apart from the extension, make sure of that case
    std::string filename_str = strip_last_num(filename_head);
    std::string filename_num = return_last_num(filename_head);
    intg filenum = string_to_intg(filename_num);

    filenum = filenum + stride;
    std::string outnum;
    std::stringstream tempstream;
    tempstream << filenum;
    tempstream >> outnum;
    if (outnum.size() < filename_num.size())
        for (uint i = 0; i < (1 + filename_num.size() - outnum.size()); ++i)
        {
            outnum = "0" + outnum;
        }

    //now add back the filename
    filename_head = filename_str + outnum;
    filename = filename_head + ext_name(filename.c_str());
    std::string ret = "";
    for (uint i = 0; i < (path_vector.size() - 1); ++i)
    {
        ret = ret + "/" + path_vector[i];
    }

    ret = ret + "/" + filename;
    return ret;
}
} // namespace ebl
