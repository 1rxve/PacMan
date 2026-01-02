#ifndef PACMANGAME_SOUNDMANAGER_H
#define PACMANGAME_SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

namespace representation {
    enum class SoundEffect {
        COIN_COLLECT,
        FRUIT_EAT,
        GHOST_FEAR,
        GHOST_EATEN,
        DEATH
    };

    class SoundManager {
    private:
        static SoundManager* instance;

        std::unordered_map<SoundEffect, sf::SoundBuffer> soundBuffers;
        std::unordered_map<SoundEffect, sf::Sound> sounds;

        // Continuous coin sound system
        sf::Sound coinSound;
        float coinSoundTimeout;
        const float COIN_TIMEOUT_DURATION = 0.3f;
        bool isCoinSoundActive;

        sf::Music menuMusic;

        SoundManager();

    public:
        static SoundManager& getInstance();

        SoundManager(const SoundManager&) = delete;
        SoundManager& operator=(const SoundManager&) = delete;

        void loadSounds();
        void playSound(SoundEffect effect);
        void update(float deltaTime);
        void stopCoinSound();
        void playMenuMusic();
        void stopMenuMusic();
        bool isMenuMusicPlaying() const;
        void setVolume(float volume);
    };
}

#endif //PACMANGAME_SOUNDMANAGER_H