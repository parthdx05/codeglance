#pragma once
#include "User.hpp"

#include "helper.hpp"

class CommitAuthor {
	private:
		std::string mName;
		std::string mEmail;
		Timestamp mCommittedDate;
	public:
		CommitAuthor();
		CommitAuthor(nlohmann::json object);
		~CommitAuthor();

		Timestamp getCommittedDate() const {
			return mCommittedDate;
		}
		std::string getName() const { return mName; }
		std::string getEmail() const { return mEmail; }
};

class Commit {
	private:
		std::string mSHA;
		Timestamp mDate;
		std::string mMessage;
		CommitAuthor mAuthor;
	public:
		Commit();
		Commit(nlohmann::json object);
		~Commit();

		Timestamp getDate() const { return mDate; }
		std::string getSHA() const { return mSHA; }
		std::string getMessage() const { return mMessage; }
		CommitAuthor getAuthor() const { return mAuthor; }
};
