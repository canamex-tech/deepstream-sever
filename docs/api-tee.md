# Demuxer and Splitter - Tee API
There are currently two types of Tees -- Demuxer and Splitter -- each with a very specific use and purpose. Both connect to downstream [Branches](/docs/api-branch.md). 

### Demuxer Tee
The Demuxer Tee is built-on NVIDIA's [Gst-nvstreamdemux plugin](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvstreamdemux.html#gst-nvstreamdemux) which, from the documentation, _"demuxes batched frames into individual buffers. It creates a separate Gst Buffer for each frame in the batch. It does not copy the video frames. Each Gst Buffer contains a pointer to the corresponding frame in the batch. The plugin pushes the unbatched Gst Buffer objects downstream on the pad corresponding to each frame’s source."_

### Splitter Tee
The Splitter Tee splits the stream -- batched or single frame -- to multiple source-pads, each connected to a unique Branch. The Tee does not copy the Gst Buffer, it simply pushes a pointer to the same buffer to each downstream Branch. 

### Sink Components as Branches
[Sink components](/docs/api-sinks.md) can be added as branches to both Demuxers and Splitters -- be aware that most Sinks require a batch size of 1.

### Dynamic Branching
With Demuxer and Splitter Tees, Branches can be added and removed at runtime while the Pipeline is playing. Refer to the [Dynamic Pipelines](/docs/overview.md#dynamic-pipelines) section under the [DSL Overview](/docs/overview.md) for more information. The Remuxer Tee does _**not**_ support dynamic branch updates at this time.

**IMPORTANT!** When using a Demuxer Tee, the maximum number of Branches must be specified prior to playing the Pipeline, a requirement imposed by NVIDIA's plugin.

### Tee Construction and Destruction
Demuxers and Splitters are created using a type specific constructor, [`dsl_tee_demuxer_new`](#dsl_tee_demuxer_new) and [`dsl_tee_splitter_new`](#dsl_tee_splitter_new) respectively.

The relationship between Pipeline/Branch and Tee is one to one with the Tee becoming the end component. Once added to a Pipeline or Branch, a Tee must be removed before it can be used with another. The relationship between Tees and Branches is one-to-many. 

Tees and Branches are deleted by calling [`dsl_component_delete`](api-component.md#dsl_component_delete), [`dsl_component_delete_many`](api-component.md#dsl_component_delete_many), or [`dsl_component_delete_all`](api-component.md#dsl_component_delete_all)

### Adding and removing Branches from a Tee
Branches are added to a Tee by calling [`dsl_tee_branch_add`](api-branch.md#dsl_tee_branch_add) or [`dsl_tee_branch_add_many`](api-branch.md#dsl_tee_branch_add_many) and removed with [`dsl_tee_branch_remove`](api-branch.md#dsl_tee_branch_remove), [`dsl_tee_branch_remove_many`](api-branch.md#dsl_tee_branch_remove_many), or [`dsl_tee_branch_remove_all`](api-branch.md#dsl_tee_branch_remove_all).

## Tee API
**Constructors**
* [`dsl_tee_demuxer_new`](#dsl_tee_demuxer_new)
* [`dsl_tee_demuxer_new_branch_add_many`](#dsl_tee_demuxer_new_branch_add_many)
* [`dsl_tee_splitter_new`](#dsl_tee_splitter_new) 
* [`dsl_tee_splitter_new_branch_add_many`](#dsl_tee_demuxer_new_branch_add_many)

**Tee Methods**
* [`dsl_tee_branch_add`](#dsl_tee_branch_add)
* [`dsl_tee_branch_add_many`](#dsl_tee_branch_add_many)
* [`dsl_tee_branch_remove`](#dsl_tee_branch_remove)
* [`dsl_tee_branch_remove_many`](#dsl_tee_branch_remove_many)
* [`dsl_tee_branch_remove_all`](#dsl_tee_branch_remove_all)
* [`dsl_tee_blocking_timeout_get`](#dsl_tee_blocking_timeout_get)
* [`dsl_tee_blocking_timeout_set`](#dsl_tee_blocking_timeout_set)
* [`dsl_tee_pph_add`](#dsl_tee_pph_add)
* [`dsl_tee_pph_remove`](#dsl_tee_pph_remove)
    
**Demuxer Tee Methods**
* [`dsl_tee_demuxer_branch_add_to`](#dsl_tee_demuxer_branch_add_to)
* [`dsl_tee_demuxer_branch_move_to`](#dsl_tee_demuxer_branch_move_to)
* [`dsl_tee_demuxer_max_branches_get`](#dsl_tee_demuxer_max_branches_get)
* [`dsl_tee_demuxer_max_branches_set`](#dsl_tee_demuxer_max_branches_set)

## Return Values
The following return codes are used by the Tee API
```C++
#define DSL_RESULT_TEE_RESULT                                       0x000A0000
#define DSL_RESULT_TEE_NAME_NOT_UNIQUE                              0x000A0001
#define DSL_RESULT_TEE_NAME_NOT_FOUND                               0x000A0002
#define DSL_RESULT_TEE_NAME_BAD_FORMAT                              0x000A0003
#define DSL_RESULT_TEE_THREW_EXCEPTION                              0x000A0004
#define DSL_RESULT_TEE_SET_FAILED                                   0x000A0005
#define DSL_RESULT_TEE_BRANCH_IS_NOT_BRANCH                         0x000A0006
#define DSL_RESULT_TEE_BRANCH_IS_NOT_CHILD                          0x000A0007
#define DSL_RESULT_TEE_BRANCH_ADD_FAILED                            0x000A0008
#define DSL_RESULT_TEE_BRANCH_MOVE_FAILED                           0x000A0009
#define DSL_RESULT_TEE_BRANCH_REMOVE_FAILED                         0x000A000A
#define DSL_RESULT_TEE_HANDLER_ADD_FAILED                           0x000A000B
#define DSL_RESULT_TEE_HANDLER_REMOVE_FAILED                        0x000A000C
#define DSL_RESULT_TEE_COMPONENT_IS_NOT_TEE                         0x000A000D
```

## Constant Values
The default blocking-timeout value is used by both the Splitter and Demuxer Tees. IMPORTANT! The timeout controls the amount of time the Tee will wait for a blocking PPH to be called to dynamically link or unlink a branch at runtime while the Pipeline is playing. This value will need to be extended if the frame-rate for the stream is less than 2 fps.  The timeout is needed in case the Source upstream has been removed or is in a bad state in which case the pad callback will never be called.
```C
#define DSL_TEE_DEFAULT_BLOCKING_TIMEOUT_IN_SEC                     1
```

## Constructors

### *dsl_tee_demuxer_new*
```C++
DslReturnType dsl_tee_demuxer_new(const wchar_t* name, uint max_branches);
```
The constructor creates a uniquely named Demuxer Tee. Construction will fail if the name is currently in use. 

**Parameters**
* `name` - [in] unique name for the Demuxer to create.
* `max_branches` - [in] maximum number of branches that can be added/connected to this Demuxer,  before or while the Pipeline is playing.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_tee_demuxer_new('my-demuxer', 8)
```

<br>

### *dsl_tee_demuxer_new_branch_add_many*
```C++
DslReturnType dsl_tee_demuxer_new_branch_add_many(const wchar_t* name, 
    uint max_branches, const wchar_t** branches);
```
The constructor creates a uniquely named Demuxer Tee and adds a list of branches to it. Construction will fail if the name is currently in use. 

**Parameters**
* `name` - [in] unique name for the Demuxer to create.
* `max_branches` - [in] maximum number of branches that can be added/connected to this Demuxer, before or while the Pipeline is playing.
* `branches` [in] Null terminated list of unique branch names to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_tee_demuxer_new_branch_add_many('my-demuxer', 2, 
    ['my-branch-1', 'my-branch-2', None])
```

<br>

### *dsl_tee_splitter_new*
```C++
DslReturnType dsl_tee_splitter_new(const wchar_t* name);
```
The constructor creates a uniquely named Splitter Tee. Construction will fail if the name is currently in use. 

**Parameters**
* `name` - [in] unique name for the Splitter to create.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_tee_splitter_new('my-splitter')
```

<br>

### *dsl_tee_splitter_new_branch_add_many*
```C++
DslReturnType dsl_tee_splitter_new_branch_add_many(const wchar_t* name, const wchar_t** branches)
```
The constructor creates a uniquely named Splitter Tee and adds a list of Branches to it. Construction will fail if the name is currently in use. 

**Parameters**
* `name` - [in] unique name for the Splitter to create.
* `branches` [in] Null terminated list of unique branch names to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_tee_splitter_new_branch_add_many('my-splitter', 
   ['my-branch-1', 'my-branch-2', None])
```

<br>

## Methods
### *dsl_tee_branch_add*
```C++
DslReturnType dsl_tee_branch_add(const wchar_t* name, const wchar_t* branch);
```
This service adds a single branch to a named Splitter or Demuxer Tee. The add service will fail if the branch is currently `in-use`. The branch's `in-use` state will be set to `true` on successful add. 

**Parameters**
* `branch` - [in] unique name for the Branch to update.
* `component` - [in] unique name of the Component to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_branch_add('my-splitter', 'my-branch’)
```

<br>

### *dsl_tee_branch_add_many*
```C++
DslReturnType dsl_tee_branch_add_many(const wchar_t* name, const wchar_t** branches);
```
This service adds a list of named Branches to a to a named Splitter or Demuxer Tee.  Each of the branches' `in-use` state will be set to true on successful add. 

**Parameters**
* `name` - [in] unique name for the Demuxer or Splitter Tee to update.
* `branches` - [in] a NULL terminated array of unique Branch names to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_tee_branch_add_many('my-splitter', 
   ['my-branch-1', 'my-branch-2', None])
```

<br>

### *dsl_tee_branch_remove*
```C++
DslReturnType dsl_tee_branch_remove(const wchar_t* name, const wchar_t* branch);
```
This service removes a single named Branch from a Demuxer or Splitter Tee. The remove service will fail if the Branch is not currently `in-use` by the Tee. The branch's `in-use` state will be set to `false` on successful removal. 

**Parameters**
* `name` - [in] unique name for the Demuxer or Splitter Tee to update.
* `branch` - [in] unique name of the Branch to remove.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_branch_remove('my-splitter', 'my-branch')
```
<br>

### *dsl_tee_branch_remove_many*
```C++
DslReturnType dsl_tee_branch_remove_many(const wchar_t* name, const wchar_t** branches);
```
This service removes a list of named components from a named Branch. The remove service will fail if any of the branches are currently `not-in-use` by the named Branch.  All of the removed branches' `in-use` state will be set to `false` on successful removal. 

**Parameters**
* `name` - [in] unique name for the Demuxer or Splitter Tee to update.
* `components` - [in] a NULL terminated array of unique Branch names to remove.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_branch_remove_many('my-splitter',
    ['my-branch-1', 'my-branch-2', None])
```

<br>

### *dsl_tee_branch_remove_all*
```C++
DslReturnType dsl_tee_branch_remove_all(const wchar_t* name);
```
This service removes all child branches from a named Demuxer or Splitter Tee. All of the removed branches' `in-use` state will be set to `false` on successful removal. 

**Parameters**
* `name` - [in] unique name for the Demuxer or Splitter Tee to update.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_branch_remove_all('my-splitter')
```

<br>

### *dsl_tee_blocking_timeout_get*
```C++
DslReturnType dsl_tee_blocking_timeout_get(const wchar_t* name, 
    uint* timeout);
```
This service gets the current [blocking-timeout](#constant-values) for the named Tee. 

**Parameters**
* `name` - [in] unique name of the Demuxer or Splitter Tee to query.
* `timeout` - [out] current blocking-timeout in units of seconds. Default = [DSL_TEE_DEFAULT_BLOCKING_TIMEOUT_IN_SEC](#constant-values).

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, timeout = dsl_tee_blocking_timeout_get('my-demuxer')
```

<br>

### *dsl_tee_blocking_timeout_set*
```C++
DslReturnType dsl_tee_blocking_timeout_set(const wchar_t* name, 
    uint timeout);
```
This service sets the [blocking-timeout](#constant-values) setting for the named Demuxer or Splitter Tee to use.

**Parameters**
* `name` - [in] unique name of the Demuxer or Splitter Tee to update.
* `max_branches` - [in] new value for blocking-timeout in units of seconds. 

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_blocking_timeout_set('my-demuxer', 5)
```

<br>

### *dsl_tee_pph_add*
```C++
DslReturnType dsl_tee_pph_add(const wchar_t* name, const wchar_t* handler);
```
This service adds a named Pad-Probe-Handler to the sink pad (only) of the named Demuxer or Splitter Tee. More than one PPH can be added to a single Tee Component.

**Parameters**
 * `name` [in] unique name of the Demuxer or SplitterTee to update
 * `handler` [in] unique name of the PPH to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_pph_add('my-demuxer-tee', 'my-meter-pph')
```

<br>

### *dsl_tee_pph_remove*
```C++
DslReturnType dsl_tee_pph_remove(const wchar_t* name, const wchar_t* handler);
```
This service removes a named Pad-Probe-Handler from a named Demuxer or Splitter Tee. The services will fail if the handler is not a child of the Tee.

**Parameters**
 * `name` [in] unique name of the Demuxer or Splitter Tee to update.
 * `handler` [in] unique name of the Pad probe handler to remove.

**Returns**
* `DSL_RESULT_SUCCESS` on successful remove. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_pph_remove('my-demuxer-tee', 'my-meter-pph')
```

<br>

---

## Demuxer Tee Methods
### *dsl_tee_demuxer_branch_add_to*
```C++
DslReturnType dsl_tee_demuxer_branch_add_to(const wchar_t* name, 
    const wchar_t* branch, uint stream_id);
```
This service adds a single [Branch component](/docs/api-branch.md) or [Sink component](/docs/api-sink.md) to a specific output stream (pad) of a named Demuxer Tee. 

**IMPORTANT!** This service may be called at runtime (while the Pipeline is playing) if there is an active/playing Source component for the specified stream.

**Parameters**
* `name` - [in] unique name of the Deuxer to update.
* `branch` - [in] unique name of the Branch to add. This may be a [Branch component](/docs/api-branch.md) or [Sink component](/docs/api-sink.md).
* `stream_id` - [in] 0-based unique id of the Demuxer output stream to add the branch to.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_demuxer_branch_add_to('my-demuxer', 'my-branch’, 2)
```

<br>

### *dsl_tee_demuxer_branch_move_to*
```C++
DslReturnType dsl_tee_demuxer_branch_move_to(const wchar_t* name, 
    const wchar_t* branch, uint stream_id);
```
This service moves a single [Branch component](/docs/api-branch.md) or [Sink component](/docs/api-sink.md) to a specific output stream (pad) of a named Demuxer Tee. The Branch must be a child of the Demuxer when this service is called.

**IMPORTANT!** This service may be called at runtime (while the Pipeline is playing) if there is an active/playing Source component for the specified stream.

**Parameters**
* `name` - [in] unique name of the Deuxer to update.
* `branch` - [in] unique name of the Branch to move. This may be a [Branch component](/docs/api-branch.md) or [Sink component](/docs/api-sink.md).
* `stream_id` - [in] 0-based unique id of the Demuxer output stream to move the branch to.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_demuxer_branch_move_to('my-demuxer', 'my-branch’, 2)
```

<br>

### *dsl_tee_demuxer_max_branches_get*
```C++
DslReturnType dsl_tee_demuxer_max_branches_get(const wchar_t* name, 
    uint* max_branches);
```
This service gets the current max-branches setting for the named Deumuxer Tee.

**Parameters**
* `name` - [in] unique name of the Deuxer to query.
* `max_branches` - [out] current setting for max-branches, the maximum number of branches that can be added to the Demuxer in any state. 

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, max_branches = dsl_tee_demuxer_max_branches_get('my-demuxer')
```

<br>

### *dsl_tee_demuxer_max_branches_set*
```C++
DslReturnType dsl_tee_demuxer_max_branches_set(const wchar_t* name, 
    uint max_branches);
```
This service sets the max-branches setting for the named Deumuxer Tee to use. This service will fail if called while the Pipeline is currently playing.

**Parameters**
* `name` - [in] unique name of the Deuxer to update.
* `max_branches` - [in] new setting for max-branches, the maximum number of branches that can be added to the Demuxer in any state. 

**Returns**
* `DSL_RESULT_SUCCESS` on successful update. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_tee_demuxer_max_branches_set('my-demuxer', 10)
```

<br>

---

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
* **Demuxer and Splitter**
* [Remuxer](/docs/api-remuxer.md)
* [On-Screen Display](/docs/api-osd.md)
* [Sink](/docs/api-sink.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
* [GST Element](/docs/api-gst.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* [ODE Accumulator](/docs/api-ode-accumulator.md)
* [ODE Acton](/docs/api-ode-action.md)
* [ODE Area](/docs/api-ode-area.md)
* [ODE Heat-Mapper](/docs/api-ode-heat-mapper.md)
* [Display Type](/docs/api-display-type.md)
* [Mailer](/docs/api-mailer.md)
* [WebSocket Server](/docs/api-ws-server.md)
* [Message Broker](/docs/api-msg-broker.md)
* [Info API](/docs/api-info.md)
