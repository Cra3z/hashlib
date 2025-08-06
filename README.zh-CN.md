# cc_hashlib
* A C++ header-only hash algorithm library (including md5, sha1, sha256, sha224, sha384, sha512);
* Use this library like Python's hashlib;
* minimum required standard: C++11
* 这是一个header-only风格的C++哈希算法库 (包含md5, sha1, sha256, sha224, sha384, sha512等算法);
* 用法与Python的hashlib库类似
* 需要编译器支持C++11及以上的标准

* Modules can be compiled through the module unit file "hashlib.ixx"
* 可通过模块单元文件"hashlib.ixx"编译模块
* Support C++20 module (currently only supports MSVC compiler, and requires it enable to use `import std;` to import the standard library. if MSVC still can't use `import std;` under /std:c++latest, read"https://learn.microsoft.com/en-us/cpp/cpp/tutorial-import-stl-named-module?view=msvc-170"
* 支持C++20模块(目前只支持MSVC编译器, 并且要求其可以使用`import std;`导入标准库. 若MSVC在/std:c++latest下仍不能`import std;`, 可参考"https://learn.microsoft.com/zh-cn/cpp/cpp/tutorial-import-stl-named-module?view=msvc-170#import-the-standard-library-with-std")
```c++
import std;
import ccat.hashlib;

auto main() -> int {
    std::cout << ccat::hashlib::md5{}.update("Hello C++20 Modules").hexdigest(); // result: 113d5ad03d09b98cf4e22eb279bcdf8c
    return 0;
}
```
## example/例子
* Use this library only including the header `hashlib`
* 仅需include头文件`hashlib`即可
```c++
#include "hashlib"
```
* Get md5 of a string by using `ccat::hash::md5`
* 使用`ccat::hash::md5`获取字符串的md5值
```c++
#include <iostream>
#include "hashlib"
auto main() ->int {
    ccat::hash::md5 m{"hello world"};
    std::cout << m.hexdigest() << '\n'; //result: 5eb63bbbe01eeed093cb22bb8f5acdc3
    return 0;
}
```
* And also you can use the method `update` or `operator<<`
* 你也可以使用`update` 或 `operator<<`
```c++
#include <iostream>
#include "hashlib"
auto main() ->int {
    ccat::hash::md5 m{};
    m.update("hello world"); //or m.update("hello"); m.update(" world");
    std::cout << m.hexdigest() << '\n'; //result: 5eb63bbbe01eeed093cb22bb8f5acdc3
    return 0;
}
```
```c++
#include <iostream>
#include "hashlib"
auto main() ->int {
    ccat::hash::md5 m{};
    m << "hello world"; //or `m << "hello" << " world";`
    std::cout << m.hexdigest() << '\n'; //result: 5eb63bbbe01eeed093cb22bb8f5acdc3
    return 0;
}
```
* Other hash algorithms are used in the same way as `ccat::hash::md5`
* More hash algorithms will be supported in the future
* 其它算法的用法与`ccat::hash::md5`相类似
* 后续将支持更多哈希算法
