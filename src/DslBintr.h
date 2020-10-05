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

#ifndef _DSL_BINTR_H
#define _DSL_BINTR_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslNodetr.h"
#include "DslPadProbeHandler.h"

namespace DSL
{
    /**
     * @brief convenience macros for shared pointer abstraction
     */
    #define DSL_BINTR_PTR std::shared_ptr<Bintr>
    #define DSL_BINTR_NEW(name) \
        std::shared_ptr<Bintr>(new Bintr(name))    

    /**
     * @class Bintr
     * @brief Implements a base container class for a GST Bin
     */
    class Bintr : public GstNodetr
    {
    public:

        /**
         * @brief named container ctor with new Bin 
         */
        Bintr(const char* name)
            : GstNodetr(name)
            , m_uniqueId(-1)
            , m_isLinked(false)
            , m_batchSize(0)
            , m_gpuId(0)
            , m_nvbufMemoryType(0)
            , m_pGstStaticSinkPad(NULL)
            , m_pGstStaticSourcePad(NULL)
        { 
            LOG_FUNC(); 

            m_pGstObj = GST_OBJECT(gst_bin_new((gchar*)name));
            if (!m_pGstObj)
            {
                LOG_ERROR("Failed to create a new GST bin for Bintr '" << name << "'");
                throw;  
            }
        }
        
        /**
         * @brief Bintr dtor to release all GST references
         */
        ~Bintr()
        {
            LOG_FUNC();
        }

        /**
         * @brief returns the current Id - managed by the Parent container
         * @return -1 when id is not assigned, i.e. bintr is not currently in use
         */
        int GetId()
        {
            LOG_FUNC();
            
            return m_uniqueId;
        }
        
        /**
         * @brief Sets the unique id for this bintr
         * @param id value to assign [0...MAX]
         */
        void SetId(int id)
        {
            LOG_FUNC();

            m_uniqueId = id;
        }
        
        /**
         * @brief Allows a client to determined derived type from base pointer
         * @param[in] typeInfo to compare against
         * @return true if this Bintr is of typeInfo, false otherwise
         */
        bool IsType(const std::type_info& typeInfo)
        {
            LOG_FUNC();
            
            return (typeInfo.hash_code() == typeid(*this).hash_code());
        }

        /**
         * @brief Adds this Bintr as a child to a ParentBinter
         * @param[in] pParentBintr to add to
         */
        virtual bool AddToParent(DSL_BASE_PTR pParent)
        {
            LOG_FUNC();
                
            return (bool)pParent->AddChild(shared_from_this());
        }
        
        /**
         * @brief removes this Bintr from the provided pParentBintr
         * @param[in] pParentBintr Bintr to remove from
         */
        virtual bool RemoveFromParent(DSL_BASE_PTR pParentBintr)
        {
            LOG_FUNC();
                
            return pParentBintr->RemoveChild(shared_from_this());
        }

        /**
         * @brief virtual function for derived classes to implement
         * a bintr type specific function to link all children.
         */
        virtual bool LinkAll() = 0;
        
        /**
         * @brief virtual function for derived classes to implement
         * a bintr type specific function to unlink all child elements.
         */
        virtual void UnlinkAll() = 0;
        
        /**
         * @brief called to determine if a Bintr's Child Elementrs are Linked
         * @return true if Child Elementrs are currently Linked, false otherwise
         */
        bool IsLinked()
        {
            LOG_FUNC();
            
            return m_isLinked;
        }

        /**
         * @brief called to determine if a Bintr is currently in use - has a Parent
         * @return true if the Bintr has a Parent, false otherwise
         */
        bool IsInUse()
        {
            LOG_FUNC();
            
            return (bool)GetParentGstElement();
        }
        
        /**
         * @brief gets the current batchSize in use by this Bintr
         * @return the current batchSize
         */
        virtual uint GetBatchSize()
        {
            LOG_FUNC();
            
            return m_batchSize;
        };
        
