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
    std::vector<std::string> m_converted;
    std::vector<std::string> m_failed;
    nlohmann::ordered_json m_report;

    //ConversionAnalyzer m_analyzer;    //for future use
    std::optional<std::string> ConvertOneProject(std::shared_ptr<ProjectInfo> project)
    {
        std::cout << "Converting " << project->GetProjectPath() << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        //if(error converting project)
        //{
            //analyzer.AnalyzeResult();
            //Generate error code and message
            //failed.emplace_back(project->GetProjectPath());
            //std::cout << "Failed" << std::endl;
            //return(error_message);
        //}

        m_converted.emplace_back(project->GetProjectPath());
        project->SetConverted();
        std::cout << "Completed" << std::endl;

        return (std::nullopt);
    }

    nlohmann::ordered_json GenerateReport(double elapsed)
    {
        auto dt = HatsDateTime();
        auto elapsed_str = std::to_string(elapsed) + " seconds";

        m_report["Report Date"] = dt.FormatDateTime();
        m_report["Executed in "] = elapsed_str;
        m_report["Status"] = (m_failed.empty() ? "Success" : "Failed");
        m_report["Project Count"] = m_converted.size() + m_failed.size();

        m_report["Completed"] = nlohmann::ordered_json();
        auto &completed_json = m_report["Completed"];
        completed_json["Count"] = m_converted.size();
        completed_json["Converted Projects"] = m_converted;

        m_report["Failed"] = nlohmann::ordered_json();
        auto &failed_json = m_report["Failed"];
        failed_json["Count"] = m_failed.size();
        failed_json["Failed Projects"] = m_failed;

        return (m_report);
    }

public:
    ProjectConverter() = default;
    ~ProjectConverter() = default;

    nlohmann::ordered_json Convert(ProjectMapper &project_map)
    {
        Reset();

        auto meter = Meter<std::ratio<1, 1>>();
        for(auto &[proj_path, project] : project_map)
        {
            if(project->Status() == ConversionStatus::NotConverted)
            {
                ConvertOneProject(project);
            }

            for(auto child_project : project->GetDependencies())
            {
                if(child_project->Status() == ConversionStatus::NotConverted)
                {
                    ConvertOneProject(child_project);
                }
            }
        }

        auto elapsed_time = meter.ElapsedTime();
        std::cout << "Conversion completed. Conversion report has been saved to ./output" << std::endl;
        return (GenerateReport(elapsed_time));
    }

    [[nodiscard]] nlohmann::ordered_json GetLastReport() const {return (m_report);}

    void Reset()
    {
        m_converted.clear();
        m_failed.clear();
        m_report.clear();
    }
};
