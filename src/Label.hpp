#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include "helper.hpp"

class Label {
	private:
		std::string mName, mId, mColor, mDescription;
	public:
		Label(nlohmann::json object);
		~Label();
};
