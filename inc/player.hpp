#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <sstream>
#include <string>
#include <cstring>

enum gamemode_persistent {
    GAMEMODE_SIMPLE,
    GAMEMODE_BASIC,
    GAMEMODE_MEDIUM,
    GAMEMODE_ADVANCED,
    GAMEMODE_PROFESSIONAL
};

enum difficulty_persistent {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
};

class player {
public:
	// Persistent data
	std::string username;
    int score_persistent;
	int rounds_persistent;
    int fails_persistent;
    float time_persistent;
    int gamemode_persistent;
    int difficulty_persistent;
    std::string theme_persistent;
	// Runtime data
    int gamemode_runtime;
    int difficulty_runtime;
    std::string theme_runtime;
	int score_runtime;
	int time_runtime;
    std::string hidden_word;
    std::string attempts;

    player(std::string __username = "none");
    ~player();

    player& fromRawPlayerData(std::stringstream& data);
    player& toRawPlayerData(std::stringstream& data);
};

#endif
