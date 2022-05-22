#include "game.hpp"

/*
    Construtor da classe do jogo, responsável por inicializar todos os elementos necessários para a
    sua execução.
*/
game::game() {
    this->running = true;
    this->state = GAME_STATE_RESET;
    this->images = getFileData("images.txt").str();
    this->active_username = "none";
    this->start_of_page = 0;

    srand(time(NULL));

    loadPlayerData();
    loadThemeData();
}

game::~game() {

}

/*
    Metodo para receber o input do utilizador. Esta função bloqueia a execução do programa.
*/
std::string game::getUserInput() {
    std::string input;
    std::cin >> input;
    return input;
}

/*
    Sabendo que o tamanho de cada "imagem" de texto é um bloco de 32 * 73 bytes,
    sendo estas imagens contíguos na memória, é possível tratar "images" como uma
    tabela de imagens, associando um índice numérico a cada uma.
*/
std::string game::getImageAtIndex(int index) {
    int size = 32 * 73;
    return this->images.substr(index * size, size);
}

/*
    Definir qual a posição do cursor do terminal, relativamente ao canto superior esquerdo.
    
    Implementado com base no seguinte tópico:
    https://stackoverflow.com/questions/54250401/how-to-control-a-cursor-position-in-c-console-application
*/
inline void game::setCursorPos(int x, int y) {
#if defined(_WIN32)
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        {(short int) (x - 1), (short int) (y - 1)}
    );
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    printf("%c[%d;%df", 0x1B, y, x);
#endif
}

/*
    Função apenas para fins estéticos. Utilizado para designar uma seleção do utilizador
    e bloquear a execução do programa durante um determinado periodo de tempo.
*/
inline void game::setSelectionDelay(int x, int y, int delay) {
    setCursorPos(x, y);
    render("> ", false);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

/*
    Guardar os dados de todos os jogadores carregados na lista dinâmica "players" no ficheiro "players.txt".
*/
void game::savePlayerData() {
    std::stringstream data;

    int playerCount = this->players.size();

    data << playerCount << "\n";
    for (player& temp : this->players) {
        temp.toRawPlayerData(data);
    }

    setFileData("players.txt", data);
}

/*
    Carregar os dados de todos os jogadores guardados no ficheiro "players.txt" na lista dinâmica "players".
*/
void game::loadPlayerData() {
    std::stringstream data = getFileData("players.txt");
    int playerCount = 0;

    data >> playerCount;
    for (int i = 0; i < playerCount; i++) {
        player loaded;
        loaded.fromRawPlayerData(data);
        this->players.push_back(loaded);
    }
}

/*
    Determinar se existe registo de algum jogador com um determinado identificador "__username".
    Caso for encontrado, é devolvido uma referência ao objeto do jogador, senão é
    criado um novo registo com o mesmo identificador e devolvido a sua referência.
*/
player& game::getPlayerFromUsername(std::string __username) {
    for (player& temp : this->players) {
        if (temp.username == __username) {
            return temp;
        }
    }

    this->players.push_back(player(__username));
    return this->players.back();
}


/*
    Guardar os dados de todos os temas carregados na lista dinâmica "themes" no ficheiro "themes.txt".
*/
void game::saveThemeData() {
    std::stringstream data;

    int themeCount = this->themes.size();

    data << themeCount << "\n";
    for (std::list<word_info>& theme : this->themes) {
        int wordCount = theme.size();
        data << wordCount << "\n";
        for (word_info& word : theme) {
            data << word.word << " " << word.occurences << "\n";
        }
    }

    setFileData("themes.txt", data);
}

/*
    Carregar todos os temas, e as suas palavras associados do ficheiro "themes.txt".
*/
void game::loadThemeData() {
    std::stringstream data = getFileData("themes.txt");
    int themeCount = 0;

    data >> themeCount;
    for (int i = 0; i < themeCount; i++) {
        std::list<word_info> loadedTheme;
        int wordCount = 0;

        data >> wordCount;
        for (int j = 0; j < (wordCount); j++) {
            word_info loadedWord;
            data >> loadedWord.word >> loadedWord.occurences;
            loadedTheme.push_back(loadedWord);
        }

        this->themes.push_back(loadedTheme);
    }
}


/*
    Pedir o nome do utilizador, pesquisar se o nome já está associado 
    a algum jogador existente, se sim - carregar os seus dados, caso contrário - criar um
    novo registo do jogador.

    Sugestão de implementação:

    if username.size() > 0 and username.size() < 25 then return GAME_STATE_MENU
    else return GAME_STATE_LOGIN
*/
int game::loginActor() {
    render(getImageAtIndex(0));
    setCursorPos(15, 31);

    std::string username = getUserInput();

    if ((username.size() > 3) && (username.size() < 15)) {
        this->active_username = username;
        getPlayerFromUsername(this->active_username);

        return GAME_STATE_MENU;
    }

    return GAME_STATE_LOGIN;
}

/*
    Apresentar uma confirmação ao utilizador se desejar mesmo sair:
        [1] => Sim
        [2] => Não

    Sugestão de implementação:

    if [1] then return GAME_STATE_LOGIN
    if [2] then return GAME_STATE_MENU
    else return GAME_STATE_LOGOUT

*/
int game::logoutActor() {
    render(getImageAtIndex(2));
    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_LOGOUT;
    }

    switch(selection[0]) {
    case '1':
        savePlayerData();
        setSelectionDelay(23, 24, 800);
        return GAME_STATE_LOGIN;

    case '2':
        setSelectionDelay(40, 24, 800);
        return GAME_STATE_MENU;
    }

    return GAME_STATE_LOGOUT;
}

