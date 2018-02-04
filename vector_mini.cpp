/* ******************************************************************** **
** @@ MiniVector
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "search.h"
#include "vector_mini.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

#define MINI_INDEX_OF      (64)     // Search Optimization
#define MINI_SIZE          (4)
#define MINI_DELTA         (4)

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ MiniVector::MiniVector()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Constructor
** ******************************************************************** */

MiniVector::MiniVector()
:  _pItems(NULL),_pSorter(NULL),_wCount(0),_wSize(0)
{
   if (max(0,MINI_SIZE))
   {  
      Resize(MINI_SIZE);
   }
}

/* ******************************************************************** **
** @@ MiniVector::MiniVector()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Constructor
** ******************************************************************** */

MiniVector::MiniVector(WORD wSize)
:  _pItems(NULL),_pSorter(NULL),_wCount(0),_wSize(0)
{
   if (max(0,wSize))
   {  
      Resize(wSize);
   }
}

/* ******************************************************************** **
** @@ MiniVector::~MiniVector()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Destructor
** ******************************************************************** */

MiniVector::~MiniVector()
{
   delete[] _pItems;
   _pItems = NULL;
}

/* ******************************************************************** **
** @@ MiniVector::Resize()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::Resize(WORD wSize)
{
   void**   pItems = NULL;

   wSize = max(0,wSize);

   if (wSize != _wSize)
   {
      if (wSize)
      {
         pItems = (void**)new char[wSize * sizeof(void*)];

         if (!pItems)
         {
            return false;
         }

         if (_wSize)
         {
            memcpy(pItems,_pItems,min(wSize,_wSize) * sizeof(void*));
         }
      }

      delete[] _pItems;
      _pItems = NULL;

      _pItems = pItems;
      _wSize  = wSize;
   }

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::At()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::At(WORD wIndex)
{
   return (wIndex >= _wCount)  ?  NULL  :  _pItems[wIndex];
}

/* ******************************************************************** **
** @@ MiniVector::PutAt()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::PutAt
(
   WORD                wIndex,
   const void* const    pItem
)
{
   if (wIndex >= _wSize)
   {
      return false;
   }

   _pItems[wIndex] = (void**)pItem;

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::InsertAt()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::InsertAt
(
   WORD                 wIndex,
   const void* const    pItem
)
{
   if (wIndex > _wSize)
   {
      return false;
   }

   if (_wCount == _wSize)
   {
      if (!Resize(_wSize + MINI_DELTA))
      {
         return false;
      }
   }

   if (_wCount > wIndex)
   {
      int   iLen = (_wCount - wIndex) * (int)sizeof(void*);

      memmove(&_pItems[wIndex + 1],&_pItems[wIndex],(WORD)iLen);
   }

   ++_wCount;

   _pItems[wIndex] = (void**)pItem;

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::RemoveAt()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::RemoveAt(WORD wIndex)
{
   if (wIndex >= _wCount)
   {
      return false;
   }

   --_wCount;

   memmove(&_pItems[wIndex],&_pItems[wIndex + 1],(_wCount - wIndex) * sizeof(void*));

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::Remove()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::Remove(const void* const pItem)
{
   int   iIndex = Indexof(pItem);

   if (iIndex == -1)
   {
      return false;
   }

   RemoveAt(iIndex);

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::Indexof()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : Linear Search
** ******************************************************************** */

int MiniVector::Indexof(const void* const pItem)
{
   for (DWORD ii = 0; ii < _wCount; ++ii)
   {
      if (pItem == _pItems[ii])
      {
         return (int)ii;
      }
   }

   return -1;
}

/* ******************************************************************** **
** @@ MiniVector::Insert()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

int MiniVector::Insert(const void* const pItem)
{
   return InsertAt(_wCount,pItem)  ?  _wCount - 1  :  -1;
}

/* ******************************************************************** **
** @@ MiniVector::Search()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Search
(
   const void* const    pKey,
   short*               pIndex,
   MINI_SEARCHER        pSearch
)
{
   if (!_pSorter)
   {
      return NULL;
   }

   for (DWORD ii = 0; ii < _wCount; ++ii)
   {
      int   iResult = 0;

      if (pSearch)
      {
         iResult = pSearch((const void** const)&_pItems[ii],(const void** const)&pKey);
      }
      else
      {
         iResult = _pSorter((const void** const)&_pItems[ii],(const void** const)&pKey);
      }

      if (!iResult)     // Found
      {
         if (pIndex)
         {
            *pIndex = (short)ii;
         }

         return _pItems[ii];
      }
   }

   if (pIndex)
   {
      *pIndex = -1;
   }

   return NULL;
}

/* ******************************************************************** **
** @@ MiniVector::First()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::First()
{
   return At(0);
}

/* ******************************************************************** **
** @@ MiniVector::Last()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Last()
{
   return At(_wCount - 1);
}

/* ******************************************************************** **
** @@ MiniVector::Prev()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Prev(const void* const pItem)
{
   short    iIndex = Indexof(pItem);

   return iIndex == -1  ?  NULL  :  At(iIndex - 1);
}

/* ******************************************************************** **
** @@ MiniVector::Next()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Next(const void* const pItem)
{
   short    iIndex = Indexof(pItem);

   return iIndex == -1  ?  NULL  :  At(iIndex + 1);
}

/* ******************************************************************** **
** @@ MiniVector::Push()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void MiniVector::Push(const void* const pItem)
{
   Insert(pItem);
}

/* ******************************************************************** **
** @@ MiniVector::Pop()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Pop()
{
   void*    pItem = At(_wCount - 1);

   RemoveAt(_wCount - 1);

   return pItem;
}

/* ******************************************************************** **
** @@ MiniVector::Peek()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 06 Jul 2001
** @  Notes  :
** ******************************************************************** */

