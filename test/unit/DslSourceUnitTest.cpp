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
#include "DslSinkBintr.h"
#include "DslSourceBintr.h"
#include "DslPipelineSourcesBintr.h"

static std::string sourceName("test-source");
static std::string uri("/opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h265.mp4");
static std::string uri2("/opt/nvidia/deepstream/deepstream/samples/streams/yoga.mp4");
static std::string filePath("/opt/nvidia/deepstream/deepstream/samples/streams/sample_1080p_h265.mp4");
static uint intrDecode(false);
static uint dropFrameInterval(0);

static std::string dewarperName("dewarper");
static std::string defConfigFile(
"/opt/nvidia/deepstream/deepstream/sources/apps/sample_apps/deepstream-dewarper-test/config_dewarper.txt");

static std::string rtspSourceName("rtsp-source");
static std::string rtspUri("rtsp://208.72.70.171:80/mjpg/video.mjpg");
static uint latency(100);
static uint timeout(20);

static std::string jpgFilePath1("/opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.jpg");
static std::string jpgFilePath2("/opt/nvidia/deepstream/deepstream/samples/streams/yoga.jpg");
static std::string multJpgFilePath("./test/streams/sample_720p.%04d.mjpeg");

static uint width(1920), height(1080), fps_n(30), fps_d(1);

static std::wstring L_bufferOutFormat(DSL_VIDEO_FORMAT_DEFAULT);
static std::string defaultBufferOutFormat(L_bufferOutFormat.begin(), 
    L_bufferOutFormat.end());

using namespace DSL;

SCENARIO( "A new AppSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "Attributes for a new AppSourceBintr" ) 
    {
        boolean isLive(true);
        
        WHEN( "The AppSourceBintr is created " )
        {
            DSL_APP_SOURCE_PTR pSourceBintr = DSL_APP_SOURCE_NEW(
                sourceName.c_str(), isLive, "I420", width, height, fps_n, fps_d);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                REQUIRE( pSourceBintr->IsLive() == isLive );
                
                uint retWidth, retHeight, retFpsN, retFpsD;
                pSourceBintr->GetDimensions(&retWidth, &retHeight);
                pSourceBintr->GetFrameRate(&retFpsN, &retFpsD);
                REQUIRE( width == retWidth );
                REQUIRE( height == retHeight );
                REQUIRE( fps_n == retFpsN );
                REQUIRE( fps_d == retFpsD );
                
                REQUIRE( pSourceBintr->GetStreamFormat() == DSL_STREAM_FORMAT_BYTE );
                REQUIRE( pSourceBintr->GetBlockEnabled() == FALSE);
                REQUIRE( pSourceBintr->GetCurrentLevelBytes() == 0);
                REQUIRE( pSourceBintr->GetMaxLevelBytes() == 200000);
                
                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
                
                pSourceBintr->GetBufferOutDimensions(&retWidth, &retHeight);
                REQUIRE( retWidth == 0 );
                REQUIRE( retHeight == 0 );
                REQUIRE( pSourceBintr->GetBufferOutOrientation() == 0);
            }
        }
    }
}

