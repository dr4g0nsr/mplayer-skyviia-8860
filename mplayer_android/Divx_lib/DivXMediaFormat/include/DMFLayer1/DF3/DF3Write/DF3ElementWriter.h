/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/


#ifndef _DF3ElementWriter_h_6FABFA86_3A10_40a9_8A74_6BC33053D0DF__INCLUDED_
#define _DF3ElementWriter_h_6FABFA86_3A10_40a9_8A74_6BC33053D0DF__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include "./DF3/DF3Common/DMFOutputStream.h"
#include "./DF3/DF3Read/EBMLMap.h"
#include "DF3EbmlHelper.h"

#include <stdarg.h>

//TODO make all sizes uint64_t
DivXError WriteSimpleElement(DMFOutputStreamHandle h, uint64_t id, EBMLDataType_e type, uint32_t* bytesWritten, ...);
DivXError WriteBinaryElement(DMFOutputStreamHandle h, uint64_t id, const void* value, uint32_t size, uint32_t* bytesWriten);
DivXError WriteUVInt(DMFOutputStreamHandle h, uint64_t value, uint32_t* bytesWritten);
DivXError WriteUVIntSpecSize(DMFOutputStreamHandle h, uint64_t value, uint32_t size, uint32_t* bytesWritten);
DivXError WriteSVInt(DMFOutputStreamHandle h, int64_t value, uint32_t* bytesWritten);
DivXError WriteId(DMFOutputStreamHandle h, uint64_t id, uint32_t* bytesWritten);
DivXError WriteValue(DMFOutputStreamHandle h, EBMLDataType_e type, uint32_t* bytesWritten, va_list *ap);
DivXError WriteUInt(DMFOutputStreamHandle h, uint64_t value, uint32_t* bytesWritten);
DivXError WriteUIntSpecSize(DMFOutputStreamHandle h, uint64_t value, uint32_t size, uint32_t* bytesWritten);
DivXError WriteSInt(DMFOutputStreamHandle h, int64_t value, uint32_t* bytesWritten);
DivXError WriteFloat(DMFOutputStreamHandle h, double value, uint32_t* bytesWritten);
DivXError WriteFloat4bytes(DMFOutputStreamHandle h, float value, uint32_t* bytesWritten);
DivXError WriteFloat8bytes(DMFOutputStreamHandle h, double value, uint32_t* bytesWritten);
DivXError WriteString(DMFOutputStreamHandle h, const DivXString* value, uint32_t* bytesWritten);
DivXError WriteDate(DMFOutputStreamHandle h, uint64_t value, uint32_t* bytesWritten);
DivXError WriteBinary(DMFOutputStreamHandle h, const void* value, unsigned int size, uint32_t* bytesWritten);
DivXError WriteBool(DMFOutputStreamHandle h, DivXBool value, uint32_t* bytesWritten);
DivXError WriteAscii(DMFOutputStreamHandle h, const char* value, uint32_t* bytesWritten);
DivXError WriteEbmlValue(DMFOutputStreamHandle h, DF3ebml_value *value, uint32_t* bytesWritten);
DivXError WriteRawUInt(DMFOutputStreamHandle h, uint64_t value, uint32_t* bytesWritten);

#ifdef __cplusplus
}
#endif

#endif /* _DF3ElementWriter_h_6FABFA86_3A10_40a9_8A74_6BC33053D0DF__INCLUDED_ */
