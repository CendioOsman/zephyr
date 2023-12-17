# Zephyr

Copyright (c) 2023, Zephyr Protocol \
Copyright (c) 2014-2023, The Monero Project \
Portions Copyright (c) 2019-2021, Haven Protocol \
Portions Copyright (c) 2012-2013 The Cryptonote developers.

## Introduction

Zephyr is a revolutionary digital currency that combines the principles of privacy and stability. Grounded firmly on the academically peer-reviewed, formally verified and empirically battle-tested Minimal Djed protocol, Zephyr embodies the first over-collateralized, private stablecoin, seamlessly incorporating the privacy features of Monero into a stablecoin system. The research and development of the Djed protocol started as a collaboration between Ergo, Emurgo and IOHK in 2020, and has since had multiple implementations and deployments (cf. [https://docs.djed.one](https://docs.djed.one)).

In simple terms, Zephyr Protocol works as follows. Users can mint or redeem Zephyr Stable Dollar (ZSD) and Zephyr Reserve Share (ZRS) coins in exchange for the base coin (ZEPH). The price for stable coins is determined by a pricing oracle, the price for reserve coins is determined using a formula based on the current reserve data.

When minting new stable/reserve coins the base coins that are exchanged (+ fee) get added to the Zephyr reserve. This reserve is the collateral backing for the stablecoin holders and is ensured to be above 400% collateral at all times. In the event that collateralization falls below 400% due to price decrease, new stable coins are unable to be minted, however the reserve coin price will drop giving incentive for users to buy reserve coins at a reduced rate.

Reserve coin holders are entitled to redeem their reserve coins for the amount of equity in the reserve based on their proportion of reserve coin holdings. This means that as stablecoins are minted and redeemed, the accumulated fees will build up in the reserve as profit for holders of the reserve coin. There is a maximum reserve ratio of 800% to prevent dilution for the reserve coin holders.

**Privacy:** Zephyr uses a cryptographically sound system to allow you to send and receive funds without your transactions being easily revealed on the blockchain (the ledger of transactions that everyone has). This ensures that your purchases, receipts, and all transfers remain private by default.

**Security:** Using the power of a distributed peer-to-peer consensus network, every transaction on the network is cryptographically secured. Individual wallets have a 25-word mnemonic seed that is only displayed once and can be written down to backup the wallet. Wallet files should be encrypted with a strong passphrase to ensure they are useless if ever stolen.

**Untraceability:** By taking advantage of ring signatures, a special property of a certain type of cryptography, Zephyr is able to ensure that transactions are not only untraceable but have an optional measure of ambiguity that ensures that transactions cannot easily be tied back to an individual user or computer.

**Decentralization:** The utility of Zephyr depends on its decentralised peer-to-peer consensus network - anyone should be able to run the zephyr software, validate the integrity of the blockchain, and participate in all aspects of the zephyr network using consumer-grade commodity hardware. Decentralization of the zephyr network is maintained by software development that minimizes the costs of running the zephyr software and inhibits the proliferation of specialized, non-commodity hardware.

## About this project

This is the core implementation of Zephyr. It is open source and completely free to use without restrictions, except for those specified in the license agreement below. There are no restrictions on anyone creating an alternative implementation of Zephyr that uses the protocol and network in a compatible manner.

As with many development projects, the repository on GitHub is considered to be the "staging" area for the latest changes. Before changes are merged into that branch on the main repository, they are tested by individual developers in their own branches, submitted as a pull request, and then subsequently tested by contributors who focus on testing and code reviews. That having been said, the repository should be carefully considered before using it in a production environment, unless there is a patch in the repository for a particular show-stopping issue you are experiencing. It is generally a better idea to use a tagged release for stability.

**Anyone is welcome to contribute to Zephyr's codebase!** If you have a fix or code change, feel free to submit it as a pull request directly to the "master" branch. In cases where the change is relatively small or does not affect other parts of the codebase, it may be merged in immediately by any one of the collaborators. On the other hand, if the change is particularly large or complex, it is expected that it will be discussed at length either well in advance of the pull request being submitted, or even directly on the pull request.

## License

See [LICENSE](LICENSE).

## Contributing

If you want to help out, see [CONTRIBUTING](docs/CONTRIBUTING.md) for a set of guidelines.

## Compiling Zephyr from source

### Dependencies

The following table summarizes the tools and libraries required to build. A
few of the libraries are also included in this repository (marked as
"Vendored"). By default, the build uses the library installed on the system
and ignores the vendored sources. However, if no library is found installed on
the system, then the vendored source will be built and used. The vendored
sources are also used for statically-linked builds because distribution
packages often include only shared library binaries (`.so`) but not static
library archives (`.a`).

| Dep         | Min. version  | Vendored | Debian/Ubuntu pkg    | Arch pkg     | Void pkg              | Fedora pkg          | Optional | Purpose         |
| ----------- | ------------- | -------- | -------------------- | ------------ | --------------------- | ------------------- | -------- | --------------- |
| GCC         | 5             | NO       | `build-essential`    | `base-devel` | `base-devel`          | `gcc`               | NO       |                 |
| CMake       | 3.5           | NO       | `cmake`              | `cmake`      | `cmake`               | `cmake`             | NO       |                 |
| pkg-config  | any           | NO       | `pkg-config`         | `base-devel` | `base-devel`          | `pkgconf`           | NO       |                 |
| Boost       | 1.58          | NO       | `libboost-all-dev`   | `boost`      | `boost-devel`         | `boost-devel`       | NO       | C++ libraries   |
| OpenSSL     | basically any | NO       | `libssl-dev`         | `openssl`    | `openssl-devel`       | `openssl-devel`     | NO       | sha256 sum      |
| libzmq      | 4.2.0         | NO       | `libzmq3-dev`        | `zeromq`     | `zeromq-devel`        | `zeromq-devel`      | NO       | ZeroMQ library  |
| OpenPGM     | ?             | NO       | `libpgm-dev`         | `libpgm`     |                       | `openpgm-devel`     | NO       | For ZeroMQ      |
| libnorm[2]  | ?             | NO       | `libnorm-dev`        |              |                       |                     | YES      | For ZeroMQ      |
| libunbound  | 1.4.16        | NO       | `libunbound-dev`     | `unbound`    | `unbound-devel`       | `unbound-devel`     | NO       | DNS resolver    |
| libsodium   | ?             | NO       | `libsodium-dev`      | `libsodium`  | `libsodium-devel`     | `libsodium-devel`   | NO       | cryptography    |
| libunwind   | any           | NO       | `libunwind8-dev`     | `libunwind`  | `libunwind-devel`     | `libunwind-devel`   | YES      | Stack traces    |
| liblzma     | any           | NO       | `liblzma-dev`        | `xz`         | `liblzma-devel`       | `xz-devel`          | YES      | For libunwind   |
| libreadline | 6.3.0         | NO       | `libreadline6-dev`   | `readline`   | `readline-devel`      | `readline-devel`    | YES      | Input editing   |
| expat       | 1.1           | NO       | `libexpat1-dev`      | `expat`      | `expat-devel`         | `expat-devel`       | YES      | XML parsing     |
| GTest       | 1.5           | YES      | `libgtest-dev`[1]    | `gtest`      | `gtest-devel`         | `gtest-devel`       | YES      | Test suite      |
| ccache      | any           | NO       | `ccache`             | `ccache`     | `ccache`              | `ccache`            | YES      | Compil. cache   |
| Doxygen     | any           | NO       | `doxygen`            | `doxygen`    | `doxygen`             | `doxygen`           | YES      | Documentation   |
| Graphviz    | any           | NO       | `graphviz`           | `graphviz`   | `graphviz`            | `graphviz`          | YES      | Documentation   |
| lrelease    | ?             | NO       | `qttools5-dev-tools` | `qt5-tools`  | `qt5-tools`           | `qt5-linguist`      | YES      | Translations    |
| libhidapi   | ?             | NO       | `libhidapi-dev`      | `hidapi`     | `hidapi-devel`        | `hidapi-devel`      | YES      | Hardware wallet |
| libusb      | ?             | NO       | `libusb-1.0-0-dev`   | `libusb`     | `libusb-devel`        | `libusbx-devel`     | YES      | Hardware wallet |
| libprotobuf | ?             | NO       | `libprotobuf-dev`    | `protobuf`   | `protobuf-devel`      | `protobuf-devel`    | YES      | Hardware wallet |
| protoc      | ?             | NO       | `protobuf-compiler`  | `protobuf`   | `protobuf`            | `protobuf-compiler` | YES      | Hardware wallet |
| libudev     | ?             | NO       | `libudev-dev`        | `systemd`    | `eudev-libudev-devel` | `systemd-devel`     | YES      | Hardware wallet |

[1] On Debian/Ubuntu `libgtest-dev` only includes sources and headers. You must
build the library binary manually. This can be done with the following command `sudo apt-get install libgtest-dev && cd /usr/src/gtest && sudo cmake . && sudo make`
then:

- on Debian:
  `sudo mv libg* /usr/lib/`
- on Ubuntu:
  `sudo mv lib/libg* /usr/lib/`

[2] libnorm-dev is needed if your zmq library was built with libnorm, and not needed otherwise

Install all dependencies at once on Debian/Ubuntu:

```
sudo apt update && sudo apt install build-essential cmake pkg-config libssl-dev libzmq3-dev libunbound-dev libsodium-dev libunwind8-dev liblzma-dev libreadline6-dev libexpat1-dev libpgm-dev qttools5-dev-tools libhidapi-dev libusb-1.0-0-dev libprotobuf-dev protobuf-compiler libudev-dev libboost-chrono-dev libboost-date-time-dev libboost-filesystem-dev libboost-locale-dev libboost-program-options-dev libboost-regex-dev libboost-serialization-dev libboost-system-dev libboost-thread-dev python3 ccache doxygen graphviz
```

Install all dependencies at once on Arch:

```
sudo pacman -Syu --needed base-devel cmake boost openssl zeromq libpgm unbound libsodium libunwind xz readline expat gtest python3 ccache doxygen graphviz qt5-tools hidapi libusb protobuf systemd
```

Install all dependencies at once on Fedora:

```
sudo dnf install gcc gcc-c++ cmake pkgconf boost-devel openssl-devel zeromq-devel openpgm-devel unbound-devel libsodium-devel libunwind-devel xz-devel readline-devel expat-devel gtest-devel ccache doxygen graphviz qt5-linguist hidapi-devel libusbx-devel protobuf-devel protobuf-compiler systemd-devel
```

Install all dependencies at once on openSUSE:

```
sudo zypper ref && sudo zypper in cppzmq-devel libboost_chrono-devel libboost_date_time-devel libboost_filesystem-devel libboost_locale-devel libboost_program_options-devel libboost_regex-devel libboost_serialization-devel libboost_system-devel libboost_thread-devel libexpat-devel libminiupnpc-devel libsodium-devel libunwind-devel unbound-devel cmake doxygen ccache fdupes gcc-c++ libevent-devel libopenssl-devel pkgconf-pkg-config readline-devel xz-devel libqt5-qttools-devel patterns-devel-C-C++-devel_C_C++
```

Install all dependencies at once on macOS with the provided Brewfile:

```
brew update && brew bundle --file=contrib/brew/Brewfile
```

FreeBSD 12.1 one-liner required to build dependencies:

```
pkg install git gmake cmake pkgconf boost-libs libzmq4 libsodium unbound
```

### Cloning the repository

Clone recursively to pull-in needed submodule(s):

```
git clone --recursive https://github.com/ZephyrProtocol/zephyr
```

If you already have a repo cloned, initialize and update:

```
cd zephyr && git submodule init && git submodule update
```

_Note_: If there are submodule differences between branches, you may need
to use `git submodule sync && git submodule update` after changing branches
to build successfully.

### Build instructions

Zephyr uses the CMake build system and a top-level [Makefile](Makefile) that
invokes cmake commands as needed.

#### On Linux and macOS

- Install the dependencies
- Change to the root of the source code directory, change to the most recent release branch, and build:

  ```bash
  cd zephyr
  git checkout <latest_release>
  make
  ```

  _Optional_: If your machine has several cores and enough memory, enable
  parallel build by running `make -j<number of threads>` instead of `make`. For
  this to be worthwhile, the machine should have one core and about 2GB of RAM
  available per thread.

  _Note_: The instructions above will compile the most stable release of the
  Zephyr software. If you would like to use and test the most recent software,
  use `git checkout master`. The master branch may contain updates that are
  both unstable and incompatible with release software, though testing is always
  encouraged.

- The resulting executables can be found in `build/release/bin`

- Add `PATH="$PATH:$HOME/zephyr/build/release/bin"` to `.profile`

- Run Zephyr with `zephyrd --detach`

- **Optional**: build and run the test suite to verify the binaries:

  ```bash
  make release-test
  ```

  _NOTE_: `core_tests` test may take a few hours to complete.

- **Optional**: to build binaries suitable for debugging:

  ```bash
  make debug
  ```

- **Optional**: to build statically-linked binaries:

  ```bash
  make release-static
  ```

Dependencies need to be built with -fPIC. Static libraries usually aren't, so you may have to build them yourself with -fPIC. Refer to their documentation for how to build them.

- **Optional**: build documentation in `doc/html` (omit `HAVE_DOT=YES` if `graphviz` is not installed):

  ```bash
  HAVE_DOT=YES doxygen Doxyfile
  ```

- **Optional**: use ccache not to rebuild translation units, that haven't really changed. Zephyr's CMakeLists.txt file automatically handles it

  ```bash
  sudo apt install ccache
  ```

#### On the Raspberry Pi

Tested on a Raspberry Pi Zero with a clean install of minimal Raspbian Stretch (2017-09-07 or later) from https://www.raspberrypi.org/downloads/raspbian/. If you are using Raspian Jessie, [please see note in the following section](#note-for-raspbian-jessie-users).

- `apt-get update && apt-get upgrade` to install all of the latest software

- Install the dependencies for Zephyr from the 'Debian' column in the table above.

- Increase the system swap size:

  ```bash
  sudo /etc/init.d/dphys-swapfile stop
  sudo nano /etc/dphys-swapfile
  CONF_SWAPSIZE=2048
  sudo /etc/init.d/dphys-swapfile start
  ```

- If using an external hard disk without an external power supply, ensure it gets enough power to avoid hardware issues when syncing, by adding the line "max_usb_current=1" to /boot/config.txt

- Clone Zephyr and checkout the most recent release version:

  ```bash
  git clone https://github.com/ZephyrProtocol/zephyr.git
  cd zephyr
  git checkout <latest_release>
  ```

- Build:

  ```bash
  USE_SINGLE_BUILDDIR=1 make release
  ```

- Wait 4-6 hours

- The resulting executables can be found in `build/release/bin`

- Add `export PATH="$PATH:$HOME/zephyr/build/release/bin"` to `$HOME/.profile`

- Run `source $HOME/.profile`

- Run Zephyr with `zephyrd --detach`

- You may wish to reduce the size of the swap file after the build has finished, and delete the boost directory from your home directory

#### _Note for Raspbian Jessie users:_

If you are using the older Raspbian Jessie image, compiling Zephyr is a bit more complicated. The version of Boost available in the Debian Jessie repositories is too old to use with Zephyr, and thus you must compile a newer version yourself. The following explains the extra steps and has been tested on a Raspberry Pi 2 with a clean install of minimal Raspbian Jessie.

- As before, `apt-get update && apt-get upgrade` to install all of the latest software, and increase the system swap size

  ```bash
  sudo /etc/init.d/dphys-swapfile stop
  sudo nano /etc/dphys-swapfile
  CONF_SWAPSIZE=2048
  sudo /etc/init.d/dphys-swapfile start
  ```

- Then, install the dependencies for Zephyr except for `libunwind` and `libboost-all-dev`

- Install the latest version of boost (this may first require invoking `apt-get remove --purge libboost*-dev` to remove a previous version if you're not using a clean install):

  ```bash
  cd
  wget https://sourceforge.net/projects/boost/files/boost/1.72.0/boost_1_72_0.tar.bz2
  tar xvfo boost_1_72_0.tar.bz2
  cd boost_1_72_0
  ./bootstrap.sh
  sudo ./b2
  ```

- Wait ~8 hours

  ```bash
  sudo ./bjam cxxflags=-fPIC cflags=-fPIC -a install
  ```

- Wait ~4 hours

- From here, follow the [general Raspberry Pi instructions](#on-the-raspberry-pi) from the "Clone Zephyr and checkout most recent release version" step.

#### On Windows:

Binaries for Windows are built on Windows using the MinGW toolchain within
[MSYS2 environment](https://www.msys2.org). The MSYS2 environment emulates a
POSIX system. The toolchain runs within the environment and _cross-compiles_
binaries that can run outside of the environment as a regular Windows
application.

**Preparing the build environment**

- Download and install the [MSYS2 installer](https://www.msys2.org), either the 64-bit or the 32-bit package, depending on your system.
- Open the MSYS shell via the `MSYS2 Shell` shortcut
- Update packages using pacman:

  ```bash
  pacman -Syu
  ```

- Exit the MSYS shell using Alt+F4
- Edit the properties for the `MSYS2 Shell` shortcut changing "msys2_shell.bat" to "msys2_shell.cmd -mingw64" for 64-bit builds or "msys2_shell.cmd -mingw32" for 32-bit builds
- Restart MSYS shell via modified shortcut and update packages again using pacman:

  ```bash
  pacman -Syu
  ```

- Install dependencies:

  To build for 64-bit Windows:

  ```bash
  pacman -S mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-zeromq mingw-w64-x86_64-libsodium mingw-w64-x86_64-hidapi mingw-w64-x86_64-unbound
  ```

  To build for 32-bit Windows:

  ```bash
  pacman -S mingw-w64-i686-toolchain make mingw-w64-i686-cmake mingw-w64-i686-boost mingw-w64-i686-openssl mingw-w64-i686-zeromq mingw-w64-i686-libsodium mingw-w64-i686-hidapi mingw-w64-i686-unbound
  ```

- Open the MingW shell via `MinGW-w64-Win64 Shell` shortcut on 64-bit Windows
  or `MinGW-w64-Win64 Shell` shortcut on 32-bit Windows. Note that if you are
  running 64-bit Windows, you will have both 64-bit and 32-bit MinGW shells.

**Cloning**

- To git clone, run:

  ```bash
  git clone --recursive https://github.com/ZephyrProtocol/zephyr.git
  ```

**Building**

- Change to the cloned directory, run:

  ```bash
  cd zephyr
  ```

- If you would like a specific [version/tag](https://github.com/ZephyrProtocol/zephyr/tags), do a git checkout for that version. eg. 'v0.18.1.2'. If you don't care about the version and just want binaries from master, skip this step:

  ```bash
  git checkout v0.18.1.2
  ```

- If you are on a 64-bit system, run:

  ```bash
  make release-static-win64
  ```

- If you are on a 32-bit system, run:

  ```bash
  make release-static-win32
  ```

- The resulting executables can be found in `build/release/bin`

- **Optional**: to build Windows binaries suitable for debugging on a 64-bit system, run:

  ```bash
  make debug-static-win64
  ```

- **Optional**: to build Windows binaries suitable for debugging on a 32-bit system, run:

  ```bash
  make debug-static-win32
  ```

- The resulting executables can be found in `build/debug/bin`

### On FreeBSD:

The project can be built from scratch by following instructions for Linux above(but use `gmake` instead of `make`).
If you are running Zephyr in a jail, you need to add `sysvsem="new"` to your jail configuration, otherwise lmdb will throw the error message: `Failed to open lmdb environment: Function not implemented`.

Zephyr is also available as a port or package as `zephyr-cli`.

### On OpenBSD:

You will need to add a few packages to your system. `pkg_add cmake gmake zeromq libiconv boost libunbound`.

The `doxygen` and `graphviz` packages are optional and require the xbase set.
Running the test suite also requires `py3-requests` package.

Build zephyr: `gmake`

Note: you may encounter the following error when compiling the latest version of Zephyr as a normal user:

```
LLVM ERROR: out of memory
c++: error: unable to execute command: Abort trap (core dumped)
```

Then you need to increase the data ulimit size to 2GB and try again: `ulimit -d 2000000`

### On NetBSD:

Check that the dependencies are present: `pkg_info -c libexecinfo boost-headers boost-libs protobuf readline libusb1 zeromq git-base pkgconf gmake cmake | more`, and install any that are reported missing, using `pkg_add` or from your pkgsrc tree. Readline is optional but worth having.

Third-party dependencies are usually under `/usr/pkg/`, but if you have a custom setup, adjust the "/usr/pkg" (below) accordingly.

Clone the zephyr repository recursively and checkout the most recent release as described above. Then build zephyr: `gmake BOOST_ROOT=/usr/pkg LDFLAGS="-Wl,-R/usr/pkg/lib" release`. The resulting executables can be found in `build/NetBSD/[Release version]/Release/bin/`.

### On Solaris:

The default Solaris linker can't be used, you have to install GNU ld, then run cmake manually with the path to your copy of GNU ld:

```bash
mkdir -p build/release
cd build/release
cmake -DCMAKE_LINKER=/path/to/ld -D CMAKE_BUILD_TYPE=Release ../..
cd ../..
```

Then you can run make as usual.

### Building portable statically linked binaries

By default, in either dynamically or statically linked builds, binaries target the specific host processor on which the build happens and are not portable to other processors. Portable binaries can be built using the following targets:

- `make release-static-linux-x86_64` builds binaries on Linux on x86_64 portable across POSIX systems on x86_64 processors
- `make release-static-linux-i686` builds binaries on Linux on x86_64 or i686 portable across POSIX systems on i686 processors
- `make release-static-linux-armv8` builds binaries on Linux portable across POSIX systems on armv8 processors
- `make release-static-linux-armv7` builds binaries on Linux portable across POSIX systems on armv7 processors
- `make release-static-linux-armv6` builds binaries on Linux portable across POSIX systems on armv6 processors
- `make release-static-win64` builds binaries on 64-bit Windows portable across 64-bit Windows systems
- `make release-static-win32` builds binaries on 64-bit or 32-bit Windows portable across 32-bit Windows systems

### Cross Compiling

You can also cross-compile static binaries on Linux for Windows and macOS with the `depends` system.

- `make depends target=x86_64-linux-gnu` for 64-bit linux binaries.
- `make depends target=x86_64-w64-mingw32` for 64-bit windows binaries.
  - Requires: `python3 g++-mingw-w64-x86-64 wine1.6 bc`
  - You also need to run:  
    `update-alternatives --set x86_64-w64-mingw32-g++ x86_64-w64-mingw32-g++-posix && update-alternatives --set x86_64-w64-mingw32-gcc x86_64-w64-mingw32-gcc-posix`
- `make depends target=x86_64-apple-darwin11` for macOS binaries.
  - Requires: `cmake imagemagick libcap-dev librsvg2-bin libz-dev libbz2-dev libtiff-tools python-dev`
- `make depends target=i686-linux-gnu` for 32-bit linux binaries.
  - Requires: `g++-multilib bc`
- `make depends target=i686-w64-mingw32` for 32-bit windows binaries.
  - Requires: `python3 g++-mingw-w64-i686`
- `make depends target=arm-linux-gnueabihf` for armv7 binaries.
  - Requires: `g++-arm-linux-gnueabihf`
- `make depends target=aarch64-linux-gnu` for armv8 binaries.
  - Requires: `g++-aarch64-linux-gnu`
- `make depends target=riscv64-linux-gnu` for RISC V 64 bit binaries.
  - Requires: `g++-riscv64-linux-gnu`
- `make depends target=x86_64-unknown-freebsd` for freebsd binaries.
  - Requires: `clang-8`
- `make depends target=arm-linux-android` for 32bit android binaries
- `make depends target=aarch64-linux-android` for 64bit android binaries

The required packages are the names for each toolchain on apt. Depending on your distro, they may have different names. The `depends` system has been tested on Ubuntu 18.04 and 20.04.

Using `depends` might also be easier to compile Zephyr on Windows than using MSYS. Activate Windows Subsystem for Linux (WSL) with a distro (for example Ubuntu), install the apt build-essentials and follow the `depends` steps as depicted above.

The produced binaries still link libc dynamically. If the binary is compiled on a current distribution, it might not run on an older distribution with an older installation of libc. Passing `-DBACKCOMPAT=ON` to cmake will make sure that the binary will run on systems having at least libc version 2.17.

### Gitian builds

See [contrib/gitian/README.md](contrib/gitian/README.md).

## Running zephyrd

The build places the binary in `bin/` sub-directory within the build directory
from which cmake was invoked (repository root by default). To run in the
foreground:

```bash
./bin/zephyrd
```

To list all available options, run `./bin/zephyrd --help`. Options can be
specified either on the command line or in a configuration file passed by the
`--config-file` argument. To specify an option in the configuration file, add
a line with the syntax `argumentname=value`, where `argumentname` is the name
of the argument without the leading dashes, for example, `log-level=1`.

To run in background:

```bash
./bin/zephyrd --log-file zephyrd.log --detach
```

To run as a systemd service, copy
[zephyrd.service](utils/systemd/zephyrd.service) to `/etc/systemd/system/` and
[zephyrd.conf](utils/conf/zephyrd.conf) to `/etc/`. The [example
service](utils/systemd/zephyrd.service) assumes that the user `zephyr` exists
and its home is the data directory specified in the [example
config](utils/conf/zephyrd.conf).

If you're on Mac, you may need to add the `--max-concurrency 1` option to
zephyr-wallet-cli, and possibly zephyrd, if you get crashes refreshing.

## Internationalization

See [README.i18n.md](docs/README.i18n.md).

## Using Tor

> There is a new, still experimental, [integration with Tor](docs/ANONYMITY_NETWORKS.md). The
> feature allows connecting over IPv4 and Tor simultaneously - IPv4 is used for
> relaying blocks and relaying transactions received by peers whereas Tor is
> used solely for relaying transactions received over local RPC. This provides
> privacy and better protection against surrounding node (sybil) attacks.

While Zephyr isn't made to integrate with Tor, it can be used wrapped with torsocks, by
setting the following configuration parameters and environment variables:

- `--p2p-bind-ip 127.0.0.1` on the command line or `p2p-bind-ip=127.0.0.1` in
  zephyrd.conf to disable listening for connections on external interfaces.
- `--no-igd` on the command line or `no-igd=1` in zephyrd.conf to disable IGD
  (UPnP port forwarding negotiation), which is pointless with Tor.
- `DNS_PUBLIC=tcp` or `DNS_PUBLIC=tcp://x.x.x.x` where x.x.x.x is the IP of the
  desired DNS server, for DNS requests to go over TCP, so that they are routed
  through Tor. When IP is not specified, zephyrd uses the default list of
  servers defined in [src/common/dns_utils.cpp](src/common/dns_utils.cpp).
- `TORSOCKS_ALLOW_INBOUND=1` to tell torsocks to allow zephyrd to bind to interfaces
  to accept connections from the wallet. On some Linux systems, torsocks
  allows binding to localhost by default, so setting this variable is only
  necessary to allow binding to local LAN/VPN interfaces to allow wallets to
  connect from remote hosts. On other systems, it may be needed for local wallets
  as well.
- Do NOT pass `--detach` when running through torsocks with systemd, (see
  [utils/systemd/zephyrd.service](utils/systemd/zephyrd.service) for details).
- If you use the wallet with a Tor daemon via the loopback IP (eg, 127.0.0.1:9050),
  then use `--untrusted-daemon` unless it is your own hidden service.

Example command line to start zephyrd through Tor:

```bash
DNS_PUBLIC=tcp torsocks zephyrd --p2p-bind-ip 127.0.0.1 --no-igd
```

A helper script is in contrib/tor/zephyr-over-tor.sh. It assumes Tor is installed
already, and runs Tor and Zephyr with the right configuration.

### Using Tor on Tails

TAILS ships with a very restrictive set of firewall rules. Therefore, you need
to add a rule to allow this connection too, in addition to telling torsocks to
allow inbound connections. Full example:

```bash
sudo iptables -I OUTPUT 2 -p tcp -d 127.0.0.1 -m tcp --dport 18081 -j ACCEPT
DNS_PUBLIC=tcp torsocks ./zephyrd --p2p-bind-ip 127.0.0.1 --no-igd --rpc-bind-ip 127.0.0.1 \
    --data-dir /home/amnesia/Persistent/your/directory/to/the/blockchain
```

## Pruning

One can store a pruned blockchain in order to reduce needed hard disk space.
A pruned blockchain can only serve part of the historical chain data to other peers, but is otherwise identical in
functionality to the full blockchain.
To use a pruned blockchain, it is best to start the initial sync with `--prune-blockchain`. However, it is also possible
to prune an existing blockchain using the `zephyr-blockchain-prune` tool or using the `--prune-blockchain` `zephyrd` option
with an existing chain. If an existing chain exists, pruning will temporarily require disk space to store both the full
and pruned blockchains.

For more detailed information see the ['Pruning' entry in the Moneropedia](https://www.getmonero.org/resources/moneropedia/pruning.html)

## Debugging

This section contains general instructions for debugging failed installs or problems encountered with Zephyr. First, ensure you are running the latest version built from the GitHub repo.

### Obtaining stack traces and core dumps on Unix systems

We generally use the tool `gdb` (GNU debugger) to provide stack trace functionality, and `ulimit` to provide core dumps in builds which crash or segfault.

- To use `gdb` in order to obtain a stack trace for a build that has stalled:

Run the build.

Once it stalls, enter the following command:

```bash
gdb /path/to/zephyrd `pidof zephyrd`
```

Type `thread apply all bt` within gdb in order to obtain the stack trace

- If however the core dumps or segfaults:

Enter `ulimit -c unlimited` on the command line to enable unlimited filesizes for core dumps

Enter `echo core | sudo tee /proc/sys/kernel/core_pattern` to stop cores from being hijacked by other tools

Run the build.

When it terminates with an output along the lines of "Segmentation fault (core dumped)", there should be a core dump file in the same directory as zephyrd. It may be named just `core`, or `core.xxxx` with numbers appended.

You can now analyse this core dump with `gdb` as follows:

```bash
gdb /path/to/zephyrd /path/to/dumpfile`
```

Print the stack trace with `bt`

- If a program crashed and cores are managed by systemd, the following can also get a stack trace for that crash:

```bash
coredumpctl -1 gdb
```

#### To run Zephyr within gdb:

Type `gdb /path/to/zephyrd`

Pass command-line options with `--args` followed by the relevant arguments

Type `run` to run zephyrd

### Analysing memory corruption

There are two tools available:

#### ASAN

Configure Zephyr with the -D SANITIZE=ON cmake flag, eg:

```bash
cd build/debug && cmake -D SANITIZE=ON -D CMAKE_BUILD_TYPE=Debug ../..
```

You can then run the zephyr tools normally. Performance will typically halve.

#### valgrind

Install valgrind and run as `valgrind /path/to/zephyrd`. It will be very slow.

### LMDB

Instructions for debugging suspected blockchain corruption as per @HYC

There is an `mdb_stat` command in the LMDB source that can print statistics about the database but it's not routinely built. This can be built with the following command:

```bash
cd ~/zephyr/external/db_drivers/liblmdb && make
```

The output of `mdb_stat -ea <path to blockchain dir>` will indicate inconsistencies in the blocks, block_heights and block_info table.

The output of `mdb_dump -s blocks <path to blockchain dir>` and `mdb_dump -s block_info <path to blockchain dir>` is useful for indicating whether blocks and block_info contain the same keys.

These records are dumped as hex data, where the first line is the key and the second line is the data.

# Known Issues

## Protocols

### Socket-based

Because of the nature of the socket-based protocols that drive zephyr, certain protocol weaknesses are somewhat unavoidable at this time. While these weaknesses can theoretically be fully mitigated, the effort required (the means) may not justify the ends. As such, please consider taking the following precautions if you are a zephyr node operator:

- Run `zephyrd` on a "secured" machine. If operational security is not your forte, at a very minimum, have a dedicated a computer running `zephyrd` and **do not** browse the web, use email clients, or use any other potentially harmful apps on your `zephyrd` machine. **Do not click links or load URL/MUA content on the same machine**. Doing so may potentially exploit weaknesses in commands which accept "localhost" and "127.0.0.1".
- If you plan on hosting a public "remote" node, start `zephyrd` with `--restricted-rpc`. This is a must.

### Blockchain-based

Certain blockchain "features" can be considered "bugs" if misused correctly. Consequently, please consider the following:

- When receiving zephyr, be aware that it may be locked for an arbitrary time if the sender elected to, preventing you from spending that zephyr until the lock time expires. You may want to hold off acting upon such a transaction until the unlock time lapses. To get a sense of that time, you can consider the remaining blocktime until unlock as seen in the `show_transfers` command.
