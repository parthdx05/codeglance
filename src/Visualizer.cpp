#include "Visualizer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <ctime>
#include <iomanip>
#include <sstream>

DORAMetrics Visualizer::calculateDORAMetrics(
    const std::vector<Event>& events,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::vector<PullRequest>& pullRequests
) {
    DORAMetrics metrics = {0.0, 0.0, 0.0, 0.0, 0, 0, 0};

    if (workflowRuns.empty() && pullRequests.empty()) {
        return metrics;
    }


    if (!workflowRuns.empty()) {
        metrics.totalDeployments = workflowRuns.size();
        metrics.failedDeployments = std::count_if(workflowRuns.begin(), workflowRuns.end(),
            [](const WorkflowRun& run) {
                return run.getConclusion() == "failure" ||
                       run.getConclusion() == "cancelled" ||
                       run.getConclusion() == "timed_out";
            });

        metrics.successfulDeployments = metrics.totalDeployments - metrics.failedDeployments;


        auto minTime = workflowRuns[0].getRunStartedAt();
        auto maxTime = workflowRuns[0].getRunStartedAt();
        for (const auto& run : workflowRuns) {
            if (run.getRunStartedAt() < minTime) minTime = run.getRunStartedAt();
            if (run.getRunStartedAt() > maxTime) maxTime = run.getRunStartedAt();
        }

        auto duration = std::chrono::duration_cast<std::chrono::hours>(maxTime - minTime).count();
        if (duration > 0) {
            metrics.deploymentFrequency = static_cast<double>(metrics.totalDeployments) / (duration / 24.0);
        }
    }


    if (!pullRequests.empty()) {
        std::vector<double> leadTimes;
        for (const auto& pr : pullRequests) {
            if (pr.getState() == "closed" && pr.getMergedAt().time_since_epoch().count() != 0) {
                auto leadTime = std::chrono::duration_cast<std::chrono::hours>(
                    pr.getMergedAt() - pr.getCreatedAt()
                ).count();
                if (leadTime > 0) {
                    leadTimes.push_back(leadTime);
                }
            }
        }

        if (!leadTimes.empty()) {
            metrics.leadTimeForChanges = std::accumulate(leadTimes.begin(), leadTimes.end(), 0.0) / leadTimes.size();
        }
    }


    if (metrics.totalDeployments > 0) {
        metrics.changeFailureRate = (static_cast<double>(metrics.failedDeployments) / metrics.totalDeployments) * 100.0;
    }


    if (metrics.failedDeployments > 0) {
        std::vector<double> recoveryTimes;
        for (const auto& run : workflowRuns) {
            if (run.getConclusion() == "failure" || run.getConclusion() == "cancelled") {
                if (run.getUpdatedAt().time_since_epoch().count() != 0 &&
                    run.getRunStartedAt().time_since_epoch().count() != 0) {
                    auto recoveryTime = std::chrono::duration_cast<std::chrono::hours>(
                        run.getUpdatedAt() - run.getRunStartedAt()
                    ).count();
                    if (recoveryTime > 0) {
                        recoveryTimes.push_back(recoveryTime);
                    }
                }
            }
        }

        if (!recoveryTimes.empty()) {
            metrics.meanTimeToRecovery = std::accumulate(recoveryTimes.begin(), recoveryTimes.end(), 0.0) / recoveryTimes.size();
        }
    }

    return metrics;
}

