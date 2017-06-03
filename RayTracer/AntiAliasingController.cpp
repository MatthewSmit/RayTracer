#include "AntiAliasingController.h"

const char* antiAliasingModeToString(AntiAliasingMode mode)
{
	switch (mode)
	{
	case AntiAliasingMode::None:
		return "None";
	case AntiAliasingMode::Regular:
		return "Regular";
	default:
		return "Unknown";
	}
}
