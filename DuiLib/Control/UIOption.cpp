#include "stdafx.h"
#include "UIOption.h"

namespace DuiLib
{
	REGIST_DUICONTROL(COptionUI)
	COptionUI::COptionUI() : CButtonUI(), m_bSelected(false), m_dwSelectedBkColor(0), m_dwSelectedTextColor(0)
	{
	}

	COptionUI::~COptionUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR COptionUI::GetClass() const
	{
		return DUI_CTR_OPTION;
	}

	LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<COptionUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CControlUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
	}

	LPCTSTR COptionUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void COptionUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool COptionUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionUI::Selected(bool bSelected, bool bTriggerEvent)
	{
		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CDuiPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false, bTriggerEvent);
						}
					}
					if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		if (m_bSelected)
			SetToolTip(GetSelectedToolTip());
		else
			SetToolTip(GetNormalTooltip());

		Invalidate();
	}

	bool COptionUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void COptionUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	LPCTSTR COptionUI::GetSelectedImage()
	{
		return m_diSelected.sDrawString;
	}

	void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
	{
		if( m_diSelected.sDrawString == pStrImage && m_diSelected.pImageInfo != NULL ) return;
		m_diSelected.Clear();
		m_diSelected.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedHotImage()
	{
		return m_diSelectedHot.sDrawString;
	}

	void COptionUI::SetSelectedHotImage( LPCTSTR pStrImage )
	{
		if( m_diSelectedHot.sDrawString == pStrImage && m_diSelectedHot.pImageInfo != NULL ) return;
		m_diSelectedHot.Clear();
		m_diSelectedHot.sDrawString = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedPushedImage()
	{
		return m_diSelectedPushed.sDrawString;
	}

	void COptionUI::SetSelectedPushedImage(LPCTSTR pStrImage)
	{
		if( m_diSelectedPushed.sDrawString == pStrImage && m_diSelectedPushed.pImageInfo != NULL ) return;
		m_diSelectedPushed.Clear();
		m_diSelectedPushed.sDrawString = pStrImage;
		Invalidate();
	}

	void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_dwSelectedTextColor = dwTextColor;
	}

	DWORD COptionUI::GetSelectedTextColor()
	{
		if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
		return m_dwSelectedTextColor;
	}

	void COptionUI::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD COptionUI::GetSelectedBkColor()
	{
		return m_dwSelectedBkColor;
	}

	DUI_DEPRECATED DWORD COptionUI::GetSelectBkColor()
	{
		return this->GetSelectedBkColor();
	}

	LPCTSTR COptionUI::GetForeImage()
	{
		return m_diFore.sDrawString;
	}

	void COptionUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL ) return;
		m_diFore.Clear();
		m_diFore.sDrawString = pStrImage;
		Invalidate();
	}
	//begin 2018/6/10
	void COptionUI::SetSelectedToolTip(LPCTSTR szTooltip)
	{
		m_sSelectedTooltip = szTooltip;
		Invalidate();
	}
	LPCTSTR COptionUI::GetSelectedToolTip()const
	{
		return m_sSelectedTooltip;
	}

	void COptionUI::SetNormalTooltip(LPCTSTR szTooltip)
	{
		m_sNormalTooltip = szTooltip;
		SetToolTip(szTooltip);
		Invalidate();
	}
	LPCTSTR COptionUI::GetNormalTooltip() const
	{
		return m_sNormalTooltip;
	}
	//end


	SIZE COptionUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	bool COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("group")) == 0) { SetGroup(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("selected")) == 0) { Selected(_tcscmp(pstrValue, _T("true")) == 0); return true; }
		else if (_tcscmp(pstrName, _T("selectedimage")) == 0) { SetSelectedImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("selectedhotimage")) == 0) { SetSelectedHotImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("selectedpushedimage")) == 0) { SetSelectedPushedImage(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("tooltip")) == 0) { SetNormalTooltip(pstrValue); return true; }
		else if (_tcscmp(pstrName, _T("selectedtooltip")) == 0) { SetSelectedToolTip(pstrValue); return true; }
		else if( _tcscmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
			return true;
		}
		else if( _tcscmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedTextColor(clrColor);
			return true;
		}
		else return CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void COptionUI::PaintStatusImage(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
			if((m_uButtonState & UISTATE_PUSHED) != 0)
			{
				if (DrawImage(hDC, m_diSelectedPushed)) goto Label_ForeImage;
			}
			if ((m_uButtonState & UISTATE_HOT) != 0)
			{
				if (DrawImage(hDC, m_diSelectedHot)) goto Label_ForeImage;
			}

			if( DrawImage(hDC, m_diSelected) ) goto Label_ForeImage;
			else if(m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
				goto Label_ForeImage;
			}	
		}

		UINT uSavedState = m_uButtonState;
		m_uButtonState &= ~UISTATE_PUSHED;
		CButtonUI::PaintStatusImage(hDC);
		m_uButtonState = uSavedState;

Label_ForeImage:
		DrawImage(hDC, m_diFore);
	}

	void COptionUI::PaintText(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				NULL, NULL, nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
		{
			UINT uSavedState = m_uButtonState;
			m_uButtonState &= ~UISTATE_PUSHED;
			CButtonUI::PaintText(hDC);
			m_uButtonState = uSavedState;
		}
	}

	void COptionUI::GetPropertyList(std::vector<UIPropertyGrid>& property_list)
	{
		__super::GetPropertyList(property_list);

		property_list.push_back(UIPropertyGrid("Option", "Option"));
		UIPropertyGrid& property = property_list.back();
		std::vector< UIPropertyGridItem >& items = property.items;
#define ARGB(a,r,g,b)        ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))
		items.push_back(UIPropertyGridItem(PropertyType::PT_String, "Group", "指定参与组合的名称"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Boolean, "Selected", "指示是否已被选中\nFalse", _variant_t(bool(false))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "SelectedImage", "指定复选框被选择后的图片"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "SelectedHotImage", "指定复选框被选择后的图片"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "SelectedPushdImage", "指定复选框被选择后的图片"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Image, "ForeImage", "指定复选框的前景图片"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_String, "SelectedTooltip", "指定复选框被选择后的提示文本"));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "SelectedBkColor", "SelectedBkColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
		items.push_back(UIPropertyGridItem(PropertyType::PT_Color, "SelectedTextColor", "SelectedTextColor", _variant_t((LONG)(ARGB(0, 0, 0, 0)))));
#undef ARGB
	}

}