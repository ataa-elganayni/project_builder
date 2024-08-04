#include "mapper.h"
#include "converter.h"
#include "builder.h"

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

    //Mapper has to be called first, obviously. We will let any exception leak and shutdown the run.
    auto mapper = ProjectMapper(std::filesystem::path{root_folder});
    mapper.Print();

    auto converter = ProjectConverter();
    auto conversion_report = converter.Convert(mapper);

    SaveReport(conversion_report, out_folder);

    mapper.Print(); //Print the projects and their dependencies again to make sure conversion happened

    auto root = mapper.GetRootProject();
    auto builder = ProjectBuilder();
    builder.Build(root);
    return 0;
}