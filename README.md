# PowerGraph-GPU

This project is used to provide a generalizable GPU computation acceleration framework for distributed graph processing system.

## Building

The current version of GraphLab PowerGraph was tested on Ubuntu Linux 64-bit 10.04,  11.04 (Natty), 12.04 (Pangolin) as well as Mac OS X 10.7 (Lion) and Mac OS X 10.8 (Mountain Lion). It requires a 64-bit operating system.

# Dependencies

To simplify installation, GraphLab PowerGraph currently downloads and builds most of its required dependencies using CMake’s External Project feature. This also means the first build could take a long time.

There are however, a few dependencies which must be manually satisfied.

* On OS X: g++ (>= 4.2) or clang (>= 3.0) [Required]
  +  Required for compiling GraphLab.

* On Linux: g++ (>= 4.3) or clang (>= 3.0) [Required]
  +  Required for compiling GraphLab.

* *nix build tools: patch, make [Required]
   +  Should come with most Mac/Linux systems by default. Recent Ubuntu version will require to install the build-essential package.

* zlib [Required]
   +   Comes with most Mac/Linux systems by default. Recent Ubuntu version will require the zlib1g-dev package.

* Open MPI or MPICH2 [Strongly Recommended]
   + Required for running GraphLab distributed. 

* JDK 6 or greater [Optional]
   + Required for HDFS support 

## Satisfying Dependencies on Mac OS X

Installing XCode with the command line tools (in XCode 4.3 you have to do this manually in the XCode Preferences -&gt; Download pane), satisfies all of these dependencies.

## Satisfying Dependencies on Ubuntu

All the dependencies can be satisfied from the repository:

    sudo apt-get update
    sudo apt-get install gcc g++ build-essential libopenmpi-dev openmpi-bin default-jdk cmake zlib1g-dev git

# Downloading GraphLab PowerGraph

You can download GraphLab PowerGraph directly from the Github Repository. Github also offers a zip download of the repository if you do not have git.

The git command line for cloning the repository is:

    git clone https://github.com/graphlab-code/graphlab.git
    cd graphlab


# Compiling and Running

```
./configure
```

In the graphlabapi directory, will create two sub-directories, release/ and debug/ . cd into either of these directories and running make will build the release or the debug versions respectively. Note that this will compile all of GraphLab, including all toolkits. Since some toolkits require additional dependencies (for instance, the Computer Vision toolkit needs OpenCV), this will also download and build all optional dependencies.

We recommend using make’s parallel build feature to accelerate the compilation process. For instance:

```
make -j4
```

will perform up to 4 build tasks in parallel. When building in release/ mode, GraphLab does require a large amount of memory to compile with the heaviest toolkit requiring 1GB of RAM.

Alternatively, if you know exactly which toolkit you want to build, cd into the toolkit’s sub-directory and running make, will be significantly faster as it will only download the minimal set of dependencies for that toolkit. For instance:

```
cd release/toolkits/graph_analytics
make -j4
```

will build only the Graph Analytics toolkit and will not need to obtain OpenCV, Eigen, etc used by the other toolkits.

## Compilation Issues
If you encounter issues please post the following on the [GraphLab forum](http://forum.graphlab.com).

* detailed description of the problem you are facing
* OS and OS version
* output of uname -a
* hardware of the machine
* utput of g++ -v and clang++ -v
* contents of graphlab/config.log and graphlab/configure.deps