void* MiniVector::Peek()
{
   return At(_wCount - 1);
}

/* ******************************************************************** **
** @@ MiniVector::Drop()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

bool MiniVector::Drop(int iItems)
{
   for (DWORD ii = (_wCount - 1); ((ii > 0) && (iItems > 0)); --iItems, --ii)
   {
      if (!RemoveAt((WORD)ii))
      {
         return false;
      }
   }

   return true;
}

/* ******************************************************************** **
** @@ MiniVector::LSearch()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::LSearch
(
   const void* const    pKey,
   int*                 pIndex,
   MINI_SEARCHER        pSearch
)
{
   if (pIndex)
   {
      *pIndex = -1;
   }

   if (!_pSorter && !pSearch)
   {
      return NULL;
   }
      
   void*    pItem = NULL;

   unsigned int   uiCnt = (unsigned int)_wCount;

   if (pSearch)
   {
      pItem = _lfind(pKey,_pItems,&uiCnt,sizeof(void*),(int (__cdecl*)(const void*,const void*))pSearch);
   }
   else
   {
      pItem = _lfind(pKey,_pItems,&uiCnt,sizeof(void*),(int (__cdecl*)(const void*,const void*))_pSorter);
   }

   if (pItem && pIndex)
   {
      for (DWORD ii = 0; ii < _wCount; ++ii)
      {
         if (pItem == _pItems[ii])
         {
            *pIndex = (int)ii;
         }
      }
   }

   return pItem;
}

/* ******************************************************************** **
** @@ MiniVector::BSearch()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::BSearch
(
   const void* const    pKey,
   int*                 pIndex,
   MINI_SEARCHER        pSearch
)
{
   if (pIndex)
   {
      *pIndex = -1;
   }

   if (!_pSorter && !pSearch)
   {
      return NULL;
   }

   void*    pItem = NULL;

   if (pSearch)
   {
      pItem = bsearch(pKey,_pItems,(DWORD)_wCount,sizeof(void*),(int (__cdecl*)(const void*,const void*))pSearch);
   }
   else
   {
      pItem = bsearch(pKey,_pItems,(DWORD)_wCount,sizeof(void*),(int (__cdecl*)(const void*,const void*))_pSorter);
   }

   if (pItem && pIndex)
   {
      for (DWORD ii = 0; ii < _wCount; ++ii)
      {
         if (pItem == _pItems[ii])
         {
            *pIndex = (int)ii;
         }
      }
   }

   return pItem;
}

/* ******************************************************************** **
** @@ MiniVector::Count()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

WORD MiniVector::Count() 
{ 
   return _wCount; 
}

/* ******************************************************************** **
** @@ MiniVector::SetSorter()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void MiniVector::SetSorter(MINI_SORTER pSorter)
{
   _pSorter = pSorter;
}

/* ******************************************************************** **
** @@ MiniVector::FirstThat()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

#pragma warning(disable: 4100)   // unreferenced formal parameter
void* MiniVector::FirstThat(MINI_TEST pTest,const void* const pArg)
{
   // TODO !
   return NULL;
}

/* ******************************************************************** **
** @@ MiniVector::LastThat()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void* MiniVector::LastThat(MINI_TEST pTest,const void* const pArg)
{
   // TODO !
   return NULL;
}

/* ******************************************************************** **
** @@ MiniVector::ForEach()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void MiniVector::ForEach(MINI_APP pAction,const void* const pArg)
{
   // TODO !
}

/* ******************************************************************** **
** @@ MiniVector::Exchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void MiniVector::Exchange(WORD wFirst,WORD wSecond)
{
   if ((wFirst >= _wCount) || (wSecond >= _wCount))
   {
      return;
   }

   void*    p1 = _pItems[wFirst];
   void*    p2 = _pItems[wSecond];

   _pItems[wFirst]  = p2;
   _pItems[wSecond] = p1;
}

/* ******************************************************************** **
**                End of File
** ******************************************************************** */
