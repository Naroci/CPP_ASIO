#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class FileAccess
{
  public:
    FileAccess(std::string FilePath)
    {
        m_FilePath = FilePath;
        setFileStream(new std::fstream(FilePath, std::ios::out | std::ios::binary));
    }
    FileAccess();
    void WriteStringToFile(std::string &ToWrite);
    void WriteBytesToFile(std::vector<char> &ToWrite);

  private:
    std::string m_FilePath;
    std::fstream *getFileStream() {return m_fileStream;};

    std::fstream *m_fileStream = nullptr;
    void setFileStream(std::fstream *filestream) { m_fileStream =  filestream; };
};