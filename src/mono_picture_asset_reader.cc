/*
    Copyright (C) 2016 Carl Hetherington <cth@carlh.net>

    This file is part of libdcp.

    libdcp is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    libdcp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdcp.  If not, see <http://www.gnu.org/licenses/>.

    In addition, as a special exception, the copyright holders give
    permission to link the code of portions of this program with the
    OpenSSL library under certain conditions as described in each
    individual source file, and distribute linked combinations
    including the two.

    You must obey the GNU General Public License in all respects
    for all of the code used other than OpenSSL.  If you modify
    file(s) with this exception, you may extend this exception to your
    version of the file(s), but you are not obligated to do so.  If you
    do not wish to do so, delete this exception statement from your
    version.  If you delete this exception statement from all source
    files in the program, then also delete it here.
*/

#include "mono_picture_asset_reader.h"
#include "mono_picture_asset.h"
#include "mono_picture_frame.h"
#include "exceptions.h"
#include "dcp_assert.h"
#include <asdcp/AS_DCP.h>

using namespace dcp;
using boost::shared_ptr;

MonoPictureAssetReader::MonoPictureAssetReader (MonoPictureAsset const * asset)
	: AssetReader (asset)
{
	_reader = new ASDCP::JP2K::MXFReader ();
	DCP_ASSERT (asset->file ());
	Kumu::Result_t const r = _reader->OpenRead (asset->file()->string().c_str());
	if (ASDCP_FAILURE (r)) {
		delete _reader;
		boost::throw_exception (FileError ("could not open MXF file for reading", asset->file().get(), r));
	}
}

MonoPictureAssetReader::~MonoPictureAssetReader ()
{
	delete _reader;
}

shared_ptr<const MonoPictureFrame>
MonoPictureAssetReader::get_frame (int n) const
{
	return shared_ptr<const MonoPictureFrame> (new MonoPictureFrame (_reader, n, _decryption_context));
}
