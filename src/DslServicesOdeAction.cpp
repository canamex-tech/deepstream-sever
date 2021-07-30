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
#include "DslOdeAction.h"

namespace DSL
{
    DslReturnType Services::OdeActionCustomNew(const char* name,
        dsl_ode_handle_occurrence_cb clientHandler, void* clientData)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_CUSTOM_NEW(name, clientHandler, clientData);

            LOG_INFO("New ODE Callback Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Callback Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionCaptureFrameNew(const char* name,
        const char* outdir, boolean annotate)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            // ensure outdir exists
            struct stat info;
            if ((stat(outdir, &info) != 0) or !(info.st_mode & S_IFDIR))
            {
                LOG_ERROR("Unable to access outdir '" << outdir << "' for Capture Action '" << name << "'");
                return DSL_RESULT_ODE_ACTION_FILE_PATH_NOT_FOUND;
            }
            m_odeActions[name] = DSL_ODE_ACTION_CAPTURE_FRAME_NEW(name, outdir, annotate);

            LOG_INFO("New Capture Frame ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Capture Frame ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionCaptureObjectNew(const char* name,
        const char* outdir)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            // ensure outdir exists
            struct stat info;
            if ((stat(outdir, &info) != 0) or !(info.st_mode & S_IFDIR))
            {
                LOG_ERROR("Unable to access outdir '" << outdir << "' for Capture Action '" << name << "'");
                return DSL_RESULT_ODE_ACTION_FILE_PATH_NOT_FOUND;
            }
            m_odeActions[name] = DSL_ODE_ACTION_CAPTURE_OBJECT_NEW(name, outdir);

