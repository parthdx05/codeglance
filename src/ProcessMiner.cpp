#include "ProcessMiner.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>

ProcessMiner::ProcessMiner(Algorithm algo)
    : mAlgorithm(algo) {}

ProcessMiner::~ProcessMiner() {}

void ProcessMiner::setAlgorithm(Algorithm algo) {
    mAlgorithm = algo;
}

ProcessMiner::Algorithm ProcessMiner::getAlgorithm() const {
    return mAlgorithm;
}

ProcessModel ProcessMiner::discoverProcessModel(const std::vector<Event>& events, const std::string& processName) {
    ProcessModel model(processName.empty() ? "Discovered Process" : processName);
    model.discoverFromEvents(events);
    return model;
}

std::vector<ProcessModel> ProcessMiner::discoverProcessModelsByCase(const std::vector<Event>& events) {
    std::vector<ProcessModel> models;
    std::map<std::string, std::vector<Event>> eventsByCase;

    for (const auto& event : events) {
        eventsByCase[event.getCaseId()].push_back(event);
    }

    for (const auto& [caseId, caseEvents] : eventsByCase) {
        ProcessModel model(caseId);
        model.discoverFromEvents(caseEvents);
        models.push_back(model);
    }

    return models;
}

ProcessModel ProcessMiner::discoverProcessModelByActivity(const std::vector<Event>& events, const std::string& activityFilter) {
    std::vector<Event> filteredEvents;
    for (const auto& event : events) {
        if (event.getActivity().find(activityFilter) != std::string::npos) {
            filteredEvents.push_back(event);
        }
    }

    ProcessModel model("Filtered: " + activityFilter);
    model.discoverFromEvents(filteredEvents);
    return model;
}

std::map<std::string, int> ProcessMiner::calculateActivityFrequency(const std::vector<Event>& events) const {
    std::map<std::string, int> frequency;
    for (const auto& event : events) {
        frequency[event.getActivity()]++;
    }
    return frequency;
}

std::map<std::pair<std::string, std::string>, int> ProcessMiner::calculateTransitionFrequency(const std::vector<Event>& events) const {
    std::map<std::pair<std::string, std::string>, int> frequency;

    for (size_t i = 1; i < events.size(); i++) {
        if (events[i].getCaseId() == events[i-1].getCaseId()) {
            std::pair<std::string, std::string> transition = {
                events[i-1].getActivity(),
                events[i].getActivity()
            };
            frequency[transition]++;
        }
    }

    return frequency;
}

double ProcessMiner::calculateProcessEfficiency(const std::vector<Event>& events) const {
    if (events.empty()) return 0.0;

    auto frequency = calculateActivityFrequency(events);
    int totalActivities = std::accumulate(frequency.begin(), frequency.end(), 0,
        [](int sum, const auto& pair) { return sum + pair.second; });

    int uniqueActivities = frequency.size();
    return static_cast<double>(uniqueActivities) / totalActivities;
}

std::vector<std::string> ProcessMiner::findBottlenecks(const std::vector<Event>& events, int threshold) const {
    std::vector<std::string> bottlenecks;
    auto frequency = calculateActivityFrequency(events);

    int maxCount = 0;
    for (const auto& [activity, count] : frequency) {
        if (count > maxCount) {
            maxCount = count;
        }
    }

    for (const auto& [activity, count] : frequency) {
        if (maxCount - count <= threshold) {
            bottlenecks.push_back(activity);
        }
    }

    return bottlenecks;
}

void ProcessMiner::printProcessStatistics(const std::vector<Event>& events) const {
    auto activityFreq = calculateActivityFrequency(events);
    auto transitionFreq = calculateTransitionFrequency(events);

    std::cout << "=== Process Statistics ===" << std::endl;
    std::cout << "Total Events: " << events.size() << std::endl;
    std::cout << "Unique Activities: " << activityFreq.size() << std::endl;
    std::cout << "Unique Transitions: " << transitionFreq.size() << std::endl;
    std::cout << std::endl;

    std::cout << "Activity Frequencies:" << std::endl;
    for (const auto& [activity, count] : activityFreq) {
        std::cout << "  " << activity << ": " << count << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Top Transitions:" << std::endl;
    std::vector<std::pair<std::pair<std::string, std::string>, int>> sortedTransitions(transitionFreq.begin(), transitionFreq.end());
    std::sort(sortedTransitions.begin(), sortedTransitions.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });

    for (size_t i = 0; i < std::min(sortedTransitions.size(), size_t(5)); i++) {
        std::cout << "  " << sortedTransitions[i].first.first << " -> "
                  << sortedTransitions[i].first.second << ": " << sortedTransitions[i].second << std::endl;
    }
}
