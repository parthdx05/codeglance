#pragma once
#include <string>
#include <vector>
#include <map>
#include "ProcessModel.hpp"
#include "Event.hpp"
#include "PullRequest.hpp"
#include "WorkflowRun.hpp"

#define MATPLOTLIBS_USE_NAMESPACE
#include <matplot/matplot.h>

struct DORAMetrics {
    double deploymentFrequency;    // Deployments per day
    double leadTimeForChanges;    // Average hours from commit to deploy
    double changeFailureRate;     // Percentage of failed deployments
    double meanTimeToRecovery;    // Average hours to recover from failure
    int totalDeployments;
    int failedDeployments;
    int successfulDeployments;
};

struct DevelopmentMetrics {
    int totalPRs;
    int totalCommits;
    int totalWorkflowRuns;
    int openPRs;
    int closedPRs;
    int mergedPRs;
    long totalAdditions;
    long totalDeletions;
    long totalChangedFiles;
    double avgPRCycleTime;       // Hours
    double avgCommitsPerPR;
    double codeChurnRatio;        // Additions / Deletions
};

class Visualizer {
public:
    Visualizer() {}
    ~Visualizer() {}

    void visualizeDORAMetrics(
        const std::vector<Event>& events,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "DORA Metrics"
    );

    void visualizeDORADashboard(
        const std::vector<Event>& events,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "DORA Dashboard"
    );

    void visualizeDeploymentFrequencyOverTime(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& title = "Deployment Frequency Over Time"
    );

    void visualizeLeadTimeDistribution(
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "Lead Time for Changes Distribution"
    );

    void visualizeChangeFailureRateTrend(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& title = "Change Failure Rate Trend"
    );

    void visualizeMTTRDistribution(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& title = "Mean Time to Recovery Distribution"
    );

    // === DEVELOPMENT METRICS ===
    void visualizeDevelopmentDashboard(
        const std::vector<Event>& events,
        const std::vector<PullRequest>& pullRequests,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& title = "Development Metrics Dashboard"
    );

    void visualizePRCycleTimeDistribution(
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "PR Cycle Time Distribution"
    );

    void visualizeCodeChurnOverTime(
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "Code Churn Over Time"
    );

    void visualizeActivityHeatmap(
        const std::vector<Event>& events,
        const std::string& title = "Development Activity Heatmap"
    );

    void visualizeCodeChurnRatio(
        const std::vector<PullRequest>& pullRequests,
        const std::string& title = "Code Churn Ratio"
    );

    void visualizeActivityFrequency(const std::map<std::string, int>& frequency, const std::string& title = "Activity Frequency");
    void visualizeTransitionMatrix(const std::map<std::pair<std::string, std::string>, int>& transitions, const std::string& title = "Transition Matrix");
    void visualizeProcessFlow(const ProcessModel& model, const std::string& title = "Process Flow");
    void visualizeTimeline(const std::vector<Event>& events, const std::string& title = "Event Timeline");
    void visualizeBottlenecks(const std::vector<std::string>& bottlenecks, const std::map<std::string, int>& frequency, const std::string& title = "Process Bottlenecks");
    void visualizeProcessEfficiency(double efficiency, const std::string& title = "Process Efficiency");

    void holdOn(bool enable) {
        mHoldOn = enable;
        matplot::hold(enable ? matplot::on : matplot::off);
    }
    void showAll() {
        matplot::show();
    }

    void saveDORAMetricsPlot(
        const std::vector<Event>& events,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "dora_metrics.png"
    );

    void saveDORADashboardPlot(
        const std::vector<Event>& events,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "dora_dashboard.png"
    );

    void saveDeploymentFrequencyPlot(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& filename = "deployment_frequency.png"
    );

    void saveLeadTimeDistributionPlot(
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "lead_time_distribution.png"
    );

    void saveChangeFailureRateTrendPlot(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& filename = "change_failure_rate_trend.png"
    );

    void saveMTTRDistributionPlot(
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& filename = "mttr_distribution.png"
    );

    // === SAVE METHODS FOR DEVELOPMENT METRICS ===
    void saveDevelopmentDashboardPlot(
        const std::vector<Event>& events,
        const std::vector<PullRequest>& pullRequests,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::string& filename = "development_dashboard.png"
    );

    void savePRCycleTimeDistributionPlot(
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "pr_cycle_time_distribution.png"
    );

    void saveCodeChurnOverTimePlot(
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "code_churn_over_time.png"
    );

    void saveActivityHeatmapPlot(
        const std::vector<Event>& events,
        const std::string& filename = "activity_heatmap.png"
    );

    void saveCodeChurnRatioPlot(
        const std::vector<PullRequest>& pullRequests,
        const std::string& filename = "code_churn_ratio.png"
    );

    void saveActivityFrequencyPlot(const std::map<std::string, int>& frequency, const std::string& filename = "activity_frequency.png");
    void saveTransitionMatrixPlot(const std::map<std::pair<std::string, std::string>, int>& transitions, const std::string& filename = "transition_matrix.png");
    void saveProcessFlowPlot(const ProcessModel& model, const std::string& filename = "process_flow.png");

private:
    bool mHoldOn;

    DORAMetrics calculateDORAMetrics(
        const std::vector<Event>& events,
        const std::vector<WorkflowRun>& workflowRuns,
        const std::vector<PullRequest>& pullRequests
    );

    DevelopmentMetrics calculateDevelopmentMetrics(
        const std::vector<Event>& events,
        const std::vector<PullRequest>& pullRequests,
        const std::vector<WorkflowRun>& workflowRuns
    );
};
