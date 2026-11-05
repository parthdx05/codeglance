#include <iostream>
#include "Github.hpp"
#include "ProcessMiningTransformer.hpp"
#include "ProcessMiner.hpp"
#include "ProcessModel.hpp"
#include "Visualizer.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <github_token> [owner] [repo]" << std::endl;
        std::cerr << "Example: " << argv[0] << " ghp_xxx microsoft vscode" << std::endl;
        return 1;
    }

    std::string token = argv[1];
    std::string owner = (argc > 2) ? argv[2] : "microsoft";
    std::string repo = (argc > 3) ? argv[3] : "vscode";

    std::cout << "\nFetching data from: " << owner << "/" << repo << std::endl;

    ProcessMiningTransformer transformer;
    Github::Repository githubRepo(owner, repo, token);
    Visualizer visualizer;

    std::cout << "\n[1/4] Fetching GitHub data..." << std::endl;

    auto pullRequests = githubRepo.getPullRequests();
    auto workflowRuns = githubRepo.getWorkflowRuns();

    std::cout << "  ✓ Fetched " << pullRequests.size() << " pull requests" << std::endl;
    std::cout << "  ✓ Fetched " << workflowRuns.size() << " workflow runs" << std::endl;

    std::cout << "\n[2/4] Transforming data to events..." << std::endl;

    for (const auto& pr : pullRequests) {
        transformer.addPullRequest(pr);
        transformer.addCommits(pr, pr.getCommits());
    }
    transformer.addWorkflowRuns(workflowRuns);

    transformer.transformToCSV("events.csv");
    std::cout << "  ✓ Exported " << transformer.getEvents().size() << " events to events.csv" << std::endl;

    std::cout << "\n[3/4] Calculating metrics..." << std::endl;

    auto events = transformer.getEvents();
    ProcessMiner miner(ProcessMiner::Algorithm::ALPHA_MINER);
    auto processModel = miner.discoverProcessModel(events, owner + "/" + repo);

    miner.printProcessStatistics(events);
    processModel.printSummary();

    std::cout << "\n[4/4] Generating DORA Metrics visualizations..." << std::endl;

    visualizer.holdOn(true);

    // visualizer.visualizeDORAMetrics(events, workflowRuns, pullRequests, "DORA Metrics - " + owner + "/" + repo);
    // visualizer.visualizeDORADashboard(events, workflowRuns, pullRequests, "DORA Dashboard - " + owner + "/" + repo);
    // visualizer.visualizeDeploymentFrequencyOverTime(workflowRuns, "Deployment Frequency - " + owner + "/" + repo);
    // visualizer.visualizeLeadTimeDistribution(pullRequests, "Lead Time Distribution - " + owner + "/" + repo);
    // visualizer.visualizeChangeFailureRateTrend(workflowRuns, "Change Failure Rate Trend - " + owner + "/" + repo);
    // visualizer.visualizeMTTRDistribution(workflowRuns, "MTTR Distribution - " + owner + "/" + repo);

    // visualizer.visualizeDevelopmentDashboard(events, pullRequests, workflowRuns, "Development Dashboard - " + owner + "/" + repo);
    // visualizer.visualizePRCycleTimeDistribution(pullRequests, "PR Cycle Time Distribution - " + owner + "/" + repo);
    // visualizer.visualizeCodeChurnOverTime(pullRequests, "Code Churn Over Time - " + owner + "/" + repo);
    // visualizer.visualizeActivityHeatmap(events, "Activity Heatmap - " + owner + "/" + repo);
    // visualizer.visualizeCodeChurnRatio(pullRequests, "Code Churn Ratio - " + owner + "/" + repo);

    auto activityFreq = miner.calculateActivityFrequency(events);
    auto transitionFreq = miner.calculateTransitionFrequency(events);

    // visualizer.visualizeActivityFrequency(activityFreq, "Activity Frequency - " + owner + "/" + repo);
    // visualizer.visualizeTransitionMatrix(transitionFreq, "Transition Matrix - " + owner + "/" + repo);
    // visualizer.visualizeProcessFlow(processModel, "Process Flow - " + owner + "/" + repo);

    double efficiency = miner.calculateProcessEfficiency(events);
    // visualizer.visualizeProcessEfficiency(efficiency, "Process Efficiency - " + owner + "/" + repo);

    auto bottlenecks = miner.findBottlenecks(events);
    // visualizer.visualizeBottlenecks(bottlenecks, activityFreq, "Bottlenecks - " + owner + "/" + repo);

    std::cout << "\nSaving visualizations to PNG files..." << std::endl;

    visualizer.saveDORAMetricsPlot(events, workflowRuns, pullRequests, "dora_metrics.png");
    visualizer.saveDORADashboardPlot(events, workflowRuns, pullRequests, "dora_dashboard.png");
    visualizer.saveDeploymentFrequencyPlot(workflowRuns, "deployment_frequency.png");
    visualizer.saveLeadTimeDistributionPlot(pullRequests, "lead_time_distribution.png");
    visualizer.saveChangeFailureRateTrendPlot(workflowRuns, "change_failure_rate_trend.png");
    visualizer.saveMTTRDistributionPlot(workflowRuns, "mttr_distribution.png");

    visualizer.saveDevelopmentDashboardPlot(events, pullRequests, workflowRuns, "development_dashboard.png");
    visualizer.savePRCycleTimeDistributionPlot(pullRequests, "pr_cycle_time_distribution.png");
    visualizer.saveCodeChurnOverTimePlot(pullRequests, "code_churn_over_time.png");
    visualizer.saveActivityHeatmapPlot(events, "activity_heatmap.png");
    visualizer.saveCodeChurnRatioPlot(pullRequests, "code_churn_ratio.png");

    visualizer.saveActivityFrequencyPlot(activityFreq, "activity_frequency.png");
    visualizer.saveTransitionMatrixPlot(transitionFreq, "transition_matrix.png");
    visualizer.saveProcessFlowPlot(processModel, "process_flow.png");

    std::cout << "  ✓ All visualizations saved!" << std::endl;

    std::cout << "\n=== SUMMARY ===" << std::endl;
    std::cout << "Repository: " << owner << "/" << repo << std::endl;
    std::cout << "Pull Requests: " << pullRequests.size() << std::endl;
    std::cout << "Workflow Runs: " << workflowRuns.size() << std::endl;
    std::cout << "Total Events: " << events.size() << std::endl;
    std::cout << "Output Files: events.csv, *.png" << std::endl;
    std::cout << "\nCheck the generated PNG files for visualizations!" << std::endl;

    visualizer.holdOn(false);
    visualizer.showAll();

    return 0;
}
