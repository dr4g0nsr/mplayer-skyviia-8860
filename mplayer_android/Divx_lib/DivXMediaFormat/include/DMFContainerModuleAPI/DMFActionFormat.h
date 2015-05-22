/*!
    @file
   @verbatim
   $Id: DMFActionFormat.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DIVXACTIONFORMAT_H_
#define _DIVXACTIONFORMAT_H_

#include "DivXTypes.h"

/* This almost makes me cry that I have to do this, but I'll be tough and do it anyway.  This
   code "must" be modified to be portable in the next sprint. */
#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(4)
#elif defined ( MACOSX )
#pragma pack(push,4)
#elif defined ( __GNUC__ )
#pragma pack(push,4)
#else
#pragma pack(push)
#pragma pack(4)
#endif

/*! Defines the possible actions
 */
typedef enum
{
    L2AudioSelectEnum,
    L2SubtitleSelectEnum,
    L2PlayEnum,
    L2ResumeEnum,
    L2MenuTransitionEnum,
    L2ButtonSelectEnum,
} L2Action_e;

/*! Subtitle Select structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2SubtitleSelect
{
    uint32_t track;
    int16_t  title;
} L2SubtitleSelect;

/*! Resume structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2Resume
{
    int16_t title;
} L2Resume;

/*! Play structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2Play
{
    int32_t mediaObjectID;
    int16_t title;
} L2Play;

/*! Audio Select structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2AudioSelect
{
    uint32_t track;
    int16_t  title;
} L2AudioSelect;

/*! Menu Transition structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2MenuTransition
{
    int16_t menu;
    int16_t button;
} L2MenuTransition;

/*! Button Select structure

    @Note This structures are assumed to be packed
 */
typedef struct _L2ButtonSelect
{
    int16_t button;
} L2ButtonSelect;

/*! Action Union, defines the possible action structures available

    @Note All contained structures are assumed to be packed
 */
typedef union _L2ActionUnion
{
    L2AudioSelect    audioselect;
    L2SubtitleSelect subtitleselect;
    L2Play           play;
    L2Resume         resume;
    L2MenuTransition menutransition;
    L2ButtonSelect   buttonselect;
} L2ActionUnion;

/*! This struct contains a union and a type enumeration for getting at all
    of the ActionStructures

    @Note All contained structures are assumed to be packed
 */
typedef struct _L2ActionStruct
{
    L2Action_e    type;
    L2ActionUnion u;
} L2ActionStruct;

#pragma pack(pop)

#endif /* _DIVXACTIONFORMAT_H_ */
