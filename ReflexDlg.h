#ifndef  _REFLEXDLG_HPP_
#define  _REFLEXDLG_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReflexDlg : public CDialog
{
   private:

      DWORD                      _dwTotalEntries;

   public:

      AC_Search*                 _pACS;
      SortedVector               _Lister;
      SortedVector               _pEventList;
      char                       _pszPath[MAX_PATH + 1];

   // Construction
   public:
   
      CReflexDlg(CWnd* pParent = NULL);   // standard constructor

      // Dialog Data
      //{{AFX_DATA(CReflexDlg)
      enum { IDD = IDD_REFLEX_DIALOG };
      CListCtrl   m_List;
      CString     m_Path;
      //}}AFX_DATA

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CReflexDlg)
   protected:
      virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
   protected:

      HICON m_hIcon;

      // Generated message map functions
      //{{AFX_MSG(CReflexDlg)
      virtual BOOL OnInitDialog();
      afx_msg void OnPaint();
      afx_msg HCURSOR OnQueryDragIcon();
      afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void OnBtnPath();
      virtual void OnOK();
      virtual void OnCancel();
      afx_msg void OnDestroy();
      afx_msg void OnBtnLoad();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

   private:

      void PopulateDix();
      void Cleanup();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
