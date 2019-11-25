/*
The MIT License

Copyright (c) 2019-Present, ROBERT HOWELL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _DSL_API_H
#define _DSL_API_H

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#define DSL_FALSE                                                   0
#define DSL_TRUE                                                    1

#define DSL_RESULT_SUCCESS                                          0x00000000
#define DSL_RESULT_API_NOT_IMPLEMENTED                              0x00000001

/**
 * Component API Return Values
 */
#define DSL_RESULT_COMPONENT_RESULT                                 0x00010000
#define DSL_RESULT_COMPONENT_NAME_NOT_UNIQUE                        0x00010001
#define DSL_RESULT_COMPONENT_NAME_NOT_FOUND                         0x00010010
#define DSL_RESULT_COMPONENT_NAME_BAD_FORMAT                        0x00010011
#define DSL_RESULT_COMPONENT_IN_USE                                 0x00010100
#define DSL_RESULT_COMPONENT_NOT_USED_BY_PIPELINE                   0x00010101

/**
 * Source API Return Values
 */
#define DSL_RESULT_SOURCE_RESULT                                    0x00100000
#define DSL_RESULT_SOURCE_NAME_NOT_UNIQUE                           0x00100001
#define DSL_RESULT_SOURCE_NAME_NOT_FOUND                            0x00100010
#define DSL_RESULT_SOURCE_NAME_BAD_FORMAT                           0x00100011
#define DSL_RESULT_SOURCE_THREW_EXCEPTION                           0x00100100
#define DSL_RESULT_SOURCE_STREAM_FILE_NOT_FOUND                     0x00100101
#define DSL_RESULT_SOURCE_NOT_IN_USE                                0x00100110
#define DSL_RESULT_SOURCE_NOT_IN_PLAY                               0x00100111
#define DSL_RESULT_SOURCE_NOT_IN_PAUSE                              0x00101000
#define DSL_RESULT_SOURCE_FAILED_TO_CHANGE_STATE                    0x00101001

/**
 * Sink Object Return Values
 */
#define DSL_RESULT_SINK_RESULT                                      0x01000000
#define DSL_RESULT_SINK_NAME_NOT_UNIQUE                             0x01000001
#define DSL_RESULT_SINK_NAME_NOT_FOUND                              0x01000010
#define DSL_RESULT_SINK_NAME_BAD_FORMAT                             0x01000011
#define DSL_RESULT_SINK_THREW_EXCEPTION                             0x01000100

/**
 * OSD Object Return Values
 */
#define DSL_RESULT_OSD_RESULT                                       0x01010000
#define DSL_RESULT_OSD_NAME_NOT_UNIQUE                              0x01010001
#define DSL_RESULT_OSD_NAME_NOT_FOUND                               0x01010010
#define DSL_RESULT_OSD_NAME_BAD_FORMAT                              0x01010011
#define DSL_RESULT_OSD_THREW_EXCEPTION                              0x01010100

/**
 * GIE Object Return Values
 */
#define DSL_RESULT_GIE_RESULT                                       0x01100000
#define DSL_RESULT_GIE_NAME_NOT_UNIQUE                              0x01100001
#define DSL_RESULT_GIE_NAME_NOT_FOUND                               0x01100010
#define DSL_RESULT_GIE_NAME_BAD_FORMAT                              0x01100011
#define DSL_RESULT_GIE_CONFIG_FILE_NOT_FOUND                        0x01100100
#define DSL_RESULT_GIE_MODEL_FILE_NOT_FOUND                         0x01100100
#define DSL_RESULT_GIE_THREW_EXCEPTION                              0x01100100

/**
 * Display Object Return Values
 */