/*
    Apresentar as seguintes opções de seleção:
        [1] => Iniciar novo jogo
        [2] => Escolher modo de funcionamento
        [3] => Mostrar as pontuações
        [4] => Voltar ao login
    Pedir para o utilizador introduzir um dos numeros.

    Sugestão de implementação:

    if [1] then return GAME_STATE_NEW_GAME
    if [2] then return GAME_STATE_GAMEMODE
    if [3] then return GAME_STATE_LEADERBOARD
    if [4] then return GAME_STATE_LOGOUT
    else return GAME_STATE_MENU

*/
int game::menuActor() {
   render(getImageAtIndex(1));

    if(getPlayerFromUsername(this->active_username).gamemode_persistent < GAMEMODE_ADVANCED) {
        setCursorPos(23, 22);
        render("                         ", false);
    }

    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_MENU;
    }

    switch(selection[0]) {
    case '1':
        setSelectionDelay(23, 16, 800);
        return GAME_STATE_NEW_GAME;
    case '2':
        setSelectionDelay(23, 18, 800);
        return GAME_STATE_GAMEMODE;
    case '3':
        setSelectionDelay(23, 20, 800);
        return GAME_STATE_LEADERBOARD;
    case '4':
        if(getPlayerFromUsername(this->active_username).gamemode_persistent < GAMEMODE_ADVANCED) {
            return GAME_STATE_MENU;
        } else {
            setSelectionDelay(23, 22, 800);
            return GAME_STATE_CONFIG;
        }
    case '5':
        setSelectionDelay(23, 24, 800);
        return GAME_STATE_LOGOUT;
    }

    return GAME_STATE_MENU;
}

