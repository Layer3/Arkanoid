#pragma once
#include "Global.h"

namespace Arkanoid::Audio
{
struct SAudioBuffer final
{
	SAudioBuffer() = delete;

	explicit SAudioBuffer(double const sampleRate_, unsigned int const bufferLength_, int const numChannels_, void* pData_)
		: sampleRate(sampleRate_)
		, bufferLength(bufferLength_)
		, numChannels(numChannels_)
		, pData(pData_)
	{}

	~SAudioBuffer() = default;

	double const       sampleRate;
	unsigned int const bufferLength;
	int const          numChannels;
	void*              pData;
};
} // Arkanoid::Audio
