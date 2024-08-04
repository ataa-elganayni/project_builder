#pragma once
#include <stack>
#include <map>
#include <regex>
#include "json.hpp"
#include "common_types.h"
#include "project_exceptions.h"

class ProjectMapper
{
private:
    using FolderInfoType = std::pair<std::vector<std::string>, std::optional<std::string>>;
    using CacheIteratorType = std::map<std::string, std::shared_ptr<ProjectInfo>>::iterator;

    std::filesystem::path m_RootFolderPath;
    std::map<std::string, std::shared_ptr<ProjectInfo>> m_ProjectCache;
    std::shared_ptr<ProjectInfo> m_RootProject {nullptr};

    FolderInfoType GetSubfolders(const std::string &folder)
    {
        auto file_pattern = std::regex(".*\\.proj$");

        auto sub_folders = std::vector<std::string>();
        auto p = std::filesystem::path {folder};
        auto project_file = std::optional<std::string> {std::nullopt};

        for (auto const &dir_entry : std::filesystem::directory_iterator{p})
        {
            //auto &sub_folder_path = dir_entry.path();
            auto &name = dir_entry.path();
            //auto name = std::filesystem::absolute(sub_folder_path).string();
            if(dir_entry.is_directory())
            {
                sub_folders.emplace_back(name);
            }
            else if(dir_entry.is_regular_file())
            {
                if (std::regex_match(name.string(), file_pattern))
                {
                    ThrowIfFalse<MapperException>(!project_file,
                                                  "ProjectMapper::GetSubfolders",
                                                  "A folder can only one project file.");
                    project_file = name;
                }
            }
        }

        return (std::pair {sub_folders, project_file});
    }

    std::vector<std::string> Load(const std::string &proj_file)
    {
        ThrowIfFalse<MapperException>(std::filesystem::exists(proj_file),
                                      "ProjectMapper::Load",
                                      std::string("Invalid project file: ") + proj_file);

        auto dependencies = std::vector<std::string>();

        auto s = std::ifstream(proj_file);
        auto project_config = nlohmann::json::parse(s);
        for(auto &reference : project_config["References"])
        {
            auto absolute_path = m_RootFolderPath / reference["Relative Path"];
            dependencies.emplace_back(absolute_path);
        }
        return (dependencies);
    }

    std::vector<std::string> LoadExternalDependency(const std::string &proj_file)
    {
        auto dependencies = std::vector<std::string>();
        return (dependencies);
    }

    //This function builds the dependency graph one step at a time as it encounters the nodes:
        //- When a project is encountered for the first time, it's first level dependencies are added
        //  and the project, along with the dependencies, are added to the cache.
        //- If the project is already in the cache and its dependency count == 0, the dependencies
        //  are added.
        //- A project that exists in the cache is never re-created. That is, if A and B have C as
        //  dependency, both point to the same C, hence, the shared_ptr.
    void UpdateCache(const std::string &file_path)
    {
        auto project_info = std::shared_ptr<ProjectInfo> {nullptr};

        if(m_ProjectCache.find(file_path) == m_ProjectCache.end())
        {
            project_info = std::make_shared<ProjectInfo>(file_path);
            m_ProjectCache[file_path] = project_info;
        }
        else
        {
            project_info = m_ProjectCache[file_path];
        }

        if(!project_info->HasDependency())
        {
            auto dependency_files = Load(file_path);
            for (auto &dependency: dependency_files)
            {
                if (m_ProjectCache.find(dependency) == m_ProjectCache.end())
                {
                    auto child_project = std::make_shared<ProjectInfo>(dependency,
                                                                                            true);
                    project_info->AddDependency(child_project);
                    m_ProjectCache[dependency] = child_project;
                }
                else
                {
                    project_info->AddDependency(m_ProjectCache[dependency]);
                }
            }
        }
    }

    void FindRootProject()
    {
        for(auto &[file_name, project] : m_ProjectCache)
        {
            if(!project->HasParent())
            {
                m_RootProject = project;
                return;
            }
        }
    }

    //Make sure that there are no circular references that hinders the build: A --> B and B --> A
    void ValidateProjectStructure()
    {

    }

    //Assume no virtual folders or symlinks that create a cycle
    void BuildMap()
    {
        auto s = std::stack<std::string>();
        s.push(m_RootFolderPath);

        while(!s.empty())
        {
            auto folder = s.top();
            s.pop();
            auto folder_data = GetSubfolders(folder);
            if(folder_data.second.has_value())
            {
                UpdateCache(*folder_data.second);
            }
            for(auto &f : folder_data.first)
            {
                s.push(f);
            }
        }
    }

public:
    explicit ProjectMapper(const std::string &root_folder) : m_RootFolderPath(root_folder)
    {
        ThrowIfFalse<MapperException>(std::filesystem::exists(root_folder),
                                      "ProjectMapper::ProjectMapper",
                                      "Root folder not found");

        BuildMap();
        FindRootProject();
    }

    ~ProjectMapper() = default;

    inline std::shared_ptr<ProjectInfo> GetRootProject()
    {
        return (m_RootProject);
    }

    void Print()
    {
        for(auto &[proj_path, project] : m_ProjectCache)
        {
            std::cout << *project;
            for(auto child_project : project->GetDependencies())
            {
                std::cout << "=========" << *child_project;
            }
            std::cout << std::endl;
        }
    }

    CacheIteratorType begin()
    {
        return(m_ProjectCache.begin());
    }

    CacheIteratorType end()
    {
        return(m_ProjectCache.end());
    }
};