/*
    Apresentar uma lista de modos de funcionamento ao utilizador, acompanhado por uma descrição de cada:
        [1] => Elementar
        [2] => Básico
        [3] => Médio
        [4] => Voltar ao menu
    
    Alterar o valor da dificuldade do jogo de acordo com a escolha do utilizador.

    Sugestão de implementação:

    if [1] then return GAME_STATE_GAMEMODE
    if [2] then return GAME_STATE_GAMEMODE
    if [3] then return GAME_STATE_GAMEMODE
    if [4] then return GAME_STATE_MENU
    else return GAME_STATE_GAMEMODE

*/
int game::gamemodeActor() {
    render(getImageAtIndex(3));
    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_MENU;
    }

    switch(selection[0]) {
    case '1':
        getPlayerFromUsername(this->active_username).gamemode_persistent = GAMEMODE_SIMPLE;
        setSelectionDelay(23, 14, 800);
        return GAME_STATE_GAMEMODE;
    case '2':
        getPlayerFromUsername(this->active_username).gamemode_persistent = GAMEMODE_BASIC;
        setSelectionDelay(23, 16, 800);
        return GAME_STATE_DIFFICULTY;
    case '3':
        getPlayerFromUsername(this->active_username).gamemode_persistent = GAMEMODE_MEDIUM;
        setSelectionDelay(23, 18, 800);
        return GAME_STATE_DIFFICULTY;
    case '4':
        getPlayerFromUsername(this->active_username).gamemode_persistent = GAMEMODE_ADVANCED;
        setSelectionDelay(23, 20, 800);
        return GAME_STATE_DIFFICULTY;
    case '5':
        getPlayerFromUsername(this->active_username).gamemode_persistent = GAMEMODE_PROFESSIONAL;
        setSelectionDelay(23, 22, 800);
        return GAME_STATE_DIFFICULTY;
    case '6':
        setSelectionDelay(23, 24, 800);
        return GAME_STATE_MENU;
    }

    return GAME_STATE_GAMEMODE;
}

/*
    Selecionar a dificuldade do jogo, apresenta opções:
        [1] => Fácil
        [2] => Médio
        [3] => Difícil
        [4] => Voltar ao Menu

    Sugestão de implementação:

    if [1] then player.difficulty_persistent = DIFFICULTY_EASY
        return GAME_STATE_GAMEMODE
    if [2] then player.difficulty_persistent = DIFFICULTY_MEDIUM
        return GAME_STATE_GAMEMODE
    if [3] then player.difficulty_persistent = DIFFICULTY_HARD
        return GAME_STATE_GAMEMODE
    if [4] then return GAME_STATE_MENU
    else return GAME_STATE_DIFFICULTY  

*/
int game::difficultyActor() {
    render(getImageAtIndex(4));
    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_DIFFICULTY;
    }

    switch(selection[0]) {
    case '1':
        getPlayerFromUsername(this->active_username).difficulty_persistent = DIFFICULTY_EASY;
        setSelectionDelay(23, 18, 800);
        return GAME_STATE_GAMEMODE;
    case '2':
        getPlayerFromUsername(this->active_username).difficulty_persistent = DIFFICULTY_MEDIUM;
        setSelectionDelay(23, 20, 800);
        return GAME_STATE_GAMEMODE;
    case '3':
        getPlayerFromUsername(this->active_username).difficulty_persistent = DIFFICULTY_HARD;
        setSelectionDelay(23, 22, 800);
        return GAME_STATE_GAMEMODE;
    case '4':
        setSelectionDelay(23, 24, 800);
        return GAME_STATE_MENU;
    }

    return GAME_STATE_GAMEMODE;
}

