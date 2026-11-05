#include "GraphQL.hpp"
#include "REST.hpp"
#include "PullRequest.hpp"
#include "WorkflowRun.hpp"

namespace Github {
	class Repository {
		private:
			std::string mOwner;
			std::string mName;
			std::string mToken;
		public:
			Repository(std::string owner, std::string name, std::string token);
			~Repository() {}
			std::vector<PullRequest> getPullRequests();
			std::vector<WorkflowRun> getWorkflowRuns();
	};
};
