#pragma once
#include <stack>
#include <map>
#include <regex>
#include "common_types.h"
#include "project_exceptions.h"

class ProjectMapper
{
private:
    using FolderInfoType = std::pair<std::vector<std::string>, std::optional<std::string>>;

    std::string m_RootFolder;
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
            auto &sub_folder_path = dir_entry.path();
            auto name = sub_folder_path.string();
            if(dir_entry.is_directory())
            {
                sub_folders.emplace_back(name);
            }
            else if(dir_entry.is_regular_file())
            {
                if (std::regex_match(name, file_pattern))
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
        auto parent_project = std::shared_ptr<ProjectInfo> {nullptr};

        if(m_ProjectCache.find(file_path) == m_ProjectCache.end())
        {
            parent_project = std::make_shared<ProjectInfo>(file_path);
            m_ProjectCache[file_path] = parent_project;
        }
        else
        {
            parent_project = m_ProjectCache[file_path];
        }

        if(!parent_project->HasDependency())
        {
            auto dependency_files = Load(file_path);
            for (auto &dependency: dependency_files)
            {
                if (m_ProjectCache.find(dependency) == m_ProjectCache.end())
                {
                    auto child_project = std::make_shared<ProjectInfo>(dependency,
                                                                                            parent_project);
                    parent_project->AddDependency(child_project);
                    m_ProjectCache[file_path] = child_project;
                }
                else
                {
                    parent_project->AddDependency(m_ProjectCache[dependency]);
                }
            }
        }
    }

    void FindRootProject()
    {
        for(auto &[fname, project] : m_ProjectCache)
        {
            if(project->GetParent() == nullptr)
            {
                m_RootProject = project;
                return;
            }
        }
    }

public:
    explicit ProjectMapper(const std::string &root_folder) :
                                                    m_RootFolder(root_folder)
    {
        ThrowIfFalse<MapperException>(std::filesystem::exists(root_folder),
                                      "ProjectMapper::ProjectMapper",
                                      "Root folder not found");

        BuildMap();
        FindRootProject();
    }

    ~ProjectMapper() = default;

    //Assume no virtual folders or symlinks that create a cycle
    void BuildMap()
    {
        auto s = std::stack<std::string>();
        s.push(m_RootFolder);

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
};

