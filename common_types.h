#pragma once
#include <string>
#include <fstream>
#include <optional>
#include "HatsDateTime.h"

using namespace Hats::Tools;

enum class ConversionStatus {NotConverted, Converted};

class ProjectInfo
{
private:
    std::string m_FilePath;
    ConversionStatus m_status {ConversionStatus::NotConverted};
    std::optional<std::shared_ptr<HatsDateTime>> m_LastBuilt;
    std::optional<std::string> m_BuildPath;
    bool m_HasParent {false};
    std::vector<std::shared_ptr<ProjectInfo>> m_dependencies;

public:
    explicit ProjectInfo(const std::string &file_path) : m_FilePath(file_path)
    {

    }

    ProjectInfo(const std::string &file_path, bool has_parent) : m_FilePath(file_path),
                                                                 m_HasParent(has_parent)
    {

    }

    ~ProjectInfo() = default;

    void SetConverted() {m_status = ConversionStatus::Converted;}

    void SetBuild(std::shared_ptr<HatsDateTime> build_time, const std::string &build_path)
    {
        m_LastBuilt = build_time;
        m_BuildPath = build_path;
    }

    void AddDependency(std::shared_ptr<ProjectInfo> project)
    {
        m_dependencies.emplace_back(project);
    }

    [[nodiscard]] inline ConversionStatus Status() const {return (m_status);}
    [[nodiscard]] inline bool HasParent() const {return (m_HasParent);}
    [[nodiscard]] inline std::vector<std::shared_ptr<ProjectInfo>> GetDependencies() {return (m_dependencies);}
    [[nodiscard]] inline std::string GetProjectPath() const {return (m_FilePath);}
    [[nodiscard]] inline std::optional<std::shared_ptr<HatsDateTime>> GetBuildTime() const
    {
        return (m_LastBuilt);
    }
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

inline std::ostream &operator <<(std::ostream &os, const ProjectInfo &project_info)
{
    auto p = std::filesystem::path(project_info.GetProjectPath());
    auto status_str = (project_info.Status() == ConversionStatus::Converted ?
                                    "Converted" : "Not Converted");

    os << "Project: " << p.filename() << " -- " << status_str << std::endl;
    return (os);
}