            LOG_INFO("New Capture Object ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Capture Object ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionCaptureCompleteListenerAdd(const char* name, 
        dsl_capture_complete_listener_cb listener, void* clientData)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);   

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->AddCaptureCompleteListener(listener, clientData))
            {
                LOG_ERROR("ODE Capture Action '" << name 
                    << "' failed to add a Capture Complete Listener");
                return DSL_RESULT_ODE_ACTION_CALLBACK_ADD_FAILED;
            }
            LOG_INFO("ODE Capture Action '" << name << "' added Listener successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception adding a Capture Complete Lister");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
        
    DslReturnType Services::OdeActionCaptureCompleteListenerRemove(const char* name, 
        dsl_capture_complete_listener_cb listener)
    {
        LOG_FUNC();
    
        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);   

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->RemoveCaptureCompleteListener(listener))
            {
                LOG_ERROR("Capture Action '" << name 
                    << "' failed to add a Capture Complete Listener");
                return DSL_RESULT_ODE_ACTION_CALLBACK_REMOVE_FAILED;
            }
            LOG_INFO("ODE Capture Action '" << name << "' added Listener successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception adding a Capture Complete Lister");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionCaptureImagePlayerAdd(const char* name, 
        const char* player)
    {
        LOG_FUNC();
    
        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);
            DSL_RETURN_IF_PLAYER_NAME_NOT_FOUND(m_players, player);
            DSL_RETURN_IF_PLAYER_IS_NOT_IMAGE_PLAYER(m_players, player)

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->AddImagePlayer(m_players[player]))
            {
                LOG_ERROR("Capture Action '" << name 
                    << "' failed to add Player '" << player << "'");
                return DSL_RESULT_ODE_ACTION_PLAYER_ADD_FAILED;
            }
            LOG_INFO("ODE Capture Action '" << name << "' added Image Player '"
                << player << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception adding Player '" << player << "'");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionCaptureImagePlayerRemove(const char* name, 
        const char* player)
    {
        LOG_FUNC();
    
        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);
            DSL_RETURN_IF_PLAYER_NAME_NOT_FOUND(m_players, player);
            DSL_RETURN_IF_PLAYER_IS_NOT_IMAGE_PLAYER(m_players, player)

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->RemoveImagePlayer(m_players[player]))
            {
                LOG_ERROR("Capture Action '" << name 
                    << "' failed to remove Player '" << player << "'");
                return DSL_RESULT_ODE_ACTION_PLAYER_REMOVE_FAILED;
            }
            LOG_INFO("ODE Capture Action '" << name << "' Removed Image Player '"
                << player << "' successfully");
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception removeing Player '" << player << "'");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
        return DSL_RESULT_SUCCESS;
    }
    
    DslReturnType Services::OdeActionCaptureMailerAdd(const char* name, 
        const char* mailer, const char* subject, boolean attach)
    {
        LOG_FUNC();
    
        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);
            DSL_RETURN_IF_MAILER_NAME_NOT_FOUND(m_mailers, mailer);

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->AddMailer(m_mailers[mailer], subject, attach))
            {
                LOG_ERROR("Capture Action '" << name 
                    << "' failed to add Mailer '" << mailer << "'");
                return DSL_RESULT_ODE_ACTION_MAILER_ADD_FAILED;
            }
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception adding Mailer '" << mailer << "'");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
        return DSL_RESULT_SUCCESS;
    }

    DslReturnType Services::OdeActionCaptureMailerRemove(const char* name, 
        const char* mailer)
    {
        LOG_FUNC();
    
        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CAPTURE_TYPE(m_odeActions, name);
            DSL_RETURN_IF_MAILER_NAME_NOT_FOUND(m_mailers, mailer);

            DSL_ODE_ACTION_CATPURE_PTR pOdeAction = 
                std::dynamic_pointer_cast<CaptureOdeAction>(m_odeActions[name]);

            if (!pOdeAction->RemoveMailer(m_mailers[mailer]))
            {
                LOG_ERROR("Capture Action '" << name 
                    << "' failed to remove Mailer '" << mailer << "'");
                return DSL_RESULT_ODE_ACTION_MAILER_REMOVE_FAILED;
            }
            LOG_INFO("ODE Capture Action '" << name << "' removed Mailer '"
                << mailer << "' successfully");
                
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Capture Action '" << name 
                << "' threw an exception removeing Player '" << mailer << "'");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionDisplayNew(const char* name, uint offsetX, uint offsetY, 
        boolean offsetYWithClassId, const char* font, boolean hasBgColor, const char* bgColor)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, font);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_NOT_CORRECT_TYPE(m_displayTypes, font, RgbaFont);
            
            DSL_RGBA_COLOR_PTR pBgColor(nullptr);
            if (hasBgColor)
            {
                DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, bgColor);
                DSL_RETURN_IF_DISPLAY_TYPE_IS_NOT_CORRECT_TYPE(m_displayTypes, bgColor, RgbaColor);

                pBgColor = std::dynamic_pointer_cast<RgbaColor>(m_displayTypes[bgColor]);
            }
            else
            {
                pBgColor = DSL_RGBA_COLOR_NEW("_no_color_", 0.0, 0.0, 0.0, 0.0);
            }

            DSL_RGBA_FONT_PTR pFont = 
                std::dynamic_pointer_cast<RgbaFont>(m_displayTypes[font]);

            m_odeActions[name] = DSL_ODE_ACTION_DISPLAY_NEW(name, 
                offsetX, offsetY, offsetYWithClassId, pFont, hasBgColor, pBgColor);
                
            LOG_INFO("New Display ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Display ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionEmailNew(const char* name, 
        const char* mailer, const char* subject)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            DSL_RETURN_IF_MAILER_NAME_NOT_FOUND(m_mailers, mailer)
            
            m_odeActions[name] = DSL_ODE_ACTION_EMAIL_NEW(name, 
                m_mailers[mailer], subject);

            LOG_INFO("New ODE Email Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Email Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionFileNew(const char* name, 
        const char* filePath, uint mode, uint format, boolean forceFlush)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            if (mode > DSL_EVENT_FILE_MODE_TRUNCATE)
            {
                LOG_ERROR("File open mode " << mode 
                    << " is invalid for ODE Action '" << name << "'");
                return DSL_RESULT_ODE_ACTION_PARAMETER_INVALID;
            }
            if (format > DSL_EVENT_FILE_FORMAT_CSV)
            {
                LOG_ERROR("File format " << format 
                    << " is invalid for ODE Action '" << name << "'");
                return DSL_RESULT_ODE_ACTION_PARAMETER_INVALID;
            }
            m_odeActions[name] = DSL_ODE_ACTION_FILE_NEW(name, 
                filePath, mode, format, forceFlush);

            LOG_INFO("New ODE File Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE File Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionFillSurroundingsNew(const char* name, const char* color)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, color);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_NOT_CORRECT_TYPE(m_displayTypes, color, RgbaColor);

            DSL_RGBA_COLOR_PTR pColor = 
                std::dynamic_pointer_cast<RgbaColor>(m_displayTypes[color]);
                
            m_odeActions[name] = DSL_ODE_ACTION_FILL_SURROUNDINGS_NEW(name, pColor);

            LOG_INFO("New ODE Fill Surroundings Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Fill Surroundings Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionFillFrameNew(const char* name, const char* color)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, color);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_NOT_CORRECT_TYPE(m_displayTypes, color, RgbaColor);

            DSL_RGBA_COLOR_PTR pColor = 
                std::dynamic_pointer_cast<RgbaColor>(m_displayTypes[color]);
                
            m_odeActions[name] = DSL_ODE_ACTION_FILL_FRAME_NEW(name, pColor);

            LOG_INFO("New ODE Fill Frame Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Fill Frame Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionFillObjectNew(const char* name, const char* color)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }

            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, color);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_NOT_CORRECT_TYPE(m_displayTypes, color, RgbaColor);

            DSL_RGBA_COLOR_PTR pColor = 
                std::dynamic_pointer_cast<RgbaColor>(m_displayTypes[color]);
                
            m_odeActions[name] = DSL_ODE_ACTION_FILL_OBJECT_NEW(name, pColor);

            LOG_INFO("New ODE Fill Object Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Fill Object Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionHandlerDisableNew(const char* name, const char* handler)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_DISABLE_HANDLER_NEW(name, handler);

            LOG_INFO("New ODE Disable Handler Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Disable Handler Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionHideNew(const char* name, boolean text, boolean border)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_HIDE_NEW(name, text, border);

            LOG_INFO("New ODE Hide Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Hide Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionLogNew(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_LOG_NEW(name);

            LOG_INFO("New ODE Log Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Log Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionDisplayMetaAddNew(const char* name, const char* displayType)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, displayType);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_BASE_TYPE(m_displayTypes, displayType);
            
            DSL_DISPLAY_TYPE_PTR pDisplayType = std::dynamic_pointer_cast<DisplayType>(m_displayTypes[displayType]);
            
            m_odeActions[name] = DSL_ODE_ACTION_DISPLAY_META_ADD_NEW(name, pDisplayType);

            LOG_INFO("New Add Display Meta Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Add Display Meta Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionDisplayMetaAddDisplayType(const char* name, const char* displayType)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            DSL_RETURN_IF_ODE_ACTION_IS_NOT_CORRECT_TYPE(m_odeActions, name, AddDisplayMetaOdeAction);
            DSL_RETURN_IF_DISPLAY_TYPE_NAME_NOT_FOUND(m_displayTypes, displayType);
            DSL_RETURN_IF_DISPLAY_TYPE_IS_BASE_TYPE(m_displayTypes, displayType);
            
            DSL_DISPLAY_TYPE_PTR pDisplayType = std::dynamic_pointer_cast<DisplayType>(m_displayTypes[displayType]);
            
            DSL_ODE_ACTION_DISPLAY_META_ADD_PTR pAction = 
                std::dynamic_pointer_cast<AddDisplayMetaOdeAction>(m_odeActions[name]);

            pAction->AddDisplayType(pDisplayType);
            
            LOG_INFO("Display Type '" << displayType << "' added to Action '" << name << "' successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Overlay Frame Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    
    DslReturnType Services::OdeActionPauseNew(const char* name, const char* pipeline)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_PAUSE_NEW(name, pipeline);

            LOG_INFO("New ODE Pause Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Pause Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionPrintNew(const char* name,
        boolean forceFlush)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_PRINT_NEW(name, forceFlush);

            LOG_INFO("New ODE Print Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Print Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionRedactNew(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_REDACT_NEW(name);

            LOG_INFO("New ODE Redact Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Redact Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionSinkAddNew(const char* name, 
        const char* pipeline, const char* sink)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_SINK_ADD_NEW(name, pipeline, sink);

            LOG_INFO("New Sink Add ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionSinkRemoveNew(const char* name, 
        const char* pipeline, const char* sink)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_SINK_REMOVE_NEW(name, pipeline, sink);

            LOG_INFO("New Sink Remove ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Sink Remove ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionSinkRecordStartNew(const char* name,
        const char* recordSink, uint start, uint duration, void* clientData)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }

            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, recordSink);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, recordSink, RecordSinkBintr);

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[recordSink]);
            
            m_odeActions[name] = DSL_ODE_ACTION_SINK_RECORD_START_NEW(name,
                pRecordSinkBintr, start, duration, clientData);

            LOG_INFO("New ODE Record Sink Start Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Record Start Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionSinkRecordStopNew(const char* name,
        const char* recordSink)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }

            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, recordSink);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, recordSink, RecordSinkBintr);

            DSL_RECORD_SINK_PTR pRecordSinkBintr = 
                std::dynamic_pointer_cast<RecordSinkBintr>(m_components[recordSink]);
            
            m_odeActions[name] = DSL_ODE_ACTION_SINK_RECORD_STOP_NEW(name,
                pRecordSinkBintr);

            LOG_INFO("New ODE Record Sink Stop Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Record Stop Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionSourceAddNew(const char* name, 
        const char* pipeline, const char* source)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_SOURCE_ADD_NEW(name, pipeline, source);

            LOG_INFO("New Source Add ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Source Add ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionSourceRemoveNew(const char* name, 
        const char* pipeline, const char* source)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_SOURCE_REMOVE_NEW(name, pipeline, source);

            LOG_INFO("New Source Remove ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Source Remove ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTapRecordStartNew(const char* name,
        const char* recordTap, uint start, uint duration, void* clientData)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, recordTap);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, recordTap, RecordTapBintr);

            DSL_RECORD_TAP_PTR pRecordTapBintr = 
                std::dynamic_pointer_cast<RecordTapBintr>(m_components[recordTap]);

            m_odeActions[name] = DSL_ODE_ACTION_TAP_RECORD_START_NEW(name,
                pRecordTapBintr, start, duration, clientData);

            LOG_INFO("New ODE Record Tap Start Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Record Tap Start Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTapRecordStopNew(const char* name,
        const char* recordTap)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            
            DSL_RETURN_IF_COMPONENT_NAME_NOT_FOUND(m_components, recordTap);
            DSL_RETURN_IF_COMPONENT_IS_NOT_CORRECT_TYPE(m_components, recordTap, RecordTapBintr);

            DSL_RECORD_TAP_PTR pRecordTapBintr = 
                std::dynamic_pointer_cast<RecordTapBintr>(m_components[recordTap]);
            
            m_odeActions[name] = DSL_ODE_ACTION_TAP_RECORD_STOP_NEW(name, pRecordTapBintr);

            LOG_INFO("New ODE Record Tap Stop Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New ODE Record Tap Stop Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionActionDisableNew(const char* name, const char* action)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_ACTION_DISABLE_NEW(name, action);

            LOG_INFO("New Action Disable ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Action Disable ODE Action'" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionActionEnableNew(const char* name, const char* action)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_ACTION_ENABLE_NEW(name, action);

            LOG_INFO("New Action Enable ODE Action'" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Action Enable ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTilerShowSourceNew(const char* name, 
        const char* tiler, uint timeout, bool hasPrecedence)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_TILER_SHOW_SOURCE_NEW(name, tiler, timeout, hasPrecedence);

            LOG_INFO("New Tiler Show Source ODE Action'" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Tiler Show Source ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTriggerResetNew(const char* name, const char* trigger)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_TRIGGER_RESET_NEW(name, trigger);

            LOG_INFO("New Trigger Reset ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Trigger Reset ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTriggerDisableNew(const char* name, const char* trigger)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_TRIGGER_DISABLE_NEW(name, trigger);

            LOG_INFO("New Trigger Disable ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Trigger Disable ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionTriggerEnableNew(const char* name, const char* trigger)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_TRIGGER_ENABLE_NEW(name, trigger);

            LOG_INFO("New Trigger Enable ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Trigger Enable ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionAreaAddNew(const char* name, 
        const char* trigger, const char* area)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_AREA_ADD_NEW(name, trigger, area);

            LOG_INFO("New Area Add ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Area Add ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionAreaRemoveNew(const char* name, 
        const char* trigger, const char* area)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            // ensure event name uniqueness 
            if (m_odeActions.find(name) != m_odeActions.end())
            {   
                LOG_ERROR("ODE Action name '" << name << "' is not unique");
                return DSL_RESULT_ODE_ACTION_NAME_NOT_UNIQUE;
            }
            m_odeActions[name] = DSL_ODE_ACTION_AREA_REMOVE_NEW(name, trigger, area);

            LOG_INFO("New Area Remove ODE Action '" << name << "' created successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("New Area Remove ODE Action '" << name << "' threw exception on create");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    DslReturnType Services::OdeActionEnabledGet(const char* name, boolean* enabled)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            
            DSL_ODE_ACTION_PTR pOdeAction = 
                std::dynamic_pointer_cast<OdeAction>(m_odeActions[name]);
         
            *enabled = pOdeAction->GetEnabled();

            LOG_INFO("ODE Action '" << name << "' returned Enabed = " 
                << *enabled  << " successfully");
            
            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Action '" << name << "' threw exception getting Enabled setting");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }                

    DslReturnType Services::OdeActionEnabledSet(const char* name, boolean enabled)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            
            DSL_ODE_ACTION_PTR pOdeAction = 
                std::dynamic_pointer_cast<OdeAction>(m_odeActions[name]);
         
            pOdeAction->SetEnabled(enabled);

            LOG_INFO("ODE Action '" << name << "' set Enabed = " 
                << enabled  << " successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Action '" << name << "' threw exception setting Enabled");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }                

    DslReturnType Services::OdeActionDelete(const char* name)
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            DSL_RETURN_IF_ODE_ACTION_NAME_NOT_FOUND(m_odeActions, name);
            
            if (m_odeActions[name].use_count() > 1)
            {
                LOG_INFO("ODE Action'" << name << "' is in use");
                return DSL_RESULT_ODE_ACTION_IN_USE;
            }
            m_odeActions.erase(name);

            LOG_INFO("ODE Action '" << name << "' deleted successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Action '" << name << "' threw exception on deletion");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }
    
    DslReturnType Services::OdeActionDeleteAll()
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);

        try
        {
            if (m_odeActions.empty())
            {
                return DSL_RESULT_SUCCESS;
            }
            for (auto const& imap: m_odeActions)
            {
                // In the case of Delete all
                if (imap.second.use_count() > 1)
                {
                    LOG_ERROR("ODE Action '" << imap.second->GetName() << "' is currently in use");
                    return DSL_RESULT_ODE_ACTION_IN_USE;
                }
            }
            m_odeActions.clear();

            LOG_INFO("All ODE Actions deleted successfully");

            return DSL_RESULT_SUCCESS;
        }
        catch(...)
        {
            LOG_ERROR("ODE Action threw exception on delete all");
            return DSL_RESULT_ODE_ACTION_THREW_EXCEPTION;
        }
    }

    uint Services::OdeActionListSize()
    {
        LOG_FUNC();
        LOCK_MUTEX_FOR_CURRENT_SCOPE(&m_servicesMutex);
        
        return m_odeActions.size();
    }
}