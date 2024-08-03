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


int main(int argc, char *argv[])
{
    auto mapper = ProjectMapper("/Users/ataa/projects/project_builder/test");
    return 0;
}