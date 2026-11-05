#include "Label.hpp"

Label::Label(nlohmann::json object) {
	if (object.is_null()) return;
	mId = object.value("id", "");
	mName = object.value("name", "");
	mColor = object.value("color", "");
	mDescription = object.value("description", "");
}

Label::~Label() {}
