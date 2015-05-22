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

#ifndef _DIVXLINKEDLIST_H_
#define _DIVXLINKEDLIST_H_

#include "DivXError.h"
#include "DivXMem.h"
#include "DivXTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _Node_t *Node_tptr;
typedef struct _Node_t
{
    void* data;
    Node_tptr prev;
    Node_tptr next;
} Node_t;
Node_t* Node_Create(DivXMem hMem);
DivXError Node_Destroy(Node_t* node, DivXMem hMem);
DivXError Node_SetData(Node_t* node, void* data);
DivXError Node_SetNext(Node_t* node, Node_t* next);
DivXError Node_GetData(Node_t* node, void** data);

typedef struct _LinkedList_t
{
    Node_tptr head;
    Node_tptr tail;
    DivXMem hMem;
} LinkedList_t;

LinkedList_t* LinkedList_Create(DivXMem hMem);

DivXError LinkedList_Destroy(LinkedList_t* linkedList);

DivXError LinkedList_GetSize(LinkedList_t* linkedList, uint32_t* size);

DivXError LinkedList_IsEmpty(LinkedList_t* linkedList, DivXBool* isEmpty);

/* TODO tested finish all code paths */
DivXError LinkedList_InsertHead(LinkedList_t* linkedList, Node_t* node);

/* TODO tested finish all code paths */
DivXError LinkedList_InsertTail(LinkedList_t* linkedList, Node_t* node);

/* TODO tested finish all code paths */
DivXError LinkedList_InsertAfter(LinkedList_t* linkedList, Node_t* refNode, Node_t* node);

/* TODO tested finish all code paths */
DivXError LinkedList_InsertBefore(LinkedList_t* linkedList, Node_t* refNode, Node_t* node);

/* TODO tested finish all code paths */
/* should implement this in terms of remove */
DivXError LinkedList_RemoveHead(LinkedList_t* linkedList, Node_t** node);

/* TODO tested finish all code paths */
/* should implement this in terms of remove */
DivXError LinkedList_RemoveTail(LinkedList_t* linkedList, Node_t** node);

/* TODO tested finish all code paths */
/* should implement this in terms of remove */
DivXError LinkedList_RemoveBefore(LinkedList_t* linkedList, Node_t* refNode, Node_t** node);

/* TODO tested finish all code paths */
/* should implement this in terms of remove */
DivXError LinkedList_RemoveAfter(LinkedList_t* linkedList, Node_t* refNode, Node_t** node);

/* TODO tested finish all code paths tested implicitly*/
DivXError LinkedList_Remove(LinkedList_t* linkedList, Node_t* refNode, Node_t** node);

/* TODO tested finish all code paths */
DivXError LinkedList_RemoveNode(LinkedList_t* linkedList, uint32_t index, Node_t** node);

DivXError LinkedList_PeakHead(LinkedList_t* linkedList, Node_t** node);
DivXError LinkedList_PeakTail(LinkedList_t* linkedList, Node_t** node);
DivXError LinkedList_PeakNode(LinkedList_t* linkedList, uint32_t index, Node_t** node);

/*

   Definition for the linked list iterator

*/
/*! iterator for linked list class */
typedef struct _LinkedListIter_t
{
    Node_tptr     curNode;
    LinkedList_t* pLinkedList;
    DivXMem       hMem;
} LinkedListIter_t;

typedef LinkedListIter_t* LinkedListIterHandle;

/*! Initialize an iterator

    @param linkedList          (IN)    - Linked list handle
    @param linkedListIter     (OUT)    - Retrieved track entry object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Init(LinkedList_t* linkedList, LinkedListIterHandle linkedListIter);

/*! Allocate an iterator

    @param linkedList          (IN)    - DF3CachelessIndex_t structure
    @param hMem                (IN)    - Memory handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
LinkedListIterHandle LinkedListIter_Create(DivXMem hMem);

/*! Destroys an allocated iterator

    @param linkedList          (IN)    - Iterator handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Destroy(LinkedListIterHandle linkedListIter);

/*! Sets the iterator back to the beginning of the list

    @param linkedList          (IN)    - Linked list
    @param iter               (OUT)    - Iterator handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Begin(LinkedListIterHandle iter);

/*! Sets the iterator to the end of the list

    @param linkedList          (IN)    - Linked list
    @param iter               (OUT)    - Iterator handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_End(LinkedListIterHandle iter);

/*! Retrieves the current node

    @param iter             (IN)    - Iterator handle
    @param pNode           (OUT)    - Returns the current node

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Get(LinkedListIterHandle iter, Node_tptr* pNode);

/*! Increments the iterator

    @param handle              (IN)    - Iterator handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Increment(LinkedListIterHandle handle);

/*! Decrements the iterator

    @param handle              (IN)    - Iterator handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError LinkedListIter_Decrement(LinkedListIterHandle handle);

#ifdef __cplusplus
}
#endif

#endif /* _DIVXLINKEDLIST_H_ */