        /**
         * @brief sets the batch size for this Bintr
         * @param the new batchSize to use
         */
        virtual bool SetBatchSize(uint batchSize)
        {
            LOG_FUNC();
            LOG_INFO("Setting batch size to '" << batchSize << "' for Bintr '" << GetName() << "'");
            
            m_batchSize = batchSize;
            return true;
        };

        /**
         * @brief sets the interval for this Bintr
         * @param the new interval to use
         */
        bool SetInterval(uint interval, uint timeout);

        /**
         * @brief gets the current state of the Bintr
         * @param[out] state current state of the Bintr
         * @return one of GST_STATE_CHANGE values.
         */
        uint GetState(GstState& state, GstClockTime timeout)
        {
            LOG_FUNC();

            uint retval = gst_element_get_state(GetGstElement(), &state, NULL, timeout);
            LOG_DEBUG("Get state returned '" << gst_element_state_get_name(state) << "' for Bintr '" << GetName() << "'");
            
            return retval;
        }
        
        /**
         * @brief Attempts to set the state of this Bintr's GST Element
         * @return true if successful transition, false on failure
         */
        bool SetState(GstState state, GstClockTime timeout)
        {
            LOG_FUNC();
            LOG_INFO("Changing state to '" << gst_element_state_get_name(state) << "' for Bintr '" << GetName() << "'");

            GstStateChangeReturn returnVal = gst_element_set_state(GetGstElement(), state);
            switch (returnVal) 
            {
                case GST_STATE_CHANGE_SUCCESS:
                    LOG_INFO("State change completed synchronously for Bintr'" << GetName() << "'");
                    return true;
                case GST_STATE_CHANGE_FAILURE:
                    LOG_ERROR("FAILURE occured when trying to change state to '" << 
                        gst_element_state_get_name(state) << "' for Bintr '" << GetName() << "'");
                    return false;
                case GST_STATE_CHANGE_NO_PREROLL:
                    LOG_INFO("Set state for Bintr '" << GetName() << "' return GST_STATE_CHANGE_NO_PREROLL");
                    return true;
                case GST_STATE_CHANGE_ASYNC:
                    LOG_INFO("State change will complete asynchronously for Bintr '" << GetName() << "'");
                    break;
                default:
                    break;
            }
            
            // Wait until state change or failure, no timeout.
            if (gst_element_get_state(GetGstElement(), NULL, NULL, timeout) == GST_STATE_CHANGE_FAILURE)
            {
                LOG_ERROR("FAILURE occured waiting for state to change to '" << gst_element_state_get_name(state) << "' for Bintr '" << GetName() << "'");
                return false;
            }
            LOG_INFO("State change completed asynchronously for Bintr'" << GetName() << "'");
            return true;
        }

        uint SyncStateWithParent(GstState& parentState, GstClockTime timeout)
        {
            LOG_FUNC();
            
            uint returnVal = gst_element_sync_state_with_parent(GetGstElement());

            switch (returnVal) 
            {
                case GST_STATE_CHANGE_SUCCESS:
                    LOG_INFO("State change completed synchronously for Bintr'" << GetName() << "'");
                    return returnVal;
                case GST_STATE_CHANGE_FAILURE:
                    LOG_ERROR("FAILURE occured when trying to sync state with Parent for Bintr '" << GetName() << "'");
                    return returnVal;
                case GST_STATE_CHANGE_NO_PREROLL:
                    LOG_INFO("Set state for Bintr '" << GetName() << "' return GST_STATE_CHANGE_NO_PREROLL");
                    return returnVal;
                case GST_STATE_CHANGE_ASYNC:
                    LOG_INFO("State change will complete asynchronously for Bintr '" << GetName() << "'");
                    break;
                default:
                    break;
            }
            uint retval = gst_element_get_state(GST_ELEMENT_PARENT(GetGstElement()), &parentState, NULL, timeout);
            LOG_INFO("Get state returned '" << gst_element_state_get_name(parentState) << "' for Parent of Bintr '" << GetName() << "'");
            return retval;
        }
        
        bool SendEos()
        {
            LOG_FUNC();
            
            return gst_pad_send_event(
                gst_element_get_static_pad(GetGstElement(), "sink"), gst_event_new_eos());            
        }
        