/*
    Mostrar a lista dos jogadores com as 7 melhores pontuações, e as estatísticas do utilizador ao lado.
    Apresentar opcão de sair:
        [1] => Voltar ao menu
    
    Sugestão de implementação:

    if [1] then return GAME_STATE_MENU
    else return GAME_STATE_LEADERBOARD

*/
int game::leaderboardActor() {
    render(getImageAtIndex(6));

    this->players.sort([](const player& p1, const player& p2) {
        return p1.score_persistent > p2.score_persistent;
    });

    // Mostrar estatísticas do jogador
    player& activePlayer = getPlayerFromUsername(this->active_username);

    setCursorPos(7, 6);
    render(activePlayer.username, false);
    setCursorPos(18, 8);
    render(std::to_string(activePlayer.score_persistent) + " Pts.", false);
    setCursorPos(18, 10);
    render(std::to_string(activePlayer.rounds_persistent), false);
    setCursorPos(18, 12);
    render(std::to_string(activePlayer.fails_persistent), false);
    setCursorPos(18, 14);
    render(std::to_string((int)(activePlayer.time_persistent / 60)) + " Min.", false);

    int num_player = 0;
    int placement = this->start_of_page;
    for (player& temp : this->players) {
        if (num_player < this->start_of_page) {
            num_player++;
            continue;
        } else if (placement >= this->start_of_page + 7) {
            break;
        }

        setCursorPos(40, 5 + (placement - this->start_of_page) * 3);
        render(std::to_string(placement), false);
        setCursorPos(44, 5 + (placement - this->start_of_page) * 3);
        render(temp.username, false);
        setCursorPos(44, 6 + (placement - this->start_of_page) * 3);
        render(std::to_string(temp.score_persistent) + " Pts.", false);
        
        placement++;
        num_player++;
    }

    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_LEADERBOARD;
    }

    switch(selection[0]) {
    case '1':
        setSelectionDelay(6, 26, 800);
        this->start_of_page = 0;
        return GAME_STATE_MENU;
    case '2':
        setSelectionDelay(26, 26, 800);
        this->start_of_page -= 7;
        if (this->start_of_page < 0) {
            this->start_of_page = 0;
        }
        return GAME_STATE_LEADERBOARD;
    case '3':
        setSelectionDelay(47, 26, 800);
        this->start_of_page += 7;
        if (this->start_of_page < 0) {
            this->start_of_page = 0;
        }
        return GAME_STATE_LEADERBOARD;
    }

    return GAME_STATE_LEADERBOARD;
}

inline void resetPlayerRuntimeData(player& activePlayer) {
    activePlayer.gamemode_runtime = activePlayer.gamemode_persistent;
    activePlayer.difficulty_runtime = activePlayer.difficulty_persistent;
    activePlayer.theme_runtime = activePlayer.theme_persistent;
    activePlayer.score_runtime = 0;
    activePlayer.time_runtime = 0;
    activePlayer.hidden_word = "";
    activePlayer.attempts = "";    
}

/*
    Verificar se o utilizador já tem algum jogo em curso.
    Caso tiver, apresenta uma confirmação se o utilizador deseja continuar esse jogo
        [1] => Sim
        [2] => Não

    Sugestão de implementação:

    if [1] then return GAME_STATE_ROUND
    if [2] then (reset player runtime data)
        if [GAMEMODE_SIMPLE] then return GAME_STATE_ROUND
        else return GAME_STATE_THEME
    else return GAME_STATE_NEW_GAME

    Caso não tiver, segue apenas para um novo jogo.

    Sugestão de implementação:

    return GAME_STATE_ROUND

*/

int game::newGameActor() {
    player& activePlayer = getPlayerFromUsername(this->active_username);

    if (activePlayer.score_runtime == 0) {
        resetPlayerRuntimeData(activePlayer);
        return GAME_STATE_THEME; 
    }
    
    render(getImageAtIndex(5));
    setCursorPos(10, 31);

    std::string selection = getUserInput();

    if (selection.size() > 1) {
        return GAME_STATE_NEW_GAME;
    }

    switch(selection[0]) {
    case '1':
        setSelectionDelay(20, 24, 800);
        return GAME_STATE_ROUND;
    case '2':
        setSelectionDelay(37, 24, 800);

        resetPlayerRuntimeData(activePlayer);
        savePlayerData();

        return GAME_STATE_THEME;  
    }

    return GAME_STATE_NEW_GAME;
}

