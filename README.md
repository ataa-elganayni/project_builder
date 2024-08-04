# Project Builder

## References
### json library
https://json.nlohmann.me/
https://github.com/nlohmann/json

### CMake Documentation
https://cmake.org/cmake/help/latest/manual/cmake.1.html

## Summary
Project Builder is standalone tool that builds projects defined in project files. The primary goal of the tool is to read the folder structure containing the projects and provide support for common operations such as project conversion. This document outlines the main features and walks through the steps to install and run the tool.

## Dependencies
### A compiler that supports C++ 17.
### CMake. This tool uses standard CMakeLists. While the development environment uses CLion's build system, any environment capable of processing CMakeLists should work.

## Development Environment

## Installation
Project Builder is a self-contained C++ project that doesn't depend on any external setup or configuration. While the tool relies on an open source library to facilitate reading json files, that loibrary in a single header file, which is included with the source code. The tool itself doesn't require installation, only compilation. 

Step 1: clone the repository:
```
git clone https://github.com/ataa-elganayni/project_builder.git
```
Step 2: build the project using a CMake. See the References section.

## Running the Tool
Navigate to the folder where project_builder executable is located. Run the executable with the full path of the projects' root folder as the only parameter.
```
ataa@Hats-Dev-MackBook project_builder % cd cmake-build-release 
ataa@Hats-Dev-MackBook cmake-build-release % ./project_builder /Users/ataa/git/project_builder/test
Project: "root.proj" -- Not Converted
=========Project: "sb1.proj" -- Not Converted
=========Project: "sb3.proj" -- Not Converted

Project: "sb1.proj" -- Not Converted
=========Project: "sb1_2.proj" -- Not Converted
=========Project: "sb3_1.proj" -- Not Converted

Project: "sb1_2.proj" -- Not Converted

Project: "sb2_1.proj" -- Not Converted

Project: "sb3.proj" -- Not Converted
=========Project: "sb2_1.proj" -- Not Converted
=========Project: "sb3_1.proj" -- Not Converted

Project: "sb3_1.proj" -- Not Converted
=========Project: "sb2_1.proj" -- Not Converted

Converting /Users/ataa/git/project_builder/test/root.proj...
Completed
Converting /Users/ataa/git/project_builder/test/sub_folder1/sb1.proj...
Completed
Converting /Users/ataa/git/project_builder/test/sub_folder3/sb3.proj...
Completed
Converting /Users/ataa/git/project_builder/test/sub_folder1/sub_folder1_2/sb1_2.proj...
Completed
Converting /Users/ataa/git/project_builder/test/sub_folder3/sub_folder3_1/sb3_1.proj...
Completed
Converting /Users/ataa/git/project_builder/test/sub_folder2/sub_folder2_1/sb2_1.proj...
Completed
Conversion completed. Conversion report has been saved to ./output
Project: "root.proj" -- Converted
=========Project: "sb1.proj" -- Converted
=========Project: "sb3.proj" -- Converted

Project: "sb1.proj" -- Converted
=========Project: "sb1_2.proj" -- Converted
=========Project: "sb3_1.proj" -- Converted

Project: "sb1_2.proj" -- Converted

Project: "sb2_1.proj" -- Converted

Project: "sb3.proj" -- Converted
=========Project: "sb2_1.proj" -- Converted
=========Project: "sb3_1.proj" -- Converted

Project: "sb3_1.proj" -- Converted
=========Project: "sb2_1.proj" -- Converted

Building /Users/ataa/git/project_builder/test/sub_folder2/sub_folder2_1/sb2_1.proj...
Completed
Building /Users/ataa/git/project_builder/test/sub_folder3/sub_folder3_1/sb3_1.proj...
Completed
Building /Users/ataa/git/project_builder/test/sub_folder3/sb3.proj...
Completed
Building /Users/ataa/git/project_builder/test/sub_folder1/sub_folder1_2/sb1_2.proj...
Completed
Building /Users/ataa/git/project_builder/test/sub_folder1/sb1.proj...
Completed
Building /Users/ataa/git/project_builder/test/root.proj...
Completed
Build completed in 30.0187 seconds
ataa@Hats-Dev-MackBook cmake-build-release % 
```
