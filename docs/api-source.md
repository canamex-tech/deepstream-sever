# Source API Reference
Sources are the head components for all DSL Pipelines. Pipelines must have at least one source in use, among other components, to reach a state of Ready. DSL supports four types of Streaming Sources, two Camera and two Decode:

**Camera Sources:**
* Camera Serial Interface ( CSI )
* Universal Serial Bus ( USB )

**Decode Sources:**
* Uniform Resource Identifier ( URI )
* Real-time Streaming Protocol ( RTSP )

#### Source Construction and Destruction
Sources are created using one of four type-specific constructors. As with all components, Streaming Sources must be uniquely named from all other Pipeline components created. 

Sources are added to a Pipeline by calling [dsl_pipeline_component_add](api-pipeline.md#dsl_pipeline_component_add) or [dsl_pipeline_component_add_many](api-pipeline.md]#dsl_pipeline_component_add_many) and removed with [dsl_pipeline_component_remove](api-pipeline.md#dsl_pipeline_component_remove), [dsl_pipeline_component_remove_many](api-pipeline.md#dsl_pipeline_component_remove_many), or [dsl_pipeline_component_remove_all]((api-pipeline.md)#dsl_pipeline_component_remove_all).

When adding multiple sources to a Pipeline, all must have the same `is_live` setting; `true` or `false`. The add services will fail on first exception. 

The relationship between Pipelines and Sources is one-to-many. Once added to a Pipeline, a Source must be removed before it can used with another. All sources are deleted by calling [dsl_component_delete](api-component.md#dsl_component_delete), [dsl_component_delete_many](api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](api-component.md#dsl_component_delete_all). Calling a delete service on a Source `in-use` by a Pipeline will fail.

#### Sources and Demuxers
When using a [Demuxer](/docs/api-tiler.md), vs. a Tiler component, each demuxed source stream must have one or more downstream [Sink](/docs/api-sink) components to end the stream. To identify this relationship, each sink is added to its upstream Source component vs. the Pipeline directly. See [dsl_source_sink_add](#dsl_source_sink_add) and [dsl_source_sink_remove](#dsl_source_sink_remove). An optional [On-Screen Display (OSD)](/docs/api-osd.md) component can be add to each source when using a Demuxer as well. See [dsl_source_osd_add](#dsl_source_osd_add) and [dsl_source_osd_remove](#dsl_source_osd_remove).

#### Maximum Source Control
There is no practical limit to the number of Sources that can be created, just to the number of Sources that can be `in use` - a child of a Pipeline - at one time. The `in-use` limit is imposed by the Jetson Model in use. 

The maximum number of `in-use` Sources is set to `DSL_DEFAULT_SOURCE_IN_USE_MAX` on DSL initialization. The value can be read by calling [dsl_source_num_in_use_max_get](#dsl_source_num_in_use_max_get) and updated with [dsl_source_num_in_use_max_set](#dsl_source_num_in_use_max_set). The number of Sources in use by all Pipelines can obtained by calling [dsl_source_get_num_in_use](#dsl_source_get_num_in_use). 


## Source API
**Typedefs**
* [dsl_rtsp_connection_data](#dsl_rtsp_connection_data)

**Client CallBack Typedefs**
* [dsl_state_change_listener_cb](#dsl_state_change_listener_cb)

**Constructors:**
* [dsl_source_csi_new](#dsl_source_csi_new)
* [dsl_source_usb_new](#dsl_source_usb_new)
* [dsl_source_uri_new](#dsl_source_uri_new)
* [dsl_source_rtsp_new](#dsl_source_rtsp_new)

**methods:**
* [dsl_source_dimensions_get](#dsl_source_dimensions_get)
* [dsl_source_framerate get](#dsl_source_framerate_get)
* [dsl_source_is_live](#dsl_source_is_live)
* [dsl_source_pause](#dsl_source_pause)
* [dsl_source_resume](#dsl_source_resume)
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
* [dsl_source_rtsp_reconnection_stats_get](#dsl_source_rtsp_reconnection_stats_get)
* [dsl_source_rtsp_reconnection_stats_clear](#dsl_source_rtsp_reconnection_stats_clear)
* [dsl_source_rtsp_state_change_listener_add](#dsl_source_rtsp_state_change_listener_add)
* [dsl_source_rtsp_state_change_listener_remove](#dsl_source_rtsp_state_change_listener_remove)
* [dsl_source_rtsp_tap_add](#dsl_source_rtsp_tap_add)
* [dsl_source_rtsp_tap_remove](#dsl_source_rtsp_tap_remove)
* [dsl_source_num_in_use_get](#dsl_source_num_in_use_get)
* [dsl_source_num_in_use_max_get](#dsl_source_num_in_use_max_get)
* [dsl_source_num_in_use_max_set](#dsl_source_num_in_use_max_set)

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
#define DSL_RESULT_SOURCE_DEWARPER_ADD_FAILED                       0x0002000C
#define DSL_RESULT_SOURCE_DEWARPER_REMOVE_FAILED                    0x0002000D
#define DSL_RESULT_SOURCE_TAP_ADD_FAILED                            0x0002000E
#define DSL_RESULT_SOURCE_TAP_REMOVE_FAILED                         0x0002000F
#define DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE                   0x00020010
#define DSL_RESULT_SOURCE_CALLBACK_ADD_FAILED                       0x00020011
#define DSL_RESULT_SOURCE_CALLBACK_REMOVE_FAILED                    0x00020012
#define DSL_RESULT_SOURCE_SET_FAILED                                0x00020013
```

## DSL State Values
```C
#define DSL_STATE_NULL                                              1
#define DSL_STATE_READY                                             2
#define DSL_STATE_PAUSED                                            3
#define DSL_STATE_PLAYING                                           4
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
* `first_connected` - epoc time in seconds for the first succesful connection, or when the stats were last cleared
* `last_connected`- epoc time in seconds for the last succesful connection, or when the stats were last cleared
* `last_disconnected` - epoc time in seconds for the last disconnection, or when the stats were last cleared
* `count` - the number of succesful connections from the start of Pipeline play, or from when the stats were last cleared
* `is_in_reconnect` - true if the RTSP Source is currently in a re-connection cycle, false otherwise.
* `retries` - number of re-connection retries for either the current cycle, if `is_in_reconnect` is true, or the last connection if `is_in_reconnect` is false`.
* `sleep` - current setting for the time to sleep between re-connection attempts after failure.
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
### *dsl_state_change_listener_cb*
```C++
typedef void (*dsl_state_change_listener_cb)(uint old_state, uint new_state, void* client_data);
```
Callback typedef for a client state-change listener. Functions of this type are added to an RTSP Source by calling [dsl_source_rtsp_state_change_listener_add](#dsl_source_rtsp_state_change_listener_add). Once added, the function will be called on every change of Pipeline state until the client removes the listener by calling [dsl_source_rtsp_state_change_listener_remove](#dsl_source_rtsp_state_change_listener_remove).

**Parameters**
* `old_state` - [in] one of [DSL State Values](#dsl-state-values) constants for the old (previous) pipeline state
* `new_state` - [in] one of [DSL State Values](#dsl-state-values) constants for the new pipeline state
* `client_data` - [in] opaque pointer to client's user data, passed into the pipeline on callback add

<br>

## Constructors

### *dsl_source_csi_new*
```C
DslReturnType dsl_source_csi_new(const wchar_t* source,
    uint width, uint height, uint fps_n, uint fps_d);
```
Creates a new, uniquely named CSI Camera Source object. 

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
DslReturnType dsl_source_usb_new(const wchar_t* source,
    uint width, uint height, uint fps_n, uint fps_d);
```
Creates a new, uniquely named USB Camera Source object. 

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

### *dsl_source_uri_new*
```C
DslReturnType dsl_source_uri_new(const wchar_t* name, const wchar_t* uri, boolean is_live,
    uint cudadec_mem_type, boolean intra_decode, uint drop_frame_interval);
```
This service creates a new, uniquely named URI Source component

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `http://`, `https://`,  or `file://` 
* `is_live` [in] `true` if the URI is a live source, `false` otherwise. File URI's will used a fixed value of `false`
* `cudadec_mem_type` - [in] one of the [Cuda Decode Memory Types](#cuda-decode-memory-types) defined below
* `intra_decode` - [in] set to true for M-JPEG codec format
* `drop_frame_interval` [in] interval to drop frames at. 0 = decode all frames

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_uri_new('my-uri-source', '../../test/streams/sample_1080p_h264.mp4',
    False, DSL_CUDADEC_MEMTYPE_DEVICE, 0)
```

<br>

### *dsl_source_rtsp_new*
```C
DslReturnType dsl_source_rtsp_new(const wchar_t* name, const wchar_t* uri, uint protocol,
    uint cudadec_mem_type, uint intra_decode, uint drop_frame_interval, uint latency, uint timeout);
```

This service creates a new, uniquely named RTSP Source component

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `rtsp://`
* `protocol` - [in] one of the [RTP Protocols](#rtp-protocols) define above
* `cudadec_mem_type` - [in] one of the [Cuda Decode Memory Types](#cuda-decode-memory-types) defined above
* `drop_frame_interval` - [in] interval to drop frames at. 0 = decode all frames
* `latency` - [in] source latency setting in milliseconds
* `timeout` - [in] maximum time between successive frame buffers in units of seconds before initiating a "reconnection-cycle". Set to 0 to disable the timeout.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_rtsp_new('dsl_source_uri_new', 'rtsp://username:password@192.168.0.17:554/rtsp-camera-1',
    True, DSL_CUDADEC_MEMTYPE_DEVICE, 200, 2)
```

<br>


## Destructors
As with all Pipeline components, Sources are deleted by calling [dsl_component_delete](api-component.md#dsl_component_delete), [dsl_component_delete_many](api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](api-component.md#dsl_component_delete_all)

## Methods

### *dsl_source_dimensions_get*
```C
DslReturnType dsl_source_dimensions_get(const wchar_t* name, uint* width, uint* height);
```
This service returns the width and height values of a named source. CSI and USB Camera sources will return the values they were created with. URI and RTSP sources will return 0's while `not-in` and will be updated once the Source has transitioned to a state of `playing`.

**Parameters**
* `source` - [in]unique name of the Source to play
* `width` - [out] width of the Source in pixels.
* `height` - [out] height of the Source in pixels.

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, width, height = dsl_source_dimensions_get('my-uri-source')
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
DslReturnType dsl_source_is_live(const wchar_t* source, boolean* is_live);
```
Returns `true` if the Source component's stream is live. CSI and USB Camera sources will always be return `True`.

**Parameters**
* `source` - [in] unique name of the Source to query
* `is_live` - [out] `true` if the source is live, false otherwise

* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, is_live = dsl_source_is_live('my-uri-source')
```

<br>

### *dsl_source_pause*
```C
DslReturnType dsl_source_pause(const wchar_t* source);
```
Sets the state of the Source component to Paused. This method tries to change the state of an `in-use` Source component to `GST_STATE_PAUSED`. The current state of the Source component can be obtained by calling [dsl_source_state_is](#dsl_source_state_is).

**Parameters**
* `source` - unique name of the Source to pause

**Returns**
* `DSL_RESULT_SUCCESS` on successful transition. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_play('my-source')
```

<br>

### *dsl_source_resume*
```C
DslReturnType dsl_source_resume(const wchar_t* source);
```
Sets the state of a `paused` Source component to `playing`. This method tries to change the state of an `in-use` Source component to `DSL_STATE_PLAYING`. The current state of the Source component can be obtained by calling [dsl_source_state_is](#dsl_source_state_is). The Pipeline, when transitioning to a state of `DSL_STATE_PLAYING`, will set each of its Sources' 
state to `DSL_STATE_PLAYING`. An individual Source, once playing, can be paused by calling [dsl_source_pause](#dsl_source_pause).

<br>

**Parameters**
* `source` - unique name of the Source to play

**Returns**
* `DSL_RESULT_SUCCESS` on successful transition. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_resume('my-source')
```

<br>


### *dsl_source_decode_uri_get*
```C
DslReturnType dsl_source_decode_uri_get(const wchar_t* name, const wchar_t** uri);
```
This service gets the current URI in use for the named URI or RTSP source

**Parameters**
* `source` - [in] unique name of the Source to update
* `uri` - [out] unique resouce identifier in use

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
* `source` - [in] unique name of the Source to update
* `uri` - [out] unique resouce identifier in use

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_decode_uri_set('my-uri-source', '../../test/streams/sample_1080p_h264.mp4')
```

<br>

### *dsl_source_decode_drop_farme_interval_get*
```C
DslReturnType dsl_source_decode_drop_farme_interval_get(const wchar_t* name, uint* interval)
```
This service gets the current drop frame interval in use by the named URI or RTSP source

**Parameters**
* `source` - [in] unique name of the Source to update
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
* `source` - [in] unique name of the Source to update
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
This service adds a previously constructed [Dewarper](api-dewarper.md) component to either a named URI or RTSP source. A source can have at most one Dewarper, and calls to add more will fail. Attempts to add a Dewarper to a Source `in use` will fail. 

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
This service remove a [Dewarper](api-dewarper.md) component, previously added with [dsl_source_decode_dewarper_add](#dsl_source_decode_dewarper_add) to a named URI source. Calls to remove will fail if the Source is currently without a Dewarper or `in use`.

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

**Note:** both `sleep_ms` and `time_out` must be greater than 10 ms. `time_out` must be >= `sleep_ms` and should be set as a multiple of. Calling this service during an active "reconnection-cycle" will terminate the current attempt with a new cycle started using the new parameters. The current number of retries will not be reset.

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

### *dsl_source_rtsp_reconnection_stats_get*
```C
DslReturnType dsl_source_rtsp_reconnection_stats_get(const wchar_t* name, 
    time_t* last, uint* count, boolean* in_reconnect, uint* retries); 
```
This service gets the current reconnection stats for the named RTSP Source.

**Parameters**
 * `name` - [in] unique name of the Source to query
 * `last` - [out] time of the last reconnect from the system time in seconds (see timeval <sys/time.h>)
 * `count` - [out] the count of reconnections for the named source, since first played or cleared.
 * `in_reconnect` - [out] true if the RTSP source is currently in a "reconnection-cycle"
 * `retries` [out] - number of reconnection retries in either the current reconnection-cycle if `in_reconnect == true` or the last reconnection if `in_reconnect == false`. 
 
**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, last, count, in_reconnect, retries = dsl_source_rtsp_reconnection_stats_get('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_reconnection_stats_clear*
```C
DslReturnType dsl_source_rtsp_reconnection_stats_clear(const wchar_t* name); 
```
This service clears the current reconnection stats for the named RTSP Source. 

**Note:** the connection `retries` count will not be cleared if `in_reconnect == true`

**Parameters**
 * `name` - [in] unique name of the Source to update
 
**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval, last, count, in_reconnect, retries = dsl_source_rtsp_reconnection_stats_get('my-rtsp-source')
```
<br>

### *dsl_source_rtsp_state_change_listener_add*
```C
DslReturnType dsl_source_rtsp_state_change_listener_add(const wchar_t* pipeline, 
    state_change_listener_cb listener, void* user_data);
```
This service adds a callback function of type [dsl_state_change_listener_cb](#dsl_state_change_listener_cb) to a
RTSP Source identified by it's unique name. The function will be called on every change-of-state with `old_state`, `new_state`, and the client provided `user_data`. Multiple callback functions can be registered with one Source, and one callback function can be registered with multiple Sources. 

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
pipeline identified by it's unique name.

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
This service adds a named Tap to a named RTSP source
**Parameters**
 * `name` [in] name of the source object to update
 * `tap` [in] name of the Tap to add

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
 * `name` [in] name of the source object to update

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_rtsp_tap_remove('my-rtsp-source')
```

<br>


### *dsl_source_num_in_use_get*
```C
uint dsl_source_num_in_use_get();
```
This service returns the total number of all Sinks currently `in-use` by all Pipelines.

**Returns**
* The current number of Sinks `in-use`

**Python Example**
```Python
sources_in_use = dsl_source_num_in_use_get()
```

<br>

### *dsl_source_num_in_use_max_get*
```C
uint dsl_source_num_in_use_max_get();
```
This service returns the "maximum number of Sources" that can be `in-use` at any one time, defined as `DSL_DEFAULT_SOURCE_NUM_IN_USE_MAX` on service initilization, and can be updated by calling [dsl_source_num_in_use_max_set](#dsl_source_num_in_use_max_set). The actual maximum is impossed by the Jetson model in use. It's the responsibility of the client application to set the value correctly.

**Returns**
* The current max number of Sources that can be `in-use` by all Pipelines at any one time. 

**Python Example**
```Python
max_source_in_use = dsl_source_num_in_use_max_get()
```

<br>

### *dsl_source_num_in_use_max_set*
```C
boolean dsl_source_num_in_use_max_set(uint max);
```
This service sets the "maximum number of Source" that can be `in-use` at any one time. The value is defined as `DSL_DEFAULT_SOURCE_NUM_IN_USE_MAX` on service initilization. The actual maximum is impossed by the Jetson model in use. It's the responsibility of the client application to set the value correctly.

**Returns**
* `false` if the new value is less than the actual current number of Sources in use, `true` otherwise

**Python Example**
```Python
retval = dsl_source_num_in_use_max_set(24)
```
---

## API Reference
* [List of all Services](/docs/api-reference-list.md)
* [Pipeline](/docs/api-pipeline.md)
* **Source**
* [Tap](/docs/api-tap.md)
* [Dewarper](/docs/api-dewarper.md)
* [Primary and Secondary GIE](/docs/api-gie.md)
* [Tracker](/docs/api-tracker.md)
* [Tiler](/docs/api-tiler.md)
* [On-Screen Display](/docs/api-osd.md)
* [Demuxer and Splitter](/docs/api-tee.md)
* [Sink](/docs/api-sink.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* [ODE Acton](/docs/api-ode-action.md)
* [ODE Area](/docs/api-ode-area.md)
* [Display Type](/docs/api-display-type.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