SCENARIO( "An AppSourceBintr can LinkAll and UnlinkAll child Elementrs correctly",
    "[SourceBintr]" )
{
    GIVEN( "A new AppSourceBintr in memory" ) 
    {
        boolean isLive(true);
        
        DSL_APP_SOURCE_PTR pSourceBintr = DSL_APP_SOURCE_NEW(
            sourceName.c_str(), isLive, "I420", width, height, fps_n, fps_d);

        WHEN( "The AppSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            // second call must fail
            REQUIRE( pSourceBintr->LinkAll() == false );

            THEN( "The AppSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A AppSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked AppSourceBintr " ) 
    {
        boolean isLive(true);

        DSL_APP_SOURCE_PTR pSourceBintr = DSL_APP_SOURCE_NEW(
            sourceName.c_str(), isLive, "I420", width, height, fps_n, fps_d);

        pSourceBintr->LinkAll();
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The AppSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The AppSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A new CustomSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A attributes for a new CustomSourceBintr" ) 
    {
        bool isLive(true);

        WHEN( "The CustomSourceBintr is created " )
        {
            DSL_CUSTOM_SOURCE_PTR pSourceBintr = DSL_CUSTOM_SOURCE_NEW(
                sourceName.c_str(), isLive);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                REQUIRE( pSourceBintr->IsLive() == isLive );
                
                uint retWidth(99), retHeight(99), retFpsN(99), retFpsD(99);
                pSourceBintr->GetDimensions(&retWidth, &retHeight);
                pSourceBintr->GetFrameRate(&retFpsN, &retFpsD);

                // should all be 0 unknown. 
                REQUIRE( retWidth == 0 );
                REQUIRE( retHeight == 0 );
                REQUIRE( retFpsN == 0 );
                REQUIRE( retFpsD == 0 );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "A CustomSourceBintr can add and remove a child element",  
    "[SourceBintr]" )
{
    GIVEN( "A new CustomBintr and Elementr" ) 
    {
        bool isLive(true);
        static const std::string elementName("element");

        DSL_CUSTOM_SOURCE_PTR pSourceBintr = DSL_CUSTOM_SOURCE_NEW(
            sourceName.c_str(), isLive);
        DSL_ELEMENT_PTR pGstElementr = DSL_ELEMENT_NEW("videotestsrc", 
            elementName.c_str());
        
        WHEN( "The an Element is added to the CustomBintr" )
        {
            REQUIRE( pSourceBintr->AddChild(pGstElementr) == true );
            
            // The second call must fail
            REQUIRE( pSourceBintr->AddChild(pGstElementr) == false );
            THEN( "The same Element can be removed correctly")
            {
                REQUIRE( pSourceBintr->RemoveChild(pGstElementr) == true );
            
                // The second call must fail
                REQUIRE( pSourceBintr->RemoveChild(pGstElementr) == false );
            }
        }
    }
}

SCENARIO( "A CustomSourceBintr can can link and unlink correctly",  
    "[SourceBintr]" )
{
    GIVEN( "A new CustomSourceBintr with an Elementr" ) 
    {
        bool isLive(true);
        static const std::string elementName1("element-1");
        static const std::string elementName2("element-2");
        static const std::string elementName3("element-3");

        DSL_CUSTOM_SOURCE_PTR pSourceBintr = DSL_CUSTOM_SOURCE_NEW(
            sourceName.c_str(), isLive);
        DSL_ELEMENT_PTR pGstElementr1 = DSL_ELEMENT_NEW("videotestsrc", 
            elementName1.c_str());
        DSL_ELEMENT_PTR pGstElementr2 = DSL_ELEMENT_NEW("capsfilter", 
            elementName2.c_str());
        
        DSL_ELEMENT_PTR pGstElementr3 = DSL_ELEMENT_NEW("identity", 
            elementName3.c_str());
        
        WHEN( "The CustomSourceBintr has no child element" )
        {
             
            THEN( "The CustomSourceBintr will fail to link")
            {
               REQUIRE( pSourceBintr->LinkAll() == false );
            }
        }
        WHEN( "The CustomSourceBintr has a single element" )
        {
            REQUIRE( pSourceBintr->AddChild(pGstElementr1) == true );
            
            THEN( "The CustomSourceBintr can be successfully Linked and unlinked")
            {
                REQUIRE( pSourceBintr->LinkAll() == true );
                pSourceBintr->UnlinkAll();
            }
        }
        WHEN( "The CustomSourceBintr has a multiple elements" )
        {
            REQUIRE( pSourceBintr->AddChild(pGstElementr1) == true );
            REQUIRE( pSourceBintr->AddChild(pGstElementr2) == true );
            REQUIRE( pSourceBintr->AddChild(pGstElementr3) == true );
            
            THEN( "The CustomSourceBintr can be successfully Linked and unlinked")
            {
                REQUIRE( pSourceBintr->LinkAll() == true );
                pSourceBintr->UnlinkAll();
            }
        }
    }
}

SCENARIO( "A new CsiSourceBintr is created correctly",  "[SourceBintr]" )
{
    if (dsl_info_gpu_type_get(0) == DSL_GPU_TYPE_INTEGRATED)
    {
        GIVEN( "A name for a new CsiSourceBintr" ) 
        {
            WHEN( "The CsiSourceBintr is created " )
            {
            
                DSL_CSI_SOURCE_PTR pSourceBintr = DSL_CSI_SOURCE_NEW(
                    sourceName.c_str(), width, height, fps_n, fps_d);

                THEN( "All memeber variables are initialized correctly" )
                {
                    REQUIRE( pSourceBintr->GetGpuId() == 0 );
                    REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                    REQUIRE( pSourceBintr->GetGstObject() != NULL );
                    REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                    REQUIRE( pSourceBintr->GetSensorId() == 0 );
                    REQUIRE( pSourceBintr->IsInUse() == false );
                    REQUIRE( pSourceBintr->IsLive() == true );
                    
                    uint retWidth, retHeight, retFpsN, retFpsD;
                    pSourceBintr->GetDimensions(&retWidth, &retHeight);
                    pSourceBintr->GetFrameRate(&retFpsN, &retFpsD);
                    REQUIRE( width == retWidth );
                    REQUIRE( height == retHeight );
                    REQUIRE( fps_n == retFpsN );
                    REQUIRE( fps_d == retFpsD );

                    std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                    REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
                }
            }
        }
    }
}

SCENARIO( "Unique sensor-ids are managed by CsiSourceBintrs correctly",  "[SourceBintr]" )
{
    if (dsl_info_gpu_type_get(0) == DSL_GPU_TYPE_INTEGRATED)
    {
        GIVEN( "A name for a new CsiSourceBintr" ) 
        {
            std::string sourceName1("test-source-1");
            std::string sourceName2("test-source-2");
            std::string sourceName3("test-source-3");
            
            WHEN( "Three CsiSourceBintrs are created " )
            {
                DSL_CSI_SOURCE_PTR pSourceBintr1 = DSL_CSI_SOURCE_NEW(
                    sourceName1.c_str(), width, height, fps_n, fps_d);

                DSL_CSI_SOURCE_PTR pSourceBintr2 = DSL_CSI_SOURCE_NEW(
                    sourceName2.c_str(), width, height, fps_n, fps_d);

                DSL_CSI_SOURCE_PTR pSourceBintr3 = DSL_CSI_SOURCE_NEW(
                    sourceName3.c_str(), width, height, fps_n, fps_d);

                THEN( "Their sensor-id values are assigned correctly" )
                {
                    REQUIRE( pSourceBintr1->GetSensorId() == 0 );
                    REQUIRE( pSourceBintr2->GetSensorId() == 1 );
                    REQUIRE( pSourceBintr3->GetSensorId() == 2 );
                }
            }
            WHEN( "Three CsiSourceBintrs are created with sernsor id updates" )
            {
                DSL_CSI_SOURCE_PTR pSourceBintr1 = DSL_CSI_SOURCE_NEW(
                    sourceName1.c_str(), width, height, fps_n, fps_d);
                    
                REQUIRE( pSourceBintr1->SetSensorId(1) == true );
                
                DSL_CSI_SOURCE_PTR pSourceBintr2 = DSL_CSI_SOURCE_NEW(
                    sourceName2.c_str(), width, height, fps_n, fps_d);

                DSL_CSI_SOURCE_PTR pSourceBintr3 = DSL_CSI_SOURCE_NEW(
                    sourceName3.c_str(), width, height, fps_n, fps_d);

                THEN( "Their sensor-id values are assigned correctly" )
                {
                    REQUIRE( pSourceBintr1->GetSensorId() == 1 );
                    REQUIRE( pSourceBintr2->GetSensorId() == 0 );
                    REQUIRE( pSourceBintr3->GetSensorId() == 2 );
                }
            }
            WHEN( "A non unique sernsor id is used on set" )
            {
                DSL_CSI_SOURCE_PTR pSourceBintr1 = DSL_CSI_SOURCE_NEW(
                    sourceName1.c_str(), width, height, fps_n, fps_d);
                
                DSL_CSI_SOURCE_PTR pSourceBintr2 = DSL_CSI_SOURCE_NEW(
                    sourceName2.c_str(), width, height, fps_n, fps_d);

                THEN( "The SetSensorId call fails" )
                {
                    REQUIRE( pSourceBintr1->SetSensorId(1) == false );
                }
            }
        }
    }
}

SCENARIO( "A CsiSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    if (dsl_info_gpu_type_get(0) == DSL_GPU_TYPE_INTEGRATED)
    {
        GIVEN( "A new CsiSourceBintr in memory" ) 
        {
            DSL_CSI_SOURCE_PTR pSourceBintr = DSL_CSI_SOURCE_NEW(
                sourceName.c_str(), width, height, fps_n, fps_d);

            WHEN( "The CsiSourceBintr is called to LinkAll" )
            {
                REQUIRE( pSourceBintr->LinkAll() == true );

                THEN( "The CsiSourceBintr IsLinked state is updated correctly" )
                {
                    REQUIRE( pSourceBintr->IsLinked() == true );
                }
            }
        }
    }
}

SCENARIO( "A CsiSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    if (dsl_info_gpu_type_get(0) == DSL_GPU_TYPE_INTEGRATED)
    {
        GIVEN( "A new, linked CsiSourceBintr " ) 
        {
            DSL_CSI_SOURCE_PTR pSourceBintr = DSL_CSI_SOURCE_NEW(
                sourceName.c_str(), width, height, fps_n, fps_d);

            pSourceBintr->LinkAll();
            REQUIRE( pSourceBintr->IsLinked() == true );

            WHEN( "The CsiSourceBintr is called to UnlinkAll" )
            {
                pSourceBintr->UnlinkAll();

                THEN( "The CsiSourceBintr IsLinked state is updated correctly" )
                {
                    REQUIRE( pSourceBintr->IsLinked() == false );
                }
            }
        }
    }
}

SCENARIO( "A new V4l2SourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A name for a new V4l2SourceBintr" ) 
    {

        static std::string defDeviceLocation("/dev/video0");

        WHEN( "The V4l2SourceBintr is created " )
        {
        
            DSL_V4L2_SOURCE_PTR pSourceBintr = DSL_V4L2_SOURCE_NEW(
                sourceName.c_str(), defDeviceLocation.c_str());

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                REQUIRE( pSourceBintr->IsLive() == true );
                
                std::string retDeviceLocaton = pSourceBintr->GetDeviceLocation();
                REQUIRE( retDeviceLocaton == defDeviceLocation );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}


SCENARIO( "A V4l2SourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new V4l2SourceBintr in memory" ) 
    {
        std::string sourceName("v4l2-source");
        static std::string defDeviceLocation("/dev/video0");

        DSL_V4L2_SOURCE_PTR pSourceBintr = DSL_V4L2_SOURCE_NEW(
            sourceName.c_str(), defDeviceLocation.c_str());

        WHEN( "The V4l2SourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The V4l2SourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A V4l2SourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked V4l2SourceBintr " ) 
    {
        std::string sourceName("v4l2-source");
        static std::string defDeviceLocation("/dev/video0");

        DSL_V4L2_SOURCE_PTR pSourceBintr = DSL_V4L2_SOURCE_NEW(
            sourceName.c_str(), defDeviceLocation.c_str());

        pSourceBintr->LinkAll();
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The V4l2SourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The V4l2SourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A V4l2SourceBintr can Get and Set its GPU ID",  "[SourceBintr]" )
{
    GIVEN( "A new V4l2SourceBintr in memory" ) 
    {
        std::string sourceName("v4l2-source");
        static std::string defDeviceLocation("/dev/video0");

        DSL_V4L2_SOURCE_PTR pSourceBintr = DSL_V4L2_SOURCE_NEW(
            sourceName.c_str(), defDeviceLocation.c_str());

        uint GPUID0(0);
        uint GPUID1(1);

        REQUIRE( pSourceBintr->GetGpuId() == GPUID0 );
        
        WHEN( "The V4l2SourceBintr's  GPU ID is set" )
        {
            REQUIRE( pSourceBintr->SetGpuId(GPUID1) == true );

            THEN( "The correct GPU ID is returned on get" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == GPUID1 );
            }
        }
    }
}

SCENARIO( "A new UriSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A name for a new UriSourceBintr" ) 
    {
        char absolutePath[PATH_MAX+1];
        std::string fullUriPath = realpath(uri.c_str(), absolutePath);
        fullUriPath.insert(0, "file:");

        WHEN( "The UriSourceBintr is created " )
        {
            DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
                sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == false );
                
                std::string returnedUri = pSourceBintr->GetUri();
                REQUIRE( returnedUri == fullUriPath );
                
                uint retWidth, retHeight, retFpsN, retFpsD;
                pSourceBintr->GetDimensions(&retWidth, &retHeight);
                pSourceBintr->GetFrameRate(&retFpsN, &retFpsD);
                REQUIRE( retWidth == 1920 );
                REQUIRE( retHeight == 1080 );
                REQUIRE( retFpsN == 30 );
                REQUIRE( retFpsD == 1 );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr in memory" ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        WHEN( "The UriSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The UriSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked UriSourceBintr " ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The UriSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The UriSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can Add a Child DewarperBintr",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr and DewarperBintr in memory" ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        DSL_DEWARPER_PTR pDewarperBintr = 
            DSL_DEWARPER_NEW(dewarperName.c_str(), defConfigFile.c_str(), 0);

        WHEN( "The DewarperBintr is added to UriSourceBintr" )
        {
            REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr) == true );

            THEN( "The UriSourceBintr correctly returns that it has a dewarper" )
            {
                REQUIRE( pSourceBintr->HasDewarperBintr() == true );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can Remove a Child DewarperBintr",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr with a child DewarperBintr" ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        DSL_DEWARPER_PTR pDewarperBintr = 
            DSL_DEWARPER_NEW(dewarperName.c_str(), defConfigFile.c_str(), 0);

        REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr) == true );

        WHEN( "The DewarperBintr is removed from the UriSourceBintr" )
        {
            REQUIRE( pSourceBintr->RemoveDewarperBintr() == true );
            
            THEN( "The UriSourceBintr correctly returns that it does not have a dewarper" )
            {
                REQUIRE( pSourceBintr->HasDewarperBintr() == false );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can ensure a single Child DewarperBintr",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr with a child DewarperBintr" ) 
    {
        std::string dewarperName2("dewarper2");

        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        DSL_DEWARPER_PTR pDewarperBintr1 = 
            DSL_DEWARPER_NEW(dewarperName.c_str(), defConfigFile.c_str(), 0);

        DSL_DEWARPER_PTR pDewarperBintr2 = 
            DSL_DEWARPER_NEW(dewarperName2.c_str(), defConfigFile.c_str(), 0);

        REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr1) == true );

        WHEN( "Adding a second DewarperBintr should fail" )
        {
            REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr2) == false );
            
            THEN( "The UriSourceBintr correctly returns that it does not have a dewarper" )
            {
                REQUIRE( pSourceBintr->HasDewarperBintr() == true );
                REQUIRE( pSourceBintr->RemoveDewarperBintr() == true );
                // removing a second time must fail
                REQUIRE( pSourceBintr->RemoveDewarperBintr() == false );
                REQUIRE( pSourceBintr->HasDewarperBintr() == false );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr with a child DewarperBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr with a child DewarperBintr" ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        DSL_DEWARPER_PTR pDewarperBintr = 
            DSL_DEWARPER_NEW(dewarperName.c_str(), defConfigFile.c_str(), 0);

        REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr) == true );

        WHEN( "The UriSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The UriSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A Linked UriSourceBintr with a child DewarperBintr can UnlinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr with a child DewarperBintr" ) 
    {
        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        DSL_DEWARPER_PTR pDewarperBintr = 
            DSL_DEWARPER_NEW(dewarperName.c_str(), defConfigFile.c_str(), 0);

        REQUIRE( pSourceBintr->AddDewarperBintr(pDewarperBintr) == true );

        REQUIRE( pSourceBintr->LinkAll() == true );

        WHEN( "The UriSourceBintr is called to LinkAll" )
        {
            pSourceBintr->UnlinkAll();
            
            THEN( "The UriSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can Set and Get its URI",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr in memory" ) 
    {
        char absolutePath[PATH_MAX+1];
        std::string fullUriPath = realpath(uri.c_str(), absolutePath);
        fullUriPath.insert(0, "file:");

        DSL_URI_SOURCE_PTR pSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        std::string returnedUri = pSourceBintr->GetUri();
        REQUIRE( returnedUri == fullUriPath );

        WHEN( "The UriSourceBintr's URI is updated " )
        {
            std::string fullUriPath2 = realpath(uri2.c_str(), absolutePath);
            fullUriPath2.insert(0, "file:");
            
            REQUIRE( pSourceBintr->SetUri(uri2.c_str()) == true );
            THEN( "The correct URI is returned on get" )
            {
                std::string returnedUri = pSourceBintr->GetUri();
                REQUIRE( returnedUri == fullUriPath2 );
            }
        }
    }
}

SCENARIO( "A UriSourceBintr can Get and Set its GPU ID",  "[SourceBintr]" )
{
    GIVEN( "A new UriSourceBintr in memory" ) 
    {
        DSL_URI_SOURCE_PTR pUriSourceBintr = DSL_URI_SOURCE_NEW(
            sourceName.c_str(), uri.c_str(), false, intrDecode, dropFrameInterval);

        uint GPUID0(0);
        uint GPUID1(1);

        REQUIRE( pUriSourceBintr->GetGpuId() == GPUID0 );
        
        WHEN( "The UriSourceBintr's  GPU ID is set" )
        {
            REQUIRE( pUriSourceBintr->SetGpuId(GPUID1) == true );

            THEN( "The correct GPU ID is returned on get" )
            {
                REQUIRE( pUriSourceBintr->GetGpuId() == GPUID1 );
            }
        }
    }
}

SCENARIO( "A new RtspSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A name for a new RtspSourceBintr" ) 
    {

        WHEN( "The RtspSourceBintr is created " )
        {
            DSL_RTSP_SOURCE_PTR pSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(), 
                rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, 
                latency, timeout);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == 
                    DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                REQUIRE( pSourceBintr->GetBufferTimeout() == timeout );
                REQUIRE( pSourceBintr->GetLatency() ==  latency );
                REQUIRE( pSourceBintr->GetDropOnLatencyEnabled() == false );
                REQUIRE( pSourceBintr->GetTlsValidationFlags() == 
                    DSL_TLS_CERTIFICATE_VALIDATE_ALL );
                REQUIRE( pSourceBintr->GetUdpBufferSize() == 524288);
                REQUIRE( pSourceBintr->GetCurrentState() == GST_STATE_NULL );
                
                dsl_rtsp_connection_data data{0};
                data.first_connected = 123;
                data.last_connected = 456;
                data.last_disconnected = 456;
                data.count = 654;
                data.is_in_reconnect = true;
                data.retries = 444;
                
                pSourceBintr->GetConnectionData(&data);
                REQUIRE( data.first_connected == 0 );
                REQUIRE( data.last_connected == 0 );
                REQUIRE( data.last_disconnected == 0 );
                REQUIRE( data.count == 0 );
                REQUIRE( data.is_in_reconnect == 0 );
                REQUIRE( data.retries == 0 );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == true );
                
                std::string returnedUri = pSourceBintr->GetUri();
                REQUIRE( returnedUri == rtspUri );
                
                uint retWidth, retHeight, retFpsN, retFpsD;
                pSourceBintr->GetDimensions(&retWidth, &retHeight);
                pSourceBintr->GetFrameRate(&retFpsN, &retFpsD);
                REQUIRE( retWidth == 0 );
                REQUIRE( retHeight == 0 );
                REQUIRE( retFpsN == 0 );
                REQUIRE( retFpsD == 0 );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "A new RtspSourceBintr's attributes can be set/get ",  "[SourceBintr]" )
{
    GIVEN( "A new RtspSourceBintr with a timeout" ) 
    {
        DSL_RTSP_SOURCE_PTR pSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(), 
            rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, latency, timeout);

        WHEN( "The RtspSourceBintr's timeout is set " )
        {
            uint newTimeout(0);
            pSourceBintr->SetBufferTimeout(newTimeout);

            THEN( "The correct value is returned on get" )
            {
                REQUIRE( pSourceBintr->GetBufferTimeout() == newTimeout );
            }
        }
        WHEN( "The RtspSourceBintr's reconnect data are set " )
        {
            time_t newLast(123), last(0);
            dsl_rtsp_connection_data data{0}, newData{0};
            data.first_connected = 123;
            data.last_connected = 456;
            data.last_disconnected = 789;
            data.count = 654;
            data.is_in_reconnect = true;
            data.retries = 444;
            pSourceBintr->_setConnectionData(data);

            THEN( "The correct value is returned on get" )
            {
                pSourceBintr->GetConnectionData(&newData);
                REQUIRE( data.first_connected == newData.first_connected );
                REQUIRE( data.last_connected == newData.last_connected );
                REQUIRE( data.last_disconnected == newData.last_disconnected );
                REQUIRE( data.count == newData.count );
                REQUIRE( data.is_in_reconnect == newData.is_in_reconnect );
                REQUIRE( data.retries == newData.retries );
            }
        }
        WHEN( "The RtspSourceBintr's TLS certificate validation flags are set " )
        {
            uint newTlsValidationFlags(DSL_TLS_CERTIFICATE_BAD_IDENTITY |
                DSL_TLS_CERTIFICATE_NOT_ACTIVATED);
            
            pSourceBintr->SetTlsValidationFlags(newTlsValidationFlags);

            THEN( "The correct value is returned on get" )
            {
                REQUIRE( pSourceBintr->GetTlsValidationFlags() == newTlsValidationFlags );
            }
        }
        WHEN( "The RtspSourceBintr's udp-buffer-size is set " )
        {
            uint newUdpBufferSize(600000);
            
            pSourceBintr->SetUdpBufferSize(newUdpBufferSize);

            THEN( "The correct value is returned on get" )
            {
                REQUIRE( pSourceBintr->GetUdpBufferSize() == newUdpBufferSize );
            }
        }
    }
}

static void source_state_change_listener_cb1(uint prev_state, uint curr_state, void* user_data)
{
    std::cout << "Source state change lister 1 called with prev_state = " 
        << prev_state << " current_state = " << curr_state << "\n";
    *(int*)user_data = 111;
}

static void source_state_change_listener_cb2(uint prev_state, uint curr_state, void* user_data)
{
    std::cout << "Source state change lister 2 called with prev_state = " 
        << prev_state << " current_state = " << curr_state << "\n";
    *(int*)user_data = 222;
}

SCENARIO( "An RtspSourceBintr can add and remove State Change Listeners",  "[SourceBintr]" )
{
    GIVEN( "A new RtspSourceBintr with a timeout" ) 
    {
        DSL_RTSP_SOURCE_PTR pRtspSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(), 
            rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, latency, timeout);
        
        WHEN( "Client Listeners are added" )
        {
            REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb1, NULL) == true );
            REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb2, NULL) == true );

            THEN( "Adding them a second time must fail" )
            {
                REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb1, NULL) == false );
                REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb2, NULL) == false );
            }
        }
        WHEN( "Client Listeners are added" )
        {
            REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb1, NULL) == true );
            REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb2, NULL) == true );

            THEN( "They can be successfully removed" )
            {
                REQUIRE( pRtspSourceBintr->RemoveStateChangeListener(source_state_change_listener_cb1) == true );
                REQUIRE( pRtspSourceBintr->RemoveStateChangeListener(source_state_change_listener_cb2) == true );
                
                // Calling a second time must fail
                REQUIRE( pRtspSourceBintr->RemoveStateChangeListener(source_state_change_listener_cb1) == false );
                REQUIRE( pRtspSourceBintr->RemoveStateChangeListener(source_state_change_listener_cb2) == false );
            }
        }
    }
}
            
