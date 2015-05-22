/*!

@file
@verbatim
$Header$

Copyright (c) 2008 DivXNetworks, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#ifndef DIVXFONT_H
#define DIVXFONT_H

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXMem.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*! Opaque DivXFont handle, the structure is defined in DivXFont.c */
typedef struct _DivXFont *DivXFont;

/*! Structure used for output font bitmap */
typedef struct  _DivXFontBitmap
{
    int32_t         rows;
    int32_t         width;
    int32_t         pitch;
    uint8_t         *buffer;
    uint16_t        num_grays;
    int8_t          palette_mode;
    int8_t        	palette;
}
DivXFontBitmap;

/*! Encoding type definition */
#define DIVXFONT_ENCODING_NONE           ft_encoding_none
#define DIVXFONT_ENCODING_UNICODE        ft_encoding_unicode
#define DIVXFONT_ENCODING_SYMBOL         ft_encoding_symbol
#define DIVXFONT_ENCODING_LATIN_1        ft_encoding_latin_1
#define DIVXFONT_ENCODING_LATIN_2        ft_encoding_latin_2
#define DIVXFONT_ENCODING_SJIS           ft_encoding_sjis
#define DIVXFONT_ENCODING_GB2312         ft_encoding_gb2312
#define DIVXFONT_ENCODING_BIG5           ft_encoding_big5
#define DIVXFONT_ENCODING_WANSUNG        ft_encoding_wansung
#define DIVXFONT_ENCODING_JOHAB          ft_encoding_johab
#define DIVXFONT_ENCODING_ADOBE_STANDARD ft_encoding_adobe_standard
#define DIVXFONT_ENCODING_ADOBE_EXPERT   ft_encoding_adobe_expert
#define DIVXFONT_ENCODING_ADOBE_CUSTOM   ft_encoding_adobe_custom
#define DIVXFONT_ENCODING_APPLE_ROMAN    ft_encoding_apple_roman

/*! A list of values that are used to select a specific hinting algorithm
   to be used by the hinter, one and only one of the following flags need to
   be Ored when calling DivXFont_SetOptions */
/*! @todo: !!!only LCD mode is supported now */
#define DIVXFONT_LOAD_TARGET_NORMAL      FT_LOAD_TARGET_NORMAL
#define DIVXFONT_LOAD_TARGET_LIGHT       FT_LOAD_TARGET_LIGHT
#define DIVXFONT_LOAD_TARGET_MONO        FT_LOAD_TARGET_MONO  /*! Anti-aliase is off for this mode */
#define DIVXFONT_LOAD_TARGET_LCD         FT_LOAD_TARGET_LCD
#define DIVXFONT_LOAD_TARGET_LCD_V       FT_LOAD_TARGET_LCD_V

/*! A list of bit-field constants used with DivXFont_SetOptions to indicate
    what kind of operations to perform during glyph loading */
#define DIVXFONT_LOAD_DEFAULT                       FT_LOAD_DEFAULT
#define DIVXFONT_LOAD_NO_SCALE                      FT_LOAD_NO_SCALE
#define DIVXFONT_LOAD_NO_HINTING                    FT_LOAD_NO_HINTING
#define DIVXFONT_LOAD_RENDER                        FT_LOAD_RENDER
#define DIVXFONT_LOAD_NO_BITMAP                     FT_LOAD_NO_BITMAP
#define DIVXFONT_LOAD_VERTICAL_LAYOUT               FT_LOAD_VERTICAL_LAYOUT
#define DIVXFONT_LOAD_FORCE_AUTOHINT                FT_LOAD_FORCE_AUTOHINT
#define DIVXFONT_LOAD_CROP_BITMAP                   FT_LOAD_CROP_BITMAP
#define DIVXFONT_LOAD_PEDANTIC                      FT_LOAD_PEDANTIC
#define DIVXFONT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH   FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH
#define DIVXFONT_LOAD_NO_RECURSE                    FT_LOAD_NO_RECURSE
#define DIVXFONT_LOAD_IGNORE_TRANSFORM              FT_LOAD_IGNORE_TRANSFORM
#define DIVXFONT_LOAD_MONOCHROME                    FT_LOAD_MONOCHROME
#define DIVXFONT_LOAD_LINEAR_DESIGN                 FT_LOAD_LINEAR_DESIGN
#define DIVXFONT_LOAD_NO_AUTOHINT                   FT_LOAD_NO_AUTOHINT