/*
    Implementado com base na seguinte questão:
    https://stackoverflow.com/questions/1761626/weighted-random-numbers
*/
std::string game::selectRandomWord(std::string theme) {
    std::list<word_info>& theme_data = getThemeFromName(theme);

    int weighted_sum = 0;
    for(word_info& word : theme_data) {
        if (word.occurences == 0) {
            continue;
        }
        int weight = 10000 / word.occurences;
        weighted_sum += weight;
    }

    int threshold = randi(0, weighted_sum);

    for(word_info& word : theme_data) {
        if (word.occurences == 0) {
            continue;
        }
        int weight = 10000 / word.occurences;
        if (threshold < weight) {
            word.occurences++;
            saveThemeData();
            return word.word;
        }
        threshold -= weight;
    }

    for(word_info& word : theme_data) {
        if (word.occurences == 0) {
            continue;
        }
        word.occurences = 1;
    }

    saveThemeData();
    return theme_data.back().word;
}

/*
    Escolher o tema pretendido do jogo.

    Sugestão de implementação:

    if [1] then return GAME_STATE_ROUND
    if [2] then return GAME_STATE_ROUND
    if [3] then return GAME_STATE_ROUND

    else return GAME_STATE_THEME
*/
int game::themeActor() {
    player& activePlayer = getPlayerFromUsername(this->active_username);

    if (activePlayer.gamemode_persistent == GAMEMODE_SIMPLE) {
        int select = randi(0, this->themes.size());
        int count = 0;
        for (std::list<word_info>& theme : this->themes) {
            if (select == count) {
                activePlayer.theme_persistent = theme.front().word;
                break;
            }
            count++;
        }
        return GAME_STATE_ROUND;
    }

    int theme_scroll = 0;
    do {
        render(getImageAtIndex(7));
        setCursorPos(10, 31);
        
        int offset = 0;
        auto theme_iter = this->themes.begin();
        while ((offset < theme_scroll) && (theme_iter != this->themes.end())) {
            offset++;
            theme_iter++;
        }

        for (int position = 0; position < 8; position++) {
            if (theme_iter != this->themes.end()) {
                word_info& word = (*theme_iter).front();
                setCursorPos(45, 4 + position * 3);
                render(word.word, false);
                theme_iter++;
            }                

            if ((position + theme_scroll) < 0) {
                continue;
            }

            setCursorPos(40, 4 + position * 3);
            render(std::to_string(position + theme_scroll), false);
        }

        setCursorPos(10, 31);
        std::string selection = getUserInput();

        if (selection.size() > 1) {
            bool exists = false;
            for (std::list<word_info>& theme : this->themes) {
                if (theme.front().word == selection) {
                    exists = true;
                    break;
                }
            }

            if (exists) {
                activePlayer.theme_persistent = selection;
                return GAME_STATE_ROUND;
            }
        } else if (selection[0] == '1') {
            theme_scroll++;
            if (theme_scroll > this->themes.size() - 8) {
                theme_scroll = this->themes.size() - 8;
            }
        } else if (selection[0] == '2') {
            theme_scroll--;
            if (theme_scroll < 0) {
                theme_scroll = 0;
            }
        }
    } while (true);

    return GAME_STATE_THEME;
}

std::list<word_info>& game::getThemeFromName(std::string __theme) {
    for (std::list<word_info>& theme : this->themes) {
        if (theme.front().word == __theme) {
            return theme;
        }
    }

    word_info identifier = {.word = __theme, .occurences = 0};
    std::list<word_info> new_theme;

    new_theme.push_back(identifier);
    this->themes.push_back(new_theme);
    return this->themes.back();
}

