#include "representation/SoundManager.h"
#include <iostream>

namespace representation {
SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager() : coinSoundTimeout(0.0f), isCoinSoundActive(false) { loadSounds(); }

SoundManager& SoundManager::getInstance() {
    if (instance == nullptr) {
        instance = new SoundManager();
    }
    return *instance;
}

void SoundManager::loadSounds() {
    if (!soundBuffers[SoundEffect::COIN_COLLECT].loadFromFile("resources/sounds/coin_collect.wav")) {
        std::cerr << "Failed to load coin_collect.wav" << std::endl;
    }

    if (!soundBuffers[SoundEffect::FRUIT_EAT].loadFromFile("resources/sounds/fruit_eat.wav")) {
        std::cerr << "Failed to load fruit_eat.wav" << std::endl;
    }

    if (!soundBuffers[SoundEffect::GHOST_FEAR].loadFromFile("resources/sounds/ghosts_fear.wav")) {
        std::cerr << "Failed to load ghosts_fear.wav" << std::endl;
    }

    if (!soundBuffers[SoundEffect::GHOST_EATEN].loadFromFile("resources/sounds/ghost_eaten.wav")) {
        std::cerr << "Failed to load ghost_eaten.wav" << std::endl;
    }

    if (!soundBuffers[SoundEffect::DEATH].loadFromFile("resources/sounds/death.wav")) {
        std::cerr << "Failed to load death.wav" << std::endl;
    }

    // Setup sounds (non-coin)
    sounds[SoundEffect::FRUIT_EAT].setBuffer(soundBuffers[SoundEffect::FRUIT_EAT]);
    sounds[SoundEffect::GHOST_FEAR].setBuffer(soundBuffers[SoundEffect::GHOST_FEAR]);
    sounds[SoundEffect::GHOST_EATEN].setBuffer(soundBuffers[SoundEffect::GHOST_EATEN]);
    sounds[SoundEffect::DEATH].setBuffer(soundBuffers[SoundEffect::DEATH]);

    // Setup continuous coin sound
    coinSound.setBuffer(soundBuffers[SoundEffect::COIN_COLLECT]);
    coinSound.setLoop(true);
    coinSound.setVolume(100.0f);

    if (!menuMusic.openFromFile("resources/sounds/menu_music.wav")) {
        std::cerr << "Failed to load menu_music.wav" << std::endl;
    }
    menuMusic.setLoop(false);
    menuMusic.setVolume(50.0f);
}

void SoundManager::update(float deltaTime) {
    if (isCoinSoundActive) {
        coinSoundTimeout -= deltaTime;

        if (coinSoundTimeout <= 0.0f) {
            coinSound.stop();
            isCoinSoundActive = false;
        }
    }
}

void SoundManager::stopCoinSound() {
    if (isCoinSoundActive) {
        coinSound.stop();
        isCoinSoundActive = false;
        coinSoundTimeout = 0.0f;
    }
}

void SoundManager::playSound(SoundEffect effect) {
    if (effect == SoundEffect::COIN_COLLECT) {
        coinSoundTimeout = COIN_TIMEOUT_DURATION;

        if (!isCoinSoundActive) {
            coinSound.play();
            isCoinSoundActive = true;
        }
    } else {
        auto it = sounds.find(effect);
        if (it != sounds.end()) {
            it->second.play();
        }
    }
}

void SoundManager::playMenuMusic() { menuMusic.play(); }

void SoundManager::stopMenuMusic() { menuMusic.stop(); }

bool SoundManager::isMenuMusicPlaying() const { return menuMusic.getStatus() == sf::Music::Playing; }

void SoundManager::setVolume(float volume) {
    coinSound.setVolume(volume);
    for (auto& [effect, sound] : sounds) {
        sound.setVolume(volume);
    }
    menuMusic.setVolume(volume);
}
} // namespace representation