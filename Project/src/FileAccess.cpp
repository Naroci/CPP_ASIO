#include "../include/FileAccess.hpp"

void FileAccess::WriteBytesToFile(std::vector<char> &ToWrite)
{
    // Überprüfen, ob der Stream geöffnet ist
    auto strm = FileAccess::getFileStream();
    if (strm && strm->is_open())
    {
        // Direkter Schreibvorgang für den gesamten Vector
        strm->write(ToWrite.data(), ToWrite.size());

        if (!strm->good())
        {
            std::cerr << "Fehler beim Schreiben der Datei!" << std::endl;
        }
    }
    else
    {
        std::cerr << "Fehler: Die Datei ist nicht geöffnet!" << std::endl;
    }
}

void FileAccess::WriteStringToFile(std::string &ToWrite)
{
    // Überprüfen, ob der Stream geöffnet ist
    auto strm = FileAccess::getFileStream();
    if (strm && strm->is_open())
    {
        // Direkter Schreibvorgang für den gesamten Vector
        strm->write(ToWrite.data(), ToWrite.size());

        if (!strm->good())
        {
            std::cerr << "Fehler beim Schreiben der Datei!" << std::endl;
        }
    }
    else
    {
        std::cerr << "Fehler: Die Datei ist nicht geöffnet!" << std::endl;
    }
}
