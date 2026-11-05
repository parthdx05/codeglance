#pragma once
#ifndef HELPER_HPP
#define HELPER_HPP
#include <iostream>
#include <string>
#include <vector>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <date/date.h>

using Timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

template<typename Obj, typename T>
void setIfNotNull(Obj& obj, const nlohmann::json& json,
		const std::string& key, void (Obj::*setter)(T)) {
    if (json.contains(key) && !json[key].is_null()) {
        (obj.*setter)(json[key]);
    }
}

template<typename Obj, typename T, typename Converter>
void setIfNotNull(Obj& obj, const nlohmann::json& json,
		const std::string& key, void (Obj::*setter)(T), Converter converter) {
    if (json.contains(key) && !json[key].is_null()) {
        (obj.*setter)(converter(json[key]));
    }
}

namespace helper {
	inline Timestamp iso8601totp(const std::string& time) {
		date::sys_time<std::chrono::milliseconds> tp;
		std::istringstream ss(time);
		date::from_stream(ss,  "%FT%TZ", tp);
		if (ss.fail()) {
			std::cerr << "Failed to parse ISO 8601 string\n";
		}
		return
			std::chrono::time_point<std::chrono::system_clock,
				std::chrono::milliseconds>(tp);
	}
};
#endif
