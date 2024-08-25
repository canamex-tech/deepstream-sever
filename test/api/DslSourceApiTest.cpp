/*
The MIT License

Copyright (c) 2019-2024, Prominence AI, Inc.

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

#include "catch.hpp"
#include "DslApi.h"
#include "Dsl.h"

static std::wstring pipeline_name(L"test-pipeline");
static std::wstring pipeline_name0(L"test-pipeline0");
static std::wstring pipeline_name1(L"test-pipeline1");
static std::wstring pipeline_name2(L"test-pipeline2");

static std::wstring source_name(L"source");
static std::wstring source_name0(L"source0");
static std::wstring source_name1(L"source1");
static std::wstring source_name2(L"source2");
static std::wstring source_name3(L"source3");
static std::wstring source_name4(L"source4");
static std::wstring source_name5(L"source5");

static uint width(1280);
static uint height(720);
static uint fps_n(30);
static uint fps_d(1);

static uint skip_frames(0);
static uint drop_frame_interval(0);

static std::wstring dewarper_name(L"dewarper");
static std::wstring defConfigFile(
    L"/opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-dewarper-test/config_dewarper.txt");

static std::wstring uri(L"/opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h265.mp4");
static std::wstring image_path(L"/opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.jpg");

static std::wstring jpeg_file_path_multi(L"./test/streams/sample_720p.%d.jpg");

static uint protocol(DSL_RTP_ALL);
static uint latency(100);
static uint timeout(0);
static uint retTimeout(123);

static uint interval(0);

static std::wstring rtsp_uri(L"rtsp://username:password@192.168.0.14:554");

static boolean is_live(false);
static std::wstring buffer_in_format(DSL_VIDEO_FORMAT_I420);


static std::wstring def_device_location(L"/dev/video0");
static std::wstring video1_device_location(L"/dev/video1");
static std::wstring video2_device_location(L"/dev/video2");
static std::wstring video3_device_location(L"/dev/video3");
static std::wstring video4_device_location(L"/dev/video4");
static std::wstring video5_device_location(L"/dev/video5");


SCENARIO( "The Components container is updated correctly on new source", 
    "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new Source is created" ) 
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
                def_device_location.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                REQUIRE( dsl_component_list_size() == 1 );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    
    
SCENARIO( "The Components container is updated correctly on Source Delete",
    "[source-api]" )
{
    GIVEN( "One Source im memory" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
            def_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_component_list_size() == 1 );
        
        WHEN( "The Source is deleted" )
        {
            REQUIRE( dsl_component_delete(source_name.c_str()) 
                == DSL_RESULT_SUCCESS );
            
            THEN( "The list and contents are updated correctly" )
            {
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A Source in use can't be deleted", 
    "[source-api]" )
{
    GIVEN( "A new Source and new pPipeline" ) 
    {

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
            def_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) 
            == DSL_RESULT_SUCCESS );

        WHEN( "The Source is added to the Pipeline" ) 
        {
            REQUIRE( dsl_pipeline_component_add(pipeline_name.c_str(), 
                source_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The Source can't be deleted" ) 
            {
                REQUIRE( dsl_component_delete(source_name.c_str()) 
                    == DSL_RESULT_COMPONENT_IN_USE );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A Source, once removed from a Pipeline, can be deleted",
    "[source-api]" )
{
    GIVEN( "A new Pipeline with a Child V4L2 Source" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
            def_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_pipeline_component_add(pipeline_name.c_str(), 
            source_name.c_str()) == DSL_RESULT_SUCCESS );
            
        WHEN( "The Source is removed from the Pipeline" ) 
        {
            REQUIRE( dsl_pipeline_component_remove(pipeline_name.c_str(),
                source_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The Source can be deleted successfully" ) 
            {
                REQUIRE( dsl_component_delete(source_name.c_str()) 
                    == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "Unique-ids and stream-ids are managed correctly for multiple sources ", 
    "[source-api]" )
{
    GIVEN( "A new Source and new pPipeline" ) 
    {

        REQUIRE( dsl_component_list_size() == 0 );

        int ret_unique_id(99);
        int ret_stream_id(99);
        REQUIRE( dsl_source_v4l2_new(source_name0.c_str(), 
            def_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name0.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name0.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        ret_unique_id = 99;
        ret_stream_id = 99;
        REQUIRE( dsl_source_v4l2_new(source_name1.c_str(), 
            video1_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name1.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name1.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        ret_unique_id = 99;
        ret_stream_id = 99;
        REQUIRE( dsl_source_v4l2_new(source_name2.c_str(), 
            video2_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name2.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name2.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        ret_unique_id = 99;
        ret_stream_id = 99;
        REQUIRE( dsl_source_v4l2_new(source_name3.c_str(), 
            video3_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name3.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name3.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        ret_unique_id = 99;
        ret_stream_id = 99;
        REQUIRE( dsl_source_v4l2_new(source_name4.c_str(), 
            video4_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name4.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name4.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        ret_unique_id = 99;
        ret_stream_id = 99;
        REQUIRE( dsl_source_v4l2_new(source_name5.c_str(), 
            video5_device_location.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_source_unique_id_get(source_name5.c_str(), 
        &ret_unique_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_unique_id == -1);
        REQUIRE( dsl_source_stream_id_get(source_name5.c_str(), 
        &ret_stream_id) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_stream_id == -1);

        REQUIRE( dsl_pipeline_new(pipeline_name0.c_str()) 
            == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pipeline_new(pipeline_name1.c_str()) 
            == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pipeline_new(pipeline_name2.c_str()) 
            == DSL_RESULT_SUCCESS );

        WHEN( "The Sources are added to the Pipeline" ) 
        {
            REQUIRE( dsl_pipeline_component_add(pipeline_name0.c_str(), 
                source_name0.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_component_add(pipeline_name0.c_str(), 
                source_name1.c_str()) == DSL_RESULT_SUCCESS );

            REQUIRE( dsl_pipeline_component_add(pipeline_name1.c_str(), 
                source_name2.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_component_add(pipeline_name1.c_str(), 
                source_name3.c_str()) == DSL_RESULT_SUCCESS );

            REQUIRE( dsl_pipeline_component_add(pipeline_name2.c_str(), 
                source_name4.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_component_add(pipeline_name2.c_str(), 
                source_name5.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The source-ids are updated correctly" ) 
            {
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name0.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00000000 );
                REQUIRE( dsl_source_stream_id_get(source_name0.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 0 );
                
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name1.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00000001 );
                REQUIRE( dsl_source_stream_id_get(source_name1.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 1 );
                
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name2.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00010000 );
                REQUIRE( dsl_source_stream_id_get(source_name2.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 0 );
                
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name3.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00010001 );
                REQUIRE( dsl_source_stream_id_get(source_name3.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 1 );
                
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name4.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00020000 );
                REQUIRE( dsl_source_stream_id_get(source_name4.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 0 );
                
                ret_unique_id = 99;
                ret_stream_id = 99;
                REQUIRE( dsl_source_unique_id_get(source_name5.c_str(), 
                &ret_unique_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_unique_id == 0x00020001 );
                REQUIRE( dsl_source_stream_id_get(source_name5.c_str(), 
                &ret_stream_id) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_id == 1 );
                
                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new App Source returns the correct attribute values",
    "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new App Source is created" ) 
        {
            REQUIRE( dsl_source_app_new(source_name.c_str(), 
                is_live, buffer_in_format.c_str(), width, height, 
                fps_n, fps_d) == DSL_RESULT_SUCCESS );

            THEN( "All default attributes are returned correctly" ) 
            {
                uint ret_width(0), ret_height(0), ret_fps_n(0), ret_fps_d(0);
                REQUIRE( dsl_source_video_dimensions_get(source_name.c_str(), 
                    &ret_width, &ret_height) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_source_frame_rate_get(source_name.c_str(), 
                    &ret_fps_n, &ret_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_width == width );
                REQUIRE( ret_height == height );
                REQUIRE( ret_fps_n == fps_n );
                REQUIRE( ret_fps_d == fps_d );
                REQUIRE( dsl_source_is_live(source_name.c_str()) == is_live );

                boolean do_timestamp(TRUE);
                REQUIRE( dsl_source_app_do_timestamp_get(source_name.c_str(),
                    &do_timestamp) == DSL_RESULT_SUCCESS );
                REQUIRE( do_timestamp == FALSE ); // default
                
                boolean block_enabled(TRUE);
                REQUIRE( dsl_source_app_block_enabled_get(source_name.c_str(),
                    &block_enabled) == DSL_RESULT_SUCCESS );
                REQUIRE( block_enabled == FALSE ); // default

                uint64_t current_level_bytes(123456);
                REQUIRE( dsl_source_app_current_level_bytes_get(source_name.c_str(),
                    &current_level_bytes) == DSL_RESULT_SUCCESS );
                REQUIRE( current_level_bytes == 0 ); // default
                
                uint64_t max_bytes(0);
                REQUIRE( dsl_source_app_max_level_bytes_get(source_name.c_str(),
                    &max_bytes) == DSL_RESULT_SUCCESS );
                REQUIRE( max_bytes == 200000 ); // default

                // BUFFER-OUT DEFAULTS COMMON TO ALL SOURCES - ONLY TESTED HERE.

                // test returned media-type
                const wchar_t* ret_media_cstrint;
                REQUIRE( dsl_source_media_type_get(source_name.c_str(), 
                    &ret_media_cstrint) == DSL_RESULT_SUCCESS );
                std::wstring exp_media_string(DSL_MEDIA_TYPE_VIDEO_XRAW);
                std::wstring ret_media_string(ret_media_cstrint);
                REQUIRE( exp_media_string == ret_media_cstrint );
                
                // test returned default buffer-out-format
                const wchar_t* ret_format_cstrint;
                REQUIRE( dsl_source_video_buffer_out_format_get(source_name.c_str(), 
                    &ret_format_cstrint) == DSL_RESULT_SUCCESS );
                std::wstring exp_format_string(DSL_VIDEO_FORMAT_DEFAULT);
                std::wstring ret_format_string(ret_format_cstrint);
                REQUIRE( exp_format_string == ret_format_cstrint );
                
                // buffer out default dimensions
                uint ret_bo_width(99), ret_bo_height(99);
                REQUIRE( dsl_source_video_buffer_out_dimensions_get(source_name.c_str(), 
                    &ret_bo_width, &ret_bo_height) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_width == 0 );
                REQUIRE( ret_bo_height == 0 );
                
                // buffer out default framerate
                uint ret_bo_fps_n(99), ret_bo_fps_d(99);
                REQUIRE( dsl_source_video_buffer_out_frame_rate_get(source_name.c_str(), 
                    &ret_bo_fps_n, &ret_bo_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_fps_n == 0 );
                REQUIRE( ret_bo_fps_d == 0 );
                
                // buffer out default crop rectangles
                uint ret_bo_rec_left(99), ret_bo_rec_top(99);
                uint ret_bo_rec_width(99), ret_bo_rec_height(99);
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(
                    source_name.c_str(), DSL_VIDEO_CROP_AT_SRC, 
                    &ret_bo_rec_left, &ret_bo_rec_top,
                    &ret_bo_rec_width, &ret_bo_rec_height) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_rec_left == 0 );
                REQUIRE( ret_bo_rec_top == 0 );
                REQUIRE( ret_bo_rec_width == 0 );
                REQUIRE( ret_bo_rec_height == 0 );
                ret_bo_rec_left = 99;
                ret_bo_rec_top = 99;
                ret_bo_rec_width = 99;
                ret_bo_rec_height = 99;
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(
                    source_name.c_str(), DSL_VIDEO_CROP_AT_SRC, 
                    &ret_bo_rec_left, &ret_bo_rec_top,
                    &ret_bo_rec_width, &ret_bo_rec_height) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_rec_left == 0 );
                REQUIRE( ret_bo_rec_top == 0 );
                REQUIRE( ret_bo_rec_width == 0 );
                REQUIRE( ret_bo_rec_height == 0 );
                
                uint ret_orientation(99);
                REQUIRE( dsl_source_video_buffer_out_orientation_get(
                    source_name.c_str(), &ret_orientation) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_orientation == 0 );
                
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "An App Source can update its settings correctly", "[source-api]" )
{
    GIVEN( "A new App Source Component" ) 
    {
        REQUIRE( dsl_source_app_new(source_name.c_str(), 
            is_live, buffer_in_format.c_str(), width, height, 
            fps_n, fps_d) == DSL_RESULT_SUCCESS );

        WHEN( "The App Source's stream-format setting is set" ) 
        {
            uint stream_format(DSL_STREAM_FORMAT_TIME); // default is BYTE
            REQUIRE( dsl_source_app_stream_format_set(source_name.c_str(),
                stream_format) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" ) 
            {
                uint ret_stream_format(DSL_STREAM_FORMAT_BYTE);
                REQUIRE( dsl_source_app_stream_format_get(source_name.c_str(),
                    &ret_stream_format) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_stream_format == stream_format );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The App Source's do-timestamp setting is set" ) 
        {
            boolean do_timestamp(TRUE); // default is FALSE
            REQUIRE( dsl_source_app_do_timestamp_set(source_name.c_str(),
                do_timestamp) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" ) 
            {
                boolean ret_do_timestamp(FALSE);
                REQUIRE( dsl_source_app_do_timestamp_get(source_name.c_str(),
                    &ret_do_timestamp) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_do_timestamp == do_timestamp );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The App Source's block-enabled setting is set" ) 
        {
            boolean block_enabled(TRUE);
            REQUIRE( dsl_source_app_block_enabled_set(source_name.c_str(),
                block_enabled) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" ) 
            {
                boolean ret_block_enabled(FALSE);
                REQUIRE( dsl_source_app_block_enabled_get(source_name.c_str(),
                    &ret_block_enabled) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_block_enabled == block_enabled );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The App Source's max-level-in-bytes setting is set" ) 
        {
            uint64_t max_bytes(100000);
            REQUIRE( dsl_source_app_max_level_bytes_set(source_name.c_str(),
                max_bytes) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" ) 
            {
                uint64_t ret_max_bytes(0);
                REQUIRE( dsl_source_app_max_level_bytes_get(source_name.c_str(),
                    &ret_max_bytes) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_max_bytes == max_bytes ); 

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An App Source can update its buffer-out settings correctly",
    "[source-api]" )
{
    GIVEN( "A new App Source Component" ) 
    {
        REQUIRE( dsl_source_app_new(source_name.c_str(), 
            is_live, buffer_in_format.c_str(), width, height, 
            fps_n, fps_d) == DSL_RESULT_SUCCESS );

        WHEN( "The App Source's buffer-out-format setting is set" ) 
        {
            std::wstring new_format_string(DSL_VIDEO_FORMAT_RGBA);
            REQUIRE( dsl_source_video_buffer_out_format_set(source_name.c_str(), 
                new_format_string.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" ) 
            {
                const wchar_t* ret_format_cstring;
                REQUIRE( dsl_source_video_buffer_out_format_get(source_name.c_str(), 
                    &ret_format_cstring) == DSL_RESULT_SUCCESS );
                std::wstring ret_format_string(ret_format_cstring);
                REQUIRE( ret_format_string == new_format_string );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The App Source's buffer-out-dimensions settings are set" ) 
        {
            uint new_width(300), new_height(400);
            REQUIRE( dsl_source_video_buffer_out_dimensions_set(source_name.c_str(), 
                new_width, new_height) == DSL_RESULT_SUCCESS );

            THEN( "The correct values are returned on get" ) 
            {
                uint ret_bo_width(99), ret_bo_height(99);
                REQUIRE( dsl_source_video_buffer_out_dimensions_get(source_name.c_str(), 
                    &ret_bo_width, &ret_bo_height) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_width == new_width );
                REQUIRE( ret_bo_height == new_height );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The App Source's buffer-out-frame-rate is set" ) 
        {
            uint new_bo_fps_n(30), new_bo_fps_d(1);
            REQUIRE( dsl_source_video_buffer_out_frame_rate_set(source_name.c_str(), 
                new_bo_fps_n, new_bo_fps_d) == DSL_RESULT_SUCCESS );

            THEN( "The correct values are returned on get" ) 
            {
                uint ret_bo_fps_n(99), ret_bo_fps_d(99);
                REQUIRE( dsl_source_video_buffer_out_frame_rate_get(source_name.c_str(), 
                    &ret_bo_fps_n, &ret_bo_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_fps_n == new_bo_fps_n );
                REQUIRE( ret_bo_fps_d == new_bo_fps_d );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        
        WHEN( "The App Source's buffer-out-crop settings are set" ) 
        {
            uint new_bo_rec_left(10), new_bo_rec_top(10);
            uint new_bo_rec_width(200), new_bo_rec_height(200);

            // first, make sure invalid when value is caught
            REQUIRE( dsl_source_video_buffer_out_crop_rectangle_set(source_name.c_str(), 
                DSL_VIDEO_CROP_AT_DEST+1, new_bo_rec_left, new_bo_rec_top,
                new_bo_rec_width, new_bo_rec_height) == DSL_RESULT_SOURCE_SET_FAILED );

            REQUIRE( dsl_source_video_buffer_out_crop_rectangle_set(source_name.c_str(), 
                DSL_VIDEO_CROP_AT_DEST, new_bo_rec_left, new_bo_rec_top,
                new_bo_rec_width, new_bo_rec_height) == DSL_RESULT_SUCCESS );

            THEN( "The correct values are returned on get" ) 
            {
                uint ret_bo_rec_left(99), ret_bo_rec_top(99);
                uint ret_bo_rec_width(99), ret_bo_rec_height(99);
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(source_name.c_str(), 
                    DSL_VIDEO_CROP_AT_DEST, &ret_bo_rec_left, &ret_bo_rec_top,
                    &ret_bo_rec_width, &ret_bo_rec_height) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_rec_left == new_bo_rec_left );
                REQUIRE( ret_bo_rec_top == new_bo_rec_top );
                REQUIRE( ret_bo_rec_width == new_bo_rec_width );
                REQUIRE( ret_bo_rec_height == new_bo_rec_height );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            } 
        }
        WHEN( "The App Source's buffer-out-orientation settings are set" ) 
        {
            // make sure an invalid value is caught
            REQUIRE( dsl_source_video_buffer_out_orientation_set(source_name.c_str(), 
                DSL_VIDEO_ORIENTATION_FLIP_UPPER_LEFT_TO_LOWER_RIGHT+1) 
                    == DSL_RESULT_SOURCE_SET_FAILED );
            
            uint new_bo_orientation(DSL_VIDEO_ORIENTATION_FLIP_VERTICALLY);
            REQUIRE( dsl_source_video_buffer_out_orientation_set(source_name.c_str(), 
                new_bo_orientation) == DSL_RESULT_SUCCESS );

            THEN( "The correct values are returned on get" ) 
            {
                uint ret_bo_orientation(99);
                REQUIRE( dsl_source_video_buffer_out_orientation_get(source_name.c_str(),
                    &ret_bo_orientation) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_bo_orientation == DSL_VIDEO_ORIENTATION_FLIP_VERTICALLY );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            } 
        }
    }
}

static void need_data_handler(uint length, void* client_data)
{
}

static void enough_data_handler(void* client_data)
{
}

SCENARIO( "A new App Source can add and remove data-handlers correctly", 
    "[source-api]" )
{
    GIVEN( "A new App Source component" ) 
    {
        
        REQUIRE( dsl_source_app_new(source_name.c_str(), is_live, 
            buffer_in_format.c_str(), width, height, 
            fps_n, fps_d) == DSL_RESULT_SUCCESS );

        WHEN( "Client data-handers are added. " ) 
        {
            REQUIRE( dsl_source_app_data_handlers_add(source_name.c_str(),
                need_data_handler, enough_data_handler, NULL) == DSL_RESULT_SUCCESS );

            // second call must fail
            REQUIRE( dsl_source_app_data_handlers_add(source_name.c_str(),
                need_data_handler, enough_data_handler, NULL) == DSL_RESULT_SOURCE_SET_FAILED );
            
            THEN( "The data-handlers can be removed correctly" ) 
            {
                REQUIRE( dsl_source_app_data_handlers_remove(source_name.c_str()) 
                    == DSL_RESULT_SUCCESS );

                // second call must fail
                REQUIRE( dsl_source_app_data_handlers_remove(source_name.c_str()) 
                    == DSL_RESULT_SOURCE_SET_FAILED );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new App Source fails to push-buffer and EOS when in a unlinked state", 
    "[source-api]" )
{
    GIVEN( "A new App Source component" ) 
    {
        
        REQUIRE( dsl_source_app_new(source_name.c_str(), is_live, 
            buffer_in_format.c_str(), width, height, 
            fps_n, fps_d) == DSL_RESULT_SUCCESS );

        WHEN( "When the App Source is in an unlinked state. " ) 
        {
            
            THEN( "The push-buffer and EOS services must fail" ) 
            {
                std::string fake_buffer("this is a fake buffer");
                
                REQUIRE( dsl_source_app_buffer_push(source_name.c_str(),
                    (void*)fake_buffer.c_str()) == DSL_RESULT_SOURCE_SET_FAILED );

                // second call must fail
                REQUIRE( dsl_source_app_eos(source_name.c_str()) 
                    == DSL_RESULT_SOURCE_SET_FAILED );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new CSI Camera Source returns the correct attribute values", 
    "[source-api]" )
{
    // Get the Device properties
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    
    if (deviceProp.integrated)
    {
        GIVEN( "An empty list of Components" ) 
        {
            REQUIRE( dsl_component_list_size() == 0 );

            WHEN( "A new Source is created" ) 
            {
                REQUIRE( dsl_source_csi_new(source_name.c_str(), 
                    width, height, fps_n, fps_d) == DSL_RESULT_SUCCESS );

                THEN( "The list size and contents are updated correctly" ) 
                {
                    uint ret_width(0), ret_height(0), ret_fps_n(0), ret_fps_d(0);
                    REQUIRE( dsl_source_video_dimensions_get(source_name.c_str(), 
                        &ret_width, &ret_height) == DSL_RESULT_SUCCESS );
                    REQUIRE( dsl_source_frame_rate_get(source_name.c_str(), 
                        &ret_fps_n, &ret_fps_d) == DSL_RESULT_SUCCESS );
                    REQUIRE( ret_width == width );
                    REQUIRE( ret_height == height );
                    REQUIRE( ret_fps_n == fps_n );
                    REQUIRE( ret_fps_d == fps_d );
                    REQUIRE( dsl_source_is_live(source_name.c_str()) == TRUE );

                    REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                }
            }
        }
    }
}    

SCENARIO( "A new CIS Camera Source set/get its sensor-id correctly", "[source-api]" )
{
    // Get the Device properties
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    
    if (deviceProp.integrated)
    {
        GIVEN( "An empty list of Components" ) 
        {
            REQUIRE( dsl_component_list_size() == 0 );

            REQUIRE( dsl_source_csi_new(source_name.c_str(), width, height, fps_n, fps_d) == DSL_RESULT_SUCCESS );

            WHEN( "The USB Source's device-location is set" ) 
            {
                // Check default first
                uint sensor_id;
                REQUIRE( dsl_source_csi_sensor_id_get(source_name.c_str(), 
                    &sensor_id) == DSL_RESULT_SUCCESS );
                REQUIRE( sensor_id == 0 );
                
                uint new_sensor_id(5);
                REQUIRE( dsl_source_csi_sensor_id_set(source_name.c_str(), 
                    new_sensor_id) == DSL_RESULT_SUCCESS );

                THEN( "The correct updated value is returned on get" ) 
                {
                    REQUIRE( dsl_source_csi_sensor_id_get(source_name.c_str(), 
                        &sensor_id) == DSL_RESULT_SUCCESS );
                    REQUIRE( sensor_id == new_sensor_id );
                    REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                }
            }
        }
    }    
}
SCENARIO( "A new V4L2 Source returns the correct attribute values", "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new V4L2 Source is created" ) 
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
                def_device_location.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                const wchar_t* device_location;
                REQUIRE( dsl_source_v4l2_device_location_get(source_name.c_str(), 
                    &device_location) == DSL_RESULT_SUCCESS );
                std::wstring ret_device_location(device_location);
                REQUIRE( ret_device_location == def_device_location );
                REQUIRE( dsl_source_is_live(source_name.c_str()) == TRUE );

                const wchar_t* c_ret_device_name;
                REQUIRE( dsl_source_v4l2_device_name_get(source_name.c_str(), 
                    &c_ret_device_name) == DSL_RESULT_SUCCESS );
                std::wstring ret_device_name(c_ret_device_name);
                REQUIRE( ret_device_name == L"" );

                int ret_device_fd;
                REQUIRE( dsl_source_v4l2_device_fd_get(source_name.c_str(), 
                    &ret_device_fd) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_device_fd == -1 );

                uint ret_device_flags;
                REQUIRE( dsl_source_v4l2_device_flags_get(source_name.c_str(), 
                    &ret_device_flags) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_device_flags == DSL_V4L2_DEVICE_TYPE_NONE );

                int retBrightness(0), retContrast(0), retHue(0);
                REQUIRE( dsl_source_v4l2_picture_settings_get(source_name.c_str(), 
                    &retBrightness, &retContrast, &retHue) == DSL_RESULT_SUCCESS );
                REQUIRE( retBrightness == 0 );
                REQUIRE( retContrast == 0 );
                REQUIRE( retHue == 0 );
                
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new V4L2 Source can set/get its device location correctly", 
    "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
            def_device_location.c_str()) == DSL_RESULT_SUCCESS );

        WHEN( "The V4L2 Source's device-location is set" ) 
        {
            // Check default first
            const wchar_t* device_location;
            REQUIRE( dsl_source_v4l2_device_location_get(source_name.c_str(), 
                &device_location) == DSL_RESULT_SUCCESS );
            std::wstring ret_device_location(device_location);
            REQUIRE( ret_device_location == def_device_location );
            
            std::wstring new_device_location(L"/dev/video1");
            REQUIRE( dsl_source_v4l2_device_location_set(source_name.c_str(), 
                new_device_location.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The correct updated value is returned on get" ) 
            {
                REQUIRE( dsl_source_v4l2_device_location_get(source_name.c_str(), 
                    &device_location) == DSL_RESULT_SUCCESS );
                ret_device_location = device_location;
                REQUIRE( ret_device_location == new_device_location );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new URI Source returns the correct attribute values", "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new URI Source is created" ) 
        {
        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(),
            false, skip_frames, drop_frame_interval) == DSL_RESULT_SUCCESS );

            THEN( "All default attributes are returned correctly" ) 
            {
                uint ret_width(99), ret_height(99), ret_fps_n(99), ret_fps_d(99);
                REQUIRE( dsl_source_video_dimensions_get(source_name.c_str(), 
                    &ret_width, &ret_height) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_source_frame_rate_get(source_name.c_str(), 
                    &ret_fps_n, &ret_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_width == 1920 );
                REQUIRE( ret_height == 1080 );
                REQUIRE( ret_fps_n == 30 );
                REQUIRE( ret_fps_d == 1 );
                REQUIRE( dsl_source_is_live(source_name.c_str()) == false );
                
                // Note URI Source convers URI to real path which has DeepStream 
                // version number - don't compare as it makes the test case version
                // version spec
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A Source not-in-use can not be Paused or Resumed", "[source-api]" )
{
    GIVEN( "A new Source not in use by a Pipeline" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new Source is not in use by a Pipeline" )
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(),
                L"/dev/video0/") == DSL_RESULT_SUCCESS );

            THEN( "The Source can not be Paused as it's not in use" ) 
            {
                REQUIRE( dsl_source_pause(source_name.c_str())  == DSL_RESULT_SOURCE_NOT_IN_USE );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
        WHEN( "A new Source is not in use by a Pipeline" )
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(),
                L"/dev/video0/") == DSL_RESULT_SUCCESS );
    
            THEN( "The Source can not be Resumed as it's not in use" ) 
            {
                REQUIRE( dsl_source_resume(source_name.c_str())  == DSL_RESULT_SOURCE_NOT_IN_USE );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}    
    
SCENARIO( "A Source in-use but in a null-state can not be Paused or Resumed", "[source-api]" )
{
    GIVEN( "A new Source not in use by a Pipeline" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );


        WHEN( "A new Source is in-use by a new Pipeline in a null-state" )
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(),
                L"/dev/video0/") == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_component_add(pipeline_name.c_str(), 
                source_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The Source can not be Paused as it's in a null-state" ) 
            {
                REQUIRE( dsl_source_pause(source_name.c_str())  == DSL_RESULT_SOURCE_NOT_IN_PLAY );
                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
        WHEN( "A new Source is in-use by a new Pipeline in a null-state" )
        {
            REQUIRE( dsl_source_v4l2_new(source_name.c_str(),
                L"/dev/video0/") == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_pipeline_component_add(pipeline_name.c_str(), 
                source_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The Source can not be Resumed as it's not in use" ) 
            {
                REQUIRE( dsl_source_resume(source_name.c_str())  == DSL_RESULT_SOURCE_NOT_IN_PAUSE );
                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}    
    
SCENARIO( "An invalid Source is caught by all Set and Get API calls", "[source-api]" )
{
    GIVEN( "A new Fake Sink as incorrect Source Type" ) 
    {
        std::wstring fakeSinkName(L"fake-sink");
            
        uint currBitrate(0);
        uint currInterval(0);
    
        uint newBitrate(2500000);
        uint newInterval(10);

        WHEN( "The File Sink Get-Set API called with a Fake sink" )
        {
            
            REQUIRE( dsl_sink_fake_new(fakeSinkName.c_str()) == DSL_RESULT_SUCCESS);

            THEN( "The Source Pause and Resume APIs fail correctly")
            {
                uint width(0), height(0);
                uint fps_n(0), fps_d(0);
                REQUIRE( dsl_source_video_dimensions_get(fakeSinkName.c_str(), &width, &height) == DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE);
                REQUIRE( dsl_source_frame_rate_get(fakeSinkName.c_str(), &fps_n, &fps_d) == DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE);
                REQUIRE( dsl_source_pause(fakeSinkName.c_str()) == DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE);
                REQUIRE( dsl_source_resume(fakeSinkName.c_str()) == DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE);
                REQUIRE( dsl_source_is_live(fakeSinkName.c_str()) == DSL_RESULT_SOURCE_COMPONENT_IS_NOT_SOURCE);

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A Dewarper can be added to and removed from a Source Component", "[source-api]" )
{
    GIVEN( "A new Source and new Dewarper" )
    {
        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(),
            false, skip_frames, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_dewarper_new(dewarper_name.c_str(), 
            defConfigFile.c_str(), 0) == DSL_RESULT_SUCCESS );

        WHEN( "The Dewarper is added to the Source" ) 
        {
            REQUIRE( dsl_source_video_dewarper_add(source_name.c_str(), 
                dewarper_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The Dewarper can be removed" )
            {
                // A second call must fail
                REQUIRE( dsl_source_video_dewarper_add(source_name.c_str(), 
                    dewarper_name.c_str()) == DSL_RESULT_SOURCE_DEWARPER_ADD_FAILED );

                REQUIRE( dsl_source_video_dewarper_remove(source_name.c_str()) 
                    == DSL_RESULT_SUCCESS );

                // A second time must fail
                REQUIRE( dsl_source_video_dewarper_remove(source_name.c_str()) 
                    == DSL_RESULT_SOURCE_DEWARPER_REMOVE_FAILED );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "Adding an invalid Dewarper to a  Source Component fails", "[source-api]" )
{
    GIVEN( "A new Source and a Fake Sink as invalid Dewarper" )
    {
        std::wstring fakeSinkName(L"fake-sink");

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(),
            false, skip_frames, drop_frame_interval) == DSL_RESULT_SUCCESS );

        WHEN( "A Fake Sink is used as Dewarper" ) 
        {
            REQUIRE( dsl_sink_fake_new(fakeSinkName.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "Adding the Fake Sink as a Dewarper will fail" )
            {
                REQUIRE( dsl_source_video_dewarper_add(source_name.c_str(), 
                    fakeSinkName.c_str()) == DSL_RESULT_COMPONENT_NOT_THE_CORRECT_TYPE );
                    
                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An RTSP Source's Timeout can be updated correctly", "[source-api]" )
{
    GIVEN( "A new RTSP Source with a 0 timeout" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );
            
        REQUIRE( dsl_source_rtsp_timeout_get(source_name.c_str(), &retTimeout) == DSL_RESULT_SUCCESS );
        REQUIRE( retTimeout == timeout );

        WHEN( "The RTSP Source's buffer timeout is updated" ) 
        {
            uint timeout(321);
            uint retTimeout(0);
            REQUIRE( dsl_source_rtsp_timeout_set(source_name.c_str(), timeout) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned after update" )
            {
                REQUIRE( dsl_source_rtsp_timeout_get(source_name.c_str(), &retTimeout) == DSL_RESULT_SUCCESS );
                REQUIRE( retTimeout == timeout );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An RTSP Source's Reconnect Stats can gotten and cleared", "[source-api]" )
{
    GIVEN( "A new RTSP Source with a 0 timeout" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );
            
        WHEN( "A client gets an RTSP Source's connection data" ) 
        {
            dsl_rtsp_connection_data data{0};
            data.first_connected = 123;
            data.last_connected = 456;
            data.last_disconnected = 789;
            data.count = 654;
            data.is_in_reconnect = true;
            data.retries = 444;
            REQUIRE( dsl_source_rtsp_connection_data_get(source_name.c_str(), 
                &data) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned after update" )
            {
                REQUIRE( data.first_connected == 0 );
                REQUIRE( data.last_connected == 0 );
                REQUIRE( data.last_disconnected == 0 );
                REQUIRE( data.count == 0 );
                REQUIRE( data.is_in_reconnect == 0 );
                REQUIRE( data.retries == 0 );

                REQUIRE( dsl_source_rtsp_connection_stats_clear(source_name.c_str()) == DSL_RESULT_SUCCESS );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An RTSP Source's latency setting can be updated correctly", 
    "[source-api]" )
{
    GIVEN( "A new RTSP Source" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );
            
        uint ret_latency(0);
        
        REQUIRE( dsl_source_rtsp_latency_get(source_name.c_str(), 
            &ret_latency) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_latency == latency );

        WHEN( "The RTSP Source's latency is updated" ) 
        {
            uint new_latency(1234);
                
            REQUIRE( dsl_source_rtsp_latency_set(source_name.c_str(), 
                new_latency) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned after update" )
            {
                REQUIRE( dsl_source_rtsp_latency_get(source_name.c_str(), 
                    &ret_latency) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_latency == new_latency );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An RTSP Source's drop-on-latency enabled setting can be updated correctly", 
    "[source-api]" )
{
    GIVEN( "A new RTSP Source" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );
            
        boolean ret_drop_on_latency(true);
        
        REQUIRE( dsl_source_rtsp_drop_on_latency_enabled_get(source_name.c_str(), 
            &ret_drop_on_latency) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_drop_on_latency == false );

        WHEN( "The RTSP Source's latency is updated" ) 
        {
            boolean new_drop_on_latency(true);
                
            REQUIRE( dsl_source_rtsp_drop_on_latency_enabled_set(source_name.c_str(), 
                new_drop_on_latency) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned after update" )
            {
                REQUIRE( dsl_source_rtsp_drop_on_latency_enabled_get(source_name.c_str(), 
                    &ret_drop_on_latency) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_drop_on_latency == new_drop_on_latency );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "An RTSP Source's tls-validation-flags can be updated correctly", "[source-api]" )
{
    GIVEN( "A new RTSP Source" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );
            
        uint ret_flags(0);
        
        REQUIRE( dsl_source_rtsp_tls_validation_flags_get(source_name.c_str(), 
            &ret_flags) == DSL_RESULT_SUCCESS );
        REQUIRE( ret_flags == DSL_TLS_CERTIFICATE_VALIDATE_ALL );

        WHEN( "The RTSP Source's tls-validation-flags is updated" ) 
        {
            uint new_flags(DSL_TLS_CERTIFICATE_UNKNOWN_CA | 
                DSL_TLS_CERTIFICATE_BAD_IDENTITY);
                
            REQUIRE( dsl_source_rtsp_tls_validation_flags_set(source_name.c_str(), 
                new_flags) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned after update" )
            {
                REQUIRE( dsl_source_rtsp_tls_validation_flags_get(source_name.c_str(), 
                    &ret_flags) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_flags == new_flags );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "Invalid tls-validation-flags are used" ) 
        {
            uint new_flags(DSL_TLS_CERTIFICATE_VALIDATE_ALL+1);
                
            REQUIRE( dsl_source_rtsp_tls_validation_flags_set(source_name.c_str(), 
                new_flags) == DSL_RESULT_SOURCE_SET_FAILED );

            THEN( "The tls-validation-flags are unchanged" )
            {
                REQUIRE( dsl_source_rtsp_tls_validation_flags_get(source_name.c_str(), 
                    &ret_flags) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_flags == DSL_TLS_CERTIFICATE_VALIDATE_ALL );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

static void source_state_change_listener_cb1(uint prev_state, uint curr_state, void* user_data)
{
}

SCENARIO( "An RTSP state-change-listener can be added and removed", "[source-api]" )
{
    GIVEN( "A new RTSP Source and client listener callback" )
    {
        REQUIRE( dsl_source_rtsp_new(source_name.c_str(), rtsp_uri.c_str(), protocol,
            skip_frames, interval, latency, timeout) == DSL_RESULT_SUCCESS );

        WHEN( "A state-change-listner is added" )
        {
            REQUIRE( dsl_source_rtsp_state_change_listener_add(source_name.c_str(),
                source_state_change_listener_cb1, NULL) == DSL_RESULT_SUCCESS );

            // ensure the same listener twice fails
            REQUIRE( dsl_source_rtsp_state_change_listener_add(source_name.c_str(),
                source_state_change_listener_cb1, NULL) == DSL_RESULT_SOURCE_CALLBACK_ADD_FAILED );

            THEN( "The same listner can be remove" ) 
            {
                REQUIRE( dsl_source_rtsp_state_change_listener_remove(source_name.c_str(),
                    source_state_change_listener_cb1) == DSL_RESULT_SUCCESS );

                // calling a second time must faile
                REQUIRE( dsl_source_rtsp_state_change_listener_remove(source_name.c_str(),
                    source_state_change_listener_cb1) == DSL_RESULT_SOURCE_CALLBACK_REMOVE_FAILED );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new File Source returns the correct attribute values", "[source-api]" )
{
    GIVEN( "Attributes for a new File Source" ) 
    {
        std::wstring w_file_path(L"/opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h265.mp4");
        std::string file_path(w_file_path.begin(), w_file_path.end());
        
        char absolutePath[PATH_MAX+1];
        std::string full_file_path(realpath(file_path.c_str(), absolutePath));
        std::wstring w_full_file_path(full_file_path.begin(), full_file_path.end());
        w_full_file_path.insert(0, L"file:");
        
        boolean repeat_enabled(1);

        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new File Source is created with a File Path" ) 
        {
            REQUIRE( dsl_source_file_new(source_name.c_str(), 
                w_file_path.c_str(), repeat_enabled) == DSL_RESULT_SUCCESS );

            THEN( "The correct attribute values are returned" ) 
            {
                const wchar_t* pRetFilePath;
                REQUIRE( dsl_source_file_file_path_get(source_name.c_str(), 
                    &pRetFilePath) == DSL_RESULT_SUCCESS );
                std::wstring w_ret_file_path(pRetFilePath);
                REQUIRE( w_ret_file_path == w_full_file_path);
                
                uint ret_width(0), ret_height(0), ret_fps_n(0), ret_fps_d(0);
                REQUIRE( dsl_source_video_dimensions_get(source_name.c_str(), 
                    &ret_width, &ret_height) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_source_frame_rate_get(source_name.c_str(), 
                    &ret_fps_n, &ret_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_width == 1920 );
                REQUIRE( ret_height == 1080 );
                REQUIRE( ret_fps_n == 30 );
                REQUIRE( ret_fps_d == 1 );
                REQUIRE( dsl_source_is_live(source_name.c_str()) == false );
                boolean ret_repeat_enabled(0);
                REQUIRE( dsl_source_file_repeat_enabled_get(source_name.c_str(), 
                    &ret_repeat_enabled) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_repeat_enabled == repeat_enabled );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "A new File Source is created with a File Path" ) 
        {
            REQUIRE( dsl_source_file_new(source_name.c_str(), 
                NULL, repeat_enabled) == DSL_RESULT_SUCCESS );

            THEN( "The correct attribute values are returned" ) 
            {
                const wchar_t* pRetFilePath; 
                std::wstring empty_file_path;
                REQUIRE( dsl_source_file_file_path_get(source_name.c_str(), 
                    &pRetFilePath) == DSL_RESULT_SUCCESS );
                std::wstring ret_file_path(pRetFilePath);
                REQUIRE( ret_file_path == empty_file_path );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A File Source Component can Set/Get its Repeat Enabled setting", "[source-api]" )
{
    GIVEN( "A new File Source" )
    {
        REQUIRE( dsl_source_file_new(source_name.c_str(), 
            uri.c_str(), false) == DSL_RESULT_SUCCESS );

        boolean retRepeatEnabled(true);
        REQUIRE( dsl_source_file_repeat_enabled_get(source_name.c_str(), 
            &retRepeatEnabled) == DSL_RESULT_SUCCESS );
        REQUIRE( retRepeatEnabled == false );

        WHEN( "The Source's Repeat Enabled setting is set" ) 
        {
            REQUIRE( dsl_source_file_repeat_enabled_set(source_name.c_str(), 
                true) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" )
            {
                REQUIRE( dsl_source_file_repeat_enabled_get(source_name.c_str(), 
                    &retRepeatEnabled) == DSL_RESULT_SUCCESS );
                REQUIRE( retRepeatEnabled == true );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "A Multi-Image Source returns the correct attribute values", "[source-api]" )
{
    GIVEN( "Attributes for a new Multi Image Source" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new Multi Image Source is created" ) 
        {
            REQUIRE( dsl_source_image_multi_new(source_name.c_str(), 
                jpeg_file_path_multi.c_str(), fps_n, fps_d) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                boolean loop_enabled(true);

                REQUIRE( dsl_source_image_multi_loop_enabled_get(source_name.c_str(), 
                    &loop_enabled) == DSL_RESULT_SUCCESS );
                REQUIRE( loop_enabled == false );

                int start_index(99), stop_index(99);
                REQUIRE( dsl_source_image_multi_indices_get(source_name.c_str(), 
                    &start_index, &stop_index) == DSL_RESULT_SUCCESS );
                REQUIRE( start_index == 0 );
                REQUIRE( stop_index == -1 );
                
                REQUIRE( dsl_source_is_live(source_name.c_str()) == false );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A Multi Image Source Component can Set/Get its settings correctly", "[source-api]" )
{
    GIVEN( "A new Multi-Image Source" )
    {
        REQUIRE( dsl_source_image_multi_new(source_name.c_str(), 
            jpeg_file_path_multi.c_str(), fps_n, fps_d) == DSL_RESULT_SUCCESS );

        WHEN( "The Source's loop-enabled setting is set" ) 
        {
            boolean new_loop_enabled(true);
            REQUIRE( dsl_source_image_multi_loop_enabled_set(source_name.c_str(), 
                new_loop_enabled) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" )
            {
                boolean ret_loop_enabled(false);

                REQUIRE( dsl_source_image_multi_loop_enabled_get(source_name.c_str(), 
                    &ret_loop_enabled) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_loop_enabled == new_loop_enabled );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
        WHEN( "The Source's start and stop index setting is set" ) 
        {
            int new_start_index(4), new_stop_index(5);
            REQUIRE( dsl_source_image_multi_indices_set(source_name.c_str(), 
                new_start_index, new_stop_index) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" )
            {
                int ret_start_index(99), ret_stop_index(99);
                REQUIRE( dsl_source_image_multi_indices_get(source_name.c_str(), 
                    &ret_start_index, &ret_stop_index) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_start_index == new_start_index );
                REQUIRE( ret_stop_index == new_stop_index );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "A new Image Stream Source returns the correct attribute values", "[source-api]" )
{
    GIVEN( "Attributes for a new Image Source" ) 
    {
        uint actual_width(1280);
        uint actual_height(720);

        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new Image Source is created" ) 
        {
            REQUIRE( dsl_source_image_stream_new(source_name.c_str(), image_path.c_str(),
                is_live, fps_n, fps_d, timeout) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                uint ret_width(0), ret_height(0), ret_fps_n(0), ret_fps_d(0);
                REQUIRE( dsl_source_video_dimensions_get(source_name.c_str(), 
                    &ret_width, &ret_height) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_source_frame_rate_get(source_name.c_str(), 
                    &ret_fps_n, &ret_fps_d) == DSL_RESULT_SUCCESS );
                REQUIRE( ret_width == actual_width );
                REQUIRE( ret_height == actual_height );
                REQUIRE( ret_fps_n == fps_n );
                REQUIRE( ret_fps_d == fps_d );
                REQUIRE( dsl_source_is_live(source_name.c_str()) == false );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A Image Stream Source Component can Set/Get its Display Timeout setting", "[source-api]" )
{
    GIVEN( "A new File Source" )
    {
        REQUIRE( dsl_source_image_stream_new(source_name.c_str(), image_path.c_str(),
            is_live, fps_n, fps_d, timeout) == DSL_RESULT_SUCCESS );

        uint retTimeout(321);
        REQUIRE( dsl_source_image_stream_timeout_get(source_name.c_str(), 
            &retTimeout) == DSL_RESULT_SUCCESS );
        REQUIRE( retTimeout == timeout );

        WHEN( "The Source's Timeout setting is set" ) 
        {
            uint newTimeout(444);
            REQUIRE( dsl_source_image_stream_timeout_set(source_name.c_str(), 
                newTimeout) == DSL_RESULT_SUCCESS );

            THEN( "The correct value is returned on get" )
            {
                REQUIRE( dsl_source_image_stream_timeout_get(source_name.c_str(), 
                    &retTimeout) == DSL_RESULT_SUCCESS );
                REQUIRE( retTimeout == newTimeout );
                    
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}

SCENARIO( "A new Duplicate Source returns the correct attribute values", 
    "[source-api]" )
{
    GIVEN( "A new V4L2 Source as Original Source" ) 
    {
        std::wstring original_source_name(L"original-source");
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(original_source_name.c_str(), 
            L"/dev/video0") == DSL_RESULT_SUCCESS );

        WHEN( "A new Duplicate Source is created" ) 
        {
            REQUIRE( dsl_source_duplicate_new(source_name.c_str(), 
                original_source_name.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                const wchar_t* ret_c_original;
                REQUIRE( dsl_source_duplicate_original_get(source_name.c_str(),
                    &ret_c_original) == DSL_RESULT_SUCCESS );

                std::wstring ret_original = ret_c_original;
                REQUIRE( ret_original == original_source_name );

                REQUIRE( dsl_source_is_live(source_name.c_str()) == TRUE );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "A new Duplicate Source can update it Origian Source correctly", 
    "[next]" )
{
    GIVEN( "Two new USB Sources as Original Sources" ) 
    {
        std::wstring original_source_name1(L"original-source-1");
        std::wstring original_source_name2(L"original-source-2");
        
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_v4l2_new(original_source_name1.c_str(), 
            L"/dev/video0") == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_source_v4l2_new(original_source_name2.c_str(), 
            L"/dev/video1") == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_source_duplicate_new(source_name.c_str(), 
            original_source_name1.c_str()) == DSL_RESULT_SUCCESS );

        const wchar_t* ret_c_original;
        REQUIRE( dsl_source_duplicate_original_get(source_name.c_str(),
            &ret_c_original) == DSL_RESULT_SUCCESS );

        std::wstring ret_original = ret_c_original;
        REQUIRE( ret_original == original_source_name1 );

        WHEN( "A the Duplicate Source's Original Source is updated" ) 
        {
            REQUIRE( dsl_source_duplicate_original_set(source_name.c_str(),
                original_source_name2.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The list size and contents are updated correctly" ) 
            {
                REQUIRE( dsl_source_duplicate_original_get(source_name.c_str(),
                    &ret_c_original) == DSL_RESULT_SUCCESS );

                std::wstring ret_original = ret_c_original;
                REQUIRE( ret_original == original_source_name2 );

                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
            }
        }
    }
}    

SCENARIO( "The Components container is updated correctly on new and delete Custom Source",
    "[source-api]" )
    
{
    GIVEN( "An empty list of Components" ) 
    {
        std::wstring source_name = L"custom-source";
        std::wstring element_name_1 = L"element-1";
        std::wstring element_name_2 = L"element-2";
        std::wstring factory_name_1 = L"videotestsrc";
        std::wstring factory_name_2 = L"capsfilter";

        boolean is_live(false);


        REQUIRE( dsl_component_list_size() == 0 );

        WHEN( "A new Custom Source is created" ) 
        {
            REQUIRE( dsl_source_custom_new(source_name.c_str(), is_live) == DSL_RESULT_SUCCESS );

            THEN( "The list size is updated correctly" ) 
            {
                REQUIRE( dsl_component_list_size() == 1 );

                REQUIRE( dsl_component_delete(source_name.c_str()) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
        WHEN( "A new Custom Source is created with multiple Elements" ) 
        {
            REQUIRE( dsl_gst_element_new(element_name_2.c_str(),
                factory_name_2.c_str()) == DSL_RESULT_SUCCESS );


            REQUIRE( dsl_source_custom_new_element_add(source_name.c_str(), is_live,
                element_name_2.c_str()) == DSL_RESULT_SUCCESS );

            THEN( "The list size is updated correctly" ) 
            {
                REQUIRE( dsl_component_list_size() == 1 );

                REQUIRE( dsl_component_delete(source_name.c_str()) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_gst_element_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
        WHEN( "A new Custom Source is created with multiple Elements" ) 
        {
            REQUIRE( dsl_gst_element_new(element_name_1.c_str(),
                factory_name_1.c_str()) == DSL_RESULT_SUCCESS );
            REQUIRE( dsl_gst_element_new(element_name_2.c_str(),
                factory_name_2.c_str()) == DSL_RESULT_SUCCESS );

            const wchar_t* elements[] = {element_name_1.c_str(), 
                element_name_2.c_str(), NULL};
            
            REQUIRE( dsl_source_custom_new_element_add_many(source_name.c_str(), is_live,
                elements) == DSL_RESULT_SUCCESS );

            THEN( "The list size is updated correctly" ) 
            {
                REQUIRE( dsl_component_list_size() == 1 );

                REQUIRE( dsl_component_delete(source_name.c_str()) == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_gst_element_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}    

SCENARIO( "The Source API checks for NULL input parameters", "[source-api]" )
{
    GIVEN( "An empty list of Components" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );
        
        int start_index(0);
        uint left(0), top(0), width(0), fps_n(0), fps_d(0);
        int brightness(0), contrast(0);
        dsl_source_app_need_data_handler_cb data_handler_cb;

        WHEN( "When NULL pointers are used as input" ) 
        {
            THEN( "The API returns DSL_RESULT_INVALID_INPUT_PARAM in all cases" ) 
            {
                REQUIRE( dsl_source_app_new(NULL, 
                    0, 0, 0, 0, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_data_handlers_add(NULL, 
                    NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_data_handlers_add(source_name.c_str(), 
                    NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_data_handlers_add(source_name.c_str(), 
                    data_handler_cb, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_data_handlers_remove(NULL) ==
                    DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_buffer_push(NULL, NULL) ==
                    DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_buffer_push(source_name.c_str(), NULL) ==
                    DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_eos(NULL) ==
                    DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_block_enabled_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_block_enabled_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_block_enabled_set(NULL,
                    0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_current_level_bytes_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_current_level_bytes_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_max_level_bytes_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_max_level_bytes_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_max_level_bytes_set(NULL,
                    0) == DSL_RESULT_INVALID_INPUT_PARAM );
                    
                REQUIRE( dsl_source_custom_new(NULL, false) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_new_element_add(
                    NULL, false, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_new_element_add(
                    source_name.c_str(), false, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_new_element_add_many(
                    NULL, false, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_new_element_add_many(
                    source_name.c_str(), false, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_add(
                    NULL, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_add(
                    source_name.c_str(), NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_add_many(NULL, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_add_many(
                    source_name.c_str(), NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_remove(
                    NULL, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_remove(
                    source_name.c_str(), NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_remove_many(
                    NULL, NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_custom_element_remove_many(
                    source_name.c_str(), NULL)
                    == DSL_RESULT_INVALID_INPUT_PARAM );
                
                    
                REQUIRE( dsl_source_csi_new(NULL, 0, 0, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_csi_sensor_id_get(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_csi_sensor_id_get(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_csi_sensor_id_set(NULL, 0) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_v4l2_new(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_new(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_device_location_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_device_location_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_device_location_set(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_device_location_set(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_picture_settings_get(NULL, 
                    NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_picture_settings_get(source_name.c_str(), 
                    NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_picture_settings_get(source_name.c_str(), 
                    &brightness, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_v4l2_picture_settings_get(source_name.c_str(), 
                    &brightness, &contrast, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                    
                REQUIRE( dsl_source_uri_new(NULL, NULL, false, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_uri_new(source_name.c_str(), NULL, false, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_new(NULL, NULL, 0, 0, 0, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_new(source_name.c_str(), NULL, 0, 0, 0, 0, 0) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_file_new(NULL, NULL, false) == DSL_RESULT_INVALID_INPUT_PARAM );
                // Note NULL file_path is valid for File and Image Sources

                REQUIRE( dsl_source_video_dimensions_get(NULL, &width, &height) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_frame_rate_get(NULL, &fps_n, &fps_d) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_uri_uri_get(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_uri_uri_get(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_uri_uri_set(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_uri_uri_set(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_dewarper_add(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_dewarper_add(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_dewarper_remove(NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_rtsp_tls_validation_flags_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_tls_validation_flags_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_tls_validation_flags_set(NULL,
                    0) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_rtsp_tap_add(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_tap_add(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_rtsp_tap_remove(NULL) 
                    == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_image_multi_new(NULL, 
                    NULL, fps_n, fps_d) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_new(source_name.c_str(), 
                    NULL, fps_n, fps_d) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_loop_enabled_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_loop_enabled_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_loop_enabled_set(NULL, 
                    false) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_indices_get(NULL, 
                    NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_image_multi_indices_get(source_name.c_str(), 
                    &start_index, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_pause(NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_resume(NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                
                REQUIRE( dsl_source_pph_add(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_pph_add(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_pph_remove(NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_pph_remove(source_name.c_str(), NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_app_do_timestamp_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_do_timestamp_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_app_do_timestamp_set(NULL,
                    0) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_media_type_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_media_type_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_buffer_out_format_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_format_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_format_set(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_format_set(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_buffer_out_dimensions_get(NULL, 
                    NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_dimensions_get(source_name.c_str(), 
                    NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_dimensions_get(source_name.c_str(), 
                    &width, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_dimensions_set(NULL, 
                    1, 1) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_buffer_out_frame_rate_get(NULL, 
                    NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_frame_rate_get(source_name.c_str(), 
                    NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_frame_rate_get(source_name.c_str(), 
                    &fps_n, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_frame_rate_set(NULL, 
                    1, 1) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(NULL, 
                    0, NULL, NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(source_name.c_str(), 
                    0, NULL, NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(source_name.c_str(), 
                    0, &left, NULL, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(source_name.c_str(), 
                    0, &left, &top, NULL, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_get(source_name.c_str(), 
                    0, &left, &top, &width, NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_crop_rectangle_set(NULL, 
                    1, 1, 1, 1, 1) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_video_buffer_out_orientation_get(NULL, 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_orientation_get(source_name.c_str(), 
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_video_buffer_out_orientation_set(NULL, 
                    1) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_duplicate_new(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_duplicate_new(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_duplicate_original_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_duplicate_original_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_duplicate_original_set(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_duplicate_original_set(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_source_name_get(0,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_unique_id_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_unique_id_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_stream_id_get(NULL,
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );
                REQUIRE( dsl_source_stream_id_get(source_name.c_str(),
                    NULL) == DSL_RESULT_INVALID_INPUT_PARAM );

                REQUIRE( dsl_component_list_size() == 0 );
            }
        }
    }
}
