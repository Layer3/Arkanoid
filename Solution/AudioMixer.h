#pragma once
#include "AudioBuffer.h"
#include "Constexpr.h"
#include <sndfile.h>

namespace Arkanoid::Audio
{
class CAudioMixer
{
public:
	CAudioMixer() = delete;

	explicit CAudioMixer(SAudioBuffer* pMixBuffer)
		: m_pMixBuffer(pMixBuffer)
	{}

	~CAudioMixer()
	{
		delete m_pMixBuffer;
	}

	sf_count_t MixFileNInN(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels);
	sf_count_t MixFile1InNPositional(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels, Vec2D const relativePosition, float const attenuationDistance = 0.0f);

	SAudioBuffer* m_pMixBuffer;
};
} // Arkanoid::Audio
