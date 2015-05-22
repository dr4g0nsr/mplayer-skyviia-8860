/*!
    @file
   @verbatim
   $Id: DMFContainerWriteState.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFCONTAINERWRITESTATE_H_
#define _DMFCONTAINERWRITESTATE_H_

/*! Container write state definitions returned by L1 GetNextState()
 */
typedef enum _DMFContainerWriteState
{
    DMF_CWSTATE_NOT_WRITING  = -3,  /*!< Container is not in a writing state */
    DMF_CWSTATE_HANDLE_ERROR = -2,    /*!< Error returning state - invalid container handle */
    DMF_CWSTATE_INVALID      = -1,  /*!< Current state invalid - container will not be usable */
    DMF_CWSTATE_TITLE        = 0,   /*!< Ready for AddTitle */
    DMF_CWSTATE_STREAMHEADERS,      /*!< Ready for AddStream */
    DMF_CWSTATE_STREAMDATA,         /*!< Ready for WriteNextBlock */
    DMF_CWSTATE_MENURIFF,           /*!< Ready for AddMenu */
    DMF_CWSTATE_MRIF,               /*!< Ready for AddMenuMedia */
    DMF_CWSTATE_DONE                /*!< Finished, nothing more to be added */
}
DMFContainerWriteState;

#endif /* _DMF_CONTAINER_WRITE_STATE_H_ */
