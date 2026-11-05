#pragma once
#include <vector>
#include <string>
#include <map>
#include "Event.hpp"
#include "ProcessModel.hpp"

class ProcessMiner {
public:
    enum class Algorithm {
        ALPHA_MINER,
        HEURISTICS_MINER,
        INDUCTIVE_MINER
    };

private:
    Algorithm mAlgorithm;
    std::vector<ProcessModel> mProcessModels;

public:
    ProcessMiner(Algorithm algo = Algorithm::ALPHA_MINER);
    ~ProcessMiner();

    void setAlgorithm(Algorithm algo);
    Algorithm getAlgorithm() const;

    ProcessModel discoverProcessModel(const std::vector<Event>& events, const std::string& processName = "");
    std::vector<ProcessModel> discoverProcessModelsByCase(const std::vector<Event>& events);
    ProcessModel discoverProcessModelByActivity(const std::vector<Event>& events, const std::string& activityFilter);

    std::map<std::string, int> calculateActivityFrequency(const std::vector<Event>& events) const;
    std::map<std::pair<std::string, std::string>, int> calculateTransitionFrequency(const std::vector<Event>& events) const;

    double calculateProcessEfficiency(const std::vector<Event>& events) const;
    std::vector<std::string> findBottlenecks(const std::vector<Event>& events, int threshold = 1) const;

    void printProcessStatistics(const std::vector<Event>& events) const;
};
