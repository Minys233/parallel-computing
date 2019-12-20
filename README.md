# 并行计算小作业

本作业分为4个任务，每个任务配有代码和作业报告。

本说明文件分别介绍环境配置和项目构建，具体代码如何运行和测试还请诸君自行尝试，并不困难。首先，本项目使用 `cmake` 进行跨平台构建，要求版本 `cmake --version ≥ 3.10` 如此可避免一些旧版本 `cmake` 的报错。因为我没有Windows机器，因此只说macOS和Ubuntu代表的Linux操作系统下如何配置依赖以及编译运行。

## 配置环境

- Linux (Ubuntu 18.04)
  - 安装OpenMPI: `sudo apt-get install libopenmpi-dev`
  - 安装freeglut: `sudo apt-get install freeglut3-dev`
  - 一个带有 `-fopenmp` 选项的编译器
- macOS
  - 安装OpenMPI: `brew install open-mpi`
  - 安装freeglut: `brew install freeglut`
  - 安装gcc: `brew install gcc`
    - 自带的clang似乎并不支持OpenMP 

另外，还需要一个 `python2` （random_reduction需要）和一个安装了 `numpy` 的 `python3` 。

## 构建项目

去 `cg/CMakeLists.txt` 和 `mandelbrot/CMakeLists.txt` 中将编译器换成你刚刚安装的支持 `-fopenmp` 选项的编译器。然后到项目的根目录中，运行以下命令构建项目即完成编译。

```bash
cmake -DCMAKE_BUILD_TYPE=Release .
make
```

`cg` 和 `random_reduction` 中有写好的简单测试脚本 `test.sh` 。