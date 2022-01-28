# nano-bench
nano-bench is a mqtt bench toolkit. 

## Install & compile
```shell
$ git clone https://github.com/nanomq/nano-bench.git 
$ cd nano-bench
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ..
$ make -j 8
```
## Usage
nano_bench support bench test for conn pub sub, You can type help to get detail usage.
```shell
$ nano_bench --help 
$ nano_bench sub --help
$ nano_bench pub --help
$ nano_bench conn --help
```
