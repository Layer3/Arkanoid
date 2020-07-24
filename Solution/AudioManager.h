#pragma once
#include "AudioMixer.h"
#include "PlayingVoice.h"
#include <portaudio.h>
#include <vector>

namespace Arkanoid::Game
{
class CRenderedObject;
} // Arkanoid::Game

namespace Arkanoid::Audio
{
class CAudioManager
{
public:

	CAudioManager();
	~CAudioManager();

	void Play(char const* filePath, bool const positioned = false, Vec2D position = Vec2D(0.0f, 0.0f));
	void RenderAudio(void* pOutBuffer);
	bool UpdatePosition(SPlayingVoice* pVoice, Vec2D const& pos);

private:

	std::vector<Arkanoid::Audio::SPlayingVoice*> m_pPlayingVoices{};
	Arkanoid::Audio::CAudioMixer*                m_pMixer = nullptr;
	PaStream*                                    m_pStream = nullptr;
	Arkanoid::Audio::SAudioBuffer*               m_pOutputBuffer = nullptr;

	int    m_numChannels = 0;
	double m_sampleRate = 0;
	int    m_bufferLength = 0;

};
} // Arkanoid::Audio
