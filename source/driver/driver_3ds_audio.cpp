#include <string>
#include <opusfile.h>

#include "driver/driver_3ds_audio.h"

namespace SuperHaxagon {
	Driver3DSAudio::Driver3DSAudio(std::string path) {
		OggOpusFile *opusFile = op_open_file(path.c_str(), &error);
	}

	Driver3DSAudio::~Driver3DSAudio() {

	}

	bool Driver3DSAudio::finished() {
		return false;
	}

	// Retrieve strings for libopusfile errors
	// Sourced from David Gow's example code: https://davidgow.net/files/opusal.cpp
	const char *Driver3DSAudio::opusStrError(int error)
	{
		switch(error) {
			case OP_FALSE:
				return "OP_FALSE: A request did not succeed.";
			case OP_HOLE:
				return "OP_HOLE: There was a hole in the page sequence numbers.";
			case OP_EREAD:
				return "OP_EREAD: An underlying read, seek or tell operation failed.";
			case OP_EFAULT:
				return "OP_EFAULT: A NULL pointer was passed where none was "
				       "expected, or an internal library error was encountered.";
			case OP_EIMPL:
				return "OP_EIMPL: The stream used a feature which is not implemented.";
			case OP_EINVAL:
				return "OP_EINVAL: One or more parameters to a function were invalid.";
			case OP_ENOTFORMAT:
				return "OP_ENOTFORMAT: This is not a valid Ogg Opus stream.";
			case OP_EBADHEADER:
				return "OP_EBADHEADER: A required header packet was not properly formatted.";
			case OP_EVERSION:
				return "OP_EVERSION: The ID header contained an unrecognised version number.";
			case OP_EBADPACKET:
				return "OP_EBADPACKET: An audio packet failed to decode properly.";
			case OP_EBADLINK:
				return "OP_EBADLINK: We failed to find data we had seen before or "
				       "the stream was sufficiently corrupt that seeking is impossible.";
			case OP_ENOSEEK:
				return "OP_ENOSEEK: An operation that requires seeking was "
				       "requested on an unseekable stream.";
			case OP_EBADTIMESTAMP:
				return "OP_EBADTIMESTAMP: The first or last granule position of a "
				       "link failed basic validity checks.";
			default:
				return "Unknown error.";
		}
	}
}

