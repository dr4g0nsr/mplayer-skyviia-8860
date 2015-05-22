/*!
    @file
@verbatim
$Id: Layer3ReadPublished.h 60057 2009-05-09 23:20:24Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

/* The following code mimics the XML structure defined in the DivX Media File XML */
/* Reference Guide.doc.  This code is based on that definition.*/

/*! The available functions in this API are numerous, but all follow the
    High Level (HL) model.

    The functions that are available are those in the layer3.h and Layer3XXXXPublished.h headers

    In the published header there are four basic functions:
      L3cmr_Get
      L3cmr_Open
      L3cmw_Set
      L3cmw_Create

    Let's go over those four functions here:

    L3cmr_Get
      Used to retrieve a property, which has a distinct value and does not
      contain any children.  These can be strings, integers, character codes, structs and more.
      Structure: L3cmr_Get<Element><Property>From<Parent>

    L3cmr_Open
      Allows the user to retrieve a child object from the HL model.  These functions
      typically take a parent handle and index and return a child handle.  The only
      except to this is in the L3cmr_OpenContainer function which takes a file name as input
      and returns a container handle.
      Structure: L3cmr_Open<Element><Child>From<Parent>

    L3cmw_Set
      This function sets a HL model property.
      Structure: L3cmw_Set<Element><Property>From<Parent>

    L3cmw_Create
      This creates X number of child objects for a parent.
      Structure: L3cmw_Create<Element><Property>From<Parent>

    All function calls have a similar form and can be easily accessed by simply knowing the
    name of the element/child/property of the HL model that you'd like to access and
    the form of the function to call.

    (HL Model Overview)

    Container

      Title
        TranslationId ...
        TitleMedia
          VideoMedia
          AudioMedia
          SubtitleMedia
            Description
              Translation

        Chapters
          Chapter
            TranslationId ...

        Streams
        TitleData
          VideoData
          AudioData
          SubtitleData

      Menus

        GlobalMenus
          Menu
            EnterAction
            ExitAction
            MenuMedia
              VideoMedia
              AudioMedia
              OverlayMedia
            DivXButton

        LanguageGroup
          Menu

            EnterAction

            ExitAction
              AudioSelect
              SubtitleSelect
              Play
                Chapter
                  TranslationId ...
              Resume
              MenuTransition

            MenuMedia
              VideoMedia
              AudioMedia
              OverlayMedia

            DivXButton
              Nav
              Position
              SelectAction
                Action
                  MenuTransition
                  SubtitleSelect
                  ButtonSelect
                  AudioSelect
                  Play
                    Chapter
                      TranslationId ...
                  Resume


   There are three open functions that drive the lower level code to do parsing (use sparingly):

    L3cmr_OpenContainer
    L3cmr_OpenMenu
    L3cmr_OpenTitle

   There are three create functions that do a similar operation on the create side:

    L3cmw_CreateContainer
    L3cmw_CreateMenu
    L3cmw_CreateTitle

   Functions for performing data read/writes are:

    L3cmw_SetPositionBlock
    L3cmw_SetPositionTime
    L3cmr_GetL3MenuDataHandle
    L3cmr_GetData
    L3GetTitleBlock
    L3GetMenuBlock
    L3cmr_SetTitleBlock
    L3cmr_SetTitleBlockPosition
    L3SetMenuBlockPosition
    L3cmw_SetData

   Functions for doing menu lookups are:

    L3cmr_GetL3StartMenu
    L3cmr_GetL3NumTitles
    L3cmr_GetL3NumMenus
    L3cmr_OpenMenuById
    L3cmr_GetTitleIndexById
    L3cmr_GetChapterById
    L3cmr_GetButtonById

 */

/** @defgroup LAYER3 DMF Layer 3 API
 *  Description of DivX Media Format API (DMF) Layer 3 Caching Layer
 *  @{
 */

#ifndef _LAYER3READPUBLISHED_H_
#define _LAYER3READPUBLISHED_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFProperties.h"
#include "DMFActionFormat.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"
#include "DMFHttpAuth.h"
#include "DMFWebProxy.h"
#include "DMFModuleInfo.h"
#include "DMFMetaModuleInfo.h"
#include "./L3Common/Layer3CommonPublished.h"
#include "DMFTagInfo.h"