#define DSL_RESULT_DISPLAY_RESULT                                   0x10000000
#define DSL_RESULT_DISPLAY_NAME_NOT_UNIQUE                          0x10000001
#define DSL_RESULT_DISPLAY_NAME_NOT_FOUND                           0x10000010
#define DSL_RESULT_DISPLAY_NAME_BAD_FORMAT                          0x10000011
#define DSL_RESULT_DISPLAY_THREW_EXCEPTION                          0x10000100
#define DSL_RESULT_DISPLAY_IS_IN_USE                                0x10000101
#define DSL_RESULT_DISPLAY_GET_FAILED                               0x10000110
#define DSL_RESULT_DISPLAY_SET_FAILED                               0x10000111

/**
 * Pipeline Object Return Values
 */
#define DSL_RESULT_PIPELINE_RESULT                                  0x11000000
#define DSL_RESULT_PIPELINE_NAME_NOT_UNIQUE                         0x11000001
#define DSL_RESULT_PIPELINE_NAME_NOT_FOUND                          0x11000010
#define DSL_RESULT_PIPELINE_NAME_BAD_FORMAT                         0x11000011
#define DSL_RESULT_PIPELINE_STATE_PAUSED                            0x11000100
#define DSL_RESULT_PIPELINE_STATE_RUNNING                           0x11000101
#define DSL_RESULT_PIPELINE_THREW_EXCEPTION                         0x11000110
#define DSL_RESULT_PIPELINE_COMPONENT_ADD_FAILED                    0x11000111
#define DSL_RESULT_PIPELINE_COMPONENT_REMOVE_FAILED                 0x11001000
#define DSL_RESULT_PIPELINE_STREAMMUX_GET_FAILED                    0x11001001
#define DSL_RESULT_PIPELINE_STREAMMUX_SET_FAILED                    0x11001010
#define DSL_RESULT_PIPELINE_FAILED_TO_PLAY                          0x11001011
#define DSL_RESULT_PIPELINE_FAILED_TO_PAUSE                         0x11001100
#define DSL_RESULT_PIPELINE_FAILED_TO_STOP                          0x11001101
#define DSL_RESULT_PIPELINE_LISTENER_NOT_UNIQUE                     0x11001110
#define DSL_RESULT_PIPELINE_LISTENER_NOT_FOUND                      0x11001111
#define DSL_RESULT_PIPELINE_HANDLER_NOT_UNIQUE                      0x11010000
#define DSL_RESULT_PIPELINE_HANDLER_NOT_FOUND                       0x11010001
#define DSL_RESULT_PIPELINE_SUBSCRIBER_NOT_UNIQUE                   0x11010010
#define DSL_RESULT_PIPELINE_SUBSCRIBER_NOT_FOUND                    0x11010011

#define DSL_CUDADEC_MEMTYPE_DEVICE                                  0
#define DSL_CUDADEC_MEMTYPE_PINNED                                  1
#define DSL_CUDADEC_MEMTYPE_UNIFIED                                 2

#define DSL_STATE_NULL                                              0
#define DSL_STATE_READY                                             1
#define DSL_STATE_PLAYING                                           2
#define DSL_STATE_PAUSED                                            4
#define DSL_STATE_IN_TRANSITION                                     5

/**
 * @brief DSL_DEFAULT values initialized on first call to DSL
 */
//TODO move to new defaults schema
#define DSL_DEFAULT_SOURCE_IN_USE_MAX                               8 
//#define DSL_DEFAULT_STREAMMUX_WIDTH                                 1280
//#define DSL_DEFAULT_STREAMMUX_HEIGHT                                720
#define DSL_DEFAULT_STREAMMUX_WIDTH                                 1920
#define DSL_DEFAULT_STREAMMUX_HEIGHT                                1080

typedef uint DslReturnType;
typedef uint boolean;


EXTERN_C_BEGIN

/**
 * @brief creates a new, uniquely named CSI Camera Source obj
 * @param[in] name unique name for the new Source
 * @param[in] width width of the source in pixels
 * @param[in] height height of the source in pixels
 * @param[in] fps-n frames/second fraction numerator
 * @param[in] fps-d frames/second fraction denominator
 * @return DSL_RESULT_SOURCE_RESULT
 */
