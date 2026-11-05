#pragma once
#include <string>
#include <iostream>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>


class GithubHandlerGQL {
	std::string mOwner;
	std::string mRepo;
	public:
	GithubHandlerGQL(std::string owner, std::string repo);
	void FetchData(std::string token);
	void FetchActions(std::string token);
};
