/*!
    @file
@verbatim
$Id: DivXNetFile.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _DIVXNETFILE_H_
#define _DIVXNETFILE_H_

#include "DivXTypes.h"
#include "DivXString.h"
#include "DivXError.h"

#include "DivXProxy.h"
#include "DivXNetFileEx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! DNF (DivX Net File) 
 
      The DivXNetFile implementation provides a direct and a sparse file implementation of a 
      networked file.  

      With sparse file turned on DivXNetFile will keep a local cache of data that has already been 
      requested.  This speeds up execution and allows for progressive playback architecture.

      With sparse file turned off DivXNetFile will always retrieve file data from the network and 
      greatly reduce the memory and system overhead of maintaining a local cache.
      
      Requests for byte ranges are kept in a list and readback of data can be halted and resumed
      as needed.

      Reads of data will be checked against the sparse file (if it exists) and returned immediately
      if the data is locally cached.  Otherwise reads will return a failure unless sparse file is 
      turned off, in which case reads will block waiting for data to be returned from the network.

      Network status can be retrieved by calling DivXNetFileState.
 */

typedef enum {
  DNF_STATE_DOWNLOADING = 1, /*!< remote connection active */
  DNF_STATE_COMPLETED,       /*!< download of file complete */
  DNF_STATE_DISCONNECTED,    /*!< connection to server lost before completed download */
  DNF_STATE_EXPIRED,         /*!< access to file has expired */
  DNF_STATE_ERROR,           /*!< generic error; temp files could be currupt; delete them */
  DNF_STATE_CLOSED           /*!< all internal resources closed */
} DNF_STATE_e;


/*! Sets a memory handle for pool based memory allocation

    @note This function sets the pool from which to draw memory, NULL == heap

    @param  DivXMem      (IN)  - Memory handle to memory pool  
    @return DivXError   (OUT)  - Returns a DivXString* to an absolute path or NULL (for error)
*/
DivXError DivXNetFileSetMemory(DivXMem hMem);

/*! Creates a new remote or local file

    @param DivXString*  (IN)     - File name to create
    @param DivXString*  (IN)     - Server URI path
    @param int32_t      (IN)     - Open flags:
      @return DNF_SparseFile
      @return DNF_WriteOnlyMode         
      @return DNF_ReadWriteMode        
      @return DNF_BinaryMode           
      @return DNF_TextMode              
      @return DNF_RandomAccessMode      
      @return DNF_SequentialAccessMode  
      @return DNF_AppendMode          
      @return DNF_TruncateMode          
      @return DNF_FailCreateOnExistMode 
      @return DNF_FailOnShare          
    @param DivXProxy    (IN)      - DivXProxy Settings, see DivXProxy for more configuration settings.
                                    Set to NULL if no Proxy Configuration
    @param int32_t      (IN)      - File permissions:
      @return DNF_ReadPermission       
      @return DNF_WritePermission     
      @return DNF_ReadWritePermission 

    @return DivXError  (OUT)      - Returns errors:
      @return DNFC_BadFileName 
      @return DNFC_InvalidProxy
      @return DNFC_CannotCreateInReadOnlyMode 
      @return DNFC_MustSpecifyPermissions 
      @return DNFC_AccessModeError 
      @return DNFC_FileExists 
      @return DNFC_InvalidOption 
      @return DNFC_TooManyOpenFiles 
      @return DNFC_InvalidFileOrPath 
      @return DNFC_FileCreateFailed
*/
DivXError DivXNetFileCreate(const DivXString *sFilename, const DivXString *sServerURI, int32_t iOpenFlags, DivXProxy proxyConfig, DivXNetFilePermissions_e iFilePermissions,DivXNetFile *pHandle);

