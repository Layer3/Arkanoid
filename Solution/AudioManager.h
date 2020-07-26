#pragma once
#include "AudioMixer.h"
#include "BiquadFilter.h"
#include "PlayingVoice.h"
#include <portaudio.h>
#include <vector>

namespace Arkanoid::Audio
{
enum class EMusic
{
	Menu,
	Game,
	Tension,

};

class CAudioManager final
{
public:

	explicit CAudioManager();
	~CAudioManager();

	SPlayingVoice* Play(char const* filePath, bool const positioned = false, Vec2D position = Vec2D(0.0f, 0.0f));
	bool           ShouldFilter();
	void           FilterBuffer(void* pOutputBuffer);
	void           SetFilterAmount(float const amount);
	void           RenderAudio(void* pOutBuffer);
	bool           UpdatePosition(SPlayingVoice* pVoice, Vec2D const& pos);
	void           SetMusic(EMusic const music);


private:

	std::vector<std::shared_ptr<Arkanoid::Audio::SPlayingVoice>> m_pPlayingVoices;
	Arkanoid::Audio::CAudioMixer*                                m_pMixer = nullptr;
	Arkanoid::Audio::SAudioBuffer*                               m_pOutputBuffer = nullptr;
	PaStream*                                                    m_pStream = nullptr;

	SPlayingVoice* m_pMusicVoice = nullptr;
	SNDFILE*       m_pMusicFiles[3]{nullptr};
	EMusic         m_playing = EMusic::Menu;
	EMusic         m_lastPlayed = EMusic::Menu;

	int    m_numChannels = 0;
	double m_sampleRate = 0;
	int    m_bufferLength = 0;

	std::atomic<float> m_filterAmount = 0.0f;
	float              m_oldFilterAmount = 0.0f;
	bool               m_filterSwitch = false;
	CBiquadFilter*     m_pFilters[4]{nullptr};
};
} // Arkanoid::Audio
