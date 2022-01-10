/*
The MIT License

Copyright (c) 2019-2021, Prominence AI, Inc.

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
#include "Dsl.h"
#include "DslApi.h"

// ---------------------------------------------------------------------------
// Shared Test Inputs 

static const std::wstring source_name(L"uri-source");
static const std::wstring uri(L"./test/streams/sample_1080p_h264.mp4");
static const uint intr_decode(false);
static const uint drop_frame_interval(0);

static const std::wstring primary_gie_name(L"primary-gie");
static const std::wstring infer_config_file(L"./test/configs/config_infer_primary_nano.txt");
static const std::wstring model_engine_file(L"./test/models/Primary_Detector_Nano/resnet10.caffemodel_b8_gpu0_fp16.engine");
        
static const std::wstring tracker_name(L"ktl-tracker");
static const uint tracker_width(480);
static const uint tracker_height(272);

static const std::wstring tiler_name(L"tiler");
static const uint width(1280);
static const uint height(720);
        
static const std::wstring osd_name(L"osd");
static const boolean text_enabled(true);
static const boolean clock_enabled(false);
static const boolean bbox_enabled(true);
static const boolean mask_enabled(false);
        
static const std::wstring ode_pph_name(L"ode-handler");

static const std::wstring window_sink_name(L"window-sink");
static const uint offsetX(100);
static const uint offsetY(140);
static const uint sinkW(1280);
static const uint sinkH(720);

static const std::wstring pipeline_name(L"test-pipeline");

static const std::wstring ode_trigger_name(L"occurrence");
static const uint class_id(0);
static const uint limit_10(10);

static const std::wstring ode_action_name(L"print");

static const std::wstring vehicle_occurrence_name(L"vehicle-occurence");
static const std::wstring first_vehicle_occurrence_name(L"first-vehicle-occurrence");
static const std::wstring vehicle_summation_name(L"vehicle-summation");
static const std::wstring vehicle_accumulation_name(L"vehicle-accumulation");
static const uint vehicle_class_id(0);
static const std::wstring bicycle_occurrence_name(L"Bicycle");
static const std::wstring first_bycle_occurrence_name(L"first-bicycle-occurrence");
static const std::wstring bicycle_summation_name(L"bicycle-summation");
static const std::wstring bicycle_accumulation_name(L"bicycle-accumulation");
static const uint bicycle_class_id(1);
static const std::wstring person_occurrence_name(L"person-occurrence");
static const std::wstring first_person_occurrence_name(L"first-person-occurrence");
static const std::wstring person_summation_name(L"person-summation");
static const std::wstring person_accumulation_name(L"person-accumulation");
static const uint person_class_id(2);
static const std::wstring roadsign_occurrence_name(L"roadsign-occurrence");
static const std::wstring first_roadsign_occurrence_name(L"first-roadsign-occurrence");
static const std::wstring roadsign_summation_name(L"roadsign-summation");
static const std::wstring roadsign_accumulation_name(L"roadsign-accumulation");
static const uint roadsign_class_id(3);

std::wstring vehicle_string(L"Vehicle count: %6");
std::wstring bycle_string(L"Bycle count: %6");
std::wstring person_string(L"Person count: %6");
std::wstring roadsign_string(L"Roadsign count: %6");

std::wstring vehicle_display_action(L"vehicle-display-action");
std::wstring bycle_display_action(L"bycle-display-action");
std::wstring person_display_action(L"person-display-action");
std::wstring roadsign_display_action(L"roadsign-display-action");

static const std::wstring light_red(L"light-red");
static const std::wstring full_white(L"full-white");
static const std::wstring light_white(L"light-white");
static const std::wstring full_black(L"full-black");

static const std::wstring font(L"arial");
static const std::wstring font_name(L"arial-14");
static const uint size(14);


#define TIME_TO_SLEEP_FOR std::chrono::milliseconds(2000)

// ---------------------------------------------------------------------------

SCENARIO( "A new Pipeline with an ODE Handler without any child ODE Triggers can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, URI source, KTL Tracker, Primary GIE, Tiled Display, ODE Hander, and Overlay Sink" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", L"primary-gie", L"ktl-tracker", L"tiler", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Occurrence ODE Trigger, and Print ODE Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Print ODE Action" ) 
    {
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_occurrence_new(ode_trigger_name.c_str(), NULL, class_id, limit_10) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_action_print_new(ode_action_name.c_str(), false) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(ode_trigger_name.c_str(), ode_action_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), ode_trigger_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", L"primary-gie", L"ktl-tracker", L"tiler", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Two Occurrence ODE Triggers, each with Redact ODE Actions can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Print ODE Action" ) 
    {

        std::wstring odeRedactActionName(L"redact");
        
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(vehicle_occurrence_name.c_str(), 
            NULL, vehicle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_occurrence_new(person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );

        // shared redaction action
        REQUIRE( dsl_ode_action_redact_new(odeRedactActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_action_add(vehicle_occurrence_name.c_str(), odeRedactActionName.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(person_occurrence_name.c_str(), odeRedactActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), vehicle_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), components) == DSL_RESULT_SUCCESS );

            THEN( "The Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Two Occurrence ODE Triggers sharing a Capture ODE Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Capture ODE Action" ) 
    {
        std::wstring captureActionName(L"capture-action");
        std::wstring outdir(L"./");

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), 
            infer_config_file.c_str(), model_engine_file.c_str(), 
            0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), 
            width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(first_vehicle_occurrence_name.c_str(), 
            NULL, vehicle_class_id, DSL_ODE_TRIGGER_LIMIT_ONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_occurrence_new(first_person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_ONE) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_capture_object_new(captureActionName.c_str(), 
            outdir.c_str()) == DSL_RESULT_SUCCESS );
        
        // Add the same capture Action to both ODE Triggers
        REQUIRE( dsl_ode_trigger_action_add(first_vehicle_occurrence_name.c_str(), 
            captureActionName.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(first_person_occurrence_name.c_str(), 
            captureActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            first_vehicle_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            first_person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", L"primary-gie", L"ktl-tracker", L"tiler", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

//SCENARIO( "A new Pipeline with an ODE Handler, an Occurrence ODE Trigger, with a Pause Pipeline ODE Action can play", "[ode-behavior]" )
//{
//    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Capture ODE Action" ) 
//    {
//        std::wstring pauseActionName(L"pause-action");
//        
//        REQUIRE( dsl_component_list_size() == 0 );
//
//        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
//            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );
//
//        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
//            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
//            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );
//
//        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), ode_pph_name.c_str(), 
//            DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_ode_trigger_occurrence_new(first_person_occurrence_name.c_str(), 
//            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_ONE) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_ode_action_pause_new(pauseActionName.c_str(), 
//            pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_ode_trigger_action_add(first_person_occurrence_name.c_str(), 
//            pauseActionName.c_str()) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
//            first_person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
//        
//        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
//            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );
//
//        const wchar_t* components[] = {L"uri-source", 
//            L"primary-gie", L"ktl-tracker", L"tiler", L"window-sink", NULL};
//        
//        WHEN( "When the Pipeline is Assembled" ) 
//        {
//            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
//        
//            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
//                components) == DSL_RESULT_SUCCESS );
//
//            THEN( "Pipeline is Able to LinkAll and Play" )
//            {
//                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
//                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
//                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
//
//                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
//                REQUIRE( dsl_pipeline_list_size() == 0 );
//                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
//                REQUIRE( dsl_component_list_size() == 0 );
//                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
//                REQUIRE( dsl_pph_list_size() == 0 );
//                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
//                REQUIRE( dsl_ode_trigger_list_size() == 0 );
//                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
//                REQUIRE( dsl_ode_action_list_size() == 0 );
//            }
//        }
//    }
//}

SCENARIO( "A new Pipeline with an ODE Handler, Four Occurrence ODE Triggers, each with ODE Display Actions can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Display ODE Actions" ) 
    {
        uint textOffsetX(10);
        uint textOffsetY(20);
        
        REQUIRE( dsl_display_type_rgba_color_new(full_black.c_str(), 
            0.0, 0.0, 0.0, 1.0) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_font_new(font_name.c_str(), font.c_str(),
            size, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), tracker_width, tracker_height) 
            == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), ode_pph_name.c_str(), 
            DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
        
        // Display actions, one per class.
        REQUIRE( dsl_ode_action_display_new(vehicle_display_action.c_str(), 
            vehicle_string.c_str(), textOffsetX, textOffsetY, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_display_new(bycle_display_action.c_str(), 
            bycle_string.c_str(), textOffsetX, textOffsetY+20, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_display_new(person_display_action.c_str(), 
            person_string.c_str(), textOffsetX, textOffsetY+40, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_display_new(roadsign_display_action.c_str(), 
            roadsign_string.c_str(), textOffsetX, textOffsetY+60, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        // Create all Summation triggers and add their repsective Display Action
        REQUIRE( dsl_ode_trigger_summation_new(vehicle_summation_name.c_str(), 
            NULL, vehicle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(vehicle_summation_name.c_str(), 
            vehicle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_summation_new(bicycle_summation_name.c_str(), 
            NULL, bicycle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(bicycle_summation_name.c_str(), 
            bycle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_summation_new(person_summation_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(person_summation_name.c_str(), 
            person_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_summation_new(roadsign_summation_name.c_str(), 
            NULL, roadsign_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(roadsign_summation_name.c_str(), 
            roadsign_display_action.c_str()) == DSL_RESULT_SUCCESS );

        const wchar_t* odeTypes[] = {L"vehicle-summation", L"bicycle-summation", 
            L"person-summation", L"roadsign-summation", NULL};
        
        REQUIRE( dsl_pph_ode_trigger_add_many(ode_pph_name.c_str(), odeTypes) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Four ODE Accumulation Triggers with ODE Display ActionS can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Four Aggreation ODE Triggers each with a Display ODE Actions" ) 
    {
        
        std::wstring displayActionName(L"display-action");
        uint textOffsetX(10);
        uint textOffsetY(20);

        REQUIRE( dsl_display_type_rgba_color_new(full_black.c_str(), 
            0.0, 0.0, 0.0, 1.0) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_font_new(font_name.c_str(), font.c_str(),
            size, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );

        
        REQUIRE( dsl_ode_action_display_new(vehicle_display_action.c_str(), 
            vehicle_string.c_str(), textOffsetX, textOffsetY, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_display_new(bycle_display_action.c_str(), 
            bycle_string.c_str(), textOffsetX, textOffsetY+20, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_display_new(person_display_action.c_str(), 
            person_string.c_str(), textOffsetX, textOffsetY+40, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_display_new(roadsign_display_action.c_str(), 
            roadsign_string.c_str(), textOffsetX, textOffsetY+60, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        // Create all occurrences
        REQUIRE( dsl_ode_trigger_accumulation_new(vehicle_accumulation_name.c_str(), 
            NULL, vehicle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(vehicle_accumulation_name.c_str(), 
            vehicle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_accumulation_new(bicycle_accumulation_name.c_str(), 
            NULL, bicycle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(bicycle_accumulation_name.c_str(), 
            bycle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_accumulation_new(person_accumulation_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(person_accumulation_name.c_str(), 
            person_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_accumulation_new(roadsign_accumulation_name.c_str(), 
            NULL, roadsign_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(roadsign_accumulation_name.c_str(), 
            roadsign_display_action.c_str()) == DSL_RESULT_SUCCESS );

        const wchar_t* odeTypes[] = {L"vehicle-accumulation", L"bicycle-accumulation", 
            L"person-accumulation", L"roadsign-accumulation", NULL};
        
        REQUIRE( dsl_pph_ode_trigger_add_many(ode_pph_name.c_str(), 
            odeTypes) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Four Summation ODE Triggers with shared ODE Inclusion Area", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Four Summation ODE Triggers, and Display ODE Action" ) 
    {
        
        std::wstring displayActionName(L"display-action");
        uint textOffsetX(10);
        uint textOffsetY(20);
        
        std::wstring printActionName(L"print-action");
        std::wstring fillActionName(L"fill-action");
        
        REQUIRE( dsl_display_type_rgba_color_new(light_red.c_str(), 
            0.2, 0.0, 0.0, 0.5) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(full_white.c_str(), 
            1.0, 1.0, 1.0, 1.0) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(light_white.c_str(), 
            1.0, 1.0, 1.0, 0.25) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(full_black.c_str(), 
            1.0, 1.0, 1.0, 1.0) == DSL_RESULT_SUCCESS );

        std::wstring font(L"arial");
        std::wstring font_name(L"arial-14");
        uint size(14);

        std::wstring areaName(L"area");
        std::wstring polygonName(L"polygon");
        uint border_width(3);

        dsl_coordinate coordinates[4] = {{365,600},{980,620},{1000, 770},{180,750}};
        uint num_coordinates(4);

        std::wstring ode_pph_name(L"ode-handler");

        REQUIRE( dsl_display_type_rgba_font_new(font_name.c_str(), font.c_str(),
            size, full_white.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        // Set Area critera, and The fill action for ODE occurrence caused by overlap
        REQUIRE( dsl_ode_trigger_occurrence_new(person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_format_bbox_new(fillActionName.c_str(), 
            0, NULL, true, light_red.c_str()) == DSL_RESULT_SUCCESS );
            
        REQUIRE( dsl_ode_trigger_action_add(person_occurrence_name.c_str(), 
            fillActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        // Create a new ODE Area for criteria
		REQUIRE( dsl_display_type_rgba_polygon_new(polygonName.c_str(), 
            coordinates, num_coordinates, 
			border_width, light_white.c_str())== DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_area_inclusion_new(areaName.c_str(), 
            polygonName.c_str(), true, DSL_BBOX_POINT_ANY) == DSL_RESULT_SUCCESS );

        // Display actions, one per class.
        REQUIRE( dsl_ode_action_display_new(vehicle_display_action.c_str(), 
            vehicle_string.c_str(), textOffsetX, textOffsetY, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_display_new(bycle_display_action.c_str(), 
            bycle_string.c_str(), textOffsetX, textOffsetY+20, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_display_new(person_display_action.c_str(), 
            person_string.c_str(), textOffsetX, textOffsetY+40, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_action_display_new(roadsign_display_action.c_str(), 
            roadsign_string.c_str(), textOffsetX, textOffsetY+60, font_name.c_str(), 
            false, full_black.c_str()) == DSL_RESULT_SUCCESS );
        
        // Create all Summation triggers and add their repsective Display Actions
        // and the shared plygon area
        REQUIRE( dsl_ode_trigger_summation_new(vehicle_summation_name.c_str(), 
            NULL, vehicle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(vehicle_summation_name.c_str(), 
            vehicle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_area_add(vehicle_summation_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_summation_new(bicycle_summation_name.c_str(), 
            NULL, bicycle_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(bicycle_summation_name.c_str(), 
            bycle_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_area_add(bicycle_summation_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_summation_new(person_summation_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(person_summation_name.c_str(), 
            person_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_area_add(person_summation_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );
            
        REQUIRE( dsl_ode_trigger_summation_new(roadsign_summation_name.c_str(), 
            NULL, roadsign_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(roadsign_summation_name.c_str(), 
            roadsign_display_action.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_area_add(roadsign_summation_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        const wchar_t* odeTypes[] = {L"vehicle-summation", L"bicycle-summation", 
            L"person-summation", L"roadsign-summation", NULL};

        REQUIRE( dsl_pph_ode_trigger_add_many(ode_pph_name.c_str(), odeTypes) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_osd_pph_add(osd_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SINK) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_ode_area_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_area_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Occurrence ODE Trigger, Start Record ODE Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Four Summation ODE Triggers, and Display ODE Action" ) 
    {
        std::wstring recordSinkName(L"record-sink");
        std::wstring outdir(L"./");
        uint codec(DSL_CODEC_H265);
        uint bitrate(2000000);
        uint interval(0);

        uint container(DSL_CONTAINER_MKV);
        
        std::wstring recordActionName(L"start-record-action");
        std::wstring printActionName(L"print-action");
        
        std::wstring ode_pph_name(L"ode-handler");
        
        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), 
            width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_sink_record_new(recordSinkName.c_str(), outdir.c_str(),
            codec, container, bitrate, interval, NULL) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_print_new(printActionName.c_str(), 
            false) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_action_sink_record_start_new(recordActionName.c_str(), 
            recordSinkName.c_str(), 2, 5, NULL) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(bicycle_occurrence_name.c_str(), 
            NULL, bicycle_class_id, DSL_ODE_TRIGGER_LIMIT_ONE) == DSL_RESULT_SUCCESS );
        
        const wchar_t* actions[] = {L"start-record-action", L"print-action", NULL};

        REQUIRE( dsl_ode_trigger_action_add_many(bicycle_occurrence_name.c_str(), 
            actions) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            bicycle_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", L"primary-gie", L"ktl-tracker", 
            L"tiler", L"osd", L"window-sink", L"record-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_ode_area_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_area_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an Occurrence ODE Trigger using an ODE Line Area can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, Line ODE Area, and Fill ODE Action" ) 
    {

        std::wstring odeFillActionName(L"fill-action");

        std::wstring ode_pph_name(L"ode-handler");
        
        std::wstring lineName(L"line");
        uint x1(300), y1(600), x2(600), y2(620);
        uint line_width(5);

        std::wstring colorName(L"opaque-white");
        double red(1.0), green(1.0), blue(1.0), alpha(1.0);
        
        std::wstring areaName  = L"line-area";

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), 
            width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SINK) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(colorName.c_str(), 
            red, green, blue, alpha) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_line_new(lineName.c_str(), 
            x1, y1, x2, y2, line_width, colorName.c_str())== DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_area_line_new(areaName.c_str(), lineName.c_str(), 
            true, DSL_BBOX_EDGE_BOTTOM) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_area_add(person_occurrence_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_format_bbox_new(odeFillActionName.c_str(), 
            0, NULL, true, colorName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_action_add(person_occurrence_name.c_str(), 
            odeFillActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(),
            person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "The Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an Occurrence ODE Trigger using an ODE Inclussion Area can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, Inclusion ODE Area, and Fill ODE Action" ) 
    {

        std::wstring odeFillActionName(L"fill-action");
        
        std::wstring polygonName(L"polygon");
        uint border_width(3);

        dsl_coordinate coordinates[4] = {{365,600},{580,620},{600, 770},{180,750}};
        uint num_coordinates(4);

        std::wstring opaqueWhite(L"opaque-white");
        double red(1.0), green(1.0), blue(1.0), alpha(1.0);
        
        std::wstring areaName  = L"inclusion-area";

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), 
            width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SINK) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(opaqueWhite.c_str(), 
            red, green, blue, alpha) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_polygon_new(polygonName.c_str(), 
            coordinates, num_coordinates, 
            border_width, opaqueWhite.c_str())== DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_area_inclusion_new(areaName.c_str(), polygonName.c_str(), 
            true, DSL_BBOX_POINT_SOUTH) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_area_add(person_occurrence_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_format_bbox_new(odeFillActionName.c_str(), 
            0, NULL, true, opaqueWhite.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_action_add(person_occurrence_name.c_str(), 
            odeFillActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "The Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_ode_area_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_area_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an Occurrence ODE Trigger using an ODE Exclusion Area can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, Inclusion ODE Area, and Fill ODE Action" ) 
    {
        std::wstring odeFillActionName(L"fill-action");
        
        std::wstring polygonName(L"polygon");
        uint border_width(3);

        dsl_coordinate coordinates[4] = {{365,600},{580,620},{600, 770},{180,750}};
        uint num_coordinates(4);

        std::wstring opaqueWhite(L"opaque-white");
        double red(1.0), green(1.0), blue(1.0), alpha(1.0);

        std::wstring areaName  = L"exclusion-area";

        REQUIRE( dsl_component_list_size() == 0 );

        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), infer_config_file.c_str(), 
            model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), 
            width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SINK) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(opaqueWhite.c_str(), 
            red, green, blue, alpha) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_polygon_new(polygonName.c_str(), 
            coordinates, num_coordinates, 
            border_width, opaqueWhite.c_str())== DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_area_exclusion_new(areaName.c_str(), polygonName.c_str(), 
            true, DSL_BBOX_POINT_SOUTH) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_occurrence_new(person_occurrence_name.c_str(), 
            NULL, person_class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_area_add(person_occurrence_name.c_str(), 
            areaName.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_format_bbox_new(odeFillActionName.c_str(), 
            0, NULL, true, opaqueWhite.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_ode_trigger_action_add(person_occurrence_name.c_str(), 
            odeFillActionName.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            person_occurrence_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "The Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
                REQUIRE( dsl_ode_area_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_area_list_size() == 0 );
                REQUIRE( dsl_display_type_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_display_type_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Occurrence ODE Trigger, and Format BBox ODE Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Format BBox ODE Action" ) 
    {
        std::wstring ode_action_name(L"format-bbox-action");
        uint border_width(8);

        std::wstring border_color_name(L"my-border-color");
        std::wstring bg_color_name(L"my-bg-color");
        
        boolean has_bg_color(true);
        
        REQUIRE( dsl_display_type_rgba_color_new(border_color_name.c_str(), 
            0.12, 0.34, 0.56, 0.78) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(bg_color_name.c_str(), 
            0.78, 0.56, 0.34, 0.43) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), 
            infer_config_file.c_str(), model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_occurrence_new(ode_trigger_name.c_str(), 
            NULL, class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_action_format_bbox_new(ode_action_name.c_str(), border_width, 
            border_color_name.c_str(), has_bg_color, 
            bg_color_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(ode_trigger_name.c_str(), 
            ode_action_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            ode_trigger_name.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Occurrence ODE Trigger, and Format Label Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Format Label Action" ) 
    {
        std::wstring ode_action_name(L"format-label-action");

        std::wstring font_name(L"font-name");
        std::wstring font(L"arial");
        uint size(14);

        std::wstring font_color_name(L"font-color");
        std::wstring font_bg_color_name(L"font-bg-color");

        double redFont(0.0), greenFont(0.0), blueFont(0.0), alphaFont(1.0);
        double redBgColor(0.12), greenBgColor(0.34), blueBgColor(0.56), alphaBgColor(0.78);
        
        boolean has_bg_color(true);
        
        REQUIRE( dsl_display_type_rgba_color_new(font_color_name.c_str(), 
            redFont, greenFont, blueFont, alphaFont) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_display_type_rgba_color_new(font_bg_color_name.c_str(), 
            redBgColor, greenBgColor, blueBgColor, alphaBgColor) == DSL_RESULT_SUCCESS );
            
        REQUIRE( dsl_display_type_rgba_font_new(font_name.c_str(), 
            font.c_str(), size, font_color_name.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_action_format_label_new(ode_action_name.c_str(),  
            font_name.c_str(), has_bg_color, font_bg_color_name.c_str()) 
                == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), 
            infer_config_file.c_str(), model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_occurrence_new(ode_trigger_name.c_str(), 
            NULL, class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(ode_trigger_name.c_str(), 
            ode_action_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            ode_trigger_name.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}

SCENARIO( "A new Pipeline with an ODE Handler, Occurrence ODE Trigger, and Customize Label Action can play", "[ode-behavior]" )
{
    GIVEN( "A Pipeline, ODE Handler, Occurrence ODE Trigger, and Costomize Label Action" ) 
    {
        std::wstring ode_action_name(L"customize-label-action");
        uint label_types[] = {DSL_METRIC_OBJECT_LOCATION,
            DSL_METRIC_OBJECT_DIMENSIONS, DSL_METRIC_OBJECT_CONFIDENCE,
            DSL_METRIC_OBJECT_PERSISTENCE};

        uint size(4);

        REQUIRE( dsl_ode_action_customize_label_new(ode_action_name.c_str(),  
            label_types, size) 
                == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_source_uri_new(source_name.c_str(), uri.c_str(), 
            false, intr_decode, drop_frame_interval) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_infer_gie_primary_new(primary_gie_name.c_str(), 
            infer_config_file.c_str(), model_engine_file.c_str(), 0) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tracker_ktl_new(tracker_name.c_str(), 
            tracker_width, tracker_height) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_tiler_new(tiler_name.c_str(), width, height) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_pph_ode_new(ode_pph_name.c_str()) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_tiler_pph_add(tiler_name.c_str(), 
            ode_pph_name.c_str(), DSL_PAD_SRC) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_ode_trigger_occurrence_new(ode_trigger_name.c_str(), 
            NULL, class_id, DSL_ODE_TRIGGER_LIMIT_NONE) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_ode_trigger_action_add(ode_trigger_name.c_str(), 
            ode_action_name.c_str()) == DSL_RESULT_SUCCESS );
        REQUIRE( dsl_pph_ode_trigger_add(ode_pph_name.c_str(), 
            ode_trigger_name.c_str()) == DSL_RESULT_SUCCESS );

        REQUIRE( dsl_osd_new(osd_name.c_str(), text_enabled, clock_enabled,
            bbox_enabled, mask_enabled) == DSL_RESULT_SUCCESS );
        
        REQUIRE( dsl_sink_window_new(window_sink_name.c_str(),
            offsetX, offsetY, sinkW, sinkH) == DSL_RESULT_SUCCESS );

        const wchar_t* components[] = {L"uri-source", 
            L"primary-gie", L"ktl-tracker", L"tiler", L"osd", L"window-sink", NULL};
        
        WHEN( "When the Pipeline is Assembled" ) 
        {
            REQUIRE( dsl_pipeline_new(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
        
            REQUIRE( dsl_pipeline_component_add_many(pipeline_name.c_str(), 
                components) == DSL_RESULT_SUCCESS );

            THEN( "Pipeline is Able to LinkAll and Play" )
            {
                REQUIRE( dsl_pipeline_play(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );
                std::this_thread::sleep_for(TIME_TO_SLEEP_FOR);
                REQUIRE( dsl_pipeline_stop(pipeline_name.c_str()) == DSL_RESULT_SUCCESS );

                REQUIRE( dsl_pipeline_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pipeline_list_size() == 0 );
                REQUIRE( dsl_component_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_component_list_size() == 0 );
                REQUIRE( dsl_pph_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_pph_list_size() == 0 );
                REQUIRE( dsl_ode_trigger_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_trigger_list_size() == 0 );
                REQUIRE( dsl_ode_action_delete_all() == DSL_RESULT_SUCCESS );
                REQUIRE( dsl_ode_action_list_size() == 0 );
            }
        }
    }
}
