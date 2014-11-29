#ifndef _TREE_H_
#define _TREE_H_

extern HWND g_hWndTree;

#define IDC_TREE_TREEVIEW   201
#define IDC_TREE_REBAR      202
#define IDC_TREE_STATUS     203

#define ITEMOFFSET          2000

LRESULT CALLBACK WndTreeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinTree(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif