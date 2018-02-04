#include "stdafx.h"

#include "math.h"

#include "folder.h"
#include "file_walker.h"
#include "vector.h"
#include "vector_sorted.h"
#include "vector_mini.h"
#include "search_ac.h"
#include "mmf.h"

#include "Reflex.h"
#include "ReflexDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SIGN_SIZE      (8192)

void _fastcall Logger(const char* const pszText);
void _fastcall Lister(const char* const pszText);

extern DWORD      dwKeepError = 0;

static MMF        _MF;

static BYTE*      _pBuf   = NULL;
static DWORD      _dwSize = 0;

static DWORD      _dwDetections   = 0;
static DWORD      _dwInfected     = 0;
static DWORD      _dwFailedToOpen = 0;
static DWORD      _dwScanned      = 0;

struct RF_ListLine
{
   int            _iNum;
   time_t         _TNow;
   char           _pszText[MAX_PATH + 1];
};

struct RF_Event
{
   int            _iID;
   char*          _pszName;
   int            _iLen;
};


static int           _iCurrent = 0;
static int           _iNextID  = 0;

static CReflexDlg*   _pThis = NULL;

/* ******************************************************************** **
** @@ Logger()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void _fastcall Logger(const char* const pszText)
{
   ::SetDlgItemText(GetActiveWindow(),IDC_STT_STATUS,pszText);
}

/* ******************************************************************** **
** @@ Lister()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void _fastcall Lister(const char* const pszText)
{
   if (!_pThis || !IsWindow(_pThis->m_hWnd))
   {
      return;
   }

   RF_ListLine*   pEntry = new RF_ListLine;

   if (!pEntry)
   {
      return;
   }

   memset(pEntry,0,sizeof(RF_ListLine));

   pEntry->_iNum = ++_iCurrent;

   time(&pEntry->_TNow);

   strcpy(pEntry->_pszText,pszText);

   if (_pThis->_Lister.Insert(pEntry) == -1)
   {
      delete pEntry;
      pEntry = NULL;
   }

   int   iCnt = _pThis->_Lister.Count();

   _pThis->m_List.SetItemCountEx(iCnt,LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
   _pThis->m_List.RedrawItems(0,iCnt);
}

/* ******************************************************************** **
** @@ CMP_Num()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int CMP_Num(const void** const pKey1,const void** const pKey2)
{
   RF_ListLine**     p1 = (RF_ListLine**)pKey1;
   RF_ListLine**     p2 = (RF_ListLine**)pKey2;

   return (*p1)->_iNum - (*p2)->_iNum;
}

/* ******************************************************************** **
** @@ CMP_ID()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int CMP_ID(const void** const pKey1,const void** const pKey2)
{
   RF_Event**     p1 = (RF_Event**)pKey1;
   RF_Event**     p2 = (RF_Event**)pKey2;

   return (*p1)->_iID - (*p2)->_iID;
}

/* ******************************************************************** **
** @@ ForEach()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

#pragma warning(disable: 4189)
void ForEach(const char* const pszFileName)
{
   char  pszText[MAX_PATH + 1];

   wsprintf(pszText,"Check: %s",pszFileName);
   Logger(pszText);

   if (!_MF.OpenReadOnly(pszFileName))
   {
      ++_dwFailedToOpen;
      return;
   }

   _pBuf   = _MF.Buffer();
   _dwSize = _MF.Size();

   char     pszTemp[MAX_PATH + 1];

   memset(pszTemp,0,sizeof(pszTemp));

   DWORD       dwStart = 0;

   bool        bFound  = false;

   RF_Event*   pFound  = NULL;

   while (true)
   {
      DWORD    dwFoundAt = 0;

      pFound = (RF_Event*)_pThis->_pACS->FindFirstLongest(_pBuf,_dwSize,dwStart,dwFoundAt);

      if (pFound)
      {
         ++_dwInfected;
         wsprintf(pszText,"*** Infected: %s <- %s",pszFileName,pFound->_pszName);
         Logger(pszText);
         Lister(pszText);
         break;
/*
         bFound = true;

         ++_dwDetections;

         int   iShift = pFound->_iLen;
      
         // Restart
         dwStart = dwFoundAt + max(1,iShift);  

         if (dwStart >= _dwSize)                    
         {
            break;
         }
*/
      }
      else
      {
         break;
      }
   }

   _MF.Close();

   ++_dwScanned;
   wsprintf(pszText,"OK: %s",pszFileName);
   Logger(pszText);
   Lister(pszText);

   _pBuf   = NULL;
   _dwSize = 0;
}

/* ******************************************************************** **
** @@ CReflexDlg::CReflexDlg()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  : Constructor
** ******************************************************************** */

