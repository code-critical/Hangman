#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <sstream>
#include <iomanip>

#include <algorithm>
#include <unordered_set>
#include <string>
#include <list>

#include <chrono>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "player.hpp"
#include "io.hpp"
#include "mathutils.hpp"

typedef struct {
    std::string word;
    int occurences;
} word_info;

class game {
private:
    // Manutenção de estado
	bool running;
	int state;

    // Renderização
    std::string images;
    int start_of_page;

    std::string getImageAtIndex(int index);
    void setCursorPos(int x, int y); 
    void setSelectionDelay(int x, int y, int delay);

    // Jogadores
    std::string active_username;
    std::list<player> players;

    player& getPlayerFromUsername(std::string __username);
    
    // Persistência de dados
    void savePlayerData();
    void loadPlayerData();

    // Alteração de temas
    std::list<std::list<word_info>> themes;

    std::list<word_info>& getThemeFromName(std::string __theme);

    void saveThemeData();
    void loadThemeData();

    std::string selectRandomWord(std::string theme);

    // Controlo do utilizador
    std::string getUserInput();

    // Atores de estado
    int loginActor();
    int logoutActor();
    int menuActor();
    int newGameActor();
    int gamemodeActor();
    int difficultyActor();
    int leaderboardActor();
    int themeActor();
    int roundActor();
    int configActor();

public:
	game();
	~game();

    void render(std::string framebuffer, bool clearscreen = true);
    void run();

};

enum game_state {
    GAME_STATE_ERROR,
    GAME_STATE_RESET,
    GAME_STATE_LOGIN,
    GAME_STATE_MENU,
    GAME_STATE_NEW_GAME,
    GAME_STATE_GAMEMODE,
    GAME_STATE_DIFFICULTY,
    GAME_STATE_LEADERBOARD,
    GAME_STATE_LOGOUT,
    GAME_STATE_THEME,
    GAME_STATE_ROUND,
    GAME_STATE_CONFIG
};

#endif