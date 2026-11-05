#pragma once
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

class User {
	private:
		std::string mLogin;
		std::string mName;
		std::string mEmail;
		long mId;
		// std::string mUrl;
	public:
		User();
		User(nlohmann::json object);
		~User();
		std::string getLogin() { return mLogin; }
		std::string getName() { return mName; }
		std::string getEmail() { return mEmail; }
		long getId() { return mId; }
};