CReflexDlg::CReflexDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CReflexDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CReflexDlg)
   m_Path = _T("");
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   memset(_pszPath,0,sizeof(_pszPath));

   _dwTotalEntries = 0;

   _pBuf   = NULL;
   _dwSize = 0;
}

/* ******************************************************************** **
** @@ CReflexDlg::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CReflexDlg)
   DDX_Control(pDX, IDC_LIST, m_List);
   DDX_Text(pDX, IDC_EDT_PATH, m_Path);
   DDV_MaxChars(pDX, m_Path, 256);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CReflexDlg::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CReflexDlg, CDialog)
   //{{AFX_MSG_MAP(CReflexDlg)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
   ON_BN_CLICKED(IDC_BTN_PATH, OnBtnPath)
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CReflexDlg::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

BOOL CReflexDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   _pThis = this;

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);       // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
   
   CenterWindow();
   
   m_List.InsertColumn(0,"##",  LVCFMT_LEFT, 50,0);
   m_List.InsertColumn(1,"Time",LVCFMT_LEFT,100,1);
   m_List.InsertColumn(2,"File",LVCFMT_LEFT,500,2);

   DWORD    dwStyle = m_List.GetExtendedStyle();

   dwStyle |= LVS_OWNERDATA | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_FLATSB | LVS_EX_GRIDLINES;
   
   m_List.SetExtendedStyle(dwStyle);

   _Lister.Resize(1024);
   _Lister.Delta(128);
   _Lister.SetSorter(CMP_Num);

   _pEventList.Resize(4096);
   _pEventList.Delta(1024);
   _pEventList.SetSorter(CMP_ID);

   _pACS = new AC_Search;

   if (!_pACS)
   {
      // Error !
      return TRUE;
   }

   GetDlgItem(IDOK)->EnableWindow(FALSE);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ******************************************************************** **
** @@ CReflexDlg::OnPaint()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CReflexDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

/* ******************************************************************** **
** @@ CReflexDlg::OnQueryDragIcon()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CReflexDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

/* ******************************************************************** **
** @@ CReflexDlg::OnGetdispinfoList()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO*      pDispInfo = (LV_DISPINFO*)pNMHDR;

   LV_ITEM*          pItem = &(pDispInfo)->item;

   int      iIndex = pItem->iItem;

   if (pItem->mask & LVIF_TEXT)
   {
      RF_ListLine*      pEntry = (RF_ListLine*)_Lister.At(iIndex);

      if (pEntry)
      {
         switch (pItem->iSubItem)
         {
            case 0: 
            {
               wsprintf(pItem->pszText,"%d",pEntry->_iNum);
               break;
            }
            case 1: 
            {
               struct tm*   TheTime;
               
               TheTime = localtime(&pEntry->_TNow);

               wsprintf(pItem->pszText,"%02d:%02d:%02d",TheTime->tm_hour,TheTime->tm_min,TheTime->tm_sec);
               break;
            }
            case 2: 
            {
               strcpy(pItem->pszText,pEntry->_pszText);
               break;
            }
         }
      }
   }

   *pResult = 0;
}

/* ******************************************************************** **
** @@ CReflexDlg::OnBtnPath()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnBtnPath() 
{
   DWORD    dwFlags =   BIF_DONTGOBELOWDOMAIN   |
                        BIF_EDITBOX             |
                        BIF_RETURNONLYFSDIRS    |
                        BIF_STATUSTEXT          |
                        BIF_VALIDATE;

   BrowseForFolder(this->m_hWnd,"Path to scan files",dwFlags,_pszPath);

   if (*_pszPath)
   {
      SetDlgItemText(IDC_EDT_PATH,_pszPath);
      UpdateData(TRUE);
   }
}

/* ******************************************************************** **
** @@ CReflexDlg::OnOK()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnOK() 
{
   _dwDetections   = 0;
   _dwInfected     = 0;
   _dwFailedToOpen = 0;
   _dwScanned      = 0;

   DWORD    dwCnt = _Lister.Count();

   if (dwCnt)
   {
      for (int ii = (int)(dwCnt - 1); ii >= 0; --ii)
      {
         RF_ListLine*   pEntry = (RF_ListLine*)_Lister.At(ii);

         if (pEntry)
         {
            _Lister.RemoveAt(ii);

            delete pEntry;
            pEntry = NULL;
         }
      }
   }

   UpdateData(TRUE);

   _iCurrent = 0; // Reset

   CWaitCursor    Waiter;

   SetDlgItemText(IDC_STT_STATUS,"");
   SetDlgItemText(IDC_EDT_DETECTIONS,"");
   SetDlgItemText(IDC_EDT_INFECTED,"");
   SetDlgItemText(IDC_EDT_FAILED,"");
   SetDlgItemText(IDC_EDT_SCANNED,"");
   SetDlgItemText(IDC_EDT_TIME,"");

   UpdateData(FALSE);

   time_t   TStart;
   time_t   TFinish;
   
   time(&TStart);

   GetDlgItemText(IDC_EDT_PATH,_pszPath,MAX_PATH);

   if (!_pszPath || !*_pszPath)
   {
      SetDlgItemText(IDC_STT_STATUS,"Err: Gimme a path to scan!");
      MessageBeep(MB_ICONEXCLAMATION);
      Waiter.Restore();
      return;
   }

   strcat(_pszPath,"\\*.*");

   char     pszText[MAX_PATH + 1];

   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(_pszPath,pszDrive,pszDir,pszFName,pszExt);


   char     pszSrchMask[MAX_PATH + 1];
   char     pszSrchPath[MAX_PATH + 1];

   strcpy(pszSrchMask,pszFName);
   strcat(pszSrchMask,pszExt);

   Walker      Visitor(ForEach,pszSrchMask,true);

   strcpy(pszSrchPath,pszDrive);
   strcat(pszSrchPath,pszDir);

   Visitor.Run(*pszSrchPath  ?  pszSrchPath  :  ".");
   
   sprintf(pszText,"%d",_dwDetections);
   SetDlgItemText(IDC_EDT_DETECTIONS,pszText);

   sprintf(pszText,"%d",_dwInfected);
   SetDlgItemText(IDC_EDT_INFECTED,pszText);

   sprintf(pszText,"%d",_dwFailedToOpen);
   SetDlgItemText(IDC_EDT_FAILED,pszText);

   sprintf(pszText,"%d",_dwScanned);
   SetDlgItemText(IDC_EDT_SCANNED,pszText);

   time(&TFinish);
   
   double   fElapsed = difftime(TFinish,TStart);

   double   fSeconds = floor(fmod(fElapsed,60.0));
   double   fMinutes = floor(fmod(fElapsed / 60.0,60.0));
   double   fHours   = floor(fElapsed / 3600.0);

   sprintf(pszText,"%02d:%02d:%02d",(int)fHours,(int)fMinutes,(int)fSeconds);
   SetDlgItemText(IDC_EDT_TIME,pszText);
      
   SetDlgItemText(IDC_STT_STATUS,"READY");

   Waiter.Restore();

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CReflexDlg::OnCancel()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnCancel() 
{
   Cleanup();
   CDialog::OnCancel();
}

/* ******************************************************************** **
** @@ CReflexDlg::OnDestroy()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnDestroy() 
{
   CDialog::OnDestroy();
   Cleanup();
}

/* ******************************************************************** **
** @@ CReflexDlg::Cleanup()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::Cleanup()
{
   DWORD    dwCnt = _Lister.Count();

   if (dwCnt)
   {
      for (int ii = (int)(dwCnt - 1); ii >= 0; --ii)
      {
         RF_ListLine*   pEntry = (RF_ListLine*)_Lister.At(ii);

         if (pEntry)
         {
            _Lister.RemoveAt(ii);

            delete pEntry;
            pEntry = NULL;
         }
      }
   }

   if (_pACS)
   {
      delete _pACS;
      _pACS = NULL;
   }

   dwCnt = _pEventList.Count();

   if (dwCnt)
   {
      for (int ii = (int)(dwCnt - 1); ii >= 0; --ii)
      {
         RF_Event*   pEvent = (RF_Event*)_pEventList.At(ii);

         if (pEvent)
         {
            _pEventList.RemoveAt(ii);

            if (pEvent->_pszName)
            {
               delete[] pEvent->_pszName;
               pEvent->_pszName = NULL;
            }

            delete pEvent;
            pEvent = NULL;
         }
      }
   }
}

/* ******************************************************************** **
** @@ CReflexDlg::OnBtnLoad()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::OnBtnLoad() 
{
   CWaitCursor    Waiter;

   time_t   TStart;
   time_t   TFinish;
   
   time(&TStart);

   PopulateDix();

   // Finalize
   if (!_pACS->Finalize())
   {
      // Error !
      delete _pACS;
      _pACS = NULL;
   }

   time(&TFinish);
   
   double   fElapsed = difftime(TFinish,TStart);

   double   fSeconds = floor(fmod(fElapsed,60.0));
   double   fMinutes = floor(fmod(fElapsed / 60.0,60.0));
   double   fHours   = floor(fElapsed / 3600.0);

   char     pszText[MAX_PATH + 1];

   sprintf(pszText,"%02d:%02d:%02d",(int)fHours,(int)fMinutes,(int)fSeconds);
   SetDlgItemText(IDC_EDT_LOADTIME,pszText);

   // Do Once
   GetDlgItem(IDC_BTN_LOAD)->EnableWindow(FALSE);

   GetDlgItem(IDOK)->EnableWindow(TRUE);
}

/* ******************************************************************** **
** @@ CReflexDlg::PopulateDix()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update : 
** @  Notes  :
** ******************************************************************** */

