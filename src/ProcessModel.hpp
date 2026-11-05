#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Event.hpp"

class ProcessModel {
public:
    struct Activity {
        std::string name;
        int count;
    };

    struct Transition {
        std::string from;
        std::string to;
        int count;
        double probability;
    };

    struct Path {
        std::vector<std::string> activities;
        int count;
    };

private:
    std::string mProcessName;
    std::vector<Activity> mActivities;
    std::vector<Transition> mTransitions;
    std::vector<Path> mPaths;
    std::map<std::string, int> mActivityCounts;
    std::map<std::pair<std::string, std::string>, int> mTransitionCounts;
    std::set<std::string> mStartActivities;
    std::set<std::string> mEndActivities;

public:
    ProcessModel(const std::string& name = "Unnamed Process");
    ~ProcessModel();

    void addEvent(const Event& event);
    void addEvents(const std::vector<Event>& events);
    void discoverFromEvents(const std::vector<Event>& events);

    std::vector<Activity> getActivities() const;
    std::vector<Transition> getTransitions() const;
    std::vector<Path> getPaths() const;
    std::vector<std::string> getStartActivities() const;
    std::vector<std::string> getEndActivities() const;

    void printSummary() const;
    void printTransitions() const;

    std::string getProcessName() const;
    void setProcessName(const std::string& name);

    std::map<std::string, int> getActivityFrequency() const;
    std::map<std::pair<std::string, std::string>, int> getTransitionFrequency() const;
};
