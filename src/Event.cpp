#include "Event.hpp"
#include "helper.hpp"
#include <sstream>
#include <iomanip>
#include <date/date.h>

Event::Event() {}
Event::~Event() {}

std::string Event::getCaseId() const { return mCaseId; }
std::string Event::getActivity() const { return mActivity; }
Timestamp Event::getTimestamp() const { return mTimestamp; }
std::string Event::getPRNumber() const { return mPRNumber; }
std::string Event::getPRTitle() const { return mPRTitle; }
std::string Event::getAuthor() const { return mAuthor; }
std::string Event::getBranch() const { return mBranch; }
std::string Event::getCommitSHA() const { return mCommitSHA; }
std::string Event::getWorkflowName() const { return mWorkflowName; }
std::string Event::getWorkflowStatus() const { return mWorkflowStatus; }
std::string Event::getWorkflowConclusion() const { return mWorkflowConclusion; }
long Event::getDurationMs() const { return mDurationMs; }
long Event::getAdditions() const { return mAdditions; }
long Event::getDeletions() const { return mDeletions; }
long Event::getChangedFiles() const { return mChangedFiles; }

void Event::setCaseId(std::string caseId) { mCaseId = caseId; }
void Event::setActivity(std::string activity) { mActivity = activity; }
void Event::setTimestamp(Timestamp timestamp) { mTimestamp = timestamp; }
void Event::setPRNumber(std::string prNumber) { mPRNumber = prNumber; }
void Event::setPRTitle(std::string prTitle) { mPRTitle = prTitle; }
void Event::setAuthor(std::string author) { mAuthor = author; }
void Event::setBranch(std::string branch) { mBranch = branch; }
void Event::setCommitSHA(std::string commitSHA) { mCommitSHA = commitSHA; }
void Event::setWorkflowName(std::string workflowName) { mWorkflowName = workflowName; }
void Event::setWorkflowStatus(std::string workflowStatus) { mWorkflowStatus = workflowStatus; }
void Event::setWorkflowConclusion(std::string workflowConclusion) { mWorkflowConclusion = workflowConclusion; }
void Event::setDurationMs(long durationMs) { mDurationMs = durationMs; }
void Event::setAdditions(long additions) { mAdditions = additions; }
void Event::setDeletions(long deletions) { mDeletions = deletions; }
void Event::setChangedFiles(long changedFiles) { mChangedFiles = changedFiles; }

std::string Event::toCSVRow() const {
    auto duration = mTimestamp.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    std::ostringstream oss;
    oss << std::quoted(mCaseId) << ","
        << std::quoted(mActivity) << ","
        << millis << ","
        << std::quoted(mPRNumber) << ","
        << std::quoted(mPRTitle) << ","
        << std::quoted(mBranch) << ","
        << std::quoted(mAuthor) << ","
        << std::quoted(mCommitSHA) << ","
        << std::quoted(mWorkflowName) << ","
        << std::quoted(mWorkflowStatus) << ","
        << std::quoted(mWorkflowConclusion) << ","
        << mAdditions << ","
        << mDeletions << ","
        << mChangedFiles << ","
        << mDurationMs;
    return oss.str();
}

std::vector<std::string> Event::getCSVHeaders() {
    return {
        "Case ID", "Activity", "Timestamp",
        "PR Number", "PR Title", "Author",
        "Branch", "Commit SHA", "Workflow Name",
        "Workflow Status", "Workflow Conclusion",
        "Duration (ms)", "Additions", "Deletions", "Changed Files"
    };
}