DslReturnType dsl_source_csi_new(const wchar_t* name,
    uint width, uint height, uint fps_n, uint fps_d);

/**
 * @brief creates a new, uniquely named URI Source obj
 * @param[in] name Unique Resource Identifier (file or live)
 * @param[in] cudadec_mem_type, use DSL_CUDADEC_MEMORY_TYPE_<type>
 * @param[in] 
 * @return DSL_RESULT_SOURCE_RESULT
 */
DslReturnType dsl_source_uri_new(const wchar_t* name, 
    const wchar_t* uri, uint cudadec_mem_type, uint intra_decode, uint drop_frame_interval);

/**
 * @brief pauses a single Source object if the Source is 
 * currently in a state of in-use and Playing..
 * @param name the name of Source component to pause
 * @return DSL_RESULT_SOURCE_RESULT
 */
DslReturnType dsl_source_pause(const wchar_t* name);

/**
 * @brief resumes a single Source object if the Source is 
 * currently in a state of in-use and Paused..
 * @param name the name of Source component to resume
 * @return DSL_RESULT_SOURCE_RESULT
 */
DslReturnType dsl_source_resume(const wchar_t* name);

/**
 * @brief returns whether the source stream is live or not
 * @param name the name of Source component to query
 * @return True if the source's stream is live
 */
boolean dsl_source_is_live(const wchar_t* name);

/**
 * @brief returns the number of sources currently in use by 
 * all of the Pipelines in memeroy. 
 * @return number of Sources in use
 */
uint dsl_source_get_num_in_use();  

/**
 * @brief Get the maximum number of in-memory sources 
 * that can be in use at any time. The maximum number is 
 * limited by Hardware, see dsl_source_set_num_in_use_max() 
 * @return the current max sources in use setting.
 */
uint dsl_source_get_num_in_use_max();  

/**
 * @brief Sets the maximum number of in-memory sources 
 * that can be in use at any time. The function overrides 
 * the default value on first call. The maximum number is 
 * limited by Hardware. The caller must ensure to set the 
 * number correctly, based on the TEGRA platform in use.
 */
void dsl_source_set_num_in_use_max(uint max);  

/**
 * @brief creates a new, uniquely named Sink obj
 * @param[in] sink unique name for the new Sink
 * @param[in] displayId
 * @param[in] overlatId
 * @param[in] offsetX
 * @param[in] offsetY
 * @param[in] width width of the Sink
 * @param[in] heigth height of the Sink
 * @return DSL_RESULT_SINK_RESULT
 */
DslReturnType dsl_sink_overlay_new(const wchar_t* name, 
    uint offsetX, uint offsetY, uint width, uint height);

/**
 * @brief creates a new, uniquely named OSD obj
 * @param[in] name unique name for the new Sink
 * @param[in] is_clock_enabled true if clock is visible
 * @return DSL_RESULT_SINK_RESULT
 */
DslReturnType dsl_osd_new(const wchar_t* name, boolean is_clock_enabled);

/**
 * @brief creates a new, uniquely named GIE object
 * @param[in] name unique name for the new GIE object
 * @param[in] infer_config_file name of the Infer Config file to use
 * @param[in] model_engine_file name of the Model Engine file to use
 * @param[in] interval
 * @param[in] uniqueId
 * @return DSL_RESULT_GIE_RESULT
 */
DslReturnType dsl_gie_primary_new(const wchar_t* name, const wchar_t* infer_config_file,
    const wchar_t* model_engine_file, uint interval, uint unique_id);

