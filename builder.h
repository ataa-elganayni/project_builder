#pragma once

class ProjectBuilder
{
private:
    //This class follows the same pattern as the converter: perform a straight build and
    //allow for a future deeper analysis.

    //ConversionAnalyzer m_analyzer;    //for future use
    std::optional<std::string> BuildOneProject(std::shared_ptr<ProjectInfo> project)
    {
        std::cout << "Building " << project->GetProjectPath() << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        //if(error building project)
        //{
        //analyzer.AnalyzeResult();
        //Generate error code and message
        //std::cout << "Failed" << std::endl;
        //return(error_message);
        //}

        project->SetBuild(std::make_shared<HatsDateTime>(), "./build");
        std::cout << "Completed" << std::endl;

        return (std::nullopt);
    }

    bool CanBuild(std::shared_ptr<ProjectInfo> project)
    {
        if(!project->GetBuildTime())
        {
            auto child_projects = project->GetDependencies();
            if(child_projects.empty())
            {
                return (true);
            }

            for(auto child : child_projects)
            {
                if(!child->GetBuildTime())
                {
                    return (false);
                }
            }

            return (true);
        }
        return (false);
    }

public:
    //Unlike conversion, building stops on first error. That is, if a project cannot be built, the
    //remaining projects will not be built either.
    void Build(std::shared_ptr<ProjectInfo> root_project)
    {
        auto s = std::stack<std::shared_ptr<ProjectInfo>>();
        s.emplace(root_project);

        auto meter = Meter<std::ratio<1, 1>>();
        while(!s.empty())
        {
            auto project = s.top();
            auto error = std::optional<std::string>{std::nullopt};
            auto can_build = CanBuild(project);

            if(can_build)
            {
                BuildOneProject(project);
            }

            if(can_build || project->GetBuildTime())
            {
                s.pop();
            }

            auto child_projects = project->GetDependencies();
            for(auto child_project : child_projects)
            {
                if(!child_project->GetBuildTime())  //only push projects that haven't been built
                {
                    s.emplace(child_project);
                }
            }
        }

        auto elapsed_time = meter.ElapsedTime();
        std::cout << "Build completed in " << elapsed_time << " seconds" << std::endl;
    }
};