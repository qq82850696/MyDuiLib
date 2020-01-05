// ChatSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ChatSocket.h"
#include "mainwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatSocket

CChatSocket::CChatSocket()
{
}

CChatSocket::CChatSocket(CMainWindow* pDlg)
{
	m_pDlg=pDlg;
	
}
CChatSocket::~CChatSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CChatSocket, CSocket)
	//{{AFX_MSG_MAP(CChatSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CChatSocket member functions

void CChatSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CSocket::OnReceive(nErrorCode);
	m_pDlg->OnReceive(this);
}

BOOL CChatSocket::MyGetSockName(CDuiString& rSocketAddress, UINT& rSocketPort)
{
	CString address;
	BOOL b = this->GetSockName(address, rSocketPort);
	rSocketAddress = address.GetBuffer();
	return b;
}
BOOL CChatSocket::MyGetPeerName(CDuiString& rSocketAddress, UINT& rSocketPort)
{
	CString address;
	BOOL b = this->GetPeerName(address, rSocketPort);
	rSocketAddress = address.GetBuffer();
	return b;
}