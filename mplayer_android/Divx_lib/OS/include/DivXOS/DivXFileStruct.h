/*!
    @file
@verbatim
$Id$

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef __DIVX_FILE_STRUCTURES_H__
#define __DIVX_FILE_STRUCTURES_H__

#define SEEK_FROM_CURR 		0 
#define SEEK_FROM_BEGIN		1
#define SEEK_FROM_END		2

typedef void *DivXFileEx;

/* Brief read funtion pointer declaration */
/*
* return value DivXError 
* [ i ] DivXFileEx* 			- file descriptor
* [i/o] void*   	  			- buffer
* [ i ] int32_t 				- bytes count
* [ o ] int32_t* 		 		- bytes read
*/
typedef DivXError (*pfnFileRead)(DivXFileEx*, void* , uint32_t, uint32_t*);

/* Brief seek function pointer declaration */
/*
* return value DivXError 
* [ i ] DivXFileEx* 			- file descriptor
* [ i ] int64_t 	  			- offset
* [ i ] int32_t 				- origin
* [ i ] uint64_t* 		 		- location
*/
typedef DivXError (*pfnFileSeek) (DivXFileEx* , int64_t, int32_t, uint64_t*);

/* Brief write function pointer declaration */
typedef DivXError (*pfnFileWrite)(DivXFileEx*);

// The structure below is intended to pass read/seek/write tools to the file reading/writing layer
// to made file abstraction possible. In case of this structure it is doesn't matter what OS we 
// are currently use and what technology is used for data reading. Engine doesn't know how but supplyed
// functions do.

typedef struct _tagDivxFileExternalObj{
// Handle to an object that represents a file.
	DivXFileEx pFile; 

// File mode flags	
	int32_t iModeFlags;

// File Functions;
	// Read file function pointer
	pfnFileRead fnReadFile;
	
	// Seek file function pointer
	pfnFileSeek fnSeekFile;
	
	// Write File function pointer
	pfnFileWrite fnWriteFile;
}DivxFileExternalObj;
typedef DivxFileExternalObj *PDivXFileExternalObj;

#ifdef __SYMBIAN32__

#define DIVX_FILE_EX(_HDF_) ( _HDF_ != NULL) ? (_HDF_->ext_file ? (_HDF_->ext_file->pFile) : NULL) : NULL

#define EXTFILE_SEEK(_HDF_, _RES_, _ARGS_)\
if(! _HDF_ )\
{\
	_RES_ = DIVX_ERR_INVALID_ARG;\
}\
else\
{\
	if(! _HDF_ ->ext_file)\
	{\
		_RES_ = DIVX_ERR_INVALID_ARG;\
	}\
	else\
	{\
		if(!_HDF_->ext_file->fnSeekFile)\
		{\
			_RES_ = DIVX_ERR_INVALID_ARG;\
		}\
		else\
		{\
			_RES_ = _HDF_->ext_file->fnSeekFile _ARGS_ ;\
		}\
	}\
}

#define EXTFILE_READ(_HDF_, _RES_, _ARGS_)\
if(! _HDF_ )\
{\
	_RES_ = DIVX_ERR_INVALID_ARG;\
}\
else\
{\
	if(! _HDF_ ->ext_file)\
	{\
		_RES_ = DIVX_ERR_INVALID_ARG;\
	}\
	else\
	{\
		if(!_HDF_->ext_file->fnReadFile)\
		{\
			_RES_ = DIVX_ERR_INVALID_ARG;\
		}\
		else\
		{\
			_RES_ = _HDF_->ext_file->fnReadFile _ARGS_ ;\
		}\
	}\
}

#define EXTFILE_WRITE(_HDF_, _RES_, _ARGS_)\
if(! _HDF_ )\
{\
	_RES_ = DIVX_ERR_INVALID_ARG;\
}\
else\
{\
	if(! _HDF_ ->ext_file)\
	{\
		_RES_ = DIVX_ERR_INVALID_ARG;\
	}\
	else\
	{\
		if(!_HDF_->ext_file->fnWriteFile)\
		{\
			_RES_ = DIVX_ERR_INVALID_ARG;\
		}\
		else\
		{\
			_RES_ = _HDF_->ext_file->fnWriteFile _ARGS_ ;\
		}\
	}\
}

#endif
#endif // __DIVX_FILE_STRUCTURES_H__
