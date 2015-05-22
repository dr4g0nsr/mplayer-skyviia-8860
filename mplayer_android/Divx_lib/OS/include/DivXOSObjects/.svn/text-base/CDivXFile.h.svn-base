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

/*! CDivXFile.h
*/

#ifndef CDIVXFILE_H
#define CDIVXFILE_H

namespace DivXOS
{
	class CDivXFile
	{
	public:
		CDivXFile() {}
		virtual ~CDivXFile() {}

		DivXError SetMemory(CDivXMem hMem);

		DivXString* RelativePathToFullPath(const CDivXString*);

		/* See DivXOS.h for error codes */
		DivXError Open(CDivXString *sFilename, int iOpenFlags, int iFilePermissions);

		/* See DivXOS.h for error codes */
		DivXError Open(CDivXString *sFilename,int iOpenFlags);

		/* See DivXOS.h for error codes */
		DivXError Seek(int64_t offset, DivXSeekLocation_e origin, uint64_t *pLocation);

		/* See DivXOS.h for error codes */
		DivXError Read(void *buffer, unsigned int count, unsigned int *pBytesRead );

		/* See DivXOS.h for error codes */
		DivXError Write(const void *buffer, unsigned int count, unsigned int *pBytesWritten);

		/* See DivXOS.h for error codes */
		DivXError Close();

		/* See DivXOS.h for error codes */
		DivXError LoadTextFile(CDivXString *sFileName,CDivXString **pText);

		DivXString *GetCWD();

		DivXError SetCWD(CDivXString *sDir);

		DivXString *GetTempFileName(const CDivXString *pDir,const CDivXString *pPrefix, CDivXMem *hMem);

		DivXString *GetTempPath();

		int SetTempPath(const CDivXString *pDir);
	};
}

#endif // CDIVXFILE_H