DevelopmentMetrics Visualizer::calculateDevelopmentMetrics(
    const std::vector<Event>& events,
    const std::vector<PullRequest>& pullRequests,
    const std::vector<WorkflowRun>& workflowRuns
) {
    DevelopmentMetrics metrics = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};

    metrics.totalPRs = pullRequests.size();
    metrics.totalWorkflowRuns = workflowRuns.size();

    for (const auto& pr : pullRequests) {
        if (pr.getState() == "open") metrics.openPRs++;
        else if (pr.getState() == "closed") metrics.closedPRs++;
        if (pr.getMergedAt().time_since_epoch().count() != 0) metrics.mergedPRs++;

        metrics.totalAdditions += pr.getAdditions();
        metrics.totalDeletions += pr.getDeletions();
        metrics.totalChangedFiles += pr.getChangedFiles();

        metrics.totalCommits += pr.getNumCommits();
    }

    if (metrics.totalPRs > 0) {
        metrics.avgCommitsPerPR = static_cast<double>(metrics.totalCommits) / metrics.totalPRs;
    }

    if (metrics.totalDeletions > 0) {
        metrics.codeChurnRatio = static_cast<double>(metrics.totalAdditions) / metrics.totalDeletions;
    } else if (metrics.totalAdditions > 0) {
        metrics.codeChurnRatio = 1.0;
    }

    if (metrics.mergedPRs > 0) {
        std::vector<double> cycleTimes;
        for (const auto& pr : pullRequests) {
            if (pr.getState() == "closed") {
                auto endTime = (pr.getMergedAt().time_since_epoch().count() != 0) ?
                    pr.getMergedAt() : pr.getClosedAt();
                auto cycleTime = std::chrono::duration_cast<std::chrono::hours>(
                    endTime - pr.getCreatedAt()
                ).count();
                if (cycleTime > 0) {
                    cycleTimes.push_back(cycleTime);
                }
            }
        }

        if (!cycleTimes.empty()) {
            metrics.avgPRCycleTime = std::accumulate(cycleTimes.begin(), cycleTimes.end(), 0.0) / cycleTimes.size();
        }
    }

    return metrics;
}



