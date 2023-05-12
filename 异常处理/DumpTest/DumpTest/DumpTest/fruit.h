#pragma once

class Fruit
{
public:
	int GetPrice()
	{
		return m_nPrice;
	}

	int GetSize()
	{
		return m_nSize;
	}

	void SetPrice(int nPrice)
	{
		m_nPrice = nPrice;
	}

	void SetSize(int nSize)
	{
		m_nSize = nSize;
	}
	
	virtual void ShowInfo();



private:
	int m_nPrice;
	int m_nSize;
};

class B1
{
public:
	virtual void fun1()
	{
		::OutputDebugString(_T("B1::fun1\n"));
	}

	virtual void fun2()
	{
		::OutputDebugString(_T("B1::fun2\n"));
	}

public:
	int m_nX;
};

class B2
{
public:
	virtual void fun1()
	{
		::OutputDebugString(_T("B2::fun1\n"));
	}

	virtual void fun3()
	{
		::OutputDebugString(_T("B2::fun3\n"));
	}

public:
	int m_nY;
};

class D : public B1, public B2
{
public:
	/*virtual void fun1()
	{
	::OutputDebugString(_T("D::fun1\n"));
	}*/

	virtual void fun2()
	{
		::OutputDebugString(_T("D::fun2\n"));
	}

	virtual void fun3()
	{
		::OutputDebugString(_T("D::fun3\n"));
	}

	virtual void fun4()
	{
		::OutputDebugString(_T("D::fun4\n"));
	}

public:
	int m_nZ;
};