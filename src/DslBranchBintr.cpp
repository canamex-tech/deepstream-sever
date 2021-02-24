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

#include "Dsl.h"
#include "DslServices.h"
#include "DslBranchBintr.h"

#include <gst/gst.h>

namespace DSL
{
    BranchBintr::BranchBintr(const char* name)
        : Bintr(name)
        , m_batchTimeout(DSL_DEFAULT_STREAMMUX_BATCH_TIMEOUT)
    {
        LOG_FUNC();
    }

    bool BranchBintr::AddPrimaryGieBintr(DSL_BASE_PTR pPrmaryGieBintr)
    {
        LOG_FUNC();
        
        if (m_pPrimaryGieBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' has an exisiting Primary GIE '" 
                << m_pPrimaryGieBintr->GetName());
            return false;
        }
        m_pPrimaryGieBintr = std::dynamic_pointer_cast<PrimaryGieBintr>(pPrmaryGieBintr);
        
        return AddChild(pPrmaryGieBintr);
    }

    bool BranchBintr::AddTrackerBintr(DSL_BASE_PTR pTrackerBintr)
    {
        LOG_FUNC();

        if (m_pTrackerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Tracker");
            return false;
        }
        m_pTrackerBintr = std::dynamic_pointer_cast<TrackerBintr>(pTrackerBintr);
        
        return AddChild(pTrackerBintr);
    }

    bool BranchBintr::AddSecondaryGieBintr(DSL_BASE_PTR pSecondaryGieBintr)
    {
        LOG_FUNC();
        
        // Create the optional Secondary GIEs bintr 
        if (!m_pSecondaryGiesBintr)
        {
            m_pSecondaryGiesBintr = DSL_PIPELINE_SGIES_NEW("sgies-bin");
            AddChild(m_pSecondaryGiesBintr);
        }
        return m_pSecondaryGiesBintr->AddChild(std::dynamic_pointer_cast<SecondaryGieBintr>(pSecondaryGieBintr));
    }

    bool BranchBintr::AddOfvBintr(DSL_BASE_PTR pOfvBintr)
    {
        LOG_FUNC();

        if (m_pOfvBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has an Optical Flow Visualizer ");
            return false;
        }
        m_pOfvBintr = std::dynamic_pointer_cast<OfvBintr>(pOfvBintr);
        
        return AddChild(m_pOfvBintr);
    }

    bool BranchBintr::AddDemuxerBintr(DSL_BASE_PTR pDemuxerBintr)
    {
        LOG_FUNC();

        if (m_pDemuxerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Demuxer");
            return false;
        }
        if (m_pSplitterBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Splitter - can't add Demuxer");
            return false;
        }
        if (m_pTilerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Tiler - can't add Demuxer");
            return false;
        }
        m_pDemuxerBintr = std::dynamic_pointer_cast<DemuxerBintr>(pDemuxerBintr);
        
        return AddChild(pDemuxerBintr);
    }

    bool BranchBintr::AddSplitterBintr(DSL_BASE_PTR pSplitterBintr)
    {
        LOG_FUNC();

        if (m_pSplitterBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Splitter");
            return false;
        }
        if (m_pDemuxerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Dumxer- can't add Splitter");
            return false;
        }
        m_pSplitterBintr = std::dynamic_pointer_cast<SplitterBintr>(pSplitterBintr);
        
        return AddChild(pSplitterBintr);
    }

    bool BranchBintr::AddTilerBintr(DSL_BASE_PTR pTilerBintr)
    {
        LOG_FUNC();

        if (m_pTilerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Tiler");
            return false;
        }
        if (m_pDemuxerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Demuxer - can't add Tiler");
            return false;
        }
        m_pTilerBintr = std::dynamic_pointer_cast<TilerBintr>(pTilerBintr);
        
        return AddChild(pTilerBintr);
    }

    bool BranchBintr::AddOsdBintr(DSL_BASE_PTR pOsdBintr)
    {
        LOG_FUNC();
        
        if (m_pOsdBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' has an exisiting OSD '" 
                << m_pOsdBintr->GetName());
            return false;
        }
        if (m_pDemuxerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Demuxer - can't add OSD");
            return false;
        }
        m_pOsdBintr = std::dynamic_pointer_cast<OsdBintr>(pOsdBintr);
        
        return AddChild(pOsdBintr);
    }

    bool BranchBintr::RemoveOsdBintr(DSL_BASE_PTR pOsdBintr)
    {
        LOG_FUNC();
        
        if (!m_pOsdBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' has no OSD to remove'");
            return false;
        }
        if (m_pOsdBintr != pOsdBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' does not own OSD' " 
                << m_pOsdBintr->GetName() << "'");
            return false;
        }
        m_pOsdBintr = nullptr;
        
        LOG_INFO("Removing OSD '"<< pOsdBintr->GetName() 
            << "' from Branch '" << GetName() << "'");
        return RemoveChild(pOsdBintr);
    }

    bool BranchBintr::AddSinkBintr(DSL_BASE_PTR pSinkBintr)
    {
        LOG_FUNC();
        
        if (m_pDemuxerBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Demuxer - can't add Sink after a Demuxer");
            return false;
        }
        if (m_pSplitterBintr)
        {
            LOG_ERROR("Branch '" << GetName() << "' already has a Tee - can't add Sink after a Tee");
            return false;
        }
        // Create the shared Sinks bintr if it doesn't exist
        if (!m_pMultiSinksBintr)
        {
            m_pMultiSinksBintr = DSL_MULTI_SINKS_NEW("sinks-bin");
            AddChild(m_pMultiSinksBintr);
        }
        return m_pMultiSinksBintr->AddChild(std::dynamic_pointer_cast<Bintr>(pSinkBintr));
    }

    bool BranchBintr::IsSinkBintrChild(DSL_BASE_PTR pSinkBintr)
    {
        LOG_FUNC();

        if (!m_pMultiSinksBintr)
        {
            LOG_INFO("Branch '" << GetName() << "' has no Sinks");
            return false;
        }
        return (m_pMultiSinksBintr->IsChild(std::dynamic_pointer_cast<SinkBintr>(pSinkBintr)));
    }

    bool BranchBintr::RemoveSinkBintr(DSL_BASE_PTR pSinkBintr)
    {
        LOG_FUNC();

        if (!m_pMultiSinksBintr)
        {
            LOG_INFO("Branch '" << GetName() << "' has no Sinks");
            return false;
        }

        // Must cast to SourceBintr first so that correct Instance of RemoveChild is called
        return m_pMultiSinksBintr->RemoveChild(std::dynamic_pointer_cast<Bintr>(pSinkBintr));
    }
    
    bool BranchBintr::LinkAll()
    {
        LOG_FUNC();
        
        if (m_isLinked)
        {
            LOG_INFO("Components for Branch '" << GetName() << "' are already assembled");
            return false;
        }
        if (!m_pDemuxerBintr and !m_pSplitterBintr and !m_pMultiSinksBintr)
//        if (!m_pDemuxerBintr and !m_pTilerBintr)
        {
            LOG_ERROR("Pipline '" << GetName() << "' has no Demuxer, Splitter or Sink - and is unable to link");
            return false;
        }
        if (m_pTrackerBintr and !m_pPrimaryGieBintr)
        {
            LOG_ERROR("Pipline '" << GetName() << "' has a Tracker and no Primary GIE - and is unable to link");
            return false;
        }
        if (m_pSecondaryGiesBintr and !m_pPrimaryGieBintr)
        {
            LOG_ERROR("Pipline '" << GetName() << "' has a Seconday GIE and no Primary GIE - and is unable to link");
            return false;
        }
        
        if (m_pPrimaryGieBintr)
        {
            // Set the GIE's batch size to the current stream muxer batch size, 
            // then LinkAll PrimaryGie Elementrs and add as the next component in the Branch
            m_pPrimaryGieBintr->SetBatchSize(m_batchSize);
            if (!m_pPrimaryGieBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pPrimaryGieBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pPrimaryGieBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Primary GIE '" << 
                m_pPrimaryGieBintr->GetName() << "' successfully");
        }
        
        if (m_pTrackerBintr)
        {
            // LinkAll Tracker Elementrs and add as the next component in the Branch
            m_pTrackerBintr->SetBatchSize(m_batchSize);
            if (!m_pTrackerBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pTrackerBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pTrackerBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Tracker '" << 
                m_pTrackerBintr->GetName() << "' successfully");
        }
        
        if (m_pSecondaryGiesBintr)
        {
            // Set the Secondary GIEs' Primary GIE Name, and set batch sizes
            m_pSecondaryGiesBintr->SetInferOnGieId(m_pPrimaryGieBintr->GetUniqueId());
            m_pSecondaryGiesBintr->SetBatchSize(m_batchSize);
            
            // LinkAll SecondaryGie Elementrs and add the Bintr as next component in the Branch
            if (!m_pSecondaryGiesBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pSecondaryGiesBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pSecondaryGiesBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up all Secondary GIEs '" << 
                m_pSecondaryGiesBintr->GetName() << "' successfully");
        }

        if (m_pOfvBintr)
        {
            // LinkAll Optical Flow Elementrs and add as the next component in the Branch
            m_pOfvBintr->SetBatchSize(m_batchSize);
            if (!m_pOfvBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pOfvBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pOfvBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Optical Flow Detector '" << 
                m_pOfvBintr->GetName() << "' successfully");
        }

        // mutually exclusive with Demuxer
        if (m_pTilerBintr)
        {
            // Link All Tiler Elementrs and add as the next component in the Branch
            m_pTilerBintr->SetBatchSize(m_batchSize);
            if (!m_pTilerBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pTilerBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pTilerBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Tiler '" << 
                m_pTilerBintr->GetName() << "' successfully");
        }

        // mutually exclusive with Demuxer
        if (m_pOsdBintr)
        {
            // LinkAll Osd Elementrs and add as next component in the Branch
            m_pOsdBintr->SetBatchSize(m_batchSize);
            if (!m_pOsdBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pOsdBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pOsdBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up OSD '" << 
                m_pOsdBintr->GetName() << "' successfully");
        }

        // mutually exclusive with TilerBintr, Pipeline-OsdBintr, and Pieline-MultiSinksBintr
        if (m_pDemuxerBintr)
        {
            // Link All Demuxer Elementrs and add as the next ** AND LAST ** component in the Pipeline
            m_pDemuxerBintr->SetBatchSize(m_batchSize);
            if (!m_pDemuxerBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pDemuxerBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pDemuxerBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Stream Demuxer '" << 
                m_pDemuxerBintr->GetName() << "' successfully");
        }

        // mutually exclusive with TilerBintr, Pipeline-OsdBintr, and Pieline-MultiSinksBintr
        if (m_pSplitterBintr)
        {
            // Link All Demuxer Elementrs and add as the next ** AND LAST ** component in the Pipeline
            m_pSplitterBintr->SetBatchSize(m_batchSize);
            if (!m_pSplitterBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pSplitterBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pSplitterBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up Stream Splitter'" << 
                m_pSplitterBintr->GetName() << "' successfully");
        }

        // mutually exclusive with Demuxer
        if (m_pMultiSinksBintr)
        {
            // Link all Sinks and their elementrs and add as finale (tail) components in the Branch
            m_pMultiSinksBintr->SetBatchSize(m_batchSize);
            if (!m_pMultiSinksBintr->LinkAll() or
                (m_linkedComponents.size() and !m_linkedComponents.back()->LinkToSink(m_pMultiSinksBintr)))
            {
                return false;
            }
            m_linkedComponents.push_back(m_pMultiSinksBintr);
            LOG_INFO("Branch '" << GetName() << "' Linked up all Sinks '" << 
                m_pMultiSinksBintr->GetName() << "' successfully");
        }
        
        m_isLinked = true;
        return true;
    }
    
    void BranchBintr::UnlinkAll()
    {
        LOG_FUNC();
        
        if (!m_isLinked)
        {
            return;
        }
        // iterate through the list of Linked Components, unlinking each
        for (auto const& ivector: m_linkedComponents)
        {
            ivector->SendEos();
            // all but the tail m_pMultiSinksBintr will be Linked to Sink
            if (ivector->IsLinkedToSink())
            {
                ivector->UnlinkFromSink();
            }
            ivector->UnlinkAll();
        }
        m_linkedComponents.clear();

        m_isLinked = false;
    }
    
    bool BranchBintr::LinkToSourceTee(DSL_NODETR_PTR pTee, const char* srcPadName)
    {
        LOG_FUNC();
        
        if (!m_linkedComponents.size())
        {
            LOG_ERROR("Unable to link empty Bramch '" << GetName() <<"'");
            return false;
        }

        GstPad* pComponentStaticSinkPad = gst_element_get_static_pad(m_linkedComponents.front()->GetGstElement(), "sink");
        if (!pComponentStaticSinkPad)
        {
            LOG_ERROR("Failed to get static Sink Pad for Branch Bintr '" << GetName() <<"'");
            return false;
        }        
        // Add a sink ghost pad to BranchBintr, using the firt componet's 
        if (!gst_element_add_pad(GetGstElement(), 
            gst_ghost_pad_new("sink", pComponentStaticSinkPad)))
        {
            gst_object_unref(pComponentStaticSinkPad);
            LOG_ERROR("Failed to add Sink Ghost Pad for BranchBintr'" << GetName() << "'");
            return false;
        }
        gst_object_unref(pComponentStaticSinkPad);
        
        return Bintr::LinkToSourceTee(pTee, srcPadName);
    }

} // DSL