/**
 * @brief creates a new, uniquely named Display obj
 * @param[in] name unique name for the new Display
 * @param[in] width width of the Display in pixels
 * @param[in] height height of the Display in pixels
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_display_new(const wchar_t* name, uint width, uint height);

/**
 * @brief returns the dimensions, width and height, for the named Tiled Display
 * @param[in] name name of the Display to query
 * @param[out] width current width of the display in pixels
 * @param[out] height current height of the display in pixels
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_display_dimensions_get(const wchar_t* name, uint* width, uint* height);

/**
 * @brief sets the dimensions, width and height, for the named Tiled Display
 * @param[in] name name of the Display to update
 * @param[in] width width to set the display in pixels
 * @param[in] height height to set the display in pixels
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_display_dimensions_set(const wchar_t* name, uint width, uint height);

/**
 * @brief returns the number of columns and rows for the named Tiled Display
 * @param[in] name name of the Display to query
 * @param[out] cols current number of colums for all Tiles
 * @param[out] rows current number of rows for all Tiles
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_display_tiles_get(const wchar_t* name, uint* cols, uint* rows);

/**
 * @brief Sets the number of columns and rows for the named Tiled Display
 * @param[in] name name of the Display to update
 * @param[in] cols current number of colums for all Tiles
 * @param[in] rows current number of rows for all Tiles
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_display_tiles_set(const wchar_t* name, uint cols, uint rows);

/**
 * @brief deletes a Component object by name
 * @param[in] component name of the Component object to delete
 * @return DSL_RESULT_COMPONENT_RESULT
 * @info the function checks that the component is not 
 * owned by a pipeline before deleting, and returns
 * DSL_RESULT_COMPONENT_IN_USE on failure
 */
DslReturnType dsl_component_delete(const wchar_t* component);

/**
 * @brief deletes a NULL terminated list of components
 * @param components NULL terminated list of names to delete
 * @return DSL_RESULT_COMPONENT_RESULT
 * @info the function ensures the existance of all components
 * in the list before making any updates, and returns
 * DSL_RESULT_COMPONENT_NAME_NOT_FOUND on failure without
 * making updates to the component list
 * @info the function checks that all components are not 
 * owned by a pipeline before deleting, and returns
 * DSL_RESULT_COMPONENT_IN_USE on failure
 */
DslReturnType dsl_component_delete_many(const wchar_t** components);

/**
 * @brief deletes all components in memory
 * @return DSL_RESULT_COMPONENT_RESULT
 * @info the function checks that all components are not 
 * owned by a pipeline before deleting, and returns
 * DSL_RESULT_COMPONENT_IN_USE on failure
 */
DslReturnType dsl_component_delete_all();

/**
 * @brief returns the current number of components
 * @return size of the list of components
 */
uint dsl_component_list_size();

/**
 * @brief returns the list of components
 * @return a NULL terminated array of char* component names
 */
const wchar_t** dsl_component_list_all();

/**
 * @brief creates a new, uniquely named Pipeline
 * @param[in] pipeline unique name for the new Pipeline
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_new(const wchar_t* pipeline);

/**
 * @brief creates a new Pipeline for each name pipelines array
 * @param pipelines a NULL terminated array of unique Pipeline names
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_new_many(const wchar_t** pipelines);

/**
 * @brief deletes a Pipeline object by name.
 * @param[in] pipeline unique name of the Pipeline to delete.
 * @return DSL_RESULT_PIPELINE_RESULT.
 * @info any/all components owned by the pipeline move
 * to a state of not-in-use.
 */
DslReturnType dsl_pipeline_delete(const wchar_t* pipeline);

/**
 * @brief deletes a NULL terminated list of pipelines
 * @param pipelines NULL terminated list of names to delete
 * @return DSL_RESULT_PIPELINE_RESULT
 * @info the function ensures the existance of all pipelines
 * in the list before making any updates, and returns
 * DSL_RESULT_PIPELINE_NAME_NOT_FOUND on failure without
 * making updates to the pipeline list
 * @info any/all components owned by the pipelines move
 * to a state of not-in-use.
 */
DslReturnType dsl_pipeline_delete_many(const wchar_t** pipelines);

