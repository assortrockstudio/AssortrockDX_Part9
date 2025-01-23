#pragma once

// ����
// 3D ������ ��ǥ�� ��Ÿ���� ���� 
struct Vtx
{	
	Vec3	vPos;
	Vec4	vColor;
	Vec2	vUV;

	Vec3	vTangent;	// ���� ����
	Vec3	vNormal;	// ���� ����
	Vec3	vBinormal;	// ������ ����
};


// ����� ���� ��û ����
struct tDebugShapeInfo
{
	DEBUG_SHAPE		Shape;
	Vec3			Position;
	Vec3			Scale;
	Vec3			Rotation;
	Matrix			matWorld;
	Vec4			Color;
	bool			DepthTest;
	float			Duration;
	float			Age;	
};

struct tTask
{
	TASK_TYPE	Type;

	DWORD_PTR	dwParam_0;
	DWORD_PTR	dwParam_1;
	DWORD_PTR	dwParam_2;
};


// ���� ���� ����ü
struct tLight
{
	Vec4	vDiffuse;		// ���ݻ� (���� ����)
	Vec4	vAmbient;		// ȯ�汤 (������ �߻��ϴ� �ּ����� ���)
	Vec4	vMaxSpecular;	// �ݻ籤 (������ �߻��ϴ� �ݻ籤�� �ִ� ���)
};

struct tLightInfo
{
	tLight	Light;

	UINT	LightType;	// 0 : DirectionalLight, 1 : PointLight, 2 : SpotLight
	Vec3	WorldDir;	// WorldSpace ���� ������ ����
	Vec3	WorldPos;	// WorldSpace ���� ������ ��ġ
	float	Range;		// SpotLight or PointLight �� ������ ���� ����
	float	Angle;		// Spot Light �� ��� ������ ��������

	Vec3	vPadding;
};


// Particle
struct tParticle
{
	Vec4	vColor;
	Vec3	vLocalPos;
	Vec3	vWorldPos;
	Vec3	vWorldInitScale;
	Vec3	vWorldCurrentScale;
	Vec3	vWorldRotation;
	
	Vec3	vForce;				// Particle �� ������ ��
	Vec3	vVelocity;
			
	float	NoiseForceAccTime;	// Noise Force �ҿ� �����ϴ� ���� �ð�
	Vec3	NoiseForceDir;		// Noise Force �� ����

	float	Mass;
	float	Life;
	float	Age;
	float	NomarlizedAge;
	int		Active;

	Vec2	Padding;
};

// Particle SpawnCount
struct tSpawnCount
{
	int  SpawnCount;
	UINT padding[3];
};

// Particle Module
struct tParticleModule
{
	// Spawn
	UINT	SpawnRate;				// �ʴ� ��ƼŬ �������� (Spawn Per Second)
	Vec4	vSpawnColor;			// ���� ���� ����
	Vec4	vSpawnMinScale;			// ���� �� �ּ� ũ��
	Vec4	vSpawnMaxScale;			// ���� �� �ִ� ũ��
		
	float	MinLife;				// �ּ� ����
	float	MaxLife;				// �ִ� ����

	UINT	SpawnShape;				// 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// �ѹ��� �߻���Ű�� Particle ��
	UINT	SpawnBurstRepeat;
	float	SpawnBurstRepeatTime;

	// Add Velocity
	UINT	AddVelocityType;		// 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;
	float	AddMaxSpeed;

	// Scale Module
	float	StartScale;
	float	EndScale;

	// Drag Module
	float	DestNormalizedAge;
	float	LimitSpeed;

	// Noise Force Module
	float	NoiseForceTerm;		// Noise Force �����Ű�� ��
	float	NoiseForceScale;	// Noise Force ũ��

	// Render Module
	Vec3	EndColor;			// ���� ����
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut ȿ���� ���۵Ǵ� Normalized Age ����
	UINT	VelocityAlignment;	// �ӵ� ���� 0 : Off, 1 : On


	// Module On / Off
	int		Module[(UINT)PARTICLE_MODULE::END];
};

struct tRay
{
	Vec3	vStart;
	Vec3	vDir;
};

// ===================
// ������� ���� ����ü
// ===================
struct tTransform
{
	Matrix	matWorld;
	Matrix	matWorldInv;
	Matrix	matView;
	Matrix	matViewInv;
	Matrix  matProj;
	Matrix  matProjInv;

	Matrix	matWV;
	Matrix  matWVP;
};
extern tTransform g_Trans;


struct tMtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	matArr[4];
	UINT	btex[16];
};

struct tAnim2DInfo
{
	Vec2	vLeftTop;
	Vec2	vSliceSize;

	Vec2	vOffset;
	Vec2	vBackground;

	int		UseAnim2D;
	int		padding[3];
};

struct tGlobalData
{
	Vec2	vResolution;	// ������ �ػ�

	float	DeltaTime;		// DT (���� ���¿� ������ ����)
	float	Time;			// ���� �������� ���� �ð�(���� ���¿� ������ ����)

	float	DeltaTimeEngine;	// ���� ���¿� ������� ����
	float	TimeEngine;			// ���� ���¿� ������� ����

	int		Light2DCount;
	int		Light3DCount;	
};

extern tGlobalData g_GlobalData;
