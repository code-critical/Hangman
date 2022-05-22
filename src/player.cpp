#include "player.hpp"
#include <iostream>

player::player(std::string __username) {
	// Persistent data
	username = __username;
    score_persistent = 0;
	rounds_persistent = 0;
    fails_persistent = 0;
    time_persistent = 0;
    gamemode_persistent = GAMEMODE_SIMPLE;
    difficulty_persistent = DIFFICULTY_EASY;
    theme_persistent = "";
	// Runtime data
    gamemode_runtime = GAMEMODE_SIMPLE;
    difficulty_runtime = DIFFICULTY_EASY;
    theme_runtime = "";
	score_runtime = 0;
	time_runtime = 0;
    hidden_word = "";
    attempts = "";
}

player::~player() {

}

player& player::fromRawPlayerData(std::stringstream& data) {
    data >> this->username
        >> this->score_persistent
        >> this->rounds_persistent
        >> this->fails_persistent
        >> this->time_persistent
        >> this->gamemode_persistent
        >> this->difficulty_persistent
        >> this->theme_persistent
        >> this->score_runtime
        >> this->time_runtime
        >> this->hidden_word
        >> this->attempts;

    if (this->theme_persistent == "none") {
        this->theme_persistent = "";
    }

    if (this->theme_runtime == "none") {
        this->theme_runtime = "";
    }

    if (this->hidden_word == "none") {
        this->hidden_word = "";
    }

    if (this->attempts == "none") {
        this->attempts = "";
    }

    return *this;
}

player& player::toRawPlayerData(std::stringstream& data) {
    if (this->theme_persistent == "") {
        this->theme_persistent = "none";
    }

    if (this->theme_runtime == "") {
        this->theme_runtime = "none";
    }

    if (this->hidden_word == "") {
        this->hidden_word = "none";
    }
    if (this->attempts == "") {
        this->attempts = "none";
    }
    data << this->username                  << '\n'
        << this->score_persistent           << '\n'
        << this->rounds_persistent          << '\n'
        << this->fails_persistent           << '\n'
        << this->time_persistent            << '\n'
        << this->gamemode_persistent        << '\n'
        << this->difficulty_persistent      << '\n'
        << this->theme_persistent           << '\n'
        << this->score_runtime              << '\n'
        << this->time_runtime               << '\n'
        << this->hidden_word                << '\n'
        << this->attempts                   << '\n';
    
    if (this->theme_persistent == "none") {
        this->theme_persistent = "";
    }

    if (this->theme_runtime == "none") {
        this->theme_runtime = "";
    }

    if (this->hidden_word == "none") {
        this->hidden_word = "";
    }

    if (this->attempts == "none") {
        this->attempts = "";
    }
    
    return *this;
}