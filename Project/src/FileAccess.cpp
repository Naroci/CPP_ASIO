#include "../include/FileAccess.hpp"

void FileAccess::WriteBytesToFile(std::vector<std::byte> &ToWrite) 
{
    // Überprüfen, ob der Stream geöffnet ist
    if (getFileStream()->is_open())
    {
        for (size_t i = 0; i < ToWrite.size(); ++i)
        {
            // Umwandlung von std::byte zu char
            char byte = static_cast<char>(ToWrite[i]);
            
            // Schreibe das Byte
            getFileStream()->write(&byte, sizeof(byte));
        }
    }
    else
    {
        std::cerr << "Fehler: Die Datei ist nicht geöffnet!" << std::endl;
    }
}


std::fstream* FileAccess::getFileStream(){
    return m_fileStream;
}

void FileAccess::setFileStream(std::fstream *stream)
{
    m_fileStream = stream;
}
