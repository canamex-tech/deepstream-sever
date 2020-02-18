import sys
sys.path.insert(0, "../../")
import time

from dsl import *

# Filespecs for the Primary GIE
inferConfigFile = '../../test/configs/config_infer_primary_nano.txt'
modelEngineFile = '../../test/models/Primary_Detector_Nano/resnet10.caffemodel_b4_fp16.engine'

# Function to be called on End-of-Stream (EOS) event
def eos_event_listener(client_data):
    print('Pipeline EOS event')
    dsl_main_loop_quit()

def main(args):

    # Since we're not using args, we can Let DSL initialize GST on first call
    while True:

        # New URI File Source
        retval = dsl_source_uri_new('uri-source-1', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 1)

        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break
        dsl_source_uri_new('uri-source-2', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 1)
        dsl_source_uri_new('uri-source-3', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 1)
        dsl_source_uri_new('uri-source-4', "../../test/streams/sample_1080p_h264.mp4", False, 0, 0, 1)

        # New Primary GIE using the filespecs above, with interval and Id
        retval = dsl_gie_primary_new('primary-gie', inferConfigFile, modelEngineFile, 1)

        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # New Tiler, setting width and height, use default cols/rows set by source count
        retval = dsl_tiler_new('tiler', 1280, 720)

        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # New OSD with clock enabled... using default values.
        retval = dsl_osd_new('on-screen-display', False)
        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # New Overlay Sink, 0 x/y offsets and same dimensions as Tiled Display
        retval = dsl_sink_overlay_new('overlay-sink', -1, 0, 0, 0, 0, 1280, 720)
        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # New Pipeline to use with the above components
        retval = dsl_pipeline_new('simple-pipeline')
        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # New Pipeline to use with the above components
        retval = dsl_pipeline_eos_listener_add('simple-pipeline', eos_event_listener, None)
        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # Add all the components to our pipeline
        retval = dsl_pipeline_component_add_many('simple-pipeline', 
            ['uri-source-1', 'uri-source-2', 'uri-source-3', 'uri-source-4', 
            'primary-gie', 'tiler', 'on-screen-display', 'overlay-sink', None])

        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # Play the pipeline
        retval = dsl_pipeline_play('simple-pipeline')

        if retval != DSL_RETURN_SUCCESS:
            print(retval)
            break

        # Once playing, we can dump the pipeline graph to dot file, which can be converted to an image file for viewing/debugging
        dsl_pipeline_dump_to_dot('simple-pipeline', 'state-playing')

        dsl_main_loop_run()
        break

    print(retval)

    dsl_pipeline_delete_all()
    dsl_component_delete_all()

if __name__ == '__main__':
    sys.exit(main(sys.argv))
