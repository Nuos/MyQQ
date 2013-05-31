


#include <commctrl.h>



bool GetTreeItemText(HWND h_tree,HTREEITEM h_treeitem,CHAR* TreeText);

bool GetTreeSelectItemText(HWND hwnd,int TreeId,CHAR* TreeText);

HTREEITEM f(HWND h_tree,HTREEITEM h_TreeItem,CHAR* TreeText);

HTREEITEM GetTreeItemByText(HWND hwnd,int TreeId,CHAR* TreeText);

HTREEITEM AddItemToTree(HWND hwnd,	int controlId,LPSTR lpszItem,int iconId,HTREEITEM hParent,BOOL bFolder);

void CreateImageList(HWND hwnd);

bool DeleteItemByText(HWND hwnd,int TreeId,char* ItemText);
