/********************************************************************
* $Id: EdtdParser.h 58500 2009-02-18 19:45:46Z jbraness $
*
* Copyright © 2008 DivX, Inc. http://www.divx.com/company
* All rights reserved.
*
* This software is the confidential and proprietary information of
* DivX, Inc. and may be used only in accordance with the terms of
* your license from DivX, Inc.
********************************************************************/

#ifndef _EdtdParser_h_8D8FFFEC_FE46_4c34_806A_8455BE28652C__INCLUDED_
#define _EdtdParser_h_8D8FFFEC_FE46_4c34_806A_8455BE28652C__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include "EdtdDocument.h"

typedef void *EdtdParserHandle;

DivXError EdtdParser_Create( EdtdParserHandle *pHandle, DivXMem hMem );

DivXError EdtdParser_Destroy( EdtdParserHandle handle );

DivXError EdtdParser_AddInputFile( EdtdParserHandle handle, const DivXString* sFileName );

DivXError EdtdParser_AddInputString( EdtdParserHandle handle, const DivXString* sInput );

DivXError EdtdParser_CreateDocument( EdtdParserHandle handle, EdtdDocumentHandle* pDocHandle );

#ifdef __cplusplus
}
#endif

#endif /* _EdtdParser_h_8D8FFFEC_FE46_4c34_806A_8455BE28652C__INCLUDED_ */
