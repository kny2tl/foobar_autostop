#pragma once
#include <foobar2000.h>
#include <play_callback.h>
#include <menu_helpers.h>

#include <atomic>
#include <windows.h>

// Persist using registry (HKCU)
static constexpr const char * REG_ROOT_SUBKEY = "Software\\autostop_plugin";
static constexpr const char * REG_VALUE_NAME = "Enabled";

// Command GUID
static const GUID COMMAND_AUTOSTOP_GUID = { 0x12345678,0x1234,0x1234,{0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0} };
static constexpr const char * COMMAND_AUTOSTOP_NAME = "Autostop";

// Runtime status (thread-safe)
static std::atomic_bool g_autostop_enabled{ false };

// Simple logging to console
/* static void logf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[512];
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
    va_end(args);
    console::print("[Autostop] %s", buf);
} */

// Persist: read from registry (HKCU). Returns false, if key/value is missing or invalid
static bool load_persist_state_registry() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_ROOT_SUBKEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return false;
    DWORD type = 0, data = 0, size = sizeof(data);
    LONG r = RegQueryValueExA(hKey, REG_VALUE_NAME, nullptr, &type, reinterpret_cast<LPBYTE>(&data), &size);
    RegCloseKey(hKey);
    return (r == ERROR_SUCCESS && type == REG_DWORD && data != 0);
}

// Persist: save to registry (HKCU)
static void save_persist_state_registry(bool v) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, REG_ROOT_SUBKEY, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) return;
    DWORD data = v ? 1u : 0u;
    RegSetValueExA(hKey, REG_VALUE_NAME, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&data), sizeof(data));
    RegCloseKey(hKey);
}

// Applies stop-after-current if toggle is enabled
static void apply_stop_after_current_if_enabled() {
    if (!g_autostop_enabled.load(std::memory_order_acquire)) return;
    auto pc = playback_control::get();
    if (pc.is_valid()) pc->set_stop_after_current(true);
}

// Play callback: new_track, starting, stop
class auto_stop_playback : public play_callback_impl_base {
public:
    auto_stop_playback()
        : play_callback_impl_base(
            play_callback::flag_on_playback_new_track
          | play_callback::flag_on_playback_starting
          | play_callback::flag_on_playback_stop) {}

    void on_playback_new_track(metadb_handle_ptr) override {
        apply_stop_after_current_if_enabled();
    }

    void on_playback_starting(play_control::t_track_command, bool) override {
        apply_stop_after_current_if_enabled();
    }

    void on_playback_stop(play_control::t_stop_reason) override {
        apply_stop_after_current_if_enabled();
    }
};

// Wrapper for instance callback
class auto_stop_playback_wrapper {
public:
    auto_stop_playback_wrapper() : m_instance(nullptr) {}
    void create() {
        if (!m_instance) m_instance = new auto_stop_playback();
    }
    void destroy() {
        delete m_instance;
        m_instance = nullptr;
    }
private:
    auto_stop_playback* m_instance;
};
static auto_stop_playback_wrapper g_autostop_wrapper;

// Initquit: read persistent and callback registration
class autostop_initquit : public initquit {
public:
    void on_init() override {
        bool stored = load_persist_state_registry();
        g_autostop_enabled.store(stored, std::memory_order_release);
        g_autostop_wrapper.create();
        if (g_autostop_enabled.load(std::memory_order_acquire)) apply_stop_after_current_if_enabled();
    }
    void on_quit() override {
        g_autostop_wrapper.destroy();
    }
};
FB2K_SERVICE_FACTORY(autostop_initquit);

// Mainmenu command: persistent toggle 
class command_autostop : public mainmenu_commands {
public:
    t_uint32 get_command_count() override { return 1; }

    GUID get_command(t_uint32) override { return COMMAND_AUTOSTOP_GUID; }

    void get_name(t_uint32, pfc::string_base & out) override { out = COMMAND_AUTOSTOP_NAME; }

    GUID get_parent() override { return pfc::guid_null; }

    bool get_description(t_uint32, pfc::string_base & out) override {
        out = "Toggle automatic stop after current track (persistent)";
        return true;
    }

    void execute(t_uint32, service_ptr_t<service_base>) override {
        bool cur = g_autostop_enabled.load(std::memory_order_acquire);
        bool next = !cur;
        g_autostop_enabled.store(next, std::memory_order_release);
        save_persist_state_registry(next);
        if (next) apply_stop_after_current_if_enabled();
    }
};
FB2K_SERVICE_FACTORY(command_autostop);