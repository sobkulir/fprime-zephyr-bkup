# Port of F' onto Zephyr

_Currently doesn't compile yet, stay tuned_


This is a minimal application running F' (v3.2.0) on Zephyr (v3.3.0). The example deployment is under `fprime/MyDeployment`.
This port is untested and unfinished, no OSAL features have been implemented and only Baremetal F' scheduler is used.

## Requirements

The development is dockerized, but the device fallthrough into the docker container was only tested on
Ubuntu. Our versions:
* Ubuntu 22.04 LTS
* Docker 20.10.23

## Initialization

The first step is to initialize the workspace folder (`zephyr-workspace`) where
the ``fprime-zephyr-app`` and all Zephyr modules will be cloned. Run the following
command:

```shell
# Setup and clone
mkdir zephyr-workspace && cd zephyr-workspace
git clone git@github.com:sobkulir/fprime-zephyr-app.git

# Run the image from the convenience script. It has reasonable defaults.
cd fprime-zephyr-app
./run_dev.py
```

Inside the docker container:
```shell
west init -l .
west update
```

## Building and running

If you don't have a board, build and run for qemu:
```shell
rm -rf build && west build -b qemu_cortex_m3 fprime/MyDeployment/
west build -t run
```

If you have a board, e.g., `nucleo_h723zg`:
```shell
rm -rf build && west build -b nucleo_h723zg fprime/MyDeployment/
sudo west flash
```

Running `fprime-gds` over UART:
```
sudo fprime-gds --uart-device /dev/ttyUSB0 --uart-baud 115200 --dictionary build/MyDeployment/Top/MyDeploymentTopologyAppDictionary.xml -n --comm-adapter uart

```

## Helpers

Deleting Autocoder generated files

```
ls -d fprime/Autocoders/Python/src/fprime_ac/generators/templates/**/**.py | sed '/.*__init__.py/d' | xargs -I{} rm -r "{}"
```

Build fs shell

```
west build -b nucleo_h723zg samples/subsys/shell/fs/ -- -DDTC_OVERLAY_FILE=/zephyr-workspace/fprime-zephyr-app/fprime/MyDeployment/boards/nucleo_h723zg.overlay
```