#ifndef PACMANGAME_SOUNDMANAGER_H
#define PACMANGAME_SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>

namespace representation {
enum class SoundEffect { COIN_COLLECT, FRUIT_EAT, GHOST_FEAR, GHOST_EATEN, DEATH };

/**
 * Singleton manager for game audio (sound effects and music).
 *
 * Key features:
 * - Centralized audio control (prevents duplicate SoundBuffer loads)
 * - Continuous coin sound: loops while collecting coins rapidly,
 *   stops after 0.3s timeout (prevents audio spam from rapid collection)
 * - Menu music: non-looping background track
 *
 * Singleton pattern ensures single audio instance (SFML limitation:
 * SoundBuffers can't be easily shared, sounds must reference same buffer).
 */
class SoundManager {
private:
    static SoundManager* instance;

    std::unordered_map<SoundEffect, sf::SoundBuffer> soundBuffers;
    std::unordered_map<SoundEffect, sf::Sound> sounds;

    sf::Sound coinSound;
    float coinSoundTimeout;
    const float COIN_TIMEOUT_DURATION = 0.3f; // Stop coin loop if no new coins collected within 300ms
    bool isCoinSoundActive;

    sf::Music menuMusic;

    SoundManager();

public:
    static SoundManager& getInstance();

    SoundManager(const SoundManager&) = delete;

    SoundManager& operator=(const SoundManager&) = delete;

    void loadSounds();

    void playSound(SoundEffect effect);

    /**
     * Updates continuous coin sound timeout.
     * Stops coin loop if timeout expires (no coins collected recently).
     * Call once per frame with deltaTime from Stopwatch.
     */
    void update(float deltaTime);

    void stopCoinSound();

    void playMenuMusic();

    void stopMenuMusic();

    bool isMenuMusicPlaying() const;

    void setVolume(float volume);
};
} // namespace representation

#endif // PACMANGAME_SOUNDMANAGER_H