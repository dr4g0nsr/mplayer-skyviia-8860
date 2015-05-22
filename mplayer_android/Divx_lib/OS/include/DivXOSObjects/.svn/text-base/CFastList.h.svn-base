// $Id: CFastList.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CFASTLIST_H
#define CFASTLIST_H

#include <DivXTypes.h>

namespace DivXOS
{
	/*! @brief List object that allows very fast insert/remove/exists functionality. */
	class CFastList
	{
    public:

        /*! @note magicKey is the flag value used to mark an array index as empty! */
        CFastList(int maxSize = 128, int magicKey = 0) : maxSize(maxSize), magicKey(magicKey) 
        { 
            /*lint -e{1732} -e{1733} new allowed in this constructor... */
            listArray = new int[maxSize];
            
            int r;

            for(r=0;r<maxSize;r++)
            {
                listArray[r] = magicKey;
            }
        }

       ~CFastList() { delete[] listArray; }

        /*! insert value into list */
        bool insert(int value)
        {
            int r;

            for(r=0;r<maxSize;r++)
            { 
                if(listArray[r] == magicKey) 
                { 
                    listArray[r] = value;
                    return true;
                } 
            }

            return false;
        }

        /*! remove all instances of value from list */
        void remove(int value)
        {
            int r;

            for(r=0;r<maxSize;r++) 
            { 
                if(listArray[r] == value) 
                { 
                    listArray[r] = magicKey; 
                } 
            }
        }

        /*! remove one instance of value from list */
        bool remove_once(int value)
        {
            int r;

            for(r=0;r<maxSize;r++)
            {
                if(listArray[r] == value)
                {
                    listArray[r] = magicKey;
                    return true;
                }
            }

            return false;
        }

        /*! check if a value exists in list */
        bool exists(int value)
        {
            int r;

            for(r=0;r<maxSize;r++) 
            { 
                if(listArray[r] == value) 
                { 
                    return true; 
                } 
            }

            return false;
        }

    private:

        /*! actual array */
        int *listArray;

        /*! maximum size of array */
        int maxSize;

        /*! magic key used to mark an array index as empty */
        int magicKey;

	};
}

#endif
