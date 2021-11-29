# ODE Action API Reference
ODE Actions implement their own "action-specific" event-handler that gets invoked by an ODE Trigger on the occurrence of an Object Detection Event (ODE). The relationship between ODE Triggers and ODE Actions is many-to-many. Multiple ODE Actions can be added to an ODE Trigger and the same ODE Action can be added to multiple ODE Triggers.

#### Actions on Metadata
Several ODE Actions can be created to update the Frame and object Metadata to be rendered by a downstream [On-Screen-Display](/docs/api-osd.md) if added.  See [dsl_ode_action_format_bbox_new](#dsl_ode_action_format_bbox_new), [dsl_ode_action_format_label_new](#dsl_ode_action_format_label_new),  [dsl_ode_action_customize_label_new](#dsl_ode_action_customize_label_new), [dsl_ode_action_fill_frame_new](#dsl_ode_action_fill_frame_new), and [dsl_ode_action_fill_object_new](#dsl_ode_action_fill_object_new).

#### Actions on Record Components
There are two actions that start a new recording session, one for the [Record-Sink](/docs/api-sink.md) created with [dsl_ode_action_sink_record_start_new](#dsl_ode_action_sink_record_start_new) and the other for the [Record-Tap](/docs/api-tap.md) created with [dsl_ode_action_tap_record_start_new](#dsl_ode_action_tap_record_start_new)

#### Actions on Actions
Actions can be created to Disable other Actions on invocation. See [dsl_ode_action_action_disable_new](#dsl_ode_action_action_disable_new) and [dsl_ode_action_action_enable_new](#dsl_ode_action_action_enable_new). 

#### Actions with ODE Occurrence Data
Actions performed with the ODE occurrence data include  [dsl_ode_action_callback_new](#dsl_ode_action_callback_new), [dsl_ode_action_display_new](#dsl_ode_action_display_new), [dsl_ode_action_log_new](#dsl_ode_action_log_new), [dsl_ode_action_email_new](dsl_ode_action_email_new), and [dsl_ode_action_print_new](#dsl_ode_action_print_new)

#### Actions on Areas
Actions can be used to Add and Remove Areas to/from a Trigger on invocation. See [dsl_ode_action_area_add_new](#dsl_ode_action_area_add_new) and [dsl_ode_action_area_remove_new](#dsl_ode_action_area_remove_new). 

#### Actions on Triggers
Actions can be created to Disable, Enable or Reset a Trigger on invocation. See [dsl_ode_action_trigger_reset_new](#dsl_ode_action_trigger_reset_new), [dsl_ode_action_trigger_disable_new](#dsl_ode_action_trigger_disable_new), and [dsl_ode_action_trigger_enable_new](#dsl_ode_action_trigger_enable_new).

#### Actions on Pipelines
There are a number of Actions that dynamically update the state or components in a Pipeline. [dsl_ode_action_pause_new](#dsl_ode_action_pause_new), [dsl_ode_action_sink_add_new](#dsl_ode_action_sink_add_new), [dsl_ode_action_sink_remove_new](#dsl_ode_action_sink_remove_new), [dsl_ode_action_source_add_new](#dsl_ode_action_source_add_new), [dsl_ode_action_source_remove_new](#dsl_ode_action_source_remove_new).

#### ODE Action Construction and Destruction
ODE Actions are created by calling one of type specific [constructors](#ode-action-api) defined below. Each constructor must have a unique name from all other Actions. Once created, Actions are deleted by calling [dsl_ode_action_delete](#dsl_ode_action_delete), [dsl_ode_action_delete_many](#dsl_ode_action_delete_many), or [dsl_ode_action_delete_all](#dsl_ode_action_delete_all). Attempting to delete an Action in-use by a Trigger will fail.

#### Adding/Removing Actions
ODE Actions are added to an ODE Trigger by calling [dsl_ode_trigger_action_add](docs/api-ode-trigger#dsl_ode_trigger_action_add) and [dsl_ode_trigger_action_add_many](docs/api-ode-trigger#dsl_ode_trigger_action_add_many) and removed with [dsl_ode_trigger_action_remove](docs/api-ode-trigger#dsl_ode_trigger_action_remove), [dsl_ode_trigger_action_remove_many](docs/api-ode-traigger#dsl_ode_trigger_action_remove_many), and [dsl_ode_trigger_action_remove_all](docs/api-ode-trigger#dsl_ode_trigger_action_remove_all).


## ODE Action API
**Types:**
* [dsl_capture_info](#dsl_capture_info)

**Callback Types:**
* [dsl_capture_complete_listener_cb](#dsl_capture_complete_listener_cb)
* [dsl_ode_handle_occurrence_cb](dsl_ode_handle_occurrence_cb)

**Constructors:**
* [dsl_ode_action_action_disable_new](#dsl_ode_action_action_disable_new)
* [dsl_ode_action_action_enable_new](#dsl_ode_action_action_enable_new)
* [dsl_ode_action_area_add_new](#dsl_ode_action_area_add_new)
* [dsl_ode_action_area_remove_new](#dsl_ode_action_area_remove_new)
* [dsl_ode_action_capture_frame_new](#dsl_ode_action_capture_frame_new)
* [dsl_ode_action_capture_object_new](#dsl_ode_action_capture_object_new)
* [dsl_ode_action_custom_new](#dsl_ode_action_custom_new)
* [dsl_ode_action_customize_label_new](#dsl_ode_action_customize_label_new)
* [dsl_ode_action_display_new](#dsl_ode_action_display_new)
* [dsl_ode_action_display_meta_add_new](#dsl_ode_action_display_meta_add_new)
* [dsl_ode_action_display_meta_add_many_new](#dsl_ode_action_display_meta_add_many_new)
* [dsl_ode_action_email_new](#dsl_ode_action_email_new)
* [dsl_ode_action_file_new](#dsl_ode_action_file_new)
* [dsl_ode_action_fill_frame_new](#dsl_ode_action_fill_frame_new)
* [dsl_ode_action_fill_surroundings_new](#dsl_ode_action_fill_surroundings_new)
* [dsl_ode_action_format_bbox_new](#dsl_ode_action_format_bbox_new)
* [dsl_ode_action_format_label_new](#dsl_ode_action_format_label_new)
* [dsl_ode_action_handler_disable_new](#dsl_ode_action_handler_disable_new)
* [dsl_ode_action_log_new](#dsl_ode_action_log_new)
* [dsl_ode_action_pause_new](#dsl_ode_action_pause_new)
* [dsl_ode_action_print_new](#dsl_ode_action_print_new)
* [dsl_ode_action_redact_new](#dsl_ode_action_redact_new)
* [dsl_ode_action_sink_add_new](#dsl_ode_action_sink_add_new)
* [dsl_ode_action_sink_remove_new](#dsl_ode_action_sink_remove_new)
* [dsl_ode_action_sink_record_start_new](#dsl_ode_action_sink_record_start_new)
* [dsl_ode_action_source_add_new](#dsl_ode_action_source_add_new)
* [dsl_ode_action_source_remove_new](#dsl_ode_action_source_remove_new)
* [dsl_ode_action_tap_record_start_new](#dsl_ode_action_tap_record_start_new)
* [dsl_ode_action_tiler_source_show_new](#dsl_ode_action_tiler_source_show_new)
* [dsl_ode_action_trigger_reset_new](#dsl_ode_action_trigger_reset_new)
* [dsl_ode_action_trigger_disable_new](#dsl_ode_action_trigger_disable_new)
* [dsl_ode_action_trigger_enable_new](#dsl_ode_action_trigger_enable_new)

**Destructors:**
* [dsl_ode_action_delete](#dsl_ode_action_delete)
* [dsl_ode_action_delete_many](#dsl_ode_action_delete_many)
* [dsl_ode_action_delete_all](#dsl_ode_action_delete_all)

**Methods:**
* [dsl_ode_action_capture_complete_listener_add](#dsl_ode_action_capture_complete_listener_add)
* [dsl_ode_action_capture_complete_listener_remove](#dsl_ode_action_capture_complete_listener_remove)
* [dsl_ode_action_capture_image_player_add](#dsl_ode_action_capture_image_player_add)
* [dsl_ode_action_capture_image_player_remove](#dsl_ode_action_capture_image_player_remove)
* [dsl_ode_action_capture_mailer_add](#dsl_ode_action_capture_mailer_add)
* [dsl_ode_action_capture_mailer_remove](#dsl_ode_action_capture_mailer_remove)
* [dsl_ode_action_customize_label_get](#dsl_ode_action_customize_label_get)
* [dsl_ode_action_customize_label_set](#dsl_ode_action_customize_label_set)
* [dsl_ode_action_enabled_get](#dsl_ode_action_enabled_get)
* [dsl_ode_action_enabled_set](#dsl_ode_action_enabled_set)
* [dsl_ode_action_list_size](#dsl_ode_action_list_size)

---

## Constants
The following symbolic constants are used by the OSD Action API
```C
#define DSL_EVENT_FILE_FORMAT_TEXT                                  0
#define DSL_EVENT_FILE_FORMAT_CSV                                   1

#define DSL_WRITE_MODE_APPEND                                       0
#define DSL_WRITE_MODE_TRUNCATE                                     1

#define DSL_METRIC_OBJECT_CLASS                                     0
#define DSL_METRIC_OBJECT_TRACKING_ID                               1
#define DSL_METRIC_OBJECT_LOCATION                                  2
#define DSL_METRIC_OBJECT_DIMENSIONS                                3
#define DSL_METRIC_OBJECT_CONFIDENCE                                4
#define DSL_METRIC_OBJECT_PERSISTENCE                               5
#define DSL_METRIC_OBJECT_OCCURRENCES                               6

```

## Return Values
The following return codes are used by the OSD Action API
```C
#define DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE                       0x000F0001
#define DSL_RESULT_ODE_ACTION_NAME_NOT_FOUND                        0x000F0002
#define DSL_RESULT_ODE_ACTION_CAPTURE_TYPE_INVALID                  0x000F0003
#define DSL_RESULT_ODE_ACTION_THREW_EXCEPTION                       0x000F0004
#define DSL_RESULT_ODE_ACTION_IN_USE                                0x000F0005
#define DSL_RESULT_ODE_ACTION_SET_FAILED                            0x000F0006
#define DSL_RESULT_ODE_ACTION_IS_NOT_ACTION                         0x000F0007
#define DSL_RESULT_ODE_ACTION_FILE_PATH_NOT_FOUND                   0x000F0008
#define DSL_RESULT_ODE_ACTION_NOT_THE_CORRECT_TYPE                  0x000F0009
#define DSL_RESULT_ODE_ACTION_CALLBACK_ADD_FAILED                   0x000F000A
#define DSL_RESULT_ODE_ACTION_CALLBACK_REMOVE_FAILED                0x000F000B
#define DSL_RESULT_ODE_ACTION_PLAYER_ADD_FAILED                     0x000F000C
#define DSL_RESULT_ODE_ACTION_PLAYER_REMOVE_FAILED                  0x000F000D
#define DSL_RESULT_ODE_ACTION_MAILER_ADD_FAILED                     0x000F000E
#define DSL_RESULT_ODE_ACTION_MAILER_REMOVE_FAILED                  0x000F000F
```
---

## Types:
### *dsl_capture_info*
```C
typedef struct dsl_capture_info
{
    uint captureId;
    const wchar_t* filename;
    const wchar_t* dirpath;
    uint width;
    uint height;
} dsl_capture_info;
```
Structure typedef used to provide Image Capture information to a Client [dsl_record_client_listener_cb](#dsl_record_client_listener_cb) function on capture and file save complete.

**Fields**
* `captureId` - the unique capture id assigned on file save.
* `filename` - filename generated for the captured image. 
* `dirpath` - directory path for the captured image
* `width` - width of the image in pixels.
* `height` - height of the image in pixels.

**Python Example**
```Python
## 
# Function to be called on Object Capture (and file-save) complete
## 
def capture_complete_listener(capture_info_ptr, client_data):
    print(' ***  Object Capture Complete  *** ')
    
    capture_info = capture_info_ptr.contents
    print('capture_id: ', capture_info.capture_id)
    print('filename:   ', capture_info.filename)
    print('dirpath:    ', capture_info.dirpath)
    print('width:      ', capture_info.width)
    print('height:     ', capture_info.height)
```

## Callback Types:
### *dsl_capture_complete_listener_cb*
```C++
typedef void (*dsl_capture_complete_listener_cb)(dsl_capture_info* info, void* client_data);
```
Callback typedef for a clients to listen for the notification that an Image or Object Capture has been completed and saved to file.

**Parameters**
* `info` [in] opaque pointer to the capture info of type [dsl_capture_info](#dsl_capture_info)
* `user_data` [in] user_data opaque pointer to client's user data, provided by the client  

<br>

### *dsl_ode_handle_occurrence_cb*
```C++
typedef void (*dsl_ode_handle_occurrence_cb)(uint64_t event_id, const wchar_t* trigger,
    void* buffer, void* display_meta, void* frame_meta, void* object_meta, void* client_data);
```
Callback typedef for a client ODE occurrence handler function. Once registered, by calling dsl_ode_action_custom_new, the function will be called on ODE occurrence. 

**Parameters**
* `event_id` [in] unique event identification.
* `trigger` [in] unique name of the ODE Event Trigger that triggered the occurrence.
* `buffer` [in] pointer to the frame buffer of type GstBuffer.
* `display_meta` [in] pointer to a NvDsDisplayMeta structure.
* `frame_meta` [in] pointer to the NvDsFrameMeta structure that triggered the ODE event.
* `object_meta` [in] pointer to the NvDsObjectMeta structure that triggered the ODE event. Note: This parameter will be set to NULL for ODE occurrences detected in Post process frame. Absence and Count ODE's as examples.
* `user_data` [in] user_data opaque pointer to client's user data, provided by the client.  

---

## Constructors
### *dsl_ode_action_action_add_new*
```C++
DslReturnType dsl_ode_action_action_add_new(const wchar_t* name, 
    const wchar_t* trigger, const wchar_t* action);
```
The constructor creates a uniquely named **Add Action** ODE Action. When invoked, this Action will attempt to add a named ODE Action to a named ODE Trigger. The Action will produce an error log message if either the Trigger or Action do not exist.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to add the ODE Action to.
* `action` - [in] unique name for the ODE Action to add to the ODE Trigger.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_action_add_new('my-add-action-action', 'my-trigger', 'my-other-action')
```

<br>

### *dsl_ode_action_action_disable_new*
```C++
DslReturnType dsl_ode_action_action_disable_new(const wchar_t* name, const wchar_t* action);
```
The constructor creates a uniquely named **Disable Action** ODE Action. When invoked, this Action will attempt to disable a named ODE Action. The Action will produce an error log message if the Action to disable does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `action` - [in] unique name for the ODE Action to disable.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_action_disable_new('my-disable-action-action', 'my-other-action')
```

<br>

### *dsl_ode_action_action_enable_new*
```C++
DslReturnType dsl_ode_action_action_enable_new(const wchar_t* name, const wchar_t* action);
```
The constructor creates a uniquely named **Enable Action** ODE Action. When invoked, this Action will attempt to enable a named ODE Action. The Action will produce an error log message if the Action to enable does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `action` - [in] unique name for the ODE Action to enable.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_action_enable_new('my-enable-action-action', 'my-other-action')
```

<br>

### *dsl_ode_action_area_add_new*
```C++
DslReturnType dsl_ode_action_area_add_new(const wchar_t* name, 
    const wchar_t* trigger, const wchar_t* area);
```
The constructor creates a uniquely named **Add Area** ODE Action. When invoked, this Action will attempt to add a named ODE Area to a named ODE Trigger. The Action will produce an error message if either the Trigger or Area do not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to add the ODE Area to.
* `area` - [in] unique name for the ODE Area to add to the ODE Trigger.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_area_add_new('my-add-area-action', 'my-trigger', 'my-area')
```

<br>

### *dsl_ode_action_area_remove_new*
```C++
DslReturnType dsl_ode_action_area_remove_new(const wchar_t* name, 
    const wchar_t* trigger, const wchar_t* area);
```
The constructor creates a uniquely named **Remove Area** ODE Action. When invoked, this Action will attempt to remove a named ODE Area from a named ODE Trigger. The Action will produce an error log message if either the Trigger or Area do not exist at the time of invocation. 

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to remove the ODE Area from.
* `area` - [in] unique name for the ODE Area to remove from the ODE Trigger.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_area_remove_new('my-remove-area-action', 'my-trigger', 'my-area')
```

<br>

### *dsl_ode_action_capture_frame_new*
```C++
DslReturnType dsl_ode_action_capture_frame_new(const wchar_t* name, const wchar_t* outdir, boolean annotate);
```
The constructor creates a uniquely named **Frame Capture** ODE Action. When invoked, this Action will capture the frame that triggered the ODE occurrence to a jpeg image file in the directory specified by `outdir`. The file name will be derived from combining the unique ODE Trigger name and unique ODE occurrence ID. The image can be annotated with one or more objects showing bounding boxes and labes. If the action is invoked by an object occurrence, then only the object will be annotated. If the action is invoked by a frame level occurrence - summation, min, max and range triggers for example - all detected objects in the frame will be annotated.

The constructor will return `DSL_RESULT_ODE_ACTION_FILE_PATH_NOT_FOUND` if `outdir` is invalid.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `outdir` - [in] absolute or relative path to the output directory to save the image file to
* `annotate` - [in] if true, the action will annotate the image with object bounding boxes and labels.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_frame_new('my-frame-capture-action', './images/frames')
```

<br>

### *dsl_ode_action_capture_object_new*
```C++
DslReturnType dsl_ode_action_capture_object_new(const wchar_t* name, const wchar_t* outdir);
```
The constructor creates a uniquely named **Object Capture** ODE Action. When invoked, this Action will capture the object -- using its OSD rectangle parameters --  that triggered the ODE occurrence to a jpeg image file in the directory specified by `outdir`. The file name will be derived from combining the unique ODE Trigger name and unique ODE occurrence ID. The constructor will return `DSL_RESULT_ODE_ACTION_FILE_PATH_NOT_FOUND` if `outdir` is invalid.

Note: Adding an Object Capture ODE Action to an Absence or Summation Trigger is meaningless and will result in a Non-Action.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `outdir` - [in] absolute or relative path to the output directory to save the image file to

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_object_new('my-object-action', './images/frames')
```

<br>

### *dsl_ode_action_custom_new*
```C++
DslReturnType dsl_ode_action_custom_new(const wchar_t* name, 
    dsl_ode_occurrence_handler_cb client_handler, void* client_data);
```
The constructor creates a uniquely named **Custom** ODE Action. When invoked, this Action will call the Client provided callback function with the Frame Meta and Object Meta that triggered the ODE occurrence. 

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `client_handler` - [in] Function of type [dsl_ode_occurrence_handler_cb](#dsl_ode_handle_occurrence_cb) to be called on Action Invocation.
* `client_data` - [in] Opaque pointer to client data returned on callback.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_callback_new('my-callback-action', my_ode_callback, my_data)
```

<br>

### *dsl_ode_action_customize_label_new*
```C++
DslReturnType dsl_ode_action_customize_label_new(const wchar_t* name,  
    const uint* content_types, uint size);
```
The constructor creates a uniquely named **Customize Label** ODE Action. When invoked, this Action updates an Object's label to display specific content.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `content_types` - [in] an array of DSL_OBJECT_LABEL constants.
* `size` - [in] size of the content_types array.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python    
retval = dsl_ode_action_display_new('my-customize-label-action', 
    [DSL_OBJECT_LABEL_TRACKING_ID, DSL_OBJECT_LABEL_PERSISTENCE], 2)
```

<br>

### *dsl_ode_action_display_new*
```C++
DslReturnType dsl_ode_action_display_new(const wchar_t* name, 
    const wchar_t* format_string, uint offset_x, uint offset_y, 
    const wchar_t* font, boolean has_bg_color, const wchar_t* bg_color);
```
The constructor creates a uniquely named **Display Data** ODE Action. When invoked, this Action adds the `format_string` with replaced metrics as metadata to the current Frame for display by a downstream On-Screen-Display (OSD) component. 

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `format_string` - [in] format string with `%` tokens for inserting `DSL_METRIC_OBJECT_<type>` values for display.
* `offsetX` - [in] offset for the display text in the X direction.
* `offsetY` - [in] offset for the display text in the Y direction.
* `font` - [in] unique name of the RGBA Font to use.
* `has_bg_color` - [in] set to true display the text with a background color.
* `bg_color` - [in] unique name of the RGBA Color to use for the background.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_display_new('primary-display-action',
    format_string = 'Following vehicle %{} for %{} seconds'.format(
        DSL_METRIC_OBJECT_TRACKING_ID, DSL_METRIC_OBJECT_PERSISTENCE),
    offset_x = 1010,
    offset_y = 110,
    font = 'verdana-bold-20-white',
    has_bg_color = False,
    bg_color = None)
```

<br>

### *dsl_ode_action_display_meta_add_new*
```C++
DslReturnType dsl_ode_action_display_meta_add_new(const wchar_t* name, const wchar_t* display_type);
```
The constructor creates a uniquely named **Add Display Meta** ODE Action. When invoked, this Action will add the Display Type to the Frame Meta that triggered the ODE occurrence.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `display_type` - [in] unique name of the Display Type to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_display_meta_add_new('my-add-display-meta-action', 'my-circle')
```

<br>

### *dsl_ode_action_display_meta_add_many_new*
```C++
DslReturnType dsl_ode_action_display_meta_add_new(const wchar_t* name, const wchar_t** display_types);
```
The constructor creates a uniquely named **Add Many Display Meta** ODE Action. When invoked, this Action will add many Display Types to the Frame Meta that triggered the ODE occurrence.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `display_types` - [in] NULL terminated list of unique names of the Display Types to add

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_display_meta_add_new('my-add-display-meta-action', 
    ['my-circle', 'my-rectangle', 'my-source-name', None])
```

<br>

### *dsl_ode_action_email_new*
```C++
DslReturnType dsl_ode_action_email_new(const wchar_t* name, const wchar_t* subject);
```
The constructor creates a uniquely named **Email** ODE Action. When invoked, this Action will send an Email message to all `To` and `Cc` addresses previously setup using the [SMTP Services API](/docs/api-smtp.md). The body of the email will contain all Fram/Object and Trigger Criteria information for the ODE occurence that triggered the event.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `subject` - [in] Subject line to add to all emails sent by this specific action.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_email_new('my-email-action', 'Bicycle has entered Inclussion Area')
```

<br>

### *dsl_ode_action_file_new*
```C++
DslReturnType dsl_ode_action_file_new(const wchar_t* name, 
    const wchar_t* file_path, uint mode, uint format, boolean force_flush);
```
The constructor creates a uniquely named **File** ODE Action. When invoked, this Action will write the Frame/Object and Trigger Criteria information for the ODE occurence that triggered the event to a specified file. The file will be created if one does exists. Existing file can be opened in either append or truncate modes.

Event data can be saved in one of two formats; formated text or comma seperated values (CSV). Click on the image below to view the CSV column headers and example data.

![CSV Event File Format](/Images/csv-file.png)

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `mode` - [in] file open mode, either DSL_EVENT_FILE_MODE_APPEND or DSL_EVENT_FILE_MODE_TRUNCATE
* `format` - [in] file format, either DSL_EVENT_FILE_FORMAT_TEXT or DSL_EVENT_FILE_FORMAT_CSV
* `file_path` - [in] absolute or relative file path specification of the output file to use.
* `force_flush` - [in] if set, the action will schedule a flush buffer operation to be performed by the idle thread.  

NOTE: although the flush event occurs in the lowest priority background (idle) thread, flushing is still a CPU intensive operation and should be used sparingly -- when tailing the file for runtime debugging as an example. Set to 0 to disable forced flushing, and to allow the operating system to more effectively handle the process.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_file_new('my-file-action', './event_files/my-events.csv', 
    DSL_WRITE_MODE_APPEND, DSL_EVENT_FILE_FORMAT_CSV, false)
```

<br>


### *dsl_ode_action_fill_frame_new*
```C++
DslReturnType dsl_ode_action_fill_frame_new(const wchar_t* name
    const wchar_t* color);
```
The constructor creates a uniquely named **Fill Frame** ODE Action. When invoked, this Action will fill the Frame with a rectangle background color added to the Frame Meta that triggered the ODE occurrence.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `color` - [in] RGBA Display Type color to fill the frame with.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_fill_frame_new('my-fill-frame-action', 'opaque-red')
```

<br>

### *dsl_ode_action_fill_surroundings_new*
```C++
DslReturnType dsl_ode_action_fill_surroundings_new(const wchar_t* name, const wchar_t* color);
```
The constructor creates a uniquely named **Fill Surroundings** ODE Action. When invoked, this Action will file the Object's surroundings, meaning everything but the objects rectangle by updating the frame metadata on ODE occurrence.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `color` - [in] RGBA Color Display Type to fill the object backround with.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_fill_surroundings_new('my-fill-object-action', 'opaque-grey')
```

<br>

### *dsl_ode_action_format_bbox_new*
```C++
DslReturnType dsl_ode_action_format_bbox_new(const wchar_t* name, uint border_width, 
    const wchar_t* border_color, boolean has_bg_color, const wchar_t* bg_color);
```
The constructor creates a uniquely named **Format Bounding Box** ODE Action. When invoked, this Action updates an Object's RGBA bounding-box line width and color for display by a downstream On-Screen-Display (OSD) component. This action can be used to hide the Object's bounding-box from view.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `border_width` - [in] border_width border line-width for the object's bounding box. Use 0 to remove the border from view.
* `border_color` - [in] unique name of the RGBA Color to use for the bounding box border. Use NULL for no-color when setting `border_width` = 0.
* `has_bg_color` - [in] set to true to fill the bounding box background color, false otherwise. 
* `bg_color` - [in] unique name of the RGBA Color to use for the background. Use NULL for no-color when `has_bg_color` = false.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_format_bbox_new('my-format-bbox-action', 
    4, 'my-custom-color, true, 'my-custom-bg-color')
```

<br>

### *dsl_ode_action_format_label_new*
```C++
DslReturnType dsl_ode_action_format_label_new(const wchar_t* name, 
    const wchar_t* font, boolean has_bg_color, const wchar_t* bg_color);
```
The constructor creates a uniquely named **Format Label** ODE Action. When invoked, this Action updates an Object's label font and color for display by a downstream On-Screen-Display (OSD) component. This action can be used to hide the Object's label from view.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `font` - [in] unique name of the RGBA Font to use for the bounding box border. Use NULL to remove the Object's label from view.
* `has_bg_color` - [in] set to true to fill the bounding box background color, false otherwise. 
* `bg_color` - [in] unique name of the RGBA Color to use for the background. Use NULL for no-color when `has_bg_color` = false.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_format_label_new('my-format-label-action', 
    'my-custom-font, true, 'my-custom-bg-color')
```

<br>


### *dsl_ode_action_handler_disable_new*
```C++
DslReturnType dsl_ode_action_handler_disable_new(const wchar_t* name, const wchar_t* handler);
```
The constructor creates a uniquely named **Disable Handler** ODE Action. When invoked, this Action will disable a named ODE Handler. The action will produce an error log message if the Handler does not exist at the time of invocation.


**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `text` - [in] unique name for the ODE Handler to disable

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_handler_disable_new('my-disable-handler-action', 'my-handler)
```

<br>

### *dsl_ode_action_log_new*
```C++
DslReturnType dsl_ode_action_log_new(const wchar_t* name);
```
The constructor creates a uniquely named **GStreamer Log** ODE Action. When invoked, this Action will log ODE information from the Frame and Object metadata to the GStreamer logging service at a level of INFO using the DSL token. The GST_DEBUG level must be set to 4 or higher for the information to be logged. The following console example sets the default GStreamer level to ERROR=1 and DSL to INFO=4.
```
$ export GST_DEBUG=1,DSL:4 
```

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `text` - [in] if true, the action hides the display text for Object/Frame that triggered the ODE occurrence
* `border` - [in] if true, the action hides the rectangle border for Object that triggered the ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_log_new('my-gstreamer-log-action')
```

<br>


### *dsl_ode_action_pause_new*
```C++
DslReturnType dsl_ode_action_pause_new(const wchar_t* name, const wchar_t* pipeline);
```
The constructor creates a uniquely named **Pause Pipeline** ODE Action. When invoked, this Action will pause a named Pipeline. The action will produce an error log message if the Pipeline does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `pipeline` - [in] the unique name for the Pipeline to pause on ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_pause_new('my-pause-action', 'my-pipeline')
```

<br>

### *dsl_ode_action_print_new*
```C++
DslReturnType dsl_ode_action_print_new(const wchar_t* name, boolean force_flush);
```
The constructor creates a uniquely named **Print** ODE Action. When invoked, this Action will print the Frame, Object and Trigger information that triggered the ODE occurrence to the console. The Print action can be very useful when setting-up/testing new ODE Triggers and Areas.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `force_flush` - [in] if set, the action will schedule a flush buffer operation to be performed by the idle thread.  

NOTE: although the flush event occurrs in the lowest priority background (idle) thread, flushing is still a CPU intensive operation and should be used sparingly -- when tailing the file for runtime debugging as an example. Set to 0 to disable forced flushing, and to allow the operating system to more effectively handle the process.


**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_print_new('my-print-action', false)
```

<br>

### *dsl_ode_action_redact_new*
```C++
DslReturnType dsl_ode_action_redact_new(const wchar_t* name);
```
The constructor creates a uniquely named **Redact Object** ODE Action. When invoked, this Action will update an Object's metadata by 
* filling in the the rectangle with a black background color with an alpha level of 1.0. 
* hiding both the display text and rectangle boarder

The action will Redact any detected object based on the GIE model and class Id in use... Face, License, etc,

Note: Adding a Redact ODE Action to an Absence or Summation Trigger is meaningless and will result in a Non-Action.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_redact_new('my-redact-action')
```

<br>

### *dsl_ode_action_sink_add_new*
```C++
DslReturnType dsl_ode_action_sink_add_new(const wchar_t* name, 
  const wchar_t* pipeline, const wchar_t* sink);
```
The constructor creates a uniquely named **Add Sink** ODE Action. When invoked, this Action will attempt to add a named Sink to a named Pipeline. The action will produce an error log message if either the Pipeline or Sink do not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `pipeline` - [in] the unique name for the Pipeline to add the Sink to on ODE occurrence
* `sink` - [in] the unique name for the Sink to add to the Pipeline on ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_sink_add_new('my-add-sink-action', 'my-pipeline', 'my-sink')
```
<br>

### *dsl_ode_action_sink_remove_new*
```C++
DslReturnType dsl_ode_action_sink_remove_new(const wchar_t* name, 
  const wchar_t* pipeline, const wchar_t* sink);
```
The constructor creates a uniquely named **Remove Sink** ODE Action. When invoked, this Action will remove a named Sink from a named Pipeline. The action will produce an error log message if either the Pipeline or Sink do not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `pipeline` - [in] the unique name for the Pipeline to remove the Sink from on ODE occurrence
* `sink` - [in] the unique name for the Sink to remove from the Pipeline on ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_sink_remove_new('my-remove-sink-action', 'my-pipeline', 'my-sink')
```

<br>

### *dsl_ode_action_sink_record_start_new*
```C++
DslReturnType dsl_ode_action_sink_record_start_new(const wchar_t* name,
    const wchar_t* record_sink, uint start, uint duration, void* client_data);
```
The constructor creates a uniquely named **Start Record Sink** ODE Action. When invoked, this Action will a Record session for the named Sink. The action will produce an error log message if the Sink does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `sink` - [in] the unique name of the Record Sink to start.
* `start` - [in] start time before current time in seconds, should be less the Record Sink's cache size.
* `duration` - [in] duration of the recording in seconds
* `client_data` - [in] opaque pointer to client data

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_sink_record_start_new('my-start-record-sink-action', 'my-record-sink', 15, 360, Null)
```

<br>

### *dsl_ode_action_source_add_new*
```C++
DslReturnType dsl_ode_action_source_add_new(const wchar_t* name, 
  const wchar_t* pipeline, const wchar_t* source);
```
The constructor creates a uniquely named **Add Source** ODE Action. When invoked, this Action will attempt to add a named Sink to a named Pipeline. The action will produce an error log message if either the Pipeline or Sink do not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `pipeline` - [in] the unique name for the Pipeline to add the Source to on ODE occurrence
* `sink` - [in] the unique name for the Source to add to the Pipeline on ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_source_add_new('my-add-source-action', 'my-pipeline', 'my-source')
```
<br>

### *dsl_ode_action_source_remove_new*
```C++
DslReturnType dsl_ode_action_source_remove_new(const wchar_t* name, 
  const wchar_t* pipeline, const wchar_t* source);
```
The constructor creates a uniquely named **Remove Source** ODE Action. When invoked, this Action will attempt to remove a named Source from a named Pipeline. The action will produce an error log message if either the Pipeline or Source do not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `pipeline` - [in] the unique name for the Pipeline to remove the Sink from on ODE occurrence
* `source` - [in] the unique name for the Sink to remove from the Pipeline on ODE occurrence

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_source_remove_new('my-remove-source-action', 'my-pipeline', 'my-source')
```

<br>

### *dsl_ode_action_tap_record_start_new*
```C++
DslReturnType dsl_ode_action_tap_record_start_new(const wchar_t* name,
    const wchar_t* record_sink, uint start, uint duration, void* client_data);
```
The constructor creates a uniquely named **Start Record Tap** ODE Action. When invoked, this Action will a Record session for the named Tap. The action will produce an error log message if the Tap does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `record_tap` - [in] the unique name of the Record Sink to start.
* `start` - [in] start time before current time in seconds, should be less the Record Sink's cache size.
* `duration` - [in] duration of the recording in seconds
* `client_data` - [in] opaque pointer to client data

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_tap_record_start_new('my-start-record-tap-action', 'my-record-tap', 15, 360, Null)
```

<br>

### *dsl_ode_action_tiler_source_show_new*
```C++
DslReturnType dsl_ode_action_tiler_source_show_new(const wchar_t* name, 
    const wchar_t* tiler, uint timeout, boolean has_precedence);
```
The constructor creates a uniquely named **Tiler Show Source** ODE Action. When invoked, this Action will call the named [Tiler](/docs/api-tiler.md) to show just the source (see [dsl_tiler_source_show_set](/docs/api-tiler.md#dsl_tiler_source_show_set) identified in the Frame's metadata that Triggered the ODE occurrence. The action will produce an error log message if the Tiler does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `tiler` - [in] the unique name of the Tiler to call on.
* `timeout` - [in] time to show the single source before switching back to "all sources" i.e. the tiled view. 
* `has_precedence` - [in] if true, will inform the Tiler that this source has precedence, even if another single source is being shown.

Note: setting `has_precedence` to true when using the action with a Trigger that has the `source_id` filter set to `DSL_ODE_ANY_SOURCE` (default) can lead to excessive switching. 

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_tiler_source_show_new('my-show-source-action', 'my-tiler', 20, False)
```

<br>

### *dsl_ode_action_trigger_disable_new*
```C++
DslReturnType dsl_ode_action_trigger_disable_new(const wchar_t* name, const wchar_t* trigger);
```
The constructor creates a uniquely named **Disable Trigger** ODE Action. When invoked, this Action will attempt to disable a named ODE Trigger. The Action will produce an error log message if the Trigger to disable does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to disable.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_trigger_disable_new('my-disable-trigger-action', 'my-trigger')
```

<br>

### *dsl_ode_action_trigger_enable_new*
```C++
DslReturnType dsl_ode_action_trigger_enable_new(const wchar_t* name, const wchar_t* trigger);
```
The constructor creates a uniquely named **Enable Trigger** ODE Action. When invoked, this Action will attempt to enable a named ODE Trigger. The Action will produce an error log message if the Trigger to enable does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to enable.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_trigger_enable_new('my-enable-trigger-action', 'my-trigger')
```

<br>

### *dsl_ode_action_trigger_reset_new*
```C++
DslReturnType dsl_ode_action_trigger_reset_new(const wchar_t* name, const wchar_t* trigger);
```
The constructor creates a uniquely named **Reset Trigger** ODE Action. When invoked, this Action will attempt to reset a named ODE Trigger. The Action will produce an error log message if the Trigger to reset does not exist at the time of invocation.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `trigger` - [in] unique name for the ODE Trigger to enable.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_trigger_reset_new('my-enable-trigger-action', 'my-trigger')
```

<br>

---

## Destructors
### *dsl_ode_action_delete*
```C++
DslReturnType dsl_ode_action_delete(const wchar_t* action);
```
This destructor deletes a single, uniquely named ODE Action. The destructor will fail if the Action is currently `in-use` by one or more ODE Triggers

**Parameters**
* `action` - [in] unique name for the ODE Action to delete

**Returns**
* `DSL_RESULT_SUCCESS` on successful deletion. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_ode_action_delete('my-action')
```

<br>

### *dsl_ode_action_delete_many*
```C++
DslReturnType dsl_action_delete_many(const wchar_t** actions);
```
This destructor deletes multiple uniquely named ODE Actions. Each name is checked for existence, with the function returning `DSL_RESULT_ACTION_NAME_NOT_FOUND` on first occurrence of failure. The destructor will fail if one of the Actions is currently `in-use` by one or more ODE Triggers

**Parameters**
* `actions` - [in] a NULL terminated array of uniquely named ODE Actions to delete.

**Returns**
* `DSL_RESULT_SUCCESS` on successful deletion. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_ode_action_delete_many(['my-action-a', 'my-action-b', 'my-action-c', None])
```

<br>

### *dsl_ode_action_delete_all*
```C++
DslReturnType dsl_ode_action_delete_all();
```
This destructor deletes all ODE Actions currently in memory. The destructor will fail if any one of the Actions is currently `in-use` by one or more ODE Triggers. 

**Returns**
* `DSL_RESULT_SUCCESS` on successful deletion. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_ode_action_delete_all()
```

<br>

---

## Methods
### *dsl_ode_action_capture_complete_listener_add*
```C++
DslReturnType dsl_ode_action_capture_complete_listener_add(const wchar_t* name, 
    dsl_capture_complete_listener_cb listener, void* client_data);
```
This service adds a callback function of type [dsl_capture_complete_listener_cb](#dsl_capture_complete_listener_cb) to a Capture Action identified by it's unique name. The function will be called on Image Capture completion with a [dsl_capture_info](#dsl_capture_info) structure and the client provided `client_data`. Multiple callback functions can be registered with one Actions, and one callback function can be registered with multiple Actions. 

**Parameters**
* `name` - [in] unique name of the Action to update.
* `listener` - [in] capture complete listener callback function to add.
* `client_data` - [in] opaque pointer to user data returned to the listner is called back

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_complete_listener_add('frame-capture-action', capture_complete_listener, None)
```

<br>

### *dsl_ode_action_capture_complete_listener_remove*
```C++
DslReturnType dsl_ode_action_capture_complete_listener_remove(const wchar_t* name, 
    dsl_capture_complete_listener_cb listener);
```
This service removes a callback function of type [dsl_capture_complete_listener_cb](#dsl_capture_complete_listener_cb) from a
pipeline identified by it's unique name, previously added with [dsl_ode_action_capture_complete_listener_add](#dsl_ode_action_capture_complete_listener_add)

**Parameters**
* `name` - [in] unique name of the Capture Action to update.
* `listener` - [in] capture complete listener callback function to remove.

**Returns**  
* `DSL_RESULT_SUCCESS` on successful removal. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_complete_listener_remove('frame-capture-action', capture_complete_listener,)
```

<br>

### *dsl_ode_action_capture_image_player_add*
```C++
DslReturnType dsl_ode_action_capture_image_player_add(const wchar_t* name, 
    const wchar_t* player);
```
This service adds an [Image Player](/docs/api-player.md) to a named Capture Action. Once added, each captured image's file_path will be added (or queued) with the Image Player to be played according to the Players settings. 

**Parameters**
* `name` - [in] unique name of the Action to update.
* `player` - [in] unique name of the Image Player to add.

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_image_player_add('frame-capture-action', 'image-player')
```

<br>

### *dsl_ode_action_capture_image_player_remove*
```C++
DslReturnType dsl_ode_action_capture_image_player_remove(const wchar_t* name, 
    const wchar_t* player);
```
This service removes an  [Image Player](/docs/api-player.md) from a named Capture Action previously added with [dsl_ode_action_capture_image_player_add](#dsl_ode_action_capture_image_player_add). 

**Parameters**
* `name` - [in] unique name of the Action to update.
* `player` - [in] unique name of the Image Player to remove.

**Returns**
* `DSL_RESULT_SUCCESS` on successful remove. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_image_player_remove('frame-capture-action', 'image-player')
```

<br>

### *dsl_ode_action_capture_mailer_add*
```C++
DslReturnType dsl_ode_action_capture_mailer_add(const wchar_t* name, 
    const wchar_t* mailer, const wchar_t* subject, boolean attach);
```
This service adds a [SMTP Mailer](/docs/api-mailer.md) to a named Capture Action. Once added, each captured image's file_path and details will be sent out according to the Mailer's settings. The image file can be attached to the email as an option.

**Parameters**
* `name` - [in] unique name of the Action to update.
* `player` - [in] unique name of the Image Player to add.
* `subject` - [in] subject line to use for all outgoing mail
* `attach` - [in] set to true to attach the image file, false otherwise

**Returns**
* `DSL_RESULT_SUCCESS` on successful add. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_mailer_add('frame-capture-action', 
    'mailer', 'Person has entered area', True)
```

<br>

### *dsl_ode_action_capture_mailer_remove*
```C++
DslReturnType dsl_ode_action_capture_mailer_remove(const wchar_t* name, 
    const wchar_t* mailer);
```
This service removes a [SMTP Mailer](/docs/api-mailer.md) from a named Capture Action previously added with [dsl_ode_action_capture_mailer_add](#dsl_ode_action_capture_mailer_add). 

**Parameters**
* `name` - [in] unique name of the Action to update.
* `player` - [in] unique name of the Image Player to remove.

**Returns**
* `DSL_RESULT_SUCCESS` on successful remove. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python
retval = dsl_ode_action_capture_mailer_remove('frame-capture-action', 'mailer')
```

<br>

### *dsl_ode_action_customize_label_get*
```C++
DslReturnType dsl_ode_action_customize_label_get(const wchar_t* name,  
    uint* content_types, uint* size);
```
The service queries a **Customize Label** ODE Action for its current label `content_types`.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `content_types` - [out] an output buffer for the service to write to.
* `size` - [inout] max size of the array as input, returned size as output .

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python    
retval, content_types, size = dsl_ode_action_display_get('my-customize-label-action')
```

<br>

### *dsl_ode_action_customize_label_set*
```C++
DslReturnType dsl_ode_action_customize_label_set(const wchar_t* name,  
    uint* content_types, uint* size);
```
The service updates a uniquely named **Customize Label** ODE Action with new label content types.

**Parameters**
* `name` - [in] unique name for the ODE Action to create.
* `content_types` - [in] an array of DSL_OBJECT_LABEL constants.
* `size` - [in] size of the content_types array.

**Returns**
* `DSL_RESULT_SUCCESS` on successful creation. One of the [Return Values](#return-values) defined above on failure.

**Python Example**
```Python    
retval = dsl_ode_action_display_set('my-customize-label-action', 
    [DSL_OBJECT_LABEL_TRACKING_ID, DSL_OBJECT_LABEL_PERSISTENCE], 2)
```

<br>

### *dsl_ode_action_enabled_get*
```c++
DslReturnType dsl_ode_action_enabled_get(const wchar_t* name, boolean* enabled);
```
This service returns the current enabled setting for the named ODE Action. Note: Actions are enabled by default at the time of construction.

**Parameters**
* `name` - [in] unique name of the ODE Action to query.
* `enabled` - [out] true if the ODE Action is currently enabled, false otherwise

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval, enabled = dsl_ode_action_enabled_get('my-action')
```

<br>

### *dsl_ode_action_enabled_set*
```c++
DslReturnType dsl_ode_action_enabled_set(const wchar_t* name, boolean enabled);
```
This service sets the enabled setting for the named ODE Action. Note: Actions are enabled by default at the time of construction.

**Parameters**
* `name` - [in] unique name of the ODE Action to update.
* `enabled` - [in] set to true to enable the ODE Action, false otherwise

**Returns**
* `DSL_RESULT_SUCCESS` on successful query. One of the [Return Values](#return-values) defined above on failure

**Python Example**
```Python
retval = dsl_ode_action_enabled_set('my-action', False)
```

<br>

### *dsl_ode_action_list_size*
```c++
uint dsl_ode_action_list_size();
```
This service returns the size of the ODE Action container, i.e. the number of Actions currently in memory. 

**Returns**
* The size of the ODE Action container

**Python Example**
```Python
size = dsl_ode_action_list_size()
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
* [Inference Engine and Server](/docs/api-infer.md)
* [Tracker](/docs/api-tracker.md)
* [Segmentation Visualizer](/docs/api-segvisual.md)
* [Tiler](/docs/api-tiler.md)
* [Demuxer and Splitter](/docs/api-tee.md)
* [On-Screen Display](/docs/api-osd.md)
* [Sink](/docs/api-sink.md)
* [Pad Probe Handler](/docs/api-pph.md)
* [ODE Trigger](/docs/api-ode-trigger.md)
* **ODE Action**
* [ODE Area](/docs/api-ode-area.md)
* [Display Type](/docs/api-display-type.md)
* [Branch](/docs/api-branch.md)
* [Component](/docs/api-component.md)
* [Mailer](/docs/api-mailer.md)
* [WebSocket Server](/docs/api-ws-server.md)
