#pragma once
#include "common_include.h"

class IManagedInterface
{
public:
	virtual ~IManagedInterface() = default;

public:
	virtual uint32_t               GetHash() const = 0;
	virtual Ptr<IManagedInterface> Clone() const = 0;
};