        /**
         * @brief Adds a Pad Probe Handler callback function to the Bintr
         * @param[in] pad pad to add the handler to; DSL_PAD_SINK | DSL_PAD SRC
         * @param[in] pPadProbeHandler shared pointer to the PPH to add
         * @return true if successful, false otherwise
         */
        bool AddPadProbeHandler(DSL_BASE_PTR pPadProbeHandler, uint pad)
        {
            LOG_FUNC();
            
            if (pad == DSL_PAD_SINK)
            {
                return m_pSinkPadProbe->AddPadProbeHandler(pPadProbeHandler);
            }
            if (pad == DSL_PAD_SRC)
            {
                return m_pSrcPadProbe->AddPadProbeHandler(pPadProbeHandler);
            }
            LOG_ERROR("Invalid Pad type = " << pad << " for Bintr '" << GetName() << "'");
            return false;
        }
            
        /**
         * @brief Removes a Pad Probe Handler callback function from the Bintr
         * @param[in] pad pad to remove the handler from; DSL_PAD_SINK | DSL_PAD SRC
         * @return false if the Bintr does not have a Meta Batch Handler to remove for the give pad.
         */
        bool RemovePadProbeHandler(DSL_BASE_PTR pPadProbeHandler, uint pad)
        {
            LOG_FUNC();
            
            if (pad == DSL_PAD_SINK)
            {
                return m_pSinkPadProbe->RemovePadProbeHandler(pPadProbeHandler);
            }
            if (pad == DSL_PAD_SRC)
            {
                return m_pSrcPadProbe->RemovePadProbeHandler(pPadProbeHandler);
            }
            LOG_ERROR("Invalid Pad type = " << pad << " for Bintr '" << GetName() << "'");
            return false;
        }
        
        
        /**
         * @brief Gets the current GPU ID used by this Bintr
         * @return the ID for the current GPU in use.
         */
        uint GetGpuId()
        {
            LOG_FUNC();

            LOG_DEBUG("Returning a GPU ID of " << m_gpuId <<"' for Bintr '" << GetName() << "'");
            return m_gpuId;
        }

        /**
         * @brief Bintr type specific implementation to set the GPU ID.
         * @return true if successfully set, false otherwise.
         */
        virtual bool SetGpuId(uint gpuId)
        {
            LOG_FUNC();
            
            if (IsInUse())
            {
                LOG_ERROR("Unable to set GPU ID for Bintr '" << GetName() 
                    << "' as it's currently in use");
                return false;
            }
            m_gpuId = gpuId;
            return true;
        }

    public:

        /**
         * @brief unique identifier managed by the 
         * parent from the point of add until removed
         */
        int m_uniqueId;
    
        /**
         * @brief current is-linked state for this Bintr
         */
        bool m_isLinked;
        
        /**
         * @brief current batch size for this Bintr
         */
        uint m_batchSize;

        /**
         * @brief current GPU Id in used by this Bintr
         */
        guint m_gpuId;

        /**
         * @brief current Memory Type used by this Bintr
         */
        guint m_nvbufMemoryType;

        /**
         * @brief Static Pad object for the Sink Elementr within this Bintr
         */
        GstPad* m_pGstStaticSinkPad;
            
        /**
         * @brief A dynamic collection of requested Sink Pads for this Bintr
         */
        std::map<std::string, GstPad*> m_pGstRequestedSinkPads;
            
        /**
         * @brief Static Pad object for the Source Elementr within this Bintr
         */
        GstPad* m_pGstStaticSourcePad;
            
        /**
         * @brief A dynamic collection of requested Souce Pads for this Bintr
         */
        std::map<std::string, GstPad*> m_pGstRequestedSourcePads;

        /**
         * @brief Sink PadProbetr for this Bintr
         */
        DSL_PAD_PROBE_PTR m_pSinkPadProbe;

        /**
         * @brief Source PadProbetr for this Bintr
         */
        DSL_PAD_PROBE_PTR m_pSrcPadProbe;
    };

} // DSL

#endif // _DSL_BINTR_H