#pragma once
#include <clap/clap.h>
#include "obx8_parameters.h"
#include "midi_handler.h"
#include "midi_device_manager.h"
#include <vector>
#include <memory>

class OBX8Plugin {
public:
    OBX8Plugin(const clap_host_t *host);
    ~OBX8Plugin();
    
    // CLAP plugin interface
    bool init();
    void destroy();
    bool activate(double sample_rate, uint32_t min_frames, uint32_t max_frames);
    void deactivate();
    bool start_processing();
    void stop_processing();
    void reset();
    clap_process_status process(const clap_process_t *process);
    const void* get_extension(const char *id);
    void on_main_thread();
    
    // Parameters extension
    uint32_t params_count() const;
    bool params_get_info(uint32_t param_index, clap_param_info_t *param_info) const;
    bool params_get_value(clap_id param_id, double *value) const;
    bool params_value_to_text(clap_id param_id, double value, char *display, uint32_t size) const;
    bool params_text_to_value(clap_id param_id, const char *display, double *value) const;
    void params_flush(const clap_input_events_t *in, const clap_output_events_t *out);
    
    // Note ports extension
    uint32_t note_ports_count(bool is_input) const;
    bool note_ports_get(uint32_t index, bool is_input, clap_note_port_info_t *info) const;
    
    // State extension
    bool state_save(const clap_ostream_t *stream) const;
    bool state_load(const clap_istream_t *stream);
    
    // GUI extension
    bool gui_is_api_supported(const char *api, bool is_floating);
    bool gui_get_preferred_api(const char **api, bool *is_floating);
    bool gui_create(const char *api, bool is_floating);
    void gui_destroy();
    bool gui_set_scale(double scale);
    bool gui_get_size(uint32_t *width, uint32_t *height);
    bool gui_can_resize();
    bool gui_get_resize_hints(clap_gui_resize_hints_t *hints);
    bool gui_adjust_size(uint32_t *width, uint32_t *height);
    bool gui_set_size(uint32_t width, uint32_t height);
    bool gui_set_parent(const clap_window_t *window);
    bool gui_set_transient(const clap_window_t *window);
    void gui_suggest_title(const char *title);
    bool gui_show();
    bool gui_hide();
    
private:
    const clap_host_t *host_;
    std::unique_ptr<OBX8ParameterManager> param_manager_;
    std::unique_ptr<MidiHandler> midi_handler_;
    std::unique_ptr<MidiDeviceManager> midi_device_manager_;
    
    // Plugin state
    double sample_rate_;
    bool is_active_;
    bool is_processing_;
    
    // Parameter values
    std::vector<double> param_values_;
    
    // GUI state
    bool gui_created_;
    double gui_scale_;
    uint32_t gui_width_;
    uint32_t gui_height_;
    
    // Helper methods
    void initializeParameters();
    void handleParameterChange(clap_id param_id, double value);
    void sendParameterToHardware(clap_id param_id, double value);
    void processIncomingMidi(const clap_input_events_t *in_events);
    void processOutgoingMidi(const clap_output_events_t *out_events);
    void onNRPNReceived(uint16_t parameter, uint16_t value);
    void onCCReceived(uint8_t cc, uint8_t value);
    void onMidiDeviceSelected(clap_id param_id, double value);
    void updateMidiDeviceList();
    void autoSelectFirstOBX8Device();
    
    // Parameter conversion helpers
    double normalizeParameterValue(const OBX8Parameter* param, double value) const;
    double denormalizeParameterValue(const OBX8Parameter* param, double normalized) const;
    uint16_t parameterToNRPNValue(const OBX8Parameter* param, double value);
    double nrpnToParameterValue(const OBX8Parameter* param, uint16_t nrpn_value);
    
    // MIDI throttling for high-frequency modulation (LFO)
    std::map<clap_id, uint64_t> last_param_send_time_;
    std::map<clap_id, double> last_param_value_;
    
    // MIDI loop prevention
    bool suppress_feedback_;
    static const uint64_t MIDI_THROTTLE_MS = 5; // 5ms minimum between sends (better for LFOs)
    uint64_t getCurrentTimeMs() const;
    
};

// CLAP plugin descriptor
extern const clap_plugin_descriptor_t obx8_plugin_descriptor;