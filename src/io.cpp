#include "io.hpp"

/*
    Obter o conteúdo de um ficheiro e guardar num stringstream.
    Facilita a eventual extração da informação através dos seus operadores sobrecarregados.
*/
std::stringstream getFileData(std::string filename) {
    std::stringstream data;
    std::ifstream file;

    file.open(filename);
    if (file.is_open()) {
        data << file.rdbuf();
        file.close();
    } else {
        std::cout << "Erro: Nao foi possivel abrir o ficheiro \'" << filename << "\'\n";
        exit(-1);
    }
    return data;
}

/*
    Guardar o conteúdo de um stringstream num ficheiro.
*/
void setFileData(std::string filename, std::stringstream& data) {
    std::ofstream file;

    file.open(filename);
    if (file.is_open()) {
        file << data.str();
        file.close();
    } else {
        std::cout << "Erro: Nao foi possivel abrir o ficheiro \'" << filename << "\'\n";
        exit(-1);
    }
}
