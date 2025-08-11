[English](README.md) | 简体中文
# hashlib

[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Language](https://img.shields.io/badge/Language-C%2B%2B11-blue.svg)]()

`Cra3z' hashlib` 是一个类似 Python hashlib 的 C++11 header-only 哈希算法库.

支持的算法：
* md5
* sha1
* sha224
* sha256
* sha384
* sha512

## 使用方法

### 使用头文件
hashlib 是一个 header-only 的库, 你可以简单地将 `include/hashlib` 目录下的头文件复制到你需要的地方.
不过, 建议将 hashlib 作为子项目嵌入到你的项目中, 或者先安装它然后通过 CMake 的 `find_package` 导入.
```cmake
find_package(hashlib REQUIRED)
target_link_libraries(<your-target> hashlib::hashlib)
```

#### 单个头文件
你也可以生成一个包含了 `include/hashlib` 目录下所有头文件的单一头文件 `hashlib.hpp`, 详见 [生成单个头文件](#生成单个头文件).

### 使用 C++20 模块
hashlib 支持 C++20 模块 (需要标准模块 `std`); 你可以生成模块文件, 然后导入它, 详见 [生成 C++20 模块](#生成-c20-模块).
```cpp
import std;
import hashlib;

int main() {
    hashlib::md5 md5;
    md5.update("hello world");
    std::println("{}", md5.hexdigest());
}
```

## 构建与安装

### 要求
- CMake 3.22+ (用于构建)
- 支持 C++11 或更新标准的 C++ 编译器

```shell
mkdir <your-build-dir>
cd <your-build-dir>
cmake -S . -B <your-build-dir>
cmake --build <your-build-dir>
cmake --install <your-build-dir> --prefix <your-install-dir>
```

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| HASHLIB_TESTS | ON | 启用测试 |
| HASHLIB_BUILD_SINGLE_HEADER | OFF | 生成单一头文件 |
| HASHLIB_BUILD_MODULE | OFF | 生成 C++20 模块 |

#### 生成单个头文件
使用以下命令, 将在构建目录中生成一个合并了 `include/hashlib` 目录下所有头文件的单个头文件 `hashlib.hpp`.
```shell
cmake -S . -B <your-build-dir> -DHASHLIB_BUILD_SINGLE_HEADER=ON
```

#### 生成 C++20 模块
使用以下命令, 将在构建目录中生成模块文件 `hashlib.cppm`.
```shell
cmake -S . -B <your-build-dir> -DHASHLIB_BUILD_MODULE=ON
```

## 示例

* 使用 `hashlib::md5` 获取字符串的 md5 值

```cpp
#include <iostream>
#include <hashlib/md5.hpp>

int main() {
    hashlib::md5 md5{std::string("hello world")};
    std::cout << md5.hexdigest() << '\n'; // 5eb63bbbe01eeed093cb22bb8f5acdc3
}
```

* 使用 `update` 方法或 `operator<<` 运算符

```cpp
#include <iostream>
#include <hashlib/md5.hpp>

int main() {
    {
        hashlib::md5 md5;
        md5.update(std::string("hello"));
        md5.update(std::string(" world"));
        std::cout << md5.hexdigest() << '\n'; // 5eb63bbbe01eeed093cb22bb8f5acdc3
    }
    {
        hashlib::md5 md5;
        md5 << std::string("hello") << std::string(" world");
        std::cout << md5.hexdigest() << '\n'; // 5eb63bbbe01eeed093cb22bb8f5acdc3
    }
}
```

* 对元素类型为`char`、`unsigned char`、`signed char` 或 `std::byte (C++17起)` 的迭代器和范围中的字节进行哈希计算
```cpp
#include <iostream>
#include <hashlib/sha1.hpp>

int main() {
    unsigned char data[] = {0x00, 0xff, 0x55, 0xaa, 0x12, 0x34, 0x56, 0x78};
    hashlib::sha1 sha1;
    sha1.update(data);
    std::cout << sha1.hexdigest() << '\n'; // f9d9a450e6e14895936f8dc796e30209528de337
}
```

```cpp
#include <iostream>
#include <fstream>
#include <hashlib/sha1.hpp>

int main() {
    std::ifstream file("example.txt", std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "打开文件失败" << std::endl;
        return EXIT_FAILURE;
    }
    hashlib::sha1 sha1;
    sha1.update(
        std::istreambuf_iterator<char>{file}, 
        std::istreambuf_iterator<char>{}
    );
    std::cout << sha1.hexdigest() << '\n'; // 输出 example.txt 文件的 SHA-1 哈希值
}
```