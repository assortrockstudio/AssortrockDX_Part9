#pragma once

#include "CRenderComponent.h"

class CDecal :
    public CRenderComponent
{
private:

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    CDecal();
    ~CDecal();
};

