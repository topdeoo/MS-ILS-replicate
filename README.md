# My replication environment

- OS: Linux x86_64 (6.1.53-1-MANJARO)
- CPU: 13th Gen Intel i5-13500H (16) @ 4.700GHz
- GPU: Intel Raptor Lake-P \[Iris Xe Graphics\]
- Memory: 16GB
- Editor: VS Code
- C Compiler: gcc(version 13.2.1) clang(version 16.0.6)
- C++ Compiler: g++(version 13.2.1) clang++(version 16.0.6)
- C/C++ build tools: cmake(version 3.27.6) xmake(version 2.8.3) make(version 4.4.1)
- Rust: rustc 1.73.0-nightly
- Python: Python 3.11.5
- Java: openjdk 21 2023-09-19
- Go: version go1.21.1 linux/amd64

# How to build

make sure that you have installed the following C++ packages:

- range-v3

when you are in the root directory of the project, run the following command:

```bash
mkdir -p build
cd build
cmake ..
make -j`nproc`
```

# How to run

when you are in the root directory of the project, run the following command:

```bash
pip install cyaron
python script/generate.py
python run_all.py
```

> the python script may generate incorrect data or stuck for reasons, if so, run `zsh script/run_all.zsh` instead.

then you can check the correctness of solution with the following command:

```bash
cd test
make -j`nproc`
cd -
python script/check_all.py
```