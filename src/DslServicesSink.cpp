/*
The MIT License

Copyright (c)   2021, Prominence AI, Inc.

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
#include "DslApi.h"
#include "DslServices.h"
#include "DslServicesValidate.h"
#include "DslSinkBintr.h"

namespace DSL
{
        DslReturnType Services::SinkFakeNew(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            m_components[name] = DSL_FAKE_SINK_NEW(name);

            LOG_INFO("New Fake Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkOverlayNew(const char* name, uint display_id,
        uint depth, uint offsetX, uint offsetY, uint width, uint height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // Get the Device properties
            cudaDeviceProp deviceProp;
            cudaGetDeviceProperties(&deviceProp, 0);
            
            if (!deviceProp.integrated)
            {
                LOG_ERROR("Overlay Sink is not supported on dGPU x86_64 builds");
                return DSL_RESULT_SINK_OVERLAY_NOT_SUPPORTED;
            }
            
            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            m_components[name] = DSL_OVERLAY_SINK_NEW(
                name, display_id, depth, offsetX, offsetY, width, height);

            LOG_INFO("New Overlay Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkWindowNew(const char* name, 
        uint offsetX, uint offsetY, uint width, uint height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            m_components[name] = DSL_WINDOW_SINK_NEW(name, offsetX, offsetY, width, height);

            LOG_INFO("New Window Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkWindowForceAspectRatioGet(const char* name, 
        boolean* force)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, WindowSinkBintr);

            DSL_WINDOW_SINK_PTR pWindowSinkBintr = 
                std::dynamic_pointer_cast<WindowSinkBintr>(m_components[name]);

            *force = pWindowSinkBintr->GetForceAspectRatio();
            
            LOG_INFO("Window Sink '" << name << "' returned Force Aspect Ration = " 
                << *force  << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Window Sink'" << name 
                << "' threw an exception getting 'force-aspect-ratio' property");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkWindowForceAspectRatioSet(const char* name, 
        boolean force)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, WindowSinkBintr);

            DSL_WINDOW_SINK_PTR pWindowSinkBintr = 
                std::dynamic_pointer_cast<WindowSinkBintr>(m_components[name]);

            if (!pWindowSinkBintr->SetForceAspectRatio(force))
            {
                LOG_ERROR("Window Sink '" << name 
                    << "' failed to Set 'force-aspec-ratio' property");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Window Sink '" << name << "' set Force Aspect Ration = " 
                << force  << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Window Sink'" << name 
                << "' threw an exception setting 'force-apect-ratio' property");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
        
    DslReturnType Services::SinkRenderOffsetsGet(const char* name, uint* offsetX, uint* offsetY)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_RENDER_SINK(m_components, name);

            DSL_RENDER_SINK_PTR pRenderSink = 
                std::dynamic_pointer_cast<RenderSinkBintr>(m_components[name]);

            pRenderSink->GetOffsets(offsetX, offsetY);
            
            LOG_INFO("Render Sink '" << name << "' returned Offset X = " 
                << *offsetX << " and Offset Y = " << *offsetY << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Render Sink '" << name << "' threw an exception getting offsets");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRenderOffsetsSet(const char* name, uint offsetX, uint offsetY)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_RENDER_SINK(m_components, name);

            DSL_RENDER_SINK_PTR pRenderSink = 
                std::dynamic_pointer_cast<RenderSinkBintr>(m_components[name]);

            if (!pRenderSink->SetOffsets(offsetX, offsetY))
            {
                LOG_ERROR("Render Sink '" << name << "' failed to set offsets");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Render Sink '" << name << "' set Offset X = " 
                << offsetX << " and Offset Y = " << offsetY << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Render Sink '" << name << "' threw an exception setting offsets");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRenderDimensionsGet(const char* name, uint* width, uint* height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_RENDER_SINK(m_components, name);

            DSL_RENDER_SINK_PTR pRenderSink = 
                std::dynamic_pointer_cast<RenderSinkBintr>(m_components[name]);

            pRenderSink->GetDimensions(width, height);

            LOG_INFO("Render Sink '" << name << "' returned Width = " 
                << *width << " and Height = " << *height << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Render Sink '" << name << "' threw an exception getting dimensions");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRenderDimensionsSet(const char* name, uint width, uint height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_RENDER_SINK(m_components, name);

            DSL_RENDER_SINK_PTR pRenderSink = 
                std::dynamic_pointer_cast<RenderSinkBintr>(m_components[name]);

            if (!pRenderSink->SetDimensions(width, height))
            {
                LOG_ERROR("Render Sink '" << name << "' failed to set dimensions");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Render Sink '" << name << "' set Width = " 
                << width << " and Height = " << height << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Render Sink '" << name << "' threw an exception setting dimensions");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkRenderReset(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_RENDER_SINK(m_components, name);

            DSL_RENDER_SINK_PTR pRenderSink = 
                std::dynamic_pointer_cast<RenderSinkBintr>(m_components[name]);

            if (!pRenderSink->Reset())
            {
                LOG_ERROR("Render Sink '" << name << "' failed to reset its render suface");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Render Sink '" << name << "' Reset successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Render Sink '" << name << "' threw an exception reseting its surface");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkFileNew(const char* name, const char* filepath, 
            uint codec, uint container, uint bitrate, uint interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            if (codec > DSL_CODEC_MPEG4)
            {   
                LOG_ERROR("Invalid Codec value = " << codec << " for File Sink '" << name << "'");
                return DSL_RESULT_SINK_CODEC_VALUE_INVALID;
            }
            if (container > DSL_CONTAINER_MKV)
            {   
                LOG_ERROR("Invalid Container value = " << container << " for File Sink '" << name << "'");
                return DSL_RESULT_SINK_CONTAINER_VALUE_INVALID;
            }
            m_components[name] = DSL_FILE_SINK_NEW(name, filepath, codec, container, bitrate, interval);
            
            LOG_INFO("New File Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkRecordNew(const char* name, const char* outdir, uint codec, uint container, 
        uint bitrate, uint interval, dsl_record_client_listener_cb clientListener)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            struct stat info;

            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            // ensure outdir exists
            if ((stat(outdir, &info) != 0) or !(info.st_mode & S_IFDIR))
            {
                LOG_ERROR("Unable to access outdir '" << outdir << "' for Record Sink '" << name << "'");
                return DSL_RESULT_SINK_FILE_PATH_NOT_FOUND;
            }

            if (codec > DSL_CODEC_MPEG4)
            {   
                LOG_ERROR("Invalid Codec value = " << codec << " for Record Sink '" << name << "'");
                return DSL_RESULT_SINK_CODEC_VALUE_INVALID;
            }
            if (container > DSL_CONTAINER_MKV)
            {   
                LOG_ERROR("Invalid Container value = " << container << " for Record Sink '" << name << "'");
                return DSL_RESULT_SINK_CONTAINER_VALUE_INVALID;
            }

            m_components[name] = DSL_RECORD_SINK_NEW(name, outdir, 
                codec, container, bitrate, interval, clientListener);
            
            LOG_INFO("New Record Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Record Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordSessionStart(const char* name, 
        uint start, uint duration, void* clientData)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!recordSinkBintr->StartSession(start, duration, clientData))
            {
                LOG_ERROR("Record Sink '" << name << "' failed to Start Session");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            return DSL_RESULT_SUCCESS;
            
            LOG_INFO("Session started successfully for Record Sink '" << name << "'");
        }
        catch(...)
        {
            LOG_ERROR("Record Sink'" << name << "' threw an exception setting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordSessionStop(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!recordSinkBintr->StopSession())
            {
                LOG_ERROR("Record Sink '" << name << "' failed to Stop Session");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Session stopped successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink'" << name << "' threw an exception setting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordOutdirGet(const char* name, const char** outdir)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            
            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            *outdir = pRecordSinkBintr->GetOutdir();
            
            LOG_INFO("Outdir = " << *outdir << " returned successfully for Record Sink '" << name << "'");
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink'" << name << "' threw an exception setting getting outdir");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordOutdirSet(const char* name, const char* outdir)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            
            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->SetOutdir(outdir))
            {
                LOG_ERROR("Record Sink '" << name << "' failed to set the outdir");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Outdir = " << outdir << " set successfully for Record Sink '" << name << "'");
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception setting getting outdir"); 
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }

        return DSL_RESULT_SUCCESS;
    }

    DslReturnType Services::SinkRecordContainerGet(const char* name, uint* container)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            *container = pRecordSinkBintr->GetContainer();

            LOG_INFO("Container = " << *container 
                << " returned successfully for Record Sink '" << name << "'");
                
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception getting Cache Size");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordContainerSet(const char* name, uint container)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            if (container > DSL_CONTAINER_MKV)
            {   
                LOG_ERROR("Invalid Container value = " 
                    << container << " for Record Sink '" << name << "'");
                return DSL_RESULT_SINK_CONTAINER_VALUE_INVALID;
            }

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->SetContainer(container))
            {
                LOG_ERROR("Record Sink '" << name << "' failed to set container");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Container = " << container 
                << " set successfully for Record Tap '" << name << "'");
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception setting container type");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
        

    DslReturnType Services::SinkRecordCacheSizeGet(const char* name, uint* cacheSize)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            // TODO verify args before calling
            *cacheSize = recordSinkBintr->GetCacheSize();

            LOG_INFO("Cashe size = " << *cacheSize << 
                " returned successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception getting cache size");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordCacheSizeSet(const char* name, uint cacheSize)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            // TODO verify args before calling
            if (!recordSinkBintr->SetCacheSize(cacheSize))
            {
                LOG_ERROR("Record Sink '" << name << "' failed to set cache size");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Record Sink '" << name 
                << "' successfully set cache size to " << cacheSize << " seconds");
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception setting cache size");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
        
    DslReturnType Services::SinkRecordDimensionsGet(const char* name, uint* width, uint* height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            // TODO verify args before calling
            recordSinkBintr->GetDimensions(width, height);

            LOG_INFO("Width = " << *width << " height = " << *height << 
                " returned successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception getting dimensions");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordDimensionsSet(const char* name, uint width, uint height)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);


            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            // TODO verify args before calling
            if (!recordSinkBintr->SetDimensions(width, height))
            {
                LOG_ERROR("Record Sink '" << name << "' failed to set dimensions");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Width = " << width << " height = " << height << 
                " returned successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception setting dimensions");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordIsOnGet(const char* name, boolean* isOn)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            *isOn = recordSinkBintr->IsOn();

            LOG_INFO("Is on = " << *isOn 
                << "returned successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name 
                << "' threw an exception getting is-recording-on flag");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordResetDoneGet(const char* name, boolean* resetDone)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);

            DSL_RECORD_SINK_PTR recordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            *resetDone = recordSinkBintr->ResetDone();

            LOG_INFO("Reset Done = " << *resetDone 
                << "returned successfully for Record Sink '" << name << "'");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name << "' threw an exception getting reset done flag");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordVideoPlayerAdd(const char* name, 
        const char* player)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
    
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            DSL_RETURN_IF_PLAYER_NAME_NOT_FOUND(m_players, player);
            DSL_RETURN_IF_PLAYER_IS_NOT_VIDEO_PLAYER(m_players, player)

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->AddVideoPlayer(m_players[player]))
            {
                LOG_ERROR("Record Sink '" << name 
                    << "' failed to add Player '" << player << "'");
                return DSL_RESULT_SINK_PLAYER_ADD_FAILED;
            }
            LOG_INFO("Record Sink '" << name 
                << "added Video Player '" << player << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name 
                << "' threw an exception adding Player '" << player << "'");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordVideoPlayerRemove(const char* name, 
        const char* player)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            DSL_RETURN_IF_PLAYER_NAME_NOT_FOUND(m_players, player);
            DSL_RETURN_IF_PLAYER_IS_NOT_VIDEO_PLAYER(m_players, player)

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->RemoveVideoPlayer(m_players[player]))
            {
                LOG_ERROR("Record Sink '" << name 
                    << "' failed to remove Player '" << player << "'");
                return DSL_RESULT_SINK_PLAYER_REMOVE_FAILED;
            }
            LOG_INFO("Record Sink '" << name 
                << "removed Video Player '" << player << "' successfully");
                
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name 
                << "' threw an exception adding Player '" << player << "'");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRecordMailerAdd(const char* name, 
        const char* mailer, const char* subject)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
    
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            DSL_RETURN_IF_MAILER_NAME_NOT_FOUND(m_mailers, mailer);

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->AddMailer(m_mailers[mailer], subject))
            {
                LOG_ERROR("Record Sink '" << name 
                    << "' failed to add Mailer '" << mailer << "'");
                return DSL_RESULT_SINK_MAILER_ADD_FAILED;
            }
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name 
                << "' threw an exception adding Mailer '" << mailer << "'");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
        return DSL_RESULT_SUCCESS;
    }

    DslReturnType Services::SinkRecordMailerRemove(const char* name, 
        const char* mailer)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
    
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RecordSinkBintr);
            DSL_RETURN_IF_MAILER_NAME_NOT_FOUND(m_mailers, mailer);

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[name]);

            if (!pRecordSinkBintr->RemoveMailer(m_mailers[mailer]))
            {
                LOG_ERROR("Record Sink '" << name 
                    << "' failed to remove Mailer '" << mailer << "'");
                return DSL_RESULT_SINK_MAILER_REMOVE_FAILED;
            }
            LOG_INFO("Record Tap '" << name 
                << "added Mailer '" << mailer << "' successfully");
        }
        catch(...)
        {
            LOG_ERROR("Record Sink '" << name 
                << "' threw an exception adding Mailer '" << mailer << "'");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
        return DSL_RESULT_SUCCESS;
    }

    DslReturnType Services::SinkEncodeVideoFormatsGet(const char* name, uint* codec, uint* container)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_ENCODE_SINK(m_components, name);

            DSL_ENCODE_SINK_PTR encodeSinkBintr = 
                std::dynamic_pointer_cast<EncodeSinkBintr>(m_components[name]);

            encodeSinkBintr->GetVideoFormats(codec, container);

            LOG_INFO("Encode Sink '" << name << "' returned Codec = " 
                << *codec << " and Container = " << *container << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("File Sink '" << name << "' threw an exception getting Video formats");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkEncodeSettingsGet(const char* name, uint* bitrate, uint* interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_ENCODE_SINK(m_components, name);

            DSL_ENCODE_SINK_PTR encodeSinkBintr = 
                std::dynamic_pointer_cast<EncodeSinkBintr>(m_components[name]);

            encodeSinkBintr->GetEncoderSettings(bitrate, interval);
            
            LOG_INFO("Encode Sink '" << name << "' returned Bitrate = " 
                << *bitrate << " and Interval = " << *interval << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("File Sink '" << name << "' threw an exception getting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkEncodeSettingsSet(const char* name, uint bitrate, uint interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_ENCODE_SINK(m_components, name);

            if (m_components[name]->IsLinked())
            {
                LOG_ERROR("Unable to set Encoder settings for File Sink '" << name 
                    << "' as it's currently linked");
                return DSL_RESULT_SINK_IS_IN_USE;
            }

            DSL_ENCODE_SINK_PTR encodeSinkBintr = 
                std::dynamic_pointer_cast<EncodeSinkBintr>(m_components[name]);

            if (!encodeSinkBintr->SetEncoderSettings(bitrate, interval))
            {
                LOG_ERROR("Encode Sink '" << name << "' failed to set Encoder settings");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Encode Sink '" << name << "' set Bitrate = " 
                << bitrate << " and Interval = " << interval << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("File Sink'" << name << "' threw an exception setting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRtspNew(const char* name, const char* host, 
            uint udpPort, uint rtspPort, uint codec, uint bitrate, uint interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure component name uniqueness 
            if (m_components.find(name) != m_components.end())
            {   
                LOG_ERROR("Sink name '" << name << "' is not unique");
                return DSL_RESULT_SINK_NAME_NOT_UNIQUE;
            }
            if (codec > DSL_CODEC_H265)
            {   
                LOG_ERROR("Invalid Codec value = " << codec << " for File Sink '" << name << "'");
                return DSL_RESULT_SINK_CODEC_VALUE_INVALID;
            }
            m_components[name] = DSL_RTSP_SINK_NEW(name, host, udpPort, rtspPort, codec, bitrate, interval);

            LOG_INFO("New RTSP Sink '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New RTSP Sink '" << name << "' threw exception on create");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::SinkRtspServerSettingsGet(const char* name, uint* udpPort, uint* rtspPort, uint* codec)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RtspSinkBintr);
            
            DSL_RTSP_SINK_PTR rtspSinkBintr = 
                std::dynamic_pointer_cast<RtspSinkBintr>(m_components[name]);

            rtspSinkBintr->GetServerSettings(udpPort, rtspPort, codec);

            LOG_INFO("RTSP Sink '" << name << "' returned UDP Port = " 
                << *udpPort << ", RTSP Port = " << *rtspPort 
                << ", and Code = " << *codec << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("RTSP Sink '" << name << "' threw an exception getting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRtspEncoderSettingsGet(const char* name, uint* bitrate, uint* interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RtspSinkBintr);

            DSL_RTSP_SINK_PTR rtspSinkBintr = 
                std::dynamic_pointer_cast<RtspSinkBintr>(m_components[name]);

            rtspSinkBintr->GetEncoderSettings(bitrate, interval);

            LOG_INFO("RTSP Sink '" << name << "' returned Bitrate = " 
                << *bitrate << " and Interval = " << *interval << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("RTSP Sink '" << name << "' threw an exception getting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkRtspEncoderSettingsSet(const char* name, uint bitrate, uint interval)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, name, RtspSinkBintr);

            DSL_RTSP_SINK_PTR rtspSinkBintr = 
                std::dynamic_pointer_cast<RtspSinkBintr>(m_components[name]);

            if (m_components[name]->IsInUse())
            {
                LOG_ERROR("Unable to set Encoder settings for RTSP Sink '" << name 
                    << "' as it's currently in use");
                return DSL_RESULT_SINK_IS_IN_USE;
            }

            if (!rtspSinkBintr->SetEncoderSettings(bitrate, interval))
            {
                LOG_ERROR("RTSP Sink '" << name << "' failed to set Encoder settings");
                return DSL_RESULT_SINK_SET_FAILED;
            }
            LOG_INFO("Encode Sink '" << name << "' set Bitrate = " 
                << bitrate << " and Interval = " << interval << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("RTSP Sink '" << name << "' threw an exception setting Encoder settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkPphAdd(const char* name, const char* handler)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_SINK(m_components, name);
            DSL_RETURN_IF_PPH_NAME_NOT_FOUND(m_padProbeHandlers, handler);

            // call on the Handler to add itself to the Tiler as a PadProbeHandler
            if (!m_padProbeHandlers[handler]->AddToParent(m_components[name], DSL_PAD_SINK))
            {
                LOG_ERROR("SINK '" << name << "' failed to add Pad Probe Handler");
                return DSL_RESULT_SINK_HANDLER_ADD_FAILED;
            }
            LOG_INFO("Sink '" << name << "' added Pad Probe Handler successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Sink '" << name << "' threw an exception adding Pad Probe Handler");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }
   
    DslReturnType Services::SinkPphRemove(const char* name, const char* handler) 
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_SINK(m_components, name);
            DSL_RETURN_IF_PPH_NAME_NOT_FOUND(m_padProbeHandlers, handler);

            // call on the Handler to remove itself from the Tee
            if (!m_padProbeHandlers[handler]->RemoveFromParent(m_components[name], DSL_PAD_SINK))
            {
                LOG_ERROR("Pad Probe Handler '" << handler << "' is not a child of Tracker '" << name << "'");
                return DSL_RESULT_SINK_HANDLER_REMOVE_FAILED;
            }
            LOG_INFO("Sink '" << name << "' removed Pad Probe Handler successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Sink '" << name << "' threw an exception removing Pad Probe Handler");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkSyncSettingsGet(const char* name,  boolean* sync, boolean* async)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_SINK(m_components, name);

            DSL_SINK_PTR pSinkBintr = 
                std::dynamic_pointer_cast<SinkBintr>(m_components[name]);

            bool bSync(false), bAsync(false);
            pSinkBintr->GetSyncSettings(&bSync, &bAsync);
            *sync = bSync;
            *async = bAsync;

            LOG_INFO("Sink '" << name << "' return Sync = " 
                << *sync << " and Async = " << *async << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Sink '" << name << "' threw an exception getting  Sync/Async settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::SinkSyncSettingsSet(const char* name,  boolean sync, boolean async)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        try
        {
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, name);
            DSL_RETURN_IF_COMPONENT_IS_NOT_SINK(m_components, name);

            DSL_SINK_PTR pSinkBintr = 
                std::dynamic_pointer_cast<SinkBintr>(m_components[name]);

            if (!pSinkBintr->SetSyncSettings(sync, async))
            {
                LOG_ERROR("Sink '" << name << "' failed to set sync/async attributes");
                return DSL_RESULT_SINK_HANDLER_REMOVE_FAILED;
            }
            LOG_INFO("Sink '" << name << "' set Sync = " 
                << sync << " and Async = " << async << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("Sink '" << name << "' threw an exception setting sync/async settings");
            return DSL_RESULT_SINK_THREW_EXCEPTION;
        }
    }

    uint Services::SinkNumInUseGet()
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        return GetNumSinksInUse();
    }
    
    uint Services::SinkNumInUseMaxGet()
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        return m_sinkNumInUseMax;
    }
    
    boolean Services::SinkNumInUseMaxSet(uint max)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        uint numInUse(0);
        
        if (max < GetNumSinksInUse())
        {
            LOG_ERROR("max setting = " << max << 
                " is less than the current number of Sinks in use = " << numInUse);
            return false;
        }
        m_sinkNumInUseMax = max;
        return true;
    }

    bool Services::IsSinkComponent(const char* component)
    {
        LOG_FUNC();
     
        return (m_components[component]->IsType(typeid(FakeSinkBintr)) or 
            m_components[component]->IsType(typeid(OverlaySinkBintr)) or
            m_components[component]->IsType(typeid(WindowSinkBintr)) or
            m_components[component]->IsType(typeid(FileSinkBintr)) or
            m_components[component]->IsType(typeid(RtspSinkBintr)));
    }
 
    uint Services::GetNumSinksInUse()
    {
        LOG_FUNC();
        
        uint numInUse(0);
        
        for (auto const& imap: m_pipelines)
        {
            numInUse += imap.second->GetNumSinksInUse();
        }
        return numInUse;
    }

}