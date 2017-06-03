#pragma once

enum class AntiAliasingMode
{
	None,
	Regular,

	Last
};

struct AntiAliasingController
{
	AntiAliasingMode mode;
	int sampleDivision;
};

const char* antiAliasingModeToString(AntiAliasingMode mode);