#include "StdAfx.h"
#include "UIGifAnim.h"
#include "Utils/unzip.h"
#ifdef _USE_GDIPLUS
using namespace Gdiplus;
#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib
{
	REGIST_DUICONTROL(CGifAnimUI)
	CGifAnimUI::CGifAnimUI(void) : CControlUI()
	{
		m_pGifImage			=	NULL;
		m_pPropertyItem		=	NULL;
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
		m_bIsAutoPlay		=	true;
		m_bIsAutoSize		=	false;
		m_bIsPlaying		=	false;

	}


	CGifAnimUI::~CGifAnimUI(void)
	{
		DeleteGif();
		m_pManager->KillTimer( this, EVENT_TIEM_ID );

	}

	LPCTSTR CGifAnimUI::GetClass() const
	{
		return DUI_CTR_GIFANIM;
	}

	LPVOID CGifAnimUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_GIFANIM) == 0 ) return static_cast<CGifAnimUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CGifAnimUI::DoInit()
	{
		InitGifImage();
	}

	bool CGifAnimUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
	{
		if ( NULL == m_pGifImage )
		{		
			InitGifImage();
		}
		if (m_bIsAutoPlay && !m_bIsPlaying)
			PlayGif();

		DrawFrame( hDC );
        return true;
	}

	void CGifAnimUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_TIMER )
			OnTimer( (UINT_PTR)event.wParam );
	}

	void CGifAnimUI::SetVisible(bool bVisible /* = true */)
	{
		CControlUI::SetVisible(bVisible);
		if (bVisible)
			PlayGif();
		else
			StopGif();
	}

	bool CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("autoplay")) == 0 ) {
			SetAutoPlay(_tcscmp(pstrValue, _T("true")) == 0);
			return true;
		}
		else if( _tcscmp(pstrName, _T("autosize")) == 0 ) {
			SetAutoSize(_tcscmp(pstrValue, _T("true")) == 0);
			return true;
		}
		else
			return CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CGifAnimUI::SetBkImage(LPCTSTR pStrImage)
	{
		LPCTSTR oldImage = GetBkImage();
		CControlUI::SetBkImage(pStrImage);

		if (oldImage == pStrImage || NULL == pStrImage) return;

		StopGif();
		DeleteGif();

		Invalidate();
	}

	void CGifAnimUI::SetAutoPlay(bool bIsAuto)
	{
		if (m_bIsAutoPlay != bIsAuto) return;
		m_bIsAutoPlay = bIsAuto;
		Invalidate();
	}

	bool CGifAnimUI::IsAutoPlay() const
	{
		return m_bIsAutoPlay;
	}

	void CGifAnimUI::SetAutoSize(bool bIsAuto)
	{
		if (m_bIsAutoSize != bIsAuto) return;
		m_bIsAutoSize = bIsAuto;
		Invalidate();
	}

	bool CGifAnimUI::IsAutoSize() const
	{
		return m_bIsAutoSize;
	}

	void CGifAnimUI::PlayGif()
	{
		if (m_bIsPlaying || m_pGifImage == NULL)
		{
			return;
		}

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );

		m_bIsPlaying = true;
	}

	void CGifAnimUI::PauseGif()
	{
		if (!m_bIsPlaying || m_pGifImage == NULL)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::StopGif()
	{
		if (!m_bIsPlaying)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		m_nFramePosition = 0;
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifAnimUI::InitGifImage()
	{
		if (!GetBkImage()) return;
		m_pGifImage = LoadGifFromFile(GetBkImage());
		if ( NULL == m_pGifImage ) return;
		UINT nCount	= 0;
		nCount	=	m_pGifImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		m_pGifImage->GetFrameDimensionsList( pDimensionIDs, nCount );
		m_nFrameCount	=	m_pGifImage->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	m_pGifImage->GetPropertyItemSize( PropertyTagFrameDelay );
		m_pPropertyItem	=	(PropertyItem*) malloc( nSize );
		m_pGifImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pPropertyItem );
		delete  pDimensionIDs;
		pDimensionIDs = NULL;

		if (m_bIsAutoSize)
		{
			SetFixedWidth(m_pGifImage->GetWidth());
			SetFixedHeight(m_pGifImage->GetHeight());
		}
		if (m_bIsAutoPlay && nSize > 0)
		{
			PlayGif();
		}
	}

	void CGifAnimUI::DeleteGif()
	{
		if ( m_pGifImage != NULL )
		{
			delete m_pGifImage;
			m_pGifImage = NULL;
		}

		if ( m_pPropertyItem != NULL )
		{
			free( m_pPropertyItem );
			m_pPropertyItem = NULL;
		}
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
	}

	void CGifAnimUI::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != EVENT_TIEM_ID )
			return;
		m_pManager->KillTimer( this, EVENT_TIEM_ID );
		this->Invalidate();

		m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
	}

	void CGifAnimUI::DrawFrame( HDC hDC )
	{
		if ( NULL == hDC || NULL == m_pGifImage ) return;
		GUID pageGuid = FrameDimensionTime;
		Graphics graphics( hDC );
		graphics.DrawImage( m_pGifImage, m_rcItem.left, m_rcItem.top, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top );
		m_pGifImage->SelectActiveFrame( &pageGuid, m_nFramePosition );
	}

	Image* CGifAnimUI::LoadGifFromFile(LPCTSTR pstrGifPath)
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			CResourceManager::Instance()->LoadAsset(pstrGifPath, NULL, &pData, &dwSize);

		} while (0);

		if (!pData)
		{
			return NULL;
		}

		Image* pImage = LoadGifFromMemory(pData, dwSize);
		CResourceManager::Instance()->FreeAsset(pData);
		return pImage;
	}

	Image* CGifAnimUI::LoadGifFromMemory( LPVOID pBuf,size_t dwSize )
	{
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);

		memcpy(pMem, pBuf, dwSize);

		IStream* pStm = NULL;
		::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
		Image *pImg = Image::FromStream(pStm);
		if(!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
		{
			pStm->Release();
			::GlobalUnlock(hMem);
			return 0;
		}
		return pImg;
	}

	void CGifAnimUI::GetPropertyList(std::vector<UIPropertyGrid>& property_list)
	{
		CControlUI::GetPropertyList(property_list);

		property_list.push_back(UIPropertyGrid("GifAnim", "GifAnim"));
		UIPropertyGrid& property = property_list.back();
		std::vector< UIPropertyGridItem >& items = property.items;

		items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "AutoPlay", "自动播放", _variant_t(bool(true))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "AutoSize", "自动调整大小", _variant_t(bool(false))));
	}
}
