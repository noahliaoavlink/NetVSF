// VSFServer.h: VSFServer DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CVSFServerApp
// 此類別的實作請參閱 VSFServer.cpp
//

class CVSFServerApp : public CWinApp
{
public:
	CVSFServerApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
