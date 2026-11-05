#pragma once
#include <string>
#include <chrono>
#include <vector>
#include "helper.hpp"

class Event {
private:
    std::string mCaseId;
    std::string mActivity;
    Timestamp mTimestamp;
    std::string mPRNumber;
    std::string mPRTitle;
    std::string mAuthor;
    std::string mBranch;
    std::string mCommitSHA;
    std::string mWorkflowName;
    std::string mWorkflowStatus;
    std::string mWorkflowConclusion;
    long mDurationMs;
    long mAdditions;
    long mDeletions;
    long mChangedFiles;

public:
    Event();
    Event(std::string caseId, std::string activity,
          std::chrono::time_point<std::chrono::milliseconds> timestamp,
          std::string prNumber = "", std::string prTitle = "",
          std::string author = "", std::string branch = "",
          std::string commitSHA = "", std::string workflowName = "",
          std::string workflowStatus = "", std::string workflowConclusion = "",
          long durationMs = 0, long additions = 0, long deletions = 0,
          long changedFiles = 0);
    ~Event();

    std::string getCaseId() const;
    std::string getActivity() const;
    Timestamp getTimestamp() const ;
    std::string getPRNumber() const;
    std::string getPRTitle() const;
    std::string getAuthor() const;
    std::string getBranch() const;
    std::string getCommitSHA() const;
    std::string getWorkflowName() const;
    std::string getWorkflowStatus() const;
    std::string getWorkflowConclusion() const;
    long getDurationMs() const;
    long getAdditions() const;
    long getDeletions() const;
    long getChangedFiles() const;

    void setCaseId(std::string caseId);
    void setActivity(std::string activity);
    void setTimestamp(Timestamp timestamp);
    void setPRNumber(std::string prNumber);
    void setPRTitle(std::string prTitle);
    void setAuthor(std::string author);
    void setBranch(std::string branch);
    void setCommitSHA(std::string commitSHA);
    void setWorkflowName(std::string workflowName);
    void setWorkflowStatus(std::string workflowStatus);
    void setWorkflowConclusion(std::string workflowConclusion);
    void setDurationMs(long durationMs);
    void setAdditions(long additions);
    void setDeletions(long deletions);
    void setChangedFiles(long changedFiles);

    std::string toCSVRow() const;
    static std::vector<std::string> getCSVHeaders();
};
