#pragma once

#include <vector>
#include <fstream>
#include <sstream>

enum ObjectType: uint8_t {
	HITOBJECT_CIRCLE = 1,
	HITOBJECT_SLIDER = 2,
	HITOBJECT_NEW_COMBO = 4,
	HITOBJECT_SPINNER = 8
};

struct hitobject {
	uint8_t type;
	uint16_t x;
	uint16_t y;
	int32_t start_time;
	int32_t end_time;
	uint32_t repeat;
	uint32_t pixel_length;

	bool IsCircle() const {
		return (type & HITOBJECT_CIRCLE) == HITOBJECT_CIRCLE;
	}

	bool IsSlider() const {
		return (type & HITOBJECT_SLIDER) == HITOBJECT_SLIDER;
	}

	bool IsSpinner() const {
		return (type & HITOBJECT_SPINNER) == HITOBJECT_SPINNER;
	}
};

struct timingpoint {
	uint32_t offset;
	float velocity;
	float ms_per_beat;
};

class beatmap {
	private:
		float slider_multiplier;

		bool GetTimingPointFromOffset(uint32_t offset, timingpoint& target_point);
		bool ParseTimingPoint(std::vector<std::wstring>& values);
		bool ParseHitObject(std::vector<std::wstring>& values);
		bool ParseDifficultySettings(std::wstring difficulty_line);
	public:
		bool Parse(std::wstring filename);

		std::vector<hitobject> hitobjects;
		std::vector<timingpoint> timingpoints;
};