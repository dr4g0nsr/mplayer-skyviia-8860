/*!
    @file
@verbatim
$Id: DivXFileEx.h 56464 2008-10-13 21:01:13Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXFILEEX_H_
#define _DIVXFILEEX_H_

#include "DivXError.h"
#include "DivXMem.h"

#ifdef _MSC_VER
#include <windows.h>   /* for MAX_PATH */
#endif

#ifndef _MSC_VER
#define MAX_PATH 512
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*! Type define for the DivXFile type */
typedef struct _DivXFileStruct_ *DivXFile;

/*! Type define for the DivXDir type */
typedef struct _DivXDirStruct_ *DivXDir;

/*! Structure for passing back file information from a directory */
typedef struct _DivXDirFileInfo_s
{
    DivXString       sFilename[DIVX_MAX_PATH + 1];
    DivXBool         bIsDirectory;
} DivXDirFileInfo_s;

/*! Converts a relative path to a full path based on the executables current location.

    @note This function allocates memory from heap, not the memory manager

    @param path (IN)   - Takes a relative path

    @return DivXString* (OUT)  - Returns a DivXString* to an absolute path or NULL (for error)
*/
DivXString* DivXFileExRelativePathToFullPath(const DivXString* path);

/*! Allocates a DivXFile handle 
    
    @return DivXFile   (OUT)  - DivXFile handle
 */
DivXFile DivXFileExAllocate();

/*! Deallocates a DivXFile handle 
  
    @param handle     (IN)  - DivXFile handle input
    @param hMem       (IN)  - DivXMem input

    @return DivXError  (OUT)  - See DivXError.h for more details
 */
DivXError DivXFileExDeallocate(DivXFile handle, DivXMem hMem);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXFileExDeallocateDivXString(DivXString *pString, DivXMem hMem);

/*! This enumeration defines the file modes */
typedef enum DivXFileMode {
  DF_ReadOnlyMode          = 1, /// only valid with open

  DF_WriteOnlyMode         = 2, /// valid with open or create
  DF_ReadWriteMode         = 3, /// valid with open or create

  DF_BinaryMode            = 0, // valid with open or create (default)
  DF_TextMode              = 4, /// valid with open or create

  DF_RandomAccessMode      = 0, /// valid with open or create, only valid with readwrite or read (default)
  DF_SequentialAccessMode  = 8, /// valid with open or create, only valid with readwrite or read

  DF_AppendMode            = 0,  /// valid with open or create, only vlaid with readwrite or read 
  DF_TruncateMode          = 16, /// valid with open or create, only valid with readwrite or write

  DF_FailCreateOnExistMode = 32, /// only valid with create

  DF_FailOnShare           = 64, /// Set share access to deny

  DF_Invalid 
} DivXFileMode_e;

/*! This enumeration defines the file permissions */
typedef enum DivXFilePermissions {
    DF_ReadPermission       =   1,
    DF_WritePermission      =   2,
    DF_ReadWritePermission  =   3
} DivXFilePermissions_e;

/*! This enumeration defines the file seek location */
typedef enum DivXSeekLocation
{
    DSL_Start,
    DSL_Current,
    DSL_End
} DivXSeekLocation_e;

/*! Creates a new file

    @param sFilename             (IN)   - Input file name
    @param iOpenFlags            (IN)   - Open flags (see DivXFileMode_e)
    @param iFilePermissions_e    (IN)   - File Permissions
    @param pHandle              (OUT)   - File handle 
    @param hMem                  (IN)   - Memory handle for allocation (use NULL for heap allocation)
    
    @return DivXError           (OUT)   - See DivXError.h for error codes 
*/
DivXError DivXFileExCreate(
    const DivXString*     sFilename, 
    int32_t               iOpenFlags, 
    DivXFilePermissions_e iFilePermissions,
    DivXFile*             pHandle, 
    DivXMem               hMem
);

/*! Opens a file
    
    @param sFilename             (IN) - Input file name
    @param iOpenFlags            (IN) - Mode to open the file in (see DivXFileMode_e)
    @param pHandle              (OUT) - DivXFile generated from opening the file 
    @param hMem                  (IN) - Memory handle for memory allocation (use NULL for heap allocation)

    @return DivXError           (OUT) - See DivXOS.h for error codes 
 */
DivXError DivXFileExOpen(
    const DivXString* sFilename,
    int32_t           iOpenFlags, 
    DivXFile*         pHandle, 
    DivXMem           hMem
);

/*! Seeks within a file
    
    @param handle                (IN) - File handle
    @param offset                (IN) - Seek position within the file
    @param origin                (IN) - Seek location (end, beginning, etc...)
    @param pLocation            (OUT) - Resulting location

    @return DivXError           (OUT) - See DivXOS.h for error codes 
 */