/*

*/
int game::configActor() {

    enum config {
        CONFIG_STATE_MENU,
        CONFIG_STATE_MODIFY,
        CONFIG_STATE_EXIT
    };

    int config_state = CONFIG_STATE_MENU;
    int config_scroll = 0;
    std::string config_name = "";

    do {
        if (config_state == CONFIG_STATE_MENU) {
            render(getImageAtIndex(20));
            setCursorPos(10, 31);

            std::string selection = getUserInput();

            if (selection.size() > 1) {
                continue;
            }

            switch(selection[0]) {
            case '1':
                setSelectionDelay(23, 18, 800);

                do {
                    render(getImageAtIndex(21));
                    setCursorPos(10, 31);
                    config_name = getUserInput();

                } while ((config_name.size() < 3) || (config_name.size() > 15));

                config_state = CONFIG_STATE_MODIFY;
                break;
            case '2':
                setSelectionDelay(23, 20, 800);

                do {
                    render(getImageAtIndex(21));
                    setCursorPos(10, 31);
                    config_name = getUserInput();

                } while ((config_name.size() < 3) || (config_name.size() > 15));

                config_state = CONFIG_STATE_MODIFY;
                break;
            case '3':
                setSelectionDelay(23, 22, 800);

                if (this->themes.size() <= 3) {
                    break;
                }

                do {
                    render(getImageAtIndex(21));
                    setCursorPos(10, 31);
                    config_name = getUserInput();

                } while ((config_name.size() < 3) || (config_name.size() > 15));

                this->themes.remove_if([&](std::list<word_info>& theme) {
                    return theme.front().word == config_name;
                });

                saveThemeData();
                config_state = CONFIG_STATE_MENU;
                break;
            case '4':
                setSelectionDelay(23, 24, 800);
                config_state = CONFIG_STATE_EXIT;
                break;
            }      
        } else if (config_state == CONFIG_STATE_MODIFY) {
            std::list<word_info>& config_data = getThemeFromName(config_name);

            render(getImageAtIndex(22));
            setCursorPos(18, 5);
            render(config_name, false);
            setCursorPos(18, 7);
            render(std::to_string(config_data.size() - 1), false);

            int offset = 0;
            auto word_iter = config_data.begin();
            while ((offset < config_scroll + 1) && (word_iter != config_data.end())) {
                offset++;
                word_iter++;
            }

            for (int position = 0; position < 8; position++) {
                if (word_iter != config_data.end()) {
                    word_info& word = *word_iter;
                    setCursorPos(45, 4 + position * 3);
                    render(word.word, false);
                    word_iter++;
                }                

                if ((position + config_scroll) < 0) {
                    continue;
                }

                setCursorPos(40, 4 + position * 3);
                render(std::to_string(position + config_scroll), false);
                
            }

            setCursorPos(10, 31);
            std::string selection = getUserInput();

            if (selection.size() > 1) {
                continue;
            }

            word_info config_word;
            bool exists = false;

            switch(selection[0]) {
            case '1':
                setSelectionDelay(10, 22, 800);
                setCursorPos(10, 31);

                config_word.word = getUserInput();
                config_word.occurences = 1;

                for (word_info& word : config_data) {
                    if (word.word == config_word.word) {
                        exists = true;
                    }
                }

                if (!exists) {
                    config_data.push_back(config_word);
                }

                break;
            case '2':
                setSelectionDelay(10, 24, 800);

                if (config_data.size() <= 9) {
                    break;
                }

                setCursorPos(10, 31);

                config_word.word = getUserInput();

                config_data.remove_if([&](const word_info& word) {
                    return word.word == config_word.word;
                });

                break;
            case '3':
                setSelectionDelay(10, 26, 800);

                saveThemeData();
                config_state = CONFIG_STATE_MENU;
                break;
            case '4':
                setSelectionDelay(44, 28, 800);
                config_scroll--;
                if (config_scroll < 0) {
                    config_scroll = 0;
                }
                break;
            case '5':
                setSelectionDelay(55, 28, 800);
                config_scroll++;
                if (config_scroll > config_data.size() - 8) {
                    config_scroll = config_data.size() - 8;
                }
                break;
            } 
        }
    } while(config_state != CONFIG_STATE_EXIT);

    return GAME_STATE_MENU;
}

