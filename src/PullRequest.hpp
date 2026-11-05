#pragma once
#include "User.hpp"
#include "Label.hpp"
#include "Commit.hpp"

#include "helper.hpp"

class PullRequest {
	private:
		long mNumber;
		std::string mId, mTitle;

		User mAuthor;

		std::string mBaseRef, mHeadRef;

		Timestamp mCreatedAt, mMergedAt, mClosedAt;
		User mCreatedBy, mMergedBy, mClosedBy;

		std::string mState;
		bool mDraft;

		std::vector<User> mAssignees;
		std::vector<User> mRequestedReviewers;

		std::string mUrl, mCommentsUrl, mCommitsUrl;

		std::vector<Label> mLabels;

		std::vector<Commit> mCommits;

		long mNumCommits;
		long mAdditions;
		long mDeletions;
		long mChangedFiles;

	public:
		PullRequest() {};
		~PullRequest() {};

		// GETTERS
		long getNumber() const { return mNumber; }
		long getNumCommits() const { return mNumCommits; }
		long getAdditions() const { return mAdditions; }
		long getDeletions() const { return mDeletions; }
		long getChangedFiles() const { return mChangedFiles; }
		std::string getId() const { return mId; }
		std::string getTitle() const { return mTitle; }
		User getAuthor() const { return mAuthor; }
		std::string getBaseRef() const { return mBaseRef; }
		std::string getHeadRef() const { return mHeadRef; }
		Timestamp getCreatedAt() const { return mCreatedAt; }
		Timestamp getMergedAt() const { return mMergedAt; }
		Timestamp getClosedAt() const { return mClosedAt; }
		User getCreatedBy() const { return mCreatedBy; }
		User getMergedBy() const { return mMergedBy; }
		User getClosedBy() const { return mClosedBy; }
		std::string getState() const { return mState; }
		bool getDraft() const { return mDraft; }
		std::vector<User> getAssignees() const { return mAssignees; }
		std::vector<User> getRequestedReviewers() const { return mRequestedReviewers; }
		std::vector<Commit> getCommits() const { return mCommits; }
		std::string getUrl() const { return mUrl; }
		std::string getCommentsUrl() const { return mCommentsUrl; }
		std::string getCommitsUrl() const { return mCommitsUrl; }
		std::vector<Label> getLabels() const { return mLabels ; }

		// SETTERS
		void setId(std::string id) { mId = id; }
		void setNumber(long number) { mNumber = number; }
		void setNumCommits(long numCommits) { mNumCommits = numCommits; }
		void setAdditions(long additions) { mAdditions = additions; }
		void setDeletions(long deletions) { mDeletions = deletions; }
		void setChangedFiles(long changedFiles) { mChangedFiles = changedFiles; }
		void setTitle(std::string title) { mTitle = title; }
		void setAuthor(User user) { mAuthor = user; }
		void setBaseRef(std::string ref) { mBaseRef = ref; }
		void setHeadRef(std::string ref) { mHeadRef = ref; }
		void setCreatedAt(Timestamp time) {
			mCreatedAt = time;
		}
		void setMergedAt(Timestamp time) {
			mMergedAt = time;
		}
		void setClosedAt(Timestamp time) {
			mClosedAt = time;
		}
		void setCreatedBy(User usr) { mCreatedBy = usr; }
		void setMergedBy(User usr) { mMergedBy = usr; }
		void setClosedBy(User usr) { mClosedBy = usr; }
		void setState(std::string state) { mState = state; }
		void setDraft(bool draft) { mDraft = draft; }
		void setAssignees(std::vector<User> assignees) { mAssignees = assignees; }
		void setRequestedReviewers(std::vector<User> requestedReviewers) {
			mRequestedReviewers = requestedReviewers;
		}
		void setLabels(std::vector<Label> labels) { mLabels = labels; }
		void setCommits(std::vector<Commit> commits) { mCommits = commits; }
		void setUrl(std::string url) { mUrl = url; }
		void setCommentsUrl(std::string url) { mCommentsUrl = url; }
		void setCommitsUrl(std::string url) { mCommitsUrl = url; }

		void print_name();
};
