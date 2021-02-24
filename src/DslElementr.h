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

#ifndef _DSL_ELEMENTR_H
#define _DSL_ELEMENTR_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslNodetr.h"

namespace DSL
{

    /**
     * @brief convenience macros for shared pointer abstraction
     */
    #define DSL_ELEMENT_PTR std::shared_ptr<Elementr>
    #define DSL_ELEMENT_NEW(type, name) \
        std::shared_ptr<Elementr>(new Elementr(type, name))   

    /**
     * @class Elementr
     * @brief Implements a container class for a GST Element
     */
    class Elementr : public GstNodetr
    {
    public:

        /**
         * @brief ctor for the container class
         * @brief[in] factoryname unique GST factory name to create from
         * @brief[in] name unique name for the Element
         */
        Elementr(const char* factoryName, const char* name)
            : GstNodetr(name)
            , m_factoryName(factoryName)
        { 
            LOG_FUNC(); 
            
            m_pGstObj = GST_OBJECT(gst_element_factory_make(factoryName, name));
            if (!m_pGstObj)
            {
                LOG_ERROR("Failed to create new Element '" << name << "'");
                throw;  
            }
        };
        
        /**
         * @brief ctor for the GST Element container class
         */
        ~Elementr()
        {
            LOG_FUNC();
        };

        /**
         * @brief Gets a GST Element's attribute of type int, owned by this Elementr
         * @param[in] name name of the attribute to set
         * @param[out] value unsigned integer value to set the attribute
         */
        void GetAttribute(const char* name, int* value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to uint value '" << value << "'");
            
            g_object_get(GetGObject(), name, value, NULL);
        }

        /**
         * @brief Sets a GST Element's attribute, owned by this Elementr to a value of int
         * @param[in] name name of the attribute to set
         * @param[in] value unsigned integer value to set the attribute
         */
        void SetAttribute(const char* name, int value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to uint value '" << value << "'");
            
            g_object_set(GetGObject(), name, value, NULL);
        }
        
        /**
         * @brief Gets a GST Element's attribute of type uint, owned by this Elementr
         * @param[in] name name of the attribute to set
         * @param[out] value unsigned integer value to set the attribute
         */
        void GetAttribute(const char* name, uint* value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to uint value '" << value << "'");
            
            g_object_get(GetGObject(), name, value, NULL);
        }

        /**
         * @brief Sets a GST Element's attribute, owned by this Elementr to a value of uint
         * @param[in] name name of the attribute to set
         * @param[in] value unsigned integer value to set the attribute
         */
        void SetAttribute(const char* name, uint value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to uint value '" << value << "'");
            
            g_object_set(GetGObject(), name, value, NULL);
        }
        
        /**
         * @brief Sets a GST Element's attribute, owned by this Elementr to a 
         * null terminated array of characters (char*)
         * @param[in] name name of the attribute to set
         * @param[in] value char* string value to set the attribute
         */
        void SetAttribute(const char* name, const char* value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to char* value '" << value << "'");
            
            g_object_set(GetGObject(), name, value, NULL);
        }
        
        /**
         * @brief Sets a GST Element's attribute, owned by this Elementr to a 
         * value of type GstCaps, created with one of gst_caps_new_* 
         * @param[in] name name of the attribute to set
         * @param[in] value char* string value to set the attribute
         */
        void SetAttribute(const char* name, const GstCaps * value)
        {
            LOG_FUNC();
            
            LOG_DEBUG("Setting attribute '" << name << "' to GstCaps* value '" << value << "'");
            
            g_object_set(GetGObject(), name, value, NULL);
        }
        
        bool IsFactoryName(const char* factoryName)
        {
            LOG_FUNC();
            
            LOG_INFO("commparing expected factory'" << factoryName << "' with actual '" 
                << m_factoryName.c_str() << "' for element '" << GetName() << "'");
            
            std::string expectedName(factoryName);
            return (expectedName == m_factoryName);
        }
        
    private:
    
        std::string m_factoryName;
    };
}

#endif // _DSL_ELEMENTR_H   