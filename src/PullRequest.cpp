#include "./PullRequest.hpp"

// METHOD DEFINITIONS
void PullRequest::print_name() {
	std::cout << mId << " " << mNumber << " " << mTitle << std::endl;
}
