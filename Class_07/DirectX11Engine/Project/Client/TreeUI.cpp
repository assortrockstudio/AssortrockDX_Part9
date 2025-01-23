#include "pch.h"
#include "TreeUI.h"

// =========
// Tree Node
// =========
UINT TreeNode::g_ID = 0;

TreeNode::~TreeNode()
{
	Safe_Del_Vector(m_vecChildNode);
}

void TreeNode::render_tick()
{
	// TreeNode �� ��� �̸� ����
	string name;
	if (m_Name.empty())
		name = string("None") + "##" + m_ID;
	else
		name = m_Name + "##" + m_ID;
	
		
	// ȭ��ǥ�� �����ų�, ����Ŭ���� ��쿡 �ڽĳ�带 ��ģ��.
	UINT iFlag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

	// ������ ����
	if (m_bFrame)
		iFlag |= ImGuiTreeNodeFlags_Framed;

	if (m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;

	

	// ���� �̸� ����
	if (m_Owner->m_bShowFileName)
	{
		char buffer[256] = {};
		_splitpath_s(name.c_str(), 0, 0, 0, 0, buffer, 256, 0, 0);
		name = buffer;
	}

	// ȭ��ǥ ����
	if (m_vecChildNode.empty())
	{
		iFlag |= ImGuiTreeNodeFlags_Leaf;
		if (m_bFrame)
			name = "   " + name;
	}

	if (ImGui::TreeNodeEx(name.c_str(), iFlag))
	{
		// ����� ������(TreeUI) �� Drag ����� ����ϱ�� �ߴٸ�...
		if (m_Owner->m_bUseDrag)
		{
			if (ImGui::BeginDragDropSource())
			{
				// TreeUI �� ������ Drag ������� ���
				m_Owner->SetDragNode(this);

				// ����� ������(TreeUI), �� Parent �� TreeUI �� �ڽ����� �ΰ��ִ� �θ�UI(ex Outliner, ContentUI)				
				ImGui::SetDragDropPayload(m_Owner->GetParentUI()->GetName().c_str(), &m_Data, sizeof(DWORD_PTR));

				// PayLoad ��, EndDratDrop ���� ���� Text �� �������� ����ȴ�.
				ImGui::Text(m_Name.c_str());
				
				ImGui::EndDragDropSource();
			}
		}

		if (m_Owner->m_bUseDrag && m_Owner->m_bUseDrop)
		{
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_Owner->GetParentUI()->GetName().c_str());

				if (nullptr != payload)
				{
					m_Owner->SetDropNode(this);
				}

				ImGui::EndDragDropTarget();
			}
		}



		// �ش� ��������� ���콺 ������ �߻��ϸ� �ش� ��带 ���õ� ���·� �����.
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{	
			if (ImGui::IsItemHovered())
			{
				// ������ ���� ���ð����� ����.
				if (!m_bFrame)
				{
					m_Owner->SetSelectedNode(this);
				}
			}			
		}

		// �ڽ� ��� render
		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_tick();
		}

		ImGui::TreePop();
	}
}













// ========
// TreeUI
// ========
TreeUI::TreeUI(const string& _Name)
	: EditorUI(_Name, "##Tree")
	, m_RootNode(nullptr)
	, m_bShowRoot(true)
	, m_bShowFileName(false)
	, m_SelectedNode(nullptr)
	, m_DragNode(nullptr)
	, m_DropNode(nullptr)
	, m_bUseDrag(false)
	, m_bUseDrop(false)
	, m_SelectedCallBack(nullptr)
	, m_SelectedInst(nullptr)
	, m_SelectedDelegate(nullptr)
	, m_DragDropInst(nullptr)
	, m_DragDropDelegate(nullptr)
{
}

TreeUI::~TreeUI()
{
	if (nullptr != m_RootNode)
	{
		delete m_RootNode;
	}
}

TreeNode* TreeUI::AddTreeNode(TreeNode* _Parent, const string& _NodeName, DWORD_PTR _dwData)
{
	TreeNode* pNewNode = new TreeNode(_NodeName, _dwData);

	// ����� �����ڰ� Ʈ������ ������
	pNewNode->m_Owner = this;

	// �θ��带 �������� ������, ��Ʈ���� �߰��ϰڴٴ� �ǹ�	
	if (nullptr == _Parent)
	{
		// �̹� ��Ʈ�� TreeUI �� �����ϸ� assert
		assert(!m_RootNode);

		m_RootNode = pNewNode;		
	}

	else
	{
		_Parent->AddChildNode(pNewNode);
	}


	return pNewNode;
}


void TreeUI::render_tick()
{	
	// Ʈ���� ��尡 �ϳ��� ���� ���
	if (nullptr == m_RootNode)
		return;

	if (m_bShowRoot)
	{
		m_RootNode->render_tick();
	}

	else
	{
		const vector<TreeNode*>& vecChildNode = m_RootNode->GetChildNode();

		for (size_t i = 0; i < vecChildNode.size(); ++i)
		{
			vecChildNode[i]->render_tick();
		}
	}

	if (m_DragNode && (m_DropNode || ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
	{
		if (m_DragDropInst && m_DragDropDelegate)
		{
			(m_DragDropInst->*m_DragDropDelegate)((DWORD_PTR)m_DragNode, (DWORD_PTR)m_DropNode);
		}

		m_DragNode = nullptr;
		m_DropNode = nullptr;
	}
}


void TreeUI::SetSelectedNode(TreeNode* _Node)
{
	// ������ ���õ� ��尡 ������ ���û��¸� ����
	if (m_SelectedNode)
		m_SelectedNode->m_bSelected = false;

	m_SelectedNode = _Node;

	if (m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = true;

		// ��ϵǾ��ִ� CallBack, Delegate �� ������ ȣ������ش�.
		if (m_SelectedCallBack)
		{
			m_SelectedCallBack();
		}

		if (m_SelectedInst && m_SelectedDelegate)
		{
			(m_SelectedInst->*m_SelectedDelegate)((DWORD_PTR)m_SelectedNode);
		}
	}	
}