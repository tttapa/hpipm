# hpipm-python

Python interface to HPIPM.

## Manual installation (Linux)

```sh
# Clean up any previous build folders just in case
rm -rf build .py-build-cmake_cache
# Install Python packaging tools
python3 -m pip install -U pip build
# Download Conan recipes to build BLASFEO and HPIPM
git clone https://github.com/tttapa/conan-recipes tttapa-conan-recipes
conan remote add tttapa-conan-recipes tttapa-conan-recipes --force
# If this is your first time using Conan, create a default profile
conan profile detect -e
# Build the HPIPM Python package (change the BLASFEO target as needed)
python3 -m build -w \
    -C 'override=conan.args+=["--build=missing"]' \
    -C 'override=conan.args+=["-o", "blasfeo/*:target=X64_INTEL_HASWELL"]' \
    -C 'override=conan.args+=["-c", "tools.cmake.cmaketoolchain:generator=Ninja"]'
# Or install it using Pip
python3 -m pip install -v . \
    -C 'override=conan.args+=["--build=missing"]' \
    -C 'override=conan.args+=["-o", "blasfeo/*:target=X64_INTEL_HASWELL"]' \
    -C 'override=conan.args+=["-c", "tools.cmake.cmaketoolchain:generator=Ninja"]'
```
