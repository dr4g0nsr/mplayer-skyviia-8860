/*!
    @file
@verbatim
$Id: avirReadStructures.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIRREADSTRUCTURES_H_
#define _AVIRREADSTRUCTURES_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVI1Read/ReadBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXError ReadAVIStructureHeader( RDBufferHandle      handleBuffer,
                                  AVIStructureHeader *header,
                                  uint64_t            offset,
                                  void               *userData,
                                  uint32_t            hint );

DivXError ReadAVIChunkHeader( RDBufferHandle  handleBuffer,
                              AVIChunkHeader *header,
                              uint64_t        offset,
                              void           *userData,
                              uint32_t        hint );

DivXError ReadDivXIdChunkHeader( RDBufferHandle     handleBuffer,
                                 DivXIdChunkHeader *header,
                                 uint64_t           offset,
                                 void              *userData,
                                 uint32_t           hint );

DivXError ReadMediaManagerChunkHeader( RDBufferHandle               handleBuffer,
                                       DivxMediaManagerChunkHeader *header,
                                       uint64_t                     offset,
                                       void                        *userData,
                                       uint32_t                     hint );

DivXError ReadLanguageMenusHeader( RDBufferHandle handleBuffer,
                                   uint64_t       offset,
                                   LanguageMenus *pHdr,
                                   void          *userData,
                                   uint32_t       hint );

DivXError ReadMenuHeader( RDBufferHandle handleBuffer,
                          uint64_t       offset,
                          DivXMediaMenu *pHdr,
                          void          *userData,
                          uint32_t       hint );

DivXError ReadButtonHeader(  RDBufferHandle handleBuffer,
                             uint64_t       offset,
                             ButtonMenu    *pButton,
                             void          *userData,
                             uint32_t       hint );

DivXError ReadPositionHeader( RDBufferHandle handleBuffer,
                              MenuRectangle *mr,
                              uint64_t       offset,
                              void          *userData,
                              uint32_t       hint );

DivXError ReadMediaSourceHeader( RDBufferHandle handleBuffer,
                                 uint64_t       startOffset,
                                 MediaSource   *source,
                                 void          *userData,
                                 uint32_t       hint );

DivXError ReadMediaTrackHeader( RDBufferHandle handleBuffer,
                                uint64_t       startOffset,
                                MediaTrack    *track,
                                void          *userData,
                                uint32_t       hint );

DivXError ReadTitleHeader( RDBufferHandle handleBuffer,
                           uint64_t       startOffset,
                           TitleInfo     *title,
                           void          *userData,
                           uint32_t       hint );

DivXError ReadTranslationEntryHeader( RDBufferHandle    handleBuffer,
                                      uint64_t          startOffset,
                                      TranslationEntry *entry,
                                      void             *userData,
                                      uint32_t          hint );

DivXError ReadChapterHeader( RDBufferHandle handleBuffer,
                             uint64_t       startOffset,
                             ChapterHeader *chap,
                             void          *userData,
                             uint32_t       hint );

DivXError ReadAVIHeader( RDBufferHandle handleBuffer,
                         uint64_t       startOffset,
                         AVIHeader     *aviHeader,
                         void          *userData,
                         uint32_t       hint );

DivXError ReadAVIStreamHeader( RDBufferHandle   handleBuffer,
                               uint64_t         startOffset,
                               AVIStreamHeader *aviStream,
                               void            *userData,
                               uint32_t         hint );

DivXError ReadAVIStreamFormatVideo( RDBufferHandle        handleBuffer,
                                    uint64_t              startOffset,
                                    AVIStreamFormatVideo *streamFormat,
                                    void                 *userData,
                                    uint32_t              hint );

DivXError ReadAVIStreamFormatAudio( RDBufferHandle        handleBuffer,
                                    uint64_t              startOffset,
                                    uint32_t              size,
                                    AVIStreamFormatAudio *streamFormat,
                                    void                 *userData,
                                    uint32_t              hint );

DivXError ReadAVIStreamFormatSubtitle( RDBufferHandle           handleBuffer,
                                       uint64_t                 startOffset,
                                       AVIStreamFormatSubtitle *streamFormat,
                                       void                    *userData,
                                       uint32_t                 hint );

DivXError ReadAVIStreamDRMInfo( RDBufferHandle    handleBuffer,
                                uint64_t          startOffset,
                                AVIStreamInfoDRM *drmInfo,
                                void             *userData,
                                uint32_t          hint );

DivXError ReadAVIIndexEntry( RDBufferHandle  handleBuffer,
                             uint64_t        startOffset,
                             AVI1IndexEntry *index,
                             void           *userData,
                             uint32_t        hint );

DivXError ReadStrn( RDBufferHandle handleBuffer,
                    uint64_t       startOffset,
                    uint8_t       *data,
                    uint32_t       sizeData,
                    void          *userData,
                    uint32_t       hint );

#ifdef __cplusplus
}
#endif

#endif /* _AVIRREADSTRUCTURES_H_ */
