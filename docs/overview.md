# DSL Overview
### Overview Contents
* [Introduction](#introduction)
* [Pipeline Components](#pipeline-components)
  * [Sources](#sources)
  * [Preprocessor](#preprocessor)
  * [Inference Engines and Servers](#inference-engines-and-servers)
  * [Multi-Object Trackers](#multi-object-trackers)
  * [Multi-Source Tiler](#multi-source-tiler)
  * [On-Screen Display](#on-screen-display)
  * [Sinks](#sinks)
  * [Tees and Branches](#tees-and-branches)
  * [Remuxer](#remuxer)
  * [Custom Components](#custom-components)
  * [Linking Components](#linking-components)
  * [Pad Probe Handlers](#pad-probe-handlers)
* [Display Types](#display-types)
* [Object Detection Event (ODE) Services](#object-detection-event-ode-services)
  * [ODE Triggers](#ode-triggers)
  * [ODE Actions](#ode-actions)
  * [ODE Areas](#ode-areas)
  * [ODE Line Crossing Analytics](#ode-line-crossing-analytics)
  * [ODE Heat Mapping](#ode-heat-mapping)
* [Selective Parallel Inference](#selective-parallel-inference)
* [Dynamic Pipelines](#dynamic-pipelines)
* [Interpipe Services](#interpipe-services)
* [Working with Buffer-Surfaces and OpenCV](#working-with-buffer-surfaces-and-opencv)
* [Smart Recording](#smart-recording)
* [RTSP Stream Connection Management](#rtsp-stream-connection-management)
* [X11 Window Services](#x11-window-services)
* [Player Services](#player-services)
* [SMTP Services](#smtp-services)
* [DSL Initialization](#dsl-initialization)
* [DSL Delete All](#dsl-delete-all)
* [Main Loop Context](#main-loop-context)
* [Service Return Codes](#service-return-codes)
* [API Reference](#api-reference)

## Introduction
The DeepStream Services Library (DSL) is best described as "the NVIDIA® DeepStream Reference Applications reimagined as a shared library of DeepStream pipeline services".

[NVIDIA’s® DeepStream SDK](https://developer.nvidia.com/deepstream-sdk) -- built on the open source [GStreamer](https://gstreamer.freedesktop.org/) "*an extremely powerful and versatile framework*<sup id="a1">[1](#f1)</sup>" -- enables experienced software developers to "*Seamlessly Develop Complex Stream Processing Pipelines*<sup id="a2">[2](#f2)</sup>". 

However, for those new to DeepStream, GStreamer comes with a learning curve that can be steep or lengthy for many. 

The core function of DSL is to provide a [simple and intuitive API](/docs/api-reference-list.md) for building, playing, and dynamically modifying NVIDIA® DeepStream Pipelines. Modifications made: (1) based on the results of the real-time video analysis, and: (2) by the application end-user through external input. An example of each:
1. Automatically starting a pre-cached recording session based on the occurrence of specific objects.
2. Interactively switching the view from one rendered Source stream to another on mouse click. 

The general approach to using DSL is to:
1. Create several uniquely named [Components](/docs/api-reference-list.md), each with a specific task to perform. 
2. Define one or more [Client callback functions](/docs/api-pipeline.md#client-callback-typedefs) and/or [Pad Probe Handlers](/docs/api-pph.md) (optional).
4. Add the Components and Callback functions to a new [Pipeline](/docs/api-pipeline.md).
5. Play the Pipeline and start/join the main execution loop.

Using Python3, for example, the above can be written as:

Create a set of Components, each with a specific function and purpose. 
```python
# new Camera Sources - setting dimensions and frames-per-second
retval += dsl_source_csi_new('my-source', 
    width=1280, height=720, fps_n=30, fps_d=1)

# create more Source Components as needed
# ...

# new Primary Inference Engine - path to config file and model engine, interval=0 - infer on every frame
retval += dsl_infer_gie_primary_new('my-pgie', 
    path_to_config_file, path_to_model_engine, interval=0)

# new Multi-Source Tiler with dimensions of width and height 
retval += dsl_tiler_new('my-tiler', width=1280, height=720)

# new On-Screen Display for inference visualization - bounding boxes and labels - 
# with both labels and clock enabled.
retval += dsl_osd_new('my-osd', text_enabled=True, clock_enabled=True,
    bbox_enabled=True, mask_enabled=False)

# new X11/EGL Window Sink for video rendering - Pipeline will create a 
# new XWindow if one is not provided.
retval += dsl_sink_window_egl_new('my-window-sink', width=1280, height=720)

if retval != DSL_RESULT_SUCCESS:
    # one of the components failed to create, handle error
```

Add the components to a new Pipeline.

```python
# Using a Null terminated list - in any order
retval = dsl_pipeline_new_component_add_many('my-pipeline', components=
    ['my-source', 'my-pgie', 'my-tiler', 'my-osd', 'my-sink', None])
```
Add one or more Client Callback Functions

```python
# Function to be called on XWindow Delete event
def xwindow_delete_event_handler(client_data):
    dsl_pipeline_stop('pipeline')
    dsl_main_loop_quit()

# add the callback function to the Window sink
retval = dsl_sink_window_delete_event_handler_add('my-pipeline', 
    xwindow_delete_event_handler, None)
```

Transition the Pipeline to a state of `PLAYING` and start/join the main loop

```python
retval = dsl_pipeline_play('my-pipeline')
if retval != DSL_RESULT_SUCCESS:
    # Pipeline failed to play, handle error
  
 # join the main loop until stopped. 
 dsl_main_loop_run()
 
 # free up all resources
 dsl_delete_all()
 ```

## Pipeline Components
There are nine primary classes of [Components](/docs/api-component.md) that can be added to a Pipeline, automatically assembled in the order shown below. Many of the classes support multiple types and in most cases multiple types can be added to a single Pipeline. 

![DSL Pipeline Components](/Images/dsl-pipeline-components.png)

## Sources
[Sources](/docs.api-source.md) are the head components for all DSL [Pipelines](/docs/api-pipeline.md) and [Players](docs/api-player.md). Pipelines must have at least one Source (and one [Sink](/docs/api-sink.md)) to transition to a state of `PLAYING`. All Pipelines have the ability to multiplex multiple source streams -- using their own built-in Stream Muxer -- as long as all Sources are of the same play-type; live vs. non-live. 

There are eleven (12) types of Source components supported, all are currently Video only. Audio-Video and Audio only Sources are currently in development.
* [App Source](/docs/api-source.md#dsl_source_app_new) - Allows the application to insert raw samples or buffers into a DSL Pipeline.
* [CSI Source](/docs/api-source.md#dsl_source_csi_new) - Camera Serial Interface (CSI) Source - Jetson platform only.
* [V4L2 Source](/docs/api-source.md#dsl_source_v4l2_new) - Stream from any V4L2 compatable device - a USB Webcam for example.
* [URI Source](/docs/api-source.md#dsl_source_uri_new) - Uniform Resource Identifier ( URI ) Source.
* [File Source](/docs/api-source.md#dsl_source_file_new) - Derived from URI Source with fixed inputs.
* [RTSP Source](/docs/api-source.md#dsl_source_rtsp_new) - Real-time Streaming Protocol ( RTSP ) Source - supports transport over TCP or UDP in unicast or multicast mode
* [Interpipe Source](/docs/api-source.md#dsl_source_interpipe_new) - Receives pipeline buffers and events from an [Interpipe Sink](/docs/api-sink.md#dsl_sink_interpipe_new). Disabled by default, requires additional [install/build steps](/docs/installing-dependencies.md).
* [Single Image Source](/docs/api-source.md#dsl_source_image_single_new) - Single frame to EOS.
* [Multi Image Source](/docs/api-source.md#dsl_source_image_multi_new) - Streamed at one image file per frame.
* [Streaming Image Source](/docs/api-source.md#dsl_source_image_stream_new)  - Single image streamed at a given frame rate. Disabled by default, requires additional [install/build steps](/docs/installing-dependencies.md).
* [Duplicate Source](/docs/api-source.md#dsl_source_duplicate_new) - Used to duplicate another Video Source so the stream can be processed differently and in parallel with the original.
* [Custom Source](/docs/api-source.md#dsl_source_custom_new) - Used to create a Custom Video Source using [GStreamer (GST) Elements](/docs/api-gst.md) created from proprietary or released GStreamer plugins.

All Sources have dimensions, width and height in pixels, and frame-rates expressed as a fractional numerator and denominator.  The URI and RTSP Source components support multiple codec formats, including H.264, H.265, and JPEG. 

A [Dewarper Component](/docs/api-dewarper.md) (not shown in the image above) capable of 360 degree and perspective dewarping can be added to any Video Source. 

All Video Sources provide programmatic control over the **formatting**, **scaling**, **cropping**, and **orienting** of their output-buffers.

A [Record-Tap](#smart-recording) (not show in the image above) can be added to a RTSP Source for cached pre-decode recording, triggered on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-pad-probe-handler) or end-user demand.

See the [Source API](/docs/api-source.md) reference section for more information.

## Preprocessor
The Preprocessor component provides a custom library interface for preprocessing input streams. Each stream can have its own preprocessing requirements. (e.g. per stream ROIs - processing Region of Interests). Streams with the same preprocessing requirements are grouped and processed together. 

NVIDIA's® plugin implementation and reference library (currently in Alpha) provide two functionalities.

* Streams with predefined ROIs (Regions of Interests) are scaled and format-converted as per the network requirements for inference. Per stream ROIs are specified in a config file.
* Raw tensor from the scaled & converted ROIs is prepared and passed to the downstream components via user metadata. Downstream plugins can access this tensor for inference.

See the [Preprocessor API](/docs/api-preproc.md) reference section for more information.

## Inference Engines and Servers
NVIDIA® GStreamer Inference Engines (GIEs) and Triton Inference Servers (TISs) perform inferencing on the input data stream. A Pipeline can have:
* Multiple Primary Gst Inference Engines (PGIE) or Primary Triton Inference Servers (PTIS) linked in succession to operate on the full frame
* Multiple Secondary Gst Inference Engines (SGIEs) or Secondary Triton Inference Servers (STISs) that can Infer-on the output of either Primary or other Secondary GIEs/TISs. 

After creation, GIEs and TISs can be updated to use a new model-engine, config file (GIE only), and/or inference interval 

With Primary GIEs and TISs, applications can add one or more [Pad Probe Handlers](#pad-probe-handlers) -- to either the sink-pad (input) or source-pad (output) -- to process the batched stream buffers and metadata for each frame and detected-object.

See the [Inference Engine and Server API](/docs/api-infer.md) reference section for more information.

DSL supports NVIDIA's [Segmentation Visualizer plugin](https://docs.nvidia.com/metropolis/deepstream/5.0DP/plugin-manual/index.html#page/DeepStream%20Plugins%20Development%20Guide/deepstream_plugin_details.3.11.html#wwpID0E0WT0HA) for viewing segmentation results produced from either a Primary Gst Inference Engine (PGIE) or Primary Triton Inference Server (TIS).

See the [Segmentation Visualizer API](/docs/api-segvisual.md) reference section for more information.

## Multi-Object Trackers
The DeepStream Services Library (DSL) supports NVIDIA's® four low-level tracker "reference implementations" all with in a single low-level library:
* **IOU** - [Intersection-Over-Unioun](https://www.researchgate.net/publication/319502501_High-Speed_Tracking-by-Detection_Without_Using_Image_Information_Challenge_winner_IWOT4S) High-Frame-Rate Tracker. 
* **NvSORT**: - NVIDIA®-enhanced Simple Online and Realtime Tracking (SORT) algorithm.
* **DeepSORT** - a re-implementation of the official DeepSORT tracker.
* **NvDCF** - an NVIDIA adapted Discriminative Correlation Filter (DCF) tracker.

Any custom library that implements the [NvDsTracker API](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvtracker.html#how-to-implement-a-custom-low-level-tracker-library) can be used as well.

Applications using Tracker components can add one or more [Pad Probe Handlers](#pad-probe-handlers) -- to either the sink-pad (input) or source-pad (output) -- to process the batched stream buffers and metadata for each frame and detected-object.

See the [Tracker API](/docs/api-tracker.md) reference section for more details. See NVIDIA's [Low-Level Tracker Comparisons and Tradeoffs](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvtracker.html#low-level-tracker-comparisons-and-tradeoffs) for additional information.

## Multi-Source Tiler
All Source components connect to the Pipeline's internal Stream Muxer -- responsible for batching multiple sources and adding the meta-data structures to each frame -- even when there is only one. When using more that one source, the muxed-batched-stream must either be Tiled **or** Demuxed before reaching an On-Screen Display or Sink component downstream.

Tiler components transform the batched-streams into a 2D grid array of tiles, one per Source component. Tilers output a single stream that can connect to a single On-Screen Display (OSD). When using a Tiler, the OSD (optional) and Sinks (minimum one) are added directly to the Pipeline or Branch to operate on the Tiler's single output stream.
```Python
# assumes all components have been created first
retval = dsl_pipeline_component_add_many('my-pipeline', 
    ['src-1', 'src-2', 'pgie', 'tiler', 'osd', 'rtsp-sink', 'window-sink', None])
```
Tilers have dimensions, width and height in pixels, and rows and columns settings. The Tiler API provides services to show a single source with [`dsl_tiler_source_show_set`](docs/api-tiler.md#dsl_tiler_source_show_set) and return to the tiled view with [`dsl_tiler_source_show_all`](/docs/api-tiler.md#dsl_tiler_source_show_all). Refer to the section on [Dynamic Pipelines](#dynamic-pipelines) for more information.

Applications using Tiler components can add one or more [Pad Probe Handlers](#pad-probe-handlers) -- to either the sink-pad (input) or source-pad (output) -- to process the batched stream buffers and metadata for each frame and detected-object.

See the [Multi-Source Tiler](/docs/api-tiler.md) reference section for additional information.

## On-Screen Display
On-Screen Display (OSD) components, using the generated object-metadata, highlight detected objects with colored bounding boxes and labels. A Clock with Positional offsets, colors and fonts can be enabled for Display. [ODE Actions](/docs/api-ode-action.md) can be used to add/update Frame and Object metadata for the OSD to display. 

See the [On-Screen Display API](/docs/api-osd.md) reference section for more information. 

Applications using On-Screen Display components can add one or more [Pad Probe Handlers](#pad-probe-handlers) -- to either the sink-pad (input) or source-pad (output) -- to process the batched stream buffers and metadata for each frame and detected-object.

##  Sinks
Sinks are the end components in each Pipeline or Branch. All Pipelines or Branches require at least one Sink Component to Play. A Fake Sink can be created if the final stream is of no interest and can simply be consumed and dropped -- a case where the `batch-meta-data` produced from the components in the Pipeline are the only data of interest. There are currently fifteen (15) types of Sink Components that can be added.

* [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new) - renders/overlays video on a Parent XWindow **(Jetson Platform Only)**... based on the 3D graphics rendering API.
* [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new) - renders/overlays video on a Parent XWindow... based on the EGL API.
* [V4L2 Sink](/docs/api-sink.md#dsl_sink_v4l2_new) - streams video to a V4L2 device or [v4l2loopback](https://github.com/umlaeute/v4l2loopback).
* [File Sink](/docs/api-sink.md#dsl_sink_file_new) - encodes video to a media container file
* [Record Sink](/docs/api-sink.md#dsl_sink_record_new) - similar to the File sink but with Start/Stop/Duration control and a cache for pre-start buffering.
* [RTMP Sink](/docs/api-sink.md#dsl_sink_record_new) - streams encoded video using the Real-time Messaging Protocol (RTMP) to social media networks, live streaming platforms, and media servers.
* [RTSP Client Sink](/docs/api-sink.md#dsl_sink_rtsp_client_new) - streams encoded video using the Real-time Streaming Protocol (RTSP) as a client of a media server. 
* [RTSP Server Sink](/docs/api-sink.md#dsl_sink_rtsp_server_new) - streams encoded video via an RTSP (UDP) Server on a specified port.
* [WebRTC Sink](/docs/api-sink.md#dsl_sink_webrtc_new) - streams encoded video to a web browser or mobile application. **(Requires GStreamer 1.18 or later)**
* [Message Sink](/docs/api-sink.md#dsl_sink_message_new) - converts Object Detection Event (ODE) metadata into a message payload and sends it to the server using a specified communication protocol.
* [Application Sink](/docs/api-sink.md#dsl_sink_app_new) - allows the application to receive buffers or samples from a DSL Pipeline.
* [Interpipe Sink](/docs/api-sink.md#dsl_sink_interpipe_new) -  allows pipeline buffers and events to flow to other independent pipelines, each with an [Interpipe Source](/docs/api-source.md#dsl_source_interpipe_new). Disabled by default, requires additional [install/build steps](/docs/installing-dependencies.md).
* [Multi-Image Sink](/docs/api-sink.md#dsl_sink_image_multi_new) - encodes and saves video frames to JPEG files at specified dimensions and frame-rate.
* [Frame-Capture Sink](/docs/api-sink.md#dsl_sink_frame_capture_new) - encodes and saves video frames to JPEG files on demand or on schedule. Disabled by default, requires additional [install/build steps](/docs/installing-dependencies.md).
* [Fake Sink](/docs/api-sink.md#dsl_sink_fake_new) - consumes/drops all data.

**3D** and **EGL** **Window Sinks** have settable dimensions, width and height in pixels, and X and Y directional offsets that can be updated after creation. 

The **Encode Sinks** (File, Record, RTMP, RTSP Client, RTSP Server, and WebRTC) support two codec formats: H.264 and H.265. Sinks that stream to file support two media container formats: MP4 and MKV  See [Smart Recording](#smart-recording) below for more information on using Record Sinks.

**RTSP Server Sinks** create RTSP servers - H.264 or H.265 - that are configured when the Pipeline is called to Play. The server is started and attached to the `main-loop` context once `[dsl_main_loop_run`](#dsl-main-loop-functions) is called. Once started, the server can accept connections based on the Sink's unique name and settings provided on creation. 

With Sinks, clients can add one or more [Pad Probe Handlers](#pad-probe-handlers) to process the batched stream buffers and metadata for each frame and detected-object -- on the input (sink pad) only.

See the [Sink API](/docs/api-sink.md) reference section for more information.

<br>

## Tees and Branches
There are two types of Tees that can be added to a Pipeline: Demuxer and Splitter.
1. **Demuxer** - used to demux the single batched frames from the Stream-muxer back into individual buffers.  
2. **Splitter** - used to split the stream, batched or otherwise, into multiple streams. It does not copy each buffer, but pushes a pointer to the same buffer to each output pad/stream. 

Branches connect to the downstream/output pads of the Tee, either as a single component in the case of a Sink or another Tee (becoming a branch), or as multiple linked components as in the case of an actual Branch. 

Important Notes: 
* Single component Branches can be added to a Tee directly, while multi-component Branches must be added to a new Branch component first.
* Branches ***can*** be added and removed from a Tee while a Pipeline is in a state of `Playing`. See the [Dynamic Pipelines](#dynamic-pipelines) section below. 
* Tees are ***not*** required when adding multiple Sinks to a Pipeline or Branch. Multi-sink management is handled by the Pipeline/Branch directly. 

See the [Demuxer and Splitter Tee API](/docs/api-tee.md) reference section for more information. 

---
## Remuxer 

The Remuxer is an aggregate component linking together a Demuxer, Tees, Streammuxers, Inference Branches, and a Metamuxer, all to implement [Selective Parallel Inference](#selective-parallel-inference). Click the link for a complete overview.

See the [Remuxer API Reference](/docs/api-remuxer.md) for more information.

---

## Custom Components
Custom Components are built with [Custom GST Elements](/docs/api-gst.md) derived from installed or proprietary GStreamer (GST) plugins. Once created, they can be added to a Pipeline along with other DSL Components. 

There are three types of Custom Components supported.
1. Custom Componets that are not Sources or Sinks - linked after the Pipeline's Streammuxer along with other DSL non Source and Sink components.
2. Custom Source Components - linked to the Pipeline's built-in Streammuxer along with other DSL Sources.
3. Custom Sink components - linked to the Pipeline or Branch's multi-sink Tee along with other DSL Sinks.

See the following reference sections for more information:
* [GST API Reference](/docs/api-gst.md)
* [Custom Component API Reference](/docs/api-component.md#custom-components)
* [Custom Source API Reference](/docs/api-source.md#custom-video-sources)
* [Custom Sink API Reference](/docs/api-sink.md#custom-video-sinks)

See the following examples for more detail:
* [pipeline_with_custom_component.py](/examples/python/pipeline_with_custom_component.py)
* [pipeline_with_custom_component.cpp](/examples/cpp/pipeline_with_custom_component.cpp)
* [pipeline_with_custom_source.py](/examples/python/pipeline_with_custom_source.py)
* [pipeline_with_custom_source.cpp](/examples/cpp/pipeline_with_custom_source.cpp)
* [pipeline_with_custom_sink.py](/examples/python/pipeline_with_custom_sink.py)
* [pipeline_with_custom_sink.cpp](/examples/cpp/pipeline_with_custom_sink.cpp)

---

## Linking Components
All Components added to the Pipeline are linked together during the call to [`dsl_pipeline_play`](/docs/api-pipeline.md#dsl_pipeline_play) and unlinked during the call to [`dsl_pipeline_stop`](/docs/api-pipeline.md#dsl_pipeline_stop). 
It is **IMPORTANT** to not that there are two methods available to link the components together as defined in `DslApi.h`.
1. `DSL_PIPELINE_LINK_METHOD_BY_ADD_ORDER` - all components are linked in the order they are added to the Pipeline (except Sources which are always linked to the Pipeline's built-in Streammuxer).
2. `DSL_PIPELINE_LINK_METHOD_BY_POSITION` - all components are linked in a specific position based on type.

The default link method is `DSL_PIPELINE_LINK_METHOD_BY_ADD_ORDER`. 

### Linking by Add-Order
When linking by add order (default):
* All Components, except for Sources, Branches, and Sinks, must be added to the Pipeline at the same time in the order they are to be linked.
* Sources are still always linked to the Streammuxer first, regardless of the order added.
* Demuxers, Tees, and Sinks must always be added last. 
* Sources and Sinks can be added/removed at runtime.
* Demuxer and Tee Branches can be added/removed at runtime.  Remuxer Branches cannot.


### Linking by Position
Linking by position allows Pipelines to be constructed or updated over subsequent calls. Using Python for example.

```python
# create the initial Pipeline
retval = dsl_pipeline_new_component_add_many('pipeline', 
    ['rtsp-source', 'primary-gie', 'egl-sink', None])

# at some point later it is determined that additional components should be
# added, perhaps after the Pipeline has been played 
retval = dsl_pipeline_component_add_many('pipeline',
    ['iou-tracker', 'on-screen-display', None])

# The Pipeline will link correctly if linking by position, 
# The Pipeline would fail to link if linked by add-order.
retval = dsl_pipeline_play('pipeline')    
```

The link-method can be changed to by positon by calling [`dsl_pipeline_link_method_set`](/docs/api-pipeline.md#dsl_pipeline_link_method_set). Click the link for more details.

When linking by position, Pipeline components are linked in the order shown in the diagram below.

![pipeline-linked-by-position-with-tiler-and-sinks](/Images/pipeline-linked-by-position-with-tiler-and-sinks.png)

| # |  Component        | Count |    Notes                                                                                   |
|---|-------------------|-------|--------------------------------------------------------------------------------------------|
| 1 | Source            | 1..n  | One Source is required to play the Pipeline. Sources are always linked to the Streammuxer. |
| 2 | Streammuxer       | 1     | Required and built into every DSL Pipeline.                                                |
| 3 | Remuxer           | 0..1  | Optional, and if added, inference components are added to the Remuxer branches.            |
| 4 | Preprocessor      | 0..1  | Optional, one at most. Always added before the Primary Inference Component(s).             |
| 5 | Primary Infer     | 0..n  | Optional, one or more Primary Inference Components linked by add order.                    |
| 6 | Tracker           | 0..1  | Optional, one at most, always linked after the Primary Inference Component(s).             |
| 7 | Secondary Infer   | 0..n  | Optional, one or more Secondary Inference Components linked as a graph.                    |
| 8 | 2D Tiler          | 0..1  | Optional, one at most. Tiler or Demuxer typically required if using multiple Sources.      | 
| 9 | On Screen Display | 0..1  | Optional, one at most. Always downstream of Tiler or in Demuxer branch.                    | 
| 10| Sinks             | 1..n  | One Sink is required to play the Pipeline. Sinks are always linked last                    | 

When using a Demuxer or Splitter Tee -- both mutually exlusive with the Tiler -- they are linked as the last element in the Pipeline as shown below. Branches are then created and added to src-pads (output) of the Demuxer or Splitter Tee. 

![pipeline-linked-by-position-with-demuxer-or-tee](/Images/pipeline-linked-by-position-with-demuxer-or-tee.png)

| #     |  Component        | Count |    Notes                                                  |
|-------|-------------------|-------|-----------------------------------------------------------|
| 1...9 | Same as above     | n     | -- components 1..9 - same notes as in the table above --  |
| 10    | Demuxer or Tee    | 0..1  | Optional, but always the last component linked if added.  |

<br>

Components, when added to a Branch, are linked in the order as shown in the diagram below.

![pipeline-linked-by-position-with-tiler-and-sinks](/Images/branch-linked-by-position-with-tiler-and-sinks.png)

| # |  Component        | Count |    Notes                                                                                   |
|---|-------------------|-------|--------------------------------------------------------------------------------------------|
| 1 | Remuxer           | 0..1  | Optional, typically added to a Pipeline with Inference branches added to the Remuxer.      |
| 2 | Preprocessor      | 0..1  | Optional, one at most. Always added before the Primary Inference Component(s).             |
| 3 | Primary Infer     | 0..n  | Optional, one or more Primary Inference Components linked by add order.                    |
| 4 | Tracker           | 0..1  | Optional, one at most, always linked after the Primary Inference Component(s).             |
| 5 | Secondary Infer   | 0..n  | Optional, one or more Secondary Inference Components linked as a graph.                    |
| 6 | 2D Tiler          | 0..1  | Optional, one at most. Tiler or Demuxer typically required if using multiple Sources.      | 
| 7 | On Screen Display | 0..1  | Optional, one at most. Always downstream of Tiler or in Demuxer branch.                    | 
| 8 | Sinks             | 1..n  | One Sink is required to play the Pipeline. Sinks are always linked last.                   | 

---

## Pad Probe Handlers
Pipeline components are linked together using directional ["pads"](https://gstreamer.freedesktop.org/documentation/gstreamer/gstpad.html?gi-language=c) with a Source Pad from one component as the producer of data connected to the Sink Pad of the next component as the consumer. Data flowing over the component’s pads can be monitored, inspected and updated using a Pad-Probe with a specific Handler function.

There are five Pad Probe Handlers that can be created and added to either a Sink or Source Pad of most Pipeline components excluding Recording Taps and Secondary GIE's.
1. [Custom PPH](/docs/api-pph.md#dsl_pph_custom_new) - allows the client to install a callback with custom behavior. 
2. [Stream Event PPH](/docs/api-pph.md#dsl_pph_stream_event_new) - allows the client to listen for and handle Stream Added, Deleted, and Ended (EOS) events.
3. [New Buffer Timeout PPH](/docs/api-pph.md#dsl_pph_buffer_timeout_new) - informs the client that a new buffer has not been received within a specified time limit.
4. [Source Meter PPH](/docs/api-pph.md#dsl_pph_meter_new) - measures the throughput for each source in the Pipeline.
5. [Object Detection Event PPH](/docs/api-pph.md#dsl_pph_ode_new) - manages a collection of [Triggers](/docs/api-ode-trigger.md) that invoke [Actions](/docs/api-ode-action.md) on the occurrence of specific frame and object metadata. 
 
See the [Pad Probe Handler API](/docs/api-pph.md) reference section for additional information.

### Custom Pad Probe Handler
Client applications can create one or more [Custom Pad Probe Handlers](/docs/api-pph.md#custom-pad-probe-handler) with callback functions to be called with every buffer that flows over a component's pad.

Using Python and [NVIDIA's python bindings](https://github.com/NVIDIA-AI-IOT/deepstream_python_apps) for example:

```Python
# Callback function to process each buffer and batch-metadata
def handle_buffer(buffer, client_data)

    # retrieve the batch metadata from the gst_buffer using NVIDIA's python bindings.
    batch_meta = pyds.gst_buffer_get_nvds_batch_meta(buffer)
    
    # cast the opaque client data back to a python object and dereference
    py_client_data = cast(client_data, POINTER(py_object)).contents.value
    
    # process/update the batch_meta as desired. 
    # ...
    
    # return true to continue processing or false to self-remove
    return true
```

```Python
# Create a new Custom PPH and add the client-callback above.
retval = dsl_pph_custom_new('custom-handler',
    client_handler=handle_buffer, client_data=my_client_data)
```

See the [complete example](/examples/python/1uri_file_pgie_iou_tracker_osd_custom_pph_window.py).

### Stream Muxer Stream Event Pad Probe Handler
The Pipeline's built-in Streammuxer sends a downstream event under the following cases: 
* The Streamux sends a `DSL_PPH_EVENT_STREAM_ADDED` event:
   * for each [Source component](/docs/api-source.md) owned by the Pipeline when it transitions to a state of PLAYING
   * and when a new Source is added at runtime.
* A `DSL_PPH_EVENT_STREAM_DELETED` event is sent if a Source is removed at runtime. 
* A `DSL_PPH_EVENT_STREAM_ENDED` event is sent when the Stream ends (EOS) including when a Source is removed at runtime.

### New Buffer Timeout Pad Probe Handler
It can be important for applications to know if a Source component -- for any reason -- has stopped receiving/producing buffers. By installing a [New Buffer Timeout Pad Probe Handler](/docs/api-pph.md#/docs/api-pph.md#new-buffer-timeout-pad-probe-handler), applications, in the event of new-buffer-timeout, can take informative and/or corrective action.

### Pipeline Meter Pad Probe Handler
The [Meter Pad Probe Handler](/docs/api-pph.md#pipeline-meter-pad-probe-handler) measures a Pipeline's throughput for each Source detected in the batched stream. When creating a Meter PPH, the client provides a callback function to be notified with new measurements at a specified interval. The notification includes the average frames-per-second over the last interval and over the current session, which can be stopped with a new session started at any time. 

### Object Detection Event Pad Probe Handler
The [Object Detection Event (ODE) Pad Probe Handler](/docs/api-pph.md#object-detection-event-ode-pad-probe-handler) manages an ordered collection of **Triggers**, each with an ordered collection of **Actions** and an optional collection of **Areas**. Together, the Triggers, Areas and Actions provide a full set of [Object Detection Event Services](#object-detection-event-ode-services). 


Refer to the [ODE Pad Probe Handler API Reference](/docs/api-pph.md) for more information.

---

## Display Types
On-Screen Display Types -- RGBA text and shapes -- can be added to a frame's metadata to be shown by an [On-Screen Display](/docs/api-osd.md) component downstream. 
There are eight base types used when creating other complete types for actual display. 
* RGBA Custom Color
* RGBA Predefined Color
* RGBA Random Color
* RGBA On-Demand Color
* RGBA Custom Color Palette
* RGBA Predefined Color Palette
* RGBA Random Color Palette
* RGBA Font

There are seven types for displaying text and shapes. 
* RGBA Text
* RGBA Line
* RGBA Multi-Line
* RGBA Arrow
* RGBA Rectangle
* RGBA Polygon
* RGBA Circle

And four types for displaying source information specific to each frame. 
* Source Stream-id
* Source Unique-id
* Source Name
* Source Dimensions

<br>

The image below provides examples of the Display Types listed above.

![RGBA Display Types](/Images/display-types.png)

Refer to the [Display Type API Reference](/docs/api-display-type.md) for more information.

---

## Object Detection Event (ODE) Services
DSL Provides an extensive set of ODE Triggers -- to Trigger on specific detection events -- and ODE Actions -- to perform specific actions when a detection event occurs. Triggers use settable criteria to process the Frame and Object metadata produced by the Primary and Secondary GIE's looking for specific detection events. When the criteria for the Trigger is met, the Trigger invokes all Actions in its ordered collection. Each unique Area and Action created can be added to multiple Triggers as shown in the diagram below. The ODE Handler has n Triggers, each Trigger has one shared Area and one unique Area, and one shared Action and one unique Action.

<img src="/Images/ode-services.png" alt="ODE Services" width="75%" height="75%">

The Handler can be added to the Pipeline before the On-Screen-Display (OSD) component allowing Actions to update the metadata for display. All Triggers can be enabled and re-enabled at runtime, either by an ODE Action, a client callback function, or directly by the application at any time.

### ODE Triggers
Current **ODE Triggers** supported:
* **Always** - triggers on every frame. Once per-frame always.
* **Absence** - triggers on the absence of objects within a frame. Once per-frame at most.
* **Occurrence** - triggers on each object detected within a frame. Once per-object at most.
* **Instance** - triggers on each new object instance across frames based on a unique tracker id. Once per new tracking id. 
* **Persistence** - triggers on each object instance that persists in view/frame for a specified period of time.
* **Summation** - triggers on the summation of all objects detected within a frame. Once per-frame always.
* **Intersection** - triggers on the intersection of two objects detected within a frame. Once per-intersecting-pair.
* **Cross** - triggers on the occurrence that an object crosses one of the Trigger's Line, Multi-Line, or Polygon Areas.
* **Count** - triggers when the count of objects within a frame is within a specified range.. Once per-frame at most.
* **New Low** - triggers when the count of objects within a frame reaches a new low count.
* **New High** trigger when the count of objects within a frame reaches a new high count.
* **Smallest** - triggers on the smallest object by area if one or more objects are detected. Once per-frame at most.
* **Largest** - triggers on the largest object by area if one or more objects are detected. Once per-frame at most.
* **Earliest** - triggers on the object that came into view the earliest (most persistent). Once per-frame at most.
* **Latest** - triggers on the object that came into view the latest (least persistent). Once per-frame at most.
* **Custom** - allows the client to provide a callback function that implements a custom "Check for Occurrence".

Triggers have optional, settable criteria and filters: 
* **Class Id** - filters on a specified GIE Class Id when checking detected objects. Use `DSL_ODE_ANY_CLASS` to disable the filter
* **Source** - filters on a unique Source name. Use `DSL_ODE_ANY_SOURCE` or NULL to disabled the filter
* **Dimensions** - filters on an object's dimensions ensuring both width and height minimums and/or maximums are met. 
* **Confidence** - filters on an object's Inference confidence or Tracker confidence requiring a minimum value.
* **Inference Component** - filters on inference metadata from a specific inference component.
* **Inference Done** - filtering on the Object's inference-done flag.
* **In-frame Areas** - filters on specific areas (see ODE Areas below) within the frame, with both areas of inclusion and exclusion supported.

Triggers have definable event-limits and frame-limits which can be set, reset when reached, and updated and anytime.  Refer to the [ODE Trigger API Reference](/docs/api-ode-trigger.md) for more information.

### ODE Actions
**ODE Actions** handle the occurrence of Object Detection Events each with a specific action under the categories below. 
* **Actions on Buffers** - capture frames and objects to JPEG images and save to file.
* **Actions on Metadata** - format and _customize object labels & bounding boxes, add text & shapes to a Frame, and much more.
* **Actions on ODE Data** - monitor, print, log, IOT message, and display ODE occurrence data on screen.
* **Actions on Recordings** - start a new recording session for a Record Tap or Sink.
* **Actions on Pipelines** - play, pause, and stop Pipelines, add/remove Sources, add/remove Sinks.
* **Actions on Players** - play, pause, and stop Players.
* **Actions on Branches** - add/remove/move Sinks and Branches.
* **Actions on Triggers** - disable/enable/reset Triggers
* **Actions on Areas** - add/remove Areas
* **Actions on Actions** - disable/enable Actions

The below screenshot, captured while running the python example [ode_persistence_and_earliest_triggers_custom_labels.py](/examples/python/ode_persistence_and_earliest_triggers_custom_labels.py), shows how ODE Triggers and Actions can be used to update the Frame and Object metadata to display event metrics.

![meta data](/Images/display-action-screenshot.png)

Refer to the [ODE Action API Reference](/docs/api-ode-action.md) for more information.

### ODE Areas
**ODE Areas**, [Lines](/docs/api-display-type.md#dsl_display_type_rgba_line_new) and [Polygons](/docs/api-display-type.md#dsl_display_type_rgba_polygon_new) can be added to any number of Triggers as additional criteria. 

* **Line Areas** - criteria is met when a specific edge of an object's bounding box - left, right, top, bottom - intersects with the Line Area
* **Polygon Areas** - criteria is met when a specific point of an object's bounding box - south, south-west, west, north-west, north, etc - is within the Polygon 

The following image was produced using: 
* An Occurrence Trigger to hide/exclude the Object Labels and Bounding Boxes for the detected objects. Note: Labels and Bounding Boxes may be disabled using the OSD API as well.
* Occurrence Trigger filtering on Person Class-Id as criteria, using:
  * A Polygon "Area of Inclusion" as additional criteria,
  * A Fill Object Action to fill the object's bounding-box with an opaque RGBA color when the Trigger criteria is met.

![Polygon Area](/Images/polygon-screenshot.png)

The above is produced with the following example

```python
# example assumes that all return values are checked before proceeding

# Create a Format Label Action to remove the Object Label from view
# Note: the label can be disabled with the OSD API as well. 
retval = dsl_ode_action_label_format_new('remove-label', 
    font=None, has_bg_color=False, bg_color=None)
            
# Create a Format Bounding Box Action to remove the box border from view
retval = dsl_ode_action_bbox_format_new('remove-border', border_width=0,
    border_color=None, has_bg_color=False, bg_color=None)

# Create an Any-Class Occurrence Trigger for our Hide Action
retval = dsl_ode_trigger_occurrence_new('every-occurrence-trigger',
    source='uri-source-1',
    class_id=DSL_ODE_ANY_CLASS,
    limit=DSL_ODE_TRIGGER_LIMIT_NONE)
retval = dsl_ode_trigger_action_add_many('every-occurrence-trigger', 
    actions=['remove-label', 'remove-border', None])

# Create the opaque red RGBA Color and "fill-object" Action to fill the bounding box
retval = dsl_display_type_rgba_color_new('opaque-red',
    red=1.0, green=0.0, blue=0.0, alpha=0.3)
retval = dsl_ode_action_fill_object_new('fill-action', color='opaque-red')

# create a list of X,Y coordinates defining the points of the Polygon.
# Polygons can have a minimum of 3, maximum of 16 points (sides)
coordinates = [dsl_coordinate(365,600), dsl_coordinate(580,620), 
    dsl_coordinate(600, 770), dsl_coordinate(180,750)]

# Create the Polygon display type using the same red RGBA Color
retval = dsl_display_type_rgba_polygon_new('polygon1', 
    coordinates=coordinates, num_coordinates=len(coordinates),
    border_width=4, color='opaque-red')
    
# New "Area of Inclusion" to use as criteria for ODE Occurrence using the Polygon object
# Test point DSL_BBOX_POINT_SOUTH = center of rectangle bottom edge must be within Polygon.
retval = dsl_ode_area_inclusion_new('polygon-area', polygon='polygon1', 
    show=True, bbox_test_point=DSL_BBOX_POINT_SOUTH)    

# New Occurrence Trigger, filtering on PERSON class_id, and with no limit on the
# number of occurrences
retval = dsl_ode_trigger_occurrence_new('person-occurrence-trigger',
    source="East Cam 1",
    class_id=PGIE_CLASS_ID_PERSON,
    limit=DSL_ODE_TRIGGER_LIMIT_NONE)

# Add the Polygon Area and Fill Object Action to the new Occurrence Trigger
retval = dsl_ode_trigger_area_add('person-occurrence-trigger', area='polygon-area')
retval = dsl_ode_trigger_action_add('person-occurrence-trigger', action='fill-action')
            
# New ODE Handler to handle all ODE Triggers with their Areas and Actions    
retval = dsl_pph_ode_new('ode-handler')
retval = dsl_pph_ode_trigger_add_many('ode-handler', 
    triggers=['any-occurrence-trigger', 'person-occurrence-trigger', None])
    
#  Then add the handler to the sink (input) pad of a Tiler.
retval = dsl_tiler_pph_add('tiler', 'ode-handler', DSL_PAD_SINK)
```

The complete example script under [/examples/python](/examples/python) can be viewed [here](/examples/python/ode_occurrence_polygon_area_inclussion_exclusion.py)

Refer to the [ODE Area API Reference](/docs/api-ode-area.md) for more information.

---

### ODE Line Crossing Analytics
The Python example [ode_line_cross_object_capture_overlay_image.py](/examples/python/ode_line_cross_object_capture_overlay_image.py) demonstrates how an [ODE Cross Trigger](/docs/api-ode-trigger.md#dsl_ode_trigger_cross_new) with an [ODE Line Area](/docs/api-ode-area.md#ode_area_line_multi_new) and [ODE Accumulator](/docs/api-ode-accumulator.md) can be used to perform line-crossing analytics. 

**Important Note:** [Multi-Line Areas](/docs/api-ode-area.md#ode_area_line_multi_new) and [Polygon Inclusion Areas](/docs/api-ode-area.md#dsl_ode_area_inclusion_new) can be used as well. 

A Cross Trigger maintains a vector of historical bounding-box coordinates for each object tracked by its unique tracking id. The Trigger, using the bounding box history and the Area's defined Test Point (SOUTH, WEST, etc.), generates an Object Trace - vector of x,y coordinates - to test for line cross with the Area's line. 

There are two methods of testing and displaying the Object Trace:
1. using `ALL` points in the vector to generate the trace to test for line-cross. 
2. using just the `END` points (earlier and latest) to generate the trace to test for line-cross. 

Note that using `ALL` points will add overhead to the processing of each detected object and considerable allocation/deallocation overhead and memory usage if displayed. `End` points are used in the below example which is why the traces appear as straight lines. The camera angle and proximity to the objects should be considered when choosing which method to use as well.

An [ODE Accumulator](/docs/api-ode-accumulator.md) with an [ODE Display Action](/docs/api-ode-action.md#dsl_ode_action_display_new) is added to the Cross Trigger to accumulate and display the number of line-crossing occurrences in the IN and OUT directions as shown in the image below.

The example creates an [ODE Print Action](/docs/api-ode-action.md#dsl_ode_action_print_new) to print each line-crossing occurrence to the console and an [ODE Capture Object Action](/docs/api-ode-action.md#dsl_ode_action_capture_object_new) with an [Image Render Player](/docs/api-player.md#dsl_player_render_image_new) to capture the object to an JPEG image file and to play/display the image.  

**Important Note:** A reminder that other actions such as the [File Action](/docs/api-ode-action.md#dsl_ode_action_file_new), [Email Action](/docs/api-ode-action.md#dsl_ode_action_email_new), and the [IOT Message Action](/docs/api-ode-action.md#dsl_ode_action_message_meta_add_new) can be leveraged with the ODE Cross Trigger as well.

![](/Images/line-cross-capture-overlay-object-image.png)

The Line Area is created with an [RGBA Line](/docs/api-display-type.md#dsl_display_type_rgba_line_new) with the line's width used as line-cross hysteresis.

```Python
# Create the RGBA Line Display Type with a width of 6 pixels for hysteresis
retval = dsl_display_type_rgba_line_new('line',
    x1=260, y1=680, x2=600, y2=660, width=6, color='opaque-red')

# Create the ODE line area to use as criteria for ODE occurrence
# Use the center point on the bounding box's bottom edge for testing
retval = dsl_ode_area_line_new('line-area', line='line',
    show=True, bbox_test_point=DSL_BBOX_POINT_SOUTH)    
```

The ODE Cross Trigger is created with a `min_frame_count` and `max_trace_points` as criteria for a line-cross occurrence.
```Python
# New Cross Trigger filtering on PERSON class_id to track and trigger on
# objects that fully cross the line. The person must be tracked for a minimum
# of 5 frames prior to crossing the line to trigger an ODE occurrence.
# The trigger can save/use up to a maximum of 200 frames of history to create
# the object's historical trace to test for line-crossing. 
retval = dsl_ode_trigger_cross_new('person-crossing-line',
    source = DSL_ODE_ANY_SOURCE,
    class_id = PGIE_CLASS_ID_PERSON,
    limit = DSL_ODE_TRIGGER_LIMIT_NONE,
    min_frame_count = 5,
    max_trace_points = 200,
    test_method = DSL_OBJECT_TRACE_TEST_METHOD_END_POINTS)

# Add the line area to the New Cross Trigger
 retval = dsl_ode_trigger_area_add('person-crossing-line', area='line-area')  

```
Each Tracked Object's historical trace can be added as display metadata for a downstream On-Screen Display to display.
```Python
# New RGBA Random Color to use for Object Trace and BBox    
retval = dsl_display_type_rgba_color_random_new('random-color',
    hue = DSL_COLOR_HUE_RANDOM,
    luminosity = DSL_COLOR_LUMINOSITY_RANDOM,
    alpha = 1.0,
    seed = 0)

# Set the Cross Trigger's view settings to enable display of the Object Trace
retval = dsl_ode_trigger_cross_view_settings_set('person-crossing-line',
    enabled=True, color='random-color', line_width=4)
```
The example creates a new [Display Action](/docs/api-ode-action.md#dsl_ode_action_display_new) and adds it to a new [ODE Accumulator](/docs/api-ode-accumulator.md). It then adds the Accumulator to the Trigger to complete the setup.
```Python
# Create a new Display Action used to display the Accumulated ODE Occurrences.
# Format the display string using the occurrences in and out tokens.
retval = dsl_ode_action_display_new('display-cross-metrics-action',
    format_string =
        "In : %" + str(DSL_METRIC_OBJECT_OCCURRENCES_DIRECTION_IN) +
        ", Out : %" + str(DSL_METRIC_OBJECT_OCCURRENCES_DIRECTION_OUT),  
    offset_x = 1200,
    offset_y = 100,
    font = 'arial-16-yellow',
    has_bg_color = False,
    bg_color = None)
           
# Create an ODE Accumulator to add to the Cross Trigger. The Accumulator
# will work with the Trigger to accumulate the IN and OUT occurrence metrics.
retval = dsl_ode_accumulator_new('cross-accumulator')
       
# Add the Display Action to the Accumulator. The Accumulator will call on
# the Display Action to display the new accumulative metrics after each frame.
retval = dsl_ode_accumulator_action_add('cross-accumulator',
    'display-cross-metrics-action')
       
# Add the Accumulator to the Line Cross Trigger.
retval = dsl_ode_trigger_accumulator_add('person-crossing-line',
    'cross-accumulator')
   
```
See the [complete example](/examples/python/ode_line_cross_object_capture_overlay_image.py) and refer to the [ODE Trigger API Reference](/docs/api-ode-trigger.md), [ODE Action API Reference](/docs/api-ode-action.md), [ODE Area API Reference](/docs/api-ode-area.md), and [ODE Accumulator API Reference](/docs/api-ode-accumulator.md) sections for more information.

<br>

---

### ODE Heat Mapping
[ODE Heat Mappers](/docs/api-ode-heat-mapper.md) are added to [ODE Triggers](/docs/api-md) to accumulate, map, and display the ODE occurrences over time. The source frame is partitioned into a configurable number of rows and columns, with each rectangle colored with a specific RGBA color value based on the number of occurrences that were detected within the corresponding area within the source frame.

The client application can `get`, `print`, `log`, `file`, and `clear` the metric occurrence data at any time.

The below image was created with the [ode_occurrence_trigger_with_heat_mapper.py](/examples/python/ode_occurrence_trigger_with_heat_mapper.py) Python example.

See the [ODE Heat-Mapper API Reference](/docs/api-ode-heat-mapper.md) for more information.

![](/Images/spectral-person-heat-map.png)

<br>

---

## Selective Parallel Inference
_Selective parallel inference_ defines a process of splitting a batched stream into parallel batched streams called branches so that each may be processed with different inference components using different model engine files. Each parallel stream can be batched with a select set or all of the original batched streams. 

The [Remuxer Component](/docs/api-remuxer.md) was developed to encapsulate the complexity of the selective linking, branching, and aggregation as shown in the diagram below. The client application creates the specific _Inference Branches_ and adds them to the Remuxer while specifying which streams to connect to.

![Parallel Inference Pipeline](/Images/parallel-inference-pipeline.png)

The example above illustrates an arbitrary use case with four _Source Components_ and three _Inference Branches_. The metadata from each inference branch is aggregated using a Metamuxer plugin and added to a branch of the original batched streams. Import The Pipeline above splits the batched stream and combined metadata after the Remuxer with 
- one branch connecting to a 2D Tiler, On Screen Display, and Window Sink for viewing, and
- the other branch connecting to a Message Sink to send the combined metadata to a Server on the cloud.

It is important to note that [GStreamer Tee plugins](https://gstreamer.freedesktop.org/documentation/coreelements/tee.html?gi-language=c) do not copy buffers, they simply push a pointer to the same buffer onto multiple source (output) pads while incrementing the reference count for each. This means that all parallel branches are processing the same shared buffers with each branch producing its own inference metadata. 

The complete "remuxing" process is outlined below with the numbered bullets corresponding to the numbers in the image above. 
1. The batched stream, on input to the Remuxer, is split using a Tee plugin with one stream connecting directly to the active sink (input) pad of the Metamuxer plugin. The active-sink-pad indicates which stream is transferred to the Metamuxer source (output) pad. The source-pad is set as a [ghost pad](https://gstreamer.freedesktop.org/documentation/gstreamer/gstghostpad.html?gi-language=c) to act as a proxy source-pad for the Remuxer component.
2. The second batched stream from the input Tee is then connected to a Demuxer to demux the batched stream back to the original 4 streams.
3. The four unbatched streams are then connected to additional Tee plugins to split the streams for parallel inference - one Tee per stream.
4. The client creates and adds Inference branches, depending on the use-case, with the above showing three.
   1. Branch-1 - Primary Triton Inference Server (PTIS).
   2. Branch-2 - Primary Triton Inference Server and IOU Tracker.
   3. Branch-3 - Primary Triton Inference Server, NvDCF Tracker, and Secondary Triton Inference Server.
5. A Streammuxer plugin, one per inference branch, is then selectively linked to some or all of the Tee's. The output of each Streammuxer is then linked to its corresponding Inference Branch. Each Streammuxer creates the base batch metadata for its specific branch. 
   1. Branch-1 - linked to process stream 0 only.
   2. Branch-2 - linked to process streams 0,1,2.
   3. Branch-3 - linked to process all streams (0-3).
6. The output of each Inference branch is then linked to the input of the Metamuxer which aggregates the metadata and adds it to the corresponding frame of the original batched stream (see first bullet).

Inference Branches may consist of a single Primary Inference Engine or Server, or multiple components as show in the diagram below.
![Inference Branch Components](/Images/inference-branch-components.png)
The following python code examples show how to create the Inference Branches -- shown in the Parallel Inference Pipeline diagram above -- and add them to a new Remuxer Component. 

**Create Inference Branch 1.**

Branch 1 consists of a single _Primary Triton Inference Server (PTIS)_ .Single Component Branches can be added to the Remuxer directly. 
```python
# Create the first PTIS using the first model with an interval of 0
retval = dsl_infer_tis_primary_new('my-primary-tis-1', primary_tis_config_file_1, 0)
```

**Create Inference Branch 2.**

Branch 2 consists of two Components, a _PTIS_ and an _IOU Tracker_. Branches with mutlple Components require an actual [Branch Component](/docs/api-branch.md) to contain them.
```python
# Create the second PTIS using the second model with an interval of 4
retval = dsl_infer_tis_primary_new('my-primary-tis-2', primary_tis_config_file_2, 4)

# New IOU Tracker, setting operational width and height.
retval = dsl_tracker_new('my-iou-tracker', iou_tracker_config_file, 480, 272)

# Create a new Branch Component and add the PTIS and Tracker to it.
retval = dsl_branch_new_component_add_many('my-branch-2', 
    ['my-primary-tis-2', 'my-iou-tracker', None])
```

**Create Inference Branch 3.**

Branch 3 consists of three Components, a _PTIS_, _NvDCF Tracker_, and Secondary _Triton Infernece Server_.
```python
# Create the third PTIS using the third model with an interval of 4
retval = dsl_infer_tis_primary_new('my-primary-tis-3', primary_tis_config_file_3, 4)

# New NvDCF Tracker, setting operational width and height
# NOTE: width and height paramaters must be multiples of 32 for dcf
retval = dsl_tracker_new('my-dcf-tracker', dcf_tracker_config_file, 640, 384)

# Create the STIS , with interval = 0
retval = dsl_infer_tis_secondary_new('my-secondary-tis', 
    secondary_infer_config_file, 'my-primary-tis-3', 0)

# Create a new Branch Component and add the PTIS, Tracker, and STIS to it.
retval = dsl_branch_new_component_add_many('my-branch-3', 
    ['my-primary-tis-3', 'my-dcf-tracker', 'my-secondary-tis', None])

```

**Create a Remuxer and add the Branches**

We can now create the Remuxer Component and the Branches to be connected to specific streams.
```python
# New Remuxer component to implement the selective parallel inference
retval = dsl_remuxer_new('remuxer')

# Branch 1 will be connected to stream 0 only.
stream_ids_b1 = [0]

# Add the first PTIS directly to the Remuxer.
# Use the "add_to" service to connect to specific streams
retval = dsl_remuxer_branch_add_to('my-remuxer', 'my-primary-tis-1', 
    stream_ids_b1, len(stream_ids_b1))

# Branch 2 will be connected to streams 0, 1, and 2.
stream_ids_b2 = [0,1,2]

# Add the second branch to the Remuxer.
# Use the "add_to" service to connect to specific streams
retval = dsl_remuxer_branch_add_to('my-remuxer', 'my-branch-2', 
    stream_ids_b2, len(stream_ids_b2))

# Branch 3 will be connected to all streams - so use the "add" service.
retval = dsl_remuxer_branch_add('my-remuxer', 'my-branch-3') 
```

**Build the Parallel Inference Pipeline**

After creating the 2D Tiler, On Screen Display, Window Sink, Message Sink, and adding them to a new Splitter Tee (not shown), we can create and build the Pipeline.

```Python
retval = dsl_pipeline_new_component_add_many('my-pipeline', 
    ['my-source-1', 'my-source-2', 'my-source-3', 'my-source-4',
    'my-remuxer', 'my-splitter-tee', None])
```

For a more complete example, see:
* [parallel_inference_on_selective_streams.py](/examples/python/parallel_inference_on_selective_streams.py)
* [parallel_inference_on_selective_streams.cpp](/examples/cpp/parallel_inference_on_selective_streams.cpp)

See the [Remuxer API Reference](/docs/api-remuxer.md) for more information.  

---

## Dynamic Pipelines
All DSL Pipelines are designed to be _dynamic-pipelines_, where key components such as Sources, Branches, and Sinks can be added and removed while the Pipeline is playing. This is as opposed to _monolithic-pipelines_ that must be fully defined at the beginning of the application. 

This section covers the following use cases for dynamically updating DSL Pipelines.

* [Dynamic Source updates when using a Tiler](#dynamic-source-updates-when-using-a-tiler).
* [Dynamic Sink updates with a Pipeline or Branch](#dynamic-sink-updates-with-a-pipeline-or-branch).
* [Dynamic Branch updates when using a Demuxer](#dynamic-branch-updates-when-using-a-demuxer).
* [Dynamic Source and Branch updates when using a Demuxer](#dynamic-source-and-branch-updates-when-using-a-demuxer).
* [Dynamic Source Stream selection when using a Tiler](#dynamic-source-stream-selection-when-using-a-tiler).
* [Dynamic Pipeline and Player Automation using ODE Services](#dynamic-pipeline-and-player-automation-using-ode-services)

### Dynamic Source updates when using a Tiler
All Pipelines require at least one [Source component](/docs/api-source.md) in order to play. Once playing, additional Sources may be added and removed. The image below illustrates a typical inference pipeline -- with _Sources_, _Streammuxer_, _Primary Inference Engine_, _IOU Tracker_, _2D Tiler_, _On-Screen-Display_, and _Window Sink_ -- with three methods for dynamically adding and removing sources: 
1. by the Application using the DSL Services API.
2. by using [Object Detection Event (ODE) Services](#object-detection-event-ode-services),
3. by enabling the end-user (Window-Sink viewer) through keyboard input. 

**IMPORTANT!** The Tiler's `columns` and `rows` properties should be set to accommodate the maximum number of Sources that will be added to the Pipeline. However, the Tiler will auto-reconfigure if a new source is added and it exceeds the space allocated for tiles.

![](/Images/dynamic-source-with-tiler.png)

#### Dynamic Source Updates by the Application
The application adds Source components to the Pipeline by calling [`dsl_pipeline_component_add`](/docs/api-pipeline.md#dsl_pipeline_component_add) or [`dsl_pipeline_component_add_many`](/docs/api-pipeline.md#dsl_pipeline_component_add_many) in any Pipeline state: STOPPED, PLAYING, or PAUSED. The Application removes Source components from the Pipeline by calling [`dsl_pipeline_component_remove`](/docs/api-pipeline.md#dsl_pipeline_component_remove) or [`dsl_pipeline_component_remove_many`](/docs/api-pipeline.md#dsl_pipeline_component_remove_many), as long as there is one Source while the Pipeline is in a state of PLAYING. 

#### Dynamic Source Updates using ODE Services
Source components can be added or removed on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-ode-services). In the example above, an [ODE Pad Probe Handler (PPH)](/docs/api-pph.md#object-detection-event-ode-pad-probe-handler) is added to the source-pad (output) of the Object Tracker. The ODE PPH will process the batch-metadata flowing over each pad. One or more [ODE Triggers](/docs/api-ode-trigger.md) are added to the ODE PPH to analyze the object-metadata and trigger on specific events. [Add-Source](/docs/api-ode-action.md#dsl_ode_action_source_add_new) and [Remove-Source](/docs/api-ode-action.md#dsl_ode_action_source_remove_new) [ODE Actions](/docs/api-ode-action.md) are added to the ODE Trigger(s) to be invoked on the occurrence of an event.

#### Dynamic Source Updates by the End-User
The Application enables the end-user (Window-Sink viewer) by adding a [`dsl_sink_window_key_event_handler_cb`](/docs/api-sink.md#dsl_sink_window_key_event_handler_cb) callback function to either a [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new) or [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new) by calling [`dsl_sink_window_key_event_handler_add`](/docs/api-sink.md#dsl_sink_window_key_event_handler_add). The callback function, called on every key-release, calls the appropriate Pipeline component add/remove service as described above. 

The following examples demonstrate how to add and remove Sources on keyboard input.
* [`dynamically_add_remove_sources_with_tiler_window_sink.py`](/examples/python/dynamically_add_remove_sources_with_tiler_window_sink.py)
* [`dynamically_add_remove_sources_with_tiler_window_sink.cpp`](/examples/cpp/dynamically_add_remove_sources_with_tiler_window_sink.cpp)

<br>

### Dynamic Sink updates with a Pipeline or Branch
When adding a Sink(s) to a Pipeline or Branch, DSL automatically inserts a Splitter Tee between the last component and the Sink(s) as shown in the image below, even if there is only one. This ensures that additional Sinks can be added (and removed) before, during or after the Pipeline is played.

As with Sources, there are three methods of dynamically adding or removing Sinks. 
1. by the Application using the DSL Services API.
2. by using [Object Detection Event (ODE) Services](#object-detection-event-ode-services),
3. by enabling the end-user (Window-Sink viewer) through keyboard input. 

![](/Images/dynamic-sinks-with-tiler.png)

#### Dynamic Sink Updates by the Application
The application adds Sink components to a Pipeline by calling [`dsl_pipeline_component_add`](/docs/api-pipeline.md#dsl_pipeline_component_add) or [`dsl_pipeline_component_add_many`](/docs/api-pipeline.md#dsl_pipeline_component_add_many) at any time. The Application removes Sink components from the Pipeline by calling [`dsl_pipeline_component_remove`](/docs/api-pipeline.md#dsl_pipeline_component_remove) or [`dsl_pipeline_component_remove_many`](/docs/api-pipeline.md#dsl_pipeline_component_remove_many).

There are similar services for adding and removing Sinks to and from a Branch. See [`dsl_branch_component_add`](api-branch.md#dsl_branch_component_add), [`dsl_branch_component_add_many`](/docs/api-branch.md#dsl_branch_component_add_many), [`dsl_branch_component_remove`](/docs/api-branch.md#dsl_branch_component_remove), [`dsl_branch_component_remove_many`](/docs/api-branch.md#dsl_branch_component_remove_many)

#### Dynamic Sink Updates using ODE Services
Sink components can be added or removed on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-ode-services). In the example above, an [ODE Pad Probe Handler (PPH)](/docs/api-pph.md#object-detection-event-ode-pad-probe-handler) is added to the source-pad (output) of the Object Tracker. The ODE PPH will process the batch-metadata flowing over each pad. One or more [ODE Triggers](/docs/api-ode-trigger.md) are added to the ODE PPH to analyze the object-metadata and trigger on specific events. [Add-Sink](/docs/api-ode-action.md#dsl_ode_action_sink_add_new) and [Remove-Sink](/docs/api-ode-action.md#dsl_ode_action_sink_remove_new) [ODE Actions](/docs/api-ode-action.md) are added to the ODE Trigger(s) to be invoked on the occurrence of an event.

#### Dynamic Sink Updates by the End-User
The Application enables the end-user (Window-Sink viewer) by adding a [`dsl_sink_window_key_event_handler_cb`](/docs/api-sink.md#dsl_sink_window_key_event_handler_cb) callback function to either a [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new) or [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new) by calling [`dsl_sink_window_key_event_handler_add`](/docs/api-sink.md#dsl_sink_window_key_event_handler_add). The callback function, called on every key-release, calls the appropriate Pipeline component add/remove service as described above. 

<br>

### Dynamic Branch updates when using a Demuxer
[Demuxer Tees](/docs/api-tee.md#demuxer-tee) demux the batched-stream -- batched together by the Pipeline's built-in Stream Muxer -- into single-stream [Branches](/docs/api-branch.md). The stream-id for each Branch matches the [stream-id for the Source](/docs/api-source.md#stream-id) producing the stream.  The maximum number of Branches can be up to the maximum number of Sources. Branches can be added to the next available unconnected pad/stream, to a specific pad/stream specified by id, or moved from one pad/stream to another.

**IMPORTANT!** When using a [Demuxer Tee](/docs/api-tee.md) , the maximum number of Branches must be specified prior to playing the Pipeline, a requirement imposed by NVIDIA's [nvstreamdemux](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvstreamdemux.html#gst-nvstreamdemux) plugin. 

**IMPORTANT!** When adding and removing Branches, a blocking pad-probe is added to block data from flowing to the Branch while linking or unlinking with the Demuxer. A [blocking-timeout](/docs/api-tee.md#constant-values) value is used to control the maximum amount of time the demuxer will wait for blocking pad-probe handler function to dynamically link or unlink the Branch. This value will need to be extended if the frame-rate for the stream is less than 2 fps. Refer to [`dsl_tee_blocking_timeout_set`](/docs/api-tee.md#dsl_tee_blocking_timeout_set) for more information. The timeout is required in case the Source upstream has been removed or is in a bad state in which case the pad-probe callback will never be called.

As with Sources and Sinks, adding and removing Branches at runtime can be done using the following three methods.
1. by the Application using the DSL Services API.
2. by using [Object Detection Event (ODE) Services](#object-detection-event-ode-services),
3. by enabling the end-user (Window-Sink viewer) through keyboard input. 

![](/Images/fixed-sources-with-demuxer-and-dynamic-branch.png)

#### Dynamic Branch Updates by the Application
With a Demuxer Tee, the Application can perform following:
* add a Branch to the first available unlinked stream by calling the base Tee service [`dsl_tee_branch_add`](/docs/api-tee.md#dsl_tee_branch_add).
* add a Branch to a specific stream by calling [`dsl_tee_demuxer_branch_add_to`](/docs/api-tee.md#dsl_tee_demuxer_branch_add_to).
* move a Branch to a specific stream by calling [`dsl_tee_demuxer_branch_move_to`](/docs/api-tee.md#dsl_tee_demuxer_branch_move_to).
* remove a Branch by calling the base Tee services [`dsl_tee_branch_remove`](/docs/api-tee.md#dsl_tee_branch_remove), [`dsl_tee_branch_remove_many`](/docs/api-tee.md#dsl_tee_branch_remove_many), or [`dsl_tee_branch_remove_all`](/docs/api-tee.md#dsl_tee_branch_remove_all).

The following examples demonstrate how to move a Branch using the [`dsl_tee_demuxer_branch_move_to`](/docs/api-tee.md#dsl_tee_demuxer_branch_move_to) services using a simple periodic timer function to mimic the on-demand behavior of an application.
* [`dynamically_move_branch_from_demuxer_stream_to_stream.py`](/examples/python/dynamically_move_branch_from_demuxer_stream_to_stream.py)
* [`dynamically_move_branch_from_demuxer_stream_to_stream.cpp`](/examples/cpp/dynamically_move_branch_from_demuxer_stream_to_stream.cpp)

#### Dynamic Branch Updates using ODE Services
Branches can be added, moved, or removed on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-ode-services). In the example above, an [ODE Pad Probe Handler (PPH)](/docs/api-pph.md#object-detection-event-ode-pad-probe-handler) is added to the source-pad (output) of the Object Tracker. The ODE PPH will process the batch-metadata flowing over each pad. One or more [ODE Triggers](/docs/api-ode-trigger.md) are added to the ODE PPH to analyze the object-metadata and trigger on specific events. The following [ODE Actions](/docs/api-ode-action.md) can be added to the ODE Trigger(s) to be invoked on the occurrence of an event.
* A Branch can be added to a Splitter or Demuxer Tee using an [Add-Branch Action](/docs/api-ode-action.md#dsl_ode_action_branch_add_new).
* A Branch can be added to the "current-stream" of a Demuxer with an [Add-Branch-to Action](/docs/api-ode-action.md#dsl_ode_action_branch_add_to_new)
* A Branch can be moved to the "current-stream" of a Demuxer with a [Move-Branch-to Action](/docs/api-ode-action.md#dsl_ode_action_branch_move_to_new)
* A Branch can be removed from a Splitter or Demuxer Tee using a [Remove-Branch Action](/docs/api-ode-action.md#dsl_ode_action_branch_remove_new)

The "current-stream" == the stream with the frame and/or object metadata that caused the event.

#### Dynamic Branch Updates by the End-User
The Application enables the end-user (Window-Sink viewer) by adding a [`dsl_sink_window_key_event_handler_cb`](/docs/api-sink.md#dsl_sink_window_key_event_handler_cb) callback function to either a [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new) or [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new) by calling [`dsl_sink_window_key_event_handler_add`](/docs/api-sink.md#dsl_sink_window_key_event_handler_add). The callback function, called on every key-release, calls the appropriate Tee add/remove service as described above. 

<br>

### Dynamic Source and Branch updates when using a Demuxer
This use case is a combination of the above cases, [Dynamic Source updates when using a Tiler](#dynamic-source-updates-when-using-a-tiler) and  [Dynamic Branch updates when using a Demuxer](#dynamic-branch-updates-when-using-a-demuxer), with some important steps to follow to ensure that both Sources and Branches can be added and removed together while the Pipeline is playing.

**IMPORTANT!** When adding a new Source and Branch, the Source must be added to the Pipeline first and allowed to fully transition to a state of Playing before adding the Branch to the Demuxer. Otherwise, adding the Branch will fail.

**IMPORTANT!** When removing a Source and Branch, the Branch must be removed from the Pipeline first while the Source is still in a state of playing. If the Source stream is in a bad state or removed first, the dynamic Branch removal will fail and the Branch will be "forcefully" removed.

<br>

###  Dynamic Source Stream selection when using a Tiler
The function of the Tiler is to compose a two-dimensional tiled frame from batched buffers based on stream-id. The Tiler is also capable of showing frames from a single source or all sources on demand. The following diagram shows a typical Inference Pipeline with a Tiler and the three methods of controlling the Tiler's Source stream selection: 
1. by the Application using the DSL Services API.
2. by using [Object Detection Event (ODE) Services](#object-detection-event-ode-services),
3. by enabling the end-user (Window-Sink viewer) through keyboard input. 

![](/Images/dynamic-tiler-output.png)

#### Dynamic Tiler Updates by the Application
With a Tiler, the Application can perform the following dynamic updates:
* show a single Source by calling [`dsl_tiler_source_show_set`](/docs/api-tiler.md#dsl_tiler_source_show_set).
* cycle through each Source by calling [`dsl_tiler_source_show_cycle`](/docs/api-tiler.md#dsl_tiler_source_show_cycle).
* show all Sources (tiled view) by calling [`dsl_tiler_source_show_all`](/docs/api-tiler.md#dsl_tiler_source_show_all)

#### Dynamic Tiler Updates using ODE Services
The Tiler's source-stream selection can be updated on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-ode-services). An [ODE Pad Probe Handler (PPH)](/docs/api-pph.md#object-detection-event-ode-pad-probe-handler) can be added to the source-pad (output) of the Object Tracker. The ODE PPH will process the batch-metadata flowing over each pad. One or more [ODE Triggers](/docs/api-ode-trigger.md) are added to the ODE PPH to analyze the object-metadata and trigger on specific events. a [Tiler-Show-Source](/docs/api-ode-action.md#dsl_ode_action_tiler_source_show_new) [ODE Action](/docs/api-ode-action.md) is added to the ODE Trigger(s) to be invoked on the occurrence of an event.

#### Dynamic Tiler Updates by the End-User
The Application enables the end-user (Window-Sink viewer)
1. by adding a [`dsl_sink_window_button_event_handler_cb`](/docs/api-sink.md##dsl_sink_window_button_event_handler_cb) callback function to either a [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new) or [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new) by calling [`dsl_sink_window_button_event_handler_add`](/docs/api-sink.md#dsl_sink_window_button_event_handler_add) The callback function, called on every mouse-button event with the current X and Y coordinates within the Window Sink, calls:
   1. [`dsl_sink_window_dimensions_get`](/docs/api-sink.md#dsl_sink_window_dimensions_get) to get the current dimensions of the Window Sink (user may have resized).
   2. [`dsl_tiler_source_show_select`](/docs/api-tiler.md#dsl_tiler_source_show_select) to show the Source corresponding to the X and Y coordinates from the mouse-click.
2. by adding a [`dsl_sink_window_key_event_handler_cb`](/docs/api-sink.md#dsl_sink_window_key_event_handler_cb) callback function to the Window Sink by calling [`dsl_sink_window_key_event_handler_add`](/docs/api-sink.md#dsl_sink_window_key_event_handler_add). The callback function, called on every keyboard key-release, calls the appropriate Tee add/remove service as described above. 

The following examples cover this use case:
* [`4uri_file_tiler_show_source_control.py`](/examples/python/4uri_file_tiler_show_source_control.py)
* [`4uri_file_tiler_show_source_control.cpp`](/examples/cpp/4uri_file_tiler_show_source_control.cpp)

<br>

### Dynamic Pipeline and Player Automation using ODE Services
Pipelines and Players may be _**played**_, _**paused**_, and _**stopped**_ on the occurrence of Object Detection Event (ODE). The image below details how an Inference Pipeline -- with _RTSP Source_, built-in _Streammuxer_, _Primary Inference Engine_, _Multi-Object Tracker_, and _Fake Sink_ -- can be used to playback a recording on detection of a Person or Face.

![](/Images/person-detection-play-player-action.png)

From the image above:
* A simple [Player](/docs/api-player.md) with _File Source_ and _Window Sink_ is set up to play a recorded video on demand.
* An [ODE Trigger](/docs/api-ode-trigger.md) with a **limit of one occurrence** is used to trigger on the detection of a Person or Face just once. Minimum Trigger criteria -- such as  dimensions or inference confidence -- can be used to minimize false positives.
* A [Play Player ODE Action](/docs/api-ode-action.md#dsl_ode_action_player_play_new) added to the ODE Trigger is used to play the Player on ODE occurrence.
* The application registers a [`dsl_player_termination_event_listener_cb`](/docs/api-player.md#dsl_player_termination_event_listener_cb) callback function with the Player to be called when the video has finished playing.
* The callback function calls [`dsl_ode_trigger_reset`](/docs/api-ode-trigger.md#dsl_ode_trigger_reset) to reset the Trigger enabling the process to repeat on the next ODE occurrence.

---

## Interpipe Services
DSL supports services for [RidgeRun's]() [Interpipe plugins](https://developer.ridgerun.com/wiki/index.php?title=GstInterpipe). From their website ***"GstInterpipe is a RidgeRun open source GStreamer plug-in that enables pipeline buffers and events to flow between two or more independent pipelines. It consists of two elements: interpipesink and interpipesrc. The interpipesrc connects with an interpipesink, from which it receives buffers and events.***

The Interpipe Sink and Source are optional/conditional DSL components.  You will need to [build and install](https://developer.ridgerun.com/wiki/index.php/GstInterpipe_-_Building_and_Installation_Guide) the RidgeRun plugins. Then update the DSL Makefile to include/build the DSL Sink and Source components. Search for the following section and set `BUILD_INTER_PIPE` to `true`,
```
# To enable the InterPipe Sink and Source components
# - set BUILD_INTER_PIPE:=true
BUILD_INTER_PIPE:=true
```

There are two examples that cover two basic use cases; **Dynamic Switching** and **Multiple Listeners**. These cases can be combined and extended to achieve complex pipelines as illustrated by RidgeRun [here](https://developer.ridgerun.com/wiki/index.php/File:Complex_pipeline.png)

### Dynamic Switching
The Interpipe Source's `listen_to` setting -- the name of the Interpipe Sink to listen to -- can be updated at any time. See the following links for examples that implement the image below.
* [interpipe_single_pipeline_dynamic_switching_between_multiple_sinks.py](/examples/python/interpipe_single_pipeline_dynamic_switching_between_multiple_sinks.py)
* [interpipe_single_pipeline_dynamic_switching_between_multiple_sinks.cpp](/examples/cpp/interpipe_single_pipeline_dynamic_switching_between_multiple_sinks.cpp)

![interpipe dynamic switching](/Images/interpipe-dynamic-switching.png)

```Python

```
### Multiple Listeners
Multiple Pipelines, each with their own Interpipe Source, can listen to the same Interpipe Sink. The following examples implement the multiple listeners use case as shown in the image below.
* [interpipe_multiple_pipelines_listening_to_single_sink.py](/examples/python/interpipe_multiple_pipelines_listening_to_single_sink.py)
* [interpipe_multiple_pipelines_listening_to_single_sink.cpp](/examples/cpp/interpipe_multiple_pipelines_listening_to_single_sink.cpp)

![interpipe multiple listeners](/Images/interpipe-multiple-listeners.png)

--- 

## Working with Buffer-Surfaces and OpenCV

### Using NVIDIA's pyds and python
NVIDIA's python bindings provides a function called [pyds.get_nvds_buf_surface()](https://docs.nvidia.com/metropolis/deepstream/5.0DP/python-api/Methods/methodsdoc.html#get-nvds-buf-surface) to get a frame-buffer-surface from a batched buffer as a Python array. The Python array, once converted to a NumPy array, can be processed using OpenCV as shown with the [Custom Pad Probe Handler](/api-pph.md)below.

```python
## 
# Custom PPH added to the sink-pad (input) of the Tiler
## 
def custom_pad_probe_handler(buffer, user_data):

    # Retrieve the batch metadata from the gst_buffer.
    # IMPORTANT! do not use the hash function to cast the buffer.
    
    batch_meta = pyds.gst_buffer_get_nvds_batch_meta(buffer)
    l_frame = batch_meta.frame_meta_list
    while l_frame is not None:
        try:
            frame_meta = pyds.glist_get_nvds_frame_meta(l_frame.data)
        except StopIteration:
            break

            # get the current frame as identified by batch_id in python array format
            n_frame = pyds.get_nvds_buf_surface(buffer, frame_meta.batch_id)

            # convert the python array into numpy array format.
            frame_image = np.array(n_frame,copy=True,order='C')

            # covert the array into cv2 default BGRA format
            frame_image = cv2.cvtColor(frame_image,cv2.COLOR_RGBA2BGRA)

            # write out the image
            cv2.imwrite(filename,frame_image)

        try:
            l_frame=l_frame.next
        except StopIteration:
            break
    return DSL_PAD_PROBE_OK
```

From the [pyds.get_nvds_buf_surface()](https://docs.nvidia.com/metropolis/deepstream/5.0DP/python-api/Methods/methodsdoc.html#get-nvds-buf-surface) documentation, _"This function returns the frame in NumPy format. **Only RGBA format is supported. For x86_64, only CUDA unified memory is supported**. For Jetson, the buffer is mapped to CPU memory."_

To meet the above requirements:

1. Use the [dsl_source_video_buffer_out_format_set](/docs/api-source.md#dsl_source_video_buffer_out_format_set) service to set the buffer out format to RGBA for each source. 

```python
# IMPORTANT! We must set the buffer format to RGBA for each source.        
retval = dsl_source_video_buffer_out_format_set('my-source-0', 
    DSL_VIDEO_FORMAT_RGBA)

```

2. (dGPU Only) Depending on which NVIDIA Streammux plugin is used, to set the buffer memory type use [dsl_component_nvbuf_mem_type_set_many](/docs/api-component.md#dsl_component_nvbuf_mem_type_set_many) or [dsl_pipeline_streammux_nvbuf_mem_type_set](/docs/api-pipeline.md#dsl_pipeline_streammux_nvbuf_mem_type_set) as shown below.
```python
# If using the new Streammux, then change the memory type of each source
if dsl_info_use_new_nvstreammux_get():
    retval = dsl_component_nvbuf_mem_type_set_many(
        ['my-source-0', 'my-source-1', 'my-source-2', 'my-source-3'],
        DSL_NVBUF_MEM_TYPE_CUDA_UNIFIED)
                
# if using the old Streammux we set the memtype of the Streammux itself.    
else:
    retval = dsl_pipeline_streammux_nvbuf_mem_type_set('pipeline',
        DSL_NVBUF_MEM_TYPE_CUDA_UNIFIED)

```

See the complete example [4file_custom_pph_using_opencv.py](/examples/python/4file_custom_pph_using_opencv.py)

### Using "DslSurfaceTransform.h and C/C++
DSL Provides a set of utility classes (used internally) to simplyfy the process of creating a frame-buffer-surface from a batched buffer that can be processed using OpenCV. The utility classes and OpenCV are used within a [Custom Pad Probe Handler](/docs/api-pph.md).

See the example [4file_custom_pph_using_opencv.cpp](/examples/cpp/4file_custom_pph_using_opencv.cpp) for complete details.

---

## Smart Recording
As mentioned above, there are two components that provide cached-video recording:
1. Record Tap - that taps into the pre-decoded stream to record the original video - added to a RTSP Source component directly.
2. Record Sink - that encodes the decoded, inferred-on, and optionally annotated stream - added to a Pipeline or Branch.

Both recording components create a fixed-size cache to buffer the last N seconds of the encoded video. Services are provided to start a recording session with a `start` point within the current cache specified in seconds before the current time and a `duration` specified in seconds from the current time. The complete duration of the recording (given sufficient time for the cache to fill) will be `start` + `duration` seconds; though a recording can be stopped at any time.  A client callback, added to the Record-Tap or Sink, is used to notify the application - first when a recording has started - and again after the recording has completed.

A smart recording can be started on the occurrence of an [Object Detection Event (ODE)](#object-detection-event-ode-services) or on user/application demand. There are examples in both Python and C/C++ for all cases.

**Python:**
* [smart_record_sink_start_session_on_ode_occurrence.py](/examples/python/smart_record_sink_start_session_on_ode_occurrence.py)
* [smart_record_sink_start_session_on_user_demand.py](/examples/python/smart_record_sink_start_session_on_user_demand.py)
* [smart_record_tap_start_session_on_ode_occurrence.py](/examples/python/smart_record_tap_start_session_on_ode_occurrence.py)
* [smart_record_tap_start_session_on_user_demand.py](/examples/python/smart_record_tap_start_session_on_user_demand.py)

**C/C++:**
* [smart_record_sink_start_session_on_ode_occurrence.py](/examples/cpp/smart_record_sink_start_session_on_ode_occurrence.cpp)
* [smart_record_sink_start_session_on_user_demand.py](/examples/cpp/smart_record_sink_start_session_on_user_demand.cpp)
* [smart_record_tap_start_session_on_ode_occurrence.py](/examples/cpp/smart_record_tap_start_session_on_ode_occurrence.cpp)
* [smart_record_tap_start_session_on_user_demand.py](/examples/cpp/smart_record_tap_start_session_on_user_demand.cpp)

When using [Object Detection Events (ODE)](#object-detection-event-ode-services), "one-time" [ODE Triggers](/docs/api-ode-trigger.md) are defined to trigger on specific occurrences of Object Detection Events, such as a person entering a predefined [ODE Area](/docs/api-ode-area.md). Start-Record [ODE Actions]() are used to start the recording on ODE occurrence. Each "one-time" trigger can be reset in the client-callback when called after each recording has finished. 

The follow example illustrates a Pipeline with multiple sources, each with a Record Tap and corresponding Occurrence Trigger with a Start-Record Action
![Record Tap](/Images/smart-recording-tap.png)

#### Using Python to implement the above.
Each Camera requires: 
* RTSP Source Component - to decode the streaming source to raw video
* Record Tap - with cache to capture pre-event video
* Occurrence Trigger - to trigger ODE occurrence on detection of an object satisfying all criteria
* Custom Action - for notification of ODE occurrence.
* Start Record Action - to start the recording

```Python
## 	
# Defines a class of all component names associated with a single RTSP Source. 
# Objects of this class will be used as "client_data" for all callback notifications.	
# defines a class of all component names associated with a single RTSP Source. 	
# The names are derived from the unique Source name	
##	
class ComponentNames:	
    def __init__(self, source):	
        self.source = source	
        self.instance_trigger = source + '-instance-trigger'
        self.record_tap = source + '-record-tap'	
        self.start_record = source + '-start-record'
        
##
# Client listener function called at the start and end of a recording session
##
def OnRecordingEvent(session_info_ptr, client_data):

    if client_data == None:
        return None

    # cast the C void* client_data back to a py_object pointer and deref
    components = cast(client_data, POINTER(py_object)).contents.value

    session_info = session_info_ptr.contents

    print('session_id: ', session_info.session_id)
    
    # If we're starting a new recording for this source
    if session_info.recording_event == DSL_RECORDING_EVENT_START:
        print('event:      ', 'DSL_RECORDING_EVENT_START')

        # In this example we will call on the Tiler to show the source that started recording.	
        retval = dsl_tiler_source_show_set('tiler', source=components.source, 
            timeout=0, has_precedence=True)	
        if (retval != DSL_RETURN_SUCCESS):
            print('Tiler show single source failed with error: ', dsl_return_value_to_string(retval))
        
    # Else, the recording session has ended for this source
    else:
        print('event:      ', 'DSL_RECORDING_EVENT_END')
        print('filename:   ', session_info.filename)
        print('dirpath:    ', session_info.dirpath)
        print('duration:   ', session_info.duration)
        print('container:  ', session_info.container_type)
        print('width:      ', session_info.width)
        print('height:     ', session_info.height)

        # if we're showing the source that started this recording
        # we can set the tiler back to showing all tiles, otherwise
        # another source has started recording and taken precedence
        retval, current_source, timeout  = dsl_tiler_source_show_get('tiler')
        if reval == DSL_RETURN_SUCCESS and current_source == components.source:
            dsl_tiler_source_show_all('tiler')

        # re-enable the one-shot trigger for the next "New Instance" of a person
        retval = dsl_ode_trigger_reset(components.instance_trigger)	
        if (retval != DSL_RETURN_SUCCESS):
            print('Failed to reset instance trigger with error:', dsl_return_value_to_string(retval))

    return None
```    
The below function creates all "1-per-source" components for a given source-name and RTSP URI.
The new Source component is added to the named Pipeline and the Trigger is added to [ODE Pad Probe Handler](/docs/api-pph.md)

```Python
##
# Function to create all "1-per-source" components, and add them to the Pipeline
# pipeline - unique name of the Pipeline to add the Source components to
# source - unique name for the RTSP Source to create
# uri - unique uri for the new RTSP Source
# ode_handler - Object Detection Event (ODE) handler to add the new Trigger and Actions to
##
def CreatePerSourceComponents(pipeline, source, rtsp_uri, ode_handler):
   
    # New Component names based on unique source name    
    components = ComponentNames(source)    

    # For each camera, create a new RTSP Source for the specific RTSP URI    
    retval = dsl_source_rtsp_new(source,     
        uri = rtsp_uri,     
        protocol = DSL_RTP_ALL,     
        skip_frames = 0,     
        drop_frame_interval = 0,     
        latency = 2000, # jitter-buffer size based on latency of 2 sec. 
        timeout = 2)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # New record tap created with our common OnRecordingEvent callback function defined above    
    retval = dsl_tap_record_new(components.record_tap,     
        outdir = './',     
        container = DSL_CONTAINER_MP4,     
        client_listener = OnRecordingEvent)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # IMPORTANT: Best to set the max-size to the maximum value we 
    # intend to use (see the xwindow_key_event_handler callback above). 
    retval = dsl_tap_record_max_size_set(components.record_tap, 
        RECORDING_START + RECORDING_DURATION)
    if retval != DSL_RETURN_SUCCESS:
        return retval    

    # IMPORTANT: Best to set the default cache-size to the maximum value we 
    # intend to use (see the xwindow_key_event_handler callback above). 
    retval = dsl_tap_record_cache_size_set(components.record_tap, RECORDING_START)
    if retval != DSL_RETURN_SUCCESS:
        return retval    

    # Add the new Tap to the Source directly    
    retval = dsl_source_rtsp_tap_add(source, tap=components.record_tap)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # Next, create the Person Instance Trigger. We will reset the trigger 
    # on DSL_RECORDING_EVENT_END. See the OnRecordingEvent() client callback 
    # function above
    retval = dsl_ode_trigger_instance_new(components.instance_trigger,     
        source=source, class_id=PGIE_CLASS_ID_PERSON, limit=1)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # Create a new Action to start the record session for this Source, 
    # with the component names as client data    
    retval = dsl_ode_action_tap_record_start_new(components.start_record,     
        record_tap=components.record_tap, start=RECORDING_START, 
        duration=RECORDING_DURATION, client_data=components)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # Add the Actions to the trigger for this source.     
    retval = dsl_ode_trigger_action_add_many(components.instance_trigger,     
        actions=[components.start_record, None])    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    

    # Add the new Source with its Record-Tap to the Pipeline    
    retval = dsl_pipeline_component_add(pipeline, source)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    
        
    # Add the new Trigger to the ODE Pad Probe Handler
    return dsl_pph_ode_trigger_add(ode_handler, components.instance_trigger)

```

The main code to create all other components and assemble the Pipeline can be written as:

```Python

        ## New Primary GIE using the filespecs above with interval = 4
        retval = dsl_infer_gie_primary_new('primary-gie', 
            primary_infer_config_file, primary_model_engine_file, 4)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New IOU Tracker, setting operational width and hieght
        retval = dsl_tracker_new('iou-tracker', iou_tracker_config_file, 480, 272)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Tiler, setting width and height, use default cols/rows set by 
        # the number of sources
        retval = dsl_tiler_new('tiler', TILER_WIDTH, TILER_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Object Detection Event (ODE) Pad Probe Handler (PPH) to manage our ODE 
        # Triggers with their ODE Actions    
        retval = dsl_pph_ode_new('ode-handler')    
        if (retval != DSL_RETURN_SUCCESS):    
            break    

         # Add our ODE Pad Probe Handler to the Sink pad of the OSD
        retval = dsl_tiler_pph_add('tiler', 
            handler='ode-handler', pad=DSL_PAD_SINK)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New OSD with text, clock and bbox display all enabled. 
        retval = dsl_osd_new('on-screen-display', 
            text_enabled=True, clock_enabled=False, 
            bbox_enabled=True, mask_enabled=False)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Window Sink, 0 x/y offsets and dimensions.
        retval = dsl_sink_window_egl_new('egl-sink', 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Live Source so best to set the Window-Sink's sync enabled setting to false.
        retval = dsl_sink_sync_enabled_set('egl-sink', False)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add the XWindow event handler functions defined above to the Window Sink
        retval = dsl_sink_window_key_event_handler_add('egl-sink', 
            xwindow_key_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_sink_window_delete_event_handler_add('egl-sink', 
            xwindow_delete_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to our pipeline    
        retval = dsl_pipeline_new_component_add_many('pipeline',     
            ['primary-gie', 'iou-tracker', 'tiler',
            'on-screen-display', 'egl-sink', None])    
        if retval != DSL_RETURN_SUCCESS:    
            break    

        # For each of our four sources, call the funtion to create the 
        # source-specific components.    
        retval = CreatePerSourceComponents('pipeline', 
            'src-1', src_url_1, 'ode-handler')    
        if (retval != DSL_RETURN_SUCCESS):    
            break    
        retval = CreatePerSourceComponents('pipeline', 
           'src-2', src_url_2, 'ode-handler')    
        if (retval != DSL_RETURN_SUCCESS):    
            break    
        retval = CreatePerSourceComponents('pipeline', 
           'src-3', src_url_3, 'ode-handler')    
        if (retval != DSL_RETURN_SUCCESS):    
            break    
        retval = CreatePerSourceComponents('pipeline', 
           'src-4', src_url_4, 'ode-handler')    
        if (retval != DSL_RETURN_SUCCESS):    
            break    

        ## Add the listener callback functions defined above    
        retval = dsl_pipeline_state_change_listener_add('pipeline', 
            state_change_listener, None)    
        if retval != DSL_RETURN_SUCCESS:    
            break    
        retval = dsl_pipeline_eos_listener_add('pipeline', eos_event_listener, None)    
        if retval != DSL_RETURN_SUCCESS:    
            break    

        # Play the pipeline    
        retval = dsl_pipeline_play('pipeline')    
        if retval != DSL_RETURN_SUCCESS:    
            break    

        dsl_main_loop_run()    
        retval = DSL_RETURN_SUCCESS    
        break    

    # Print out the final result
    print(dsl_return_value_to_string(retval))

    # Cleanup all DSL/GST resources
    dsl_delete_all()

```

---

## RTSP Stream Connection Management
RTSP Source Components have "built-in" stream connection management for detecting and resolving 
1. a failed first connection.
2. stream disconnections.   

When creating an RTSP Source, the client application can specify a `next-buffer-timeout` defined as the maximum time to wait in seconds for each new frame buffer before the Source's Stream Manager -- determining that the connection has been lost -- resets the Source and tries to reconnect.

The Stream manager uses three client settable parameters to control the connection behavior. 

1. `initial-connection-timeout` - the maximum time to wait for the RTSP Source to connect and generate the first buffer when the Pipeline is first played, in units of seconds.
2. `reconnection-sleep` - the time to sleep between failed connection attempts - also in seconds. 
3. `reconnection-timeout` - the maximum time to wait for an asynchronous state change to complete before determining that reconnection has failed - also in seconds. 

Note: Setting the reconnection timeout to a value less than the device's socket timeout can result in the Stream failing to connect. All three parameters are set to defaults when the Source is created -- defined in `dslapi.h` as:
```C
#define DSL_RTSP_FIRST_CONNECTION_TIMEOUT_S   20
#define DSL_RTSP_RECONNECTION_SLEEP_S         10
#define DSL_RTSP_RECONNECTION_TIMEOUT_S       30
```

The client can register a `state-change-listener` callback function to be notified on every change-of-state to monitor the connection process and update the reconnection parameters when needed.

Expanding on the [Smart Recording](#smart-recording) example above,

```Python
##
# Function to be called by all Source components on every change of state
# old_state - the previous state of the source prior to change
# new_state - the new state of source after the state change
# client_data - components object containing the name of the Source
##
def SourceStateChangeListener(old_state, new_state, client_data):

    # cast the C void* client_data back to a py_object pointer and deref
    components = cast(client_data, POINTER(py_object)).contents.value

    print('RTSP Source ', components.source, 'change-of-state: previous =',
        dsl_state_value_to_string(old_state), '- new =', dsl_state_value_to_string(new_state))
    
    # A change of state to NULL occurs on every disconnection and after each failed retry.
    # A change of state to PLAYING occurs on every successful connection.
    if (new_state == DSL_STATE_NULL or new_state == DSL_STATE_PLAYING):
    
        # Query the Source for it's current statistics and reconnection parameters
        retval, data = dsl_source_rtsp_connection_data_get(components.source)
        
        print('Connection data for source:', components.source)
        print('  is connected:     ', data.is_connected)
        print('  first connected:  ', time.ctime(data.first_connected))
        print('  last connected:   ', time.ctime(data.last_connected))
        print('  last disconnected:', time.ctime(data.last_disconnected))
        print('  total count:      ', data.count)
        print('  in is reconnect:  ', data.is_in_reconnect)
        print('  retries:          ', data.retries)
        print('  sleep time:       ', data.sleep,'seconds')
        print('  timeout:          ', data.timeout, 'seconds')

        if (new_state == DSL_STATE_PLAYING):
            print("setting the time to sleep between re-connection retries to 4 seconds for quick recovery")
            dsl_source_rtsp_reconnection_params_set(components.source, sleep=4, timeout=30)
            
        # If we're in a reconnection cycle, check if the number of quick recovery attempts has
        # been reached. (20 * 4 =~ 80 seconds), before backing off on the time between retries 
        elif (data.is_in_reconnect and data.retries == 20):
            print("extending the time to sleep between re-connection retries to 20 seconds")
            dsl_source_rtsp_reconnection_params_set(components.source, sleep=20, timeout=30)
```
When creating each RTSP Source component, set the Source's next-buffer-timeout, and then add the common `SourceStateChangeListener` callback to the Source with the `components` object as `client_data` to be returned on change-of-state. 

```Python
##
# Function to create all "1-per-source" components, and add them to the Pipeline
# pipeline - unique name of the Pipeline to add the Source components to
# source - unique name for the RTSP Source to create
# uri - unique uri for the new RTSP Source
# ode_handler - Object Detection Event (ODE) handler to add the new Trigger and Actions to
##
def CreatePerSourceComponents(pipeline, source, rtsp_uri, ode_handler):
   
    # New Component names based on unique source name
    components = ComponentNames(source)
    
    # For each camera, create a new RTSP Source for the specific RTSP URI    
    retval = dsl_source_rtsp_new(source,     
        uri = rtsp_uri,     
        protocol = DSL_RTP_ALL,     
        skip_frames = 0,     
        drop_frame_interval = 0,     
        latency = 1000,
        timeout = 2)    
    if (retval != DSL_RETURN_SUCCESS):    
        return retval    
        
    # Add our state change listener to the new source, with the component names as client data
    retval = dsl_source_rtsp_state_change_listener_add(source, 
        client_listener=source_state_change_listener,
        client_data=components)
    if (retval != DSL_RETURN_SUCCESS):
        return retval
        
    # ---- create the remaining components
    
```
Refer to the [Source API](/docs/api-source.md) documentation for more information. The script [ode_occurrence_4rtsp_start_record_tap_action.py](/examples/python/ode_occurrence_4rtsp_start_record_tap_action.py) provides a complete example.

---

## X11 Window Services
DSL provides X11 Window Services for Pipelines that use either a [3D Window Sink](/docs/api-sink.md#dsl_sink_window_3d_new)(Jetson only) or [EGL Window Sink](/docs/api-sink.md#dsl_sink_window_egl_new). An Application can create an XWindow - using GTK+ for example - and pass the window handle to a Window Sink prior to playing, or let each Window Sink create their own XWindow to use by default.

The client application can register callback functions to handle window events -- `ButtonPress`, `KeyRelease`, and `WindowDelete` -- caused by user interaction. 

Expanding on the [Smart Recording](#smart-recording) example above, with its four Sources and Tiled Display, the following Client callback functions provide examples of how user input can be used to control the application. 

The first callback allows the user to `select` a single source stream within the tiled view based on the positional coordinates of a `ButtonPress`. The selected stream will be shown for a specified time period, or until the window is clicked on again. A timeout value of 0 will disable the timer.

``` Python
## 
# Function to be called on XWindow Button Press event
# button - id of the button pressed, one of Button1..Button5
# x_pos - x positional coordinate relative to the windows top/left corner
# y_pos - y positional coordinate relative to the windows top/left corner
# client_data - unused. 
## 
def XWindowButtonEventHandler(button, x_pos, y_pos, client_data):
    print('button = ', button, ' pressed at x = ', x_pos, ' y = ', y_pos)
    
    # time to show the single source before returning to view all. A timeout value of 0
    # will disable the Tiler's timer and show the single source until called on again.
    global SHOW_SOURCE_TIMEOUT

    if (button == Button1):
        # get the Window Sink's current XWindow dimensions as the User may have resized it.
        retval, width, height = dsl_sink_window_dimensions_get('my-window-sink')
        
        # call the Tiler to show the source based on the x and y button coordinates relative
        # to the current window dimensions obtained from the XWindow.
        dsl_tiler_source_show_select('my-tiler',
            x_pos, y_pos, width, height, timeout=SHOW_SOURCE_TIMEOUT)
```
The second callback, called on KeyRelease, allows the user to
1. show a single source, or all
2. cycle through all sources on a time interval, 
3. quit the application. 

```Python
## 
# Function to be called on XWindow KeyRelease event
# key_string - the ASCII key string value of the key pressed and released
# client_data
## 
def XWindowKeyReleaseEventHandler(key_string, client_data):
    print('key released = ', key_string)
    
    global SHOW_SOURCE_TIMEOUT
        
    # if one of the unique source Ids, show source
    elif key_string >= '0' and key_string <= '3':
        retval, source = dsl_source_name_get(int(key_string))
        if retval == DSL_RETURN_SUCCESS:
            dsl_tiler_source_show_set('my-tiler',
                source=source, timeout=SHOW_SOURCE_TIMEOUT, has_precedence=True)
            
    # C = cycle All sources
    elif key_string.upper() == 'C':
        dsl_tiler_source_show_cycle('my-tiler', timeout=SHOW_SOURCE_TIMEOUT)

    # A = show All sources
    elif key_string.upper() == 'A':
        dsl_tiler_source_show_all('my-tiler')

    # Q or Esc = quit application
    if key_string.upper() == 'Q' or key_string == '':
        dsl_pipeline_stop('my-pipeline')
        dsl_main_loop_quit()
```
The third callback is called when the user closes/deletes the XWindow allowing the application to exit from the main-loop and delete all resources

```Python
# Function to be called on XWindow Delete event
def XWindowDeleteEventHandler(client_data):
    print('delete window event')
    dsl_pipeline_stop('my-pipeline')
    dsl_main_loop_quit()

```
The callback functions are added to one or more Window Sinks after creation. The XWindow, in this example, is set into `full-screen` mode before the Pipeline is played.

```Python
while True:

    # New Pipeline
    retval = dsl_pipeline_new('pipeline')
    if retval != DSL_RETURN_SUCCESS:
        break

    retval = dsl_sink_window_egl_new('my-window-sink', 0, 0, width=1280, height=720)
    if (retval != DSL_RETURN_SUCCESS):
        break
    # Add the XWindow event handler functions defined above
    retval = dsl_sink_window_button_event_handler_add('my-window-sink', XWindowButtonEventHandler, None)
    if retval != DSL_RETURN_SUCCESS:
        break
    retval = dsl_sink_window_key_event_handler_add('my-window-sink', XWindowKeyReleaseEventHandler, None)
    if retval != DSL_RETURN_SUCCESS:
        break
    retval = dsl_sink_window_delete_event_handler_add('my-window-sink', XWindowDeleteEventHandler, None)
    if retval != DSL_RETURN_SUCCESS:
        break

    # Set the XWindow into 'full-screen' mode for a kiosk look and feel.         
    retval = dsl_sink_window_fullscreen_enabled_set('my-window-sink', enabled=True)
    if retval != DSL_RETURN_SUCCESS:
        break
        
    # Create all other required components and add them to the Pipeline (see some examples above)
    # ...
 
    retval = dsl_pipeline_play('pipeline')
    if retval != DSL_RETURN_SUCCESS:
        break
 
    # Start/Join with main loop until released - blocking call
    dsl_main_loop_run()
    retval = DSL_RETURN_SUCCESS
    break

#print out the final result
print(dsl_return_value_to_string(retval))

# clean up all resources
dsl_delete_all()
```

<br>

---

## Player Services
Players are specialized Pipelines that simplify the processes of:
* testing/confirming camera connections and URIs
* rendering captured images and video recordings.

The following python3 example shows how to add a **Video Render Player** to a **Smart Record Sink" which will automatically playback each video on recording complete. See the [Player API Documentation](/docs/api-player.md) for more information.

```Python
# New Record-Sink that will buffer encoded video while waiting for the ODE trigger/action, 
retval = dsl_sink_record_new('record-sink', outdir="./", codec=DSL_CODEC_H265, container=DSL_CONTAINER_MKV, 
    bitrate=12000000, interval=0, client_listener=recording_event_listener)
if retval != DSL_RETURN_SUCCESS:
    break

# Create the Video Render Player with a NULL file_path to be updated by the Smart Record Sink
retval = dsl_player_render_video_new(
    name = 'video-player',
    file_path = None,
    render_type = DSL_RENDER_TYPE_3D,
    offset_x = 500, 
    offset_y = 20, 
    zoom = 50,
    repeat_enabled = False)
if retval != DSL_RETURN_SUCCESS:
    break

# Add the Player to the Record Sink. The Sink will add/queue
# the file_path to each video recording created. 
retval = dsl_sink_record_video_player_add('record-sink', 
    player='video-player')
if retval != DSL_RETURN_SUCCESS:
    break
```

See the script [ode_occurrence_object_capture_overlay_image.py](/examples/python/ode_occurrence_object_capture_overlay_image.py) for the complete example.

<br>

---

## SMTP Services
Secure outgoing SMTP email services allow clients to provide server info, credentials and header data (From, To, Cc, Subject, etc.) - settings required for an [ODE Email Action](/docs/api-ode-action.md#dsl_ode_action_email_new) to send email notifications on an [Object Detection Event (ODE) Occurrence](#object-detection-event-pad-probe-handler).

Message content is sent out using multipart mime-type. Adding attachments, including captured images, will be supported in a future release.

Refer to the [SMTP Services](/docs/api-smtp.md) for more information.

See the example script [ode_occurrence_uri_send_smtp_mail.py](/examples/python/ode_occurrence_uri_send_smtp_mail.py) for additional reference.

---

## DSL Initialization
The library is automatically initialized on **any** first call to DSL. There is no explicit init or deint service. DSL will initialize GStreamer at this time unless the calling application has already done so. 

<br>

## DSL Delete All
All DSL and GStreammer resources should be deleted on code exit by calling DSL to delete all.
```Python
dsl_delete_all()
```

<br>

## Main Loop Context
After creating all components, adding them to a Pipeline, and setting the Pipeline's state to Playing, the Application must call `dsl_main_loop_run()`. The service creates a mainloop that runs/iterates the default GLib main context to check if anything the Pipeline is watching for has happened. The main loop will be run until another thread -- typically a "client callback function" called from the Pipeline's context -- calls `dsl_main_loop_quit()`

<br>

## Service Return Codes
Most DSL services return values of type `DslReturnType`, return codes of `0` indicating success and `non-0` values indicating failure. All possible return codes are defined as symbolic constants in `DslApi.h` When using Python3, DSL provides a convenience service `dsl_return_value_to_string()` to use as there are no "C" equivalent symbolic constants or enum types in Python.  

**Note:** This convenience service is the preferred method as the return code values are subject to change

`DSL_RESULT_SUCCESS` is defined in both `DslApi.h` and `dsl.py`. The non-zero Return Codes are defined in `DslApi.h` only.

**DslApi.h**
```C
#define DSL_RESULT_SUCCESS 0

typedef uint DslReturnType
```
**Python Script**
```Python
from dsl import *

retVal = dsl_sink_rtsp_server_new('rtsp-sink', host_uri, 5400, 8554, DSL_CODEC_H264, 4000000, 0)

if dsl_return_value_to_string(retval) eq 'DSL_RESULT_SINK_NAME_NOT_UNIQUE':
    # handle error
```

<br>

## Getting Started
* [Installing Dependencies](/docs/installing-dependencies.md)
* [Building and Importing DSL](/docs/building-dsl.md)

## API Reference
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

--- 
* <b id="f1">1</b> Quote from GStreamer documentation [here](https://gstreamer.freedesktop.org/documentation/?gi-language=c). [↩](#a1)
* <b id="f2">2</b> Quote from NVIDIA's Developer Site [here](https://developer.nvidia.com/deepstream-sdk). [↩](#a2)