void Visualizer::visualizeDORAMetrics(
    const std::vector<Event>& events,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    DORAMetrics metrics = calculateDORAMetrics(events, workflowRuns, pullRequests);

    std::vector<std::string> metricNames = {
        "Deployment Frequency\n(deploys/day)",
        "Lead Time for Changes\n(hours)",
        "Change Failure Rate\n(%)",
        "Mean Time to Recovery\n(hours)"
    };

    std::vector<double> metricValues = {
        metrics.deploymentFrequency,
        metrics.leadTimeForChanges,
        metrics.changeFailureRate,
        metrics.meanTimeToRecovery
    };

    std::vector<double> elite = {1.0, 1.0, 0.0, 1.0};
    std::vector<double> high = {1.0, 24.0, 15.0, 24.0};
    std::vector<double> medium = {1.0, 168.0, 30.0, 168.0};
    std::vector<double> low = {1.0, 720.0, 45.0, 720.0};

    matplot::figure(true);
    matplot::title(title);

    std::vector<double> x = {1, 2, 3, 4};
    std::vector<double> width = {0.15, 0.15, 0.15, 0.15};
    std::vector<double> elite_plot = {1+width[0], 2+width[0], 3+width[0], 4+width[0]};
    std::vector<double> high_plot = {1+2*width[0], 2+2*width[0], 3+2*width[0], 4+2*width[0]};
    std::vector<double> medium_plot = {1+3*width[0], 2+3*width[0], 3+3*width[0], 4+3*width[0]};
    std::vector<double> low_plot = {1+4*width[0], 2+4*width[0], 3+4*width[0], 4+4*width[0]};

    matplot::bar(x, metricValues)->display_name("Your Team");
    matplot::hold(matplot::on);
    matplot::bar(elite_plot, elite)->display_name("Elite");
    matplot::bar(high_plot, high)->display_name("High");
    matplot::bar(medium_plot, medium)->display_name("Medium");
    matplot::bar(low_plot, low)->display_name("Low");

    std::vector<double> ticks = {1.5, 2.5, 3.5, 4.5};
    matplot::xticks(ticks);
    matplot::xticklabels(metricNames);
    matplot::ylabel("Value");
    matplot::legend();
    matplot::grid(matplot::on);
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeDORADashboard(
    const std::vector<Event>& events,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    DORAMetrics metrics = calculateDORAMetrics(events, workflowRuns, pullRequests);
    DevelopmentMetrics devMetrics = calculateDevelopmentMetrics(events, pullRequests, workflowRuns);

    // auto subplots = matplot::subplot(2, 2, 0);

    matplot::subplot(2, 2, 0);
    std::vector<std::string> labels = {"Deployment Frequency", "Lead Time", "Change Failure Rate", "MTTR"};
    std::vector<double> values = {
        metrics.deploymentFrequency,
        metrics.leadTimeForChanges,
        metrics.changeFailureRate,
        metrics.meanTimeToRecovery
    };
    matplot::bar(values);
    matplot::xticklabels(labels);
    matplot::title("DORA Metrics");
    matplot::grid(matplot::on);

    matplot::subplot(2, 2, 1);
    std::vector<double> statusValues = {
        static_cast<double>(metrics.successfulDeployments),
        static_cast<double>(metrics.failedDeployments),
        static_cast<double>(metrics.totalDeployments - metrics.successfulDeployments - metrics.failedDeployments)
    };
    std::vector<std::string> statusLabels = {
        "Successful (" + std::to_string(metrics.successfulDeployments) + ")",
        "Failed (" + std::to_string(metrics.failedDeployments) + ")",
        "In Progress (" + std::to_string(metrics.totalDeployments - metrics.successfulDeployments - metrics.failedDeployments) + ")"
    };
    matplot::pie(statusValues, statusLabels);
    matplot::title("Deployment Status");

    matplot::subplot(2, 2, 2);
    std::vector<double> prStatusValues = {
        static_cast<double>(devMetrics.openPRs),
        static_cast<double>(devMetrics.closedPRs),
        static_cast<double>(devMetrics.mergedPRs)
    };
    std::vector<std::string> prStatusLabels = {
        "Open (" + std::to_string(devMetrics.openPRs) + ")",
        "Closed (" + std::to_string(devMetrics.closedPRs) + ")",
        "Merged (" + std::to_string(devMetrics.mergedPRs) + ")"
    };
    matplot::pie(prStatusValues, prStatusLabels);
    matplot::title("PR Status Distribution");

    matplot::subplot(2, 2, 3);
    std::vector<double> churnValues = {
        static_cast<double>(devMetrics.totalAdditions),
        static_cast<double>(devMetrics.totalDeletions)
    };
    std::vector<std::string> churnLabels = {
        "Additions (" + std::to_string(devMetrics.totalAdditions) + ")",
        "Deletions (" + std::to_string(devMetrics.totalDeletions) + ")"
    };
    matplot::bar(churnValues);
    matplot::xticklabels(churnLabels);
    matplot::title("Code Churn");
    matplot::grid(matplot::on);

    matplot::sgtitle(title);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeDeploymentFrequencyOverTime(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& title
) {
    if (workflowRuns.empty()) {
        std::cout << "No workflow runs to visualize" << std::endl;
        return;
    }

    std::map<std::string, int> deploymentsByDay;
    for (const auto& run : workflowRuns) {
        auto timestamp = run.getRunStartedAt();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm* tm = std::localtime(&time_t);

        char date[11];
        std::strftime(date, sizeof(date), "%Y-%m-%d", tm);
        deploymentsByDay[date]++;
    }

    std::vector<std::string> dates;
    std::vector<int> counts;
    for (const auto& [date, count] : deploymentsByDay) {
        dates.push_back(date);
        counts.push_back(count);
    }

    std::sort(dates.begin(), dates.end());
    std::vector<int> sortedCounts;
    for (const auto& date : dates) {
        sortedCounts.push_back(deploymentsByDay[date]);
    }

    matplot::figure(true);
    matplot::bar(sortedCounts);
    matplot::xticklabels(dates);
    matplot::title(title);
    matplot::xlabel("Date");
    matplot::ylabel("Number of Deployments");
    matplot::grid(matplot::on);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeLeadTimeDistribution(
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    if (pullRequests.empty()) {
        std::cout << "No pull requests to visualize" << std::endl;
        return;
    }

    std::vector<double> leadTimes;
    for (const auto& pr : pullRequests) {
        if (pr.getState() == "closed" && pr.getMergedAt().time_since_epoch().count() != 0) {
            auto leadTime = std::chrono::duration_cast<std::chrono::hours>(
                pr.getMergedAt() - pr.getCreatedAt()
            ).count();
            if (leadTime > 0 && leadTime < 720) {
                leadTimes.push_back(leadTime);
            }
        }
    }

    if (leadTimes.empty()) {
        std::cout << "No merged pull requests with valid lead times" << std::endl;
        return;
    }

    matplot::figure(true);
    auto hist = matplot::hist(leadTimes, 20);
    hist->num_bins(20);
    matplot::title(title);
    matplot::xlabel("Lead Time (hours)");
    matplot::ylabel("Number of PRs");
    matplot::grid(matplot::on);

    double meanLeadTime = std::accumulate(leadTimes.begin(), leadTimes.end(), 0.0) / leadTimes.size();
    std::vector<double> mltimevec = {meanLeadTime, meanLeadTime};
    std::vector<double> histvals = {0, *std::max_element(hist->values().begin(), hist->values().end())};
    matplot::hold(matplot::on);
    matplot::plot(mltimevec, histvals,
                 "-r")->display_name("Mean: " + std::to_string(static_cast<int>(meanLeadTime)) + "h");
    matplot::legend();
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeChangeFailureRateTrend(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& title
) {
    if (workflowRuns.empty()) {
        std::cout << "No workflow runs to visualize" << std::endl;
        return;
    }

    std::map<std::string, std::pair<int, int>> weeklyStats;
    for (const auto& run : workflowRuns) {
        auto timestamp = run.getRunStartedAt();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm* tm = std::localtime(&time_t);

        char week[20];
        std::strftime(week, sizeof(week), "%Y-W%V", tm);

        weeklyStats[week].first++;
        if (run.getConclusion() == "failure" ||
            run.getConclusion() == "cancelled" ||
            run.getConclusion() == "timed_out") {
            weeklyStats[week].second++;
        }
    }

    std::vector<std::string> weeks;
    std::vector<double> failureRates;
    for (const auto& [week, stats] : weeklyStats) {
        weeks.push_back(week);
        failureRates.push_back((static_cast<double>(stats.second) / stats.first) * 100.0);
    }

    std::sort(weeks.begin(), weeks.end());
    std::vector<double> sortedRates;
    for (const auto& week : weeks) {
        sortedRates.push_back(weeklyStats[week].second * 100.0 / weeklyStats[week].first);
    }

    matplot::figure(true);
    matplot::plot(sortedRates, "-o");
    matplot::xticklabels(weeks);
    matplot::title(title);
    matplot::xlabel("Week");
    matplot::ylabel("Failure Rate (%)");
    matplot::grid(matplot::on);

    matplot::hold(matplot::on);
    std::vector<double> benchmark_data = {0, static_cast<double>(weeks.size()-1)};
    std::vector<double> benchmark_threshold = {15.0, 15.0};
    matplot::plot(benchmark_data, benchmark_threshold, "--r")->display_name("15% Benchmark");
    matplot::legend();
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeMTTRDistribution(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& title
) {
    if (workflowRuns.empty()) {
        std::cout << "No workflow runs to visualize" << std::endl;
        return;
    }

    std::vector<double> recoveryTimes;
    for (const auto& run : workflowRuns) {
        if (run.getConclusion() == "failure" ||
            run.getConclusion() == "cancelled" ||
            run.getConclusion() == "timed_out") {
            if (run.getUpdatedAt().time_since_epoch().count() != 0 &&
                run.getRunStartedAt().time_since_epoch().count() != 0) {
                auto recoveryTime = std::chrono::duration_cast<std::chrono::hours>(
                    run.getUpdatedAt() - run.getRunStartedAt()
                ).count();
                if (recoveryTime > 0 && recoveryTime < 720) {
                    recoveryTimes.push_back(recoveryTime);
                }
            }
        }
    }

    if (recoveryTimes.empty()) {
        std::cout << "No failed workflow runs with valid recovery times" << std::endl;
        return;
    }

    matplot::figure(true);
    auto hist = matplot::hist(recoveryTimes, 20);
    hist->num_bins(20);
    matplot::title(title);
    matplot::xlabel("Recovery Time (hours)");
    matplot::ylabel("Number of Incidents");
    matplot::grid(matplot::on);

    double meanMTTR = std::accumulate(recoveryTimes.begin(), recoveryTimes.end(), 0.0) / recoveryTimes.size();
    matplot::hold(matplot::on);
    std::vector<double> meanmttrs = {meanMTTR, meanMTTR};
    std::vector<double> max_hist = {0, *std::max_element(hist->values().begin(), hist->values().end())};
    matplot::plot(meanmttrs, max_hist,
                 "-r")->display_name("Mean: " + std::to_string(static_cast<int>(meanMTTR)) + "h");
    matplot::legend();
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}



void Visualizer::visualizeDevelopmentDashboard(
    const std::vector<Event>& events,
    const std::vector<PullRequest>& pullRequests,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& title
) {
    DevelopmentMetrics metrics = calculateDevelopmentMetrics(events, pullRequests, workflowRuns);

    // auto subplots = matplot::subplot(2, 2);

    matplot::subplot(2, 2, 0);
    std::vector<std::string> labels = {
        "Total PRs",
        "Total Commits",
        "Total Workflow Runs",
        "Avg Commits/PR"
    };
    std::vector<double> values = {
        static_cast<double>(metrics.totalPRs),
        static_cast<double>(metrics.totalCommits),
        static_cast<double>(metrics.totalWorkflowRuns),
        metrics.avgCommitsPerPR
    };
    matplot::bar(values);
    matplot::xticklabels(labels);
    matplot::title("Overall Metrics");
    matplot::grid(matplot::on);

    matplot::subplot(2, 2, 1);
    std::vector<double> prValues = {
        static_cast<double>(metrics.openPRs),
        static_cast<double>(metrics.closedPRs),
        static_cast<double>(metrics.mergedPRs)
    };
    std::vector<std::string> prLabels = {
        "Open (" + std::to_string(metrics.openPRs) + ")",
        "Closed (" + std::to_string(metrics.closedPRs) + ")",
        "Merged (" + std::to_string(metrics.mergedPRs) + ")"
    };
    matplot::pie(prValues, prLabels);
    matplot::title("PR Status Distribution");

    matplot::subplot(2, 2, 2);
    std::vector<double> codeValues = {
        static_cast<double>(metrics.totalAdditions),
        static_cast<double>(metrics.totalDeletions)
    };
    std::vector<std::string> codeLabels = {
        "Additions (" + std::to_string(metrics.totalAdditions) + ")",
        "Deletions (" + std::to_string(metrics.totalDeletions) + ")"
    };
    matplot::bar(codeValues);
    matplot::xticklabels(codeLabels);
    matplot::title("Code Changes");
    matplot::grid(matplot::on);

    matplot::subplot(2, 2, 3);
    if (metrics.avgPRCycleTime > 0) {
        std::vector<double> cycleValues = {metrics.avgPRCycleTime};
        std::vector<std::string> cycleLabels = {"Avg: " + std::to_string(static_cast<int>(metrics.avgPRCycleTime)) + "h"};
        matplot::bar(cycleValues);
        matplot::xticklabels(cycleLabels);
        matplot::title("Average PR Cycle Time");
        matplot::grid(matplot::on);
    } else {
        matplot::text(0.5, 0.5, "No cycle time data");
        matplot::title("Average PR Cycle Time");
    }

    matplot::sgtitle(title);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizePRCycleTimeDistribution(
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    if (pullRequests.empty()) {
        std::cout << "No pull requests to visualize" << std::endl;
        return;
    }

    std::vector<double> cycleTimes;
    for (const auto& pr : pullRequests) {
        if (pr.getState() == "closed") {
            auto endTime = (pr.getMergedAt().time_since_epoch().count() != 0) ?
                pr.getMergedAt() : pr.getClosedAt();
            auto cycleTime = std::chrono::duration_cast<std::chrono::hours>(
                endTime - pr.getCreatedAt()
            ).count();
            if (cycleTime > 0 && cycleTime < 720) {
                cycleTimes.push_back(cycleTime);
            }
        }
    }

    if (cycleTimes.empty()) {
        std::cout << "No closed pull requests with valid cycle times" << std::endl;
        return;
    }

    matplot::figure(true);
    auto hist = matplot::hist(cycleTimes, 20);
    hist->num_bins(20);
    matplot::title(title);
    matplot::xlabel("Cycle Time (hours)");
    matplot::ylabel("Number of PRs");
    matplot::grid(matplot::on);

    double meanCycleTime = std::accumulate(cycleTimes.begin(), cycleTimes.end(), 0.0) / cycleTimes.size();
    matplot::hold(matplot::on);
    std::vector<double> meanTimes = {meanCycleTime, meanCycleTime};
    std::vector<double> max_hist = {0, *std::max_element(hist->values().begin(), hist->values().end())};
    matplot::plot(meanTimes, max_hist,
                 "-r")->display_name("Mean: " + std::to_string(static_cast<int>(meanCycleTime)) + "h");
    matplot::legend();
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeCodeChurnOverTime(
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    if (pullRequests.empty()) {
        std::cout << "No pull requests to visualize" << std::endl;
        return;
    }

    std::map<std::string, std::pair<long, long>> weeklyChurn;
    for (const auto& pr : pullRequests) {
        auto timestamp = pr.getCreatedAt();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm* tm = std::localtime(&time_t);

        char week[20];
        std::strftime(week, sizeof(week), "%Y-W%V", tm);

        weeklyChurn[week].first += pr.getAdditions();
        weeklyChurn[week].second += pr.getDeletions();
    }

    std::vector<std::string> weeks;
    std::vector<long> additions;
    std::vector<long> deletions;
    for (const auto& [week, churn] : weeklyChurn) {
        weeks.push_back(week);
        additions.push_back(churn.first);
        deletions.push_back(churn.second);
    }

    std::sort(weeks.begin(), weeks.end());
    std::vector<long> sortedAdditions, sortedDeletions;
    for (const auto& week : weeks) {
        sortedAdditions.push_back(weeklyChurn[week].first);
        sortedDeletions.push_back(weeklyChurn[week].second);
    }

    matplot::figure(true);
    matplot::plot(sortedAdditions, "-g")->display_name("Additions");
    matplot::hold(matplot::on);
    matplot::plot(sortedDeletions, "-r")->display_name("Deletions");
    matplot::xticklabels(weeks);
    matplot::title(title);
    matplot::xlabel("Week");
    matplot::ylabel("Lines of Code");
    matplot::legend();
    matplot::grid(matplot::on);
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeActivityHeatmap(
    const std::vector<Event>& events,
    const std::string& title
) {
    if (events.empty()) {
        std::cout << "No events to visualize" << std::endl;
        return;
    }

    std::map<int, std::map<int, int>> heatmapData;

    for (const auto& event : events) {
        auto timestamp = event.getTimestamp();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm* tm = std::localtime(&time_t);

        int dayOfWeek = tm->tm_wday;
        int hour = tm->tm_hour;

        heatmapData[dayOfWeek][hour]++;
    }

    const int days = 7;
    const int hours = 24;
    std::vector<std::vector<double>> matrix(days, std::vector<double>(hours, 0.0));

    for (int day = 0; day < days; day++) {
        for (int hour = 0; hour < hours; hour++) {
            if (heatmapData.count(day) && heatmapData[day].count(hour)) {
                matrix[day][hour] = static_cast<double>(heatmapData[day][hour]);
            }
        }
    }

    matplot::figure(true);
    auto heatmap = matplot::heatmap(matrix);

    std::vector<std::string> dayLabels = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    std::vector<std::string> hourLabels;
    for (int h = 0; h < 24; h++) {
        hourLabels.push_back(std::to_string(h));
    }

    matplot::xticklabels(hourLabels);
    matplot::yticklabels(dayLabels);
    matplot::title(title);
    matplot::colorbar();

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeCodeChurnRatio(
    const std::vector<PullRequest>& pullRequests,
    const std::string& title
) {
    if (pullRequests.empty()) {
        std::cout << "No pull requests to visualize" << std::endl;
        return;
    }

    std::vector<double> churnRatios;
    std::vector<std::string> prLabels;

    for (const auto& pr : pullRequests) {
        if (pr.getDeletions() > 0) {
            double ratio = static_cast<double>(pr.getAdditions()) / pr.getDeletions();
            churnRatios.push_back(ratio);
            prLabels.push_back("PR #" + std::to_string(pr.getNumber()));
        } else if (pr.getAdditions() > 0) {
            churnRatios.push_back(1.0);
            prLabels.push_back("PR #" + std::to_string(pr.getNumber()));
        }
    }

    if (churnRatios.empty()) {
        std::cout << "No pull requests with code changes" << std::endl;
        return;
    }

    matplot::figure(true);
    matplot::bar(churnRatios);
    matplot::xticklabels(prLabels);
    matplot::title(title);
    matplot::xlabel("Pull Request");
    matplot::ylabel("Additions / Deletions Ratio");
    matplot::grid(matplot::on);

    double avgRatio = std::accumulate(churnRatios.begin(), churnRatios.end(), 0.0) / churnRatios.size();
    matplot::hold(matplot::on);
    std::vector<double> churn = {0, static_cast<double>(churnRatios.size()-1)};
    std::vector<double> avgRatios = {avgRatio, avgRatio};
    matplot::plot(churn,
                 avgRatios, "--r")->display_name("Average: " + std::to_string(avgRatio));
    matplot::legend();
    matplot::hold(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::saveDORAMetricsPlot(
    const std::vector<Event>& events,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizeDORAMetrics(events, workflowRuns, pullRequests);
    matplot::save(filename);
}

void Visualizer::saveDORADashboardPlot(
    const std::vector<Event>& events,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizeDORADashboard(events, workflowRuns, pullRequests);
    matplot::save(filename);
}

void Visualizer::saveDeploymentFrequencyPlot(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& filename
) {
    visualizeDeploymentFrequencyOverTime(workflowRuns);
    matplot::save(filename);
}

void Visualizer::saveLeadTimeDistributionPlot(
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizeLeadTimeDistribution(pullRequests);
    matplot::save(filename);
}

void Visualizer::saveChangeFailureRateTrendPlot(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& filename
) {
    visualizeChangeFailureRateTrend(workflowRuns);
    matplot::save(filename);
}

void Visualizer::saveMTTRDistributionPlot(
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& filename
) {
    visualizeMTTRDistribution(workflowRuns);
    matplot::save(filename);
}

void Visualizer::saveDevelopmentDashboardPlot(
    const std::vector<Event>& events,
    const std::vector<PullRequest>& pullRequests,
    const std::vector<WorkflowRun>& workflowRuns,
    const std::string& filename
) {
    visualizeDevelopmentDashboard(events, pullRequests, workflowRuns);
    matplot::save(filename);
}

void Visualizer::savePRCycleTimeDistributionPlot(
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizePRCycleTimeDistribution(pullRequests);
    matplot::save(filename);
}

void Visualizer::saveCodeChurnOverTimePlot(
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizeCodeChurnOverTime(pullRequests);
    matplot::save(filename);
}

void Visualizer::saveActivityHeatmapPlot(
    const std::vector<Event>& events,
    const std::string& filename
) {
    visualizeActivityHeatmap(events);
    matplot::save(filename);
}

void Visualizer::saveCodeChurnRatioPlot(
    const std::vector<PullRequest>& pullRequests,
    const std::string& filename
) {
    visualizeCodeChurnRatio(pullRequests);
    matplot::save(filename);
}

void Visualizer::visualizeActivityFrequency(const std::map<std::string, int>& frequency, const std::string& title) {
    if (frequency.empty()) return;

    std::vector<std::string> activities;
    std::vector<double> counts;

    for (const auto& [activity, count] : frequency) {
        activities.push_back(activity);
        counts.push_back(static_cast<double>(count));
    }

    auto bar = matplot::bar(counts);
    bar->display_name(title);
    matplot::title(title);
    matplot::xlabel("Activity");
    matplot::ylabel("Frequency");

    std::vector<double> positions(activities.size());
    std::iota(positions.begin(), positions.end(), 0.0);
    matplot::xticks(positions);
    matplot::xticklabels(activities);
    matplot::grid(matplot::on);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeTransitionMatrix(const std::map<std::pair<std::string, std::string>, int>& transitions, const std::string& title) {
    if (transitions.empty()) {
        std::cout << "No transitions to visualize" << std::endl;
        return;
    }

    std::set<std::string> allActivities;
    for (const auto& [transition, count] : transitions) {
        allActivities.insert(transition.first);
        allActivities.insert(transition.second);
    }

    std::vector<std::string> activities(allActivities.begin(), allActivities.end());
    std::map<std::string, size_t> activityIndex;
    for (size_t i = 0; i < activities.size(); i++) {
        activityIndex[activities[i]] = i;
    }

    size_t n = activities.size();
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 0.0));

    for (const auto& [transition, count] : transitions) {
        size_t fromIdx = activityIndex[transition.first];
        size_t toIdx = activityIndex[transition.second];
        matrix[fromIdx][toIdx] = static_cast<double>(count);
    }

    auto heatmap = matplot::heatmap(matrix);
    std::vector<double> positions(n);
    std::iota(positions.begin(), positions.end(), 0.0);
    matplot::xticks(positions);
    matplot::xticklabels(activities);
    matplot::yticks(positions);
    matplot::yticklabels(activities);
    matplot::title(title);
    matplot::colorbar();

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeProcessFlow(const ProcessModel& model, const std::string& title) {
    auto activities = model.getActivities();
    auto transitions = model.getTransitions();

    if (activities.empty()) return;

    size_t n = activities.size();
    std::vector<double> x(n), y(n);

    double centerX = 0.5;
    double centerY = 0.5;
    double radius = 0.4;
    double angleStep = 2 * M_PI / n;

    std::map<std::string, size_t> activityIndex;
    for (size_t i = 0; i < n; i++) {
        double angle = i * angleStep;
        x[i] = centerX + radius * cos(angle);
        y[i] = centerY + radius * sin(angle);
        activityIndex[activities[i].name] = i;
    }

    matplot::figure(true);
    matplot::title(title);
    matplot::axis(matplot::equal);

    for (const auto& trans : transitions) {
        size_t fromIdx = activityIndex[trans.from];
        size_t toIdx = activityIndex[trans.to];
        matplot::plot({x[fromIdx], x[toIdx]}, {y[fromIdx], y[toIdx]}, "-b");
    }

    for (size_t i = 0; i < n; i++) {
        matplot::text(x[i], y[i], activities[i].name);
    }

    matplot::axis(matplot::off);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeTimeline(const std::vector<Event>& events, const std::string& title) {
    if (events.empty()) return;

    std::vector<double> timestamps;
    std::vector<double> yPositions(events.size(), 0.0);

    for (const auto& event : events) {
        auto duration = event.getTimestamp().time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        timestamps.push_back(static_cast<double>(millis));
    }

    matplot::figure(true);
    matplot::scatter(timestamps, yPositions)->marker_size(10);
    matplot::title(title);
    matplot::xlabel("Timestamp (ms)");
    matplot::ylabel("Event");
    matplot::grid(matplot::on);

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeBottlenecks(const std::vector<std::string>& bottlenecks, const std::map<std::string, int>& frequency, const std::string& title) {
    if (bottlenecks.empty()) return;

    std::vector<std::string> labels;
    std::vector<double> values;

    for (const auto& activity : bottlenecks) {
        if (frequency.find(activity) != frequency.end()) {
            labels.push_back(activity);
            values.push_back(static_cast<double>(frequency.at(activity)));
        }
    }

    if (!values.empty()) {
        matplot::figure(true);
        matplot::pie(values, labels);
        matplot::title(title);
    }

    if (!mHoldOn) matplot::show();
}

void Visualizer::visualizeProcessEfficiency(double efficiency, const std::string& title) {
    std::vector<double> x = {0};
    std::vector<double> y = {efficiency * 100};

    matplot::bar(x, y);
    matplot::xticks({0});
    matplot::xticklabels({"Efficiency"});
    matplot::ylim({0, 100});
    matplot::title(title);
    matplot::text(0, efficiency * 100, std::to_string(static_cast<int>(efficiency * 100)) + "%");

    if (!mHoldOn) matplot::show();
}


void Visualizer::saveActivityFrequencyPlot(const std::map<std::string, int>& frequency, const std::string& filename) {
    visualizeActivityFrequency(frequency);
    matplot::save(filename);
}

void Visualizer::saveTransitionMatrixPlot(const std::map<std::pair<std::string, std::string>, int>& transitions, const std::string& filename) {
    visualizeTransitionMatrix(transitions);
    matplot::save(filename);
}

void Visualizer::saveProcessFlowPlot(const ProcessModel& model, const std::string& filename) {
    visualizeProcessFlow(model);
    matplot::save(filename);
}
