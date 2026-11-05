#include "Github.hpp"

// Endpoints
// "https://api.github.com/graphql"
// std::string url = "https://api.github.com/repos/" + mOwner + "/" + mRepo + "/actions/runs";

using namespace Github;

Repository::Repository(std::string owner, std::string name, std::string token) {
	mOwner = owner;
	mName = name;
	mToken = token;
}

std::vector<PullRequest> Repository::getPullRequests() {
	std::vector<PullRequest> PRs;
	std::string url = "https://api.github.com/graphql";

	nlohmann::json payload;
	payload["query"] = R"(query {
  repository(owner: ")" + mOwner + R"(", name: ")" + mName + R"(")
	{pullRequests(last:100){
		pageInfo{hasNextPage endCursor}
		edges{node{
			id number title
			createdAt author{login}
			mergedAt mergedBy{login}
			closedAt
			headRefName headRefOid baseRefName baseRefOid
			state isDraft
			additions deletions changedFiles
			mergeCommit{oid}
			assignees(first:100){edges{node{login name email}}}
			labels(first:100){edges{node{id name description color}}}
			commits(first:250){
				totalCount
				edges{node{
					commit{url oid message committedDate author{name email}}}}}}}}}}
)";

	try
	{
		nlohmann::json response = GraphQL::query(url, payload, mToken);

		if (!response.is_null()) {
			nlohmann::json PRarr = response["data"]["repository"]["pullRequests"]["edges"];
			for (const auto& PRjson : PRarr) {
				PullRequest out;
				nlohmann::json node = PRjson["node"];
				setIfNotNull(out, node, "id", &PullRequest::setId);
				setIfNotNull(out, node, "number", &PullRequest::setNumber);
				setIfNotNull(out, node, "title", &PullRequest::setTitle);
				out.setAuthor(User(node["author"]));

				setIfNotNull(out, node, "createdAt", &PullRequest::setCreatedAt, helper::iso8601totp);
				setIfNotNull(out, node, "mergedAt", &PullRequest::setMergedAt, helper::iso8601totp);
				out.setMergedBy(User(node["mergedBy"]));
				setIfNotNull(out, node, "closedAt", &PullRequest::setClosedAt, helper::iso8601totp);

				// TODO: change this to support OID?
				setIfNotNull(out, node, "headRefName", &PullRequest::setHeadRef);
				setIfNotNull(out, node, "baseRefName", &PullRequest::setBaseRef);

				setIfNotNull(out, node, "state", &PullRequest::setState);
				setIfNotNull(out, node, "isDraft", &PullRequest::setDraft);

				setIfNotNull(out, node, "additions", &PullRequest::setAdditions);
				setIfNotNull(out, node, "deletions", &PullRequest::setDeletions);
				setIfNotNull(out, node, "changedFiles", &PullRequest::setChangedFiles);
				setIfNotNull(out, node["commits"], "totalCount", &PullRequest::setNumCommits);

				std::vector<Label> labels;
				for (const auto& label: node["labels"]["edges"]) {
					labels.push_back(Label(label));
				}
				out.setLabels(labels);

				std::vector<User> assigned;
				for (const auto& assignee: node["assignees"]["edges"]) {
					assigned.push_back(User(assignee));
				}
				out.setAssignees(assigned);

				std::vector<Commit> commits;
				for (const auto& commit: node["commits"]["edges"]) {
					commits.push_back(Commit(commit));
				}
				out.setCommits(commits);

				// std::vector<User> mRequestedReviewers;

				PRs.push_back(out);
			}
		}
	} catch (const std::exception& e) {
			std::cerr << "Error fetching Pull Requests: " << e.what() << std::endl;
	}
	return PRs;
}

std::vector<WorkflowRun> Repository::getWorkflowRuns() {
	std::vector<WorkflowRun> runs;
	std::string url = "https://api.github.com/repos/" + mOwner + "/" + mName + "/actions/runs";

	try {
			nlohmann::json response = REST::get(url, mToken);
			if (response.is_null()) return runs;

			if (response.is_object() && response.contains("workflow_runs")) {
					for (const auto& node : response["workflow_runs"]) {
						if (node.is_null()) break;
						WorkflowRun run;
						setIfNotNull(run, node, "id", &WorkflowRun::setId);
						setIfNotNull(run, node, "name", &WorkflowRun::setName);
						setIfNotNull(run, node, "sha", &WorkflowRun::setHeadSha);
						setIfNotNull(run, node, "event", &WorkflowRun::setEvent);
						setIfNotNull(run, node, "status", &WorkflowRun::setStatus);
						setIfNotNull(run, node, "conclusion", &WorkflowRun::setConclusion);
						setIfNotNull(run, node, "run_attempt", &WorkflowRun::setRunAttempt);
						setIfNotNull(run, node, "run_started_at", &WorkflowRun::setRunStartedAt, helper::iso8601totp);
						setIfNotNull(run, node, "created_at", &WorkflowRun::setCreatedAt, helper::iso8601totp);
						setIfNotNull(run, node, "updated_at", &WorkflowRun::setUpdatedAt, helper::iso8601totp);
						setIfNotNull(run, node, "duration", &WorkflowRun::setDurationMs);
						setIfNotNull(run, node, "html_url", &WorkflowRun::setHtmlUrl);
						setIfNotNull(run, node, "pull_requests_url", &WorkflowRun::setPullRequestsUrl);
						if (node.contains("actor") && !node["actor"].is_null()) {
								run.setTriggeringActor(User(node["actor"]));
						}
						runs.push_back(run);
					}
			}
	} catch (const std::exception& e) {
			std::cerr << "Error fetching workflow runs: " << e.what() << std::endl;
	}

	return runs;
}
