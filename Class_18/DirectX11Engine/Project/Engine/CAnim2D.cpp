#include "pch.h"
#include "CAnim2D.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CAssetMgr.h"

#include "CTexture.h"


CAnim2D::CAnim2D()
	: m_Animator(nullptr)
	, m_CurIdx(0)
	, m_Finish(false)
	, m_Time(0.f)
{
}

CAnim2D::CAnim2D(const CAnim2D& _Other)
	: CEntity(_Other)
	, m_Animator(nullptr)
	, m_vecFrm(_Other.m_vecFrm)
	, m_BackgroundSize(_Other.m_BackgroundSize)
	, m_CurIdx(_Other.m_CurIdx)
	, m_Finish(_Other.m_Finish)
	, m_Time(_Other.m_Time)
	, m_AtlasTex(_Other.m_AtlasTex)
{
}

CAnim2D::~CAnim2D()
{
}

void CAnim2D::Create(Ptr<CTexture> _AltasTex, Vec2 _LeftTopPixelPos, Vec2 _vSlicePixelSize, Vec2 _BackgroundSize
					, int _FrameCount, UINT _FPS)
{
	assert(_AltasTex.Get());

	m_AtlasTex = _AltasTex;

	m_BackgroundSize.x = _BackgroundSize.x / m_AtlasTex->GetWidth();
	m_BackgroundSize.y = _BackgroundSize.y / m_AtlasTex->GetHeight();
	

	for (int i = 0; i < _FrameCount; ++i)
	{
		tAnim2DFrm frm = {};

		frm.LeftTop.x = (_LeftTopPixelPos.x + _vSlicePixelSize.x * i) / m_AtlasTex->GetWidth();
		frm.LeftTop.y = _LeftTopPixelPos.y / m_AtlasTex->GetHeight();

		frm.Duration = 1.f / (float)_FPS;

		frm.SliceSize.x = _vSlicePixelSize.x / m_AtlasTex->GetWidth();
		frm.SliceSize.y = _vSlicePixelSize.y / m_AtlasTex->GetHeight();
		
		/*if (i == 1)
		{
			frm.Offset.x += 5.f / m_AtlasTex->GetWidth();
			frm.Offset.y = 0.f;			
		}*/

		m_vecFrm.push_back(frm);
	}	
}

void CAnim2D::finaltick()
{
	if (m_Finish)
		return;

	m_Time += DT;

	if (m_vecFrm[m_CurIdx].Duration < m_Time)
	{		
		m_Time = m_Time - m_vecFrm[m_CurIdx].Duration;
		++m_CurIdx;

		if ((int)m_vecFrm.size() <= m_CurIdx)
		{
			m_CurIdx = (int)m_vecFrm.size() - 1;
			m_Finish = true;
		}
	}	
}

void CAnim2D::Binding()
{
	// ��Ʋ�� �ؽ��� 
	if (nullptr != m_AtlasTex)
		m_AtlasTex->Binding(14);

	// ������ ����
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION);

	tAnim2DInfo info = {};
	info.vLeftTop = m_vecFrm[m_CurIdx].LeftTop;
	info.vSliceSize = m_vecFrm[m_CurIdx].SliceSize;
	info.vOffset = m_vecFrm[m_CurIdx].Offset;
	info.vBackground = m_BackgroundSize;
	info.UseAnim2D = 1;

	pCB->SetData(&info);
	pCB->Binding();
}

void CAnim2D::Clear()
{
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION);
	pCB->Clear();
}

void CAnim2D::Save(const wstring& _RelativeFolderPath)
{	
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _RelativeFolderPath + GetName() + L".anim";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");
	
	if (nullptr == pFile)
	{
		MessageBox(nullptr, L"�ִϸ��̼� ���� ����", L"�ִϸ��̼� ���� ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� ����
	SaveWString(GetName(), pFile);

	// ������ ���� ����
	size_t i = m_vecFrm.size();
	fwrite(&i, sizeof(size_t), 1, pFile);	

	// ������ ���� ����
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), i, pFile);

	// BackgroundSize ����
	fwrite(&m_BackgroundSize, sizeof(Vec2), 1, pFile);

	// ��Ʋ�� �ؽ��� ���� ����
	SaveAssetRef(m_AtlasTex, pFile);

	fclose(pFile);
}

void CAnim2D::Load(const wstring& _RelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _RelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		MessageBox(nullptr, L"�ִϸ��̼� �ε� ����", L"�ִϸ��̼� �ε� ����", MB_OK);
		return;
	}

	// �ִϸ��̼� �̸� �ε�
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// ������ ���� �ε�
	size_t i = 0;
	fread(&i, sizeof(size_t), 1, pFile);

	// ������ ���� �ε�
	m_vecFrm.resize(i);
	fread(m_vecFrm.data(), sizeof(tAnim2DFrm), i, pFile);

	// BackgroundSize �ε�
	fread(&m_BackgroundSize, sizeof(Vec2), 1, pFile);

	// ��Ʋ�� �ؽ��� �ε�
	LoadAssetRef(m_AtlasTex, pFile);

	fclose(pFile);
}


void CAnim2D::SaveToLevelFile(FILE* _File)
{
	// Animation �� �̸������� ����
	SaveWString(GetName(), _File);

	// ������ ���� ����
	size_t FrmCount = m_vecFrm.size();
	fwrite(&FrmCount, sizeof(size_t), 1, _File);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), m_vecFrm.size(), _File);	
	
	// ��׶��� ũ��
	fwrite(&m_BackgroundSize, sizeof(Vec2), 1, _File);

	// �����ϴ� ��Ʋ�� �ؽ��� ���� ����
	SaveAssetRef(m_AtlasTex, _File);
}

void CAnim2D::LoadFromLevelFile(FILE* _File)
{
	// Animation �� �̸����� �ε�
	wstring name;
	LoadWString(name, _File);
	SetName(name);

	// ������ ���� �ε�
	size_t FrmCount = 0;
	fread(&FrmCount, sizeof(size_t), 1, _File);
	m_vecFrm.resize(FrmCount);
	fread(m_vecFrm.data(), sizeof(tAnim2DFrm), FrmCount, _File);

	// ��׶��� ũ��
	fread(&m_BackgroundSize, sizeof(Vec2), 1, _File);

	// �����ϴ� ��Ʋ�� �ؽ��� ���� ����
	LoadAssetRef(m_AtlasTex, _File);
}