/**
 * @brief deletes all pipelines in memory
 * @return DSL_RESULT_COMPONENT_RESULT
 * @info any/all components owned by the pipelines move
 * to a state of not-in-use.
 */
DslReturnType dsl_pipeline_delete_all();

/**
 * @brief returns the current number of pipelines
 * @return size of the list of pipelines
 */
uint dsl_pipeline_list_size();

/**
 * @brief returns the list of pipelines
 * @return a NULL terminated array of char* pipeline names
 */
const wchar_t** dsl_pipeline_list_all();

/**
 * @brief adds a single components to a Pipeline 
 * @param[in] pipeline name of the pipepline to update
 * @param[in] components NULL terminated array of component names to add
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_component_add(const wchar_t* pipeline, 
    const wchar_t* component);

/**
 * @brief adds a list of components to a Pipeline 
 * @param[in] name name of the pipepline to update
 * @param[in] components NULL terminated array of component names to add
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_component_add_many(const wchar_t* pipeline, 
    const wchar_t** components);

/**
 * @brief removes a Component from a Pipeline
 * @param[in] pipeline name of the Pipepline to update
 * @param[in] component name of the Component to remove
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_component_remove(const wchar_t* pipeline, 
    const wchar_t* component);

/**
 * @brief removes a list of Components from a Pipeline
 * @param[in] pipeline name of the Pipeline to update
 * @param[in] components NULL terminated array of component names to remove
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_component_remove_many(const wchar_t* pipeline, 
    const wchar_t** components);

/**
 * @brief 
 * @param[in] pipeline name of the pipepline to query
 * @return 
 */
DslReturnType dsl_pipeline_streammux_batch_properties_get(const wchar_t* pipeline, 
    uint* batchSize, uint* batchTimeout);

/**
 * @brief 
 * @param[in] pipeline name of the pipepline to update
 * @return 
 */
DslReturnType dsl_pipeline_streammux_batch_properties_set(const wchar_t* pipeline, 
    uint batchSize, uint batchTimeout);

/**
 * @brief 
 * @param[in] pipeline name of the pipepline to query
 * @return 
 */
DslReturnType dsl_pipeline_streammux_dimensions_get(const wchar_t* pipeline, 
    uint width, uint height);

/**
 * @brief 
 * @param[in] pipeline name of the pipepline to update
 * @return 
 */
DslReturnType dsl_pipeline_streammux_dimensions_set(const wchar_t* pipeline, 
    uint width, uint height);

/**
 * @brief returns the current setting, enabled/disabled, for the fixed-aspect-ratio 
 * attribute for the named Tiled Display
 * @param[in] name name of the Display to query
 * @param[out] enable true if the aspect ration is fixed, false if not
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_pipeline_streammux_padding_get(const wchar_t* name, boolean* enabled);

/**
 * @brief updates the current setting - enabled/disabled - for the fixed-aspect-ratio 
 * attribute for the named Tiled Display
 * @param[in] name name of the Display to update
 * @param[out] enable set true to fix the aspect ratio, false to disable
 * @return DSL_RESULT_DISPLAY_RESULT
 */
DslReturnType dsl_pipeline_streammux_padding_set(const wchar_t* name, boolean enabled);


/**
 * @brief pauses a Pipeline if in a state of playing
 * @param[in] pipeline unique name of the Pipeline to pause.
 * @return DSL_RESULT.
 */
DslReturnType dsl_pipeline_pause(const wchar_t* pipeline);

/**
 * @brief plays a Pipeline if in a state of paused
 * @param[in] pipeline unique name of the Pipeline to play.
 * @return DSL_RESULT_PIPELINE_RESULT.
 */
DslReturnType dsl_pipeline_play(const wchar_t* pipeline);

/**
 * @brief Stops a Pipeline if in a state of paused or playing
 * @param[in] pipeline unique name of the Pipeline to stop.
 * @return DSL_RESULT_PIPELINE_RESULT.
 */
DslReturnType dsl_pipeline_stop(const wchar_t* pipeline);

