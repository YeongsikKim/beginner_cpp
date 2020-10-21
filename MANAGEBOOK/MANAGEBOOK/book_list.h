#pragma once

class CBook
{
public:
	VOID NodeBookCreate();
	VOID NodeBookRevise(int iBookNum);
	VOID NodeBookErase(int iBookNum);
	VOID NodeBookView();
	VOID RentalService(int iStudentID);
	VOID ReturnService();
};
