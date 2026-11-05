cmake_minimum_required(VERSION 4.0)

project(CodeGlance)
set(CMAKE_CXX_STANDARD 20)

add_executable(glance)

target_sources(glance
	PRIVATE
		src/Label.cpp
		src/User.cpp
		src/Commit.cpp
		src/PullRequest.cpp
		src/Github.cpp
		src/main.cpp
		src/Event.cpp
		src/CSVWriter.cpp
		src/ProcessMiningTransformer.cpp
		src/ProcessMiner.cpp
		src/ProcessModel.cpp
		src/Visualizer.cpp
)

# DEPENDENCIES
include(FetchContent)
FetchContent_Declare(
	cpr
	GIT_REPOSITORY	"https://github.com/libcpr/cpr.git"
	GIT_TAG					"1.12.0"
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY	"https://github.com/d99kris/rapidcsv.git"
  GIT_TAG					"v9.07"
)
FetchContent_MakeAvailable(rapidcsv)

FetchContent_Declare(
	json
	URL 						"https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz"
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
	date
	GIT_REPOSITORY 	"https://github.com/HowardHinnant/date"
	GIT_TAG					"v3.0.5"
)
FetchContent_MakeAvailable(date)

FetchContent_Declare(
	matplotplusplus
	GIT_REPOSITORY    "https://github.com/alandefreitas/matplotplusplus.git"
	GIT_TAG           "origin/master"
)
FetchContent_GetProperties(matplotplusplus)
FetchContent_MakeAvailable(matplotplusplus)
execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/patch/patch-matplotpp.sh ${matplotplusplus_SOURCE_DIR}
				#WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

target_link_libraries(glance
	PRIVATE
		cpr::cpr
		rapidcsv
		nlohmann_json::nlohmann_json
		date
		matplot
)