/**
 * @brief gets the current state of a Pipeline
 * @param[in] pipeline unique name of the Pipeline to query
 * @return DSL_RESULT_PIPELINE_PAUSED | DSL_RESULT_PIPELINE_PLAYING
 */
DslReturnType dsl_pipeline_get_state(const wchar_t* pipeline);

/**
 * @brief dumps a Pipeline's graph to dot file.
 * @param[in] pipeline unique name of the Pipeline to dump
 * @param[in] filename name of the file without extention.
 * The caller is responsible for providing a correctly formated filename
 * The diretory location is specified by the GStreamer debug 
 * environment variable GST_DEBUG_DUMP_DOT_DIR
 */ 
DslReturnType dsl_pipeline_dump_to_dot(const wchar_t* pipeline, wchar_t* filename);

/**
 * @brief dumps a Pipeline's graph to dot file prefixed
 * with the current timestamp.  
 * @param[in] pipeline unique name of the Pipeline to dump
 * @param[in] filename name of the file without extention.
 * The caller is responsible for providing a correctly formated filename
 * The diretory location is specified by the GStreamer debug 
 * environment variable GST_DEBUG_DUMP_DOT_DIR
 */ 
DslReturnType dsl_pipeline_dump_to_dot_with_ts(const wchar_t* pipeline, wchar_t* filename);

/**
 * @brief callback typedef for a client listener function. Once added to a Pipeline, 
 * the function will be called when the Pipeline changes state.
 * @param[in] prev_state one of DSL_PIPELINE_STATE constants for the previous pipeline state
 * @param[in] curr_state one of DSL_PIPELINE_STATE constants for the current pipeline state
 * @param[in] user_data opaque pointer to client's data
 */
typedef void (*dsl_state_change_listener_cb)(uint prev_state, uint curr_state, void* user_data);

/**
 * @brief adds a callback to be notified on change of Pipeline state
 * @param[in] pipeline name of the pipepline to update
 * @param[in] listener pointer to the client's function to call on state change
 * @param[in] userdata opaque pointer to client data passed into the listner function.
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_state_change_listener_add(const wchar_t* pipeline, 
    dsl_state_change_listener_cb listener, void* userdata);

/**
 * @brief removes a callback previously added with dsl_pipeline_state_change_listener_add
 * @param[in] pipeline name of the pipepline to update
 * @param[in] listener pointer to the client's function to remove
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_state_change_listener_remove(const wchar_t* pipeline, 
    dsl_state_change_listener_cb listener);

/**
 * @brief callback typedef for a client event handler function. Once added to a Pipeline, 
 * the function will be called when the Pipeline receives window events for the Tiled Display.
 * @param[in] 
 * @param[in] 
 * @param[in] user_data opaque pointer to client's data
 */
typedef void (*dsl_display_event_handler_cb)(uint prev_state, uint curr_state, void* user_data);

/**
 * @brief adds a callback to be notified on display/window event [ButtonPress|KeyRelease]
 * @param[in] pipeline name of the pipepline to update
 * @param[in] handler pointer to the client's function to call to handle window events.
 * @param[in] user_data opaque pointer to client data passed into the listner function.
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_display_event_handler_add(const wchar_t* pipeline, 
    dsl_display_event_handler_cb handler, void* user_data);

/**
 * @brief removes a callback previously added with dsl_display_event_handler_add
 * @param[in] pipeline name of the pipepline to update
 * @param[in] handler pointer to the client's function to remove
 * @return DSL_RESULT_PIPELINE_RESULT
 */
DslReturnType dsl_pipeline_display_event_handler_remove(const wchar_t* pipeline, 
    dsl_display_event_handler_cb handler);

/**
 * @brief entry point to the GST Main Loop
 * Note: This is a blocking call - executes an endless loop
 */
void dsl_main_loop_run();

EXTERN_C_END

#endif /* _DSL_API_H */
