/* ******************************************************************** **
** @@ Vector Header File
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Dscr   :
** ******************************************************************** */

#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

typedef  int (__cdecl *SORTER)  (const void** const pKey1,const void** const pKey2);
typedef  int (__cdecl *SEARCHER)(const void** const pKey1,const void** const pKey2);

typedef bool (*TEST)(const void* const pKey1,const void* const pKey2);
typedef void (*APP) (const void* const pKey1,const void* const pKey2);

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   Classes
** ******************************************************************** */

/* ******************************************************************** **
** @@ class Vector
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

class Vector
{
   protected:

      void**         _pItems;
      DWORD          _dwCount;
      DWORD          _dwSize;
      SORTER         _pSorter;
      WORD           _wDelta;
      bool           _bGrow;
      bool           _bShrink;

   public:

               Vector();
               Vector(DWORD dwSize,WORD wDelta,bool bGrow = true,bool bShrink = false);
      virtual ~Vector();

      void     Attach(void** pBuf,DWORD dwCount,DWORD dwSize);
      void**   Detach(DWORD& rCount,DWORD& rSize);
      void**   GetData();

      DWORD    Count();
      DWORD    Size ();
      bool     Resize(DWORD dwSize);
      void     Delta (WORD wDelta);
      void     SetGrowMode  (bool bGrow);
      void     SetShrinkMode(bool bShrink);

      void*    At       (DWORD dwIndex);
      bool     PutAt    (DWORD dwIndex,const void* const pItem);
      bool     InsertAt (DWORD dwIndex,const void* const pItem);
      bool     RemoveAt (DWORD dwIndex);
      void     Exchange (DWORD dwFirst,DWORD dwSecond);

      void*    FirstThat(TEST pTest,  const void* const pArg);
      void*    LastThat (TEST pTest,  const void* const pArg);
      void     ForEach  (APP  pAction,const void* const pArg);

      void*    LSearch(const void* const pKey,int* pIndex = NULL,SEARCHER pSearch = NULL);
      void*    BSearch(const void* const pKey,int* pIndex = NULL,SEARCHER pSearch = NULL);

      void     SetSorter(SORTER pSorter);
      SORTER   GetSorter() const;

      void     QSort();

      bool     Remove(const void* const pItem);

      virtual int    Indexof(const void* const pItem);
      virtual int    Insert (const void* const pItem);

      virtual void*  Search(const void* const pKey,int* pIndex = NULL,SEARCHER pSearch = NULL);

      //  Linked list functions
      void*    First();
      void*    Last ();
      void*    Prev (const void* const pItem);
      void*    Next (const void* const pItem);

      //  Stack functions
      void     Push (const void* const pItem);
      void*    Pop ();
      void*    Peek();
      bool     Drop(int iItems);

      // Queue functions
      int      Enqueue(const void* const pItem);
      void*    Dequeue();
};

/* ******************************************************************** **
** @@                   Prototypes
** ******************************************************************** */

#endif

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
