# Introduction

**bigint.cpp** - Is An Arbitrary-Precision Integer Arithmetic Library written in C++.

# Installation

1. First of all you should install `meson` and `pkg-config`.
	* On Linux it can be done by using your distro package manager.
	* On Windows you can use `chocolatey` or `vcpkg`.
2. Clone this project and enter the project directory:
```bash
git clone https://github.com/infastin/bigint.cpp
cd bigint.cpp
```
3. Generate build configuration and compile:
```bash
meson build
meson compile -C build
```
4. Run tests (if you want):
```bash
meson test -C build
```
5. Install library (if you want):
```bash
meson --reconfigure --buildtype=release build
# On Windows run without sudo
sudo meson install -C build
```
6. Include library in other projects using build system:
* CMake:
```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBIGINT REQUIRED libbigint)

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${LIBBIGINT_INCLUDE_DIRS})
target_link_libraries(main ${LIBBIGINT_LIBRARIES})
```
* Meson:
```meson
bigint_dep = dependency('libbigint')
executable('main', 'main.cpp',
	dependencies: bigint_dep
)
```

### Note

If `pkg-config` can't find the library, than specify enviroment variable `PKG_CONFIG_PATH` with the path to the library.

# Usage

All operators are overloaded. Use as regular integers.

```cpp
#include <iostream>
#include <vector>
#include <bigint.hpp>

int main()
{
	bigint a = 10;
	bigint al = 10L;
	bigint all = 10LL;
	bigint au = 10U;
	bigint aul = 10UL;
	bigint aull = 10ULL;
	
	/* Also you can construct bigint with integers of different bases. */
	bigint b = "0xFF";
	bigint c = "0o77";
	bigint d = "0b10";
	
	/* Useful methods */
	
	/* You can specify base in the range [2, 16] and prefix of the output number */
	std::string str = a.to_string(16, "0x");
	
	int ia = a.to_int();
	long la = a.to_long();
	long long lla = a.to_llong();
	unsigned int uia = a.to_uint();
	unsigned long ula = a.to_ulong();
	unsigned long long ulla = a.to_ullong();
	
	/* The byte array consists of groups of 4 (WORD_SIZE) bytes, the last byte represents the sign */
	std::string v = a.to_byte_array();
	bigint e = bigint::from_byte_array(v);
	bigint f = bigint::from_byte_array(v.data(), v.size());
	
	bigint asqrt = a.sqrt();
	bigint aabs = a.abs();
	
	auto t = a.div(b);
	bigint quot = t.first; // Quotient
	bigint rem = t.second; // Remainder
	
	size_t ss = b.size(); // Size of bigint in bytes
	
	std::cin >> a;
	std::cout << a << std::endl;
}
```

# License

* MIT
