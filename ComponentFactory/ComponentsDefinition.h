#pragma once
#include <DirectX12/Common/d3dUtil.h>
#include <ECS/BaseComponent.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

class LocationComponent
	:public ECS::BaseComponent
{
public:
	union
	{
		XMFLOAT2 _xmFloat2;
		float _arr[2];
		struct
		{
			float _x, _y;
		};
	};
	
	XMFLOAT4 _quatRotation;
};

class Bound2DBoxComponent
	:public ECS::BaseComponent
{
public:
	float _maxX, _minX;
	float _maxY, _minY;
};

class LifeComponent
	:public ECS::BaseComponent
{
public:
	int _lifeValue;
};

class DamageComponent
	:public ECS::BaseComponent
{
public:
	int _damageValue;
};

class MotionComponent
	:public ECS::BaseComponent
{
public:
	XMFLOAT2 _velocity;
	XMFLOAT2 _accelaration;
};
