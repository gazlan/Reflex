/* ******************************************************************** **
** @@ MiniVector
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Dscr   :
** ******************************************************************** */

#ifndef _VECTOR_MINI_HPP_
#define _VECTOR_MINI_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

typedef  int (__fastcall *MINI_SORTER)  (const void** const pKey1,const void** const pKey2);
typedef  int (__fastcall *MINI_SEARCHER)(const void** const pKey1,const void** const pKey2);

typedef bool (*MINI_TEST)(const void* const pKey1,const void* const pKey2);
typedef void (*MINI_APP) (const void* const pKey1,const void* const pKey2);

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
** @@ class MiniVector
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

class MiniVector
{
   protected:

      void**               _pItems;
      MINI_SORTER          _pSorter;
      WORD                 _wCount;
      WORD                 _wSize;

   public:

               MiniVector();
               MiniVector(WORD wSize);
      virtual ~MiniVector();

      WORD     Count();
      bool     Resize(WORD dwSize);
      void     SetSorter(MINI_SORTER pSorter);

      void*    At       (WORD wIndex);
      bool     PutAt    (WORD wIndex,const void* const pItem);
      bool     InsertAt (WORD wIndex,const void* const pItem);
      bool     RemoveAt (WORD wIndex);
      void     Exchange (WORD wFirst,WORD wSecond);

      void*    FirstThat(MINI_TEST pTest,  const void* const pArg);
      void*    LastThat (MINI_TEST pTest,  const void* const pArg);
      void     ForEach  (MINI_APP  pAction,const void* const pArg);

      void*    LSearch(const void* const pKey,int* pIndex = NULL,MINI_SEARCHER pSearch = NULL);
      void*    BSearch(const void* const pKey,int* pIndex = NULL,MINI_SEARCHER pSearch = NULL);

      bool     Remove(const void* const pItem);

      virtual int    Indexof(const void* const pItem);
      virtual int    Insert (const void* const pItem);

      virtual void*  Search(const void* const pKey,short* pIndex = NULL,MINI_SEARCHER pSearch = NULL);

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
};

/* ******************************************************************** **
** @@                   Prototypes
** ******************************************************************** */

#endif

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
