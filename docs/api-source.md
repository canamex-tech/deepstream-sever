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

Sources are added to a Pipeline by calling [dsl_pipeline_component_add](api-pipeline.md#dsl_pipeline_component_add) or [dsl_pipeline_component_add_many](api-pipeline.md#dsl_pipeline_component_add_many) and removed with [dsl_pipeline_component_remove](api-pipeline.md#dsl_pipeline_component_remove), [dsl_pipeline_component_remove_many](api-pipeline.md#dsl_pipeline_component_remove_many), or [dsl_pipeline_component_remove_all]((api-pipeline.md#dsl_pipeline_component_remove_all).

When adding multiple sources to a Pipeline, all must have the same `is_live` setting; `true` or `false`. The add services will fail on first exception. 

The relationship between Pipelines and Sources is one-to-many. Once added to a Pipeline, a Source must be removed before it can used with another. All sources are deleted by calling [dsl_component_delete](api-component.md#dsl_component_delete), [dsl_component_delete_many](api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](api-component.md#dsl_component_delete_all). Calling a delete service on a Source `in-use` by a Pipeline will fail.

#### Sources and Demuxers
When using a [Demuxer](/docs/api-tiler.md), vs. a Tiler component, each demuxed source stream must have one or more downstream [Sink](/docs/api-sink) components to end the stream. To identify this relationship, each sink is added to its upstream Source component vs. the Pipeline directly. See [dsl_source_sink_add](#dsl_source_sink_add) and [dsl_source_sink_remove](#dsl_source_sink_remove). An optional [On-Screen Display (OSD)](/docs/api-osd.md) component can be add to each source when using a Demuxer as well. See [dsl_source_osd_add](#dsl_source_osd_add) and [dsl_source_osd_remove](#dsl_source_osd_remove).

#### Maximum Source Control
There is no practical limit to the number of Sources that can be created, just to the number of Sources that can be `in use` - a child of a Pipeline - at one time. The `in-use` limit is imposed by the Jetson Model in use. 

The maximum number of `in-use` Sources is set to `DSL_DEFAULT_SOURCE_IN_USE_MAX` on DSL initialization. The value can be read by calling [dsl_source_num_in_use_max_get](#dsl_source_num_in_use_max_get) and updated with [dsl_source_num_in_use_max_set](#dsl_source_num_in_use_max_set). The number of Sources in use by all Pipelines can obtained by calling [dsl_source_get_num_in_use](#dsl_source_get_num_in_use). 


## Source API
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
* [dsl_source_rtsp_tap_add](#dsl_source_rtsp_tap_add)
* [dsl_source_rtsp_tap_remove](#dsl_source_rtsp_tap_remove)
* [dsl_source_num_in_use_get](#dsl_source_num_in_use_get)
* [dsl_source_num_in_use_max_get](#dsl_source_num_in_use_max_get)
* [dsl_source_num_in_use_max_set](#dsl_source_num_in_use_max_set)

## Return Values
Streaming Source Methods use the following return codes, in addition to the general [Component API Return Values](/docs/api-component.md).
```C++
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
```

## Cuda Decode Memory Types
```C++
#define DSL_CUDADEC_MEMTYPE_DEVICE                                  0
#define DSL_CUDADEC_MEMTYPE_PINNED                                  1
#define DSL_CUDADEC_MEMTYPE_UNIFIED                                 2
```

## RTP Protocols
```C++
#define DSL_RTP_TCP                                                 0x04
#define DSL_RTP_ALL                                                 0x07
```

<br>

## Constructors

### *dsl_source_csi_new*
```C++
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
```C++
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
```C++
DslReturnType dsl_source_uri_new(const wchar_t* name, const wchar_t* uri, boolean is_live,
    uint cudadec_mem_type, boolean intra_decode, uint drop_frame_interval);
```
This service creates a new, uniquely named URI Source component

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `http://`, `https://`,  or `file://` 
* `is_live` [in] `true` if the URI is a live source, `false` otherwise. File URI's will used a fixed value of `false`
* `cudadec_mem_type` - [in] one of the [Cuda Decode Memory Types](#Cuda Decode Memory Types) defined below
* `intra_decode` - [in] set to true for M-JPEG codec format
* `drop_frame_interval` [in] interval to drop frames at. 0 = decode all frames

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_uri_new('dsl_source_uri_new', '../../test/streams/sample_1080p_h264.mp4',
    False, DSL_CUDADEC_MEMTYPE_DEVICE, 0)
```

<br>

### *dsl_source_rtsp_new*
```C++
DslReturnType dsl_source_rtsp_new(const wchar_t* name, const wchar_t* uri, uint protocol,
    uint cudadec_mem_type, uint intra_decode, uint drop_frame_interval, uint latency);
```

This service creates a new, uniquely named URI Source component

**Parameters**
* `name` - [in] unique name for the new Source
* `uri` - [in] fully qualified URI prefixed with `http://`, `https://`
* `is_live` [in] `true` if the URI is a live source, `false` otherwise.
* `protocol` - [in] one of the [RTP Protocols](#rtp-protocols) define above
* `cudadec_mem_type` - [in] one of the [Cuda Decode Memory Types](#Cuda Decode Memory Types) defined above
* `drop_frame_interval` [in] interval to drop frames at. 0 = decode all frames
* `latency` [in] source latency setting in milliseconds

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_source_uri_new('dsl_source_uri_new', 'http://localhost::8050/rtsp-camera-1',
    True, DSL_CUDADEC_MEMTYPE_DEVICE, 0)
```

<br>


## Destructors
As with all Pipeline components, Sources are deleted by calling [dsl_component_delete](api-component.md#dsl_component_delete), [dsl_component_delete_many](api-component.md#dsl_component_delete_many), or [dsl_component_delete_all](api-component.md#dsl_component_delete_all)

## Methods

### *dsl_source_dimensions_get*
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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
```C++
DslReturnType dsl_source_decode_dewarper_add(const wchar_t* source, const wchar_t* dewarper);
```
This service adds a previously constructed [Dewarper](api-dewarper.md) component to either a named URI or RTSP source. A source can have at most one Dewarper, and calls to add more will fail. Attempts to add a Dewarper to a Source `in use` will fail. 

**Parameters**
* `source` - [in] unique name of the Source to update
* `dewarper` - [in] unique name of the Dewarper to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_decode_dewarper_add('my-uri-source', 'my-dewarper')
```

<br>

### *dsl_source_decode_dewarper_remove*
```C++
DslReturnType dsl_source_decode_dewarper_remove(const wchar_t* source);
```
This service remove a [Dewarper](api-dewarper.md) component, previously added with [dsl_source_decode_dewarper_add](#dsl_source_decode_dewarper_add) to a named URI source. Calls to remove will fail if the Source is currently without a Dewarper or `in use`.

**Parameters**
* `source` - [in] unique name of the Source to update

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_source_uri_dewarper_remove('my-uri-source')
```

<br>

### *dsl_source_rtsp_tap_add*
```C++
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
```C++
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
```C++
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
```C++
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
```C++
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



