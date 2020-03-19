[microui](https://github.com/rxi/microui)到[EGE](https://xege.org)上的移植

microui版本：[zmui](https://www.github.com/chirsz-ever/microui)，EGE版本：19.01+。

## 示例编译运行方法
目前仅支持 Mingw 编译，建议使用 [TDM-GCC](https://jmeubank.github.io/tdm-gcc/)，注意安装过程中选择 "Add to PATH"。

在相应的子目录下（如 [demo/microui_demo](demo/microui_demo)）执行 `make` 命令（Mingw 附带的程序名为 `mingw32-make`，以下用 `make` 指称），即可启动编译过程。

如果使用的 32 位 Mingw，则执行 `make` 命令时需要附加参数：

```sh
$ make EGE_LIB=../../libgraphics.a
```

如果想使用 64 位 Mingw 编译出 32 位可执行程序，则执行 `make` 命令时需要附加参数：

```sh
$ make EGE_LIB=../../libgraphics.a CC='gcc -m32' CXX='g++ -m32'
```