/*
    Responsável pela execução do jogo.
    (Infelizmente não houve tempo para documentar esta parte).
    
*/
int game::roundActor() {
    render(getImageAtIndex(8));

    player& activePlayer = getPlayerFromUsername(this->active_username);
    std::string display_word;

    if (activePlayer.score_runtime == 0) {
        activePlayer.gamemode_runtime = activePlayer.gamemode_persistent;
        activePlayer.difficulty_runtime = activePlayer.difficulty_persistent;
        activePlayer.theme_runtime = activePlayer.theme_persistent;
        activePlayer.hidden_word = selectRandomWord(activePlayer.theme_runtime);
        activePlayer.attempts = "";
    }

    for(int i = 0; i < activePlayer.hidden_word.size(); i++) {
        display_word += "_ ";
    }

    int multiplier = (activePlayer.difficulty_runtime == DIFFICULTY_EASY) * 3 
        + (activePlayer.difficulty_runtime == DIFFICULTY_MEDIUM) * 5
        + (activePlayer.difficulty_runtime == DIFFICULTY_HARD) * 8;

    int unique = 0;
    int correct = 0;
    int fails = 0;

    // Encontrar numero de carateres únicas na palavra secreta.
    {    
        std::unordered_set<char> temp;
        for (int i = 0; i < activePlayer.hidden_word.size(); i++) {
            temp.insert(activePlayer.hidden_word[i]);
        }
        unique = temp.size();
    }

    for(int i = 0; i < activePlayer.attempts.size(); i++) { 
        int occurences_hidden_word = std::count(
            activePlayer.hidden_word.begin(),
            activePlayer.hidden_word.end(),
            activePlayer.attempts[i]
        );
        if (occurences_hidden_word == 0) {
            fails++;
        } else {
            correct++;
        }
    }

    while((fails < 9) && (correct < unique)) {
        render(getImageAtIndex(8 + fails));
        
        std::chrono::time_point<std::chrono::steady_clock> clock_start = std::chrono::steady_clock::now();
       
        std::string failed_attempts = "";
        for(int i = 0; i < activePlayer.attempts.size(); i++) {
            for(int j = 0; j < activePlayer.hidden_word.size(); j++) {
                if (activePlayer.attempts[i] == activePlayer.hidden_word[j]) {
                    display_word[j * 2] = activePlayer.attempts[i];
                } else if (
                    (failed_attempts.find(activePlayer.attempts[i]) > failed_attempts.size()) && 
                    (activePlayer.hidden_word.find(activePlayer.attempts[i]) > activePlayer.hidden_word.size())
                ) {
                    failed_attempts += activePlayer.attempts[i];
                }
            }
        }

        setCursorPos(10, 4);
        render(std::to_string(activePlayer.score_runtime), false);

        setCursorPos(10, 6);
        render(activePlayer.theme_persistent, false);

        setCursorPos((int)map(display_word.size(), 0, 38, 36, 16), 29);
        render(display_word, false);

        if (activePlayer.gamemode_runtime >= GAMEMODE_BASIC) {
            setCursorPos(10, 2);
            render(std::to_string(activePlayer.time_runtime), false);
        } else {
            setCursorPos(10, 2);
            render("---", false);
        }

        if (activePlayer.gamemode_runtime == GAMEMODE_MEDIUM) {
            setCursorPos(56, 3);
            render(failed_attempts, false);
        }

        setCursorPos(10, 31);
        std::string answer = getUserInput();

        if (answer.size() > 1) {
            continue;
        } else if (answer[0] == '1') {
            setSelectionDelay(57, 29, 800);
            render(getImageAtIndex(2));
            setCursorPos(10, 31);

            std::string selection = getUserInput();

            if (selection.size() > 1) {
                return GAME_STATE_LOGOUT;
            }

            switch(selection[0]) {
            case '1':
                savePlayerData();
                setSelectionDelay(23, 24, 800);
                return GAME_STATE_MENU;
            case '2':
                setSelectionDelay(40, 24, 800);
                break;
            }
            continue;
        }

        std::chrono::time_point<std::chrono::steady_clock> clock_end = std::chrono::steady_clock::now();

        int occurences_hidden_word = std::count(
            activePlayer.hidden_word.begin(),
            activePlayer.hidden_word.end(),
            answer[0]
        );

        int occurences_attempts = std::count(
            activePlayer.attempts.begin(),
            activePlayer.attempts.end(),
            answer[0]
        );

        if ((occurences_hidden_word > 0) && (occurences_attempts == 0)) {
            activePlayer.attempts += answer;
            correct++;
        } else if (occurences_attempts == 0) {
            activePlayer.attempts += answer;
            fails++;
        }

        std::chrono::duration<float> difference = (clock_end - clock_start);
        float duration = difference.count();

        activePlayer.time_runtime += duration;
        activePlayer.score_runtime = (correct + map(duration, 0, 10, 1, -0.5)) * multiplier;
        
        savePlayerData();
    }

    if(fails == 9) {
        render(getImageAtIndex(19));
    } else {
        render(getImageAtIndex(18));

        setCursorPos(33,14);
        render(std::to_string(activePlayer.score_runtime), false);

        setCursorPos(33,18);
        render(std::to_string(fails), false);

        if(activePlayer.gamemode_runtime >= GAMEMODE_SIMPLE) {
            setCursorPos(33,22);
            render(std::to_string(activePlayer.time_runtime), false);
        }
    }

    activePlayer.score_persistent += activePlayer.score_runtime;
    activePlayer.rounds_persistent++;
    activePlayer.time_persistent += activePlayer.time_runtime;
    activePlayer.fails_persistent += fails;

    resetPlayerRuntimeData(activePlayer);
    savePlayerData();

    setCursorPos(10,31);
    std::string answer;
    do {
        answer = getUserInput();
    } while (answer != "4");

    return GAME_STATE_MENU;
}

