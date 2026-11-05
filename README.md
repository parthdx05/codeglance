# CodeGlance
A C++20 command-line tool that fetches raw Github repository activity and converts it into actionable software engineering insights.

# Build Instructions
## Pre-requisites
- Linux/Unix machine with GNU bash
- GNU Make
- CMake 4.0+
- C++ compiler with C++20 support (GNU GCC/LLVM Clang)

## Commands
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
make -C build
```

# Usage
## Pre-requisites
- [Github Personal Access Token](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens)

## Command
```sh
./glance <github_token> [owner] [repo]
```
**Note:** For demonstration purposes defaults are:
-	owner = "microsoft"
- repo = "vscode"

# Features
- DORA metrics (with benchmark bands)
	- Deployment Frequency — deploys per day, from workflow-run history
	- Lead Time for Changes — mean mergedAt − createdAt (hours) over merged PRs
	- Change Failure Rate — failed / cancelled / timed-out runs as a percentage, with a 15% benchmark trend line
	- Mean Time to Recovery — mean updatedAt − runStartedAt over failed runs
- Development metrics
	- PR status distribution (open / closed / merged)
	- Commits per PR, additions / deletions totals, code churn ratio
	- Average PR cycle time, with distribution histograms
	- Code churn over time (weekly additions vs. deletions)
- Process mining
	- Activity frequency ranking from the repository event stream
	- Directly-follows transition matrix, rendered as a heatmap
	- Bottleneck detection — where your process concentrates and stalls
	- Circular process-flow graph of the discovered process model
	- 7×24 activity heatmap (day-of-week × hour) and event timeline
- Output
	-	Save function that exports the corresponding plot figure to an image file
