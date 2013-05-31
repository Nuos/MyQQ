

#include "stdafx.h"
#include "resource.h"
#include "TreeviewFunction.h"




/* ************************************
* GetTreeItemText
* 功能	根据节点的句柄得到节点的文本
* 参数	h_tree，Tree View控件句柄
*		h_treeitem，节点句柄
*		TreeText，接收得到的节点文本
* 返回值	成功返回true失败返回false
**************************************/
bool GetTreeItemText(HWND h_tree,HTREEITEM h_treeitem,CHAR* TreeText)  
{
	if(TreeText==NULL)
		return false;
	
	//构造接收结构
	TVITEM tree;
	tree.mask= TVIF_TEXT ;
	tree.pszText=new CHAR[255];
	tree.cchTextMax=255;
	tree.hItem=h_treeitem;
	//进行接收			
	//TreeView_EnsureVisible(hwnd,hse1);
	if(TreeView_GetItem(h_tree,&tree))
	{
		sprintf(TreeText,"%s",tree.pszText);
		delete [] tree.pszText;
		return true;
	}
	else
	{
		sprintf(TreeText,"");
		return false;
	}
}

/* ************************************
* GetTreeSelectItemText
* 功能	得到当前treeview选中的节点的文本
* 参数	hwnd，Tree View控件所在窗口句柄
*		TreeId，treeview控件的id
*		TreeText，接收得到的节点文本
* 返回值	成功返回true失败返回false
**************************************/
bool GetTreeSelectItemText(HWND hwnd,int TreeId,CHAR* TreeText)
{
	//得到控件句柄
	HWND h_tree=GetDlgItem(hwnd,TreeId);
	//得到选中节点句柄
	HTREEITEM h_treeitem=TreeView_GetSelection(h_tree);
	return GetTreeItemText(h_tree,h_treeitem,TreeText);
}




/* ************************************
* f(递归函数)
* 功能	GetTreeItemByText函数的服务函数
* 参数	h_tree，Tree View控件句柄
*		h_TreeItem，节点句柄
*		TreeText，要查找节点文本
* 返回值	成功返回该节点的HTREEITEM失败返回NULL
**************************************/
HTREEITEM f(HWND h_tree,HTREEITEM h_TreeItem,CHAR* TreeText)
{
	//MessageBox(NULL,TreeText,TEXT(""),0);
	if((h_TreeItem==NULL))
		return NULL;
	CHAR getText[255];	
	GetTreeItemText(h_tree,h_TreeItem,getText);
	if(!strcmp(TreeText,getText))
		return h_TreeItem;
	HTREEITEM h_rTreeItem;
	if((h_rTreeItem=f(h_tree,TreeView_GetChild(h_tree,h_TreeItem),TreeText))!=NULL)
		return h_rTreeItem;
	if((h_rTreeItem=f(h_tree,TreeView_GetNextSibling(h_tree,h_TreeItem),TreeText))!=NULL)
		return h_rTreeItem;
	else
		return NULL;
}


/* ************************************
* GetTreeItemByText
* 功能	通过节点文本查找到节点句柄
* 参数	hwnd，Tree View控件所在窗口句柄
*		TreeId，treeview控件的id
*		TreeText，要查找节点文本
* 返回值	成功返回该节点的HTREEITEM失败返回NULL
**************************************/
HTREEITEM GetTreeItemByText(HWND hwnd,int TreeId,CHAR* TreeText)
{
	//得到控件句柄
	HWND h_tree=GetDlgItem(hwnd,TreeId);
	//得到root节点句柄
	HTREEITEM h_TreeItem=TreeView_GetRoot(h_tree);
	//CHAR TreeTextc[255];
	//wcstombs(TreeTextc,TreeText,sizeof(TreeTextc)/sizeof(char));
	
	h_TreeItem=f(h_tree,h_TreeItem,TreeText);
	return h_TreeItem;
}

