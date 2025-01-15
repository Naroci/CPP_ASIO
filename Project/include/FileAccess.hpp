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
        m_fileStream = new std::fstream(m_FilePath);
    }
    FileAccess();
    void WriteBytesToFile(std::vector<char> &ToWrite);

  private:
    std::string m_FilePath;
    std::fstream *getFileStream();

    std::fstream *m_fileStream = nullptr;
    void setFileStream(std::fstream *filestream);
};