/*
    Cada vez que é chamado, a imagem que está carregado no framebuffer é imprimido para o ecrã.
    Tem uma especificação para limpar o conteúdo do ecrã dependendo do sistema operativo.
*/
void game::render(std::string framebuffer, bool clearscreen) {
    if (clearscreen) { 
    #if defined(_WIN32)
        system("cls");
    #elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #endif
    }
    std::cout.write(framebuffer.c_str(), framebuffer.size());
    std::cout.flush();
}

/*
    Implementação da máquina de estados do jogo, responsável por avaliar o estado atual, 
    encaminhar a sua execução para o respetivo "actor", atualizando o estado do jogo.

    A execução do jogo termina quando "running" é avaliado a falso.

*/
void game::run() {
    do {
        switch (this->state) {
        case GAME_STATE_RESET:
        case GAME_STATE_LOGIN:
            this->state = loginActor();
            break;         
        case GAME_STATE_MENU:
            this->state = menuActor();
            break; 
        case GAME_STATE_NEW_GAME:
            this->state = newGameActor();
            break;
        case GAME_STATE_GAMEMODE:
            this->state = gamemodeActor();
            break;
        case GAME_STATE_DIFFICULTY:
            this->state = difficultyActor();
            break;
        case GAME_STATE_LEADERBOARD:
            this->state = leaderboardActor();
            break;
        case GAME_STATE_LOGOUT:
            this->state = logoutActor();
            break;
        case GAME_STATE_THEME:
            this->state = themeActor();
            break; 
        case GAME_STATE_ROUND:
            this->state = roundActor();
            break; 
        case GAME_STATE_CONFIG:
            this->state = configActor();
            break; 
        case GAME_STATE_ERROR:
        default:
            this->running = false;
        }

	} while (this->running);
}
