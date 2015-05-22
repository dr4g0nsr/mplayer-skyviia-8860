/*!
    @file
   @verbatim
   $Id: DMFHttpAuth.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFHTTPAUTH_H_
#define _DMFHTTPAUTH_H_

#include "DivXTypes.h"

/*! This struct contains the username and password to complete http authorization across the network.
 */
#define HTTP_CREATE 0x01
#define HTTP_EXISTING 0x02

typedef struct _DMFHttpAuth
{
    DivXString *url;
    DivXString *username;
    DivXString *password;

    uint32_t maxBlock;
    uint32_t minBlock;
    uint32_t bandwidth;

    DivXString *basePath;
    DivXString *dataPath;
    DivXString *statusPath;
    DivXString *outputPath;

    uint8_t mode;
} DMFHttpAuth;

#endif /* _DMFHTTPAUTH_H_
          The following lines left intentionally blank */
