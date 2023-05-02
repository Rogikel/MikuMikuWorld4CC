#include "ApplicationConfiguration.h"
#include "StringOperations.h"
#include "JsonIO.h"
#include <filesystem>
#include <fstream>

using namespace nlohmann;

namespace MikuMikuWorld
{
	constexpr const char* CONFIG_VERSION{ "1.4.0" };

	ApplicationConfiguration::ApplicationConfiguration() : version{ CONFIG_VERSION }
	{
		restoreDefault();
	}

	void ApplicationConfiguration::read(const std::string& filename)
	{
		std::wstring wFilename = mbToWideStr(filename);
		if (!std::filesystem::exists(wFilename))
			return;

		std::ifstream configFile(wFilename);
		json config;
		configFile >> config;
		configFile.close();

		version = jsonIO::tryGetValue<std::string>(config, "version", "1.0");
		if (!version.size())
			version = "1.0.0";

		if (jsonIO::keyExists(config, "window"))
		{
			const json& window = config["window"];
			maximized = jsonIO::tryGetValue<bool>(window, "maximized", false);
			vsync = jsonIO::tryGetValue<bool>(window, "vsync", true);

			windowPos = jsonIO::tryGetValue(window, "position", Vector2{});
			if (windowPos.x <= 0) windowPos.x = 150;
			if (windowPos.y <= 0) windowPos.y = 100;

			windowSize = jsonIO::tryGetValue(window, "size", Vector2{});
			if (windowSize.x <= 0 || windowSize.y <= 0)
			{
				windowSize.x = 1200;
				windowSize.y = 800;
			}
		}

		if (jsonIO::keyExists(config, "timeline"))
		{
			timelineWidth = jsonIO::tryGetValue<int>(config["timeline"], "lane_width", 30);
			notesHeight = jsonIO::tryGetValue<int>(config["timeline"], "notes_height", 35);
			division = jsonIO::tryGetValue<int>(config["timeline"], "division", 8);
			zoom = jsonIO::tryGetValue<float>(config["timeline"], "zoom", 2.0f);
			laneOpacity = jsonIO::tryGetValue<float>(config["timeline"], "lane_opacity", 0.6f);
			backgroundBrightness = jsonIO::tryGetValue<float>(config["timeline"], "background_brightness", 0.5f);

			useSmoothScrolling = jsonIO::tryGetValue<bool>(config["timeline"], "smooth_scrolling_enable", true);
			smoothScrollingTime = jsonIO::tryGetValue<float>(config["timeline"], "smooth_scrolling_time", 67.0f);

			scrollMode = jsonIO::tryGetValue<std::string>(config["timeline"], "scroll_mode");
			if (!scrollMode.size())
				scrollMode = "follow_cursor";
		}

		if (jsonIO::keyExists(config, "theme"))
		{
			accentColor = jsonIO::tryGetValue<int>(config["theme"], "accent_color", 1);
			userColor = jsonIO::tryGetValue(config["theme"], "user_color", Color{});
			baseTheme = UI::intToBaseTheme(jsonIO::tryGetValue<int>(config["theme"], "base_theme", 0));
		}

		if (jsonIO::keyExists(config, "save"))
		{
			autoSaveEnabled	= jsonIO::tryGetValue<bool>(config["save"], "auto_save_enabled", true);
			autoSaveInterval = jsonIO::tryGetValue<int>(config["save"], "auto_save_interval", 5);
			autoSaveMaxCount = jsonIO::tryGetValue<int>(config["save"], "auto_save_max_count", 100);
		}

		if (jsonIO::keyExists(config, "audio"))
		{
			masterVolume	= std::clamp(jsonIO::tryGetValue<float>(config["audio"], "master_volume", 0.8f), 0.0f, 1.0f);
			bgmVolume		= std::clamp(jsonIO::tryGetValue<float>(config["audio"], "bgm_volume", 1.0f), 0.0f, 1.0f);
			seVolume		= std::clamp(jsonIO::tryGetValue<float>(config["audio"], "se_volume", 1.0f), 0.0f, 1.0f);
		}

		if (jsonIO::keyExists(config, "input") && jsonIO::keyExists(config["input"], "bindings"))
		{
			for (auto& [key, value] : config["input"]["bindings"].items())
				keyConfigMap[key] = KeyConfiguration{ key, value };
		}
	}

	void ApplicationConfiguration::write(const std::string& filename)
	{
		json config;

		// update to latest version
		config["version"] = CONFIG_VERSION;
		config["window"]["position"] = {
			{"x", windowPos.x},
			{"y", windowPos.y}
		};

		config["window"]["size"] = {
			{"x", windowSize.x},
			{"y", windowSize.y}
		};

		config["window"]["maximized"] = maximized;
		config["window"]["vsync"] = vsync;

		config["timeline"] = {
			{"lane_width", timelineWidth},
			{"notes_height", notesHeight},
			{"division", division},
			{"zoom", zoom},
			{"lane_opacity", laneOpacity},
			{"background_brightness", backgroundBrightness},
			{"smooth_scrolling_enable", useSmoothScrolling},
			{"smooth_scrolling_time", smoothScrollingTime},
			{"scroll_mode", scrollMode}
		};

		config["theme"] = {
			{"accent_color", accentColor},
			{"user_color",
				{
					{"r", userColor.r},
					{"g", userColor.g},
					{"b", userColor.b},
					{"a", userColor.a}
				}
			},
			{
				"base_theme", UI::baseThemeToInt(baseTheme)
			}
		};

		config["save"] = {
			{"auto_save_enabled", autoSaveEnabled},
			{"auto_save_interval", autoSaveInterval},
			{"auto_save_max_count", autoSaveMaxCount}
		};

		config["audio"] = {
			{"master_volume", masterVolume},
			{"bgm_volume", bgmVolume},
			{"se_volume", seVolume}
		};

		json bindings;
		for (const auto& [_, command] : keyConfigMap)
			bindings[command.commandName] = command.keyBindings;

		config["input"] = {
			{"bindings", bindings}
		};

		std::wstring wFilename = mbToWideStr(filename);
		std::ofstream configFile(wFilename);
		configFile << std::setw(4) << config;
		configFile.flush();
		configFile.close();
	}

	void ApplicationConfiguration::restoreDefault()
	{
		windowPos = Vector2(150, 100);
		windowSize = Vector2(1200, 800);
		maximized = false;
		vsync = true;
		accentColor = 1;
		userColor = Color(0.2f, 0.2f, 0.2f, 1.0f);

		timelineWidth = 30;
		notesHeight = 35;
		division = 8;
		zoom = 2.0f;
		laneOpacity = 0.6f;
		backgroundBrightness = 0.5f;
		useSmoothScrolling = true;
		smoothScrollingTime = 67.0f;
		scrollMode = "follow_cursor";

		autoSaveEnabled = true;
		autoSaveInterval = 5;
		autoSaveMaxCount = 100;

		masterVolume = 0.8f;
		bgmVolume = 1.0f;
		seVolume = 1.0f;
	}
}