from conan import ConanFile
from conan.tools.build.cppstd import check_min_cppstd
from conan.tools.cmake import CMake, cmake_layout

class HashlibRecipe(ConanFile):
    name = "hashlib"
    version = "1.1.1"
    author = "Cra3z"
    description = "A C++ header-only hash algorithm library"
    topics = ("header-only", "hash", "sha", "md5")
    homepage = "https://github.com/Cra3z/hashlib"
    url = "https://github.com/Cra3z/hashlib"
    license = "MIT"

    settings = "os", "arch", "compiler", "build_type"
    exports_sources = (
        "include/*",
        "cmake/*",
        "tests/*",
        "CMakeLists.txt"
    )
    generators = "CMakeToolchain"

    def validate(self):
        check_min_cppstd(self, "11")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.22]")

    def layout(self):
        cmake_layout(self)

    def build(self):
        enable_testing = "OFF" if self.conf.get("tools.build:skip_test", default=False) else "ON"
        cmake = CMake(self)
        cmake.configure(variables={
            "HASHLIB_TESTS": enable_testing,
        })
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "hashlib")
        self.cpp_info.set_property("cmake_target_name", "hashlib::hashlib")
        self.cpp_info.libs = []
        self.cpp_info.includedirs = ["hashlib/include"]

    def package_id(self):
        self.info.clear()