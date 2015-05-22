/*!
    @file
@verbatim
$Id: Object.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _OBJECT_H_
#define _OBJECT_H_


#include <stddef.h>  /* for size_t, ptrdiff_t and NULL */
#include <string.h>  /* for memcpy prototype */
#include <stdlib.h>  /* for malloc/free prototypes */
 
/**
 * Abstract base class 'Object'
 */
typedef struct Object *Object;
typedef struct ObjectClass *ObjectClass;
struct ObjectClass {
   size_t __size;       /* size of the Object instance */
   ObjectClass __super; /* superclass descriptor pointer */
   void (*Des)(Object);
};
extern struct ObjectClass __Object;
struct Object {
   ObjectClass __vptr; /* private virtual pointer */
}; 

/* protected constructor and destructor as macros */
#define Object_Con(this) {(this)->__vptr=&__Object; }
#define Object_Des(this) {}

/* Test for type compatibility, use through macro only */
int Object__IsKindOf(Object this, void *class);

/* Handling objects on the heap, use through macros only */
Object Object__AllocArr(size_t size, short dim);
void Object__DeleteArr(Object obj);
 
/* Dummy implementation for abstract methods */
void Object_NoIm(void);
  
/**
 * Macros for declaring classes
 */
#define CLASS(CX, SX)\
   typedef struct CX *CX;\
   struct CX {struct SX super;
#define IMPLEMENTS(IX) struct IX *IX
#define VTABLE(CX, SX) };\
   extern struct CX##Class __##CX;\
   typedef struct CX##Class *CX##Class;\
   struct CX##Class {struct SX##Class super;
#define METHODS };
#define END_CLASS

/**
 * Macros for declaring interfaces
 */
#define OBJ_BEGIN_INTERFACE(IX) typedef struct IX **IX;\
   struct IX {ptrdiff_t __offset;
#define OBJ_END_INTERFACE };
#define EXTENDS(IX) struct IX IX

/**
 * Macros for defining VTABLEs and binding virtual functions

 */
#define BEGIN_VTABLE(CX, SX) struct CX##Class __##CX;\
   static void CX##ClassCon(CX t){CX##Class vptr=&__##CX;\
   memcpy(vptr,((Object)t)->__vptr,sizeof(struct SX##Class));\
   ((ObjectClass)vptr)->__size=sizeof(struct CX);
#define VMETHOD(CX, MX) ((CX##Class)vptr)->MX
#define IMETHOD(IX, MX)\
   if (vptr->IX.__offset==0)\
      vptr->IX.__offset=(char*)&t->IX-(char*)t;\
   vptr->IX.MX
#define END_VTABLE\
   ((ObjectClass)vptr)->__super=((Object)t)->__vptr;}

/**
 * Macros needed in constructors
 */
#define VHOOK(OX, CX)\
   {if(((ObjectClass)&__##CX)->__size==0) CX##ClassCon(OX);\
   ((Object)(OX))->__vptr=(void*)&__##CX;}
#define IHOOK(OX, CX, IX) (OX)->IX=&__##CX.IX
 
/**
 * Macro for invoking virtual class methods
 * and macro for invoking interface methods.
 */
#define VCALL(OX, CX, MX)\
   (*((CX##Class)(((Object)(OX))->__vptr))->MX)((CX)(OX)
#define ICALL(IPX, MX) (*(*(IPX))->MX)(I_TO_OBJ(IPX)
#define END_CALL )

/**
 * Macros for handling objects on the heap
 */
#define ALLOC(CX)  (CX)malloc(sizeof(struct CX))
#define OBJ_DELETE(OX)\
   {VCALL(OX, Object, Des)END_CALL; free((void*)(OX));}
#define ALLOC_ARR(CX,DX)\
   (CX)Object__AllocArr(sizeof(struct CX), DX)
#define DELETE_ARR(OX)  Object__DeleteArr((Object)(OX))

/**
 * Macros for RTTI (Run-Time Type Information)
 */
#define IS_RUNTIME_CLASS(OX, CX)\
   Object__IsKindOf((Object)(OX),(void*)&__##CX)
#define I_TO_OBJ(IPX) (Object)((char*)(IPX)-(*(IPX))->__offset)

#endif