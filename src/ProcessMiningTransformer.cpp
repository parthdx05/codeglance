#include "ProcessMiningTransformer.hpp"
#include "CSVWriter.hpp"
#include "helper.hpp"

ProcessMiningTransformer::ProcessMiningTransformer() {}

ProcessMiningTransformer::~ProcessMiningTransformer() {}

void ProcessMiningTransformer::addPullRequest(const PullRequest& pr) {
    std::string caseId = "PR_" + std::to_string(pr.getNumber());
    mPRCaseIdMap[pr.getNumber()] = caseId;
    std::string author = pr.getAuthor().getLogin();

    Event prCreated;
    prCreated.setCaseId(caseId);
    prCreated.setActivity("PR Created");
    prCreated.setTimestamp(pr.getCreatedAt());
    prCreated.setPRNumber(std::to_string(pr.getNumber()));
    prCreated.setPRTitle(pr.getTitle());
    prCreated.setAuthor(author);
    prCreated.setBranch(pr.getBaseRef());
    prCreated.setAdditions(pr.getAdditions());
    prCreated.setDeletions(pr.getDeletions());
    prCreated.setChangedFiles(pr.getChangedFiles());
    mEvents.push_back(prCreated);

    if (pr.getState() == "closed") {
        if (pr.getMergedAt().time_since_epoch().count() != 0) {
            Event prMerged;
            prMerged.setCaseId(caseId);
            prMerged.setActivity("PR Merged");
            prMerged.setTimestamp(pr.getMergedAt());
            prMerged.setPRNumber(std::to_string(pr.getNumber()));
            prMerged.setPRTitle(pr.getTitle());
            prMerged.setAuthor(pr.getMergedBy().getLogin());
            prMerged.setBranch(pr.getBaseRef());
            mEvents.push_back(prMerged);
        }

        Event prClosed;
        prClosed.setCaseId(caseId);
        prClosed.setActivity("PR Closed");
        prClosed.setTimestamp(pr.getClosedAt());
        prClosed.setPRNumber(std::to_string(pr.getNumber()));
        prClosed.setPRTitle(pr.getTitle());
        prClosed.setAuthor(author);
        prClosed.setBranch(pr.getBaseRef());
        mEvents.push_back(prClosed);
    }
}

void ProcessMiningTransformer::addCommits(const PullRequest& pr, const std::vector<Commit>& commits) {
    auto it = mPRCaseIdMap.find(pr.getNumber());
    if (it == mPRCaseIdMap.end()) {
        addPullRequest(pr);
        it = mPRCaseIdMap.find(pr.getNumber());
    }
    std::string caseId = it->second;
    std::string author = pr.getAuthor().getLogin();

    for (const auto& commit : commits) {
        Event commitEvent;
        commitEvent.setCaseId(caseId);
        commitEvent.setActivity("Commit");
        commitEvent.setTimestamp(commit.getDate());
        commitEvent.setPRNumber(std::to_string(pr.getNumber()));
        commitEvent.setPRTitle(pr.getTitle());
        commitEvent.setAuthor(author);
        commitEvent.setBranch(pr.getHeadRef());
        commitEvent.setCommitSHA(commit.getSHA());
        mEvents.push_back(commitEvent);
    }
}

void ProcessMiningTransformer::addWorkflowRuns(const std::vector<WorkflowRun>& runs) {
    for (const auto& run : runs) {
        std::string caseId = "WORKFLOW_" + std::to_string(run.getId());

        Event runEvent;
        runEvent.setCaseId(caseId);
        runEvent.setActivity("Workflow Started");
        runEvent.setTimestamp(run.getRunStartedAt());
        runEvent.setWorkflowName(run.getName());
        runEvent.setWorkflowStatus(run.getStatus());
        runEvent.setWorkflowConclusion(run.getConclusion());
        runEvent.setDurationMs(run.getDurationMs());
        mEvents.push_back(runEvent);

        if (run.getConclusion() != "") {
            Event runCompleted;
            runCompleted.setCaseId(caseId);
            runCompleted.setActivity("Workflow Completed");
            runCompleted.setTimestamp(run.getUpdatedAt());
            runCompleted.setWorkflowName(run.getName());
            runCompleted.setWorkflowStatus(run.getStatus());
            runCompleted.setWorkflowConclusion(run.getConclusion());
            runCompleted.setDurationMs(run.getDurationMs());
            mEvents.push_back(runCompleted);
        }
    }
}

void ProcessMiningTransformer::addWorkflowRunsForPR(const PullRequest& pr, const std::vector<WorkflowRun>& runs) {
    auto it = mPRCaseIdMap.find(pr.getNumber());
    if (it == mPRCaseIdMap.end()) {
        addPullRequest(pr);
        it = mPRCaseIdMap.find(pr.getNumber());
    }
    std::string caseId = it->second;

    for (const auto& run : runs) {
        Event runEvent;
        runEvent.setCaseId(caseId);
        runEvent.setActivity("Workflow Run");
        runEvent.setTimestamp(run.getRunStartedAt());
        runEvent.setPRNumber(std::to_string(pr.getNumber()));
        runEvent.setPRTitle(pr.getTitle());
        runEvent.setWorkflowName(run.getName());
        runEvent.setWorkflowStatus(run.getStatus());
        runEvent.setWorkflowConclusion(run.getConclusion());
        runEvent.setDurationMs(run.getDurationMs());
        mEvents.push_back(runEvent);
    }
}

std::vector<Event> ProcessMiningTransformer::getEvents() const {
    return mEvents;
}

void ProcessMiningTransformer::clearEvents() {
    mEvents.clear();
    mPRCaseIdMap.clear();
}

void ProcessMiningTransformer::sortEventsByTimestamp() {
    std::sort(mEvents.begin(), mEvents.end(),
        [](const Event& a, const Event& b) {
            return a.getTimestamp() < b.getTimestamp();
        });
}

void ProcessMiningTransformer::transformToCSV(const std::string& filename) {
    sortEventsByTimestamp();
    CSVWriter writer(filename);
    writer.writeEvents(mEvents);
}

void ProcessMiningTransformer::transformToMultipleCSV(const std::string& baseFilename) {
    sortEventsByTimestamp();
    CSVWriter writer(baseFilename + "_all.csv");
    writer.writeEvents(mEvents);

    std::map<std::string, std::vector<Event>> eventsByCase;
    for (const auto& event : mEvents) {
        eventsByCase[event.getCaseId()].push_back(event);
    }

    for (const auto& [caseId, caseEvents] : eventsByCase) {
        CSVWriter caseWriter(baseFilename + "_" + caseId + ".csv");
        caseWriter.writeEvents(caseEvents);
    }
}
