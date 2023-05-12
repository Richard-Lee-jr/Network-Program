#pragma once
#include "fruit.h"
#include <string>
#include <iostream>

class SouthFruit : public Fruit
{
public:

	void SetDes(std::string sDes)
	{
		//std::cout << m_nSize << m_nPrice << std::endl;
		m_sDes = sDes;
	}

	std::string GetDes()
	{
		return m_sDes;
	}

	virtual void ShowInfo();
	

	virtual void ShowDes();
	

private:
	std::string m_sDes;	//ÃèÊö
};