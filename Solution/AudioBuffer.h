#pragma once
#include "Constexpr.h"
#include <corecrt_malloc.h>
#include <sndfile.h>

namespace Arkanoid::Audio
{
struct SAudioBuffer
{
	SAudioBuffer() = delete;

	explicit SAudioBuffer(double const sampleRate_, unsigned int const bufferLength_, int const numChannels_, void* pData_)
		: sampleRate(sampleRate_)
		, bufferLength(bufferLength_)
		, numChannels(numChannels_)
		, pData(pData_)
	{}

	~SAudioBuffer()
	{
		free(pData);
	}

	double const       sampleRate;
	unsigned int const bufferLength;
	int const          numChannels;
	void*              pData;
};
} // Arkanoid::Audio