/*! Open a remote or local file

    @param DivXString*  (IN)     - File name to open
    @param DivXString*  (IN)     - Server URI path
    @param int32_t      (IN)     - Open flags:
      @return DNF_SparseFile
      @return DNF_ReadOnlyMode
      @return DNF_WriteOnlyMode         
      @return DNF_ReadWriteMode        
      @return DNF_BinaryMode           
      @return DNF_TextMode              
      @return DNF_RandomAccessMode      
      @return DNF_SequentialAccessMode  
      @return DNF_AppendMode          
      @return DNF_TruncateMode          
      @return DNF_FailOnShare          
    @param DivXProxy    (IN)      - DivXProxy Settings, see DivXProxy for more configuration settings.
                                    Set to NULL if no Proxy Configuration
    @param DivXNetFile* (IN)      - File handle:

    @return DivXError  (OUT)      - Returns errors:
      @return DNFC_BadFileName 
      @return DNFC_InvalidProxy
      @return DNFC_CannotCreateInReadOnlyMode 
      @return DNFC_MustSpecifyPermissions 
      @return DNFC_AccessModeError 
      @return DNFC_FileExists 
      @return DNFC_InvalidOption 
      @return DNFC_TooManyOpenFiles 
      @return DNFC_InvalidFileOrPath 
      @return DNFC_FileCreateFailed
*/
DivXError DivXNetFileOpen(const DivXString *sFilename,int32_t iOpenFlags, DivXProxy proxyConfig, DivXNetFile *pHandle);

/*! Returns the file state

    @param  DivXMem      (IN)  - Memory handle to memory pool  
    @return DNF_STATE_e (OUT)  - Returns the file download state
*/
DNF_STATE_e DivXNetFileState(DivXNetFile handle);

/*!
    Seeks to a position within the remote file

    @param DivXNetFile        (IN)     - DivXNetFile handle input
    @param int64_t            (IN)     - Byte offset within the file to seek to
    @param DivXSeekLocation_e (IN)     - Seek location:
                                            DSL_Start
                                            DSL_Current
                                            DSL_End
    @param uint64_t*          (OUT)    - Resulting location in the file
    @return DivXError         (OUT)    - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_InvalidOffset
      @return DNFC_InvalidOrigin
      @return DNFC_InvalidPointerLocation
*/
DivXError  DivXNetFileSeek(DivXNetFile handle, uint64_t offset, DivXSeekLocation_e origin, uint64_t *pLocation);

/*!
    Request data to be read from the remote file

    @param DivXNetFile   (IN)     - DivXNetFile handle input
    @param uint64_t      (IN)     - Offset into the file
    @param uint32_t     (OUT)     - Length of file to request

    @return DivXError   (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
      @return DNFC_InvalidOffset
      @return DNFC_InvalidLength
*/
DivXError DivXNetFileRequest(DivXNetFile handle, uint64_t offset, uint32_t len);

/*!
    Stops all requested data from being read from the remote file

    @param DivXNetFile   (IN)     - DivXNetFile handle input
    @param uint64_t      (IN)     - Offset into the file
    @param uint32_t     (OUT)     - Length of file to request

    @return DivXError   (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
*/
DivXError DivXNetFileRequestHalt(DivXNetFile handle);

/*!
    Resumes all requested data reads from the remote file

    @param DivXNetFile   (IN)     - DivXNetFile handle input
    @param uint64_t      (IN)     - Offset into the file
    @param uint32_t     (OUT)     - Length of file to request

    @return DivXError   (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
*/
DivXError DivXNetFileRequestResume(DivXNetFile handle);

/*!
    Reads data from the remote file

    @param DivXNetFile   (IN)     - DivXNetFile handle input
    @param void*        (OUT)     - Data buffer to read into
    @param uint32_t      (IN)     - Count of data to read
    @param uint32_t*    (OUT)     - Actual bytes read

    @return DivXError   (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
      @return DNFC_InvalidBuffer
      @return DNFC_InvalidLength
      @return DNFC_EOF
      @return DNFC_InvalidBytesReadPointer
*/
DivXError DivXNetFileRead(DivXNetFile handle, void *buffer, uint32_t len, uint32_t *pBytesRead );

/*!
    Writes data to a remote file

    @param DivXNetFile   (IN)     - DivXNetFile handle input
    @param void*         (IN)     - Data buffer to read into
    @param uint32_t      (IN)     - Count of data to write
    @param uint32_t*    (OUT)     - Actual bytes written

    @return DivXError   (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
      @return DNFC_InvalidBuffer
      @return DNFC_InvalidLength
      @return DNFC_InvalidBytesWrittenPointer
*/
DivXError DivXNetFileWrite(DivXNetFile handle, const void *buffer, uint32_t count, uint32_t *pBytesWritten);

/*!
    Closes a file

    @param DivXNetFile      (IN)     - DivXNetFile handle input

    @return DivXError      (OUT)     - Return errors:
      @return DNFC_InvalidHandle
      @return DNFC_NoConnection
*/
DivXError DivXNetFileClose(DivXNetFile handle);

#ifdef __cplusplus
}
#endif

#endif
// Last line left intentionally blank

