#include "Application.h"
#include "ResourceManager.h"
#include "StringOperations.h"
#include "InputListener.h"
#include "Colors.h"
#include "Color.h"
#include "UI.h"
#include "Utilities.h"
#include "Localization.h"
#include "tinyfiledialogs.h"
#include "Clipboard.h"
#include <filesystem>

namespace MikuMikuWorld
{
	std::string Application::version;
	std::string Application::appDir;

	Application::Application(const std::string& root) :
		lastAppTimeUpdate{ 0 }, appFrame{ 0 }, appTime{ 0 }, initialized{ false }
	{
		appDir = root;
		version = getVersion();
	}

	Result Application::initialize()
	{
		if (initialized)
			return Result(ResultStatus::Success, "App is already initialized");

		readSettings();

		Result result = initOpenGL();
		if (!result.isOk())
			return result;

		result = imgui.initialize(window);
		if (!result.isOk())
			return result;

		renderer = new Renderer();
		editor = new ScoreEditor();

		// apply config settings
		editor->setDivision(config.division);
		editor->setScrollMode(config.scrollMode);
		editor->canvas.setLaneWidth(config.timelineWidth);
		editor->canvas.setNotesHeight(config.notesHeight);
		editor->canvas.setZoom(config.zoom);
		editor->canvas.setUseSmoothScrolling(config.useSmoothScrolling);
		editor->canvas.setSmoothScrollingTime(config.smoothScrollingTime);
		editor->canvas.setLaneTransparency(config.timelineTransparency);
		editor->canvas.setBackgroundBrightness(config.backgroundBrightness);

		editor->audio.setMasterVolume(config.masterVolume);
		editor->audio.setBGMVolume(config.bgmVolume);
		editor->audio.setSEVolume(config.seVolume);

		editor->presetManager.loadPresets(appDir + "library/");

		imgui.applyAccentColor(config.accentColor);

		autoSave.setEditorInstance(editor);
		autoSave.readConfig(config);

		setupCommands();

		loadResources();
		int bgTex = ResourceManager::getTexture("default");
		if (bgTex != -1)
			editor->canvas.changeBackground(ResourceManager::textures[bgTex]);

		initialized = true;
		return Result::Ok();;
	}

	const std::string& Application::getAppDir()
	{
		return appDir;
	}

	const std::string& Application::getAppVersion()
	{
		return version;
	}

