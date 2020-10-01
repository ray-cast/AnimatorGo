#include "time_module.h"

namespace rabbit
{
	TimeModule::TimeModule() noexcept
	{
		this->reset();
	}

	TimeModule::~TimeModule() noexcept
	{
	}

	void
	TimeModule::reset() noexcept
	{
		this->playing_ = false;
		this->spp = 100;
		this->recordFps = 30.0f;
		this->playFps = 30.0f;
		this->playIterationCounts = 4;
		this->normalIterationCounts = 1;
		this->endFrame = 0;
		this->startFrame = 0;
		this->timeLength = 0;
		this->curTime = 0;
	}

	void 
	TimeModule::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("spp") != reader.end())
			this->spp = reader["spp"];
		if (reader.find("recordFps") != reader.end())
			this->recordFps = reader["recordFps"];
		if (reader.find("playFps") != reader.end())
			this->playFps = reader["playFps"];
		if (reader.find("playIterationCounts") != reader.end())
			this->playIterationCounts = reader["playIterationCounts"];
		if (reader.find("normalIterationCounts") != reader.end())
			this->normalIterationCounts = reader["normalIterationCounts"];
	}

	void 
	TimeModule::save(octoon::runtime::json& writer) noexcept
	{
		writer["spp"] = this->spp;
		writer["recordFps"] = this->recordFps;
		writer["playFps"] = this->playFps;
		writer["playIterationCounts"] = this->playIterationCounts;
		writer["normalIterationCounts"] = this->normalIterationCounts;
	}
}