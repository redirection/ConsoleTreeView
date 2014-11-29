#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include "main.h"
#include "tree.h"
#include "resource.h"

// Global variables
HWND g_hWndTree;

int WINAPI WinTree(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// prepare for XP style controls
	InitCommonControls();

	// Initialize variables
	static char szClassName[] = "ConsoleTree";
	static char szWindowName[] = "ConsoleTree";

	// Initialize global variables
	g_hInstance = GetModuleHandle(0);

	// Define & register window class
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.cbSize = sizeof(WndClass);						// size of structure
	WndClass.lpszClassName = szClassName;					// name of window class
	WndClass.lpfnWndProc = WndTreeProc;						// points to a window procedure
	WndClass.hInstance = g_hInstance;						// handle to instance
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// predefined app. icon
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		// small class icon
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// predefined arrow
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);	// background brush
	WndClass.lpszMenuName = NULL;							// name of menu resource

	if (!RegisterClassEx(&WndClass))
	{
		return 0;
	}

	// Create actual window
	g_hWndTree = CreateWindowEx(WS_EX_WINDOWEDGE, szClassName, szWindowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 400, 0, 0, g_hInstance, 0);

	if (!g_hWndTree)
	{
		return 0;
	}

	ShowWindow(g_hWndTree, nCmdShow);
	UpdateWindow(g_hWndTree);

	return 0;
}