SCENARIO( "An RtspSourceBintr calls all State Change Listeners on change of state", "[SourceBintr]" )
{
    GIVEN( "A new RtspSourceBintr with a timeout" ) 
    {
        uint userData1(0), userData2(0);

        DSL_RTSP_SOURCE_PTR pRtspSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(), 
            rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, latency, timeout);

        REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb1, &userData1) == true );
        REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb2, &userData2) == true );
        
        WHEN( "The current state is changed" )
        {
            pRtspSourceBintr->SetCurrentState(GST_STATE_READY);

            THEN( "All client listeners are called on state change" )
            {
                REQUIRE( pRtspSourceBintr->GetCurrentState() == GST_STATE_READY );
                
                // simulate timer callback
                REQUIRE( pRtspSourceBintr->NotifyClientListeners() == FALSE );
                // Callbacks will change user data if called
                REQUIRE( userData1 == 111 );
                REQUIRE( userData2 == 222 );
            }
        }
    }
}

SCENARIO( "An RtspSourceBintr's Stream Management callback behaves correctly", "[SourceBintr]" )
{
    GIVEN( "A new RtspSourceBintr with a timeout" ) 
    {
        uint userData1(0), userData2(0);

        DSL_RTSP_SOURCE_PTR pRtspSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(), 
            rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, latency, timeout);

        REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb1, &userData1) == true );
        REQUIRE( pRtspSourceBintr->AddStateChangeListener(source_state_change_listener_cb2, &userData2) == true );

        std::string pipelineSourcesName = "pipeline-sources";

        DSL_PIPELINE_SOURCES_PTR pPipelineSourcesBintr = 
            DSL_PIPELINE_SOURCES_NEW(pipelineSourcesName.c_str(), 0);
            
        DSL_SOURCE_PTR pSourceBintr = std::dynamic_pointer_cast<SourceBintr>(pRtspSourceBintr);
            
        // Source needs a parent to test reconnect - required for source to call "gst_element_sync_state_with_parent"
        pPipelineSourcesBintr->AddChild(pSourceBintr);
        
        WHEN( "The Source is in reset" )
        {
            dsl_rtsp_connection_data data{0};
            data.is_in_reconnect = true;
            data.retries = 1;
            pRtspSourceBintr->_setConnectionData(data);

            THEN( "The Stream Management callback returns true immediately" )
            {
                // Note: this test requires (currently) additional manual/visual confirmation of console log output
                REQUIRE( pRtspSourceBintr->StreamManager() == true );
            }
        }
        WHEN( "The Source is NOT in reset and lastBufferTime is uninitialized" )
        {
            dsl_rtsp_connection_data data{0};
            data.is_in_reconnect = false;
            data.retries = 0;
            pRtspSourceBintr->_setConnectionData(data);

            THEN( "The Stream Management callback returns true immediately" )
            {
                // Note: this test requires (currently) additional manual/visual confirmation of console log output
                REQUIRE( pRtspSourceBintr->StreamManager() == true );
            }
        }
        WHEN( "The Source is NOT in reset and lastBufferTime = current time" )
        {
            dsl_rtsp_connection_data data{0};
            data.is_in_reconnect = false;
            data.retries = 0;
            pRtspSourceBintr->_setConnectionData(data);
            // get the current time and update the Source buffer timestamp
            timeval currentTime{0};
            gettimeofday(&currentTime, NULL);
            pRtspSourceBintr->_getTimestampPph()->SetTime(currentTime);

            THEN( "The Stream Management callback returns true immediately" )
            {
                // Note: this test requires (currently) additional manual/visual confirmation of console log output
                REQUIRE( pRtspSourceBintr->StreamManager() == true );
            }
        }
        WHEN( "The Source is NOT in reset and currentTime-lastBufferTime > timeout" )
        {
            dsl_rtsp_connection_data data{0};
            data.is_in_reconnect = false;
            data.retries = 0;
            pRtspSourceBintr->_setConnectionData(data);
            pRtspSourceBintr->SetCurrentState(GST_STATE_PLAYING);
            // get the current time and update the Source buffer timestamp
            timeval currentTime{0};
            gettimeofday(&currentTime, NULL);
            currentTime.tv_sec -= timeout;
            pRtspSourceBintr->_getTimestampPph()->SetTime(currentTime);

            THEN( "The Stream Management callback Initiates a Reconnect Cycle" )
            {
                // Note: this test requires (currently) additional manual/visual confirmation of console log output
                REQUIRE( pRtspSourceBintr->StreamManager() == true );

                // simulate timer callback
                REQUIRE( pRtspSourceBintr->NotifyClientListeners() == FALSE );

                // simulate a reconnection timer - 
                REQUIRE( pRtspSourceBintr->ReconnectionManager() == true );
            }
        }
    }
}

