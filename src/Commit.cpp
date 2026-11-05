#include "Commit.hpp"

Commit::Commit() {

}

Commit::Commit(nlohmann::json object) {
	if (!object.is_null() && object.contains("commit")) {
		mSHA = object["commit"].value("oid", "");
		mAuthor = CommitAuthor(object["commit"]["author"]);
		mMessage = object["commit"].value("message", "");
		mDate = helper::iso8601totp(object["commit"]["authoredDate"]);
	}
}

Commit::~Commit() {

}

CommitAuthor::CommitAuthor(nlohmann::json object) {
	if (object.is_null()) return;
	mName = object.value("name", "");
	mEmail = object.value("email", "");
	mCommittedDate = helper::iso8601totp(object["date"]);
}

CommitAuthor::CommitAuthor() {}

CommitAuthor::~CommitAuthor() {}
