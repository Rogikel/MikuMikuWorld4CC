#pragma once
#include <unordered_map>
#include <string>

namespace MikuMikuWorld
{
	extern std::unordered_map<std::string, std::string> en{
		// menu bar
		{"file", "File"},
		{"new", "New"},
		{"open", "Open"},
		{"save", "Save"},
		{"save_as", "Save As"},
		{"export", "Export"},
		{"exit", "Exit"},
		{"edit", "Edit"},
		{"undo", "Undo"},
		{"redo", "Redo"},
		{"select_all", "Select All"},
		{"settings", "Settings"},
		{"window", "Window"},
		{"vsync", "VSync"},
		{"show_performance", "Show Performance Metrics"},
		{"debug", "Debug"},
		{"create_auto_save", "Create Auto Save"},
		{"help", "Help"},
		{"about", "About"},

		// dialogs
		{"unsaved_changes", "Unsaved Changes"},
		{"ask_save", "Save current changes?"},
		{"warn_unsaved", "Any unsaved changes will be lost."},
		{"save_changes", "Save Changes"},
		{"discard_changes", "Discard Changes"},
		{"cancel", "Cancel"},
		{"general", "General"},
		{"key_config", "Key Config"},
		{"auto_save", "Auto Save"},
		{"auto_save_enable", "Auto Save Enabled"},
		{"auto_save_interval", "Auto Save Interval (min)"},
		{"auto_save_count", "Maximum Auto Save Entries"},
		{"accent_color", "Accent Color"},
		{"accent_color_help", "Select an accent color to apply. The first slot can be customized from the color controls below."},
		{"select_accent_color", "Select an accent color."},
		{"display_mode", "Display Mode"},
		{"custom_color", "Custom Color"},
		{"timeline", "Timeline"},
		{"lane_width", "Lane Width"},
		{"notes_height", "Notes Height"},
		{"use_smooth_scroll", "Use Smooth Scrolling"},
		{"smooth_scroll_time", "Smooth Scrolling Time"},
		{"background", "Background"},
		{"background_brightnes", "Background Brightness"},
		{"lanes_opacity", "Lanes Opacity"},
		{"video", "Video"},
		{"vsync_enable", "Enable VSync"},

		// score editor
		{"toolbox", "Toolbox"},
		{"controls", "Controls"},
		{"chart_properties", "Chart Properties"},
		{"notes_timeline", "Notes Timeline"},
		{"select", "Select"},
		{"tap", "Tap"},
		{"hold", "Hold"},
		{"hold_step", "Hold Mid"},
		{"flick", "Flick"},
		{"critical", "Critical"},
		{"bpm", "BPM"},
		{"time_signature", "Time Signature"},
		{"none", "None"},
		{"page", "Page"},
		{"follow_cursor", "Follow Cursor"},
		{"up", "Up"},
		{"left", "Left"},
		{"right", "Right"},
		{"visible", "Visible"},
		{"invisible", "Invisible"},
		{"ignored", "Ignored"},
		{"linear", "Linear"},
		{"ease_in", "Ease-In"},
		{"ease_out", "Ease-Out"},
		{"delete", "Delete"},
		{"copy", "Copy"},
		{"cut", "Cut"},
		{"paste", "Paste"},
		{"flip_paste", "Flip Paste"},
		{"flip", "Flip"},
		{"note_width", "Note Width"},
		{"shrink_down", "Shrink To First"},
		{"shrink_up", "Shrink To Last"},
		{"step_type", "Step Type"},
		{"ease_type", "Ease Type"},
		{"flick_type", "Flick Type"},
		{"metadata", "Metadata"},
		{"title", "Title"},
		{"designer", "Designer"},
		{"artist", "Artist"},
		{"jacket", "Jacket"},
		{"audio", "Audio"},
		{"music_file", "Music File"},
		{"music_offset", "Music Offset"},
		{"volume_master", "Master Volume"},
		{"volume_bgm", "BGM Volume"},
		{"volume_se", "SE Volume"},
		{"statistics", "Statistics"},
		{"taps", "Taps"},
		{"flicks", "Flicks"},
		{"holds", "Holds"},
		{"steps", "Hold Mids"},
		{"total", "Total"},
		{"combo", "Combo"},
		{"goto_measure", "Go To Measure"},
		{"division", "Division"},
		{"division_suffix", " Division"},
		{"custom", "Custom "},
		{"custom_division", "Custom Division"},
		{"scroll_mode", "Scroll Mode"},
		{"zoom", "Zoom"},
		{"show_step_outlines", "Show Step Outlines"},
		{"edit_bpm", "Edit Tempo"},
		{"tick", "Tick"},
		{"remove", "Remove"},
		{"edit_time_signature", "Edit Time Signature"},
		{"measure", "Measure"},
		{"skill", "Skill"},
		{"increase_note_size", "Increase Note Size"},
		{"decrease_note_size", "Decrease Note Size"},

		// preset manager
		{"presets", "Presets"},
		{"no_presets", "No presets available"},
		{"name", "Name"},
		{"description", "Description"},
		{"create_preset", "Create Preset"},
		{"search", "Search..."},
		{"confirm", "Confirm"},

		// commands
		{"action", "Action"},
		{"keys", "Keys"},
		{"add", "Add"},
		{"cmd_key_listen", "Listening for input..."},
		{"reset", "New"},
		{"toggle_playback_play_pause", "Toggle Playback (Play / Pause)"},
		{"toggle_playback_play_stop", "Toggle Playback (Play / Stop)" },
		{"cancel_paste", "Cancel Paste"},
		{"stop", "Stop"},
		{"prev_tick", "Previous Tick"},
		{"next_tick", "Next Tick"},
		{"timeline_select", "Timeline - Select"},
		{"timeline_tap", "Timeline - Insert Tap"},
		{"timeline_hold", "Timeline - Insert Hold"},
		{"timeline_hold_mid", "Timeline - Insert Hold Mid"},
		{"timeline_flick", "Timeline - Insert Flick"},
		{"timeline_make_critical", "Timeline - Insert Critical"},
		{"timeline_bpm", "Timeline - Insert BPM"},
		{"timeline_time_signature", "Timeline - Insert Time Signature"}
	};
}
