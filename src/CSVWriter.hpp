#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Event.hpp"

class CSVWriter {
private:
    std::string mFilename;
    bool mHeadersWritten;

public:
    CSVWriter(std::string filename);
    ~CSVWriter();

    void writeHeader();
    void writeEvent(const Event& event);
    void writeEvents(const std::vector<Event>& events);
};
