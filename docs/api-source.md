# Source API Reference
Sources are the head components for all DSL Pipelines. Pipelines must have at least one source in use - among other components - to transition to a state of playing. DSL supports the following Source types.

### Camera Sources:
* Camera Serial Interface ( CSI ) - Jetson platform only.
* Universal Serial Bus ( USB )

### Decode Sources:
* Uniform Resource Identifier ( URI )
* Video File
* Real-time Streaming Protocol ( RTSP )

### Image Sources:
* Single Image ( single frame to EOS )
* Multi Image ( streamed at one image file per frame )
* Streaming Image ( single image streamed at a given frame rate )

### Application Source: 
* Used by applications to insert data into a DSL pipeline.

### Interpipe Source:
* Interpipe - requires additional install/build steps for the RidgeRun `gst-interpipe` plugins. Refer to the [Interpipe Services](/docs/overview.md#interpipe-services) overview for more information.

#### Source Construction and Destruction
Sources are created using one of six type-specific [constructors](#constructors). As with all components, Streaming Sources must be uniquely named from all other Pipeline components created.

Sources are added to a Pipeline by calling [dsl_pipeline_component_add](api-pipeline.md#dsl_pipeline_component_add) or [dsl_pipeline_component_add_many](api-pipeline.md]#dsl_pipeline_component_add_many) and removed with [dsl_pipeline_component_remove](api-pipeline.md#dsl_pipeline_component_remove), [dsl_pipeline_component_remove_many](api-pipeline.md#dsl_pipeline_component_remove_many), or [dsl_pipeline_component_remove_all]((api-pipeline.md)#dsl_pipeline_component_remove_all).

When adding multiple sources to a Pipeline, all must have the same `is_live` setting; `true` or `false`. The add services will fail on the first exception.

The relationship between Pipelines and Sources is one-to-many. Once added to a Pipeline, a Source must be removed before it can be used with another. All sources are deleted by calling [dsl_component_delete](api-component.md#dsl_component_delete), [dsl_component_delete_many](api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](api-component.md#dsl_component_delete_all). Calling a delete service on a Source `in-use` by a Pipeline will fail.

#### Sources and Demuxers
When using a [Demuxer](/docs/api-tiler.md), vs. a Tiler component, each demuxed source stream must have one or more downstream [Sink](/docs/api-sink) components to end the stream.

## Source API
**Typedefs**
* [dsl_rtsp_connection_data](#dsl_rtsp_connection_data)

**Client Callback Typedefs**
* [dsl_source_app_need_data_handler_cb](#dsl_source_app_need_data_handler_cb)
* [dsl_source_app_enough_data_handler_cb](#dsl_source_app_enough_data_handler_cb)
* [dsl_state_change_listener_cb](#dsl_state_change_listener_cb)

**Constructors:**
* [dsl_source_app_new](#dsl_source_app_new)
* [dsl_source_csi_new](#dsl_source_csi_new)
* [dsl_source_usb_new](#dsl_source_usb_new)
* [dsl_source_uri_new](#dsl_source_uri_new)
* [dsl_source_rtsp_new](#dsl_source_rtsp_new)
* [dsl_source_file_new](#dsl_source_file_new)
* [dsl_source_image_new](#dsl_source_image_new)
* [dsl_source_image_multi_new](#dsl_source_image_multi_new)
* [dsl_source_image_stream_new](#dsl_source_image_stream_new)
* [dsl_source_interpipe_new](#dsl_source_interpipe_new)

**methods:**
* [dsl_source_app_data_handlers_add](#dsl_source_app_data_handlers_add)
* [dsl_source_app_data_handlers_remove](#dsl_source_app_data_handlers_remove)
* [dsl_source_app_buffer_push](#dsl_source_app_buffer_push)
* [dsl_source_app_sample_push](#dsl_source_app_sample_push)
* [dsl_source_app_eos](#dsl_source_app_eos)
* [dsl_source_app_buffer_format_get](#dsl_source_app_buffer_format_get)
* [dsl_source_app_buffer_format_set](#dsl_source_app_buffer_format_set)
* [dsl_source_app_block_enabled_get](#dsl_source_app_block_enabled_get)
* [dsl_source_app_block_enabled_set](#dsl_source_app_block_enabled_set)
* [dsl_source_app_current_level_bytes_get](#dsl_source_app_current_level_bytes_get)
* [dsl_source_app_max_level_bytes_get](#dsl_source_app_max_level_bytes_get)
* [dsl_source_app_max_level_bytes_set](#dsl_source_app_max_level_bytes_set)
* [dsl_source_csi_sensor_id_get](#dsl_source_csi_sensor_id_get)
* [dsl_source_csi_sensor_id_set](#dsl_source_csi_sensor_id_set)
* [dsl_source_usb_device_location_get](#dsl_source_usb_device_location_get)
* [dsl_source_usb_device_location_set](#dsl_source_usb_device_location_set)
* [dsl_source_decode_uri_get](#dsl_source_decode_uri_get)
* [dsl_source_decode_uri_set](#dsl_source_decode_uri_set)
* [dsl_source_decode_drop_farme_interval_get](#dsl_source_decode_drop_farme_interval_get)
* [dsl_source_decode_drop_farme_interval_set](#dsl_source_decode_drop_farme_interval_set)
* [dsl_source_decode_dewarper_add](#dsl_source_decode_dewarper_add)
* [dsl_source_decode_dewarper_remove](#dsl_source_decode_dewarper_remove)
* [dsl_source_rtsp_timeout_get](#dsl_source_rtsp_timeout_get)
* [dsl_source_rtsp_timeout_set](#dsl_source_rtsp_timeout_set)
* [dsl_source_rtsp_reconnection_params_get](#dsl_source_rtsp_reconnection_params_get)
* [dsl_source_rtsp_reconnection_params_set](#dsl_source_rtsp_reconnection_params_set)
* [dsl_source_rtsp_connection_data_get](#dsl_source_rtsp_connection_data_get)
* [dsl_source_rtsp_connection_stats_clear](#dsl_source_rtsp_connection_stats_clear)
* [dsl_source_rtsp_state_change_listener_add](#dsl_source_rtsp_state_change_listener_add)
* [dsl_source_rtsp_state_change_listener_remove](#dsl_source_rtsp_state_change_listener_remove)
* [dsl_source_rtsp_tap_add](#dsl_source_rtsp_tap_add)
* [dsl_source_rtsp_tap_remove](#dsl_source_rtsp_tap_remove)
* [dsl_source_file_path_get](#dsl_source_file_path_get)
* [dsl_source_file_path_set](#dsl_source_file_path_set)
* [dsl_source_file_repeat_enabled_get](#dsl_source_file_repeat_enabled_get)
* [dsl_source_file_repeat_enabled_set](#dsl_source_file_repeat_enabled_set)
* [dsl_source_image_multi_loop_enabled_get](#dsl_source_image_multi_loop_enabled_get)
* [dsl_source_image_multi_loop_enabled_set](#dsl_source_image_multi_loop_enabled_set)
* [dsl_source_image_multi_indices_get](#dsl_source_image_multi_indices_get)
* [dsl_source_image_multi_indices_set](#dsl_source_image_multi_indices_set)
* [dsl_source_image_stream_timeout_get](#dsl_source_image_stream_timeout_get)
* [dsl_source_image_stream_timeout_set](#dsl_source_image_stream_timeout_get)
* [dsl_source_interpipe_listen_to_get](#dsl_source_interpipe_listen_to_get)
* [dsl_source_interpipe_listen_to_set](#dsl_source_interpipe_listen_to_set)
* [dsl_source_interpipe_accept_settings_get](#dsl_source_interpipe_accept_settings_get)
* [dsl_source_interpipe_accept_settings_set](#dsl_source_interpipe_accept_settings_set)
* [dsl_source_do_timestamp_get](#dsl_source_do_timestamp_get)
* [dsl_source_do_timestamp_set](#dsl_source_do_timestamp_set)
* [dsl_source_dimensions_get](#dsl_source_dimensions_get)
* [dsl_source_framerate get](#dsl_source_framerate_get)
* [dsl_source_is_live](#dsl_source_is_live)
* [dsl_source_pause](#dsl_source_pause)
* [dsl_source_resume](#dsl_source_resume)
* [dsl_source_pph_add](#dsl_source_pph_add)
* [dsl_source_pph_remove](#dsl_source_pph_remove)

## Return Values
Streaming Source Methods use the following return codes, in addition to the general [Component API Return Values](/docs/api-component.md).
```C++
#define DSL_RESULT_SOURCE_RESULT                                    0x00020000
#define DSL_RESULT_SOURCE_NAME_NOT_UNIQUE                           0x00020001
#define DSL_RESULT_SOURCE_NAME_NOT_FOUND                            0x00020002
#define DSL_RESULT_SOURCE_NAME_BAD_FORMAT                           0x00020003
#define DSL_RESULT_SOURCE_NOT_FOUND                                 0x00020004
#define DSL_RESULT_SOURCE_THREW_EXCEPTION                           0x00020005
#define DSL_RESULT_SOURCE_FILE_NOT_FOUND                            0x00020006
#define DSL_RESULT_SOURCE_NOT_IN_USE                                0x00020007
#define DSL_RESULT_SOURCE_NOT_IN_PLAY                               0x00020008
#define DSL_RESULT_SOURCE_NOT_IN_PAUSE                              0x00020009
#define DSL_RESULT_SOURCE_FAILED_TO_CHANGE_STATE                    0x0002000A
#define DSL_RESULT_SOURCE_CODEC_PARSER_INVALID                      0x0002000B
#define DSL_RESULT_SOURCE_CODEC_PARSER_INVALID                      0x0002000B
#define DSL_RESULT_SOURCE_DEWARPER_ADD_FAILED                       0x0002000C
#define DSL_RESULT_SOURCE_DEWARPER_REMOVE_FAILED                    0x0002000D
#define DSL_RESULT_SOURCE_TAP_ADD_FAILED                            0x0002000E
#define DSL_RESULT_SOURCE_TAP_REMOVE_FAILED                         0x0002000F
#define DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE                   0x00020010
#define DSL_RESULT_SOURCE_COMPONENT_IS_NOT_DECODE_SOURCE            0x00020011
#define DSL_RESULT_SOURCE_COMPONENT_IS_NOT_FILE_SOURCE              0x00020012
#define DSL_RESULT_SOURCE_CALLBACK_ADD_FAILED                       0x00020013
#define DSL_RESULT_SOURCE_CALLBACK_REMOVE_FAILED                    0x00020014
#define DSL_RESULT_SOURCE_SET_FAILED                                0x00020015
#define DSL_RESULT_SOURCE_CSI_NOT_SUPPORTED                         0x00020016
```

## DSL State Values
```C
#define DSL_STATE_NULL                                              1
#define DSL_STATE_READY                                             2
#define DSL_STATE_PAUSED                                            3
#define DSL_STATE_PLAYING                                           4
```

## Gstreamer Stream format types
```C
#define DSL_STREAM_FORMAT_I420                                      0
#define DSL_STREAM_FORMAT_RGBA                                      1   
#define DSL_STREAM_FORMAT_NV12                                      2
```

## Gstreamer Buffer format types
```C
#define DSL_BUFFER_FORMAT_BYTE                                      2
#define DSL_BUFFER_FORMAT_TIME                                      3
```

## Cuda Decode Memory Types
```C
#define DSL_CUDADEC_MEMTYPE_DEVICE                                  0
#define DSL_CUDADEC_MEMTYPE_PINNED                                  1
#define DSL_CUDADEC_MEMTYPE_UNIFIED                                 2
```

## RTP Protocols
```C++
#define DSL_RTP_TCP                                                 0x04
#define DSL_RTP_ALL                                                 0x07
```

<br>

---

## Types
### dsl_rtsp_connection_data
This DSL Type defines a structure of Connection Stats and Parameters for a given RTSP Source. The data is returned to the client by calling [dsl_source_rtsp_connection_data_get](#dsl_source_rtsp_connection_data_get).

```C
typedef struct dsl_rtsp_connection_data
{
    boolean is_connected;
    time_t first_connected;
    time_t last_connected;
    time_t last_disconnected;
    uint count;
    boolean is_in_reconnect;
    uint retries;
    uint sleep;
    uint timeout;
}dsl_rtsp_connection_data;
```

**Fields**
* `is_connected` true if the RTSP Source is currently in a connected state, false otherwise
* `first_connected` - epoch time in seconds for the first successful connection, or when the stats were last cleared
* `last_connected`- epoch time in seconds for the last successful connection, or when the stats were last cleared
* `last_disconnected` - epoch time in seconds for the last disconnection, or when the stats were last cleared
* `count` - the number of successful connections from the start of Pipeline play, or from when the stats were last cleared
* `is_in_reconnect` - true if the RTSP Source is currently in a reconnection cycle, false otherwise.
* `retries` - number of re-connection retries for either the current cycle, if `is_in_reconnect` is true, or the last connection if `is_in_reconnect` is false`.
* `sleep` - current setting for the time to sleep between reconnection attempts after failure.
* `is_connect` - true if the RTSP Source is currently in a connected state, false otherwise.
* `timeout` - current setting for the maximum time to wait for an asynchronous state change to complete before resetting the source and then retrying again after the next sleep period.

**Python Example**
```Python
retval, data = dsl_source_rtsp_connection_data_get('rtsp-source')

print('Connection data for source:', 'rtsp-source')
print('  is connected:     ', data.is_connected)
print('  first connected:  ', time.ctime(data.first_connected))
print('  last connected:   ', time.ctime(data.last_connected))
print('  last disconnected:', time.ctime(data.last_disconnected))
print('  total count:      ', data.count)
print('  in is reconnect:  ', data.is_in_reconnect)
print('  retries:          ', data.retries)
print('  sleep time:       ', data.sleep,'seconds')
print('  timeout:          ', data.timeout, 'seconds')
```

<br>

## Client CallBack Typedefs
### *dsl_source_app_need_data_handler_cb*
```C++
typedef void (*dsl_source_app_need_data_handler_cb)(uint length, void* client_data);
```
Callback typedef for the App Source Component. The function is registered with the App Source by calling [dsl_source_app_data_handlers_add](#dsl_source_app_data_handlers_add). Once the Pipeline is playing, the function will be called when the Source needs new data to process.

**Parameters**
* `length` - [in] the amount of bytes needed.  The length is just a hint and when it is set to -1, any number of bytes can be pushed into the App Source.
* `client_data` - [in] opaque pointer to client's user data, passed into the pipeline on callback add.

<br>

### *dsl_source_app_enough_data_handler_cb*
```C++
typedef void (*dsl_source_app_enough_data_handler_cb)(void* client_data);
```
Callback typedef for the App Source Component. The function is registered with the App Source by calling [dsl_source_app_data_handlers_add](#dsl_source_app_data_handlers_add). Once the Pipeline is playing, the function will be called when the Source has enough data to process. It is recommended that the application stops calling [dsl_source_app_buffer_push](#dsl_source_app_buffer_push) until [dsl_source_app_need_data_handler_cb](#dsl_source_app_need_data_handler_cb) is called again.

**Parameters**
* `client_data` - [in] opaque pointer to client's user data, passed into the pipeline on callback add.

<br>

### *dsl_state_change_listener_cb*
```C++
typedef void (*dsl_state_change_listener_cb)(uint old_state, uint new_state, void* client_data);
```
Callback typedef for a client state-change listener. Functions of this type are added to an RTSP Source by calling [dsl_source_rtsp_state_change_listener_add](#dsl_source_rtsp_state_change_listener_add). Once added, the function will be called on every change of Pipeline state until the client removes the listener by calling [dsl_source_rtsp_state_change_listener_remove](#dsl_source_rtsp_state_change_listener_remove).

**Parameters**
* `old_state` - [in] one of [DSL State Values](#dsl-state-values) constants for the old (previous) pipeline state.
* `new_state` - [in] one of [DSL State Values](#dsl-state-values) constants for the new pipeline state.
* `client_data` - [in] opaque pointer to client's user data, passed into the pipeline on callback add.

<br>

## Constructors

### *dsl_source_app_new*
```C
DslReturnType dsl_source_app_new(const wchar_t* name, boolean is_live, 
    uint stream_format, uint width, uint height, uint fps_n, uint fps_d);
```
Creates a new, uniquely named App Source component to insert data -- buffers or samples -- into a DSL pipeline.

**Parameters**
* `source` - [in] unique name for the new Source
* `is_live` - [in] set to true to instruct the source to behave like a live source. This includes that it will only push out buffers in the PLAYING state.
* `stream_format` - [in]  one of the [DSL_STREAM_FORMAT](#gstreamer-stream-format-types) constants.
* `width` - [in] width of the source in pixels
* `height` - [in] height of the source in pixels
* `fps-n` - [in] frames per second fraction numerator
* `fps-d` - [in] frames per second fraction denominator

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_new('my-app-source', True,
    DSL_STREAM_FORMAT_I420, 1280, 720, 30, 1)
```

<br>

### *dsl_source_csi_new*
```C
DslReturnType dsl_source_csi_new(const wchar_t* source,
    uint width, uint height, uint fps_n, uint fps_d);
```
Creates a new, uniquely named CSI Camera Source component.

**Important:** A unique sensor-id is assigned to each CSI Source on creation, starting with 0. The default setting can be overridden by calling [dsl_source_decode_uri_set](#dsl_source_decode_uri_set). The call will fail if the given sensor-id is not unique. If a source is deleted, the sensor-id will be re-assigned to a new CSI Source if one is created.

**Parameters**
* `source` - [in] unique name for the new Source
* `width` - [in] width of the source in pixels
* `height` - [in] height of the source in pixels
* `fps-n` - [in] frames per second fraction numerator
* `fps-d` - [in] frames per second fraction denominator

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_csi_new('my-csi-source', 1280, 720, 30, 1)
```

<br>

### *dsl_source_usb_new*
```C
DslReturnType dsl_source_usb_new(const wchar_t* name,
    uint width, uint height, uint fps_n, uint fps_d);
```
Creates a new, uniquely named USB Camera Source component.

**Important:** A unique device-location is assigned to each USB Source on creation, starting with `/dev/video0`, followed by `/dev/video1`, and so on. The default assignment can be overridden by calling [dsl_source_usb_device_location_set](#dsl_source_usb_device_location_set). The call will fail if the given device-location is not unique. If a source is deleted, the device-location will be re-assigned to a new USB Source if one is created.

**Parameters**
* `source` - [in] unique name for the new Source
* `width` - [in] width of the source in pixels
* `height` - [in] height of the source in pixels
* `fps-n` - [in] frames per second fraction numerator
* `fps-d` - [in] frames per second fraction denominator

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_usb_new('my-csi-source', 1280, 720, 30, 1)
```
<br>

### *dsl_source_uri_new*
```C
DslReturnType dsl_source_uri_new(const wchar_t* name, const wchar_t* uri,
    boolean is_live, boolean intra_decode, uint drop_frame_interval);
```
This service creates a new, uniquely named URI Source component.

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `http://`, `https://`,  or `file://`
* `is_live` [in] `true` if the URI is a live source, `false` otherwise. File URI's will use a fixed value of `false`
* `intra_decode` - [in] set to true for M-JPEG codec format
* `drop_frame_interval` [in] number of frames to drop between each decoded frame. 0 = decode all frames

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_uri_new('my-uri-source', '../../test/streams/sample_1080p_h264.mp4',
    False, 0)
```

<br>

### *dsl_source_rtsp_new*
```C
DslReturnType dsl_source_rtsp_new(const wchar_t* name, const wchar_t* uri, uint protocol,
    uint intra_decode, uint drop_frame_interval, uint latency, uint timeout);
```

This service creates a new, uniquely named RTSP Source component

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `rtsp://`
* `protocol` - [in] one of the [RTP Protocols](#rtp-protocols) define above
* `drop_frame_interval` - [in] interval to drop frames at. 0 = decode all frames
* `latency` - [in] source latency setting in milliseconds
* `timeout` - [in] maximum time between successive frame buffers in units of seconds before initiating a "reconnection-cycle". Set to 0 to disable the timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_rtsp_new('dsl_source_uri_new',
    'rtsp://username:password@192.168.0.17:554/rtsp-camera-1', True, 200, 2)
```

<br>

### *dsl_source_file_new*
```C
DslReturnType dsl_source_file_new(const wchar_t* name,
    const wchar_t* file_path, boolean repeat_enabled);
```
This service creates a new, uniquely named File Source component. The Source implements a URI Source with
* `is_live = false`
* `cudadec_mem_type = DSL_CUDADEC_MEMTYPE_DEVICE`
* `intra_decode = false`
* `drop_frame_interval = 0`

**Parameters**
* `name` - [in] unique name for the new Source
* `file_path` - [in] absolute or relative path to the file to play
* `repeat_enabled` [in] set to `true` to repeat the file on end-of-stream (EOS), `false` otherwise

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_file_new('my-uri-source', './streams/sample_1080p_h264.mp4', false)
```

<br>

### *dsl_source_image_new*
```C
DslReturnType dsl_source_image_new(const wchar_t* name,
    const wchar_t* file_path);
```
This service creates a new, uniquely named Image Source component. The Image is streamed as a single frame followed by an End of Stream (EOS) event.

**Parameters**
* `name` - [in] unique name for the new Source
* `file_path` - [in] absolute or relative path to the image file to play
*
**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_image_new('my-image-source', './streams/image4.jpg')
```
<br>

### *dsl_source_image_multi_new*
```C
DslReturnType dsl_source_image_multi_new(const wchar_t* name,
    const wchar_t* file_path, uint fps_n, uint fps_d);
```
This service creates a new, uniquely named Multi Image Source component that decodes multiple images specified by a folder/filename-pattern using the printf style %d.

Example: `./my_images/image.%d04.mjpg`, where the files in "./my_images/" are named `image.0000.mjpg`, `image.0001.mjpg`, `image.0002.mjpg` etc.

The images are streamed one per frame at the specified framerate. A final EOS event occurs once all images have been played.

**Parameters**
* `name` - [in] unique name for the new Source.
* `file_path` - [in] absolute or relative path to the image files to play specified with the printf style %d.
* `fps-n` - [in] frames per second fraction numerator.
* `fps-d` - [in] frames per second fraction denominator.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_image_multi_new('my-multi-image-source', './my_images/image.%d04.mjpg')
```

<br>

### *dsl_source_image_stream_new*
```C
DslReturnType dsl_source_image_stream_new(const wchar_t* name,
    const wchar_t* file_path, boolean is_live, uint fps_n, uint fps_d, uint timeout);
```
This service creates a new, uniquely named Streaming Image Source component. The Image is overlaid on top of a mock video stream that plays at a specified frame rate. The video source can mock both live and non-live sources allowing the Image to be batched along with other Source components.

**Parameters**
* `name` - [in] unique name for the new Source
* `file_path` - [in] absolute or relative path to the image file to play
* `is_live` [in] true if the Source is to act as a live source, false otherwise.
* `fps-n` - [in] frames per second fraction numerator
* `fps-d` - [in] frames per second fraction denominator
* `timeout` [in] time to stream the image before generating an end-of-stream (EOS) event, in units of seconds. Set to 0 for no timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_image_stream_new('my-image-stream-source', './streams/image4.jpg',
    false, 30, 1, 0)
```

<br>

### *dsl_source_interpipe_new*
```C
DslReturnType dsl_source_interpipe_new(const wchar_t* name,
    const wchar_t* listen_to, boolean is_live,
    boolean accept_eos, boolean accept_events);
```
This service creates a new, uniquely named Interpipe Source component to listen to an Interpipe Sink Component. The Sink to `listen_to` can be updated dynamically while in a playing state.

Refer to the [Interpipe Services](/docs/overview.md#interpipe-services) overview for more information.

**Parameters**
* `name` - [in] unique name for the new Source
* `listen_to` - [in] unique name of the Interpipe Sink to listen to.
* `is_live` - [in] set to true to act as live source, false otherwise.
* `accept_eos` - [in] set to true to accept EOS events from the Interpipe Sink, false otherwise.
* `accept_events` - [in] set to true to accept events (except EOS event) from the Inter-Pipe Sink, false otherwise.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_interpipe_new('my-interpipe-source', 'my-interpipe-sink',
    false, true, true)
```

<br>

## Destructors
As with all Pipeline components, Sources are deleted by calling [dsl_component_delete](/docs/api-component.md#dsl_component_delete), [dsl_component_delete_many](/docs/api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](/docs/api-component.md#dsl_component_delete_all)

## Methods

### *dsl_source_app_data_handlers_add*
```C
DslReturnType dsl_source_app_data_handlers_add(const wchar_t* name, 
    dsl_source_app_need_data_handler_cb need_data_handler, 
    dsl_source_app_enough_data_handler_cb enough_data_handler, 
    void* client_data);
```
Adds data-handler callback functions to a named App Source component.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `need_data_handler` - [in] callback function to be called when new data is needed.
* `enough_data_handler` - [in] callback function to be called when the Source has enough data to process.
* `client_data` - [in]  opaque pointer to client data passed back into the client_handler functions.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_data_handlers_add('my-app-source',
    my_need_data_handler, my_enough_data_handler, NULL)
```
<br>

### *dsl_source_app_data_handlers_remove*
```C
DslReturnType dsl_source_app_data_handlers_remove(const wchar_t* name);
```
This service removes data-handler callback functions -- previously added with [dsl_source_app_data_handlers_add](#dsl_source_app_data_handlers_add) -- from a named App Source component.

**Parameters**
* `name` - [in] unique name of the Source to update.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_data_handlers_remove('my-app-source')
```

<br>

### *dsl_source_app_buffer_push*
```C
DslReturnType dsl_source_app_buffer_push(const wchar_t* name, void* buffer);
```
This service pushes a new buffer to a uniquely named App Source component for processing.

**Parameters**
* `name` - [in] unique name of the Source to push to.
* `buffer` - [in] buffer to push to the App Source.

**Returns**
* `DSL_RESULT_SUCCESS` on successful push. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_buffer_push('my-app-source', buffer)
```
<br>

### *dsl_source_app_sample_push*
```C
DslReturnType dsl_source_app_sample_push(const wchar_t* name, void* sample);
```
This service pushes a new sample to a uniquely named App Source component for processing.

**Parameters**
* `name` - [in] unique name of the Source to push to.
* `sample` - [in] sample to push to the App Source.

**Returns**
* `DSL_RESULT_SUCCESS` on successful push. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_sample_push('my-app-source', sample)
```

<br>

### *dsl_source_app_eos*
```C
DslReturnType dsl_source_app_eos(const wchar_t* name);
```
This service notifies a uniquely named App Source component that no more buffers are available.

**Parameters**
* `name` - [in] unique name of the Source to end-of-stream.

**Returns**
* `DSL_RESULT_SUCCESS` on successful EOS. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_eos('my-app-source')
```

<br>

### *dsl_source_app_buffer_format_get*
```C
DslReturnType dsl_source_app_buffer_format_get(const wchar_t* name, 
    uint* buffer_format);
```
This service gets the current buffer-format setting for the named App Source Component.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `buffer_format` - [out] one of the [DSL_BUFFER_FORMAT](#gstreamer-buffer-format-types) constants. Default = DSL_BUFFER_FORMAT_BYTE.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, format = dsl_source_app_buffer_format_get('my-app-source')
```
<br>

### *dsl_source_app_buffer_format_set*
```C
DslReturnType dsl_source_app_buffer_format_set(const wchar_t* name, 
    uint buffer_format);
```
This service sets the buffer-format setting for the named App Source Component.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `buffer_format` - [in] one of the [DSL_BUFFER_FORMAT](#gstreamer-buffer-format-types) constants.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_buffer_format_set('my-app-source', DSL_BUFFER_FORMAT_TIME)
```

<br>

### *dsl_source_app_block_enabled_get*
```C
DslReturnType dsl_source_app_block_enabled_get(const wchar_t* name, 
    boolean* enabled);
```
This service gets the block enabled setting for the named App Source Component. If true, when max-bytes are queued and after the enough-data signal has been emitted, the source will block any further push calls until the amount of queued bytes drops below the max-bytes limit.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `enabled` - [out] current block enabled setting. Default = FALSE.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, enabled = dsl_source_app_block_enabled_get('my-app-source')
```
<br>

### *dsl_source_app_block_enabled_set*
```C
DslReturnType dsl_source_app_block_enabled_set(const wchar_t* name, 
    boolean enabled);
```
This service sets the block enabled setting for the named App Source Component. If true, when max-bytes are queued and after the enough-data signal has been emitted, the source will block any further push calls until the amount of queued bytes drops below the max-bytes limit.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `enabled` - [in]  new block-enabled setting to use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_block_enabled_set('my-app-source', True)
```

<br>

### *dsl_source_app_current_level_bytes_get*
```C
DslReturnType dsl_source_app_current_level_bytes_get(const wchar_t* name,
    uint64_t* level);
```
This service gets the current level of queued data in bytes for the named App Source Component.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `level` - [out] current queue level in units of bytes.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, current_level = dsl_source_app_current_level_bytes_get('my-app-source')
```
<br>

### *dsl_source_app_max_level_bytes_get*
```C
DslReturnType dsl_source_app_max_level_bytes_get(const wchar_t* name,
    uint64_t* level);
```
This services gets the maximum amount of bytes that can be queued for the named App Source Component. After the maximum amount of bytes are queued, the App Source will call the [dsl_source_app_enough_data_handler_cb](#dsl_source_app_enough_data_handler_cb) callback function.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `level` - [out] current max-level in units of bytes. Default = 200000.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, max_level = dsl_source_app_max_level_bytes_get('my-app-source')
```
<br>

### *dsl_source_app_max_level_bytes_set*
```C
DslReturnType dsl_source_app_max_level_bytes_set(const wchar_t* name,
    uint64_t level);
```
This services sets the maximum amount of bytes that can be queued for the named App Source Component. After the maximum amount of bytes are queued, the App Source will call the [dsl_source_app_enough_data_handler_cb](#dsl_source_app_enough_data_handler_cb) callback function.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `level` - [in]  new max-level in units of bytes.  Default = 200000.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_app_max_level_bytes_set('my-app-source', 100000)
```

<br>

### *dsl_source_csi_sensor_id_get*
```C
DslReturnType dsl_source_csi_sensor_id_get(const wchar_t* name,
    uint* sensor_id);
```
This service gets the sensor-id setting for the named CSI Source. A unique sensor-id is assigned to each CSI Source on creation, starting with 0. The default setting can be overridden by calling [dsl_source_decode_uri_set](#dsl_source_decode_uri_set). The call will fail if the given sensor-id is not unique. If a source is deleted, the sensor-id will be re-assigned to a new CSI Source if one is created.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `sensor_id` - [out] unique sensor-id in use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, sensor_id = dsl_source_csi_sensor_id_get('my-csi-source')
```
<br>

### *dsl_source_csi_sensor_id_set*
```C
DslReturnType dsl_source_csi_sensor_id_set(const wchar_t* name,
    uint sensor_id);
```
This service sets the sensor-id setting for the named CSI Source to use. A unique sensor-id is assigned to each CSI Source on creation, starting with 0. This service will fail if the given sensor-id is not unique. If a source is deleted, the sensor-id will be re-assigned to a new CSI Source if one is created.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `sensor_id` - [in] unique sensor-id for the Source to use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_csi_sensor_id_set('my-csi-source', 1)
```

<br>

### *dsl_source_usb_device_location_get*

```C
DslReturnType dsl_source_usb_device_location_get(const wchar_t* name,
    const wchar_t** device_location);
```
This service gets the device-location setting for the named USB Source. A unique device-location is assigned to each USB Source on creation, starting with `/dev/video0`, followed by `/dev/video1`, and so on. The default assignment can be overridden by calling [dsl_source_usb_device_location_set](#dsl_source_usb_device_location_set). The call will fail if the given device-location is not unique. If a source is deleted, the device-location will be re-assigned to a new USB Source if one is created.


**Parameters**
* `name` - [in] unique name of the Source to query.
* `device_location` - [out] device location string in use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, device_location = dsl_source_usb_device_location_get('my-usb-source')
```
<br>

### *dsl_source_usb_device_location_set*
```C
DslReturnType dsl_source_usb_device_location_set(const wchar_t* name,
    const wchar_t* device_location);
```
This service sets the sensor-id setting for the named CSI Source to use.  A unique device-location is assigned to each USB Source on creation, starting with `/dev/video0`, followed by `/dev/video1`, and so on. This service will fail if the given device-location is not unique. If a source is deleted, the device-location will be re-assigned to a new USB Source if one is created.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `device_location` - [in] unique device-location for the Source to use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_usb_device_location_set('my-usb-source', '/dev/video1')
```

<br>

### *dsl_source_decode_uri_get*
```C
DslReturnType dsl_source_decode_uri_get(const wchar_t* name, const wchar_t** uri);
```
This service gets the current URI in use for the named URI or RTSP source.

**Parameters**
* `name` - [in] unique name of the Source to query.
* `uri` - [out] unique resource identifier in use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, uri = dsl_source_decode_uri_get('my-uri-source')
```
<br>

### *dsl_source_decode_uri_set*
```C
DslReturnType dsl_source_decode_uri_set(const wchar_t* name, const wchar_t* uri);
```
This service sets the URI to use by the named URI or RTSP source.

**Parameters**
* `name` - [in] unique name of the Source to update.
* `uri` - [in] unique resource identifier for the Source to use.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_decode_uri_set('my-uri-source', '../../test/streams/sample_1080p_h264.mp4')
```

<br>

### *dsl_source_decode_drop_farme_interval_get*
```C
DslReturnType dsl_source_csi_sensor_id_set(const wchar_t* name,
    uint sensor_id);
```
This service gets the current drop frame interval in use by the named URI or RTSP source

**Parameters**
* `name` - [in] unique name of the Source to update
* `interval` - [out] current drop frame interval currently in use

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, interval = dsl_source_decode_drop_frame_interval_get('my-uri-source')
```
<br>

### *dsl_source_decode_drop_farme_interval_set*
```C
DslReturnType dsl_source_decode_drop_farme_interval_set(const wchar_t* name, uint interval);
```
This service sets the drop frame interval to use by the named URI or RTSP source.

**Parameters**
* `name` - [in] unique name of the Source to update
* `interval` - [in] new drop frame interval to use

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_decode_drop_farme_interval_set('my-uri-source', 2)
```

<br>

### *dsl_source_decode_dewarper_add*
```C
DslReturnType dsl_source_decode_dewarper_add(const wchar_t* name, const wchar_t* dewarper);
```
This service adds a previously constructed [Dewarper](api-dewarper.md) component to either a named URI or RTSP source. A source can have at most one Dewarper, and calls to add more will fail. Attempts to add a Dewarper to a Source in a state of `PLAYING` or `PAUSED` will fail.

**Parameters**
* `name` - [in] unique name of the Source to update
* `dewarper` - [in] unique name of the Dewarper to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_decode_dewarper_add('my-uri-source', 'my-dewarper')
```

<br>

### *dsl_source_decode_dewarper_remove*
```C
DslReturnType dsl_source_decode_dewarper_remove(const wchar_t* name);
```
This service remove a [Dewarper](api-dewarper.md) component, previously added with [dsl_source_decode_dewarper_add](#dsl_source_decode_dewarper_add) to a named URI source. Calls to remove will fail if the Source is in a state of `PLAYING` or `PAUSED` will fail.

**Parameters**
* `name` - [in] unique name of the Source to update

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_uri_dewarper_remove('my-uri-source')
```

<br>

### *dsl_source_rtsp_timeout_get*
```C
DslReturnType dsl_source_rtsp_timeout_get(const wchar_t* name, uint* timeout);
```
This service gets the current frame buffer timeout value for the named RTSP Source

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `timeout` - [out] time to wait (in seconds) between successive frames before determining the connection is lost. If set to 0 then timeout is disabled.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, timeout = dsl_source_rtsp_timeout_get('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_timeout_set*
```C
DslReturnType dsl_source_rtsp_timeout_set(const wchar_t* name, uint timeout);
```
This service sets the frame buffer timeout value for the named RTSP Source. Setting the `timeout` to 0 will disable stream management and terminate any reconnection cycle if in progress.

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `timeout` - [in] time to wait (in seconds) between successive frames before determining the connection is lost. Set to 0 to disable timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_timeout_set('my-rtsp-source', timeout)
```
<br>

### *dsl_source_rtsp_reconnection_params_get*
```C
DslReturnType dsl_source_rtsp_reconnection_params_get(const wchar_t* name, uint* sleep_ms, uint* timeout_ms);
```
This service gets the current reconnection params in use by the named RTSP Source. The parameters are set to DSL_RTSP_RECONNECT_SLEEP_TIME_MS and DSL_RTSP_RECONNECT_TIMEOUT_MS on Source creation.

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `sleep_ms` - [out] time to sleep between successively checking the status of the asynchronous reconnection
 * `timeout_ms` - [out] time to wait before terminating the current reconnection try and restarting the reconnection cycle again.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, sleep_ms, timeout_ms = dsl_source_rtsp_reconnection_params_get('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_reconnection_params_set*
```C
DslReturnType dsl_source_rtsp_reconnection_params_get(const wchar_t* name, uint* sleep_ms, uint* timeout_ms);
```
This service sets the reconnection params for the named RTSP Source. The parameters are set to DSL_RTSP_RECONNECT_SLEEP_TIME_MS and DSL_RTSP_RECONNECT_TIMEOUT_MS on Source creation.

**Note:** Both `sleep_ms` and `time_out` must be greater than 10 ms. `time_out` must be >= `sleep_ms` and should be set as a multiple of. Calling this service during an active "reconnection-cycle" will terminate the current attempt with a new cycle started using the new parameters. The current number of retries will not be reset.

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `sleep_ms` - [out] time to sleep between successively checking the status of the asynchronous reconnection
 * `timeout_ms` - [out] time to wait before terminating the current reconnection try and restarting the reconnection cycle again.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_reconnection_params_get('my-rtsp-source', sleep_ms, timeout_ms)
```
<br>

### *dsl_source_rtsp_connection_data_get*
```C
DslReturnType dsl_source_rtsp_connection_data_get(const wchar_t* name, dsl_rtsp_connection_data* data);
```
This service gets the current connection stats for the named RTSP Source.

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `data` [out] - pointer to a [dsl_rtsp_connection_data](#dsl_rtsp_connection_data) structure.
 
**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, connection_data = dsl_source_rtsp_connection_data_get('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_connection_stats_clear*
```C
DslReturnType dsl_source_rtsp_connection_stats_clear(const wchar_t* name);
```
This service clears the current reconnection stats for the named RTSP Source.

**Note:** the connection `retries` count will not be cleared if `in_reconnect == true`

**Parameters**
 * `name` - [in] unique name of the Source to update
 
**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_connection_stats_clear('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_state_change_listener_add*
```C
DslReturnType dsl_source_rtsp_state_change_listener_add(const wchar_t* pipeline,
    state_change_listener_cb listener, void* user_data);
```
This service adds a callback function of type [dsl_state_change_listener_cb](#dsl_state_change_listener_cb) to a
RTSP Source identified by its unique name. The function will be called on every change-of-state with `old_state`, `new_state`, and the client provided `user_data`. Multiple callback functions can be registered with one Source, and one callback function can be registered with multiple Sources.

**Parameters**
* `name` - [in] unique name of the RTSP Source to update.
* `listener` - [in] state change listener callback function to add.
* `user_data` - [in] opaque pointer to user data returned to the client when listener is called back

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
def state_change_listener(old_state, new_state, user_data, user_data):
    print('old_state = ', old_state)
    print('new_state = ', new_state)
   
retval = dsl_source_rtsp_state_change_listener_add('my-rtsp-source', state_change_listener, None)
```

<br>

### *dsl_source_rtsp_state_change_listener_remove*
```C
DslReturnType dsl_source_rtsp_state_change_listener_remove(const wchar_t* name,
    dsl_state_change_listener_cb listener);
```
This service removes a callback function of type [state_change_listener_cb](#state_change_listener_cb) from a
pipeline identified by its unique name.

**Parameters**
* `pipeline` - [in] unique name of the Pipeline to update.
* `listener` - [in] state change listener callback function to remove.

**Returns**  
* `DSL_RESULT_SUCCESS` on successful removal. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_state_change_listener_remove('my-pipeline', state_change_listener)
```

<br>

### *dsl_source_rtsp_tap_add*
```C
DslReturnType dsl_source_rtsp_tap_add(const wchar_t* name, const wchar_t* tap);
```
This service adds a named Tap to a named RTSP source. There is currently only one type of Tap which is the [Smart Recording Tap](/docs/api-tap.md#dsl_tap_record_new)

**Parameters**
 * `name` [in] unique name of the Source object to update
 * `tap` [in] unique name of the Tap to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_tap_add('my-rtsp-source', 'my-record-tap')
```

<br>

### *dsl_source_rtsp_tap_remove*
```C
DslReturnType dsl_source_rtsp_tap_remove(const wchar_t* name);
```

Removes a Tap component from an RTSP Source component. The call will fail if the RTSP source is without a Tap component.  

**Parameters**
 * `name` [in] name of the Source object to update

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_tap_remove('my-rtsp-source')
```

<br>

### *dsl_source_file_path_get*
```C
DslReturnType dsl_source_file_path_get(const wchar_t* name, const wchar_t** file_path);
```
This service gets the current file path in use for the named File, Image, or Streaming Image source

**Parameters**
* `name` - [in] unique name of the Source to query
* `file_path` - [out] absolute file path setting in use by the File source

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, file_path = dsl_source_file_path_get('my-file-source')
```
<br>

### *dsl_source_file_path_set*
```C
DslReturnType dsl_source_file_path_set(const wchar_t* name, const wchar_t* file_path);
```
This service sets the file path to use by the named File, Image, or Streaming Image source

**Parameters**
* `name` - [in] unique name of the Source to update
* `file_path` - [in] absolute or relative File Path to use

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_file_path_set('my-file-source', './streams/sample_1080p_h264.mp4')
```

<br>

### *dsl_source_file_repeat_enabled_get*
```C
DslReturnType dsl_source_file_repeat_enabled_get(const wchar_t* name, boolean* enabled);
```
This service gets the current repeat-enabled setting in use for the named File source

**Parameters**
* `name` - [in] unique name of the Source to query
* `repeat_enabled` - [out] if true, the File source will repeat the file on end-of-stream (EOS).

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, repeat_enabled = dsl_source_file_repeat_enabled_get('my-file-source')
```
<br>

### *dsl_source_file_repeat_enabled_set*
```C
DslReturnType dsl_source_file_repeat_enabled_set(const wchar_t* name, boolean enabled);
```
This service sets the repeat-enabled setting to use by the named File source.

**Parameters**
* `name` - [in] unique name of the Source to update
* `repeat_enabled` - [in] if true, the File source will repeat the file on an end-of-stream (EOS).

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_file_repeat_enabled_set('my-file-source', True)
```

<br>

### *dsl_source_image_multi_loop_enabled_get*
```C
DslReturnType dsl_source_image_multi_loop_enabled_get(const wchar_t* name,
    boolean* enabled);
```
This service gets the current loop-enabled setting for the named Multi-Image source.

**Parameters**
* `name` - [in] unique name of the Source to query
* `enabled` - [out] if true, the Multi-Image source will loop to the `start_index` (default=0) when the last image is played. The Source will stop on the last image if false (default).

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, loop_enabled = dsl_source_image_multi_loop_enabled_get('my-multi-image-source')
```
<br>

### *dsl_source_image_multi_loop_enabled_set*
```C
DslReturnType dsl_source_image_multi_loop_enabled_set(const wchar_t* name,
    boolean enabled);
```
This service sets the loop-enabled setting for the named Multi-Image Source to use.

**Parameters**
* `name` - [in] unique name of the Source to update
* `enabled` - [in] if true, the Multi-Image source will loop to the `start_index` (default=0) when the last image is played. The Source will stop on the last image if false (default).

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_image_multi_loop_enabled_set('my-multi-image-source', True)
```

<br>

### *dsl_source_image_multi_indices_get*
```C
DslReturnType dsl_source_image_multi_indices_get(const wchar_t* name,
    int* start_index, int* stop_index);
```
This service gets the current start and stop index settings for the named Multi-Image source.

**Parameters**
* `name` - [in] unique name of the Source to query
* `start_index` - [out] index to start with. When the end of the loop is reached, the current index will be set to the start-index. Default = 0.
* `stop_index` - [out] index to stop on, Default = -1 (no stop).

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, start_index, stop_index = dsl_source_image_multi_indices_get('my-multi-image-source')
```
<br>

### *dsl_source_image_multi_indices_set*
```C
DslReturnType dsl_source_image_multi_indices_set(const wchar_t* name,
    int start_index, int stop_index);
```
This service sets the start and stop index settings for the named Multi-Image Source to use.

**Parameters**
* `name` - [in] unique name of the Source to update
* `start_index` - [in] index to start with. When the end of the loop is reached, the current index will be set to the start-index. Default = 0.
* `stop_index` - [in] index to stop on, Default = -1 (no stop).

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_image_multi_indices_set('my-multi-image-source', 10, -1)
```

<br>

### *dsl_source_image_stream_timeout_get*
```C
DslReturnType dsl_source_image_stream_timeout_get(const wchar_t* name, uint* timeout);
```
This service gets the current timeout setting in use for the named Streaming Image source

**Parameters**
* `name` - [in] unique name of the Image Source to query
* `timeout` - [out] current timeout setting in units of seconds. 0 = no timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, timeout = dsl_source_image_stream_timeout_get('my-image-source')
```
<br>

### *dsl_source_image_multi_loop_enabled_set*
```C
DslReturnType dsl_source_image_multi_loop_enabled_set(const wchar_t* name,
    boolean enabled);
```
This service sets the loop-enabled setting for the named Multi-Image Source to use.

**Parameters**
* `name` - [in] unique name of the Source to update
* `enabled` - [in] if true, the Multi-Image source will loop to the `start_index` (default=0) when the last image is played. The Source will stop on the last image if false (default).

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_image_multi_loop_enabled_set('my-multi-image-source', True)
```

<br>
### *dsl_source_image_stream_timeout_get*
```C
DslReturnType dsl_source_image_stream_timeout_get(const wchar_t* name, uint* timeout);
```
This service gets the current timeout setting in use for the named Streaming Image source

**Parameters**
* `name` - [in] unique name of the Image Source to query
* `timeout` - [out] current timeout setting in units of seconds. 0 = no timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, timeout = dsl_source_image_stream_timeout_get('my-image-source')
```
<br>

### *dsl_source_image_stream_timeout_set*
```C
DslReturnType dsl_source_image_stream_timeout_set(const wchar_t* name, uint timeout);
```
This service sets the File Path to use by the named Streaming Image source.

**Parameters**
* `name` - [in] unique name of the Image Source to update
* `timeout` - [out] new timeout setting in units of seconds. 0 = no timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_image_stream_timeout_set('my-image-source', 30)
```

<br>

### *dsl_source_interpipe_listen_to_get*
```C
DslReturnType dsl_source_interpipe_listen_to_get(const wchar_t* name,
    const wchar_t** listen_to);
```
This service gets the name of the Interpipe Sink the named Interpipe Source component is currently listening to.

**Parameters**
* `name` - [in] unique name of the Interpipe Source to query
* `listen_to` - [out]  unique name of the Interpipe Sink the Source is listening to.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, listen_to = dsl_source_interpipe_listen_to_get('my-interpipe-source')
```
<br>

### *dsl_source_interpipe_listen_to_set*
```C
DslReturnType dsl_source_interpipe_listen_to_get(const wchar_t* name,
    const wchar_t* listen_to);
```
This service sets the name of the Interpipe Sink to listen to for the name Interpipe Source.

**Parameters**
* `name` - [in] unique name of the Interpipe Source to update.
* `listen_to` - [out]  unique name of the Interpipe Sink listening to.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_interpipe_listen_to_set('my-interpipe-source', 'my-interpipe-sink-2')
```
<br>

### *dsl_source_interpipe_accept_settings_get*
```C
DslReturnType dsl_source_interpipe_accept_settings_get(const wchar_t* name,
    boolean* accept_eos, boolean* accept_events);
```
This service gets the current accept settings in use by the named Interpipe Source.

**Parameters**
* `name` - [in] unique name of the Interpipe Source to query
* `accept_eos` - [out] if true, the Source accepts EOS events from the Interpipe Sink.
* `accept_event` - [out] if true, the Source accepts events (except EOS event) from the Interpipe Sink.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, accept_eos, accept_events = dsl_source_interpipe_accept_settings_get(
    'my-interpipe-source')
```
<br>

### *dsl_source_interpipe_accept_settings_set*
```C
DslReturnType dsl_source_interpipe_accept_settings_get(const wchar_t* name,
    boolean accept_eos, boolean accept_events);
```
This service sets the accept settings for the named Interpipe Source to use.

**Parameters**
* `name` - [in] unique name of the Interpipe Source to update
* `accept_eos` - [in] set to true to accept EOS events from the Inter-Pipe Sink, false otherwise.
* `accept_event` - [in] set to true to accept events (except EOS event) from the Inter-Pipe Sink, false otherwise.

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_interpipe_accept_settings_get('my-interpipe-source',
    True, True)
```
<br>

### *dsl_source_dimensions_get*
```C
DslReturnType dsl_source_dimensions_get(const wchar_t* name, uint* width, uint* height);
```
This service returns the width and height values of a named source. CSI and USB Camera sources will return the values they were created with. URI and RTSP sources will return 0's while `not-in` and will be updated once the Source has transitioned to a state of `playing`.

**Parameters**
* `source` - [in] unique name of the Source to play
* `width` - [out] width of the Source in pixels.
* `height` - [out] height of the Source in pixels.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, width, height = dsl_source_dimensions_get('my-uri-source')
```

<br>

### *dsl_source_do_timestamp_get*
```C
DslReturnType dsl_source_do_timestamp_get(const wchar_t* name, 
    boolean* do_timestamp);
```
This service gets the do-timestamp setting for the named Source Component.

**Parameters**
* `source` - [in] unique name of the Source to play.
* `do_timestamp` - [out]  if TRUE, the source will automatically timestamp outgoing buffers based on the current running_time.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, do_timestamep = dsl_source_do_timestamp_set('my-app-source')
```

<br>

### *dsl_source_do_timestamp_set*
```C
DslReturnType dsl_source_do_timestamp_set(const wchar_t* name, 
    boolean do_timestamp);
```
This service sets the do-timestamp setting for the named Source Component.

**Parameters**
* `source` - [in] unique name of the Source to play.
* `do_timestamp` - [in]  set to TRUE to have the source automatically timestamp outgoing buffers based on the current running_time.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_do_timestamp_set('my-app-source', True)
```

<br>

### *dsl_source_framerate_get*
```C
DslReturnType dsl_source_frame_rate_get(const wchar_t* name, uint* fps_n, uint* fps_n);
```
This service returns the fractional frames per second as numerator and denominator for a named source. CSI and USB Camera sources will return the values they were created with. URI and RTSP sources will return 0's while `not-in` and will be updated once the Source has transitioned to a state of `playing`.

**Parameters**
* `source` - [in] unique name of the Source to play.
* `fps_n` - [out] width of the Source in pixels.
* `fps_d` - [out] height of the Source in pixels.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, fps_n, fps_d = dsl_source_dimensions_get('my-uri-source')
```

<br>

### *dsl_source_is_live*
```C
DslReturnType dsl_source_is_live(const wchar_t* name, boolean* is_live);
```
Returns `true` if the Source component's stream is live. CSI and USB Camera sources will always return `True`.

**Parameters**
* `name` - [in] unique name of the Source to query
* `is_live` - [out] `true` if the source is live, false otherwise

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, is_live = dsl_source_is_live('my-uri-source')
```

<br>

### *dsl_source_pause*
```C
DslReturnType dsl_source_pause(const wchar_t* name);
```
Sets the state of the Source component to Paused. This method tries to change the state of an `in-use` Source component to `GST_STATE_PAUSED`. The current state of the Source component can be obtained by calling [dsl_source_state_is](#dsl_source_state_is).

**Parameters**
* `name` - unique name of the Source to pause

**Returns**
* `DSL_RESULT_SUCCESS` on successful transition. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_play('my-source')
```

<br>

### *dsl_source_resume*
```C
DslReturnType dsl_source_resume(const wchar_t* name);
```
Sets the state of a `paused` Source component to `playing`. This method tries to change the state of an `in-use` Source component to `DSL_STATE_PLAYING`. The current state of the Source component can be obtained by calling [dsl_source_state_is](#dsl_source_state_is). The Pipeline, when transitioning to a state of `DSL_STATE_PLAYING`, will set each of its Sources'
state to `DSL_STATE_PLAYING`. An individual Source, once playing, can be paused by calling [dsl_source_pause](#dsl_source_pause).

<br>

**Parameters**
* `name` - unique name of the Source to play

**Returns**
* `DSL_RESULT_SUCCESS` on successful transition. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_resume('my-source')
```

<br>

### *dsl_source_pph_add*
```C++
DslReturnType dsl_source_pph_add(const wchar_t* name, const wchar_t* handler);
```

This service adds a [Pad Probe Handler](/docs/api-pph.md) -- typically a [New Buffer Timeout PPH](/docs/api-pph.md#dsl_pph_buffer_timeout_new) --- to the Source pad (only) of the named Source Component. 

**Important Note** Adding an [Object Detection Event PPH](/docs/api-pph.md#dsl_pph_ode_new) or an [Non-Maximum Processor PPH](/docs/api-pph.md#dsl_pph_nmp_new) will result in a NOP as there is no batch-metadata attached to the buffers for these PPHs to process. The initial frame level batch-metadata is added to the buffers by the Pipelines's Stream-muxer downstream of the Source. 

**Parameters**
* `name` - [in] unique name of the Source Component to update.
* `handler` - [in] unique name of Pad Probe Handler to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**

```Python
retval = dsl_source_pph_add('my-csi-source-1', 'my-buffer-timeout-pph-1')
```

<br>

### *dsl_source_pph_remove*
```C++
DslReturnType dsl_source_pph_remove(const wchar_t* name, const wchar_t* handler);
```
This service removes a [Pad Probe Handler](/docs/api-pph.md) from the Source pad of the named Source Component. The service will fail if the named handler is not owned by the named source.

**Parameters**
* `name` - [in] unique name of the Source Component to update.
* `handler` - [in] unique name of Pad Probe Handler to remove

**Returns**
* `DSL_RESULT_SUCCESS` on successful remove. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_pph_remove('my-csi-source-1', 'my-buffer-timeout-pph-1')
```


---

## API Reference
* [List of all Services](/docs/api-reference-list.md)
* [Pipeline](/docs/api-pipeline.md)
* [Player](/docs/api-player.md)
* **Source**
* [Tap](/docs/api-tap.md)
* [Dewarper](/docs/api-dewarper.md)
* [Preprocessor](/docs/api-preproc.md)
* [Inference Engine and Server](/docs/api-infer.md)
* [Tracker](/docs/api-tracker.md)
* [Segmentation Visualizer](/docs/api-segvisual.md)
* [Tiler](/docs/api-tiler.md)
* [Demuxer and Splitter](/docs/api-tee.md)
* [On-Screen Display](/docs/api-osd.md)
* [Sink](/docs/api-sink.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* [ODE Accumulator](/docs/api-ode-accumulator.md)
* [ODE Acton](/docs/api-ode-action.md)
* [ODE Area](/docs/api-ode-area.md)
* [ODE Heat-Mapper](/docs/api-ode-heat-mapper.md)
* [Display Type](/docs/api-display-type.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
* [Mailer](/docs/api-mailer.md)
* [WebSocket Server](/docs/api-ws-server.md)
* [Message Broker](/docs/api-msg-broker.md)
* [Info API](/docs/api-info.md)
