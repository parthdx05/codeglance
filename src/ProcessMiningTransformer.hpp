#pragma once
#include <vector>
#include <string>
#include <map>
#include "PullRequest.hpp"
#include "Commit.hpp"
#include "WorkflowRun.hpp"
#include "Event.hpp"

class ProcessMiningTransformer {
private:
    std::vector<Event> mEvents;
    std::map<long, std::string> mPRCaseIdMap;

public:
    ProcessMiningTransformer();
    ~ProcessMiningTransformer();

    void addPullRequest(const PullRequest& pr);
    void addCommits(const PullRequest& pr, const std::vector<Commit>& commits);
    void addWorkflowRuns(const std::vector<WorkflowRun>& runs);
    void addWorkflowRunsForPR(const PullRequest& pr, const std::vector<WorkflowRun>& runs);

    std::vector<Event> getEvents() const;
    void clearEvents();
    void sortEventsByTimestamp();

    void transformToCSV(const std::string& filename);
    void transformToMultipleCSV(const std::string& baseFilename);
};
