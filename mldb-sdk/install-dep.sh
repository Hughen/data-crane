#!/bin/bash

if [[ "`whoami`" != "root" ]]; then
    echo "please run as root"
    exit 1
fi

cmake_inst_ver="3.17.0"

function vercomp () {
    if [[ $1 == $2 ]]; then
        return 0
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++)); do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++)); do
        if [[ -z ${ver2[i]} ]]; then
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]})); then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]})); then
            return 2
        fi
    done

    return 0
}

function install_cmake() {
    wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v${cmake_inst_ver}/cmake-${cmake_inst_ver}-Linux-x86_64.sh
    sh cmake-linux.sh -- --skip-license --prefix=/usr/local
    rm -f cmake-linux.sh
}

# TODO: check python version and install it
python_install_kits=""
py_version=$(python3 --version 2>/dev/null | awk '{print $2;}')
if [ -z "$py_version" ]; then
    python_install_kits="python3 python3-dev python3-pip"
else
    vercomp $py_version 3.5
    if [ $? -eq 2 ]; then
        echo "Python version: $py_version is not supported."
        echo "Please manually upgrade to 3.5 and above."
        exit 2
    fi
fi

apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    wget libcurl4-openssl-dev \
    build-essential autoconf libtool pkg-config git libssl-dev \
    libre2-dev ${python_install_kits}

cmake_version=$(cmake --version 2>/dev/null | grep version | grep -v grep | awk '{print $3;}')
if [ -z "$cmake_version" ]; then
    install_cmake
else
    vercomp $cmake_version 3.13
    if [ $? -eq 2 ]; then
        echo -n "cmake version: $cmake_version is too old"
        echo ", and will be replace the old version($cmake_version) with new version(${cmake_inst_ver})"
        echo
        apt-get remove --purge -y cmake cmake-data
        install_cmake
        hash -r
    fi
fi

tsing_pypi_code=$(curl -s -o /dev/null -w "%{http_code}" -m 2 \
    https://pypi.tuna.tsinghua.edu.cn/simple)
if [[ "$tsing_pypi_code" != "000" ]]; then
    pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple setuptools pybind11
else
    pip3 install setuptools pybind11
    if [ $? -ne 0 ]; then
        exit 3
    fi
fi

core_num=$(getconf _NPROCESSORS_ONLN)
parallel_n=""
if [ $core_num -gt 1 ]; then
    parallel_n=$(($core_num-1))
fi

# secure for process tailer of removal
cd /tmp
git clone --recurse-submodules -b v1.31.0 https://github.com/grpc/grpc
pushd grpc
trap "rm -rf /tmp/grpc" HUP INT QUIT PIPE TERM EXIT
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ../..
make -j ${parallel_n}
make install
popd
popd
