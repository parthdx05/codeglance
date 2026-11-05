#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>
#include "User.hpp"

class WorkflowRun {
	private:
		long mId;
		std::string mName;
		std::string mHeadSha;
		std::string mEvent;
		std::string mStatus;
		std::string mConclusion;
		int mRunAttempt;
		Timestamp mRunStartedAt;
		Timestamp mCreatedAt;
		Timestamp mUpdatedAt;
		User mTriggeringActor;
		long mDurationMs;
		std::string mHtmlUrl;
		std::string mPullRequestsUrl;
	public:
    WorkflowRun() {}
    ~WorkflowRun() {}

		long getId() const { return mId; }
    std::string getName() const { return mName; }
    std::string getHeadSha() const { return mHeadSha; }
    std::string getEvent() const { return mEvent; }
    std::string getStatus() const { return mStatus; }
    std::string getConclusion() const { return mConclusion; }
    int getRunAttempt() const { return mRunAttempt; }
    Timestamp getRunStartedAt() const { return mRunStartedAt; }
    Timestamp getCreatedAt() const { return mCreatedAt; }
    Timestamp getUpdatedAt() const { return mUpdatedAt; }
    User getTriggeringActor() const { return mTriggeringActor; }
    long getDurationMs() const { return mDurationMs; }
    std::string getHtmlUrl() const { return mHtmlUrl; }
    std::string getPullRequestsUrl() const { return mPullRequestsUrl; }

    // SETTERS
    void setId(long id) { mId = id; }
    void setName(const std::string& name) { mName = name; }
    void setHeadSha(const std::string& sha) { mHeadSha = sha; }
    void setEvent(const std::string& event) { mEvent = event; }
    void setStatus(const std::string& status) { mStatus = status; }
    void setConclusion(const std::string& conclusion) { mConclusion = conclusion; }
    void setRunAttempt(int attempt) { mRunAttempt = attempt; }
    void setRunStartedAt(const Timestamp& time){
			mRunStartedAt = time;
		}
    void setCreatedAt(const Timestamp& time) {
			mCreatedAt = time;
		}
    void setUpdatedAt(const Timestamp& time) {
			mUpdatedAt = time;
		}
    void setTriggeringActor(const User& actor) { mTriggeringActor = actor; }
    void setDurationMs(long duration) { mDurationMs = duration; }
    void setHtmlUrl(const std::string& url) { mHtmlUrl = url; }
    void setPullRequestsUrl(const std::string& url) { mPullRequestsUrl = url; }
};
