#pragma once
#include <iostream>
#include <string>

#include <cpr/cpr.h>

#include "./PullRequest.hpp"

class GithubHandler {
	private:
		std::string mOwner;
		std::string mRepo;
	public:
		GithubHandler(std::string owner, std::string repo);
		~GithubHandler();
		void FetchPRs();
		void FetchPRMetadata(long pr_num);
};