/*!
 This will create a new font instance using the memory manager provided. It performs
 any underlying font object initialization including Freetype2 library initialization.

  @param phFont(OUT) - On completion, this handle should be stored and used in all
                        subsequent calls for accessing data on the specific font processing
  @param hMem   (IN) - DThe memory manager.

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_New(
    DivXFont    *phFont,
    DivXMem     hMem);

/*!
 This will destroy the font object.

  @param hFont (IN) - Font handle

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_Delete(
    DivXFont    hFont);

/*!
 This set the encoding of the char set so that the proper char map could be used.

  @param hFont   (IN) - Font handle
  @param encoding(IN) – The char encoding set. The supported encoding method should
                        be enumerated.

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_SetOptions(
    DivXFont    hFont,
    uint32_t    encoding,
    uint32_t    options);

/*!
 This will load the font char maps and glyphs from a file.

  @param hFont       (IN) - Font handle
  @param fontFilePath(IN) – Path to the font file
  @param offset      (IN) – Offset from the start of the file. This is the position
                            where font file starts
  @param size        (IN) – Font File Size. If the size is 0,  the file size will be
                            set as input size

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_InstallFontFile(
    DivXFont        hFont,
    const uint8_t   *fontFilePath,
    int32_t         offset,
    int32_t         size);

/*!
 This will load the font char maps and glyphs from memory.

  @param hFont       (IN) - Font handle
  @param mem         (IN) – Pointer to the first byte of the memory
  @param size        (IN) – Size of the font file data in memory

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_InstallFontMem(
    DivXFont    hFont,
    int8_t      *mem,
    int32_t     size);

/*!
 This sets the current active font if there're multiple fonts installed.
 If this API is not called, the default active font is set to the font with index 0

  @param hFont       (IN) - Font handle
  @param index       (IN) – Font index

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_SetActiveFont(
    DivXFont    hFont, 
    int8_t      index);

/*!
 This scales the glyph based on the display resolution and font size.

  @param hFont       (IN) - Font handle
  @param sizePt      (IN) – Font size in pt uint
  @param mem         (IN) – Horizontal resolution size
  @param mem         (IN) – Vertical resolution size

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_SetPixelSize(
    DivXFont    hFont,
    int32_t     sizePt,
    int32_t     resX,
    int32_t     resY);

/*!
 This loads the glyph by the given char code and stores it internally. If the glyph needs
 transform, set useCacheForGlyph to false. If a glyph is loaded through cache, it couldn't
 be transformed. Note: There is only one internal slot for glyph.

  @param hFont       (IN) - Font handle
  @param pCharCode   (IN) – Pointer to the character code
  @param charLength  (IN) – The length of the input char code
  @param useCacheForGlyph  (IN) – Use image chache or not

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_LoadGlyph(
    DivXFont    hFont,
    uint8_t     *pCharCode,
    int32_t     charLength,
    DivXBool    useCacheForGlyph);

/*!
 This emboldens the glyph.

  @param hFont       (IN) - Font handle

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_EmboldenGlyph(
   DivXFont     hFont);

/*!
 This converts the glyph into bitmap based on the render mode. The bitmap is owned
 by DivXFont lib. DivXFont_ReleaseGlyph should be called to relase the memory.

  @param hFont       (IN)  - Font handle
  @param pBitmap     (OUT) – Contains all information regarding the converted bitmap
  @param pLeft       (OUT) - Point to glyph left bearing
  @param pTop        (OUT) - Point to glyph top bearing
  @param pXAdvance   (OUT) - Point to glyph x advance
  @param pYAdvance   (OUT) - Point to glyph y advance

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_RenderGlyph(
    DivXFont                    hFont,
    DivXFontBitmap              *pBitmap,
    int32_t                     *pLeft, 
    int32_t                     *pTop,
    int32_t                     *pXAdvance,
    int32_t                     *pYAdvance);

/*!
 This release the glyph resource

  @param hFont       (IN) - Font handle

  @return DIVX_ERR_SUCCESS for success, DIVX_ERR_FAILURE otherwise
 */
DivXError DivXFont_ReleaseGlyph(
    DivXFont hFont);

#ifdef __cplusplus
}
#endif

#endif
