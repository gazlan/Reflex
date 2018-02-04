#include "stdafx.h"

#include <stdio.h>
#include <string.h>

#include "vector_mini.h"
#include "vector.h"
#include "search_ac.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AC_QUEUE_SIZE         (16384)
#define AC_QUEUE_DELTA        (4096)

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
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ AC_Sorter()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

static int __fastcall AC_Sorter(const void** const pKey1,const void** const pKey2)
{
   AC_Edge**   p1 = (AC_Edge**)pKey1;
   AC_Edge**   p2 = (AC_Edge**)pKey2;

   if ((*p1)->_byLabel > (*p2)->_byLabel)
   {
      return 1;
   }
   else if ((*p1)->_byLabel < (*p2)->_byLabel)
   {
      return -1;
   }
   else
   {
      return 0;
   }
}

/* ******************************************************************** **
** @@ AC_Edge::AC_Edge()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

AC_Edge::AC_Edge(const AC_Node* const pGoto,BYTE byLabel)
{
   _pGoto   = (AC_Node*)pGoto;
   _byLabel = byLabel;
}

/* ******************************************************************** **
** @@ AC_Edge::~AC_Edge()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Destructor
** ******************************************************************** */

AC_Edge::~AC_Edge()
{
   if (_pGoto)
   {
      delete _pGoto;
      _pGoto = NULL;
   }
}

/* ******************************************************************** **
** @@ AC_Node::AC_Node()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

AC_Node::AC_Node()
:  _pFail(NULL),_pData(NULL),_wPatLen(0)
{
   _Siblings.SetSorter(AC_Sorter);
}

/* ******************************************************************** **
** @@ AC_Node::AC_Node()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

AC_Node::AC_Node(const AC_Node* const pFail,const void* const pData,WORD wPatLen)
:  _pFail((AC_Node*)pFail),_pData((void*)pData),_wPatLen(wPatLen)
{
   _Siblings.SetSorter(AC_Sorter);
}

/* ******************************************************************** **
** @@ AC_Node::~AC_Node()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Destructor
** ******************************************************************** */

AC_Node::~AC_Node()
{
   int   iCount = _Siblings.Count();

   for (int ii = (iCount - 1); ii >= 0; --ii)
   {
      AC_Edge*    pEdge = (AC_Edge*)_Siblings.At(ii);

      _Siblings.RemoveAt(ii);

      if (pEdge)
      {
         delete pEdge;
         pEdge = NULL;
      }
   }
}

/* ******************************************************************** **
** @@ AC_Node::Link()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

bool AC_Node::Link(const AC_Node* const pGoto,BYTE byLabel)
{
   AC_Edge*    pEdge = new AC_Edge(pGoto,byLabel);

   if (!pEdge)
   {
      // Error !
      return false;
   }

   if (_Siblings.Insert(pEdge) == -1)
   {
      // Error !
      delete pEdge;
      pEdge = NULL;
      return false;
   }
   
   return true;
}

/* ******************************************************************** **
** @@ AC_Node::Link()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

bool AC_Node::LinkCase(const AC_Node* const pGoto,BYTE byLabel)
{
   // TODO !
   return false;
}

/* ******************************************************************** **
** @@ AC_Node::Follow()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

AC_Node* AC_Node::Follow(BYTE byLabel) 
{
   AC_Edge     Edge(NULL,byLabel);

   AC_Edge*    pFound = (AC_Edge*)_Siblings.Search(&Edge);

   if (pFound)
   {
      return pFound->_pGoto;
   }

   return NULL;
}

/* ******************************************************************** **
** @@ AC_Node::FollowCase()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

AC_Node* AC_Node::FollowCase(BYTE byLabel) 
{
   // TODO !
   return NULL;
}

/* ******************************************************************** **
** @@ AC_Search::AC_Search()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

AC_Search::AC_Search()
:  _pLast(NULL)
{
   // Create New ZeroState
   _pRoot = new AC_Node;

   if (!_pRoot)
   {
      // Error !
      return;
   }
}

/* ******************************************************************** **
** @@ AC_Search::~AC_Search()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Destructor
** ******************************************************************** */

AC_Search::~AC_Search()
{
   if (_pRoot) 
   {
      delete _pRoot;
      _pRoot = NULL;
   }
}

