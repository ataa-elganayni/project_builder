#include "mapper.h"
#include "converter.h"
#include "builder.h"

/*
 *

[14:40] Christopher Krawczyk (SQL)
	1	Given a directory structure beginning at "root", which contains projects matching the file extension "*proj", write a program that can traverse the file system (directory structure) and convert (upgrade) the projects to build in the new build system.
	2	Projects can have dependencies on other projects outside the "root" directory structure, which must also be converted.
	3	Projects must build completely meaning all of their dependencies are also converted, including the external ones.
 
	•	You may assume that a static method "void ConvertProject(string projectPath)" exists, which will convert a single project.
	•	You may assume a Project object\node that you can "Project Load(string projectPath)", which contains a string array of it's 1st level dependencies.
 
EX:
- [root]
|- foo.proj
|- foo.cs
|- bar.proj
|- bar.cs
|- [subdir_1]
|- void.proj
|- void.cs
|- [subdir_2]
|- [subdir_2_1]
|- void2.proj
|- void2.cs
|- void2.txt
|- [subdir_3]
|- bar2.proj
|- bar2.cs
|- bar2.png
|- [subdir_N]
|- ...
 
 */
//Requirements:
//Take a root folder as input and find all *proj files.
//Convert projects using the provided API.
//Create dependency chart of all projects.
//Build using the dependecy chart.


//c:\temp\some_folder
//c:\temp\some_folder\folder2

std::filesystem::path CreateOutputFolder()
{
    auto this_folder = std::filesystem::path(__FILE__);
    this_folder = this_folder.parent_path();
    auto out_folder = this_folder / "output";

    if(!std::filesystem::exists(out_folder))
    {
        std::filesystem::create_directories(out_folder);
    }

    return (out_folder);
}

std::filesystem::path CreateBuildFolder()
{
    auto this_folder = std::filesystem::path(__FILE__);
    this_folder = this_folder.parent_path();
    auto out_folder = this_folder / "build";

    if(!std::filesystem::exists(out_folder))
    {
        std::filesystem::create_directories(out_folder);
    }

    return (out_folder);
}

void SaveReport(nlohmann::ordered_json &conversion_report, std::filesystem::path &out_folder)
{
    auto dt = HatsDateTime();
    auto ts = dt.GetTimeStamp();
    auto fname = std::string("conversion_report_") + std::to_string(ts) + ".json";
    auto report_file_path = out_folder / fname;
    auto out_file = std::ofstream(report_file_path, std::ios::out | std::ios::trunc);
    out_file << conversion_report.dump(4);
    out_file.flush();
    out_file.close();
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Root folder wasn't provided." << std::endl;
        return(-1);
    }

    auto root_folder = std::string(argv[1]);
    if(!std::filesystem::exists(root_folder))
    {
        std::cout << "Invalid root folder. " << root_folder << " isn't found." << std::endl;
        return(-1);
    }

    auto out_folder = CreateOutputFolder();
    CreateBuildFolder();

    auto mapper = ProjectMapper(std::filesystem::path{root_folder});
    mapper.Print();

    auto converter = ProjectConverter();
    auto conversion_report = converter.Convert(mapper);

    SaveReport(conversion_report, out_folder);

    mapper.Print();

    auto root = mapper.GetRootProject();
    auto builder = ProjectBuilder();
    builder.Build(root);
    return 0;
}