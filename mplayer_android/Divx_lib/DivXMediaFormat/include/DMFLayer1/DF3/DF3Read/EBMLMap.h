/*!
    @file
@verbatim
$Id: EBMLMap.h 54769 2008-07-31 04:26:06Z econverse $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _EBMLMAP_H_
#define _EBMLMAP_H_

#include "DF3/DF3Common/DMFInputStream.h"
#include "DivXInt.h"
#include "EdtdDocument.h"

/*! EBML instruction definition */
typedef int32_t EBMLMapInstruction;

/*! EBML type definition */
typedef int32_t EBMLMapType;

typedef enum {
    EBML_ID_GOls
} EBMLID_e;

/*! EBML Map Element definition */
typedef struct __EBMLMapElement__
{
    uint64_t id;
    EBMLDataType_e dataType;
    const DivXString *name;
} EBMLMapElement;

/*! EBML Map Structure definition */
typedef struct __EBMLMap__
{
    int32_t numElements;
    EBMLMapElement* elem;
} EBMLMap;

/*! Errors for DMFebmlParser_Open function */
typedef enum {
    DMFebmlMap_GetInstructionErr_NoError,
    DMFebmlMap_GetInstructionErr_MemHandleInvalid,
    DMFebmlMap_GetInstructionErr_InvalidArgument,
    DMFebmlMap_GetInstructionErr_NotEnoughMemory,
    DMFebmlMap_GetInstructionErr_NumErrors
}DMFebmlMap_GetInstructionErr;
/*! Get instruction from EBML Map

    @note

    @param pEBMLMap         (IN)  - Pointer to EBMLMap handle
    @param type             (OUT) - EBMLMapType
    @param type             (OUT) - EBMLMap

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DMFebmlMap_GetInstruction(EBMLMap* pEBMLMap, EBMLMapType* pType, EBMLMapInstruction* pInst);

DivXBool EBMLMap_Lookup( EBMLMap* pEBMLMap, uint64_t elementID, EBMLMapElement *element );

/*! EBML Map Get Instruction function definition */
typedef DivXError (* pfnDMFebmlMap_GetInstruction)      (struct __EBMLMap__*, EBMLMapType*, EBMLMapInstruction*);

#endif
/* _EBMLMAP_H_ */
