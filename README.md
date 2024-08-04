# Project Builder

## References

#### json library
https://json.nlohmann.me/
https://github.com/nlohmann/json

#### CMake Documentation
https://cmake.org/cmake/help/latest/manual/cmake.1.html

## Summary
Project Builder is a standalone tool that builds projects defined in project files. The primary goal of the tool is to read the folder structure containing the projects and provide support for common operations such as project conversion. This document outlines the main features and walks through the steps to install and run the tool.

## Dependencies
- A compiler that supports C++ 17.
- CMake. This tool uses standard CMakeLists. While the development environment uses CLion's build system, any environment capable of processing CMakeLists should work.

## Development Environment
- Mac OSX 14.5
- CLion 2024.1.4
- CMake 3.5

## Windows Support
As indicated above, the tool is a self-contained standard C++ implementation. There is no OS-specific calls or API that hinder compilation on Windows. However, the paths used in the supplied sample project files use UNIX format. In order to reuse the same sample project files, the paths would have to be changed. This a matter of changing the '/' to '\'. 
The implementation itself relies exclusively on std::filesystem to construct folder and file name, which is compatible with Windows. Any symptoms indicating a behavior to the contrary ought to be considered bugs.

## Source Code Structure
- All newly-written code is under the root of the source code tree.
- The bin folder includes the pre-built executable.
- The test folder contains the sample project files.
- Conversion reports are saved to the output folder.
- The external folder contains the source code that already existed and not written specifically for this exercise.

## Installation
Project Builder is a self-contained C++ project that doesn't depend on any external setup or configuration. While the tool relies on an open source library to facilitate reading and writing json files, that library is a single-header file, which is included with the source code. The tool itself doesn't require installation. Further, compilation is only needed if the provided executable isn't compatible with the target system.

Step 1: clone the repository:
```
git clone https://github.com/ataa-elganayni/project_builder.git
```
Step 2: build the project using a CMake. See the References section.
Step 3: Use the provided Mac OS executable, which can be found in the ./bin folder

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

## The Mechanics of the Build Process

Given the graph nature of the structure and the fact that multiple projects can depend on the same child project, the system is designed to eliminate the need to build the same project more than once. That is, given two projects A and B that depend on C, the following two conditions always hold true:
- C is always built before A and B.
- C is only built once.

This is achieved by pointing both A and B to the same object C. Put differently, the same object C appears twice in both A's and B's dependency lists. There are serious implications to this design that are discussed under Limitations and Future Improvement sections. For the purpose of understanding the mechanics of the execution, suffice it to say that this structure facilitates fast execution, even for relatively large number of projects, and keeps the implementation simpler and easier to maintain.

## Features and Components
### Project Mapping
The is the main feature that scans and builds a full map of the projects and their dependencies. This components also acts as a cache in which the projects can be looked up, which eliminates the need to continuously iterate over the projects' objects.
Mapping is implemented by the ProjectConverter class. ProjectConverter takes the path to a root folder and builds two data structures: an std::map that acts as a cache and a graph of projects, which reflects the dependency map. ProjectConverter also acts as an iterator over the loaded projects, which facilitates conversion and building. Finally, ProjectConverter performs checks to ensure all projects' paths exist and that the graph is valid and doesn't have circular references.

### Project Conversion
As the name implies, this is the feature that takes one or more projects and converts each one in turn. Project conversion processes all projects, logging errors as they are encountered, and produces a conversion report. Conversion reports are in json.
Conversion is implemented by ProjectConverter, which maintains two lists of successful and failed conversions. Since conversions are independent of dependency among projects, ProjectConverter operates directly on the mapper, converting each project in turn. 

### Project Builder
This feature depends on the previous features. It's main purpose is to build a project and all its dependencies. Unlike the conversion, the build stops on first error.
Building the projects is handled by ProjectBuilder. Due to its dependency on the relationship among projects, ProjectBuilder accepts the root project as a parameter. Internally, ProjectBuilder uses a stack to build the leaf nodes first. For non-leaf nodes, the build process checks the status of each child project and ensures that children are built first.

### Auxiliary Libraries
In order to facilitate a quicker implementation, and enable the presentation of my work beyond the confined scope of this exercise, I introduced HatsDateTime.h, which is a date/time library I implemented for my Hybrid Adaptive Trading System (Hats). This library has't been implemented specifically for this exercise. Rather, it has existed for a long time. HatsDateTime can be found under the external folder.

## Constraints
If project A requires project B in order to build and B also depends on A, then neither can be built. A constraint of this, and other, build systems is that there cannot be any circular dependency that causes the build to be deadlocked due to two projects waiting for each other. MSBuild, for example, provides a property called CircularDependency to indicate that "a circular dependency was detected." It's important to note that this doesn't affect the common scenario of have multiple projects referencing the same project, which simply must be built before its parent projects.

## Limitations
- External Dependency Support: As of this writing, the system doesn't distinguish between dependencies within the provided root folder and dependencies outside it. On one hand, the implementation is generic enough that external dependencies may work without further action. On the other hand, it's not difficult to imagine a scenario where a dependency outside of the specified root folder may require special processing. This is considered a limitation of the system that must be addressed.
- Serial Execution: For the purpose of this exercise, all projects are converted, and built, serially, in the same thread. Further, the implementation, as it stands, is not thread-safe. However, the way the implementation is structured makes it relatively easy to mitigate these limitations.

## Future Improvements
- A powerful feature of any  build system is its ability to build projects in parallel. Another feature is to be able to perform distributed build. An obvious improvement for this project is to add thread-safety and allow the build to run in parallel. Following, is a straightforward approach:
  - Scan the projects and separate leaf projects, all of which can be built simultaneously.
  - Scan the rest of the projects and separate trees, or sub-trees, that don't share a common node. Each tree or sub-tree can be handled by a different thread.
  - Progressively employ more threads as the lower level nodes are built. For example, two sub-trees within the graph may depend on multiple common nodes. The system can, then, force the two sub-trees to wait until the common nodes are built, then assigns the sub-trees to be executed in parallel.
- Policy-Based Conversion: The conversion process can benefit greately by introducing user-defined policies. It's easy to see that, in some cases, the steps needed to convert some projects can differ from others. For example, external dependencies indicated in the requirements of this exercise can be much simpler to implement if the conversion class can accept a policy template, which can be specialized for different cases.
