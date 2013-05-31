

#include "stdafx.h"
#include "resource.h"
#include "TreeviewFunction.h"




/* ************************************
* GetTreeItemText
* ����	���ݽڵ�ľ���õ��ڵ���ı�
* ����	h_tree��Tree View�ؼ����
*		h_treeitem���ڵ���
*		TreeText�����յõ��Ľڵ��ı�
* ����ֵ	�ɹ�����trueʧ�ܷ���false
**************************************/
bool GetTreeItemText(HWND h_tree,HTREEITEM h_treeitem,CHAR* TreeText)  
{
	if(TreeText==NULL)
		return false;
	
	//������սṹ
	TVITEM tree;
	tree.mask= TVIF_TEXT ;
	tree.pszText=new CHAR[255];
	tree.cchTextMax=255;
	tree.hItem=h_treeitem;
	//���н���			
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
* ����	�õ���ǰtreeviewѡ�еĽڵ���ı�
* ����	hwnd��Tree View�ؼ����ڴ��ھ��
*		TreeId��treeview�ؼ���id
*		TreeText�����յõ��Ľڵ��ı�
* ����ֵ	�ɹ�����trueʧ�ܷ���false
**************************************/
bool GetTreeSelectItemText(HWND hwnd,int TreeId,CHAR* TreeText)
{
	//�õ��ؼ����
	HWND h_tree=GetDlgItem(hwnd,TreeId);
	//�õ�ѡ�нڵ���
	HTREEITEM h_treeitem=TreeView_GetSelection(h_tree);
	return GetTreeItemText(h_tree,h_treeitem,TreeText);
}




/* ************************************
* f(�ݹ麯��)
* ����	GetTreeItemByText�����ķ�����
* ����	h_tree��Tree View�ؼ����
*		h_TreeItem���ڵ���
*		TreeText��Ҫ���ҽڵ��ı�
* ����ֵ	�ɹ����ظýڵ��HTREEITEMʧ�ܷ���NULL
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
* ����	ͨ���ڵ��ı����ҵ��ڵ���
* ����	hwnd��Tree View�ؼ����ڴ��ھ��
*		TreeId��treeview�ؼ���id
*		TreeText��Ҫ���ҽڵ��ı�
* ����ֵ	�ɹ����ظýڵ��HTREEITEMʧ�ܷ���NULL
**************************************/
HTREEITEM GetTreeItemByText(HWND hwnd,int TreeId,CHAR* TreeText)
{
	//�õ��ؼ����
	HWND h_tree=GetDlgItem(hwnd,TreeId);
	//�õ�root�ڵ���
	HTREEITEM h_TreeItem=TreeView_GetRoot(h_tree);
	//CHAR TreeTextc[255];
	//wcstombs(TreeTextc,TreeText,sizeof(TreeTextc)/sizeof(char));
	
	h_TreeItem=f(h_tree,h_TreeItem,TreeText);
	return h_TreeItem;
}

/* ************************************
* AddItemToTree
* ����	ΪTree View�ؼ����ӽڵ�
* ����	hwndTV,Tree View�ؼ����ڴ��ڵľ��
*		controlId,treeview�ؼ���id
*		lpszItem���ڵ������ı���
*		iconId  �ڵ��ͼ��id
*		hParent�����ڵ��������ΪNULL�������Ӹ��ڵ�
*		bFolder���Ƿ�����ӽڵ㣨Ӱ��ڵ�ͼ��ICON��
* ����ֵ	�����Ľڵ���
**************************************/
HTREEITEM AddItemToTree(
						HWND hwnd,
						int controlId,
						LPSTR lpszItem, 
						int iconId,
						HTREEITEM hParent, 
						BOOL bFolder)
{
	//MessageBox(hwnd,"����AddItemToTree...","",0);
	Sleep(10);
	TVITEM tvi;
	TVITEM tParent;
	TVINSERTSTRUCT tvins;
	HTREEITEM hme;
	// ���TVITEM�ṹ
	// ͨ��TVITEM��mask�ֶΣ�˵���Ǵ��ֶ�����Ч��
	tvi.mask = TVIF_TEXT   | TVIF_PARAM| TVIF_IMAGE| TVIF_SELECTEDIMAGE;
	// �ֽڵ��ı����ı�����
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]);	
	// ����bFolder����Ϊ�ڵ�ָ����ͬ��ͼ��
	
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
	
	
	// ���TVINSERTSTRUCT�ṹ
	tvins.item = tvi;
	// �²�����ӽڵ��λ�ã�TVI_SORT��ʾ���ı�����
	// ��������TVI_FIRST TVI_LAST�ȡ�	
	tvins.hInsertAfter = TVI_SORT;
	// ���hParentΪNULL�������Ľڵ�Ϊ���ڵ㣬
	// ����hParentΪ�丸�ڵ�
