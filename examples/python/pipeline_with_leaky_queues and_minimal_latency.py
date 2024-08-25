################################################################################
# The MIT License
#
# Copyright (c) 2019-2024, Prominence AI, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

################################################################################
#
# The simple example demonstrates how to create a set of Pipeline components, 
# specifically:
#   - URI Source
#   - Preprocessor
#   - Primary GIE
#   - IOU Tracker
#   - On-Screen Display
#   - Window Sink
# ...and how to add them to a new Pipeline and play
#
# Specific services must be called for the PGIE to be able to receive tensor-meta
# buffers from the Preprocessor component.
# 
# The example registers handler callback functions with the Pipeline for:
#   - key-release events
#   - delete-window events
#   - end-of-stream EOS events
#   - Pipeline change-of-state events
#  
################################################################################

#!/usr/bin/env python

import sys
from dsl import *

# Unique name for each of the Pipeline components
URI_SOURCE_NAME = 'uri-source'
PREPROC_NAME = 'preprocessor'
PRIMARY_GIE_NAME = 'primary-gie'
IOU_TRACKER = 'iou-tracker'
OSD_NAME = 'on-screen-display'
WINDOW_SINK_NAME = 'window-sink'

# Null terminated list of all Pipeline Component names - will be used when 
# setting Queue properties collectively. eg. 
#     retval = dsl_component_queue_leaky_set_many(COMPONENTS, 
#         DSL_COMPONENT_QUEUE_LEAKY_DOWNSTREAM)

COMPONENTS = [URI_SOURCE_NAME, PREPROC_NAME, PRIMARY_GIE_NAME, 
    IOU_TRACKER, OSD_NAME, WINDOW_SINK_NAME, None]

# File URI for our source.
uri_file = "/opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h265.mp4"

# Config file used with the Preprocessor
preproc_config_file = \
    '/opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-preprocess-test/config_preprocess.txt'
    
# Filespecs for the Primary GIE and IOU Trcaker
primary_infer_config_file = \
    '/opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-preprocess-test/config_infer.txt'

# IMPORTANT! ensure that the model-engine was generated with the config from the Preprocessing example
#  - apps/sample_apps/deepstream-preprocess-test/config_infer.txt
primary_model_engine_file = \
    '/opt/nvidia/deepstream/deepstream/samples/models/Primary_Detector/resnet18_trafficcamnet.etlt_b8_gpu0_int8.engine'
    
    
# Filespec for the IOU Tracker config file
iou_tracker_config_file = \
    '/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_IOU.yml'

WINDOW_WIDTH = 1280
WINDOW_HEIGHT = 720

## 
# Function to be called on XWindow KeyRelease event
## 
def xwindow_key_event_handler(key_string, client_data):
    print('key released = ', key_string)
    if key_string.upper() == 'P':
        dsl_pipeline_pause('pipeline')
    elif key_string.upper() == 'R':
        dsl_pipeline_play('pipeline')
    elif key_string.upper() == 'Q' or key_string == '' or key_string == '':
        dsl_pipeline_stop('pipeline')
        dsl_main_loop_quit()
 
## 
# Function to be called on XWindow Delete event
## 
def xwindow_delete_event_handler(client_data):
    print('delete window event')
    dsl_pipeline_stop('pipeline')
    dsl_main_loop_quit()

## 
# Function to be called on End-of-Stream (EOS) event
## 
def eos_event_listener(client_data):
    print('Pipeline EOS event')
    dsl_pipeline_stop('pipeline')
    dsl_main_loop_quit()

## 
# Function to be called on every change of Pipeline state
## 
def state_change_listener(old_state, new_state, client_data):
    print('previous state = ', old_state, ', new state = ', new_state)
    if new_state == DSL_STATE_PLAYING:
        dsl_pipeline_dump_to_dot('pipeline', "state-playing")