/* ******************************************************************** **
** @@ AC_Search::FindFirstShortest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void* AC_Search::FindFirstShortest
(
   BYTE*          pBuf,
   DWORD          dwSize,
   DWORD          dwStartPos,
   DWORD&         rFoundAt
)
{
   AC_Node*      pState = _pRoot;

   for (DWORD ii = dwStartPos; ii < dwSize; ++ii)
   {
      while (!pState->Follow(pBuf[ii])) 
      {
         pState = pState->_pFail;
      }

      pState = pState->Follow(pBuf[ii]);

      if (pState->_wPatLen) 
      {
         _pLast = pState;

         rFoundAt = ii - pState->_wPatLen + 1;
   
         return pState->_pData;
      }
   }

   rFoundAt = 0;

   return NULL;
}

/* ******************************************************************** **
** @@ AC_Search::FindFirstLongest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Similar to the FindFirstShortest function, but tries to return the longest match.
** ******************************************************************** */

void* AC_Search::FindFirstLongest
(
   BYTE*       pBuf,
   DWORD       dwSize,
   DWORD       dwStartPos,
   DWORD&      rFoundAt
)
{
   AC_Node*      pState = _pRoot;

   rFoundAt = 0;
   
   int   iNextPos = -1;

   AC_Node*   pFound = 0;

   for (DWORD ii = dwStartPos; ii < dwSize; ++ii)
   {
      if (!pState->Follow(pBuf[ii]) && (iNextPos != -1)) 
      {
         _pLast = pState;

         return pFound->_pData;
      }

      while (!pState->Follow(pBuf[ii]))
      {
         pState = pState->_pFail;
      }
      
      pState = pState->Follow(pBuf[ii]);
      
      if (pState->_wPatLen) 
      {
         pFound = pState;

         rFoundAt = ii - (pFound->_wPatLen - 1);
         iNextPos = ii + 1;
      }
   }

   // If we reach the end of the pBuf, we still have to double check if we had a longest match queued up. 
   if (iNextPos != -1) 
   {
      return pState->_pData;
   }
   
   return NULL;
}

/* ******************************************************************** **
** @@ AC_Search::FindAllShortest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void AC_Search::FindAllShortest
(
   BYTE*          pBuf,
   DWORD          dwSize,
   DWORD          dwStartPos,
   AS_Callback    pCallback,
   bool           bOverlaps
)
{
   AC_Node*      pStart = _pRoot;

   DWORD    dwFoundAt = 0;

   while (true)
   {
      void*    pFound = _FindFirstShortest(pStart,pBuf,dwSize,dwStartPos,dwFoundAt);

      if (!pFound)
      {
         return;
      }

      if (pCallback)
      {
         pCallback(pFound,dwFoundAt);
      }

      dwStartPos = dwFoundAt + 1;

      pStart = bOverlaps  ?  _pLast  :  _pRoot;
   }
}

/* ******************************************************************** **
** @@ AC_Search::FindAllShortest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void AC_Search::FindAllLongest
(
   BYTE*          pBuf,
   DWORD          dwSize,
   DWORD          dwStartPos,
   AS_Callback    pCallback,
   bool           bOverlaps
)
{
   AC_Node*      pStart = _pRoot;

   DWORD    dwFoundAt = 0;

   while (true)
   {
      void*    pFound = _FindFirstLongest(pStart,pBuf,dwSize,dwStartPos,dwFoundAt);

      if (!pFound)
      {
         return;
      }

      if (pCallback)
      {
         pCallback(pFound,dwFoundAt);
      }

      dwStartPos = dwFoundAt + 1;

      pStart = bOverlaps  ?  _pLast  :  _pRoot;
   }
}

/* ******************************************************************** **
** @@ AC_Search::Finalize()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

#pragma warning(disable: 4706)
bool AC_Search::Finalize()
{
   Vector*     pQueue = new Vector(AC_QUEUE_SIZE,AC_QUEUE_DELTA);

   if (!pQueue)
   {
      // Error !
      return false;
   }

   _pRoot->_pFail = _pRoot;

   for (int ii = 0; ii < ALPHABET_SIZE; ++ii)
   {
      AC_Node*   pNode = _pRoot->Follow((BYTE)ii);

      if (pNode)
      {
         // Construct Failure()
         if (pQueue->Enqueue(pNode) == -1)
         {
            delete pQueue;
            pQueue = NULL;
            return false;
         }

         pNode->_pFail = _pRoot;
      }
   }

   // Set Fail() for Depth > 0
   AC_Node*   r = NULL;

   while (r = (AC_Node*)pQueue->Dequeue())
   {
      for (int ii = 0; ii < ALPHABET_SIZE; ++ii)
      {
         AC_Node*   s = r->Follow((BYTE)ii);
                     
         if (!s)
         {
            continue;
         }

         if (pQueue->Enqueue(s) == -1)
         {
            delete pQueue;
            pQueue = NULL;
            return false;
         }

         AC_Node*   pState = r->_pFail;
         
         while (!pState->Follow((BYTE)ii))
         {
            pState = pState->_pFail;
         }  

         s->_pFail = pState->Follow((BYTE)ii);
      }
   }

   delete pQueue;
   pQueue = NULL;

   return true;
}

/* ******************************************************************** **
** @@ AC_Search::Add()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Short form for Non-text append
** ******************************************************************** */

