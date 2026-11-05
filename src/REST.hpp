#pragma once
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "helper.hpp"

namespace REST {
		static nlohmann::json get(std::string url, std::string token, bool dump = true) {
			nlohmann::json response;
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
				response = nlohmann::json::parse(r.text);

				if (dump) {
					std::ofstream outResponse("rest_response.json");
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
