Study Cpp About.

### Linux - vcpkg
```bash
git clone --depth=1 https://github.com/microsoft/vcpkg.git
cd ./vcpkg && ./bootstrap-vcpkg.sh

export PATH="$PATH_TO_VCPKG_DIR:$PATH"

vcpkg integrate install

vcpkg install benchmark eigen3 libigl spdlog tbb
```

### Windows - vcpkg
```bash
git clone --depth=1 https://github.com/microsoft/vcpkg.git
cd ./vcpkg && ./bootstrap-vcpkg.bat

.\vcpkg.exe integrate install

# 将 vcpkg 添加到系统环境变量 (注意以管理员模式打开Powershell)
# - 获取当前 vcpkg 的绝对路径
$vcpkgPath = (Get-Item .).FullName
# - 将 vcpkg 路径添加到系统环境变量 PATH
[System.Environment]::SetEnvironmentVariable('PATH', "$env:PATH;$vcpkgPath", [System.EnvironmentVariableTarget]::Machine)
# - 刷新当前会话的环境变量
$env:Path = [System.Environment]::GetEnvironmentVariable('PATH', [System.EnvironmentVariableTarget]::Machine)


vcpkg install benchmark eigen3 libigl spdlog tbb
```