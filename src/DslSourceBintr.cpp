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

#include "Dsl.h"
#include "DslSourceBintr.h"
#include "DslPipelineBintr.h"

#define N_DECODE_SURFACES 16
#define N_EXTRA_SURFACES 1

namespace DSL
{
    SourceBintr::SourceBintr(const char* name)
        : Bintr(name)
        , m_sensorId(-1)
        , m_isLive(TRUE)
        , m_width(0)
        , m_height(0)
        , m_fps_n(0)
        , m_fps_d(0)
        , m_latency(100)
        , m_numDecodeSurfaces(N_DECODE_SURFACES)
        , m_numExtraSurfaces(N_EXTRA_SURFACES)
    {
        LOG_FUNC();
    }
    
    SourceBintr::~SourceBintr()
    {
        LOG_FUNC();
    }
    
    bool SourceBintr::AddToParent(DSL_NODETR_PTR pParentBintr)
    {
        LOG_FUNC();
        
        // add 'this' Source to the Parent Pipeline 
        return std::dynamic_pointer_cast<PipelineBintr>(pParentBintr)->
            AddSourceBintr(std::dynamic_pointer_cast<SourceBintr>(shared_from_this()));
    }

    bool SourceBintr::IsParent(DSL_NODETR_PTR pParentBintr)
    {
        LOG_FUNC();
        
        // check if 'this' Source is child of Parent Pipeline 
        return std::dynamic_pointer_cast<PipelineBintr>(pParentBintr)->
            IsSourceBintrChild(std::dynamic_pointer_cast<SourceBintr>(shared_from_this()));
    }

    bool SourceBintr::RemoveFromParent(DSL_NODETR_PTR pParentBintr)
    {
        LOG_FUNC();
        
        if (!IsParent(pParentBintr))
        {
            LOG_ERROR("Source '" << m_name << "' is not a child of Pipeline '" << pParentBintr->m_name << "'");
            return false;
        }
        
        // remove 'this' Source from the Parent Pipeline 
        return std::dynamic_pointer_cast<PipelineBintr>(pParentBintr)->
            RemoveSourceBintr(std::dynamic_pointer_cast<SourceBintr>(shared_from_this()));
    }

    int SourceBintr::GetSensorId()
    {
        LOG_FUNC();
        
        return m_sensorId;
    }

    void SourceBintr::SetSensorId(int id)
    {
        LOG_FUNC();

        if (!m_pSourceElement)
        {
            LOG_ERROR("Source Element for SourceBintr '" << m_name << "' has not been instantiated");
            throw;
        }
        m_sensorId = id;
        m_pSourceElement->SetAttribute("sensor-id", m_sensorId);
    }

    void SourceBintr::LinkToSink(DSL_NODETR_PTR pStreamMux)
    {
        LOG_FUNC();

        std::string sinkPadName = "sink_" + std::to_string(m_sensorId);
        
        LOG_INFO("Linking Source '" << m_name << "' to Pad '" << sinkPadName 
            << "' for StreamMux '" << pStreamMux->m_name << "'");
       
        m_pGstSourcePad = gst_element_get_static_pad(GST_ELEMENT(m_pGstObj), "src");
        if (!m_pGstSourcePad)
        {
            LOG_ERROR("Failed to get Static Source Pad for Streaming Source '" << m_name << "'");
        }

        m_pGstSinkPad = gst_element_get_request_pad(
            GST_ELEMENT(pStreamMux->m_pGstObj), sinkPadName.c_str());
            
        if (!m_pGstSinkPad)
        {
            LOG_ERROR("Failed to get Requested Sink Pad for StreamMux '" << pStreamMux->m_name << "'");
        }
            
        Bintr::LinkToSink(pStreamMux);
    }

