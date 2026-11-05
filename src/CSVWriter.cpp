#include "CSVWriter.hpp"

CSVWriter::CSVWriter(std::string filename)
    : mFilename(filename), mHeadersWritten(false) {}

CSVWriter::~CSVWriter() {}

void CSVWriter::writeHeader() {
    if (mHeadersWritten) return;

    std::ofstream file(mFilename, std::ios::app);
    if (file.is_open()) {
        auto headers = Event::getCSVHeaders();
        for (size_t i = 0; i < headers.size(); ++i) {
            file << "\"" << headers[i] << "\"";
            if (i < headers.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
        mHeadersWritten = true;
    }
}

void CSVWriter::writeEvent(const Event& event) {
    writeHeader();
    std::ofstream file(mFilename, std::ios::app);
    if (file.is_open()) {
        file << event.toCSVRow() << "\n";
    }
}

void CSVWriter::writeEvents(const std::vector<Event>& events) {
    writeHeader();
    std::ofstream file(mFilename, std::ios::app);
    if (file.is_open()) {
        for (const auto& event : events) {
            file << event.toCSVRow() << "\n";
        }
    }
}
