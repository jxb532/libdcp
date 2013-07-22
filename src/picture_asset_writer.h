/*
    Copyright (C) 2012-2013 Carl Hetherington <cth@carlh.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <stdint.h>
#include <string>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "metadata.h"

namespace libdcp {

class MonoPictureAsset;	
class StereoPictureAsset;	

struct FrameInfo
{
	FrameInfo (uint64_t o, uint64_t s, std::string h)
		: offset (o)
		, size (s)
		, hash (h)
	{}

	FrameInfo (std::istream& s);

	void write (std::ostream& s);
	
	uint64_t offset;
	uint64_t size;
	std::string hash;
};

class PictureAssetWriter : public boost::noncopyable
{
public:
	virtual FrameInfo write (uint8_t *, int) = 0;
	virtual void fake_write (int) = 0;
	virtual void finalize () = 0;
	
protected:

	PictureAssetWriter (bool, MXFMetadata const &);
	virtual void start (uint8_t *, int) = 0;
	
	/** Number of picture frames written to the asset so far */
	int _frames_written;
	bool _started;
	/** true if finalize() has been called */
	bool _finalized;
	bool _overwrite;
	MXFMetadata _metadata;
};

/** A helper class for writing to MonoPictureAssets progressively (i.e. writing frame-by-frame,
 *  rather than giving libdcp all the frames in one go).
 *
 *  Objects of this class can only be created with MonoPictureAsset::start_write().
 *
 *  Frames can be written to the MonoPictureAsset by calling write() with a JPEG2000 image
 *  (a verbatim .j2 file).  finalize() must be called after the last frame has been written.
 *  The action of finalize() can't be done in MonoPictureAssetWriter's destructor as it may
 *  throw an exception.
 */
class MonoPictureAssetWriter : public PictureAssetWriter
{
public:
	FrameInfo write (uint8_t* data, int size);
	void fake_write (int size);
	void finalize ();

private:
	friend class MonoPictureAsset;

	MonoPictureAssetWriter (MonoPictureAsset *, bool, MXFMetadata const &);
	void start (uint8_t *, int);

	/* do this with an opaque pointer so we don't have to include
	   ASDCP headers
	*/
	   
	struct ASDCPState;
	boost::shared_ptr<ASDCPState> _state;

	MonoPictureAsset* _asset;
};

class StereoPictureAssetWriter : public PictureAssetWriter
{
public:
	FrameInfo write (uint8_t* data, int size);
	void fake_write (int size);
	void finalize ();

private:
	friend class StereoPictureAsset;

	StereoPictureAssetWriter (StereoPictureAsset *, bool, MXFMetadata const &);
	void start (uint8_t *, int);

	/* do this with an opaque pointer so we don't have to include
	   ASDCP headers
	*/
	   
	struct ASDCPState;
	boost::shared_ptr<ASDCPState> _state;

	StereoPictureAsset* _asset;
};

}