    void SourceBintr::UnlinkFromSink()
    {
        LOG_FUNC();

        // If we're currently linked to the 
        if (IsLinkedToSink())
        {
            // Unlink from the Stream Muxer first.
            gst_pad_unlink(m_pGstSourcePad, m_pGstSinkPad);
            Nodetr::UnlinkFromSink();
        }
    }
    
    CsiSourceBintr::CsiSourceBintr(const char* name, 
        guint width, guint height, guint fps_n, guint fps_d)
        : SourceBintr(name)
    {
        LOG_FUNC();

        m_width = width;
        m_height = height;
        m_fps_n = fps_n;
        m_fps_d = fps_d;

        m_pSourceElement = DSL_ELEMENT_NEW(NVDS_ELEM_SRC_CAMERA_CSI, "csi_camera_elem");
        m_pCapsFilter = DSL_ELEMENT_NEW(NVDS_ELEM_CAPS_FILTER, "src_caps_filter");

        m_pSourceElement->SetAttribute("bufapi-version", TRUE);
        m_pSourceElement->SetAttribute("maxperf", TRUE);

        GstCaps * pCaps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "NV12",
            "width", G_TYPE_INT, m_width, "height", G_TYPE_INT, m_height, 
            "framerate", GST_TYPE_FRACTION, m_fps_n, m_fps_d, NULL);
        if (!pCaps)
        {
            LOG_ERROR("Failed to create new Simple Capabilities for '" << name << "'");
            throw;  
        }

        GstCapsFeatures *feature = NULL;
        feature = gst_caps_features_new("memory:NVMM", NULL);
        gst_caps_set_features(pCaps, 0, feature);

        m_pCapsFilter->SetAttribute("caps", pCaps);
        
        gst_caps_unref(pCaps);        

        AddChild(m_pSourceElement);
        AddChild(m_pCapsFilter);
        