void CReflexDlg::PopulateDix()
{
   UpdateData(TRUE);

   FILE*    pIn = fopen("reflex.db","rt");

   if (!pIn)
   {
      // Error
      SetDlgItemText(IDC_STT_STATUS,"Err: Can't open signature file");
      MessageBeep(MB_ICONEXCLAMATION);
      return;
   }

   const DWORD    BUF_SIZE = (0x01 << 14);   // About 16 Kb !

   char     pBuf[BUF_SIZE];

   int   iTotal  = 0;
   int   iReject = 0;

   while (fgets(pBuf,BUF_SIZE,pIn))
   {
      pBuf[BUF_SIZE - 1] = 0; // ASCIIZ !

      DWORD    dwEOL = strcspn(pBuf,"\r\n");

      pBuf[dwEOL] = 0;  // Remove EOL chars

      char*    pFound = strchr(pBuf,'=');

      if (pFound)
      {
         *pFound = 0; // Cut here   
      }

      int      iLen = strlen(pBuf);

      RF_Event*   pEvent = new RF_Event;

      if (!pEvent)
      {
         // Error!
         break;
      }

      pEvent->_pszName = new char[iLen + 1];

      if (!pEvent->_pszName)
      {
         // Error!
         break;
      }

      pEvent->_iID = ++_iNextID;

      strncpy(pEvent->_pszName,pBuf,iLen);
      pEvent->_pszName[iLen] = 0; // ASCIIZ

      BYTE     pSignature[(MAX_SIGN_SIZE + 1) >> 1];  // Half of Text representation
      int      iSignSize = 0;

      memset(pSignature,0,sizeof(pSignature));
      
      ++pFound; // Advance

      iLen = strlen(pFound);

      if (_pEventList.Insert(pEvent) == -1)
      {
         delete[] pEvent->_pszName;
         pEvent->_pszName = NULL;

         delete pEvent;
         pEvent = NULL;
      }

      if (iLen < 2)
      {
         // Too short to be valid!
         break;
      }

      // Convert Text-2-Bin
      for (int ii = 0; ii < iLen; ii += 2)
      {
         int     Lo = toupper(pFound[ii + 1]);
         int     Hi = toupper(pFound[ii]);

         int   iSignLo = 0;

         if ((Lo >= '0') && (Lo <= '9'))
         {
            iSignLo = Lo - '0';
         }
         else if ((Lo >= 'A') && (Lo <= 'F'))
         {
            iSignLo = Lo - 'A' + 0x0A;
         }
         else
         {
            // Error !
            SetDlgItemText(IDC_STT_STATUS,"Err: Invalid signature character");
            MessageBeep(MB_ICONEXCLAMATION);
            break;
         }

         int   iSignHi = 0;

         if ((Hi >= '0') && (Hi <= '9'))
         {
            iSignHi = Hi - '0';
         }
         else if ((Hi >= 'A') && (Hi <= 'F'))
         {
            iSignHi = Hi - 'A' + 0x0A;
         }
         else
         {
            // Error !
            SetDlgItemText(IDC_STT_STATUS,"Err: Invalid signature character");
            MessageBeep(MB_ICONEXCLAMATION);
            break;
         }

         pSignature[iSignSize++] = (BYTE)(iSignLo | (iSignHi << 4));
      }

      // Note: Up to USHORT_MAX Item Size !
      if (!_pACS->Add(pSignature,(WORD)iSignSize,pEvent))
      {
         #ifdef _DEBUG
         printf("Err: Can't add [%s: %s] signature. Possible duplicate.\n",pEvent->_pszName,pFound);
         #endif

         ++iReject;
         continue;
      }

      ++iTotal;
   }

   fclose(pIn);
   pIn = NULL;

   char     pszText[MAX_PATH + 1];

   sprintf(pszText,"%d",iTotal + iReject);
   SetDlgItemText(IDC_EDT_TOTAL,pszText);
  
   sprintf(pszText,"%d",iTotal);
   SetDlgItemText(IDC_EDT_LOADED,pszText);
  
   sprintf(pszText,"%d",iReject);
   SetDlgItemText(IDC_EDT_REJECTED,pszText);
  
   UpdateData(FALSE);
}
