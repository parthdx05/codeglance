#include "github.hpp"

void check_connection () {
	cpr::Response r = cpr::Get(
		cpr::Url{"https://api.github.com/octocat"},
		cpr::Header{
			{"Accept", "application/vnd.github+json"},
			{"X-GitHub-Api-Version", "2022-11-28"}
		}
	);

	if (r.status_code == 200) {
		std::cout << "OK" << std::endl;
	}
}

GithubHandler::GithubHandler(std::string owner, std::string repo) {
	mOwner = owner;
	mRepo = repo;
	check_connection();
}

GithubHandler::~GithubHandler() {}

void GithubHandler::FetchPRs() {
	std::string url = "https://api.github.com/repos/"
		+ mOwner + "/" + mRepo + "/pulls?state=all";

	std::cout << url << std::endl;

	cpr::Response r = cpr::Get(
		cpr::Url{url},
		cpr::Header{
			// {},
			{"Accept", "application/vnd.github+json"},
			{"X-GitHub-Api-Version", "2022-11-28"}
		}
	);
	if (r.status_code == 200) {
		std::cout << "OK" << std::endl;
		nlohmann::json object = nlohmann::json::parse(r.text);
		for (nlohmann::json pr : object) {
			PullRequest preq(pr);
			preq.print_name();
			FetchPRMetadata(preq.getNumber());
		}
	}
}

void GithubHandler::FetchPRMetadata(long pr_num) {
	std::string url = "https://api.github.com/repos/"
		+ mOwner + "/" + mRepo + "/pulls/"
		+ std::to_string(pr_num);

	std::cout << url << std::endl;

	cpr::Response r = cpr::Get(
		cpr::Url{url},
		cpr::Header{
			// {},
			{"Accept", "application/vnd.github+json"},
			{"X-GitHub-Api-Version", "2022-11-28"}
		}
	);

	if (r.status_code == 200) {
		std::cout << "OK" << std::endl;
		nlohmann::json object = nlohmann::json::parse(r.text);
		PullRequestMetadata prm(object);
		prm.print_data();
	}
}