#include "./DMFTranslator/libTranslator.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Function takes initialization parameters for running layer 3

    @param mem            (IN)  - DivXMem memory handle (NULL if using the heap)
    @param libTransHdl    (IN)  - Translator handle (string translation)
    @param httpAuth       (IN)  - DMFHttpAuth (Http Authorization name and password), NULL if not needed
    @param webProxy       (IN)  - Proxy information (NULL if not needed)
    @param handle         (OUT) - Returns an instance of the L3ResourcesHandle

    @return DivXError   (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_Init( DivXMem            mem,
                      TranslatorHandle   libTransHdl,
                      DMFHttpAuth       *httpAuth,
                      DMFWebProxy       *webProxy,
                      L3ResourcesHandle *handle );

/** Retrieves the L3 resources set during init

    @param handle       (IN) - Container to retrieve resources from
    @param res         (OUT) - Returns an instance of the L3ResourcesHandle

    @return DivXError   (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetResources(ContainerL3Handle handle, L3ResourcesHandle* res);

/** Function takes initialization parameters for running layer 3

    @param mem               (IN)  - DivXMem memory handle (NULL if using the heap)
    @param libTransHdl       (IN)  - Translator handle (string translation)
    @param httpAuth          (IN)  - DMFHttpAuth (Http Authorization name and password), NULL if not needed
    @param webProxy          (IN)  - Proxy information (NULL if not needed)
    @param handle            (OUT) - Returns an instance of the L3ResourcesHandle
    @param modules           (IN)  - The modules to load into the system
    @param moduleCount       (IN)  - The number of modules to load

    @return DivXError   (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_InitModules( DivXMem            mem,
                             TranslatorHandle   libTransHdl,
                             DMFHttpAuth       *httpAuth,
                             DMFWebProxy       *webProxy,
                             L3ResourcesHandle *handle,
                             DMFModuleInfo     **modules,
                             int32_t            moduleCount);

/** Function de-initializes parameters for running layer 3

    @param handle       (OUT) - Returns a structure/handle?? to the layer 2 parsed file

    @return DivXError   (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_DeInit( L3ResourcesHandle handle );

/** Function returns a layer3 handle to the opened container

    @param fileName      (IN)  - File name of this container
    @param outputHandle  (IN)  - Memory handle from above layer3 for the container
    @param res           (IN)  - Memory available in the system

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_OpenContainer( const DivXString    *fileName,
                               ContainerL3Handle   *outputHandle,
                               L3ResourcesHandle    res );

/** Function closes (and cleans up the Layer3 container)

    @param  outputHandle (IN) - Container to close
    @param  res          (IN) - Handle to system resources

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_CloseContainer( ContainerL3Handle outputHandle,
                                L3ResourcesHandle res );

/** Function returns a layer3 handle to the opened container

    @param iIdx            (IN)  - Index of title to open
    @param containerHandle (IN)  - Defines the container to open and how to open it
    @param outputHandle    (OUT) - Returns a structure to the layer 3 title
    @param resourcesHandle (IN)  - Handle to system resources

    @return DivXError      (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_OpenTitle( uint16_t          iIdx,
                           ContainerL3Handle containerHandle,
                           TitleL3Handle  *outputHandle,
                           L3ResourcesHandle resourcesHandle );

/** Function returns a layer3 handle to the opened container

    @param iIdx              (IN)  - Index t the correct menu to open (0 based)
    @param lgIdx             (IN)  - 0 = GlobalMenus Menus, 1 - N = LanguageGroup Menus
    @param inputHandle       (IN)  - Defines the container to open and how to open it
    @param menuHandle        (OUT) - Returns a handle to the layer 3 menu structure
    @param resourcesHandle   (IN)  - Handle to system resources

    @return DivXError       (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_OpenMenu( uint16_t          iIdx,
                          uint16_t          lgIdx,
                          ContainerL3Handle inputHandle,
                          MenuL3HandlePtr   menuHandle,
                          L3ResourcesHandle resourcesHandle );

/** Returns the numer of titles based on a container handle

    @param handle          (IN)  - Handle to the container
    @param uiNumTitles     (OUT) - The number of titles in the container

    @return DivXError      (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetNumTitles( ContainerL3Handle handle,
                              uint32_t         *uiNumTitles );

/** Returns the numer of menus based on a container handle

    @param handle         (IN)  - Handle to the container
    @param uiNumMenus     (OUT) - The number of menus in the container

    @return DivXError     (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetNumMenus( ContainerL3Handle handle,
                             uint32_t         *uiNumMenus );

/** Function returns an MenuTrack Handle from an ObjectID

    @param id              (IN)  - Object ID for menu referencing
    @param inHandle        (IN)  - Handle to the container
    @param MenuL3Handle*   (OUT) - Returned AudioTrack Handle
    @param lgIdx1          (IN)  - 0 = GlobalMenus Menus, 1 - N = LanguageGroup Menus
    @param resourcesHandle (IN)  - Handle to system resources

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_OpenMenuById( uint32_t          id,
                              ContainerL3Handle inHandle,
                              MenuL3Handle     *outHandle,
                              int16_t          *lgIdx1,
                              L3ResourcesHandle resourcesHandle );

/** Function returns an MenuTrack Handle from an ObjectID

    @param inHandle      (IN)  - Container handle
    @param id            (IN)  - Object ID
    @param index         (OUT) - Returned Index

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleIndexById( ContainerL3Handle inHandle,
                                   uint32_t          id,
                                   int16_t          *index );

/** Function returns an Chapter Handle from an ObjectID

    @param id             (IN)  - Object ID for menu referencing
    @param inHandle       (IN)  - Title handle.
    @param outHandle      (OUT) - Returned Chapter Handle

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterById( uint32_t          id,
                                TitleL3Handle     inHandle,
                                ChapterL3Handle  *outHandle );

/** FIXME needs to be time based.

Returns chapter index given a time for the title

    @param inHandle        (IN) - Title handle
    @param frame           (IN) - Time in DivXTime
    @param index           (OUT) - Returned index of the chapter, invalid if error returned

    @return DivXError      (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterIndexByTime( TitleL3Handle inHandle,
                                        DivXTime      time,
                                        int32_t      *index );

/** Function returns an DivXButton Handle from an ObjectID

    @param id            (IN)  - Object ID for menu referencing
    @param inHandle      (IN)  - Handle to Menu
    @param outHandle     (OUT) - populated with handle to DivXButton

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetButtonById( uint32_t        id,
                               MenuL3Handle    inHandle,
                               ButtonL3Handle *outHandle );

/** Function returns an DivXButton Handle from a Position

    @param xPos             (IN)  - The x position of the button
    @param yPos             (IN)  - The y position of the button
    @param inHandle         (IN)  - Handle to Menu
    @param outHandle        (OUT) - populated with handle to DivXButton

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetButtonByPosition( int16_t         xPos,
                                     int16_t         yPos,
                                     MenuL3Handle    inHandle,
                                     ButtonL3Handle *outHandle);

/**
   @param int32_t max     (IN)  - Index to access, if -1 creates default maximum
   @param res             (IN)  - Handle to system resources
   @param inHandle        (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle       (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetIndexedButtonFromMenu( int32_t           max,
                                          L3ResourcesHandle res,
                                          MenuL3Handle      inHandle,
                                          ButtonL3Handle   *outHandle );

/** Function returns an Title Handle from an ObjectID

    @param id            (IN)  - Object ID for menu referencing
    @param inHandle      (IN)  - Handle to container
    @param titleHandle*  (OUT) - Returned Title Handle
    @param res           (IN)  - Handle to system resources

    @return DivXError          (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_OpenTitleById( uint32_t          id,
                               ContainerL3Handle inHandle,
                               TitleL3Handle    *titleHandle,
                               L3ResourcesHandle res);

/**
   Notes: Returns the number of titles in the file

   @param handle             (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val                (OUT) - Number of titles in the file

   @return DivXError         (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberTitleFromContainer( ContainerL3Handle handle,
                                             int16_t          *val );

/**
   Notes: This element determines the existence of a menu block

   @param handle             (IN) - Input handle for accessing the element.
   @param val                (OUT) - Number of menus in the file (this is only to be used as 0 or > 0)

   @return DivXError         (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberMenusFromContainer( ContainerL3Handle handle,
                                             int32_t          *val );

/**
   Notes: This element and its children define a Title.

   @param idx         (IN)  - Index of the element to access
   @param res         (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle   (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenTitleFromContainer( uint32_t          idx,
                                        L3ResourcesHandle res,
                                        ContainerL3Handle inHandle,
                                        TitleL3Handle    *outHandle );

/**
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenStreamsFromTitle( uint32_t          idx,
                                      L3ResourcesHandle res,
                                      TitleL3Handle     inHandle,
                                      StreamsL3Handle  *outHandle );

/**
   Notes:
   This element contains one or more LanguageGroup elements and a GlobalMenus element if one exists. 3.2 Menu Memory
   Requirements and Limitations The menu data structures are stored in the ‘MENU’ chunk contained in ‘DMNU’ chunk.
   The maximum size of the ‘MENU’ chunk is 300KB. If the ’MENU’ chunk is greater than 300KB, the menu is not a valid
   menu and the playback device may ignore the menu and play the first title in the file. For each object, the maximum
   number that can be stored in the ‘MENU’ chunk is detailed in 3.4 Menu Object Data Model. These maximums are mutually
   exclusive, that is, a ‘MENU’ chunk can not contain all objects at their maximum within the 300KB limit. For example,
   the ‘MENU’ chunk can support 1000 menus (DivXMediaMenu), or it can support 100 buttons (ButtonMenu) on each menu;
   however it can’t support 1000 menus with 100 buttons on each menu. Most ‘MENU’ chunks will contain a combination of
   different objects, each staying within their maximums. For example, a ‘MENU’ chunk can support 21 menus with 98 buttons
   each. It is very important that playback devices do not limit themselves to only the maximums of individual objects.
   They must also support the various combinations of objects within those limitations.

   @param res         (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle   (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenMenusFromContainer( L3ResourcesHandle res,
                                        ContainerL3Handle inHandle,
                                        MenusL3Handle    *outHandle );

/**
   Notes:
   This element along with its children defines a menu.
   @param idx         (IN)  - Index of the element to access
   @param res         (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle   (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenMenuFromLanguageGroup( uint32_t              idx,
                                           L3ResourcesHandle     res,
                                           LanguageGroupL3Handle inHandle,
                                           MenuL3Handle         *outHandle );

/**
   Notes:
   This element along with its children defines a menu.
   @param idx          (IN)  - Index of the element to access
   @param res          (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle     (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle    (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenMenuFromGlobalMenus( uint32_t            idx,
                                         L3ResourcesHandle   res,
                                         GlobalMenusL3Handle inHandle,
                                         MenuL3Handle       *outHandle );

/**
   Notes:
    This element and its children define a Title.
   @param handle       (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val          (OUT) - Pointer to number of streams

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberStreamsFromTitle( TitleL3Handle handle,
                                           int16_t      *val );

/**
   Notes:
   This element and its children define a Title.
   @param handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val    (OUT) - Pointer to number of streams

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetTitleNumStreamsFromContainer( TitleL3Handle handle,
                                                 int16_t *val );

/**
   @param nStream           (IN) - Stream index
   @param handle            (IN) - Title media handle
   @param videoStruct       (OUT) - Retrieved stream structure
   @param mem               (IN) - Memory handle

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetVideoStreamStructFromTitleMedia( int32_t                nStream,
                                                    TitleMediaL3Handle     handle,
                                                    DMFVideoStreamInfo1_t *videoStruct,
                                                    DivXMem                mem );

/**
   @param nStream             (IN) - Stream index
   @param handle              (IN) - Title media handle
   @param videoStruct         (OUT) - Retrieved stream structure
   @param mem                 (IN) - Memory handle

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetVideoStreamStructFromMenuMedia( int32_t                nStream,
                                                   MenuMediaL3Handle      handle,
                                                   DMFVideoStreamInfo1_t *videoStruct,
                                                   DivXMem                mem );

/**
   @param nStream             (IN) - Stream index
   @param handle              (IN) - Title media handle
   @param audioStruct         (OUT) - Retrieved stream structure
   @param mem                 (IN) - Memory handle

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetAudioStreamStructFromTitleMedia( int32_t                nStream,
                                                    TitleMediaL3Handle     handle,
                                                    DMFAudioStreamInfo1_t *audioStruct);

/**
   @param nStream        (IN) - Stream index
   @param handle         (IN) - Title media handle
   @param audioStruct    (OUT) - Retrieved stream structure
   @param mem            (IN) - Memory handle

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetAudioStreamStructFromMenuMedia( int32_t                nStream,
                                                   MenuMediaL3Handle      handle,
                                                   DMFAudioStreamInfo1_t *audioStruct);

/**
   @param idx          (IN) - Stream index
   @param handle       (IN) - Title media handle
   @param subStruct    (OUT) - Retrieved stream structure

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetSubtitleStreamStructFromTitleMedia( uint32_t                  idx,
                                                       TitleMediaL3Handle        handle,
                                                       DMFSubtitleStreamInfo1_t *subStruct );

/**
   @param idx          (IN) - Stream index
   @param handle       (IN) - Title media handle
   @param subStruct    (OUT) - Retrieved stream structure

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetOverlayStreamStructFromMenuMedia( uint32_t                  idx,
                                                     MenuMediaL3Handle         handle,
                                                     DMFSubtitleStreamInfo1_t *subStruct );

/**
   Retreives the total duration for a title.

   @param inHandle           (IN)  - Input handle for accessing the element.
   @param res                (IN)  - Resource handle for memory access and lib Translator.
   @param dtDuration         (OUT) - The total duration for the title.

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetTotalDurationFromTitleMedia( TitleMediaL3Handle inHandle, 
                                                L3ResourcesHandle  res,
                                                DivXTime          *dtDuration );

/**
   Retreives the total duration for a title.

   @param inHandle          (IN)  - Input handle for accessing the element.
   @param res               (IN)  - Resource handle for memory access and lib Translator.
   @param dtDuration        (OUT) - The total duration for the title.

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetTotalDurationFromMenuMedia( MenuMediaL3Handle inHandle, 
                                               L3ResourcesHandle  res,
                                               DivXTime          *dtDuration );

/**
   @param handle          (IN)  - SubtitleMedia handle
   @param val             (OUT) - Locale

   @return DivXError            (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetSubtitleMediaLocaleFromTitleMedia( SubtitleMediaL3Handle handle,
                                                      DivXString           *val );

/**
   @param inHandle     (IN) - SubtitleMedia handle
   @param locale             (IN) - Locale string
   @param val                 (OUT) - Index

   @return DivXError            (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAudioMediaIndexByLocale( TitleMediaL3Handle inHandle,
                                            const DivXString  *locale,
                                            int32_t           *val );

/**
   @param inHandle           (IN) - SubtitleMedia handle
   @param locale             (IN) - Locale string
   @param val                (OUT) - Index

   @return DivXError            (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetSubtitleMediaIndexByLocale( TitleMediaL3Handle inHandle,
                                               const DivXString  *locale,
                                               int32_t           *val );

/**
   @param StreamsL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int32_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetStreamsObjectIDFromTitle( StreamsL3Handle,
                                             int32_t * );

/**
   Notes: This element contains one or more Chapter children that define Chapter points within a Title.
         Each Chapter element defines a sub-unit of a Title based on time or frame locations within the
         Title's media. Players can display the list of Chapter elements to allow the user to start playback
         at various fixed points within the Title. A Chapter element can also be used as a parameter for
         Play commands, allowing DivX Menus to direct playback to fixed points within the Title.

   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenChaptersFromTitle( L3ResourcesHandle res,
                                       TitleL3Handle     inHandle,
                                       ChaptersL3Handle *outHandle );

/**
   Notes: This element contains one or more Chapter children that define Chapter points within a Title.
         Each Chapter element defines a sub-unit of a Title based on time or frame locations within the
         Title's media. Players can display the list of Chapter elements to allow the user to start playback
         at various fixed points within the Title. A Chapter element can also be used as a parameter for Play
         commands, allowing DivX Menus to direct playback to fixed points within the Title.

   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Pointer to number of chapters

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberChapterFromChapters( ChaptersL3Handle handle,
                                              int16_t         *val );

/**
   Notes: This element contains one or more Menu elements that are defined to be language neutral. This is
         commonly done to allow the user to select a language to use before beginning the language specific
         Menu system.

   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenGlobalMenusFromMenus( L3ResourcesHandle    res,
                                          MenusL3Handle        inHandle,
                                          GlobalMenusL3Handle *outHandle );

/**
   Notes: This element contains one or more Menu elements that are defined to be language neutral. This is
         commonly done to allow the user to select a language to use before beginning the language specific
         Menu system.

   @param handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val    (OUT) - Pointer to number of menus

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberMenuFromGlobalMenus( GlobalMenusL3Handle handle,
                                              int16_t            *val );

/**
   Notes: This element contains one or more Menu elements that are defined to be language neutral. This is
         commonly done to allow the user to select a language to use before beginning the language specific
         Menu system.

   @param MenusL3Handle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int32_t*      (OUT) - Ptr to returned type

   @return DivXError    (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetGlobalMenusObjectIDFromMenus( GlobalMenusL3Handle,
                                                 int32_t * );

/**
   Notes: This element contains one or more Menu elements that are defined to be language neutral. This is
         commonly done to allow the user to select a language to use before beginning the language specific
         Menu system.

   @param MenusL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*        (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetGlobalMenusStartFromMenus( GlobalMenusL3Handle,
                                              int16_t * );

/**
   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Pointer to number of langauge groups

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberLanguageGroupFromMenus( MenusL3Handle handle,
                                                 int16_t      *val );

/**
   @param handle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val       (OUT) - Number of global menus (this is always 0 or 1, other values are illegal)

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberGlobalMenusFromMenus( MenusL3Handle handle,
                                               int16_t      *val );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Pointer to output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenLanguageGroupFromMenus( uint32_t               idx,
                                            L3ResourcesHandle      res,
                                            MenusL3Handle          inHandle,
                                            LanguageGroupL3Handle *outHandle );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Pointer to number of menus

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberMenuFromLanguageGroup( LanguageGroupL3Handle handle,
                                                int16_t              *val );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param MenusL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int32_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetLanguageGroupObjectIDFromMenus( LanguageGroupL3Handle,
                                                   int32_t * );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param MenusL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetLanguageGroupLanguageFromMenus( LanguageGroupL3Handle,
                                                   DivXString * );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param MenusL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetLanguageGroupStartMenuFromMenus( LanguageGroupL3Handle,
                                                    int16_t * );

/**
   Notes: This element contains a group of Menus that all contain the same language.

   @param MenusL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetLanguageGroupRootMenuFromMenus( LanguageGroupL3Handle,
                                                   int16_t * );

/**
   Notes: This element along with its children defines a menu.

   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Index of the element to access

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberButtonFromMenu( MenuL3Handle handle,
                                         int16_t     *val );

/**
   Notes: This element along with its children defines a menu.

   @param LanguageGroupL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t* idx (OUT) - Index of the element to access

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberButtonFromMenu( MenuL3Handle handle,
                                         int16_t     *val );

/**
   Notes: This element along with its children defines a menu.

   @param LanguageGroupL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int32_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetMenuObjectIDFromLanguageGroup( MenuL3Handle,
                                                  int16_t * );

/**
   Notes: This element along with its children defines a menu.

   @param LanguageGroupL3Handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*           (OUT) - Ptr to returned type

   @return DivXError            (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetMenuNameFromLanguageGroup( MenuL3Handle,
                                              int16_t * );

/**
   Notes:
   This element along with its children defines a menu.
   @param LanguageGroupL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetMenuTypeFromLanguageGroup( MenuL3Handle,
                                              int16_t * );

/**
   Notes:
   This element along with its children defines a menu.
   @param LanguageGroupL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetMenuStartButtonFromLanguageGroup( MenuL3Handle,
                                                     int16_t * );

/**
   Notes:
   This element contains the track elements that define the media for a Menu.
   @param iIdx (IN)  - Index of the element to access
   @param res  (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle  (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenMenuMediaFromMenu( uint32_t           iIdx,
                                       L3ResourcesHandle  res,
                                       MenuL3Handle       inHandle,
                                       MenuMediaL3Handle *outHandle );

/**
   Notes: Returns the number of streams in menu media

   @param handle     (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val        (OUT) - Number of streams returned

   @return DivXError            (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberStreamFromMenuMedia( MenuMediaL3Handle handle,
                                              int16_t          *val );

/**
   Notes: This element contains the track elements that define the media for a Menu.

   @param handle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val       (OUT) - Pointer to number of audio media

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberAudioMediaFromMenuMedia( MenuMediaL3Handle handle,
                                                  int16_t          *val );

/**
   Notes:
   This element contains the track elements that define the media for a Menu.
   @param handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val    (OUT) - Pointer to number of overlay media

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberOverlayMediaFromMenuMedia( MenuMediaL3Handle handle,
                                                    int16_t          *val );

/**
   Notes: This element contains the VideoMedia,AudioMedia and/or SubtitleMedia elements that defines the media
         tracks for a Title.

   @param res        (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle  (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenTitleMediaFromTitle( L3ResourcesHandle   res,
                                         TitleL3Handle       inHandle,
                                         TitleMediaL3Handle *outHandle );

/**
   Notes: This element contains the VideoMedia,AudioMedia and/or SubtitleMedia elements that defines the media
   tracks for a Title.

   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Pointer to number of audio media

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberAudioMediaFromTitleMedia( TitleMediaL3Handle handle,
                                                   int16_t           *val );

/**
   Notes: This element contains the VideoMedia,AudioMedia and/or SubtitleMedia elements that defines the media
         tracks for a Title.

   @param iIdx      (IN) - Index to retrieve
   @param res       (IN) - Resource handle
   @param inHandle  (IN) - TitleMedia Handle
   @param outHandle (OUT) - Pointer to subtitleMedia handle

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenSubtitleMediaFromTitleMedia( uint32_t               iIdx,
                                                 L3ResourcesHandle      res,
                                                 TitleMediaL3Handle     inHandle,
                                                 SubtitleMediaL3Handle *outHandle );

/**
   Notes: Rtrieves the number of subtitle media objects in title media.

   @param handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val    (OUT) - Pointer to number of subtitle media

   @return DivXError     (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberSubtitleMediaFromTitleMedia( TitleMediaL3Handle handle,
                                                      int16_t           *val );

/**
   Notes: Rtrieves the number of video media objects in title media.

   @param handle (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val    (OUT) - Pointer to number of subtitle media

   @return DivXError     (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberVideoMediaFromTitleMedia( TitleMediaL3Handle handle,
                                                      int16_t           *val );

/**
   Notes: L3cmr_Opens a video media object from a title media object.

   @param idx         (IN)  - Index of video media
   @param res         (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle   (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenVideoMediaFromTitleMedia( uint32_t            idx,
                                              L3ResourcesHandle   res,
                                              TitleMediaL3Handle  inHandle,
                                              VideoMediaL3Handle *outHandle );

/**
   Notes: This element defines the location in which to find the source for a video track. Note: When a non
         DivX Media File is used as the source, the content is treated as Title = 1.

   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenVideoMediaFromMenuMedia( L3ResourcesHandle   res,
                                             MenuMediaL3Handle   inHandle,
                                             VideoMediaL3Handle *outHandle );

/**
   Notes: This element defines the location in which to find the source for a video track. Note: When a non
         DivX Media File is used as the source, the content is treated as Title = 1.
   @param MenuMediaL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int32_t*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetVideoMediaObjectIDFromMenuMedia( VideoMediaL3Handle,
                                                    int32_t * );

/**
   Notes:
   This element is used to define the source for an audio track. Note: When a non DivX Media File is used as the source, the content is treated as Title = 1.
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenAudioMediaFromMenuMedia( uint32_t            idx,
                                             L3ResourcesHandle   res,
                                             MenuMediaL3Handle   inHandle,
                                             AudioMediaL3Handle *outHandle );

/**
   Notes:
   This element is used to define the source for an audio track. Note: When a non DivX Media File is used as the source, the content is treated as Title = 1.
   @param idx         (IN)  - Index of the element to access
   @param res         (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle   (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenAudioMediaFromTitleMedia( uint32_t            idx,
                                              L3ResourcesHandle   res,
                                              TitleMediaL3Handle  inHandle,
                                              AudioMediaL3Handle *outHandle );

/**
   Notes:
   This element is used to define the source for an audio track. Note: When a non DivX Media File is used as the source, the content is treated as Title = 1.
   @param TitleMediaL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param DivXString*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetAudioMediaLocaleFromTitleMedia( AudioMediaL3Handle,
                                                   DivXString * );

/**
   Notes:
   This element is used to define a DivXButton on a Menu.
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenButtonFromMenu( uint32_t          idx,
                                    L3ResourcesHandle res,
                                    MenuL3Handle      inHandle,
                                    ButtonL3Handle   *outHandle );

/**
   Notes:
   This element is used to define a DivXButton on a Menu.
   @param ButtonL3Handle    (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetButtonOverlayFrameFromMenu( ButtonL3Handle,
                                               int16_t * );

/**
   Notes:
   This element contains the commands to be executed when a Menu begins playback.
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenEnterActionFromMenu( L3ResourcesHandle    res,
                                         MenuL3Handle         inHandle,
                                         EnterActionL3Handle *outHandle );

/**
   Notes:
   This element contains the commands to be executed when a Menu begins playback.
   @param handle       (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val          (OUT) - Pointer Number of Actions

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberActionFromEnterAction( EnterActionL3Handle handle,
                                                int16_t            *val );

/**
   Notes: This element contains the commands to be executed when a Menu begins playback.

   @param handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val      (OUT) - Pointer to Number of Actions

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberActionFromSelectAction( SelectActionL3Handle handle,
                                                 int16_t             *val );

/**
   Notes:
   This element contains the commands to be executed when a Menu begins playback.
   @param handle       (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val          (OUT) - Pointer to number of actions
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNumberActionFromExitAction( ExitActionL3Handle handle,
                                               int16_t            *val );

/**
   Notes:
   This element contains the commands to be executed when a Menu finishes playback. There are three common uses of this event. 1) To loop back to the beginning of the same menu using the MenuTransition command. 2) To go to a different menu using the MenuTransition command. This is commonly done when the Menu is actually a "bridging" piece of video between two menus. 3) To being playback of a Title with the Play or Resume command. This is commonly done when the Menu is actually a "bridging" piece of transition video between a menu and a Title. Important: Actions have unions for children because they must be in the proper order for correct execution.
   @param res            (IN)  - Index of the element to access
   @param inHandle       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param outHandle      (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenExitActionFromMenu( L3ResourcesHandle   res,
                                        MenuL3Handle        inHandle,
                                        ExitActionL3Handle *outHandle );

/**
   Notes:
   This element defines the navigation for a DivXButton. When the user presses an arrow key while the button is highlighted, the Nav element defines what DivXButton should be switched to, if any. Note: The Nav element is required but no navigation is required to be defined. Warning: If you do not define navigation directions, then if the button is selected, there is no way to move to other buttons unless a pointer control (mouse, touchscreen) is present in the player.
   @param iIdx       (IN)  - Index of the element to access
   @param res        (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle  (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenNavFromButton( uint32_t          iIdx,
                                   L3ResourcesHandle res,
                                   ButtonL3Handle    inHandle,
                                   NavL3Handle      *outHandle );

/**
   Notes:
   This element defines the navigation for a DivXButton. When the user presses an arrow key while the button is highlighted, the Nav element defines what DivXButton should be switched to, if any. Note: The Nav element is required but no navigation is required to be defined. Warning: If you do not define navigation directions, then if the button is selected, there is no way to move to other buttons unless a pointer control (mouse, touchscreen) is present in the player.
   @param ButtonL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*          (OUT) - Ptr to returned type
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNavLeftFromButton( NavL3Handle,
                                      int16_t * );

/**
   Notes: This element defines the navigation for a DivXButton. When the user presses an arrow key while the
         button is highlighted, the Nav element defines what DivXButton should be switched to, if any. Note:
         The Nav element is required but no navigation is required to be defined. Warning: If you do not
         define navigation directions, then if the button is selected, there is no way to move to other
         buttons unless a pointer control (mouse, touchscreen) is present in the player.

   @param ButtonL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNavRightFromButton( NavL3Handle,
                                       int16_t * );

/**
   Notes: This element defines the navigation for a DivXButton. When the user presses an arrow key while the
         button is highlighted, the Nav element defines what DivXButton should be switched to, if any. Note: The Nav
         element is required but no navigation is required to be defined. Warning: If you do not define navigation
         directions, then if the button is selected, there is no way to move to other buttons unless a pointer control
         (mouse, touchscreen) is present in the player.

   @param ButtonL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNavUpFromButton( NavL3Handle,
                                    int16_t * );

/**
   Notes: This element defines the navigation for a DivXButton. When the user presses an arrow key while the
         button is highlighted, the Nav element defines what DivXButton should be switched to, if any. Note: The Nav
         element is required but no navigation is required to be defined. Warning: If you do not define navigation
         directions, then if the button is selected, there is no way to move to other buttons unless a pointer
         control (mouse, touchscreen) is present in the player.

   @param ButtonL3Handle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param int16_t*          (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetNavDownFromButton( NavL3Handle,
                                      int16_t * );

/**
   Notes: This element defines the input source of the Overlays to be used for button highlighting for a menu.
   This element is only required if Buttons have been defined for the Menu. Note: In DivX Media Menus, the
   background video and the overlays must match in resolution. Note: When a non DivX Media File is used as
   the source, the content is treated as Title = 1.

   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenOverlayMediaFromMenuMedia( uint32_t              idx,
                                               L3ResourcesHandle     res,
                                               MenuMediaL3Handle     inHandle,
                                               OverlayMediaL3Handle *outHandle );

/**
   Notes:
   This element is used to define the screen position of the "active area" of a DivXButton. This information is used on Players that have a pointing device (mouse, touch screen etc). When the cursor is placed within the boundaries of the rectangle defined by this element, the DivXButton is highlighted. Note: In DivX Media Menus, the background video and the overlays must match in resolution. The resolution of the overlays and video is the coordinate system used to define this element. The upper left corner of the screen is coordinate 0,0
   @param iIdx      (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenPositionFromButton( uint32_t          iIdx,
                                        L3ResourcesHandle res,
                                        ButtonL3Handle    inHandle,
                                        PositionL3Handle *outHandle );

/**
   Notes:
   This element contains child elements that define commands to be execute when a DivXButton is pressed. Note: Only one of the Play, Resume, MenuTransition, and ButtonSelect commands may be used in a single SelectAction element. The children of SelectAction have been unionized to ensure an ordered
   sequence.
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenSelectActionFromButton( L3ResourcesHandle     res,
                                            ButtonL3Handle        inHandle,
                                            SelectActionL3Handle *outHandle );

/**
   Notes:
    This element defines a command that will cause the playback of a Menu.
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenActionFromSelectAction( uint32_t             idx,
                                            L3ResourcesHandle    res,
                                            SelectActionL3Handle inHandle,
                                            ActionL3Handle      *outHandle );

/**
   Notes:
    This element defines a command that will cause the playback of a Menu.
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenActionFromExitAction( uint32_t           idx,
                                          L3ResourcesHandle  res,
                                          ExitActionL3Handle inHandle,
                                          ActionL3Handle    *outHandle );

/**
   Notes:
    This element defines a command that will cause the playback of a Menu.
   @param idx       (IN)  - Index of the element to access
   @param res       (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle  (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle (OUT) - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenActionFromEnterAction( uint32_t            idx,
                                           L3ResourcesHandle   res,
                                           EnterActionL3Handle inHandle,
                                           ActionL3Handle     *outHandle );

/**
    Note: Returns the number of translations lookups in the file

    @param inHandle       (IN) - Select action handle
    @param idx            (IN) - Action index
    @param actionStruct   (OUT) - Action structure returned

    @return DivXError         (OUT) - Return error code (See DMFErrors.h)
 */
DivXError L3cmr_GetActionFromSelectAction( SelectActionL3Handle inHandle,
                                           int16_t              idx,
                                           L2ActionStruct      *actionStruct );

/*!
    Note: Returns the number of translations lookups in the file

    @param inHandle      (IN) - Select action handle
    @param idx           (IN) - Action index
    @param actionStruct  (OUT) - Action structure returned

    @return DivXError         (OUT) - Return error code (See DMFErrors.h)
 */
DivXError L3cmr_GetActionFromEnterAction( EnterActionL3Handle inHandle,
                                          int16_t             idx,
                                          L2ActionStruct     *actionStruct );

/*!
    Note: Returns the number of translations lookups in the file

    @param inHandle            (IN) - Select action handle
    @param idx                 (IN) - Action index
    @param actionStruct       (OUT) - Action structure returned

    @return DivXError         (OUT) - Return error code (See DMFErrors.h)
 */
DivXError L3cmr_GetActionFromExitAction( ExitActionL3Handle inHandle,
                                         int16_t            idx,
                                         L2ActionStruct    *actionStruct );

/*!
   Notes:
   This element defines a chapter point in a title. Play commands may use a chapter point as the location to start playback from. Players (software or hardware) may display the Chapter points that are defined for a Title and allow the user to select a Chapter point for playback. Note: A pair of either StartTime/EndTime and StartFrame/EndFrame must be described at minimum. You cannot mix the two sets, and for the highest amount of positional accuracy the frame numbers should be used.

   @param idx                (IN)  - Index of the element to access
   @param res                (IN)  - Resource handle for memory access and lib Translator.  This should be null.
   @param inHandle           (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param outHandle         (OUT)  - Output handle for the retrieved element.  Used as the handle for lower level elements

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_OpenChapterFromChapters( uint32_t          idx,
                                         L3ResourcesHandle res,
                                         ChaptersL3Handle  inHandle,
                                         ChapterL3Handle  *outHandle );

/*!
   Notes: This element defines a chapter point in a title. Play commands may use a chapter point as the
         location to start playback from. Players (software or hardware) may display the Chapter points
         that are defined for a Title and allow the user to select a Chapter point for playback. Note: A
         pair of either StartTime/EndTime and StartFrame/EndFrame must be described at minimum. You cannot
         mix the two sets, and for the highest amount of positional accuracy the frame numbers should be used.

   @param inHandle   (IN)  - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param id         (OUT) - Ptr to returned type

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetChapterObjectIDFromChapters( ChapterL3Handle inHandle,
                                                int16_t* id);

/*!  
   Notes:  Retrieves the translation table (retrieves the translation table within the api)

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param res         (OUT) - Resource pointer that contains translation table

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 **/
DivXError L3cmr_GetTranslationTable( ContainerL3Handle inHandle,
                                     L3ResourcesHandle res );

/**
    Note: Returns the number of translations lookups in the file

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val         (OUT) - Returns number of translation lookup
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetNumberTranslationLookup(MenusL3Handle inHandle,
                                           int16_t *val );

/**
    Note: Return the translation language for a lookup entry

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param str         (OUT) - Returns the translation language
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTranslationLanguage( TranslationLookupL3Handle inHandle,
                                                    DivXString *str );

/**
    Note: Returns the number of lookup entries

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val         (OUT) - Returns the number of lookup entries
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetNumberLookupEntries(TranslationLookupL3Handle inHandle,
                                                 int32_t *val );

/**
    Note: Returns the translation string

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param str         (OUT) - Returns translation string
   @param res          (IN) - Resource handle (contains translation table)
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTranslationString( TranslationLookupL3Handle inHandle,
                                      DivXString              **str,
                                      L3ResourcesHandle         res );

/**
    Note: Returns the translation Id

   @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
   @param val         (OUT) - Returns translation id
   
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTranslationId( TranslationLookupL3Handle inHandle,
                                  uint32_t           *val );

/**
    Note: Returns translation id from stream

    @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param val         (OUT) - Returns stream translation id from streams

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetStreamTranslationIdFromStreams( StreamsL3Handle inHandle,
                                                   uint32_t       *val );

/**
    Note: Returns translation id from title

    @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param val         (OUT) - Returns title translation id from container

    @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleTranslationIdFromContainer( TitleL3Handle inHandle,
                                                    uint32_t     *val );

/**
    Note: Returns translation id from chapter

    @param inHandle     (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param val         (OUT) - Returns chapter translation id from title
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterTranslationIdFromTitle( ChapterL3Handle inHandle,
                                                  uint32_t       *val );

/**
    Note: Returns container prop from container

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pContainerProps  (OUT) - Returns container properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetContainerPropsFromContainer( ContainerL3Handle   inHandle,
                                                DMFContainerProp_t *pContainerProps );

/**
    Note: Return title prop from title

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pTitleProps      (OUT) - Returns title properties from title
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitlePropsFromTitle( TitleL3Handle   inHandle,
                                        DMFTitleProp_t *pTitleProps );

/**
    L3cmr_Gets the chapter properties.

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pChapterProps    (OUT) - Returns chapter properties from chapter
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterPropsFromChapter( ChapterL3Handle   inHandle,
                                            DMFChapterProp_t *pChapterProps );


/**
    L3cmr_Gets the chapter name count.

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param nCount           (OUT) - Returns chapter name count
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterNamePropCountFromChapter( ChapterL3Handle inPtr, uint32_t *nCount);

/**
    L3cmr_Gets the chapter name count.

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param nCount            (IN) - Index of name property to get.
    @param ppNameProp       (OUT) - Returns the name property.
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetChapterNamePropFromChapter( ChapterL3Handle inPtr, uint32_t index, DMFNameProp_t **ppNameProp );

/**
    L3cmr_Gets the menus system properties.

    @param inHandle          (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pMenusProp       (OUT) - Returns menus properties from container

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenusPropFromContainer( ContainerL3Handle inHandle,
                                           DMFMenusProp_t   *pMenusProp );

/**
    L3cmr_Gets the menu properties.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pMenuProp       (OUT) - Returns menu properties from menu

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuPropsFromMenu( MenuL3Handle   inHandle,
                                      DMFMenuProp_t *pMenuProp );

/**
    L3cmr_Gets the properties for the Language Group menus.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param nLangMenu        (IN) - language group index
    @param pLangGroupProp  (OUT) - Returns language group properties from menus

   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuLangGroupPropFromMenus( MenusL3Handle           inHandle,
                                               int32_t                 nLangMenu,
                                               DMFLanguageGroupProp_t *pLangGroupProp );

/**
    L3cmr_Gets the properties for the Global menus.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param nGlobalMenu      (IN) - global menu index
    @param pLangGroupProp  (OUT) - Returns global menus properties from menus
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuGlobalGroupPropFromMenus( MenusL3Handle         inHandle,
                                                 int32_t               nGlobalMenu,
                                                 DMFGlobalMenusProp_t *pGlobalGroupProp );

/**
    L3cmr_Gets the properties for the title media.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pTitleMediaProp (OUT) - Returns title media properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleMediaPropFromTitleMedia( TitleMediaL3Handle   inHandle,
                                                 DMFTitleMediaProp_t *pTitleMediaProp );

/**
    L3cmr_Gets the properties for the menu media.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pMenuMediaProp  (OUT) - Returns menu media properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuMediaPropFromMenuMedia( MenuMediaL3Handle   inHandle,
                                               DMFMenuMediaProp_t *pMenuMediaProp );

/**
    L3cmr_Gets the properties for the video media.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pVideoMediaProp (OUT) - Returns video media property
    @param nVideoMedia      (IN) - Video media index
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetVideoMediaPropFromTitleMedia( TitleMediaL3Handle   inHandle,
                                                 DMFVideoMediaProp_t *pVideoMediaProp,
                                                 int32_t              nVideoMedia );

/**
    L3cmr_Gets the properties for the audio media.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pAudioMediaProp (OUT) - Returns audio media property
    @param nAudioMedia      (IN) - Audio media index
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAudioMediaPropFromTitleMedia( TitleMediaL3Handle   inHandle,
                                                 DMFAudioMediaProp_t *pAudioMediaProp,
                                                 int32_t              nAudioMedia );

/**
    L3cmr_Gets the properties for the subtitle media.

    @param inHandle            (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pSubtitleMediaProp (OUT) - Returns subtitle media property
    @param nSubtitleMedia      (IN) - Subtitle media index
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetSubtitleMediaPropFromTitleMedia( TitleMediaL3Handle      inHandle,
                                                    DMFSubtitleMediaProp_t *pSubtitleMediaProp,
                                                    int32_t                 nSubtitleMedia );

/**
    L3cmr_Gets the properties for the DivXButton.

    @param inHandle            (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pButtonProp        (OUT) - Returns button properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetButtonPropFromButton( ButtonL3Handle   inHandle,
                                         DMFButtonProp_t *pButtonProp );

/**
    L3cmr_Gets the properties for the Navigation.

    @param inHandle            (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pNavProp           (OUT) - Returns Nav properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetNavigationPropFromButton( ButtonL3Handle inHandle,
                                             DMFNavProp_t  *pNavProp );

/**
    L3cmr_Gets the properties for the Position.

    @param inHandle            (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pPosProp           (OUT) - Returns Position properties
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetPositionPropFromButton( ButtonL3Handle     inHandle,
                                           DMFPositionProp_t *pPosProp );

/** Sets which streams to retrieve with GetTitleBlock

    Note:  This is used with the new efficient API for the currently active stream

    @param titleHandle      (IN)  - Title handle
    @param blockType        (IN)  - Block type (video, audio, subtitle) retrieved
    @param nStream          (OUT)  - stream retrieved
    @param bActive            (IN)  - Sets stream active (true) or inactive (false)

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetTitleActiveStream( TitleL3Handle  titleHandle,
                                      DMFBlockType_t blockType,
                                      int32_t        nStream,
                                      DivXBool       bActive );

/** Reads a chunk from a title, used with L3cmr_SetActiveStream.

    Note:  This is the most efficient API for pulling stream data from the file.  This should be used whenever
           possible.

    @param titleHandle       (IN)  - Title handle
    @param pBlockType        (IN)  - Block type (video, audio, subtitle) retrieved
    @param pnStream          (OUT)  - stream retrieved
    @param pBlockNode        (OUT)  - Block structure, must be pointed to a valid memory region

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleBlock( TitleL3Handle   titleHandle,
                               DMFBlockType_t *pBlockType,
                               uint32_t       *pnStream,
                               DMFBlockNode_t *pBlockNode );

/** Get title block by stream, retrieves a block based on the stream number.

    Note:  This is not the most efficient API to use, consider using L3cmr_GetTitleBlock (w/ L3cmr_SetActiveStream) to
           more efficiently pull blocks from the file.

    @param titleHandle        (IN)  - Title handle
    @param blockType          (IN)  - Block type, video, audio, subtitle
    @param streamNumOfType    (IN)  - index of that stream type
    @param pBlockNode        (OUT)  - Block structure, must be pointed to a valid memory region

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleBlockByStream( TitleL3Handle   titleHandle,
                                       DMFBlockType_t  blockType,
                                       int32_t         streamNumOfType,
                                       DMFBlockNode_t *pBlockNode );

/** L3cmr_Gets the index of the next key frame from title

    @param titleHandle   (IN)  - Title handle
    @param pTime         (OUT) - Position of the next sync point.

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */

DivXError L3cmr_GetNextSyncPointFromTitle( TitleL3Handle titleHandle,
                                           DivXTime *pTime );

/** L3cmr_Gets the index of the previous key frame from title

    @param titleHandle   (IN)  - Title handle
    @param pTime         (OUT) - Position of the next sync point.

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */

DivXError L3cmr_GetPreviousSyncPointFromTitle( TitleL3Handle titleHandle,
                                               DivXTime *pTime );


/** Sets which streams to retrieve with GetMenuBlock

    Note:  This is used with the new efficient API for the currently active stream

    @param menuHandle         (IN)  - Menu handle
    @param blockType          (IN)  - Block type (video, audio, subtitle) retrieved
    @param nStream            (OUT)  - stream retrieved
    @param bActive            (IN)  - Sets stream active (true) or inactive (false)

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetMenuActiveStream( MenuL3Handle   menuHandle,
                                     DMFBlockType_t blockType,
                                     int32_t        nStream,
                                     DivXBool       bActive );

/** Reads a chunk from a title, used with L3cmr_SetActiveStream.

    Note:  This is the most efficient API for pulling stream data from the file.  This should be used whenever
           possible.

    @param menuHandle         (IN)  - Title handle
    @param pBlockType         (IN)  - Block type (video, audio, subtitle) retrieved
    @param pnStream          (OUT)  - stream retrieved
    @param pBlockNode        (OUT)  - Block structure, must be pointed to a valid memory region

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuBlock( MenuL3Handle    menuHandle,
                              DMFBlockType_t *pBlockType,
                              uint32_t       *pnStream,
                              DMFBlockNode_t *pBlockNode );

/** Get title block by stream, retrieves a block based on the stream number.

    Note:  This is not the most efficient API to use, consider using L3cmr_GetTitleBlock (w/ L3cmr_SetActiveStream) to
           more efficiently pull blocks from the file.

    @param menuHandle         (IN)  - menu handle
    @param blockType          (IN)  - Block type, video, audio, subtitle
    @param streamNumOfType    (IN)  - index of that stream type
    @param pBlockNode         (OUT)  - Block structure, must be pointed to a valid memory region

    @return DivXError    (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuBlockByStream( MenuL3Handle    menuHandle,
                                      DMFBlockType_t  blockType,
                                      int32_t         streamNumOfType,
                                      DMFBlockNode_t *pBlockNode );

/** L3cmw_Sets the title block position by block number, optionally returns time

    @param titleHandle     (IN)     - The title handle for setting block position
    @param blockType       (IN)     - The type of block to grab (video, audio or subtitle)
    @param streamNumOftype (IN)     - The stream index value
    @param time            (OUT)    - Optional: The actual time set returned, set to null to option out

    @return DivXError      (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetTitlePosition( TitleL3Handle  titleHandle,
                                  DMFBlockType_t blockType,
                                  int32_t        streamNumOfType,
                                  DivXTime      *time );

/** L3cmw_Sets the menu block position by block number

    @param menuHandle       (IN)     - The menu handle for setting block position
    @param blockType        (IN)     - The type of block to grab (video, audio or subtitle)
    @param streamNumOfType  (IN)     - The stream index value
    @param time             (OUT)    - Optional: The actual time set returned, set to null to option out

    @return DivXError      (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetMenuPosition( MenuL3Handle   menuHandle,
                                 DMFBlockType_t blockType,
                                 uint32_t       streamNumOfType,
                                 DivXTime       *time);

/** L3cmr_Gets info based on the query Id

    This is a catch all for calls not handled by L3.

    @param hContainer         (IN)  - Container handle
    @param QueryValueID       (IN)  - Query ID
    @param Value              (OUT) - Variant for passing data out
    @param Index              (IN)  - Index of data to get

    @return DivXError         (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetInfo( ContainerL3Handle hContainer,
                         int32_t           QueryValueID,
                         DMFVariant       *Value,
                         int32_t           Index );

/** L3cmr_Sets info based on the query Id

    This is a catch all for calls not handled by L3.

    @param hContainer         (IN)  - Container handle
    @param QueryValueID       (IN)  - Query ID
    @param Value              (IN) - Variant for passing data in
    @param Index              (IN)  - Index of data to get
    @param Value              (OUT) - Variant for passing data out

    @return DivXError         (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetInfo( ContainerL3Handle inHandle,
                         int32_t           QueryValueID,
                         DMFVariant       *Value,
                         int32_t           Index,
                         DMFVariant       *Output);

/** L3cmr_Get DRM Support Level

    @param inHandle           (IN) - Title handle
    @param drmLevel           (OUT) - DRM Support Level (0, 1, 2 and 3)

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleDrmSupportLevel( TitleL3Handle         inHandle,
                                         DMFDrmSupportLevel_e *drmLevel );

/** L3cmr_Get DRM Support Level

    @param inHandle           (IN) - Menu handle
    @param drmLevel           (IN) - L3cmw_Set the DRM level to get

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuDrmSupportLevel( MenuL3Handle          inHandle,
                                        DMFDrmSupportLevel_e *drmLevel );

/** L3cmr_Gets the Title DRM Data block

    @param inHandle          (IN) - Title handle
    @param drmLevel          (IN) - L3cmw_Set the DRM level to get
    @param buffer            (OUT) - A buffer for the data to be retrieved into
    @param size              (OUT) - Maximum size of the buffer (for DRM1 approx 2K)

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleDrmData( TitleL3Handle        inHandle,
                                 DMFDrmSupportLevel_e drmLevel,
                                 uint8_t             *buffer,
                                 uint32_t             size );

/** L3cmr_Gets the Menu DRM Data block

    @param inHandle       (IN) - Menu handle
    @param drmLevel    (IN) - L3cmw_Set the DRM level to get
    @param buffer            (OUT) - A buffer for the data to be retrieved into
    @param size              (OUT) - Maximum size of the buffer (for DRM1 approx 2K)

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuDrmData( MenuL3Handle         inHandle,
                                DMFDrmSupportLevel_e drmLevel,
                                uint8_t             *buffer,
                                uint32_t             size );

/** Get the titles offset in the file.

    @param inHandle      (IN) - Title handle
    @param offset            (OUT) - The offset in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetTitleOffsetFromTitle(TitleL3Handle inHandle, uint64_t *offset);

/** Get the titles offset in the file.

    @param inHandle       (IN) - Menu handle
    @param offset            (OUT) - The offset in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMenuOffsetFromMenu(MenuL3Handle inHandle, uint64_t *offset);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromTitleVideoMedia(VideoMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromTitleAudioMedia(AudioMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromTitleSubtitleMedia(SubtitleMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromMenuVideoMedia(VideoMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromMenuAudioMedia(AudioMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Gets the track id of the stream.

    @param inHandle          (IN) - Handle to the track.
    @param absoluteTrack    (OUT) - The actual track id in the file.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAbsoluteTrackNumberFromMenuOverlayMedia(OverlayMediaL3Handle inHandle, int32_t *absoluteTrack);

/** Sets the gap for sync points in the interal index cache for playback.
    For example a value of 300 would only store sync points every 300 video
    frames only allowing seeking to those points.

    @param inHandle     (IN) - Handle to the track.
    @param syncGap      (IN) - The min gap between keyframes stored in internal index.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetInternalIndexSyncGap(ContainerL3Handle inHandle, int32_t syncGap);

/** Will set the AVI 2.0 parser to use the reduced memory internal indexing scheme.

    @param inHandle     (IN) - Handle to the track.
    @param useCacheless (IN) - Set to non-zero to use the cacheless indexing scheme.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetCachelessIndexScheme(ContainerL3Handle inHandle, uint8_t useCacheless);

/** Retreives the AVI 2.0 parser flag indicating the reduced memory internal indexing scheme..

    @param inHandle           (IN) - Handle to the title.
    @param useCacheless (OUT) - Returns 1 if using use the cacheless indexing scheme.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetCachelessIndexScheme(ContainerL3Handle inHandle, uint8_t *useCacheless);

/** Will set the AVI 2.0 parser to use another reduced memory internal indexing scheme.

    @param inHandle     (IN) - Handle to the track.
    @param useMinMem    (IN) - Set to non-zero to use the min mem indexing scheme.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_SetMinMemUsage(ContainerL3Handle inHandle, uint8_t useMinMem);

/** Retreives the AVI 2.0 parser flag indicating the another reduced memory internal indexing scheme..

    @param inHandle           (IN) - Handle to the title.
    @param useMinMem      (OUT) - Returns 1 if using use the min mem indexing scheme.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetMinMemUsage(ContainerL3Handle inHandle, uint8_t *useMinMem);

/** Retreives the suggested buffer size of the reading of blocks from the title.

    @param inHandle           (IN) - Handle to the title.
    @param suggestBufferSize (OUT) - The suggest size for the read buffer.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
 
DivXError L3cmr_GetCurrentTitleSuggestedBufferSize( TitleL3Handle inHandle, int32_t *suggestBufferSize );

/** Retreives the suggested buffer size of the reading of blocks from the menu.

    @param inHandle           (IN) - Handle to the title.
    @param suggestBufferSize (OUT) - The suggest size for the read buffer.

    @return DivXError        (OUT) - Returns a DivXError error code (See DMFErrors.h)
 */
DivXError L3cmr_GetCurrentMenuSuggestedBufferSize( MenuL3Handle inHandle, int32_t *suggestBufferSize );

/*!
    Return the tags to the first entry

    @param handle         (IN) - A handle to the instance

    @return DivXError
*/
DivXError L3cmr_FirstTag( TitleL3Handle handle);

/*!
    Keep grabbing tags until we have no more

    @param handle         (IN) - A handle to the instance

    @return DivXError
*/
DivXError L3cmr_NextTag( TitleL3Handle handle);

/*!
    Retrieve the tag size and other information

    @param handle         (IN) - A handle to the instance
    @param tagInfo       (OUT) - information about the tag including size and index information

    @return DivXError
*/
DivXError L3cmr_GetTagInfo( TitleL3Handle handle, DMFTagInfo_t* tagInfo );

/*!
    Retrieve the tag size and other information

    @param handle         (IN) -  A handle to the instance

    @param type           (IN) -  Enum of target type to retrieve

    @param index          (IN) -  Index of target type to retrieve

    @param value          (OUT) - Value of target

    @return DivXError
*/
DivXError L3cmr_GetTargetByTypeIdx(TitleL3Handle handle, DMF_TAG_TARGET_e type, 
                                   int32_t index, int32_t *value);

/*!
    Retrieve the tag

    @param handle         (IN)  - A handle to the instance
    @param tagData        (OUT) - Actual tag data

    @return DivXError
*/
DivXError L3cmr_GetTag( TitleL3Handle handle, uint8_t* tagData );

/*!
    Get video track information

    @param handle         (IN)  - A handle to the instance
    @param pTrackProp    (OUT)  - Get the track properties

    @return DivXError
*/
DivXError L3cmr_GetVideoTrackInfo( TitleL3Handle handle, uint32_t index, DMFTrackProp_t* pTrackProp );

/*!
    Get video master track information

    @param handle         (IN)  - A handle to the instance
    @param pTrackProp    (OUT)  - Get the master track properties

    @return DivXError
*/
DivXError L3cmr_GetVideoMasterTrackInfo( TitleL3Handle handle, uint32_t index, DMFMasterTrackProp_t* pMasterTrackProp );

/*!
    Get video trick track information

    @param handle         (IN)  - A handle to the instance
    @param pTrackProp    (OUT)  - Get the trick track properties

    @return DivXError
*/
DivXError L3cmr_GetVideoTrickTrackInfo( TitleL3Handle handle, uint32_t index, DMFTrickTrackProp_t* pTrickTrackProp );

/**
    L3cmr_Gets the properties for the audio media.

    @param inHandle         (IN) - Input handle for accessing the element.  Call a higher level L3cmr_Open to get.
    @param pAudioMediaProp (OUT) - Returns audio media property
    @param nAudioMedia      (IN) - Audio media index
    
   @return DivXError   (OUT) - Returns error code (See DMFErrors.h)
 */
DivXError L3cmr_GetAudioMediaPropFromMenuMedia( MenuMediaL3Handle   inHandle,
                                                 DMFAudioMediaProp_t *pAudioMediaProp,
                                                 int32_t              nAudioMedia );

#ifdef __cplusplus
}
#endif

#endif /* _LAYER3READPUBLISHED_H_ */
/**  @}  */