/* ************************************
* AddItemToTree
* 功能	为Tree View控件增加节点
* 参数	hwndTV,Tree View控件所在窗口的句柄
*		controlId,treeview控件的id
*		lpszItem，节点名（文本）
*		iconId  节点的图标id
*		hParent，父节点句柄，如果为NULL，则增加根节点
*		bFolder，是否具有子节点（影响节点图标ICON）
* 返回值	创建的节点句柄
**************************************/
HTREEITEM AddItemToTree(
						HWND hwnd,
						int controlId,
						LPSTR lpszItem, 
						int iconId,
						HTREEITEM hParent, 
						BOOL bFolder)
{
	//MessageBox(hwnd,"进入AddItemToTree...","",0);
	Sleep(10);
	TVITEM tvi;
	TVITEM tParent;
	TVINSERTSTRUCT tvins;
	HTREEITEM hme;
	// 填充TVITEM结构
	// 通过TVITEM的mask字段，说明那此字段是有效的
	tvi.mask = TVIF_TEXT   | TVIF_PARAM| TVIF_IMAGE| TVIF_SELECTEDIMAGE;
	// 字节的文本及文本长度
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]);	
	// 根据bFolder参数为节点指定不同的图标
	
	if(bFolder)
	{
		tvi.iImage = iconId;
		tvi.iSelectedImage = iconId;
		tvi.cChildren = I_CHILDRENCALLBACK;
	}
	else
	{
		tvi.iImage = iconId;
		tvi.iSelectedImage = iconId;
		tvi.cChildren = 0;
	}
	
	
	// 填充TVINSERTSTRUCT结构
	tvins.item = tvi;
	// 新插入的子节点的位置，TVI_SORT表示按文本排序
	// 还可以是TVI_FIRST TVI_LAST等。	
	tvins.hInsertAfter = TVI_SORT;
	// 如果hParent为NULL，则插入的节点为根节点，
	// 否则hParent为其父节点
//	
	if(hParent == NULL)
	{
		tvins.hParent = TVI_ROOT;
	}
	else
	{
		tvins.hParent = hParent;
	}
	// 调用TreeView_InsertItem宏，插入新节点
	//MessageBox(hwnd,"AddItemToTree...","",0);
	hme =(HTREEITEM)SendDlgItemMessage(hwnd,controlId,TVM_INSERTITEM,0,(LPARAM)&tvins);
	//MessageBox(hwnd,"退出AddItemToTree...","",0);
	return hme;
}


/* ************************************
* CreateImageList
* 功能	建立图标列表
* 参数	HWND hwnd 窗口句柄
* 返回值	无
**************************************/
void CreateImageList(HWND hwnd)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HIMAGELIST himl;
    HICON hicon = NULL;
    HWND hwndTV = GetDlgItem(hwnd,IDC_TREE1);
    HINSTANCE g_hinst = (HINSTANCE)GetWindowLong(hwndTV,GWL_HINSTANCE);
    himl = ImageList_Create(16, 16, ILC_COLOR32, 4, 4);
	//遍历当前目录得到/icon目录编号最大的文件
	WIN32_FIND_DATA findfile;
	HANDLE hFindFile;
	hFindFile=FindFirstFile("./icon/*.ico",&findfile);
	int MaxIconNum = 0;

	if( hFindFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwnd,TEXT("加载图标文件失败,可能是icon目录下图标文件丢失,请重新安装"),TEXT("错误"),MB_OK);
		return;
	}
	else
	{
		do
		{
			char c_IcoNum[25];
			memset(c_IcoNum,0,sizeof(c_IcoNum));
			sscanf(findfile.cFileName,"%[^.]",c_IcoNum);
			int num = atoi(c_IcoNum);
			if (num > MaxIconNum)
			{
				MaxIconNum = num;
			}		
		}
		while( FindNextFile(hFindFile,&findfile) );
	}
	//将图标文件导入到图标列表
	for (int i = 0;i < MaxIconNum; i++)
	{
		char IconName[255];
		sprintf(IconName,"./icon/%d.ico",i);
		hicon = (HICON) LoadImage(NULL,IconName,IMAGE_ICON,0,0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		if ( hicon == NULL)
		{
			//如果加载失败说明该文件不存在,那么就加载0.ico
			hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_DEFINE) );
			MessageBox(hwnd,"ddd","",0);
		}
		ImageList_AddIcon(himl, hicon);
		hicon = NULL;
	}


	TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);
	DWORD dwStyles=GetWindowLong(hwndTV,GWL_STYLE);//获取树控制原风格
    dwStyles|=TVS_EDITLABELS|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
    SetWindowLong(hwndTV,GWL_STYLE,dwStyles);//设置风格

	
	return;
}


//将信息从treeview中删除
/* ************************************
* DeleteItemByText
* 功能	根据文本值将信息从treeview中删除
* 参数	hwnd，Tree View控件所在窗口句柄
*		TreeId，treeview控件的id
*		ItemText，要删除节点文本
* 返回值	成功返回该节点的true失败返回false
**************************************/
bool DeleteItemByText(HWND hwnd,int TreeId,char* ItemText)
{
	HTREEITEM hTreeItem = GetTreeItemByText(hwnd,TreeId,ItemText);
	if (hTreeItem == NULL)
	{
		return false;
	}
	if ( !SendDlgItemMessage(hwnd,TreeId,TVM_DELETEITEM,0,(LPARAM)hTreeItem) )
	{
		return false;
	}
	else
	{
		return true;
	}
}