//	
	if(hParent == NULL)
	{
		tvins.hParent = TVI_ROOT;
	}
	else
	{
		tvins.hParent = hParent;
	}
	// ����TreeView_InsertItem�꣬�����½ڵ�
	//MessageBox(hwnd,"AddItemToTree...","",0);
	hme =(HTREEITEM)SendDlgItemMessage(hwnd,controlId,TVM_INSERTITEM,0,(LPARAM)&tvins);
	//MessageBox(hwnd,"�˳�AddItemToTree...","",0);
	return hme;
}


/* ************************************
* CreateImageList
* ����	����ͼ���б�
* ����	HWND hwnd ���ھ��
* ����ֵ	��
**************************************/
void CreateImageList(HWND hwnd)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HIMAGELIST himl;
    HICON hicon = NULL;
    HWND hwndTV = GetDlgItem(hwnd,IDC_TREE1);
    HINSTANCE g_hinst = (HINSTANCE)GetWindowLong(hwndTV,GWL_HINSTANCE);
    himl = ImageList_Create(16, 16, ILC_COLOR32, 4, 4);
	//������ǰĿ¼�õ�/iconĿ¼��������ļ�
	WIN32_FIND_DATA findfile;
	HANDLE hFindFile;
	hFindFile=FindFirstFile("./icon/*.ico",&findfile);
	int MaxIconNum = 0;

	if( hFindFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwnd,TEXT("����ͼ���ļ�ʧ��,������iconĿ¼��ͼ���ļ���ʧ,�����°�װ"),TEXT("����"),MB_OK);
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
	//��ͼ���ļ����뵽ͼ���б�
	for (int i = 0;i < MaxIconNum; i++)
	{
		char IconName[255];
		sprintf(IconName,"./icon/%d.ico",i);
		hicon = (HICON) LoadImage(NULL,IconName,IMAGE_ICON,0,0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		if ( hicon == NULL)
		{
			//�������ʧ��˵�����ļ�������,��ô�ͼ���0.ico
			hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_DEFINE) );
			MessageBox(hwnd,"ddd","",0);
		}
		ImageList_AddIcon(himl, hicon);
		hicon = NULL;
	}


	TreeView_SetImageList(hwndTV, himl, TVSIL_NORMAL);
	DWORD dwStyles=GetWindowLong(hwndTV,GWL_STYLE);//��ȡ������ԭ���
    dwStyles|=TVS_EDITLABELS|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
    SetWindowLong(hwndTV,GWL_STYLE,dwStyles);//���÷��

	
	return;
}


//����Ϣ��treeview��ɾ��
/* ************************************
* DeleteItemByText
* ����	�����ı�ֵ����Ϣ��treeview��ɾ��
* ����	hwnd��Tree View�ؼ����ڴ��ھ��
*		TreeId��treeview�ؼ���id
*		ItemText��Ҫɾ���ڵ��ı�
* ����ֵ	�ɹ����ظýڵ��trueʧ�ܷ���false
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

