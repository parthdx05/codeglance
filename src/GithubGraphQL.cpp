#include "GithubGraphQL.hpp"

#include <fstream>


GithubHandlerGQL::GithubHandlerGQL(std::string owner, std::string repo) {
	mOwner = owner;
	mRepo = repo;
	// check_connection();
}

void GithubHandlerGQL::FetchData(std::string token) {
	nlohmann::json payload;
	payload["query"] = R"(query
	{repository(owner: ")" + mOwner + R"(",name:")" + mRepo + R"(")
	{pullRequests(last:100){
		pageInfo{hasNextPage endCursor}
		edges{node{
			id number title createdAt author{login} mergedAt mergedBy{login} closedAt
			headRefName headRefOid baseRefName baseRefOid state isDraft
			additions deletions changedFiles mergeCommit{oid}
			assignees(first:100){edges{node{login name email}}}
			labels(first:100){edges{node{name description color}}}
			commits(first:250){
				totalCount
				edges{node{commit{url oid message committedDate author{name email}}}}}}}}}}
)";

	cpr::Response r = cpr::Post(
			cpr::Url{"https://api.github.com/graphql"},
			cpr::Bearer{token},
			cpr::Header{
				{"Accept", "application/vnd.github+json"},
				{"X-GitHub-Api-Version", "2022-11-28"}
			},
			cpr::Body{payload.dump()}
	);

	if (r.status_code == 200) {
		std::cout << "OK" << std::endl;
		// std::cout << r.text << std::endl;

		nlohmann::json response = nlohmann::json::parse(r.text);

		std::ofstream outResponse("response.json");
		outResponse << response.dump(1, '\t');
		outResponse.close();
	}
	else {
		std::cout << std::to_string(r.status_code) << std::endl;
		std::cout << r.text << std::endl;
	}
}

void GithubHandlerGQL::FetchActions(std::string token) {
	std::string url = "https://api.github.com/repos/" + mOwner + "/" + mRepo + "/actions/runs";
	std::cout << url << std::endl;
	cpr::Response r = cpr::Get(
		cpr::Url{url},
		cpr::Bearer{token},
		cpr::Header{
			{"Accept", "application/vnd.github+json"},
			{"X-GitHub-Api-Version", "2022-11-28"}
		}
	);
	if (r.status_code == 200) {
		std::cout << "OK" << std::endl;
		nlohmann::json object = nlohmann::json::parse(r.text);
		std::ofstream outResponse("actions.json");
		outResponse << object.dump(1, '\t');
		outResponse.close();
	}
}
