/*!
    @file
@verbatim
$Id: DivXFile.h 58738 2009-03-03 02:14:18Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXFILE_H_
#define _DIVXFILE_H_

#if ! defined _MSC_VER

/* Force 64 bit file support for libraries using LSF support (linux with GNU C library) */
//#ifndef LARGE_FILE_SUPPORT
#define LARGE_FILE_SUPPORT
//#endif /* end of define LARGE_FILE_SUPPORT */
//#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
//#endif /* end of define _LARGEFILE_SOURCE */
//#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
//#endif /* end of define _LARGEFILE64_SOURCE */
#define _FILE_OFFSET_BITS 64

#endif

#include "DivXFileEx.h"

#ifdef __cplusplus
extern "C"
{
#endif

DivXError DivXFileSetMemory(DivXMem hMem);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXFileDeallocateDivXString(DivXString *pString);

/*! Converts a relative path to a full path based on the executables current location.

    @note This function allocates memory from heap, not the memory manager

    @param DivXString*  (IN)   - Takes a relative path
    @return DivXString* (OUT)  - Returns a DivXString* to an absolute path or NULL (for error)
*/
DivXString* DivXFileRelativePathToFullPath(const DivXString*);

/*! Creates a new file

    @param sFilename  (IN)     - File name to create
    @param iOpenFlags (IN)     - Open flags:
                                      DF_WriteOnlyMode         
                                      DF_ReadWriteMode        
                                      DF_BinaryMode           
                                      DF_TextMode              
                                      DF_RandomAccessMode      
                                      DF_SequentialAccessMode  
                                      DF_AppendMode          
                                      DF_TruncateMode          
                                      DF_FailCreateOnExistMode 
                                      DF_FailOnShare          
    @param iFilePermissions (IN)  - File permissions:
                                      DF_ReadPermission       
                                      DF_WritePermission     
                                      DF_ReadWritePermission 
    @param pHandle     (OUT)      - handle of the resulting file
    @return DivXError  (OUT)      - Returns errors:
                                      DFC_BadFileName 
                                      DFC_CannotCreateInReadOnlyMode 
                                      DFC_MustSpecifyPermissions 
                                      DFC_AccessModeError 
                                      DFC_FileExists 
                                      DFC_InvalidOption 
                                      DFC_TooManyOpenFiles 
                                      DFC_InvalidFileOrPath 
                                      DFC_FileCreateFailed
*/
DivXError DivXFileCreate(const DivXString *sFilename, int32_t iOpenFlags, DivXFilePermissions_e iFilePermissions,DivXFile *pHandle);

/*! Open a file

    @param sFilename    (IN)     - File name to open
    @param iOpenFlags   (IN)     - Open flags:
                                      DF_ReadOnlyMode
                                      DF_WriteOnlyMode         
                                      DF_ReadWriteMode        
                                      DF_BinaryMode           
                                      DF_TextMode              
                                      DF_RandomAccessMode      
                                      DF_SequentialAccessMode  
                                      DF_AppendMode          
                                      DF_TruncateMode          
                                      DF_FailOnShare          
    @param pHandle     (OUT)      - handle of the resulting file
    @return DivXError  (OUT)      - Returns errors:
                                      DFC_BadFileName 
                                      DFC_CannotCreateInReadOnlyMode 
                                      DFC_MustSpecifyPermissions 
                                      DFC_AccessModeError 
                                      DFC_FileExists 
                                      DFC_InvalidOption 
                                      DFC_TooManyOpenFiles 
                                      DFC_InvalidFileOrPath 
                                      DFC_FileCreateFailed
*/
DivXError DivXFileOpen(const DivXString *sFilename,int32_t iOpenFlags, DivXFile *pHandle);

/*!
    Seeks to a position within the file

    @param handle             (IN)     - DivXFile handle input
    @param offset             (IN)     - Byte offset within the file to seek to
    @param origin             (IN)     - Seek location:
                                            DSL_Start
                                            DSL_Current
                                            DSL_End
    @param pLocation          (OUT)    - Resulting location in the file
    @return DivXError         (OUT)    - See DivXError.h for error codes
*/
DivXError  DivXFileSeek(DivXFile handle, int64_t offset, DivXSeekLocation_e origin, uint64_t *pLocation);

/*!
    Reads data from the file

    @param handle        (IN)     - DivXFile handle input
    @param buffer       (OUT)     - Data buffer to read into
    @param count         (IN)     - Count of data to read
    @param pBytesRead   (OUT)     - Actual bytes read

    @return DivXError   (OUT)     - See DivXError.h for error codes
*/
DivXError DivXFileRead(DivXFile handle, void *buffer, uint32_t count, uint32_t *pBytesRead );

/*!
    Writes data to a file

    @param handle        (IN)     - DivXFile handle input
    @param buffer        (IN)     - Data buffer to read into
    @param count         (IN)     - Count of data to write
    @param pBytesWritten (OUT)    - Actual bytes written

    @return DivXError   (OUT)     - See DivXError.h for error codes
*/
DivXError DivXFileWrite(DivXFile handle, const void *buffer, uint32_t count, uint32_t *pBytesWritten);

/*!
    Closes a file

    @param handle        (IN)     - DivXFile handle input

    @return DivXError   (OUT)     - See DivXError.h for error codes
*/
DivXError DivXFileClose(DivXFile handle);

/*!
    Loads a text file

    @param sFileName  (IN)     - File name of the file to open
    @param pText      (IN)     - Text buffer ptr to store the string into (allocated in the function)

    @return DivXError (OUT)     - See DivXError.h for error codes
*/
DivXError DivXFileLoadTextFile(const DivXString *sFileName,DivXString **pText);

/*!
    Gets the current working directory

    @note This pointer should be deallocated by calling DivXFileDeallocateDivXString

    @return DivXString*  (OUT)     - Returns a string pointing the current working directory (NULL if error)
*/
DivXString *DivXFileGetCWD();

/*!
    Removes the file pointed to by path

    @param path           (IN)     - Path to the file to delete

    @return DivXError    (OUT)     - DivXError code for File deletion, if failure either write protected or non-existent
*/
DivXError DivXFileDelete(const DivXString* path);

/*!
    Renames oldname to newname

    @param oldname        (IN)     - Path to old file
    @param newname        (IN)     - Path to new file

    @return DivXError    (OUT)     - DivXError code for File deletion, if failure either write protected or non-existent
*/
DivXError DivXFileRename(const DivXString* oldname, const DivXString* newname);

/*!
    Sets the current working directory

    @param sDir          (IN)      - String representing the new current working directory path

    @return DivXError   (OUT)     - See DivXError.h for error codes
*/
DivXError DivXFileSetCWD(const DivXString *sDir);


/*! Returns a temporary file name

    @note In Linux the pDir and pPrefix arguments are not used temporary path is /tmp/fileXXXXX

    @param pDir        (IN)  - Null terminated DivXString*
    @param pPrefix     (IN)  - Null terminated DivXString*
    @return DivXString (OUT) - Created DivXString w/ whole path
*/
DivXString *DivXFileGetTmpFileName(const DivXString *pDir, const DivXString *pPrefix);

/*! Returns a tmp path DivXString

    @note This function gets the temporary path for DivXFile.  Must use SetTmpPath before
          calling this function.

    @return DivXString*      (OUT)  - Path to the temporary directory
*/
const DivXString *DivXFileGetTmpPath();

/*! Set the tmp path DivXString
 
    @param  pDir        (IN)  - Null terminated DivXString*

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXFileSetTmpPath(const DivXString *pDir);

/*! Return the file pointer's position
 
    @param handle        (IN)     - DivXFile handle input
    @param pLocation     (OUT)    - Location in the file

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXFileTell(DivXFile handle, uint64_t *pLocation);

/*! Return the size of file 
 
    @param sFilename    (IN)  - DivXFile filename
    @param pSize        (OUT) - Size of the file

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXFileSize(const DivXString *sFilename, int64_t* size );

/*! Returns path to the file
 
    @param handle       (IN)  - DivXFile handle input
    @param sPath        (OUT) - Pointer to DivXString
    @param bufsize      (IN)  - Number of characters allocated for sPath

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXFileGetFullPath(DivXFile handle, DivXString *sPath, uint32_t bufsize);





/*! Open a directory
 
    @param sPathname    (IN)  - Directory name to open
    @param handle       (IN)  - Pointer to DivXDir handle to associate to directory

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXDirOpen(const DivXString *sPathname, DivXDir *pHandle);

/*! Open a directory (with memory handle)
 
    @param sPathname    (IN)  - Directory name to open
    @param handle       (IN)  - Pointer to DivXDir handle to associate to directory
    @param hMem         (IN)  - Memory pool to use

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXDirExOpen(const DivXString *sPathname, DivXDir *pHandle, DivXMem hMem);

/*! Close a directory
 
    @param handle       (IN)  - DivXDir handle to close

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXDirClose(DivXDir handle);

/*! Get the next file in the directory.
    
    This function will allocate memory for the filename DivXString within the
    DivXDirInfo structure.  Each subsequent call will deallocate the previous
    DivXString and reallocate for the next string.
    
    The final call will deallocate the last string and leave the string pointer 
    as NULL, which, along with a return of DIVX_ERR_SUCCESS, will indicate that the
    last file of the directory has been reached.
 
    @param handle           (IN)  - DivXDir handle to close
    @param DivXDirFileInfo  (OUT) - structure to hold information about the file in the directory

    @return DivXError   (OUT) - See DivXError.h for error codes
*/
DivXError DivXDirGetNextFileInfo(DivXDir handle, DivXDirFileInfo_s *pFileInfo);


#ifdef __cplusplus
}
#endif

#endif
