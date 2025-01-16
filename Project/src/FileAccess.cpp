#include "../include/FileAccess.hpp"

void FileAccess::WriteBytesToFile(std::vector<char> &ToWrite)
{
    // Überprüfen, ob der Stream geöffnet ist
    if (getFileStream()->is_open())
    {
        for (size_t i = 0; i < ToWrite.size(); ++i)
        {
            // Schreibe das Byte
            getFileStream()->write(&ToWrite[i], sizeof(ToWrite[i]));
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
    if (strm->is_open())
    {
        for (size_t i = 0; i < ToWrite.size(); ++i)
        {
            // Schreibe das Byte
            getFileStream()->write(&ToWrite[i], sizeof(ToWrite[i]));
        }
    }
    else
    {
        std::cerr << "Fehler: Die Datei ist nicht geöffnet!" << std::endl;
    }
}