def main(args):

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:
        # New URI File Source using the filespec defined above
        retval = dsl_source_uri_new(URI_SOURCE_NAME, uri_file, False, False, 0)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Preprocessor component using the config filespec defined above.
        retval = dsl_preproc_new(PREPROC_NAME, preproc_config_file)
        if retval != DSL_RETURN_SUCCESS:
            break
        
        # New Primary GIE using the filespecs above with interval = 0
        retval = dsl_infer_gie_primary_new(PRIMARY_GIE_NAME, 
            primary_infer_config_file, primary_model_engine_file, 0)
        if retval != DSL_RETURN_SUCCESS:
            break

        # **** IMPORTANT! for best performace we explicity set the GIE's batch-size 
        # to the number of ROI's defined in the Preprocessor configuraton file.
        retval = dsl_infer_batch_size_set(PRIMARY_GIE_NAME, 2)
        if retval != DSL_RETURN_SUCCESS:
            break

        # **** IMPORTANT! we must set the input-meta-tensor setting to true when
        # using the preprocessor, otherwise the GIE will use its own preprocessor.
        retval = dsl_infer_gie_tensor_meta_settings_set(PRIMARY_GIE_NAME,
            input_enabled=True, output_enabled=False);

        # New IOU Tracker, setting operational frame width and height.
        retval = dsl_tracker_new(IOU_TRACKER, iou_tracker_config_file, 480, 272)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New OSD with text, clock and bbox display all enabled. 
        retval = dsl_osd_new(OSD_NAME, 
            text_enabled=True, clock_enabled=True, 
            bbox_enabled=True, mask_enabled=False)
        if retval != DSL_RETURN_SUCCESS:
            break

        # New Window Sink with x/y offsets and dimensions.
        # EGL Sink runs on both platforms. 3D Sink is Jetson only.
        if (dsl_info_gpu_type_get(0) == DSL_GPU_TYPE_INTEGRATED):
            retval = dsl_sink_window_3d_new(WINDOW_SINK_NAME, 0, 0, 
                WINDOW_WIDTH, WINDOW_HEIGHT)
        else:
            retval = dsl_sink_window_egl_new(WINDOW_SINK_NAME, 0, 0, 
                WINDOW_WIDTH, WINDOW_HEIGHT)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add the XWindow event handler functions defined above
        retval = dsl_sink_window_key_event_handler_add(WINDOW_SINK_NAME, 
            xwindow_key_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_sink_window_delete_event_handler_add(WINDOW_SINK_NAME, 
            xwindow_delete_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to our pipeline
        retval = dsl_pipeline_new_component_add_many('pipeline', COMPONENTS)
        if retval != DSL_RETURN_SUCCESS:
            break

        ## Add the listener callback functions defined above
        retval = dsl_pipeline_state_change_listener_add('pipeline', 
            state_change_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_eos_listener_add('pipeline', 
            eos_event_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # ---------------------------------------------------------------------------
        # Component Queue settings to minimze latency

        retval = dsl_component_queue_leaky_set(URI_SOURCE_NAME, 
            DSL_COMPONENT_QUEUE_LEAKY_DOWNSTREAM)        
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_component_queue_leaky_set_many(COMPONENTS, 
            DSL_COMPONENT_QUEUE_LEAKY_DOWNSTREAM)        
        if retval != DSL_RETURN_SUCCESS:
            break

        retval = dsl_component_queue_max_size_set(URI_SOURCE_NAME, 
            DSL_COMPONENT_QUEUE_UNIT_OF_BUFFERS, 9)        
        if retval != DSL_RETURN_SUCCESS:
            break

        # ---------------------------------------------------------------------------
        # Play the pipeline
        retval = dsl_pipeline_play('pipeline')
        if retval != DSL_RETURN_SUCCESS:
            break

        dsl_main_loop_run()
        retval = DSL_RETURN_SUCCESS
        break

    # Print out the final result
    print(dsl_return_value_to_string(retval))

    dsl_pipeline_delete_all()
    dsl_component_delete_all()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