LRESULT CALLBACK WndTreeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CREATE:
		{
			INITCOMMONCONTROLSEX icex;

			icex.dwSize = sizeof(icex);
			icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES;

			InitCommonControlsEx(&icex);

			// Create TreeView Control
			HFONT hfDefault;
			HWND hTreeView;

			hTreeView = CreateWindowEx(0, WC_TREEVIEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 100, 100,
				hWnd, (HMENU)IDC_TREE_TREEVIEW, GetModuleHandle(NULL), NULL);

			if (hTreeView == NULL)
				MessageBox(hWnd, "failed to create treeview control", "error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hTreeView, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

			TVINSERTSTRUCT tvisItem;
			HTREEITEM hRoot;
			HTREEITEM hItem;
			CHAR szText[81];
			HBITMAP hBitmap;
			HDC hDC;
			int dX;
			int dY;

			HIMAGELIST g_hImageList;

			// Create and initialize image list used with control
			g_hImageList = ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,14,1);

			// Load images based on color depth
			hDC = GetDC(hWnd);
			if(GetDeviceCaps(hDC, BITSPIXEL) <= 8)
				hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
			else
				hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_DEFAULT));
			ReleaseDC(hWnd, hDC);

			// Add images to image list
			ImageList_AddMasked(g_hImageList, hBitmap, RGB(255, 0, 255));

			// Assign image list to control
			TreeView_SetImageList(hTreeView, g_hImageList, TVSIL_NORMAL);

			// Add root item
			tvisItem.hParent = TVI_ROOT;
			tvisItem.hInsertAfter = TVI_LAST;
			tvisItem.itemex.mask = TVIF_STATE|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_INTEGRAL;
			tvisItem.itemex.state = TVIS_EXPANDED;
			tvisItem.itemex.stateMask = TVIS_EXPANDED;
			tvisItem.itemex.pszText = "The Planets";
			tvisItem.itemex.cchTextMax = 11;
			tvisItem.itemex.iImage = 0;
			tvisItem.itemex.iSelectedImage = 0;
			tvisItem.itemex.iIntegral = 1;

			hRoot = TreeView_InsertItem(hTreeView, &tvisItem);

			// Add items to control
			tvisItem.itemex.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_INTEGRAL;
			for(dX=1;dX<10;dX++)
			{
				LoadString(g_hInstance, ITEMOFFSET+(dX-1)*4, szText, sizeof(szText));

				tvisItem.hParent = hRoot;
				tvisItem.itemex.pszText = szText;
				tvisItem.itemex.cchTextMax = (int)strlen(szText);
				tvisItem.itemex.iImage = dX;
				tvisItem.itemex.iSelectedImage = dX;

				hItem = TreeView_InsertItem(hTreeView, &tvisItem);

				// Add sub-items
				for(dY=1;dY<4;dY++)
				{
					LoadString(g_hInstance, ITEMOFFSET+((dX-1)*4)+dY, szText, sizeof(szText));

					tvisItem.hParent = hItem;
					tvisItem.itemex.pszText = szText;
					tvisItem.itemex.cchTextMax = (int)strlen(szText);
					tvisItem.itemex.iImage = dY+9;
					tvisItem.itemex.iSelectedImage = dY+9;

					TreeView_InsertItem(hTreeView, &tvisItem);
				}
			}

			// Clean up
			DeleteObject(hBitmap);

			HWND g_hUserToolTip;

			// Set tooltip handle, returned via 'tooltip' keyword in parser
			g_hUserToolTip = TreeView_GetToolTips(hTreeView);

			// Create Rebar Control
			REBARINFO rbi;
			REBARBANDINFO rbbInfo;

			TBBUTTON tbbStruct[10];
			UINT dImages[] = { 0, 1, 2, -1, 3, 4, -1, 5, -1, 6 };

			COMBOBOXEXITEM cbeItem;
			LPSTR pLabels[] = { "Desktop","Network","3½ Floppy (A:)","HDD (C:)","Control Spy","CDROM (D:)","Printers","Control Panel","Network","Recycle Bin" };
			UINT dIndent[] = { 0, 1, 2, 2, 3, 2, 2, 2, 1, 1 };

			RECT rcControl;
			//HBITMAP hBitmap;
			//int dX;

			HIMAGELIST g_hToolbarIL = NULL;
			HIMAGELIST g_hToolbarHotIL = NULL;
			HIMAGELIST g_hComboBoxExIL = NULL;


			HWND hRebar;

			hRebar = CreateWindowEx(0, REBARCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
				CCS_ADJUSTABLE | RBS_VARHEIGHT | RBS_BANDBORDERS, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_TREE_REBAR, GetModuleHandle(NULL), NULL);

			memset(&rbi, 0, sizeof(rbi));
			rbi.cbSize = sizeof(rbi);
			rbi.fMask = 0;
			rbi.himl = (HIMAGELIST)0;
			
			SendMessage(hRebar, RB_SETBARINFO, 0, (LPARAM)&rbi);

			// Toolbar child
            HWND hToolBar;

			hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | CCS_NORESIZE | CCS_NODIVIDER,
				0, 0, 0, 0, hRebar, NULL, GetModuleHandle(NULL), NULL);

			// Since created toolbar with CreateWindowEx, specify the button structure size
			// This step is not required if CreateToolbarEx was used
			SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

			// Create and initialize image list used for child toolbar
			g_hToolbarIL = ImageList_Create(20, 20, ILC_COLORDDB | ILC_MASK, 7, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_INTERFACE));
			ImageList_AddMasked(g_hToolbarIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Create and initialize hot image list used for child toolbar
			g_hToolbarHotIL = ImageList_Create(20, 20, ILC_COLORDDB | ILC_MASK, 7, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_INTERFACEHOT));
			ImageList_AddMasked(g_hToolbarHotIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Add image lists to toolbar
			SendMessage(hToolBar, TB_SETIMAGELIST, 0,(LPARAM)g_hToolbarIL);
			SendMessage(hToolBar, TB_SETHOTIMAGELIST, 0,(LPARAM)g_hToolbarHotIL);

			// Set toolbar padding
			SendMessage(hToolBar, TB_SETINDENT, 2, 0);

			// Add buttons to control
			for(dX=0;dX<10;dX++)
			{
				// Add button
				if(dImages[dX] != -1)
				{
					tbbStruct[dX].iBitmap = dImages[dX];
					tbbStruct[dX].fsStyle = TBSTYLE_BUTTON;
				}
				else
				{
					tbbStruct[dX].iBitmap = 0;
					tbbStruct[dX].fsStyle = TBSTYLE_SEP;
				}
				tbbStruct[dX].fsState = TBSTATE_ENABLED;
				tbbStruct[dX].idCommand = 40001 + dX;
				tbbStruct[dX].dwData = 0;
				tbbStruct[dX].iString = -1;
			}

			SendMessage(hToolBar, TB_ADDBUTTONS, 10, (LPARAM)tbbStruct);

			// Add Rebar band

			rbbInfo.cbSize = sizeof(REBARBANDINFO);

			rbbInfo.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE;
			rbbInfo.fStyle = RBBS_GRIPPERALWAYS|RBBS_CHILDEDGE;
			rbbInfo.hwndChild = hToolBar;
			rbbInfo.cxMinChild = 0;
			rbbInfo.cyMinChild = HIWORD(SendMessage(hToolBar,TB_GETBUTTONSIZE,0,0));
			rbbInfo.cx = 200;

			SendMessage(hRebar,RB_INSERTBAND,-1,(LPARAM)&rbbInfo);

			// COMBOBOXEX

			HWND hComboBoxEx;

			hComboBoxEx = CreateWindowEx(0, WC_COMBOBOXEX, NULL,
				WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, //CBS_DROPDOWNLIST,
				0, 0, 300, 150, hRebar, NULL, GetModuleHandle(NULL), NULL);

			// Create and initialize image list used for child ComboBoxEx
			g_hComboBoxExIL = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 10, 1);
			hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_COMPUTER));
			ImageList_AddMasked(g_hComboBoxExIL, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);

			// Assign image list to control
			SendMessage(hComboBoxEx, CBEM_SETIMAGELIST, 0, (LPARAM)g_hComboBoxExIL);

			// Add items to control
			for(dX=0;dX<10;dX++)
			{
				cbeItem.mask = CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT|CBEIF_INDENT;
				cbeItem.iItem = -1;
				cbeItem.pszText = pLabels[dX];
				cbeItem.cchTextMax = (int)strlen(pLabels[dX]);
				cbeItem.iImage = dX;
				cbeItem.iSelectedImage = dX;
				cbeItem.iIndent = dIndent[dX];

				SendMessage(hComboBoxEx, CBEM_INSERTITEM, 0, (LPARAM)&cbeItem);
			}

			// Select first item
			SendMessage(hComboBoxEx, CB_SETCURSEL, 4, 0);

			// Add Rebar band

			rbbInfo.cbSize = sizeof(REBARBANDINFO);
			rbbInfo.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_TEXT|RBBIM_CHILDSIZE|RBBIM_SIZE;
			rbbInfo.fStyle = RBBS_GRIPPERALWAYS|RBBS_CHILDEDGE;
			rbbInfo.hwndChild = hComboBoxEx;
			GetWindowRect(hComboBoxEx, &rcControl);
			rbbInfo.cxMinChild = 0;
			rbbInfo.cyMinChild = rcControl.bottom-rcControl.top;
			rbbInfo.lpText = "Address";
			rbbInfo.cx = 200;

			SendMessage(hRebar, RB_INSERTBAND, -1, (LPARAM)&rbbInfo);


			// Create StatusBar Control
			HWND hStatus;
			int statwidths[] = {100, -1};

			hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_TREE_STATUS, GetModuleHandle(NULL), NULL);

			SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"0 ms");
			SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)"Console Ready...");
		}
		break;
		case WM_SIZE:
		{
			HWND hTool;
			RECT rcTool;
			int iToolHeight;

			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			HWND hTreeView;
			int iTreeViewHeight;
			RECT rcClient;

			// Size toolbar and get height

			hTool = GetDlgItem(hWnd, IDC_TREE_REBAR);

			GetWindowRect(hTool, &rcTool);
			iToolHeight = rcTool.bottom - rcTool.top;

			// Resize rebar
			MoveWindow(hTool, 0, 0, LOWORD(lParam), iToolHeight, FALSE);

			// Size statusbar and get height

			hStatus = GetDlgItem(hWnd, IDC_TREE_STATUS);
			SendMessage(hStatus, WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;

			// Calculate the remaining height and size richedit

			GetClientRect(hWnd, &rcClient);

			iTreeViewHeight = rcClient.bottom - iToolHeight - iStatusHeight;

			hTreeView = GetDlgItem(hWnd, IDC_TREE_TREEVIEW);
			SetWindowPos(hTreeView, NULL, 0, iToolHeight, rcClient.right, iTreeViewHeight, SWP_NOZORDER);

			InvalidateRect(hWnd, 0, FALSE);
		}
		break;
		case WM_CLOSE:
		{
			// Do not destroy window yet
			ShowWindow(hWnd, SW_HIDE);

			return 0;
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case ID_FILE_NEW:
					MessageBox(hWnd, "ID_FILE_NEW", "info", MB_OK | MB_ICONINFORMATION);
				break;
				case ID_FILE_OPEN:
					MessageBox(hWnd, "ID_FILE_OPEN", "info", MB_OK | MB_ICONINFORMATION);
				break;
				case ID_FILE_SAVEAS:
					MessageBox(hWnd, "ID_FILE_SAVEAS", "info", MB_OK | MB_ICONINFORMATION);
				break;
			}
		}
		break;
		case WM_GETMINMAXINFO:
		{
			// Set window size constraints
			((MINMAXINFO *)lParam)->ptMinTrackSize.x = 300;
			((MINMAXINFO *)lParam)->ptMinTrackSize.y = 400;
		}
		break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}