#include "player_module.h"

namespace unreal
{
	PlayerModule::PlayerModule() noexcept
	{
		this->reset();
	}

	PlayerModule::~PlayerModule() noexcept
	{
	}

	void
	PlayerModule::reset() noexcept
	{
		this->finish = true;
		this->isPlaying = false;
		this->playFps = 30.0f;
		this->recordFps = 30.0f;
		this->previewFps = 30.f;
		this->endFrame = 0;
		this->startFrame = 0;
		this->timeLength = 0;
		this->curTime = 0;
		this->syncTime = 0.1f; // 100ms
		this->takeupTime = 0;
		this->estimatedTime = 0;
	}

	void 
	PlayerModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["finish"].is_boolean())
			this->finish = reader["finish"].get<nlohmann::json::boolean_t>();
		if (reader["playFps"].is_number_float())
			this->playFps = reader["playFps"].get<nlohmann::json::number_float_t>();
		if (reader["recordFps"].is_number_float())
			this->recordFps = reader["recordFps"].get<nlohmann::json::number_float_t>();
		if (reader["previewFps"].is_number_float())
			this->previewFps = reader["previewFps"].get<nlohmann::json::number_float_t>();
	}

	void 
	PlayerModule::save(nlohmann::json& writer) noexcept
	{
		writer["finish"] = this->finish.getValue();
		writer["playFps"] = this->playFps.getValue();
		writer["recordFps"] = this->recordFps.getValue();
		writer["previewFps"] = this->previewFps.getValue();
	}

	void
	PlayerModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->finish.disconnect();
		this->isPlaying.disconnect();
		this->recordFps.disconnect();
		this->previewFps.disconnect();
		this->endFrame.disconnect();
		this->startFrame.disconnect();
		this->timeLength.disconnect();
		this->curTime.disconnect();
		this->takeupTime.disconnect();
		this->estimatedTime.disconnect();
	}
}