/********************************************************************
* $Id: EdtdDocumentSession.h 58500 2009-02-18 19:45:46Z jbraness $
*
* Copyright © 2008 DivX, Inc. http://www.divx.com/company
* All rights reserved.
*
* This software is the confidential and proprietary information of
* DivX, Inc. and may be used only in accordance with the terms of
* your license from DivX, Inc.
********************************************************************/

#ifndef _EdtdDocumentSession_h_DF8B0A3A_0C19_476e_80CA_4292D25876C8__INCLUDED_
#define _EdtdDocumentSession_h_DF8B0A3A_0C19_476e_80CA_4292D25876C8__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include "EdtdDocument.h"

typedef struct _EdtdDocumentSession_t *EdtdDocumentSessionHandle;

DivXError EdtdDocumentSession_Create( EdtdDocumentSessionHandle *pHandle, DivXMem hMem );

DivXError EdtdDocumentSession_Destroy( EdtdDocumentSessionHandle handle );

DivXError EdtdDocumentSession_SetDocument( EdtdDocumentSessionHandle handle, EdtdDocumentHandle hDocHandle );

DivXError EdtdDocumentSession_GetCurrentElement( EdtdDocumentSessionHandle handle, EdtdElementHandle* pElemHandle );

DivXError EdtdDocumentSession_StepNextSibling( EdtdDocumentSessionHandle handle );

DivXError EdtdDocumentSession_StepPrevSibling( EdtdDocumentSessionHandle handle );

DivXError EdtdDocumentSession_StepToChild( EdtdDocumentSessionHandle handle );

DivXError EdtdDocumentSession_GetNumExplicitChildren( EdtdDocumentSessionHandle hThis, const DivXString* sParentName, uint32_t* pNumChildren );

DivXError EdtdDocumentSession_GetNumExplicitChildrenExp( EdtdDocumentSessionHandle hThis, EdtdElementHandle elemHandle, uint32_t* pNumChildren);

DivXError EdtdDocumentSession_GetExplicitChild(EdtdDocumentSessionHandle handle, const DivXString* sParentName, uint32_t childIndex, EdtdElementHandle* childElem);

DivXError EdtdDocumentSession_GetExplicitChildExp(EdtdDocumentSessionHandle handle, EdtdElementHandle parentHandle, uint32_t childIndex, EdtdElementHandle* childElem);

DivXError EdtdDocumentSession_IsExplicitChild(EdtdDocumentSessionHandle handle, EdtdElementHandle parentHandle, EdtdElementHandle childHandle, DivXBool* pVal); 

DivXError EdtdDocumentSession_StepToParent( EdtdDocumentSessionHandle handle );

DivXError EdtdDocumentSession_GetCurrentLevel( EdtdDocumentSessionHandle handle, uint32_t* pLevel );

/*! Return whether or not the current node lists the current parent as a parent

    @param handle       (IN)  - handle to session
    @param pVal         (OUT) - true if this node is actually declared as a child of the current parent

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError EdtdDocumentSession_IsCurrentExplicit( EdtdDocumentSessionHandle handle, DivXBool* pVal );

/*! Return whether or not the current node has a "%children;" descendent

    @param handle       (IN)  - handle to session
    @param pVal         (OUT) - true if this node (or descendent) may adopt the current node's siblings

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError EdtdDocumentSession_IsCurrentAdoptive( EdtdDocumentSessionHandle handle, DivXBool* pVal );

#ifdef __cplusplus
}
#endif
    
#endif /* _EdtdDocumentSession_h_DF8B0A3A_0C19_476e_80CA_4292D25876C8__INCLUDED_ */
