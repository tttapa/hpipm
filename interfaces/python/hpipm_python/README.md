# hpipm-python

Python interface to HPIPM.

## Manual installation (Linux)

```sh
# Clean up any previous build folders just in case
rm -rf build .py-build-cmake_cache
# Install Python packaging tools
python3 -m pip install -U pip build conan
# Download Conan recipes to build BLASFEO and HPIPM
git clone https://github.com/tttapa/conan-recipes tttapa-conan-recipes
conan remote add tttapa-conan-recipes tttapa-conan-recipes --force
# If this is your first time using Conan, create a default profile
conan profile detect ||:
# Build BLASFEO (statically) and HPIPM (shared); change BLASFEO target if needed
conan install . --build=missing \
    -o 'blasfeo/*:target=X64_INTEL_HASWELL' \
    -o 'hpipm/*:shared=True' \
    -c 'tools.cmake.cmaketoolchain:generator=Ninja Multi-Config' \
    -s build_type=Release
# Build the HPIPM Python package
python3 -m build -w .
# Or install it using Pip
python3 -m pip install -v .
```