SCENARIO( "A RtspSourceBintr can Get and Set its GPU ID",  "[SourceBintr]" )
{
    GIVEN( "A new RtspSourceBintr in memory" ) 
    {
//        std::string uri("rtsp://hddn01.skylinewebcams.com/live.m3u8?a=e8inqgf08vq4rp43gvmkj9ilv0");
        DSL_RTSP_SOURCE_PTR pRtspSourceBintr = DSL_RTSP_SOURCE_NEW(sourceName.c_str(),
            rtspUri.c_str(), DSL_RTP_ALL, intrDecode, dropFrameInterval, latency, timeout);

        uint GPUID0(0);
        uint GPUID1(1);

        REQUIRE( pRtspSourceBintr->GetGpuId() == GPUID0 );
        
        WHEN( "The RtspSourceBintr's GPU ID is set" )
        {
            REQUIRE( pRtspSourceBintr->SetGpuId(GPUID1) == true );

            THEN( "The correct GPU ID is returned on get" )
            {
                REQUIRE( pRtspSourceBintr->GetGpuId() == GPUID1 );
            }
        }
    }
}

SCENARIO( "A new FileSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A name for a new FileSourceBintr" ) 
    {
        char absolutePath[PATH_MAX+1];
        std::string fullFillPath = realpath(filePath.c_str(), absolutePath);
        fullFillPath.insert(0, "file:");

        WHEN( "The FileSourceBintr is created " )
        {
            DSL_FILE_SOURCE_PTR pSourceBintr = DSL_FILE_SOURCE_NEW(
                sourceName.c_str(), filePath.c_str(), false);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == false );
                
                std::string returnedUri = pSourceBintr->GetUri();
                REQUIRE( returnedUri == fullFillPath );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "A FileSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new FileSourceBintr in memory" ) 
    {
        DSL_FILE_SOURCE_PTR pSourceBintr = DSL_FILE_SOURCE_NEW(
            sourceName.c_str(), filePath.c_str(), false);

        WHEN( "The FileSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The FileSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A FileSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked FileSourceBintr " ) 
    {
        DSL_FILE_SOURCE_PTR pSourceBintr = DSL_FILE_SOURCE_NEW(
            sourceName.c_str(), filePath.c_str(), false);

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The FileSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The FileSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A new ImageStreamSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "Attributes for a new ImageStreamSourceBintr" ) 
    {
        char absolutePath[PATH_MAX+1];
        std::string fullFillPath = realpath(jpgFilePath1.c_str(), absolutePath);

        WHEN( "The ImageStreamSourceBintr is created " )
        {
            DSL_IMAGE_STREAM_SOURCE_PTR pSourceBintr = DSL_IMAGE_STREAM_SOURCE_NEW(
                sourceName.c_str(), jpgFilePath1.c_str(), false, 1, 1, 0);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == false );
                
                REQUIRE( pSourceBintr->GetTimeout() == 0 );
                
                std::string returnedFilePath = pSourceBintr->GetUri();
                REQUIRE( returnedFilePath == fullFillPath );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "An ImageStreamSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new ImageStreamSourceBintr in memory" ) 
    {
        DSL_IMAGE_STREAM_SOURCE_PTR pSourceBintr = DSL_IMAGE_STREAM_SOURCE_NEW(
            sourceName.c_str(), jpgFilePath1.c_str(), false, 1, 1, 0);

        WHEN( "The ImageStreamSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The ImageStreamSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "An ImageStreamSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked ImageStreamSourceBintr " ) 
    {
        DSL_IMAGE_STREAM_SOURCE_PTR pSourceBintr = DSL_IMAGE_STREAM_SOURCE_NEW(
            sourceName.c_str(), jpgFilePath1.c_str(), true, 1, 1, 0);

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The ImageStreamSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The ImageStreamSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A new SingleImageSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "Attributes for a new SingleImageSourceBintr" ) 
    {
        char absolutePath[PATH_MAX+1];
        std::string fullFillPath = realpath(jpgFilePath1.c_str(), absolutePath);

        WHEN( "The SingleImageSourceBintr is created " )
        {
            DSL_SINGLE_IMAGE_SOURCE_PTR pSourceBintr = DSL_SINGLE_IMAGE_SOURCE_NEW(
                sourceName.c_str(), jpgFilePath1.c_str());

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == false );
                
                std::string returnedFilePath = pSourceBintr->GetUri();
                REQUIRE( returnedFilePath == fullFillPath );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "An SingleImageSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new SingleImageSourceBintr in memory" ) 
    {
        DSL_SINGLE_IMAGE_SOURCE_PTR pSourceBintr = DSL_SINGLE_IMAGE_SOURCE_NEW(
            sourceName.c_str(), jpgFilePath1.c_str());

        WHEN( "The SingleImageSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The SingleImageSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "An SingleImageSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked SingleImageSourceBintr " ) 
    {
        DSL_SINGLE_IMAGE_SOURCE_PTR pSourceBintr = DSL_SINGLE_IMAGE_SOURCE_NEW(
            sourceName.c_str(), jpgFilePath1.c_str());

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The SingleImageSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The SingleImageSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A new MultiImageSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "Attributes for a new MultiImageSourceBintr" ) 
    {
        WHEN( "The MultiImageSourceBintr is created " )
        {
            DSL_MULTI_IMAGE_SOURCE_PTR pSourceBintr = DSL_MULTI_IMAGE_SOURCE_NEW(
                sourceName.c_str(), multJpgFilePath.c_str(), 1, 1);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == false );

                REQUIRE( pSourceBintr->GetLoopEnabled() == false );
                int startIndex(99), stopIndex(99);

                pSourceBintr->GetIndices(&startIndex, &stopIndex);
                REQUIRE( startIndex == 0 );
                REQUIRE( stopIndex == -1 );
                
                std::string returnedFilePath = pSourceBintr->GetUri();
                REQUIRE( returnedFilePath == multJpgFilePath );

                std::string retBufferOutFormat(pSourceBintr->GetBufferOutFormat());
                REQUIRE( retBufferOutFormat == defaultBufferOutFormat);
            }
        }
    }
}

SCENARIO( "An MultiImageSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new MultiImageSourceBintr in memory" ) 
    {
        DSL_MULTI_IMAGE_SOURCE_PTR pSourceBintr = DSL_MULTI_IMAGE_SOURCE_NEW(
            sourceName.c_str(), multJpgFilePath.c_str(), 1, 1);

        WHEN( "The MultiImageSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The MultiImageSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "An MultiImageSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked MultiImageSourceBintr " ) 
    {
        DSL_MULTI_IMAGE_SOURCE_PTR pSourceBintr = DSL_MULTI_IMAGE_SOURCE_NEW(
            sourceName.c_str(), multJpgFilePath.c_str(), 1, 1);

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The MultiImageSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The MultiImageSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A new DuplicateSourceBintr is created correctly",  "[SourceBintr]" )
{
    GIVEN( "A name for a new DuplicateSourceBintr" ) 
    {
        std::string originalSourceName("original-source");
        bool isLive(true);
        
        WHEN( "The DuplicateSourceBintr is created " )
        {
            DSL_DUPLICATE_SOURCE_PTR pSourceBintr = DSL_DUPLICATE_SOURCE_NEW(
                sourceName.c_str(), originalSourceName.c_str(), isLive);

            THEN( "All memeber variables are initialized correctly" )
            {
                REQUIRE( pSourceBintr->GetGpuId() == 0 );
                REQUIRE( pSourceBintr->GetNvbufMemType() == DSL_NVBUF_MEM_TYPE_DEFAULT );
                REQUIRE( pSourceBintr->GetGstObject() != NULL );
                REQUIRE( pSourceBintr->GetRequestPadId() == -1 );
                REQUIRE( pSourceBintr->IsInUse() == false );
                
                std::string retOriginalName = pSourceBintr->GetOriginal();
                REQUIRE( retOriginalName == originalSourceName);
                
                // Must reflect use of file stream
                REQUIRE( pSourceBintr->IsLive() == isLive );
            }
        }
    }
}

SCENARIO( "A DuplicateSourceBintr can LinkAll child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new DuplicateSourceBintr in memory" ) 

    {
        std::string originalSourceName("original-source");
        bool isLive(true);
        
        DSL_DUPLICATE_SOURCE_PTR pSourceBintr = DSL_DUPLICATE_SOURCE_NEW(
            sourceName.c_str(), originalSourceName.c_str(), isLive);

        WHEN( "The DuplicateSourceBintr is called to LinkAll" )
        {
            REQUIRE( pSourceBintr->LinkAll() == true );

            THEN( "The DuplicateSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == true );
            }
        }
    }
}

SCENARIO( "A DuplicateSourceBintr can UnlinkAll all child Elementrs correctly",  "[SourceBintr]" )
{
    GIVEN( "A new, linked DuplicateSourceBintr " ) 
    {
        std::string originalSourceName("original-source");
        bool isLive(true);
        
        DSL_DUPLICATE_SOURCE_PTR pSourceBintr = DSL_DUPLICATE_SOURCE_NEW(
            sourceName.c_str(), originalSourceName.c_str(), isLive);

        REQUIRE( pSourceBintr->LinkAll() == true );
        REQUIRE( pSourceBintr->IsLinked() == true );

        WHEN( "The DuplicateSourceBintr is called to UnlinkAll" )
        {
            pSourceBintr->UnlinkAll();

            THEN( "The DuplicateSourceBintr IsLinked state is updated correctly" )
            {
                REQUIRE( pSourceBintr->IsLinked() == false );
            }
        }
    }
}

SCENARIO( "A DuplicateSourceBintr added and removed correctly",  "[SourceBintr]" )
{
    GIVEN( "A new DuplicateSourceBintr and VideoSourceBintr" ) 
    {
        std::string originalSourceName("original-source");
        bool isLive(true);
        
        DSL_FILE_SOURCE_PTR pVideoSourceBintr = DSL_FILE_SOURCE_NEW(
            originalSourceName.c_str(), filePath.c_str(), false);

        DSL_DUPLICATE_SOURCE_PTR pSourceBintr = DSL_DUPLICATE_SOURCE_NEW(
            sourceName.c_str(), originalSourceName.c_str(), isLive);

        WHEN( "The DuplicateSourceBintr is added to the VideoSourceBintr" )
        {
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pSourceBintr) == true );

            // second call must fail
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pSourceBintr) == false );

            THEN( "The same DuplicateSourceBintr can be removed correctly" )
            {
                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pSourceBintr) == true );

                // second call must fail
                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pSourceBintr) == false );
            }
        }
    }
}

