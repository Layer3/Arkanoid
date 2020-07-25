#pragma once
#include "sndfile.h"
#include "RenderedObject.h"
#include <atomic>

namespace Arkanoid::Audio
{
	struct SPlayingVoice final
	{
		SPlayingVoice() = default;

		explicit SPlayingVoice(SNDFILE* pFile_, unsigned char const numFileChannels_, unsigned short const fileSampleRate_, bool const positioned_ = false, Vec2D const& position_ = Vec2D(0.0f, 0.0f))
			: pFile(pFile_)
			, numChannels(numFileChannels_)
			, sampleRate(fileSampleRate_)
			, positioned(positioned_)
			, position(position_.x, position_.y)
			, isValid(true)
		{}

		SPlayingVoice(SPlayingVoice& playingVoice)
			: pFile(playingVoice.pFile)
			, numChannels(playingVoice.numChannels)
			, sampleRate(playingVoice.sampleRate)
			, positioned(playingVoice.positioned)
			, position(playingVoice.position.x, playingVoice.position.y)
			, isValid(true)
		{}

		~SPlayingVoice()
		{
			sf_close(pFile);
		}

		SNDFILE*             pFile;
		unsigned char const  numChannels;
		unsigned short const sampleRate;
		bool                 positioned;
		bool                 isValid;
		AtomicVec2D          position;
	};
} // Arkanoid::Audio
