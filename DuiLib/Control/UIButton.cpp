#include "stdafx.h"
#include "UIButton.h"

namespace DuiLib
{
	REGIST_DUICONTROL(CButtonUI)
	CButtonUI::CButtonUI()
		: CLabelUI(), m_uButtonState(0)
		, m_dwHotTextColor(0)
		, m_dwPushedTextColor(0)
		, m_dwFocusedTextColor(0)
		, m_dwHotBkColor(0)
		, m_uFadeAlphaDelta(0)
		, m_uFadeAlpha(255)
	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	LPCTSTR CButtonUI::GetClass() const
	{
		return DUI_CTR_BUTTON;
	}

	LPVOID CButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_BUTTON) == 0 ) return static_cast<CButtonUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CButtonUI::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void CButtonUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			if (IsKeyboardEnabled() && IsEnabled()) {
				if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN)
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();

				if (!m_sSoundNameDown.IsEmpty()) {
					CPaintManagerUI::PlaySound(m_sSoundNameDown.GetData());
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_RBUTTONUP)
		{		
			if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_RCLICK, event.wParam, event.lParam);
			}
		}
		if (event.Type == UIEVENT_DBLCLICK)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Activate();
				Invalidate();
				m_pManager->SendNotify(this, DUI_MSGTYPE_DBCLICK, event.wParam, event.lParam);
				if (!m_sSoundNameDown.IsEmpty()) {
					CPaintManagerUI::PlaySound(m_sSoundNameDown.GetData());
				}
			}
		}
		if (event.Type == UIEVENT_RDBLCLICK)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_RDBCLICK, event.wParam, event.lParam);
			}
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() && IsEnabled()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
            if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
                if( IsEnabled() ) {
                    if( (m_uButtonState & UISTATE_HOT) == 0  ) {
                        m_uButtonState |= UISTATE_HOT;
                        Invalidate();
                    }

					if (!m_sSoundNameOver.IsEmpty()) {
						CPaintManagerUI::PlaySound(m_sSoundNameOver.GetData());
					}
                }
            }
			if ( GetFadeAlphaDelta() > 0 ) {
				m_pManager->SetTimer(this, FADE_TIMERID, FADE_ELLAPSE);
			}
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
            if( !::PtInRect(&m_rcItem, event.ptMouse ) ) {
                if( IsEnabled() ) {
                    if( (m_uButtonState & UISTATE_HOT) != 0  ) {
                        m_uButtonState &= ~UISTATE_HOT;
                        Invalidate();
                    }
                }
                if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
                if ( GetFadeAlphaDelta() > 0 ) {
                    m_pManager->SetTimer(this, FADE_TIMERID, FADE_ELLAPSE);
                }
            }
            else {
                if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
                return;
            }
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		if( event.Type == UIEVENT_TIMER  && event.wParam == FADE_TIMERID ) 
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				if( m_uFadeAlpha > m_uFadeAlphaDelta ) m_uFadeAlpha -= m_uFadeAlphaDelta;
				else {
					m_uFadeAlpha = 0;
					m_pManager->KillTimer(this, FADE_TIMERID);
				}
			}
			else {
				if( m_uFadeAlpha < 255-m_uFadeAlphaDelta ) m_uFadeAlpha += m_uFadeAlphaDelta;
				else {
					m_uFadeAlpha = 255;
					m_pManager->KillTimer(this, FADE_TIMERID);
				}
			}
			Invalidate();
			return;
		}
		CLabelUI::DoEvent(event);
	}

	bool CButtonUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return true;
	}

	void CButtonUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	void CButtonUI::SetHotBkColor( DWORD dwColor )
	{
		m_dwHotBkColor = dwColor;
	}

	DWORD CButtonUI::GetHotBkColor() const
	{
		return m_dwHotBkColor;
	}

	void CButtonUI::SetHotTextColor(DWORD dwColor)
	{
		m_dwHotTextColor = dwColor;
	}

	DWORD CButtonUI::GetHotTextColor() const
	{
		return m_dwHotTextColor;
	}

	void CButtonUI::SetPushedTextColor(DWORD dwColor)
	{
		m_dwPushedTextColor = dwColor;
	}

	DWORD CButtonUI::GetPushedTextColor() const
	{
		return m_dwPushedTextColor;
	}

	void CButtonUI::SetFocusedTextColor(DWORD dwColor)
	{
		m_dwFocusedTextColor = dwColor;
	}

	DWORD CButtonUI::GetFocusedTextColor() const
	{
		return m_dwFocusedTextColor;
	}

	LPCTSTR CButtonUI::GetNormalImage()
	{
		return m_diNormal.sDrawString;
	}

	void CButtonUI::SetNormalImage(LPCTSTR pStrImage)
	{
		if( m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL ) return;
		m_diNormal.Clear();
		m_diNormal.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetHotImage()
	{
		return m_diHot.sDrawString;
	}

	void CButtonUI::SetHotImage(LPCTSTR pStrImage)
	{
		if( m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL ) return;
		m_diHot.Clear();
		m_diHot.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetPushedImage()
	{
		return m_diPushed.sDrawString;
	}

	void CButtonUI::SetPushedImage(LPCTSTR pStrImage)
	{
		if( m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL ) return;
		m_diPushed.Clear();
		m_diPushed.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetFocusedImage()
	{
		return m_diFocused.sDrawString;
	}

	void CButtonUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		if( m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL ) return;
		m_diFocused.Clear();
		m_diFocused.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetDisabledImage()
	{
		return m_diDisabled.sDrawString;
	}

	void CButtonUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		if( m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL ) return;
		m_diDisabled.Clear();
		m_diDisabled.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetForeImage()
	{
		return m_diFore.sDrawString;
	}

	void CButtonUI::SetForeImage( LPCTSTR pStrImage )
	{
		if( m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL ) return;
		m_diFore.Clear();
		m_diFore.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetHotForeImage()
	{
		return m_diHotFore.sDrawString;
	}

	void CButtonUI::SetHotForeImage( LPCTSTR pStrImage )
	{
		if( m_diHotFore.sDrawString == pStrImage && m_diHotFore.pImageInfo != NULL ) return;
		m_diHotFore.Clear();
		m_diHotFore.sDrawString = pStrImage;
		Invalidate();
	}

	void CButtonUI::SetFiveStatusImage(LPCTSTR pStrImage)
	{
		m_fiveStatueImage = pStrImage;
		m_diNormal.Clear();
		m_diNormal.sDrawString = pStrImage;
		DrawImage(NULL, m_diNormal);
		if (m_diNormal.pImageInfo) {
			LONG width = m_diNormal.pImageInfo->nX / 5;
			LONG height = m_diNormal.pImageInfo->nY;
			m_diNormal.rcBmpPart = CDuiRect(0, 0, width, height);
			if( m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0 ) {
				m_cxyFixed.cx = width;
				m_cxyFixed.cy = height;
			}
		}

		m_diPushed.Clear();
		m_diPushed.sDrawString = pStrImage;
		DrawImage(NULL, m_diPushed);
		if (m_diPushed.pImageInfo) {
			LONG width = m_diPushed.pImageInfo->nX / 5;
			LONG height = m_diPushed.pImageInfo->nY;
			m_diPushed.rcBmpPart = CDuiRect(width, 0, width*2, height);
		}

		m_diHot.Clear();
		m_diHot.sDrawString = pStrImage;
		DrawImage(NULL, m_diHot);
		if (m_diHot.pImageInfo) {
			LONG width = m_diHot.pImageInfo->nX / 5;
			LONG height = m_diHot.pImageInfo->nY;
			m_diHot.rcBmpPart = CDuiRect(width*2, 0, width*3, height);
		}

		m_diFocused.Clear();
		m_diFocused.sDrawString = pStrImage;
		DrawImage(NULL, m_diFocused);
		if (m_diFocused.pImageInfo) {
			LONG width = m_diFocused.pImageInfo->nX / 5;
			LONG height = m_diFocused.pImageInfo->nY;
			m_diFocused.rcBmpPart = CDuiRect(width*3, 0, width*4, height);
		}

		m_diDisabled.Clear();
		m_diDisabled.sDrawString = pStrImage;
		DrawImage(NULL, m_diDisabled);
		if (m_diDisabled.pImageInfo) {
			LONG width = m_diDisabled.pImageInfo->nX / 5;
			LONG height = m_diDisabled.pImageInfo->nY;
			m_diDisabled.rcBmpPart = CDuiRect(width*4, 0, width*5, height);
		}

		Invalidate();
	}

	LPCTSTR CButtonUI::GetFiveStatusImage()
	{
		return m_fiveStatueImage;
	}

	void CButtonUI::SetFadeAlphaDelta(BYTE uDelta)
	{
		m_uFadeAlphaDelta = uDelta;
	}

	BYTE CButtonUI::GetFadeAlphaDelta()
	{
		return m_uFadeAlphaDelta;
	}

	SIZE CButtonUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	bool CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("pushedimage")) == 0) { SetPushedImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("disabledimage")) == 0) { SetDisabledImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("hotforeimage")) == 0) { SetHotForeImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("fivestatusimage")) == 0) { SetFiveStatusImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("fadedelta")) == 0) { SetFadeAlphaDelta((BYTE)_ttoi(pstrValue)); return true; }
		else if( _tcscmp(pstrName, _T("hotbkcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotBkColor(clrColor);
			return true;
		}
		else if( _tcscmp(pstrName, _T("hottextcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotTextColor(clrColor);
			return true;
		}
		else if( _tcscmp(pstrName, _T("pushedtextcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetPushedTextColor(clrColor);
			return true;
		}
		else if( _tcscmp(pstrName, _T("focusedtextcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetFocusedTextColor(clrColor);
			return true;
		}
		else return CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CButtonUI::PaintText(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();

		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor, \
			NULL, NULL, nLinks, m_iFont, m_uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor, \
			m_iFont, m_uTextStyle);
	}

	void CButtonUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if (DrawImage(hDC, m_diDisabled)) goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if (!DrawImage(hDC, m_diPushed))
				DrawImage(hDC, m_diNormal);
			if (DrawImage(hDC, m_diPushedFore)) return;
			else goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( GetFadeAlphaDelta() > 0 ) {
				if( m_uFadeAlpha == 0 ) {
					m_diHot.uFade = 255;
					DrawImage(hDC, m_diHot);
				}
				else {
					m_diNormal.uFade = m_uFadeAlpha;
					DrawImage(hDC, m_diNormal);
					m_diHot.uFade = 255 - m_uFadeAlpha;
					DrawImage(hDC, m_diHot);
				}
			}
			else {
				if (!DrawImage(hDC, m_diHot))
					DrawImage(hDC, m_diNormal);
			}

			if (DrawImage(hDC, m_diHotFore)) return;
			else if(m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
				return;
			}
			else goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if (DrawImage(hDC, m_diFocused)) goto Label_ForeImage;;
		}

		if ( GetFadeAlphaDelta() > 0 ) {
			if( m_uFadeAlpha == 255 ) {
				m_diNormal.uFade = 255;
				DrawImage(hDC, m_diNormal);
			}
			else {
				m_diHot.uFade = 255 - m_uFadeAlpha;
				DrawImage(hDC, m_diHot);
				m_diNormal.uFade = m_uFadeAlpha;
				DrawImage(hDC, m_diNormal);
			}
		}
		else {
			DrawImage(hDC, m_diNormal);
		}

Label_ForeImage:
		DrawImage(hDC, m_diFore);
	}

	void CButtonUI::GetPropertyList(std::vector<UIPropertyGrid>& property_list)
	{
		__super::GetPropertyList(property_list);

		property_list.push_back(UIPropertyGrid("Button", "Button"));
		UIPropertyGrid& property = property_list.back();
		std::vector< UIPropertyGridItem >& items = property.items;
#define ARGB(a,r,g,b)        ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))

		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "NormalImage", "指定按钮正常显示时的图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "HotImage", "指定按钮获得热点时的图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "PushedImage", "指定按钮被按压下时的图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "FocusedImage", "指定按钮获得焦点后的图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "DisabledImage", "指定按钮被禁用后的图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "ForeImage", "指定按钮正常显示ForeImage", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "HotForeImage", "指定按钮获得热点时的Fore图片", _variant_t("")));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "FiveStatusImage", "五态图片", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Number, "FadeDelta", "渐变", _variant_t(0)));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "HotBkColor", "HotBkColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "HotTextColor", "HotTextColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "PushedTextColor", "PushedTextColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "FocusedTextColor", "FocusedTextColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));

#undef ARGB
	}
}