#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include <comutil.h>

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString);
		STRINGorID(unsigned int nID);
		operator LPCTSTR() { return m_lpstr; }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
    class CDuiString;
	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(long x, long y);
		CDuiPoint(LPARAM lParam);
        CDuiPoint(LPCTSTR pstrValue);
        CDuiString ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(long cx, long cy);
        CDuiSize(LPCTSTR pstrValue);
        CDuiString ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(long iLeft, long iTop, long iRight, long iBottom);
        CDuiRect(LPCTSTR pstrValue);
        CDuiString ToString();

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

    /////////////////////////////////////////////////////////////////////////////////////
    //
	class CDuiPtrArray;
    class UILIB_API CDuiString
    {
    public:
        enum { MAX_LOCAL_STRING_LEN = 63 };

        CDuiString();
        CDuiString(const TCHAR ch);
        CDuiString(const CDuiString& src);
        CDuiString(LPCTSTR lpsz, int nLen = -1);
        ~CDuiString();

        void Empty();
        int GetLength() const;
        bool IsEmpty() const;
        TCHAR GetAt(int nIndex) const;
        void Append(LPCTSTR pstr);
        void Assign(LPCTSTR pstr, int nLength = -1);
        LPCTSTR GetData() const;

        void SetAt(int nIndex, TCHAR ch);
        operator LPCTSTR() const;

        TCHAR operator[] (int nIndex) const;
        const CDuiString& operator=(const CDuiString& src);
        const CDuiString& operator=(const TCHAR ch);
        const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
        const CDuiString& operator=(LPCSTR lpStr);
        const CDuiString& operator+=(LPCSTR lpStr);
#else
        const CDuiString& operator=(LPCWSTR lpwStr);
        const CDuiString& operator+=(LPCWSTR lpwStr);
#endif
        CDuiString operator+(const CDuiString& src) const;
        CDuiString operator+(LPCTSTR pstr) const;
        const CDuiString& operator+=(const CDuiString& src);
        const CDuiString& operator+=(LPCTSTR pstr);
        const CDuiString& operator+=(const TCHAR ch);

        bool operator == (LPCTSTR str) const;
        bool operator != (LPCTSTR str) const;
        bool operator <= (LPCTSTR str) const;
        bool operator <  (LPCTSTR str) const;
        bool operator >= (LPCTSTR str) const;
        bool operator >  (LPCTSTR str) const;

        int Compare(LPCTSTR pstr) const;
        int CompareNoCase(LPCTSTR pstr) const;

        void MakeUpper();
        void MakeLower();

        CDuiString Left(int nLength) const;
        CDuiString Mid(int iPos, int nLength = -1) const;
        CDuiString Right(int nLength) const;

        int Find(TCHAR ch, int iPos = 0) const;
        int Find(LPCTSTR pstr, int iPos = 0) const;
        int ReverseFind(TCHAR ch) const;
        int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
		int Replace(TCHAR ch, TCHAR chTo);
		int Replace(TCHAR ch, LPCTSTR pstrTo);

        void SplitToArray(std::vector<CDuiString>& OutArr, LPCTSTR pattern = _T(" "));
        void TrimLeft();
        void TrimRight();
        void Trim();

		int __cdecl FormatVaList(LPCTSTR pstrFormat, va_list Args);
        int __cdecl Format(LPCTSTR pstrFormat, ...);
        int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
		static CDuiString FormatString(LPCTSTR pstrFormat, ...);
	public:
		template<typename T>
		CDuiString& operator<<(T v); // will generate link error
		CDuiString& operator<<(char v);
		CDuiString& operator<<(unsigned char v);
		CDuiString& operator<<(short v);
		CDuiString& operator<<(unsigned short v);
		CDuiString& operator<<(int v);
		CDuiString& operator<<(unsigned int v);
		CDuiString& operator<<(long v);
		CDuiString& operator<<(unsigned long v);
		CDuiString& operator<<(bool v);
		CDuiString& operator<<(float v);
		CDuiString& operator<<(double v);
#ifdef _UNICODE
		CDuiString& operator<<(LPCTSTR str);
#else
		CDuiString& operator<<(LPCWSTR str);
#endif
		CDuiString& operator<<(CDuiString &v);
    protected:
        LPTSTR m_pstr;
        TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
    };

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPtrArray
	{
	public:
		CDuiPtrArray(int iPreallocSize = 0);
		CDuiPtrArray(const CDuiPtrArray& src);
		~CDuiPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex, int iCount = 1);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiValArray
	{
	public:
		CDuiValArray(int iElementSize, int iPreallocSize = 0);
		~CDuiValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex,  int iCount = 1);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

    struct TITEM;
	class UILIB_API CDuiStringPtrMap
	{
	public:
		CDuiStringPtrMap(int nSize = 83);
		~CDuiStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		void Empty();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CDuiVariant : public _variant_t
	{
	public:
		CDuiVariant(const _variant_t& src):_variant_t(src)
		{
		}

		CDuiVariant(LPOLESTR s) : _variant_t((BSTR)s)
		{
		}
		CDuiVariant(IDispatch* disp) : _variant_t(disp)
		{
		}
		CDuiVariant(int r, int g, int b, int a = 0) : _variant_t(((DWORD)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)) | (((DWORD)(BYTE)(a)) << 24)))
		{
		}
		CDuiVariant(int r, int g) : CDuiVariant(r, g, 0, 0)
		{
		}
		CDuiVariant(RECT& rect) : CDuiVariant(rect.left, rect.top, rect.right, rect.top)
		{
		}
		CDuiVariant(SIZE& sz):CDuiVariant(sz.cx, sz.cy)
		{
		}
		CDuiVariant(POINT& pt) :CDuiVariant(pt.x, pt.y)
		{
		}
	};

}// namespace DuiLib

#endif // __UTILS_H__