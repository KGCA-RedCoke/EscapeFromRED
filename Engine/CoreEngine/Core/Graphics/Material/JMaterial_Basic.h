#pragma once
#include "JMaterial.h"

class JMaterial_Basic : public JMaterial
{
public:
	JMaterial_Basic(JTextView InName = "Material_Basic");

private:
	void InitializeParams() override;
};
