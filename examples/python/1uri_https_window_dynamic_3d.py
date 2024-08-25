##############################################################################
# The MIT License
#
# Copyright (c) 2019-2023, Prominence AI, Inc.
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

#!/usr/bin/env python

import sys
import time

from dsl import *

source_uri = 'http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/ElephantsDream.mp4'

# Filespecs for the Primary GIE and IOU Trcaker
primary_infer_config_file = \
    '/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_infer_primary.txt'
primary_model_engine_file = \
    '/opt/nvidia/deepstream/deepstream/samples/models/Primary_Detector/resnet18_trafficcamnet.etlt_b8_gpu0_int8.engine'


MAX_3D_SINK_COUNT = 2 # hardware limited
cur_3d_sink_count = 0

buffering = False

## 
# Function to be called on when a buffering-message is recieved on the Pipeline bus.
## 
def buffering_message_handler(source, percent, client_data):

    global buffering

    if percent == 100:
        print('playing pipeline - buffering complete at 100 %')
        dsl_pipeline_play('pipeline')
        buffering = False

    else:
        if not buffering:
            print('pausing pipeline - buffering starting at ', percent, '%')
            dsl_pipeline_pause('pipeline')
        buffering = True

## 
# Function to be called on XWindow KeyRelease event
## 
def xwindow_key_event_handler(key_string, client_data):

    global MAX_3D_SINK_COUNT, cur_3d_sink_count

    print('key released = ', key_string)
    if key_string.upper() == 'P':
        dsl_pipeline_pause('pipeline')
    elif key_string.upper() == 'R':
        dsl_pipeline_play('pipeline')
    elif key_string.upper() == 'Q' or key_string == '' or key_string == '':
        dsl_pipeline_stop('pipeline')
        dsl_main_loop_quit()

    # Add a new 3D Sink
    elif key_string == '+': 
        if cur_3d_sink_count < MAX_3D_SINK_COUNT:
            cur_3d_sink_count += 1
            sink_name = '3d-sink-' + str(cur_3d_sink_count)
            print('adding sink ', sink_name)
            dsl_sink_window_3d_new(sink_name, 100*cur_3d_sink_count, 
                100*cur_3d_sink_count, 360, 180)
            dsl_sink_sync_enabled_set(sink_name, False)
            dsl_pipeline_component_add('pipeline', sink_name)

    # Remove the last sink added
    elif key_string == '-': 
        if cur_3d_sink_count > 0:
            sink_name = '3d-sink-' + str(cur_3d_sink_count)
            print('removing sink ', sink_name)
            dsl_pipeline_component_remove('pipeline', sink_name)
            dsl_component_delete(sink_name)
            cur_3d_sink_count -= 1
 
## 
# Function to be called on XWindow Delete event
## 
def xwindow_delete_event_handler(client_data):
    print('delete window event')
    dsl_pipeline_stop('pipeline')
    dsl_main_loop_quit()

# Function to be called on End-of-Stream (EOS) event
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

    print('*******************************************************')
    print(' Press + to add new 3D Sink')
    print(' Press - to remove last added 3D Sink')
    print('*******************************************************')

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:

        ## First new URI File Source
        retval = dsl_source_uri_new('uri-source', source_uri, False, False, 0)
        if retval != DSL_RETURN_SUCCESS:
            break

        ## New Window Sink, same dimensions as tiler
        retval = dsl_sink_window_egl_new('egl-sink', 0, 0, 1280, 720)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add the XWindow event handler functions defined above
        retval = dsl_sink_window_key_event_handler_add('egl-sink', 
            xwindow_key_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_sink_window_delete_event_handler_add('egl-sink', 
            xwindow_delete_event_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Add all the components to a new pipeline
        retval = dsl_pipeline_new_component_add_many('pipeline', 
            ['uri-source', 'egl-sink', None])
        if retval != DSL_RETURN_SUCCESS:
            break

        ## Add the Pipeline callback functions defined above
        retval = dsl_pipeline_buffering_message_handler_add('pipeline',
            buffering_message_handler, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_state_change_listener_add('pipeline', 
            state_change_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break
        retval = dsl_pipeline_eos_listener_add('pipeline', 
            eos_event_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            break

        # Play the pipeline
        retval = dsl_pipeline_play('pipeline')
        if retval != DSL_RETURN_SUCCESS:
            break

        # Join with main loop until released - blocking call
        dsl_main_loop_run()
        retval = DSL_RETURN_SUCCESS
        break

    # Print out the final result
    print(dsl_return_value_to_string(retval))

    dsl_pipeline_delete_all()
    dsl_component_delete_all()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
