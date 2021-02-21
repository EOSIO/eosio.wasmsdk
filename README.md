# EOSIO.CDT (Contract Development Toolkit)
## Version : 1.8.0-rc1

EOSIO.CDT is a toolchain for WebAssembly (WASM) and set of tools to facilitate smart contract development for the EOSIO platform. In addition to being a general purpose WebAssembly toolchain, [EOSIO](https://github.com/eosio/eos) specific optimizations are available to support building EOSIO smart contracts.  This new toolchain is built around [Clang 7](https://github.com/eosio/llvm), which means that EOSIO.CDT has the most currently available optimizations and analyses from LLVM, but as the WASM target is still considered experimental, some optimizations are incomplete or not available.

### New Introductions
As of this release two new repositories are under the suite of tools provided by **EOSIO.CDT**.  These are the [Ricardian Template Toolkit](https://github.com/eosio/ricardian-template-toolkit) and the [Ricardian Specification](https://github.com/eosio/ricardian-spec).  The **Ricardian Template Toolkit** is a set of libraries to facilitate smart contract writers in crafting their Ricardian contracts.  The Ricardian specification is the working specification for the above mentioned toolkit.  Please note that both projects are **alpha** releases and are subject to change.

### Attention
- Please see the [Upgrading Guide 1.2 to 1.3](https://eosio.github.io/eosio.cdt/latest/upgrading/1.2-to-1.3) and [Upgrading Guide 1.5 to 1.6](https://eosio.github.io/eosio.cdt/latest/upgrading/1.5-to-1.6) to be aware of any breaking changes.
- There is currently a known issue that a minimum of 2 CPU cores is required for using EOSIO.CDT

## Binary Releases
EOSIO.CDT currently supports Mac OS X brew, Linux x86_64 Debian packages, and Linux x86_64 RPM packages.

**If you have previously installed EOSIO.CDT, run the `uninstall` script (it is in the directory where you cloned EOSIO.CDT) before downloading and using the binary releases.**

### Mac OS X Brew Install
```sh
brew tap eosio/eosio.cdt
brew install eosio.cdt
```

### Mac OS X Brew Uninstall
```sh
brew remove eosio.cdt
```

### Debian Package Install
```sh
wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
sudo apt install ./eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb
```

### Debian Package Uninstall
```sh
sudo apt remove eosio.cdt
```

### RPM Package Install
```sh
wget https://github.com/eosio/eosio.cdt/releases/download/v1.7.0/eosio.cdt-1.7.0-1.el7.x86_64.rpm
sudo yum install ./eosio.cdt-1.7.0-1.el7.x86_64.rpm
```

### RPM Package Uninstall
```sh
sudo yum remove eosio.cdt
```

## Guided Installation or Building from Scratch
```sh
git clone --recursive https://github.com/eosio/eosio.cdt
cd eosio.cdt
mkdir build
cd build
cmake ..
make -j8
```

From here onward you can build your contracts code by simply exporting the `build` directory to your path, so you don't have to install globally (makes things cleaner).
Or you can install globally by running this command:

```sh
sudo make install
```

### Uninstall after manual installation

```sh
sudo rm -fr /usr/local/eosio.cdt
sudo rm -fr /usr/local/lib/cmake/eosio.cdt
sudo rm /usr/local/bin/eosio-*
```

## Installed Tools
---
* eosio-cpp
* eosio-cc
* eosio-ld
* eosio-init
* eosio-abidiff
* eosio-wasm2wast
* eosio-wast2wasm
* eosio-ranlib
* eosio-ar
* eosio-objdump
* eosio-readelf

Below tools are not installed after brew install, you get them only by building the repository and installing from scracth, [see here](#guided_installation_or_building_from_scratch)
eosio-abidiff
eosio-ranlib
eosio-ar
eosio-objdump
eosio-readelf

## Contributing

[Contributing Guide](./CONTRIBUTING.md)

[Code of Conduct](./CONTRIBUTING.md#conduct)

## License

[MIT](./LICENSE)

## Important

See [LICENSE](./LICENSE) for copyright and license terms.

All repositories and other materials are provided subject to the terms of this [IMPORTANT](./IMPORTANT.md) notice and you must familiarize yourself with its terms.  The notice contains important information, limitations and restrictions relating to our software, publications, trademarks, third-party resources, and forward-looking statements.  By accessing any of our repositories and other materials, you accept and agree to the terms of the notice.
