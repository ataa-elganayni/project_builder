#pragma once
#include <string>
#include <fstream>
#include <optional>

class ProjectInfo
{
private:
    std::string m_FilePath;
    std::optional<int64_t> m_LastBuilt;
    std::optional<std::string> m_BuildPath;
    std::shared_ptr<ProjectInfo> m_ParentProject {nullptr};
    std::vector<std::shared_ptr<ProjectInfo>> m_dependencies;

public:
    explicit ProjectInfo(const std::string &file_path) : m_FilePath(file_path)
    {

    }

    ProjectInfo(const std::string &file_path, std::shared_ptr<ProjectInfo> parent) :
                                                                        m_FilePath(file_path),
                                                                        m_ParentProject(parent)
    {

    }

    ~ProjectInfo() = default;

    void SetBuild(int64_t build_time, std::string &build_path)
    {
        m_LastBuilt = build_time;
        m_BuildPath = build_path;
    }

    void AddDependency(std::shared_ptr<ProjectInfo> project)
    {
        m_dependencies.emplace_back(project);
    }

    [[nodiscard]] inline std::shared_ptr<ProjectInfo> GetParent() {return (m_ParentProject);}
    [[nodiscard]] inline std::vector<std::shared_ptr<ProjectInfo>> GetDependencies() {return (m_dependencies);}
    [[nodiscard]] inline std::string GetProjectPath() const {return (m_FilePath);}
    [[nodiscard]] inline std::optional<int64_t> GetBuildTime() const {return (m_LastBuilt);}
    [[nodiscard]] inline std::optional<std::string> GetBuildPath() const {return (m_BuildPath);}
    [[nodiscard]] inline bool IsBuilt() const {return (m_BuildPath.has_value());}
    [[nodiscard]] inline bool HasDependency() const {return (!m_dependencies.empty());}
};

inline bool operator ==(const ProjectInfo &lhs, const ProjectInfo &rhs)
{
    return (lhs.GetProjectPath() == rhs.GetProjectPath());
}

inline bool operator <(const ProjectInfo& lhs, const ProjectInfo& rhs)
{
    return (lhs.GetProjectPath() < rhs.GetProjectPath());
}

inline bool operator >(const ProjectInfo &lhs, const ProjectInfo &rhs) { return(rhs < lhs);}
inline bool operator >=(const ProjectInfo &lhs, const ProjectInfo &rhs) {return(!(rhs > lhs));}
inline bool operator <=(const ProjectInfo &lhs, const ProjectInfo &rhs) {return(!(rhs < lhs));}