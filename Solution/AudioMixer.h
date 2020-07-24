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

	explicit CAudioMixer(double const sampleRate_, unsigned int const bufferLength_, int const numChannels_)
		: m_pBufferData(malloc(sizeof(float)* bufferLength_* numChannels_))
		, m_pMixBuffer(new SAudioBuffer(sampleRate_, bufferLength_, numChannels_, m_pBufferData))
	{}

	~CAudioMixer()
	{
		delete m_pMixBuffer;
		free(m_pBufferData);
	}

	sf_count_t MixFileNInN(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels);
	sf_count_t MixFile1InNPositional(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels, Vec2D const relativePosition, float const attenuationDistance = 0.0f);

	void*         m_pBufferData;
	SAudioBuffer* m_pMixBuffer;
};
} // Arkanoid::Audio
