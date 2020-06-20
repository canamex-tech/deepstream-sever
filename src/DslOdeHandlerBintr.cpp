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
#include "DslOdeHandlerBintr.h"
#include "DslBranchBintr.h"

namespace DSL
{

    OdeHandlerBintr::OdeHandlerBintr(const char* name)
        : Bintr(name)
        , m_isEnabled(true)
    {
        LOG_FUNC();

        m_pQueue = DSL_ELEMENT_NEW(NVDS_ELEM_QUEUE, "ode-handler-queue");
        
        Bintr::AddChild(m_pQueue);

        m_pQueue->AddGhostPadToParent("sink");
        m_pQueue->AddGhostPadToParent("src");

        // New src pad probe for event processing and reporting
        m_pSrcPadProbe = DSL_PAD_PROBE_NEW("ode-handler-src-pad-probe", "src", m_pQueue);
        
        if (!AddBatchMetaHandler(DSL_PAD_SRC, PadBufferHandler, this))
        {
            LOG_ERROR("OdeHandlerBintr '" << m_name << "' failed to add probe buffer handler on create");
            throw;
        }
    }

    OdeHandlerBintr::~OdeHandlerBintr()
    {
        LOG_FUNC();

        if (m_isLinked)
        {    
            UnlinkAll();
        }
        RemoveAllChildren();
    }

    bool OdeHandlerBintr::AddToParent(DSL_BASE_PTR pParentBintr)
    {
        LOG_FUNC();
        
        // add 'this' ode-handler to the Parent Pipeline 
        return std::dynamic_pointer_cast<BranchBintr>(pParentBintr)->
            AddOdeHandlerBintr(shared_from_this());
    }
    
    bool OdeHandlerBintr::LinkAll()
    {
        LOG_FUNC();
        
        if (m_isLinked)
        {
            LOG_ERROR("OdeHandlerBintr '" << m_name << "' is already linked");
            return false;
        }

        // single element, nothing to link
        m_isLinked = true;
        
        return true;
    }
    
    void OdeHandlerBintr::UnlinkAll()
    {
        LOG_FUNC();
        
        if (!m_isLinked)
        {
            LOG_ERROR("OdeHandlerBintr '" << m_name << "' is not linked");
            return;
        }
        // single element, nothing to unlink
        m_isLinked = false;
    }
    
    bool OdeHandlerBintr::AddChild(DSL_BASE_PTR pChild)
    {
        LOG_FUNC();
        
        if (!Base::AddChild(pChild))
        {
            LOG_ERROR("Failed to add ODE Trigger' " << pChild->GetName() 
                << "' as a child of '" << GetName() << "'");
            return false;
        }
        m_pOdeTriggers[pChild->GetName()] = pChild;
        return true;
    }

    bool OdeHandlerBintr::RemoveChild(DSL_BASE_PTR pChild)
    {
        LOG_FUNC();
        
        if (!Base::RemoveChild(pChild))
        {
            LOG_ERROR("Failed to remove ODE Trigger' " << pChild->GetName() 
                << "' as a child of '" << GetName() << "'");
            return false;
        }
        m_pOdeTriggers.erase(pChild->GetName());
        return true;
    }
    
    void OdeHandlerBintr::RemoveAllChildren()
    {
        LOG_FUNC();
        
        m_pOdeTriggers.clear();
        Base::RemoveAllChildren();
    }

    bool OdeHandlerBintr::GetEnabled()
    {
        LOG_FUNC();
        
        return m_isEnabled;
    }
    
    bool OdeHandlerBintr::SetEnabled(bool enabled)
    {
        LOG_FUNC();

        if (m_isEnabled == enabled)
        {
            LOG_ERROR("Can't set Handler Enabled to the same value of " 
                << enabled << " for OdeHandlerBintr '" << GetName() << "' ");
            return false;
        }
        m_isEnabled = enabled;
        
        if (enabled)
        {
            LOG_INFO("Enabling the OdeHandlerBintr '" << GetName() << "'");
            
            return AddBatchMetaHandler(DSL_PAD_SRC, PadBufferHandler, this);
        }
        LOG_INFO("Disabling the OdeHandlerBintr '" << GetName() << "'");
        
        return RemoveBatchMetaHandler(DSL_PAD_SRC, PadBufferHandler);
    }
    
    bool OdeHandlerBintr::HandlePadBuffer(GstBuffer* pBuffer)
    {
        NvDsBatchMeta* batchMeta = gst_buffer_get_nvds_batch_meta(pBuffer);
        
        // For each frame in the batched meta data
        for (NvDsMetaList* pFrameMetaList = batchMeta->frame_meta_list; pFrameMetaList != NULL; pFrameMetaList = pFrameMetaList->next)
        {
            // Check for valid frame data
            NvDsFrameMeta* pFrameMeta = (NvDsFrameMeta*) (pFrameMetaList->data);
            if (pFrameMeta != NULL)
            {
                // Preprocess the frame
                for (const auto &imap: m_pOdeTriggers)
                {
                    DSL_ODE_TRIGGER_PTR pOdeTrigger = std::dynamic_pointer_cast<OdeTrigger>(imap.second);
                    pOdeTrigger->PreProcessFrame(pBuffer, pFrameMeta);
                }
                // For each detected object in the frame.
                for (NvDsMetaList* pMeta = pFrameMeta->obj_meta_list; pMeta != NULL; pMeta = pMeta->next)
                {
                    // Check for valid object data
                    NvDsObjectMeta* pObjectMeta = (NvDsObjectMeta*) (pMeta->data);
                    if (pObjectMeta != NULL)
                    {
                        // For each ODE Trigger owned by this ODE Manager, check for ODE
                        for (const auto &imap: m_pOdeTriggers)
                        {
                            DSL_ODE_TRIGGER_PTR pOdeTrigger = std::dynamic_pointer_cast<OdeTrigger>(imap.second);
                            pOdeTrigger->CheckForOccurrence(pBuffer, pFrameMeta, pObjectMeta);
                        }
                    }
                }
                
                // After each detected object is checked for ODE individually, post process 
                // each frame for Absence events, Limit events, etc. (i.e. frame level events).
                for (const auto &imap: m_pOdeTriggers)
                {
                    DSL_ODE_TRIGGER_PTR pOdeTrigger = std::dynamic_pointer_cast<OdeTrigger>(imap.second);
                    pOdeTrigger->PostProcessFrame(pBuffer, pFrameMeta);
                }
            }
        }
        return true;
    }
    
    static boolean PadBufferHandler(void* pBuffer, void* user_data)
    {
        return static_cast<OdeHandlerBintr*>(user_data)->
            HandlePadBuffer((GstBuffer*)pBuffer);
    }
    
}