SCENARIO( "Multiple DuplicateSourceBintrs can be added and linked with a VideoSourceBintr",  
    "[SourceBintr]" )
{
    GIVEN( "A new DuplicateSourceBintr and VideoSourceBintr" ) 
    {
        std::string originalSourceName("original-source");
        std::string duplicateSourceName1("duplicate-source-1");
        std::string duplicateSourceName2("duplicate-source-2");
        std::string duplicateSourceName3("duplicate-source-3");
        std::string duplicateSourceName4("duplicate-source-4");
        bool isLive(true);
        
        std::string sourceName("v4l2-source");
        static std::string defDeviceLocation("/dev/video0");

        DSL_V4L2_SOURCE_PTR pVideoSourceBintr = DSL_V4L2_SOURCE_NEW(
            sourceName.c_str(), defDeviceLocation.c_str());

        DSL_DUPLICATE_SOURCE_PTR pDuplicateSourceBintr1 = DSL_DUPLICATE_SOURCE_NEW(
            duplicateSourceName1.c_str(), originalSourceName.c_str(), isLive);
        DSL_DUPLICATE_SOURCE_PTR pDuplicateSourceBintr2 = DSL_DUPLICATE_SOURCE_NEW(
            duplicateSourceName2.c_str(), originalSourceName.c_str(), isLive);
        DSL_DUPLICATE_SOURCE_PTR pDuplicateSourceBintr3 = DSL_DUPLICATE_SOURCE_NEW(
            duplicateSourceName3.c_str(), originalSourceName.c_str(), isLive);
        DSL_DUPLICATE_SOURCE_PTR pDuplicateSourceBintr4 = DSL_DUPLICATE_SOURCE_NEW(
            duplicateSourceName4.c_str(), originalSourceName.c_str(), isLive);

        WHEN( "The DuplicateSourceBintrs are added to the VideoSourceBintr and all are linked" )
        {
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pDuplicateSourceBintr1) == true );
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pDuplicateSourceBintr2) == true );
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pDuplicateSourceBintr3) == true );
            REQUIRE( pVideoSourceBintr->AddDuplicateSource(pDuplicateSourceBintr4) == true );

            REQUIRE( pVideoSourceBintr->LinkAll() == true );
            REQUIRE( pDuplicateSourceBintr1->LinkAll() == true );
            REQUIRE( pDuplicateSourceBintr2->LinkAll() == true );
            REQUIRE( pDuplicateSourceBintr3->LinkAll() == true );
            REQUIRE( pDuplicateSourceBintr4->LinkAll() == true );

            THEN( "All Sources can be unliked and removed correctly" )
            {
                pVideoSourceBintr->UnlinkAll();
                pDuplicateSourceBintr1->UnlinkAll();
                pDuplicateSourceBintr2->UnlinkAll();
                pDuplicateSourceBintr3->UnlinkAll();
                pDuplicateSourceBintr4->UnlinkAll();

                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pDuplicateSourceBintr1) == true );
                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pDuplicateSourceBintr2) == true );
                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pDuplicateSourceBintr3) == true );
                REQUIRE( pVideoSourceBintr->RemoveDuplicateSource(
                    pDuplicateSourceBintr4) == true );
            }
        }
    }
}
