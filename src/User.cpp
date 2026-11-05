#include "User.hpp"

User::User() {

}

User::User(nlohmann::json object) {
	if (object.is_null()) return;
	mLogin = object.value("login", "");
	mId = object.value("id", 0);
	mName = object.value("name", "");
	mEmail = object.value("email", "");
}

User::~User() {

}
