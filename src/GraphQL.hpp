#pragma once
#include "helper.hpp"

namespace GraphQL {
	static nlohmann::json query(std::string url, nlohmann::json payload, std::string token, bool dump = true) {
		nlohmann::json response;
		cpr::Response r = cpr::Post(
				cpr::Url{url},
				cpr::Bearer{token},
				cpr::Header{
					{"Accept", "application/vnd.github+json"},
					{"X-GitHub-Api-Version", "2022-11-28"}
				},
				cpr::Body{payload.dump()}
		);

		if (r.status_code == 200) {
			std::cout << "OK" << std::endl;

			response = nlohmann::json::parse(r.text);

			if (dump) {
				std::ofstream outResponse("gql_response.json");
				outResponse << response.dump(1, '\t');
				outResponse.close();
			}
		}
		else {
			std::cerr << "ERROR: HTTP " << r.status_code << " for " << url << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw std::runtime_error("HTTP error: " + std::to_string(r.status_code));
			// std::cout << std::to_string(r.status_code) << " " << url << std::endl;
			// std::cout << r.text << std::endl;
		}
		return response;
	}
}
