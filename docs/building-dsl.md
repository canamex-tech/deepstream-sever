# Building and Importing DSL

The DSL shared library is built using GCC and GNU Make. The MakeFile is located in the DSL root folder. There are a few simple steps involved in creating and installing the shared library, `libdsl.so`. The current default `make` -- prior to releasing DSL v1.0 -- builds DSL into a command line test-executable for running all API and Unit level test cases. The DSL source-only object files, built for the test application, are re-linked into the `libdsl.so` shared library.

## Contents / Steps
1. Clone this repository to pull down all source
2. Use the make (all) default to build the `dsl-test-app` executable
3. Use the `sudo make install` option to build the object files into `libdsl.so` and intall the lib
4. Generate trafficcamnet engine files (optional)
5. Import the shared lib using Python3

### Clone the Repository
Clone the repository to pull all source code required to build the DSL test application - then navigate to the `deepstream-services-library` root folder.
```
git clone https://github.com/prominenceai/deepstream-services-library.git
cd deepstream-services-library
```

***Import Note: When building with GStreamer v1.18 on Ubuntu 20.04, you will need to set the GStreamer sub version in the Makefile to 18 for the WebRTC Sink and WebSocket source code to be included in the build.***

### Make (all)
Invoke the standard make (all) to compile all source code and test scenarios into objects files, and link them into a [Catch2](https://github.com/catchorg/Catch2) test application. On successful build, the `dsl-test-app` will be found under the same root folder.

```bash
make -j$(nproc)
```

### Make and install the shared library and install dsl.py
Once the dsl-test-app.exe has been created, the source-only objects can be re-linked into the `libdsl.so` with the make install option. Root level privileges are required for the Makefile to copy the lib to `/usr/local/lib` once built. 

The DSL Python bindings file (dsl.py) is also copied to `.local/lib/python3.6/site-packages` with the following command.

```
sudo make install
```

### Generate trafficcamnet engine files (optional)

execute the python script in the `deepstream-services-library` root folder.
```bash
sudo python3 make_trafficcamnet_engine_files.py
```
**Note:** this script can take several minutes to run.

The following files are generated
```
/opt/nvidia/deepstream/deepstream/samples/models/Primary_Detector/resnet18_trafficcamnet.etlt_b8_gpu0_int8.engine
/opt/nvidia/deepstream/deepstream/samples/models/Secondary_VehicleMake/resnet18_vehiclemakenet.etlt_b8_gpu0_int8.engine
/opt/nvidia/deepstream/deepstream/samples/models/Secondary_VehicleTypes/resnet18_vehicletypenet.etlt_b8_gpu0_int8.engine
```
Update the Primary detector path specification in the script to generate files for other devices.


### Import the shared lib using Python3
The shared lib `libdsl.so` is mapped to Python3 using [CTypes](https://docs.python.org/3/library/ctypes.html) in the python module `dsl.py`. 

Import dsl into your python code.
```python
#!/usr/bin/env python
from dsl import *

# New CSI Live Camera Source
retval = dsl_source_csi_new('csi-source', 1280, 720, 30, 1)

if retval != DSL_RETURN_SUCCESS:
    print(retval)
    # --- handle error
```

## Getting Started
* [Installing DSL Dependencies](/docs/installing-dependencies.md)
* **Building and Importing DSL**

## API Reference
* [Overview](/docs/overview.md)
* [List of all Services](/docs/api-reference-list.md)
* [Pipeline](/docs/api-pipeline.md)
* [Player](/docs/api-player.md)
* [Source](/docs/api-source.md)
* [Tap](/docs/api-tap.md)
* [Dewarper](/docs/api-dewarper.md)
* [Preprocessor](/docs/api-preproc.md)
* [Inference Engine and Server](/docs/api-infer.md)
* [Tracker](/docs/api-tracker.md)
* [Segmentation Visualizer](/docs/api-segvisual.md)
* [Tiler](/docs/api-tiler.md)
* [Demuxer and Splitter Tees](/docs/api-tee.md)
* [Remuxer](/docs/api-remuxer.md)
* [On-Screen Display](/docs/api-osd.md)
* [Sink](docs/api-sink.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
* [GST Element](/docs/api-gst.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* [ODE Accumulator](/docs/api-ode-accumulator.md)
* [ODE Action ](/docs/api-ode-action.md)
* [ODE Area](/docs/api-ode-area.md)
* [ODE Heat-Mapper](/docs/api-ode-heat-mapper.md)
* [Display Type](/docs/api-display-type.md)
* [Mailer](/docs/api-mailer.md)
* [WebSocket Server](/docs/api-ws-server.md)
* [Message Broker](/docs/api-msg-broker.md)
* [Info API](/docs/api-info.md)