DivXError  DivXFileExSeek(
    DivXFile handle, 
    int64_t offset, 
    DivXSeekLocation_e origin, 
    uint64_t *pLocation
);

/*! Reads from the file
    
    @param handle                (IN) - File handle
    @param buffer               (OUT) - Buffer to read data into
    @param count                 (IN) - Size of data to read
    @param pBytesRead           (OUT) - Bytes read

    @return DivXError           (OUT) - See DivXOS.h for error codes 
*/
DivXError DivXFileExRead(
    DivXFile handle, 
    void *buffer, 
    uint32_t count, 
    uint32_t *pBytesRead 
);

/*! Writes to a file
    
    @param handle                (IN) - File handle
    @param buffer               (OUT) - Buffer to read data out of
    @param count                 (IN) - Size of data to write
    @param pBytesWritten        (OUT) - Bytes written

    @return DivXError           (OUT) - See DivXOS.h for error codes 
*/
DivXError DivXFileExWrite(DivXFile handle, const void *buffer, uint32_t count, uint32_t *pBytesWritten);

/*! Closes a file 
    
    @param handle                (IN) - DiXFile handle

    @return DivXError           (OUT) - See DivXOS.h for error codes 
 */
DivXError DivXFileExClose(DivXFile handle);

/*! Loads a text file to string
    
    @param sFileName    (IN) - File name to open
    @param pText        (OUT) - String allocated with file contents
    @param hMem          (IN) - Memory handle for memory allocation (use NULL to allocate from the heap)

    @return DivXError           (OUT) - See DivXOS.h for error codes 
*/
DivXError DivXFileExLoadTextFile(
    const DivXString *sFileName,
    DivXString      **pText, 
    DivXMem hMem
);

/*! Returns the current working directory path

    @note This pointer should be deallocated by calling DivXFileExDeallocateDivXString

    @return DivXString*      (OUT) - Path to the current working directory, string must be deallocated (uses default heap)
 */
DivXString *DivXFileExGetCWD(DivXMem hMem);

/*! Sets the current working directory path

    @param sDir       (IN) - Path to set for the current working directory

    @return DivXError        (OUT) - See DivXError.h for error codes
*/
DivXError DivXFileExSetCWD(const DivXString *sDir);

/*!
    Gets the current working directory

    @param path        (IN)     - Path to the file to delete
    @param hMem        (IN)     - Memory handle


    @return DivXError    (OUT)     - DivXError code for File deletion, if failure either write protected or non-existent
*/
DivXError DivXFileExDelete(const DivXString* path, DivXMem hMem);

/*!
    Renames oldname to newname

    @param oldname        (IN)     - Path to old file
    @param newname        (IN)     - Path to new file
    @param hMem           (IN)     - Memory handle

    @return DivXError    (OUT)     - DivXError code for File deletion, if failure either write protected or non-existent
*/
DivXError DivXFileExRename(const DivXString* oldname, const DivXString* newname, DivXMem hMem);

/*! Finds a DivXString sub string within a DivXString

    @note  From the compiler [../../src/linux/DivXFile.c:861: warning: the use of `tmpnam' is dangerous, better use `mkstemp']
           The mkstemp is used in the function DivXFileExGetTmpFile

    @param pDir        (IN)  - Null terminated DivXString*
    @param pPrefix     (IN)  - Null terminated DivXString*
    @param hMem        (IN)  - Memory handle

    @return DivXString (OUT) - Created DivXString w/ whole path
*/
DivXString *DivXFileExGetTmpFileName(const DivXString *pDir,const DivXString *pPrefix, DivXMem hMem);

/*! Returns a tmp path DivXString

    @note 

    @return DivXString (OUT) - DivXString temporary
*/
const DivXString *DivXFileExGetTmpPath();

/*! Sets a tmp path DivXString

    @param pDir        (IN)  - Null terminated DivXString*

    @param DivXString (IN) - Sets the temporary path string
*/
DivXError DivXFileExSetTmpPath(const DivXString *pDir);

/*! Creates a temporary file whose name is based on the template string

    @note Use DIVX_STR("/tmp/fileXXXXXX") or DIVX_STR("C:/tmp/fileXXXXXX")
          to create the template.

    @param dsTemplate  (IN) - Sets the template string to be used
    @param hMem        (IN) - Memory handle
    
    @return DivXfile  (OUT) - Returns the DivXFile handle created
*/
DivXFile DivXFileExGetTmpFile(const DivXString* dsTemplate, DivXMem hMem);

#ifdef __cplusplus
}
#endif

#endif
