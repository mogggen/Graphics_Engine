#pragma once
#include "core/MathLibrary.h"

struct Vertex
{
	V3 pos;
	V4 tangentBiTangentSignedScalar;
	V2 texel;
	V3 normal;
};