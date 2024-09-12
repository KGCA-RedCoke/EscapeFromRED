#pragma once

#include <d2d1.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <dwrite.h>
#include <wrl.h>

#include "Debug/Assert.h"
#include "Core/Utils/Math/Vector4.h"
#include "Core/Utils/Math/Vector.h"
#include "Core/Utils/Math/TMatrix.h"

using namespace Microsoft::WRL;
using namespace DirectX;

#define M_RightVector g_XMIdentityR0
#define M_UpVector g_XMIdentityR1
#define M_ForwardVector g_XMIdentityR2
