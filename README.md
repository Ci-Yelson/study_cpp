Study Cpp About.

### vcpkg
```
git clone --depth=1 https://github.com/microsoft/vcpkg.git
cd ./vcpkg && ./bootstrap-vcpkg.sh

export PATH="$PATH_TO_VCPKG_DIR:$PATH"

vcpkg integrate install

vcpkg install benchmark eigen3 libigl spdlog tbb
```