	void Application::frameTime()
	{
		float currentFrame = glfwGetTime();
		frameDelta = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	void Application::dispose()
	{
		if (initialized)
		{
			imgui.shutdown();
			glfwDestroyWindow(window);
			glfwTerminate();
		}
		initialized = false;
	}

	void Application::readSettings()
	{
		config.read(appDir + APP_CONFIG_FILENAME);

		windowPos = config.windowPos;
		windowSize = config.windowSize;
		maximized = config.maximized;
		vsync = config.vsync;
		UI::accentColors[0].color = ImVec4{
			config.userColor.r,
			config.userColor.g,
			config.userColor.b,
			config.userColor.a
		};
	}

	void Application::writeSettings()
	{
		config.maximized = maximized;
		config.vsync = vsync;
		config.windowPos = windowPos;
		config.windowSize = windowSize;
		config.userColor = Color{
			UI::accentColors[0].color.x,
			UI::accentColors[0].color.y,
			UI::accentColors[0].color.z,
			UI::accentColors[0].color.w
		};

		config.accentColor = imgui.getAccentColor();

		config.timelineWidth = editor->canvas.getLaneWidth();
		config.notesHeight = editor->canvas.getNotesHeight();
		config.zoom = editor->canvas.getZoom();
		config.division = editor->getDivision();
		config.scrollMode = scrollModes[(int)editor->getScrollMode()];
		config.timelineTransparency = editor->canvas.getLaneTransparency();
		config.backgroundBrightness = editor->canvas.getBackgroundBrightness();
		config.useSmoothScrolling = editor->canvas.isUseSmoothScrolling();
		config.smoothScrollingTime = editor->canvas.getSmoothScrollingTime();

		config.masterVolume = editor->audio.getMasterVolume();
		config.bgmVolume = editor->audio.getBGMVolume();
		config.seVolume = editor->audio.getSEVolume();

		commandManager.writeCommands(config);

		config.write(appDir + APP_CONFIG_FILENAME);
	}

	void Application::reset()
	{
		if (!editor->isUptoDate())
			resetting = true;
		else
			editor->reset();
	}

	void Application::open()
	{
		resetting = true;
		shouldPickScore = true;
	}

	void Application::updateDialogs()
	{
		if (aboutOpen)
		{
			ImGui::OpenPopup(MODAL_TITLE("about"));
			aboutOpen = false;
		}

		if (settingsOpen)
		{
			ImGui::OpenPopup(MODAL_TITLE("settings"));
			settingsOpen = false;
		}

		about();
		settingsDialog();

		if (resetting)
		{
			if (!editor->isUptoDate())
			{
				if (!unsavedOpen)
				{
					ImGui::OpenPopup(MODAL_TITLE("unsaved_changes"));
					unsavedOpen = true;
				}

				if (warnUnsaved())
				{
					if (pendingDropScoreFile.size())
					{
						editor->loadScore(pendingDropScoreFile);
						pendingDropScoreFile = "";
					}
					else if (shouldPickScore)
					{
						editor->open();
						shouldPickScore = false;
					}
					else
					{
						editor->reset();
					}

					resetting = false;
				}
			}
			else if (shouldPickScore)
			{
				editor->open();
				resetting = false;
				shouldPickScore = false;
			}
			else if (pendingDropScoreFile.size())
			{
				editor->loadScore(pendingDropScoreFile);
				pendingDropScoreFile = "";
				resetting = false;
			}
		}

		if (exiting)
		{
			if (!editor->isUptoDate())
			{
				if (!unsavedOpen)
				{
					ImGui::OpenPopup(MODAL_TITLE("unsaved_changes"));
					unsavedOpen = true;
				}

				if (warnUnsaved())
					glfwSetWindowShouldClose(window, 1);
			}
			else
			{
				glfwSetWindowShouldClose(window, 1);
			}
		}
	}

	void Application::appendOpenFile(std::string filename)
	{
		pendingOpenFiles.push_back(filename);
		dragDropHandled = false;
	}

	void Application::handlePendingOpenFiles()
	{
		std::string scoreFile = "";
		std::string musicFile = "";

		for (auto it = pendingOpenFiles.rbegin(); it != pendingOpenFiles.rend(); ++it)
		{
			std::string extension = File::getFileExtension(*it);
			std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

			if (extension == SUS_EXTENSION || extension == MMWS_EXTENSION)
				scoreFile = *it;
			else if (extension == ".mp3" || extension == ".wav" || extension == ".flac" || extension == ".ogg")
				musicFile = *it;

			if (scoreFile.size() && musicFile.size())
				break;
		}

		if (scoreFile.size())
		{
			resetting = true;
			pendingDropScoreFile = scoreFile;
		}

		if (musicFile.size())
			editor->loadMusic(musicFile);

		pendingOpenFiles.clear();
		dragDropHandled = true;
	}

	void Application::update()
	{
		if (!dragDropHandled)
			handlePendingOpenFiles();

		imgui.initializeLayout();

		InputListener::update(window);
		if (!ImGui::GetIO().WantCaptureKeyboard)
			commandManager.processKeyboardShortcuts();

		menubar.update(settingsOpen, aboutOpen, vsync, exiting, showPerformanceMetrics);
		updateDialogs();
		editor->update(frameDelta, renderer, &commandManager);
		commandManager.updateWindow();
		autoSave.update();

		if (glfwGetTime() - lastAppTimeUpdate >= 0.05f)
		{
			appTime = stopwatch.elapsed();
			appFrame = frameDelta;
			lastAppTimeUpdate = glfwGetTime();
		}

		if (showPerformanceMetrics)
		{
			ImGui::BeginMainMenuBar();
			ImGui::SetCursorPosX(ImGui::GetMainViewport()->WorkSize.x - 300);
			ImGui::Text("app time: %.2fms :: frame time: %.2fms (%.0fFPS)",
				appTime * 1000,
				appFrame * 1000,
				1 / appFrame
			);

			ImGui::EndMainMenuBar();
		}
	}

	void Application::loadResources()
	{
		ResourceManager::loadShader(appDir + "res/shaders/basic2d");
		ResourceManager::loadTexture(appDir + "res/textures/tex_notes.png");
		ResourceManager::loadTexture(appDir + "res/textures/tex_hold_path.png");
		ResourceManager::loadTexture(appDir + "res/textures/tex_hold_path_crtcl.png");
		ResourceManager::loadTexture(appDir + "res/textures/default.png");

		// load more languages here
		Localization::loadDefault();
		Localization::load("ja", appDir + "res/i18n/ja.csv");

		std::string locale = Utilities::getSystemLocale();
		if (!Localization::setLanguage(locale))
		{
			// fallback to default language if language resource file is not found.
			Localization::setLanguage("en");
		}
	}

	void Application::setupCommands()
	{
		commandManager.add("reset", { {CTRL, GLFW_KEY_N} }, [this] { this->reset(); });
		commandManager.add("open", { {CTRL, GLFW_KEY_O} }, [this] { this->open(); });
		commandManager.add("save", { {CTRL, GLFW_KEY_S} }, [this] { this->editor->saveAs(); });
		commandManager.add("save_as", { {CTRL | SHIFT, GLFW_KEY_S} }, [this] { this->editor->saveAs(); });
		commandManager.add("export", { {CTRL, GLFW_KEY_E} }, [this] { this->editor->exportSUS(); });
		commandManager.add("toggle_playback", { {NONE, GLFW_KEY_SPACE} }, [this] { editor->togglePlaying(); });
		commandManager.add("copy", { {CTRL, GLFW_KEY_C} }, [this] { editor->copy(); },
			[this] { return editor->isAnyNoteSelected(); });

		commandManager.add("paste", { {CTRL, GLFW_KEY_V} }, [this] { editor->paste(); },
			[] { return Clipboard::hasData(); });

		commandManager.add("flip_paste", { {CTRL | SHIFT, GLFW_KEY_V} }, [this] { editor->flipPaste(); },
			[] { return Clipboard::hasData(); });

		commandManager.add("undo", { {CTRL, GLFW_KEY_Z} }, [this] { editor->undo(); },
			[this] { return editor->history.hasUndo(); });

		commandManager.add("redo", { {CTRL, GLFW_KEY_Y} }, [this] { editor->redo(); },
			[this] { return editor->history.hasRedo(); });

		commandManager.add("select_all", { {CTRL, GLFW_KEY_A} }, [this] { editor->selectAll(); },
			[this] { return !editor->isPlaying(); });

		commandManager.add("cancel_paste", { {NONE, GLFW_KEY_ESCAPE} }, [this] { editor->cancelPaste(); });
		commandManager.add("delete", { {NONE, GLFW_KEY_DELETE} }, [this] { editor->deleteSelected(); },
			[this] { return editor->isAnyNoteSelected(); });

		commandManager.add("stop", { {NONE, GLFW_KEY_BACKSPACE} }, [this] { editor->stop(); });
		commandManager.add("flip", { {CTRL, GLFW_KEY_F} }, [this] { editor->flipSelected(); },
			[this] { return editor->isAnyNoteSelected(); });

		commandManager.add("shrink_down", { {CTRL, GLFW_KEY_H} }, [this] { editor->shrinkSelected(0); },
			[this] { return editor->isAnyNoteSelected(); });

		commandManager.add("shrink_up", { {CTRL | SHIFT, GLFW_KEY_H} }, [this] { editor->shrinkSelected(1); },
			[this] { return editor->isAnyNoteSelected(); });

		commandManager.add("prev_tick", { {NONE, GLFW_KEY_DOWN} }, [this] { editor->previousTick(); },
			[this] { return !editor->isPlaying(); });

		commandManager.add("next_tick", { {NONE, GLFW_KEY_UP} }, [this] { editor->nextTick(); },
			[this] { return !editor->isPlaying();  });

		commandManager.add("timeline_select", { {NONE, GLFW_KEY_1}, {NONE, GLFW_KEY_KP_1} },
			[this] { editor->changeMode(TimelineMode::Select); });

		commandManager.add("timeline_tap", { {NONE, GLFW_KEY_2}, {NONE, GLFW_KEY_KP_2} },
			[this] { editor->changeMode(TimelineMode::InsertTap); });

		commandManager.add("timeline_hold", { {NONE, GLFW_KEY_3}, {NONE, GLFW_KEY_KP_3} },
			[this] { editor->changeMode(TimelineMode::InsertLong); });

		commandManager.add("timeline_hold_mid", { {NONE, GLFW_KEY_4}, {NONE, GLFW_KEY_KP_4} },
			[this] { editor->changeMode(TimelineMode::InsertLongMid); });

		commandManager.add("timeline_flick", { {NONE, GLFW_KEY_5}, {NONE, GLFW_KEY_KP_5} },
			[this] { editor->changeMode(TimelineMode::InsertFlick); });

		commandManager.add("timeline_make_critical", { {NONE, GLFW_KEY_6}, {NONE, GLFW_KEY_KP_6} },
			[this] { editor->changeMode(TimelineMode::MakeCritical); });

		commandManager.add("timeline_bpm", { {NONE, GLFW_KEY_7}, {NONE, GLFW_KEY_KP_7} },
			[this] { editor->changeMode(TimelineMode::InsertBPM); });

		commandManager.add("timeline_time_signature", { {NONE, GLFW_KEY_8}, {NONE, GLFW_KEY_KP_8} },
			[this] { editor->changeMode(TimelineMode::InsertTimeSign); });

		commandManager.readCommands(config);
		menubar.setCommandManagerInstance(&commandManager);
	}

	void Application::run()
	{
		while (!glfwWindowShouldClose(window))
		{
			stopwatch.reset();

			glfwPollEvents();
			frameTime();

			imgui.begin();
			update();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			imgui.draw(window);
			glfwSwapBuffers(window);
		}

		writeSettings();
		editor->presetManager.savePresets(appDir + "library/");
		autoSave.writeConfig(config);
	}
}
