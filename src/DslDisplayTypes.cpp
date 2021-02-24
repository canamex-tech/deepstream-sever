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

#include "DslDisplayTypes.h"
#include "DslServices.h"

namespace DSL
{

    // ********************************************************************

    
    DisplayType::DisplayType(const char* name)
        : Base(name)
    {
        LOG_FUNC();
    }

    DisplayType::~DisplayType()
    {
        LOG_FUNC();
    }
    
    void DisplayType::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();
        LOG_ERROR("Base Display Type can not be overlaid");
    }
    
    // ********************************************************************

    RgbaColor::RgbaColor(const char* name, double red, double green, double blue, double alpha)
        : DisplayType(name)
        , NvOSD_ColorParams{red, green, blue, alpha}
    {
        LOG_FUNC();
    }

    RgbaColor::~RgbaColor()
    {
        LOG_FUNC();
    }
    
    // ********************************************************************

    RgbaFont::RgbaFont(const char* name, const char* font, uint size, DSL_RGBA_COLOR_PTR color)
        : DisplayType(name)
        , m_fontName(font)
        , NvOSD_FontParams{NULL, size, *color}
    {
        LOG_FUNC();
    }

    RgbaFont::~RgbaFont()
    {
        LOG_FUNC();
    }
    
    // ********************************************************************

    RgbaText::RgbaText(const char* name, const char* text, uint x_offset, uint y_offset, 
        DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , m_text(text)
        , m_pFont(pFont)
        , NvOSD_TextParams{NULL, x_offset, y_offset, 
            *pFont, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    RgbaText::~RgbaText()
    {
        LOG_FUNC();
    }

    void RgbaText::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        NvOSD_TextParams *pTextParams = &pDisplayMeta->text_params[pDisplayMeta->num_labels++];

        // copy over our text params, display_text currently == NULL
        *pTextParams = *this;
        
        // need to allocate storage for actual text, then copy.
        pTextParams->display_text = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        m_text.copy(pTextParams->display_text, MAX_DISPLAY_LEN, 0);
        
        // Font, font-size, font-color
        pTextParams->font_params.font_name = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        m_pFont->m_fontName.copy(pTextParams->font_params.font_name, MAX_DISPLAY_LEN, 0);
    }
        
    // ********************************************************************

    RgbaLine::RgbaLine(const char* name, uint x1, uint y1, uint x2, uint y2, 
        uint width, DSL_RGBA_COLOR_PTR pColor)
        : DisplayType(name)
        , NvOSD_LineParams{x1, y1, x2, y2, width, *pColor}
    {
        LOG_FUNC();
    }

    RgbaLine::~RgbaLine()
    {
        LOG_FUNC();
    }

    void RgbaLine::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        pDisplayMeta->line_params[pDisplayMeta->num_lines++] = *this;
    }
    
    // ********************************************************************

    RgbaArrow::RgbaArrow(const char* name, uint x1, uint y1, uint x2, uint y2, 
        uint width, uint head, DSL_RGBA_COLOR_PTR pColor)
        : DisplayType(name)
        , NvOSD_ArrowParams{x1, y1, x2, y2, width, (NvOSD_Arrow_Head_Direction)head, *pColor}
    {
        LOG_FUNC();
    }

    RgbaArrow::~RgbaArrow()
    {
        LOG_FUNC();
    }


    void RgbaArrow::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        pDisplayMeta->arrow_params[pDisplayMeta->num_arrows++] = *this;
    }

    // ********************************************************************

    RgbaRectangle::RgbaRectangle(const char* name, uint left, uint top, uint width, uint height, 
        uint borderWidth, DSL_RGBA_COLOR_PTR pColor, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , NvOSD_RectParams{(float)left, (float)top, (float)width, (float)height, 
            borderWidth, *pColor, hasBgColor, 0, *pBgColor}
    {
        LOG_FUNC();
    }

    RgbaRectangle::~RgbaRectangle()
    {
        LOG_FUNC();
    }

    void RgbaRectangle::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        pDisplayMeta->rect_params[pDisplayMeta->num_rects++] = *this;
    }
    
    // ********************************************************************

    RgbaCircle::RgbaCircle(const char* name, uint x_center, uint y_center, uint radius,
        DSL_RGBA_COLOR_PTR pColor, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , NvOSD_CircleParams{x_center, y_center, radius, *pColor, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    RgbaCircle::~RgbaCircle()
    {
        LOG_FUNC();
    }

    void RgbaCircle::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        pDisplayMeta->circle_params[pDisplayMeta->num_circles++] = *this;
    }

    // ********************************************************************

    SourceDimensions::SourceDimensions(const char* name, uint x_offset, uint y_offset, 
        DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , m_pFont(pFont)
        , NvOSD_TextParams{NULL, x_offset, y_offset, 
            *pFont, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    SourceDimensions::~SourceDimensions()
    {
        LOG_FUNC();
    }

    void SourceDimensions::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        NvOSD_TextParams *pTextParams = &pDisplayMeta->text_params[pDisplayMeta->num_labels++];

        // copy over our text params, display_text currently == NULL
        *pTextParams = *this;
        
        std::string text = std::to_string(pFrameMeta->source_frame_width) + " x " + 
            std::to_string(pFrameMeta->source_frame_height);

        // need to allocate storage for actual text, then copy.
        pTextParams->display_text = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        text.copy(pTextParams->display_text, MAX_DISPLAY_LEN, 0);
        
        // Font, font-size, font-color
        pTextParams->font_params.font_name = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        m_pFont->m_fontName.copy(pTextParams->font_params.font_name, MAX_DISPLAY_LEN, 0);
    }

    // ********************************************************************

    SourceFrameRate::SourceFrameRate(const char* name, uint x_offset, uint y_offset, 
        DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , m_pFont(pFont)
        , NvOSD_TextParams{NULL, x_offset, y_offset, 
            *pFont, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    SourceFrameRate::~SourceFrameRate()
    {
        LOG_FUNC();
    }

    void SourceFrameRate::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        NvOSD_TextParams *pTextParams = &pDisplayMeta->text_params[pDisplayMeta->num_labels++];

        // copy over our text params, display_text currently == NULL
        *pTextParams = *this;
        
        std::string text = std::to_string(pFrameMeta->source_frame_width) + " x " + 
            std::to_string(pFrameMeta->source_frame_height);

        // need to allocate storage for actual text, then copy.
        pTextParams->display_text = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        text.copy(pTextParams->display_text, MAX_DISPLAY_LEN, 0);
        
        // Font, font-size, font-color
        pTextParams->font_params.font_name = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        m_pFont->m_fontName.copy(pTextParams->font_params.font_name, MAX_DISPLAY_LEN, 0);
    }
    
    // ********************************************************************

    SourceNumber::SourceNumber(const char* name, uint x_offset, uint y_offset, 
        DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , m_pFont(pFont)
        , NvOSD_TextParams{NULL, x_offset, y_offset, 
            *pFont, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    SourceNumber::~SourceNumber()
    {
        LOG_FUNC();
    }

    void SourceNumber::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        NvOSD_TextParams *pTextParams = &pDisplayMeta->text_params[pDisplayMeta->num_labels++];

        // copy over our text params, display_text currently == NULL
        *pTextParams = *this;
        
        std::string numberString(std::to_string(pFrameMeta->source_id));
        // need to allocate storage for actual text, then copy.
        pTextParams->display_text = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        numberString.copy(pTextParams->display_text, MAX_DISPLAY_LEN, 0);
        
        // Font, font-size, font-color
        pTextParams->font_params.font_name = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
        m_pFont->m_fontName.copy(pTextParams->font_params.font_name, MAX_DISPLAY_LEN, 0);
        
    }

    // ********************************************************************

    SourceName::SourceName(const char* name, uint x_offset, uint y_offset, 
        DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor)
        : DisplayType(name)
        , m_pFont(pFont)
        , NvOSD_TextParams{NULL, x_offset, y_offset, 
            *pFont, hasBgColor, *pBgColor}
    {
        LOG_FUNC();
    }

    SourceName::~SourceName()
    {
        LOG_FUNC();
    }

    void SourceName::AddMeta(NvDsDisplayMeta* pDisplayMeta, NvDsFrameMeta* pFrameMeta) 
    {
        LOG_FUNC();

        NvOSD_TextParams *pTextParams = &pDisplayMeta->text_params[pDisplayMeta->num_labels++];

        // copy over our text params, display_text currently == NULL
        *pTextParams = *this;
        
        const char* name;
        
        if (Services::GetServices()->SourceNameGet(pFrameMeta->source_id, &name) == DSL_RESULT_SUCCESS)
        {
            std::string nameString(name);
            // need to allocate storage for actual text, then copy.
            pTextParams->display_text = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
            nameString.copy(pTextParams->display_text, MAX_DISPLAY_LEN, 0);
            
            // Font, font-size, font-color
            pTextParams->font_params.font_name = (gchar*) g_malloc0(MAX_DISPLAY_LEN);
            m_pFont->m_fontName.copy(pTextParams->font_params.font_name, MAX_DISPLAY_LEN, 0);
        }
        
    }
}
    
