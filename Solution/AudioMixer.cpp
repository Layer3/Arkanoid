#include "AudioMixer.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <xutility>

namespace Arkanoid::Audio
{
//////////////////////////////////////////////////////////////////////////////////
sf_count_t CAudioMixer::MixFileNInN(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels, float const volume, bool const loop, bool const fade, bool const fadeIn)
{
	std::fill(static_cast<float*>(m_pMixBuffer->pData), static_cast<float*>(m_pMixBuffer->pData) + (m_pMixBuffer->bufferLength * m_pMixBuffer->numChannels), 0.0f);

	int const numBufferChannels = pOutBuffer->numChannels;
	int const channelsToWrite =
		(pOutBuffer->numChannels < numFileChannels)
		? pOutBuffer->numChannels
		: numFileChannels;

	auto const requestedSamples = static_cast<sf_count_t>(pOutBuffer->bufferLength);
	auto pStreamBuffer = static_cast<float*>(pOutBuffer->pData);
	auto const pMixBuffer = static_cast<float*>(m_pMixBuffer->pData);

	sf_count_t numFramesRead = 0;
	numFramesRead = sf_readf_float(pFile, pMixBuffer, requestedSamples);

	if(loop)
	{
		while (numFramesRead != requestedSamples)
		{
			sf_seek(pFile, 0, SF_SEEK_SET);
			numFramesRead += sf_readf_float(pFile, (pMixBuffer + numFramesRead), (requestedSamples - numFramesRead));
		}
	}

	if (fade)
	{
		float const fadeFactor = 1.0f / static_cast<float>(requestedSamples);

		if (fadeIn)
		{
			for (sf_count_t i = 0; i < numFramesRead; ++i)
			{
				for (int j = 0; j < channelsToWrite; ++j)
				{
					*pStreamBuffer++ += (pMixBuffer[i * channelsToWrite + j] * fadeFactor * i) * volume;
				}

				pStreamBuffer += (numBufferChannels - channelsToWrite);
			}
		}
		else
		{
			for (sf_count_t i = 0; i < numFramesRead; ++i)
			{
				for (int j = 0; j < channelsToWrite; ++j)
				{
					*pStreamBuffer++ += (pMixBuffer[i * channelsToWrite + j] * fadeFactor * (requestedSamples - i)) * volume;
				}

				pStreamBuffer += (numBufferChannels - channelsToWrite);
			}
		}
	}
	else
	{
		for (sf_count_t i = 0; i < numFramesRead; ++i)
		{
			for (int j = 0; j < channelsToWrite; ++j)
			{
				*pStreamBuffer++ += pMixBuffer[i * channelsToWrite + j] * volume;
			}

			pStreamBuffer += (numBufferChannels - channelsToWrite);
		}
	}

	return numFramesRead;
}

//////////////////////////////////////////////////////////////////////////////////
sf_count_t CAudioMixer::MixFile1InNPositional(SAudioBuffer const* const pOutBuffer, SNDFILE* const pFile, int const numFileChannels, Vec2D const relativeRotation, float const distanceFactor)
{
	std::fill(static_cast<float*>(m_pMixBuffer->pData), static_cast<float*>(m_pMixBuffer->pData) + ((m_pMixBuffer->bufferLength) * m_pMixBuffer->numChannels), 0.0f);

	sf_count_t numFramesRead = 0;

	// We expect at least 2 channels to mix positional audio
	if (pOutBuffer->numChannels == 1)
	{
		return numFramesRead;
	}

	// TODO: there should be a global for this.
	float volumeAttenuation = 1.0f;
	if (distanceFactor != 0.0f)
	{
		volumeAttenuation = 1.0f - (distanceFactor * 0.6f);
		
		if (volumeAttenuation < 0.0f) volumeAttenuation = 0.0f;
		if (volumeAttenuation > 1.0f) volumeAttenuation = 1.0f;
	}

	float const listenerCosine =
		relativeRotation.y
		/ (sqrtf(relativeRotation.x * relativeRotation.x + relativeRotation.y * relativeRotation.y));

	// Differentiate between Stereo(up to 2.1) and surround setups
	if (pOutBuffer->numChannels <= 3)
	{
		float volumeRight = 1.0f;
		float volumeLeft = 1.0f;

		if (relativeRotation.x < 0.0f)
		{
			float directionFactor = fabsf(listenerCosine);
			directionFactor = (powf(g_euler, (2.0f * (directionFactor - 1.0f))) * directionFactor);
			volumeRight = 1.0f - (directionFactor * 0.3f);
			volumeLeft = 0.1f + (directionFactor * 0.6f);
		}
		else
		{
			float directionFactor = fabsf(listenerCosine);
			directionFactor = (powf(g_euler, (2.0f * (directionFactor - 1.0f))) * directionFactor);
			volumeLeft = 1.0f - (directionFactor * 0.3f);
			volumeRight = 0.1f + (directionFactor * 0.6f);
		}

		int const numBufferChannels = pOutBuffer->numChannels;
		auto requestedSamples = static_cast<sf_count_t>(pOutBuffer->bufferLength);
		auto pStreamBuffer = static_cast<float*>(pOutBuffer->pData);
		auto const pMixBuffer = static_cast<float*>(m_pMixBuffer->pData);

		numFramesRead = sf_readf_float(pFile, pMixBuffer, requestedSamples);

		for (sf_count_t i = 0; i < numFramesRead; ++i)
		{
			*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeLeft * volumeAttenuation;
			*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeRight * volumeAttenuation;

			pStreamBuffer += (numBufferChannels - 2);
		}
	}
	else
	{
		// panning over 3 Speakers with 1 primary and 2 secondary channels
		// Assuming the most common setups for now without center: Quad and 6 Speaker surround
		float listenerAngle = (acosf(listenerCosine) * (180.0f / static_cast<float>(M_PI)));

		listenerAngle =
			(relativeRotation.x > 0.0f)
			? listenerAngle
			: 360.0f - listenerAngle;

		int const numBufferChannels = pOutBuffer->numChannels;
		int const channelsToWrite = (numBufferChannels > 7) ? 6 : 4;

		float quadrantProxy = 0.0f;
		float quadrantFine = listenerAngle / (360.0f / channelsToWrite);
		quadrantFine = modff(quadrantFine, &quadrantProxy);
		int const quadrant = static_cast<int>(quadrantProxy);
		float const quadrantFinePrimary = (quadrantFine < 0.5f) ? quadrantFine * 2.0f : 1.0f - (quadrantFine - 0.5f) * 1.0f;

		float const volumeSecondaryLeft = 0.7f * quadrantFine;
		float const volumePrimary = 0.7f + (0.25f * quadrantFinePrimary);
		float const volumeSecondaryRight = 0.7f * (1.0f - quadrantFine);

		float volumeFrontLeft = 0.0f;
		float volumeFrontRight = 0.0f;
		float volumeRearLeft = 0.0f;
		float volumeRearRight = 0.0f;
		float volumeSideLeft = 0.0f;
		float volumeSideRight = 0.0f;

		switch (quadrant)
		{
		case 0:
			{
				if (channelsToWrite == 4)
				{
					volumeFrontLeft = volumePrimary;
					volumeFrontRight = volumeSecondaryRight;
					volumeRearLeft = volumeSecondaryLeft;
					volumeRearRight = 0.0f;
				}
				else
				{
					volumeFrontLeft = volumePrimary;
					volumeFrontRight = volumeSecondaryRight;
					volumeRearLeft = 0.0f;
					volumeRearRight = 0.0f;
					volumeSideLeft = volumeSecondaryLeft;
					volumeSideRight = 0.0f;
				}

				break;
			}
		case 1:
			{
				if (channelsToWrite == 4)
				{
					volumeFrontLeft = volumeSecondaryRight;
					volumeFrontRight = 0.0f;
					volumeRearLeft = volumePrimary;
					volumeRearRight = volumeSecondaryLeft;
				}
				else
				{
					volumeFrontLeft = volumeSecondaryRight;
					volumeFrontRight = 0.0f;
					volumeRearLeft = volumeSecondaryLeft;
					volumeRearRight = 0.0f;
					volumeSideLeft = volumePrimary;
					volumeSideRight = 0.0f;
				}

				break;
			}
		case 2:
			{
				if (channelsToWrite == 4)
				{
					volumeFrontLeft = 0.0f;
					volumeFrontRight = volumeSecondaryLeft;
					volumeRearLeft = volumeSecondaryRight;
					volumeRearRight = volumePrimary;
				}
				else
				{
					volumeFrontLeft = 0.0f;
					volumeFrontRight = 0.0f;
					volumeRearLeft = volumePrimary;
					volumeRearRight = volumeSecondaryLeft;
					volumeSideLeft = volumeSecondaryRight;
					volumeSideRight = 0.0f;
				}

				break;
			}
		case 3:
			{
				if (channelsToWrite == 4)
				{
					volumeFrontLeft = volumeSecondaryLeft;
					volumeFrontRight = volumePrimary;
					volumeRearLeft = 0.0f;
					volumeRearRight = volumeSecondaryRight;
				}
				else
				{
					volumeFrontLeft = 0.0f;
					volumeFrontRight = 0.0f;
					volumeRearLeft = volumeSecondaryRight;
					volumeRearRight = volumePrimary;
					volumeSideLeft = 0.0f;
					volumeSideRight = volumeSecondaryLeft;
				}

				break;
			}
		case 4:
			{
				volumeFrontLeft = 0.0f;
				volumeFrontRight = volumeSecondaryLeft;
				volumeRearLeft = 0.0f;
				volumeRearRight = volumeSecondaryRight;
				volumeSideLeft = 0.0f;
				volumeSideRight = volumePrimary;

				break;
			}
		case 5:
			{
				volumeFrontLeft = volumeSecondaryLeft;
				volumeFrontRight = volumePrimary;
				volumeRearLeft = 0.0f;
				volumeRearRight = 0.0f;
				volumeSideLeft = 0.0f;
				volumeSideRight = volumeSecondaryRight;

				break;
			}
		default:
			{
				volumeFrontLeft = 0.0f;
				volumeFrontRight = 0.0f;
				volumeRearLeft = 0.0f;
				volumeRearRight = 0.0f;
				volumeSideLeft = 0.0f;
				volumeSideRight = 0.0f;

				break;
			}
		}

		auto const requestedSamples = static_cast<sf_count_t>(pOutBuffer->bufferLength);
		auto pStreamBuffer = static_cast<float*>(pOutBuffer->pData);
		auto const pMixBuffer = static_cast<float*>(m_pMixBuffer->pData);

		numFramesRead = sf_readf_float(pFile, pMixBuffer, requestedSamples);

		if (channelsToWrite == 4)
		{
			for (sf_count_t i = 0; i < numFramesRead; ++i)
			{
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeFrontLeft * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeFrontRight * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeRearLeft * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeRearRight * volumeAttenuation;

				pStreamBuffer += (numBufferChannels - channelsToWrite);
			}
		}
		else
		{
			for (sf_count_t i = 0; i < numFramesRead; ++i)
			{
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeFrontLeft * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeFrontRight * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeRearLeft * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeRearRight * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeSideLeft * volumeAttenuation;
				*pStreamBuffer++ += pMixBuffer[i * numFileChannels] * volumeSideRight * volumeAttenuation;

				pStreamBuffer += (numBufferChannels - channelsToWrite);
			}
		}
	}

	return numFramesRead;
}
} // Arkanoid::Audio
