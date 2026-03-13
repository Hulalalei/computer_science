#!/bin/bash
set -e

echo '-- Automatically installing ArchVim system dependencies...'

cd "$(dirname $0)/.."

get_linux_distro() {
    if grep -Eq "Ubuntu" /etc/*-release 2> /dev/null; then
        echo "Ubuntu"
    elif grep -Eq "Deepin" /etc/*-release 2> /dev/null; then
        echo "Deepin"
    elif grep -Eq "Raspbian" /etc/*-release 2> /dev/null; then
        echo "Raspbian"
    elif grep -Eq "uos" /etc/*-release 2> /dev/null; then
        echo "UOS"
    elif grep -Eq "LinuxMint" /etc/*-release 2> /dev/null; then
        echo "LinuxMint"
    elif grep -Eq "elementary" /etc/*-release 2> /dev/null; then
        echo "elementaryOS"
    elif grep -Eq "Debian" /etc/*-release 2> /dev/null; then
        echo "Debian"
    elif grep -Eq "Kali" /etc/*-release 2> /dev/null; then
        echo "Kali"
    elif grep -Eq "Parrot" /etc/*-release 2> /dev/null; then
        echo "Parrot"
    elif grep -Eq "CentOS" /etc/*-release 2> /dev/null; then
        echo "CentOS"
    elif grep -Eq "fedora" /etc/*-release 2> /dev/null; then
        echo "fedora"
    elif grep -Eq "openSUSE" /etc/*-release 2> /dev/null; then
        echo "openSUSE"
    elif grep -Eq "Arch Linux" /etc/*-release 2> /dev/null; then
        echo "ArchLinux"
    elif grep -Eq "ManjaroLinux" /etc/*-release 2> /dev/null; then
        echo "ManjaroLinux"
    elif grep -Eq "Gentoo" /etc/*-release 2> /dev/null; then
        echo "Gentoo"
    elif grep -Eq "alpine" /etc/*-release 2> /dev/null; then
        echo "Alpine"
    elif [ "x$(uname -s)" = "xDarwin" ]; then
        echo "MacOS"
    else
        echo "Unknown"
    fi
}

detect_platform() {
  local platform="$(uname -s | tr '[:upper:]' '[:lower:]')"

  # check for MUSL
  if [ "${platform}" = "linux" ]; then
    if ldd /bin/sh | grep -i musl >/dev/null; then
      platform=linux_musl
    fi
  fi

  # mingw is Git-Bash
  if echo "${platform}" | grep -i mingw >/dev/null; then
    platform=win
  fi

  echo "${platform}"
}

detect_arch() {
  local arch="$(uname -m | tr '[:upper:]' '[:lower:]')"

  if echo "${arch}" | grep -i arm >/dev/null; then
    # ARM is fine
    echo "${arch}"
  else
    if [ "${arch}" = "i386" ]; then
      arch=x86
    elif [ "${arch}" = "x86_64" ]; then
      arch=x64
    elif [ "${arch}" = "aarch64" ]; then
      arch=arm64
    fi

    # `uname -m` in some cases mis-reports 32-bit OS as 64-bit, so double check
    if [ "${arch}" = "x64" ] && [ "$(getconf LONG_BIT)" -eq 32 ]; then
      arch=x86
    fi

    echo "${arch}"
  fi
}

pcall() {
    "$@"
    echo -- ERROR: failed to install: "$@"
}

ensure_pip() {
    python="$(which python3 || which python)"
    if ! $python -m pip --version 2> /dev/null; then
        (curl --connect-timeout 8 https://bootstrap.pypa.io/get-pip.py | $python) || true
    fi
    if ! $python -m pip --version 2> /dev/null; then
        pcall $python -m ensurepip
    fi
    pcall $python -m pip --version
}

install_pip() {
    ensure_pip
    python="$(which python3 || which python)"
    if "$python" -m pip install --help | grep break-system-packages; then
        args="--break-system-packages"
    else
        args=
    fi
    index=(-i https://mirrors.tuna.tsinghua.edu.cn/pypi/web/simple)

    packages=(pynvim openai tiktoken)
    for package in "${packages[@]}"; do
        pcall "$python" -m pip install -U "${index[@]}" $package $break
    done
}

install_npm() {
    registry=--registry=https://registry.npmmirror.com
    pcall npm install -g pyright $registry
}

install_pacman() {
    sudo pacman -S --noconfirm ripgrep
    sudo pacman -S --noconfirm fzf
    sudo pacman -S --noconfirm cmake
    sudo pacman -S --noconfirm make
    sudo pacman -S --noconfirm git
    sudo pacman -S --noconfirm gcc
    sudo pacman -S --noconfirm python
    sudo pacman -S --noconfirm python-pip
    sudo pacman -S --noconfirm curl
    sudo pacman -S --noconfirm clang
    sudo pacman -S --noconfirm nodejs
    sudo pacman -S --noconfirm npm
    sudo pacman -S --noconfirm lua-language-server
    sudo pacman -S --noconfirm pyright
    sudo pacman -S --noconfirm python-pynvim
    sudo pacman -S --noconfirm python-openai
    sudo pacman -S --noconfirm python-tiktoken
}

install_apt() {
    export DEBIAN_FRONTEND=noninteractive
    sudo apt update
    pcall sudo apt-get install -y ripgrep
    pcall sudo apt-get install -y fzf
    sudo apt-get install -y cmake
    sudo apt-get install -y make
    sudo apt-get install -y git
    sudo apt-get install -y gcc
    sudo apt-get install -y python3
    sudo apt-get install -y python3-pip
    sudo apt-get install -y curl
    pcall sudo apt-get install -y clangd
    pcall sudo apt-get install -y clang-format
    pcall sudo apt-get install -y nodejs
    pcall sudo apt-get install -y npm
    install_pip
    install_npm
}

install_yum() {
    sudo yum-config-manager --add-repo=https://copr.fedorainfracloud.org/coprs/carlwgeorge/ripgrep/repo/epel-7/carlwgeorge-ripgrep-epel-7.repo
    pcall sudo yum install -y ripgrep
    pcall sudo yum install -y fzf
    sudo yum install -y cmake
    sudo yum install -y make
    sudo yum install -y git
    sudo yum install -y gcc
    sudo yum install -y python3 || sudo yum install -y python
    sudo yum install -y curl
    pcall sudo yum install -y clangd
    pcall sudo yum install -y clang-format
    pcall sudo yum install -y nodejs
    pcall sudo yum install -y npm
    install_pip
    install_npm
}


install_dnf() {
    pcall sudo dnf install -y ripgrep
    pcall sudo dnf install -y fzf
    sudo dnf install -y cmake
    sudo dnf install -y make
    sudo dnf install -y git
    sudo dnf install -y gcc
    sudo dnf install -y python3 || sudo dnf install -y python
    sudo dnf install -y curl
    pcall sudo dnf install -y clangd
    pcall sudo dnf install -y clang-format
    pcall sudo dnf install -y nodejs
    pcall sudo dnf install -y npm
    install_pip
    install_npm
}

install_zypper() {
    pcall sudo zypper in --no-confirm ripgrep || true
    pcall sudo zypper in --no-confirm fzf || true
    sudo zypper in --no-confirm cmake
    sudo zypper in --no-confirm make
    sudo zypper in --no-confirm git
    sudo zypper in --no-confirm gcc
    sudo zypper in --no-confirm python
    sudo zypper in --no-confirm curl
    pcall sudo zypper in --no-confirm clangd
    pcall sudo zypper in --no-confirm clang-format
    pcall sudo zypper in --no-confirm nodejs
    pcall sudo zypper in --no-confirm npm
    install_pip
    install_npm
}

install_brew() {
    pcall brew install ripgrep
    pcall brew install fzf
    brew install cmake
    brew install make
    brew install git
    brew install gcc
    brew install python
    brew install curl
    pcall brew install clangd
    pcall brew install clang-format
    pcall brew install node
    pcall brew install npm
    pcall brew install lua-language-server
    install_pip
    install_npm
}

do_install() {
    distro=`get_linux_distro`
    echo "-- Linux distro detected: $distro"

    if [ $distro = "Ubuntu" ]; then
        install_apt
    elif [ $distro = "Deepin" ]; then
        install_apt
    elif [ $distro = "Debian" ]; then
        install_apt
    elif [ $distro = "Kali" ]; then
        install_apt
    elif [ $distro = "Raspbian" ]; then
        install_apt
    elif [ $distro = "ArchLinux" ]; then
        install_pacman
    elif [ $distro = "ManjaroLinux" ]; then
        install_pacman
    elif [ $distro = "fedora" ]; then
        install_dnf
    elif [ $distro = "openSUSE" ]; then
        install_zypper
    elif [ $distro = "CentOS" ]; then
        install_yum
    elif [ $distro = "MacOS" ]; then
        install_brew
    else
        # TODO: add more Linux distros here..
        echo "-- WARNING: Unsupported Linux distro: $distro"
        echo "-- The script will not install any dependent packages like clangd."
        echo "-- You will have to manually install clangd, if you plan to make a working C++ IDE."
        echo "-- If you know how to install them, feel free to contribute to this GitHub repository: github.com/archibate/vimrc"
        exit 1
    fi

    echo "-- System dependency installation complete!"
}

do_install