        m_pCapsFilter->AddGhostPadToParent("src");
    }

    CsiSourceBintr::~CsiSourceBintr()
    {
        LOG_FUNC();

        if (m_isLinked)
        {    
            UnlinkAll();
        }
    }
    
    bool CsiSourceBintr::LinkAll()
    {
        LOG_FUNC();

        if (m_isLinked)
        {
            LOG_ERROR("CsiSourceBintr '" << m_name << "' is already in a linked state");
            return false;
        }
        m_pSourceElement->LinkToSink(m_pCapsFilter);
        m_isLinked = true;
        
        return true;
    }

    void CsiSourceBintr::UnlinkAll()
    {
        LOG_FUNC();

        if (!m_isLinked)
        {
            LOG_ERROR("CsiSourceBintr '" << m_name << "' is not in a linked state");
            return;
        }
//        m_pSourceElement->UnlinkFromSink();
        m_isLinked = false;
    }

    UriSourceBintr::UriSourceBintr(const char* name, const char* uri,
        uint cudadecMemType, uint intraDecode)
        : SourceBintr(name)
        , m_cudadecMemtype(cudadecMemType)
        , m_intraDecode(intraDecode)
        , m_accumulatedBase(0)
        , m_prevAccumulatedBase(0)
    {
        LOG_FUNC();
        
        // TODO Check for Live Source
        
        std::ifstream streamUriFile(uri);
        if (!streamUriFile.good())
        {
            LOG_ERROR("URI '" << uri << "' Not found");
            throw;
        }        
        
        // File source, not live - setup full path
        char absolutePath[PATH_MAX+1];
        m_uri = realpath(uri, absolutePath);
        m_uri.insert(0, "file:");
        m_isLive = FALSE;
        
        LOG_INFO("URI Path = " << m_uri);
              
        m_pSourceElement = DSL_ELEMENT_NEW(NVDS_ELEM_SRC_URI, "uri-src-elem");
        m_pSourceQueue = DSL_ELEMENT_NEW(NVDS_ELEM_QUEUE, "src-queue");
        m_pTee = DSL_ELEMENT_NEW(NVDS_ELEM_TEE, "tee");
        m_pFakeSinkQueue = DSL_ELEMENT_NEW(NVDS_ELEM_QUEUE, "fake-sink-queue");
        m_pFakeSink = DSL_ELEMENT_NEW(NVDS_ELEM_SINK_FAKESINK, "fake-sink");
        
        m_pSourceElement->SetAttribute("uri", m_uri.c_str());

        g_signal_connect(G_OBJECT(m_pSourceElement->m_pGstObj), "pad-added", 
            G_CALLBACK(OnPadAddedCB), this);
        g_signal_connect(G_OBJECT(m_pSourceElement->m_pGstObj), "child-added", 
            G_CALLBACK(OnChildAddedCB), this);
        g_signal_connect(G_OBJECT(m_pSourceElement->m_pGstObj), "source-setup",
            G_CALLBACK(OnSourceSetupCB), this);

        g_object_set_data(G_OBJECT(m_pSourceQueue->m_pGstObj), "source", this);
        

        m_pFakeSink->SetAttribute("sync", false);
        m_pFakeSink->SetAttribute("async", false);

        AddChild(m_pSourceElement);
        AddChild(m_pSourceQueue);
        AddChild(m_pTee);
        AddChild(m_pFakeSinkQueue);
        AddChild(m_pFakeSink);
        
        // Source Ghost Pad for Source Queue
        m_pSourceQueue->AddGhostPadToParent("src");
    }

    UriSourceBintr::~UriSourceBintr()
    {
        LOG_FUNC();
    }

    bool UriSourceBintr::LinkAll()
    {
        LOG_FUNC();

        if (m_isLinked)
        {
            LOG_ERROR("UriSourceBintr '" << m_name << "' is already in a linked state");
            return false;
        }

        GstPadTemplate* pPadTemplate = 
            gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(m_pTee->m_pGstObj), "src_%u");
        if (!pPadTemplate)
        {
            LOG_ERROR("Failed to get Pad Template for '" << m_name << "'");
            return false;
        }
        
        // The TEE for this source is linked to both the "source queue" and "fake sink queue"

        m_pGstSourcePad = gst_element_request_pad(
            GST_ELEMENT(m_pTee->m_pGstObj), pPadTemplate, NULL, NULL);
        if (!m_pGstSourcePad)
        {
            LOG_ERROR("Failed to get Tee Pad for PipelineSinksBintr '" << m_name <<" '");
            return false;
        }
        m_pTee->LinkToSink(m_pSourceQueue);

        m_pGstSourcePad = gst_element_request_pad(
            GST_ELEMENT(m_pTee->m_pGstObj), pPadTemplate, NULL, NULL);
        if (!m_pGstSourcePad)
        {
            LOG_ERROR("Failed to get Tee Pad for PipelineSinksBintr '" << m_name <<" '");
            return false;
        }
        m_pTee->LinkToSink(m_pFakeSinkQueue);
    
        m_pFakeSinkQueue->LinkToSink(m_pFakeSink);
        m_isLinked = true;

        return true;
    }

    void UriSourceBintr::UnlinkAll()
    {
        LOG_FUNC();
    
        if (!m_isLinked)
        {
            LOG_ERROR("CsiSourceBintr '" << m_name << "' is not in a linked state");
            return;
        }
        m_pFakeSinkQueue->UnlinkFromSink();
        m_isLinked = false;
    }

    void UriSourceBintr::HandleOnPadAdded(GstElement* pBin, GstPad* pPad)
    {
        LOG_FUNC();

        // The "pad-added" callback will be called twice for each URI source,
        // once each for the decoded Audio and Video streams. Since we only 
        // want to link to the Video source pad, we need to know which of the
        // two pads/calls this is for.
        GstCaps* pCaps = gst_pad_query_caps(pPad, NULL);
        GstStructure* structure = gst_caps_get_structure(pCaps, 0);
        std::string name = gst_structure_get_name(structure);
        
        LOG_INFO("Caps structs name " << name);
        if (name.find("video") != std::string::npos)
        {
            m_pGstSinkPad = gst_element_get_static_pad(GST_ELEMENT(m_pTee->m_pGstObj), "sink");
            if (!m_pGstSinkPad)
            {
                LOG_ERROR("Failed to get Static Source Pad for Streaming Source '" << m_name << "'");
            }
            
            if (gst_pad_link(pPad, m_pGstSinkPad) != GST_PAD_LINK_OK) 
            {
                LOG_ERROR("Failed to link decodebin to pipeline");
                throw;
            }
            
            // Update the cap memebers for this URI Source Bintr
            gst_structure_get_uint(structure, "width", &m_width);
            gst_structure_get_uint(structure, "height", &m_height);
            gst_structure_get_fraction(structure, "framerate", (gint*)&m_fps_n, (gint*)&m_fps_d);
            
            LOG_INFO("Video decode linked for URI source '" << m_name << "'");
        }
    }

    void UriSourceBintr::HandleOnChildAdded(GstChildProxy* pChildProxy, GObject* pObject,
        gchar* name)
    {
        LOG_FUNC();
        
        std::string strName = name;

        LOG_DEBUG("Child object with name '" << strName << "'");
        
        if (strName.find("decodebin") != std::string::npos)
        {
            g_signal_connect(G_OBJECT(pObject), "child-added",
                G_CALLBACK(OnChildAddedCB), this);
        }

        else if (strName.find("nvcuvid") != std::string::npos)
        {
            g_object_set(pObject, "gpu-id", m_gpuId, NULL);
            g_object_set(pObject, "cuda-memory-type", m_cudadecMemtype, NULL);
            g_object_set(pObject, "source-id", m_sensorId, NULL);
            g_object_set(pObject, "num-decode-surfaces", m_numDecodeSurfaces, NULL);
            
            if (m_intraDecode)
            {
                g_object_set(pObject, "Intra-decode", m_intraDecode, NULL);
            }
        }

        else if ((strName.find("omx") != std::string::npos) && m_intraDecode)
        {
            g_object_set(pObject, "skip-frames", 2, NULL);
            g_object_set(pObject, "disable-dvfs", TRUE, NULL);
        }

        else if (strName.find("nvjpegdec") != std::string::npos)
        {
            g_object_set(pObject, "DeepStream", TRUE, NULL);
        }

        else if ((strName.find("nvv4l2decoder") != std::string::npos) && m_intraDecode)
        {
            g_object_set (pObject, "skip-frames", 2, NULL);
#ifdef __aarch64__
            g_object_set(pObject, "enable-max-performance", TRUE, NULL);
            g_object_set(pObject, "bufapi-version", TRUE, NULL);
#else
            g_object_set(pObject, "gpu-id", pUriSourceBintr->gpuId, NULL);
            g_object_set(G_OBJECT(pObject), "cudadec-memtype", m_cudadecMemtype, NULL);
#endif
            g_object_set(pObject, "drop-frame-interval", m_dropFrameInterval, NULL);
            g_object_set(pObject, "num-extra-surfaces", m_numExtraSurfaces, NULL);

            // if the source is from file, then setup Stream buffer probe function
            // to handle the stream restart/loop on GST_EVENT_EOS.
            if (!m_isLive)
            {
                GstPadProbeType mask = (GstPadProbeType) 
                    (GST_PAD_PROBE_TYPE_EVENT_BOTH |
                    GST_PAD_PROBE_TYPE_EVENT_FLUSH | 
                    GST_PAD_PROBE_TYPE_BUFFER);
                    
// TODO !!!!        StaticPadtr sinkPadtr("sink", GST_ELEMENT(pObject));
                
//                m_bufferProbeId = sinkPadtr.AddPad(mask, StreamBufferRestartProbCB, this);
            }
        }
    }
    
    GstPadProbeReturn UriSourceBintr::HandleStreamBufferRestart(GstPad* pPad, GstPadProbeInfo* pInfo)
    {
        LOG_FUNC();
        
        GstEvent* event = GST_EVENT(pInfo->data);

        if (pInfo->type & GST_PAD_PROBE_TYPE_BUFFER)
        {
            GST_BUFFER_PTS(GST_BUFFER(pInfo->data)) += m_prevAccumulatedBase;
        }
        
        else if (pInfo->type & GST_PAD_PROBE_TYPE_EVENT_BOTH)
        {
            if (GST_EVENT_TYPE(event) == GST_EVENT_EOS)
            {
                g_timeout_add(1, StreamBufferSeekCB, this);
            }
            else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
            {
                GstSegment* segment;

                gst_event_parse_segment(event, (const GstSegment**)&segment);
                segment->base = m_accumulatedBase;
                m_prevAccumulatedBase = m_accumulatedBase;
                m_accumulatedBase += segment->stop;
            }
            switch (GST_EVENT_TYPE (event))
            {
            case GST_EVENT_EOS:
            // QOS events from downstream sink elements cause decoder to drop
            // frames after looping the file since the timestamps reset to 0.
            // We should drop the QOS events since we have custom logic for
            // looping individual sources.
            case GST_EVENT_QOS:
            case GST_EVENT_SEGMENT:
            case GST_EVENT_FLUSH_START:
            case GST_EVENT_FLUSH_STOP:
                return GST_PAD_PROBE_DROP;
            default:
                break;
            }
        }
        return GST_PAD_PROBE_OK;
    }

    void UriSourceBintr::HandleOnSourceSetup(GstElement* pObject, GstElement* arg0)
    {
        if (g_object_class_find_property(G_OBJECT_GET_CLASS(arg0), "latency")) 
        {
            g_object_set(G_OBJECT(arg0), "latency", "cb_sourcesetup set %d latency\n", NULL);
        }
    }
    
    gboolean UriSourceBintr::HandleStreamBufferSeek()
    {
        gst_element_set_state(GST_ELEMENT(m_pGstObj), GST_STATE_PAUSED);

        gboolean retval = gst_element_seek(GST_ELEMENT(m_pGstObj), 1.0, GST_FORMAT_TIME,
            (GstSeekFlags)(GST_SEEK_FLAG_KEY_UNIT | GST_SEEK_FLAG_FLUSH),
            GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

        if (!retval)
        {
            LOG_WARN("Failure to seek");
        }

        gst_element_set_state(GST_ELEMENT(m_pGstObj), GST_STATE_PLAYING);
    }
    
    
    static void OnPadAddedCB(GstElement* pBin, GstPad* pPad, gpointer pSource)
    {
        static_cast<UriSourceBintr*>(pSource)->HandleOnPadAdded(pBin, pPad);
    }
    
    static void OnChildAddedCB(GstChildProxy* pChildProxy, GObject* pObject,
        gchar* name, gpointer pSource)
    {
        static_cast<UriSourceBintr*>(pSource)->HandleOnChildAdded(pChildProxy, pObject, name);
    }
    
    static void OnSourceSetupCB(GstElement* pObject, GstElement* arg0, 
        gpointer pSource)
    {
        static_cast<UriSourceBintr*>(pSource)->HandleOnSourceSetup(pObject, arg0);
    }
    
    static GstPadProbeReturn StreamBufferRestartProbCB(GstPad* pPad, 
        GstPadProbeInfo* pInfo, gpointer pSource)
    {
        return static_cast<UriSourceBintr*>(pSource)->
            HandleStreamBufferRestart(pPad, pInfo);
    }

    static gboolean StreamBufferSeekCB(gpointer pSource)
    {
        return static_cast<UriSourceBintr*>(pSource)->HandleStreamBufferSeek();
    }
} // SDL namespace