bool AC_Search::Add
(
   const BYTE* const          pPattern,
   WORD                       wPatSize,
   const void* const          pData
)
{
   if (!wPatSize)
   {
      // Error !
      return false;
   }

   AC_Node*      pNext = _pRoot;
   AC_Node*      pGoto  = NULL;

   for (DWORD ii = 0; ii < wPatSize; ++ii)
   {
      pGoto = pNext->Follow(pPattern[ii]);

      if (!pGoto)
      {
         break;
      }
      
      pNext = pGoto;
   }

   if (ii == wPatSize) 
   {
      // Don't allow duplicates !
      if (!pGoto->_wPatLen)
      {
         pGoto->_wPatLen = wPatSize;
         pGoto->_pData   = (void*)pData;

         return true;
      }

      // Duplicated keys !
      return false;
   }

   // Process rest of the pattern 
   for ( ; ii < wPatSize; ++ii)
   {
      pGoto = new AC_Node;

      if (!pGoto)
      {
         // Error !
         return false;
      }

      // Create transition
      pNext->Link(pGoto,pPattern[ii]);
      
      pNext = pGoto;
   }

   pGoto->_wPatLen = wPatSize;
   pGoto->_pData   = (void*)pData;

   return true;
}

/* ******************************************************************** **
** @@ AC_Search::AddCase()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Long form for Text append
** ******************************************************************** */

bool AC_Search::AddCase
(
   const BYTE* const          pPattern,
   WORD                       wPatSize,
   const void* const          pData
)
{
   // TODO !
   return false;
}

/* ******************************************************************** **
** @@ AC_Search::_FindFirstShortest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Internal use only !
** ******************************************************************** */

void* AC_Search::_FindFirstShortest
(
   AC_Node*       pStart,
   BYTE*          pBuf,
   DWORD          dwSize,
   DWORD          dwStartPos,
   DWORD&         rFoundAt
)
{
   for (DWORD ii = dwStartPos; ii < dwSize; ++ii)
   {
      while (!pStart->Follow(pBuf[ii])) 
      {
         pStart = pStart->_pFail;
      }

      pStart = pStart->Follow(pBuf[ii]);

      if (pStart->_wPatLen) 
      {
         _pLast = pStart;

         rFoundAt = ii - (pStart->_wPatLen - 1);
   
         return pStart->_pData;
      }
   }

   rFoundAt = 0;

   return NULL;
}

/* ******************************************************************** **
** @@ AC_Search::_FindFirstLongest()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Internal use only !
** @  Notes  : Similar to the FindFirstShortest function, but tries to return the longest match.
** ******************************************************************** */

void* AC_Search::_FindFirstLongest
(
   AC_Node*       pStart,
   BYTE*          pBuf,
   DWORD          dwSize,
   DWORD          dwStartPos,
   DWORD&         rFoundAt
)
{
   rFoundAt = 0;
   
   int      iNextPos = -1;

   AC_Node*   pFound = NULL;

   for (DWORD ii = dwStartPos; ii < dwSize; ++ii)
   {
      if (!pStart->Follow(pBuf[ii]) && (iNextPos != -1)) 
      {
         _pLast = pStart;

         return pFound->_pData;
      }

      while (!pStart->Follow(pBuf[ii]))
      {
         pStart = pStart->_pFail;
      }
      
      pStart = pStart->Follow(pBuf[ii]);
      
      if (pStart->_wPatLen) 
      {
         pFound = pStart;

         rFoundAt = ii - (pFound->_wPatLen - 1);
         iNextPos = ii + 1;
      }
   }

   // If we reach the end of the pBuf, we still have to double check if we had a longest match queued up. 
   if (iNextPos != -1) 
   {
      return pStart->_pData;
   }
   
   return NULL;
}

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
