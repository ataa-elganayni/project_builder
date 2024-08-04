#pragma once
#include <iostream>
#include <thread>
#include "queue"
#include "json.hpp"
#include "common_types.h"
#include "mapper.h"
#include "HatsDateTime.h"
#include "ExecutionMeter.h"

using namespace Hats::Tools;

class ProjectConverter
{
private:
    //ConversionAnalyzer m_analyzer;    //for future use
    std::optional<std::string> ConvertOneProject(std::shared_ptr<ProjectInfo> project)
    {
        std::cout << "Converting " << project->GetProjectPath() << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        //if(error converting project)
        //{
        //analyzer.AnalyzeResult();
        //Generate error code and message
        //std::cout << "Failed" << std::endl;
        //return(error_message);
        //}
        project->SetConverted();
        std::cout << "Completed" << std::endl;

        return (std::nullopt);
    }

    nlohmann::ordered_json GenerateReport(const std::vector<std::string> &converted,
                                          const std::vector<std::string> &failed,
                                          double elapsed) //elapsed is in milliseconds
    {
        auto dt = HatsDateTime();
        auto elapsed_str = std::to_string(elapsed) + " seconds";

        auto conversion_report = nlohmann::ordered_json();
        conversion_report["Report Date"] = dt.FormatDateTime();
        conversion_report["Executed in "] = elapsed_str;
        conversion_report["Status"] = (failed.empty() ? "Success" : "Failed");
        conversion_report["Project Count"] = converted.size() + failed.size();

        conversion_report["Completed"] = nlohmann::ordered_json();
        auto &completed_json = conversion_report["Completed"];
        completed_json["Count"] = converted.size();
        completed_json["Converted Projects"] = converted;

        conversion_report["Failed"] = nlohmann::ordered_json();
        auto &failed_json = conversion_report["Failed"];
        failed_json["Count"] = failed.size();
        failed_json["Failed Projects"] = failed;

        return (conversion_report);
    }

public:
    ProjectConverter() = default;
    ~ProjectConverter() = default;

    nlohmann::ordered_json Convert(ProjectMapper &project_map)
    {
        auto converted = std::vector<std::string>();
        auto failed = std::vector<std::string>();

        auto meter = Meter<std::ratio<1, 1>>();
        for(auto &[proj_path, project] : project_map)
        {
            if(project->Status() == ConversionStatus::NotConverted)
            {
                auto error = ConvertOneProject(project);
                if(error)
                {
                    failed.emplace_back(project->GetProjectPath());
                }
                else
                {
                    converted.emplace_back(project->GetProjectPath());
                }
            }

            for(auto child_project : project->GetDependencies())
            {
                if(child_project->Status() == ConversionStatus::NotConverted)
                {
                    auto error = ConvertOneProject(child_project);
                    if (error)
                    {
                        failed.emplace_back(project->GetProjectPath());
                    }
                    else
                    {
                        converted.emplace_back(project->GetProjectPath());
                    }
                }
            }
        }

        auto elapsed_time = meter.ElapsedTime();
        std::cout << "Conversion completed. Conversion report has been saved to ./output" << std::endl;
        return (GenerateReport(converted, failed, elapsed_time));
    }
};
