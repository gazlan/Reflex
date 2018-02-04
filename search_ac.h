#ifndef _AHO_CORASICK_HPP_
#define _AHO_CORASICK_HPP_

#define ALPHABET_SIZE         (256)

typedef void (*AS_Callback)(void* pFound,DWORD dwFoundAt);

class AC_Node
{                          
   public:

      MiniVector           _Siblings;
      AC_Node*             _pFail;
      void*                _pData;
      WORD                 _wPatLen;

   public:

       AC_Node();
       AC_Node(const AC_Node* const pFail,const void* const pData,WORD wPatLen);
      ~AC_Node();

      bool        Link(const AC_Node* const pGoto,BYTE byLabel);
      bool        LinkCase(const AC_Node* const pGoto,BYTE byLabel);
      AC_Node*    Follow(BYTE byLabel);
      AC_Node*    FollowCase(BYTE byLabel);
};

// Represents a single labeled edge.
class AC_Edge 
{  
   public:

      AC_Node*             _pGoto;
      BYTE                 _byLabel;

   public:

       AC_Edge(const AC_Node* const pGoto,BYTE byLabel);
      ~AC_Edge();
};

typedef DWORD (*Searcher)(AC_Node* pState,BYTE* pBuf,DWORD dwSize,int iStartPos,int& riFoundAt,AC_Node** pLastState);

class AC_Search
{
   public:
   
      AC_Node*      _pRoot;
      AC_Node*      _pLast;

   public:
   
       AC_Search();
      ~AC_Search(); 

      bool     Add    (const BYTE* const pPattern,WORD wPatSize,const void* const pData);
      bool     AddCase(const BYTE* const pPattern,WORD wPatSize,const void* const pData);
      bool     Finalize();

      void*    FindFirstShortest(BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,DWORD& rFoundAt);
      void*    FindFirstLongest (BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,DWORD& rFoundAt);

      // NEVER use bOverlaps = true !!
      // Smth buggy in this implementation...
      // For repeated patterns (like 'aaa') it's lead to INFINITE LOOP !!
      void     FindAllShortest(BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,AS_Callback pCallback,bool bOverlaps = false);
      void     FindAllLongest (BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,AS_Callback pCallback,bool bOverlaps = false);

   private:
               
      inline void*   _FindFirstShortest(AC_Node* pState,BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,DWORD& rFoundAt);
      inline void*   _FindFirstLongest (AC_Node* pState,BYTE* pBuf,DWORD dwSize,DWORD dwStartPos,DWORD& rFoundAt);
};

#endif

/* ******************************************************************** **
** @@                   End of File
** ******************************************************************** */
