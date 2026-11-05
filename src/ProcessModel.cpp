#include "ProcessModel.hpp"
#include <iostream>
#include <algorithm>

ProcessModel::ProcessModel(const std::string& name)
    : mProcessName(name) {}

ProcessModel::~ProcessModel() {}

void ProcessModel::addEvent(const Event& event) {
    std::string activity = event.getActivity();
    mActivityCounts[activity]++;

    if (mActivityCounts[activity] == 1) {
        mActivities.push_back({activity, 0});
    }

    for (auto& act : mActivities) {
        if (act.name == activity) {
            act.count = mActivityCounts[activity];
            break;
        }
    }
}

void ProcessModel::addEvents(const std::vector<Event>& events) {
    for (const auto& event : events) {
        addEvent(event);
    }
}

void ProcessModel::discoverFromEvents(const std::vector<Event>& events) {
    if (events.empty()) return;

    mActivities.clear();
    mTransitions.clear();
    mPaths.clear();
    mActivityCounts.clear();
    mTransitionCounts.clear();
    mStartActivities.clear();
    mEndActivities.clear();

    std::vector<std::string> currentPath;
    std::string currentCase = events[0].getCaseId();

    for (size_t i = 0; i < events.size(); i++) {
        const auto& event = events[i];
        std::string activity = event.getActivity();

        mActivityCounts[activity]++;

        if (i == 0 || event.getCaseId() != events[i-1].getCaseId()) {
            currentPath.clear();
            currentPath.push_back(activity);
            mStartActivities.insert(activity);
        } else {
            std::string prevActivity = events[i-1].getActivity();
            std::pair<std::string, std::string> transitionKey = {prevActivity, activity};
            mTransitionCounts[transitionKey]++;

            currentPath.push_back(activity);
        }

        if (i == events.size() - 1 || event.getCaseId() != events[i+1].getCaseId()) {
            mEndActivities.insert(activity);
            mPaths.push_back({currentPath, 1});
        }
    }

    for (const auto& [activity, count] : mActivityCounts) {
        mActivities.push_back({activity, count});
    }

    for (const auto& [transition, count] : mTransitionCounts) {
        int totalFrom = 0;
        for (const auto& t : mTransitionCounts) {
            if (t.first.first == transition.first) {
                totalFrom += t.second;
            }
        }
        double probability = totalFrom > 0 ? static_cast<double>(count) / totalFrom : 0.0;
        mTransitions.push_back({transition.first, transition.second, count, probability});
    }
}

std::vector<ProcessModel::Activity> ProcessModel::getActivities() const {
    return mActivities;
}

std::vector<ProcessModel::Transition> ProcessModel::getTransitions() const {
    return mTransitions;
}

std::vector<ProcessModel::Path> ProcessModel::getPaths() const {
    return mPaths;
}

std::vector<std::string> ProcessModel::getStartActivities() const {
    return std::vector<std::string>(mStartActivities.begin(), mEndActivities.end());
}

std::vector<std::string> ProcessModel::getEndActivities() const {
    return std::vector<std::string>(mEndActivities.begin(), mEndActivities.end());
}

void ProcessModel::printSummary() const {
    std::cout << "Process Model: " << mProcessName << std::endl;
    std::cout << "Total Activities: " << mActivities.size() << std::endl;
    std::cout << "Total Transitions: " << mTransitions.size() << std::endl;
    std::cout << "Total Paths: " << mPaths.size() << std::endl;
    std::cout << "Start Activities: ";
    for (const auto& act : mStartActivities) {
        std::cout << act << " ";
    }
    std::cout << std::endl;
    std::cout << "End Activities: ";
    for (const auto& act : mEndActivities) {
        std::cout << act << " ";
    }
    std::cout << std::endl;
}

void ProcessModel::printTransitions() const {
    std::cout << "Transitions:" << std::endl;
    for (const auto& trans : mTransitions) {
        std::cout << "  " << trans.from << " -> " << trans.to
                  << " (count: " << trans.count << ", prob: " << trans.probability << ")" << std::endl;
    }
}

std::string ProcessModel::getProcessName() const {
    return mProcessName;
}

void ProcessModel::setProcessName(const std::string& name) {
    mProcessName = name;
}

std::map<std::string, int> ProcessModel::getActivityFrequency() const {
    return mActivityCounts;
}

std::map<std::pair<std::string, std::string>, int> ProcessModel::getTransitionFrequency() const {
    return mTransitionCounts;
}
