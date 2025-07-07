#include "obx8_plugin.h"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

const clap_plugin_descriptor_t obx8_plugin_descriptor = {
    .clap_version = CLAP_VERSION_INIT,
    .id = "com.spobx8.edit",
    .name = "SPOBX8Edit",
    .vendor = "SPOBX8",
    .url = "",
    .manual_url = "",
    .support_url = "",
    .version = "1.0.0",
    .description = "Hardware editor for Oberheim OBX8 synthesizer",
    .features = (const char *[]){
        CLAP_PLUGIN_FEATURE_INSTRUMENT,
        CLAP_PLUGIN_FEATURE_UTILITY,
        nullptr
    }
};

OBX8Plugin::OBX8Plugin(const clap_host_t *host) 
    : host_(host)
    , param_manager_(std::make_unique<OBX8ParameterManager>())
    , midi_handler_(std::make_unique<MidiHandler>())
    , midi_device_manager_(std::make_unique<MidiDeviceManager>())
    , sample_rate_(44100.0)
    , is_active_(false)
    , is_processing_(false)
    , gui_created_(false)
    , gui_scale_(1.0)
    , gui_width_(800)
    , gui_height_(600) {
    
    initializeParameters();
    
    // Set up MIDI callbacks
    midi_handler_->setNRPNCallback([this](uint16_t parameter, uint16_t value) {
        onNRPNReceived(parameter, value);
    });
    
    midi_handler_->setCCCallback([this](uint8_t cc, uint8_t value) {
        onCCReceived(cc, value);
    });
    
    // Set up MIDI device callback
    midi_device_manager_->setMidiReceiveCallback([this](const uint8_t* data, size_t length) {
        if (length >= 3) {
            MidiMessage msg;
            msg.status = data[0];
            msg.data1 = data[1];
            msg.data2 = data[2];
            msg.timestamp = 0;
            midi_handler_->processMidiMessage(msg);
        }
    });
    
    updateMidiDeviceList();
}

OBX8Plugin::~OBX8Plugin() {
}

bool OBX8Plugin::init() {
    return true;
}

void OBX8Plugin::destroy() {
    // GUI cleanup would go here if implemented
}

bool OBX8Plugin::activate(double sample_rate, uint32_t min_frames, uint32_t max_frames) {
    sample_rate_ = sample_rate;
    is_active_ = true;
    return true;
}

void OBX8Plugin::deactivate() {
    is_active_ = false;
}

bool OBX8Plugin::start_processing() {
    is_processing_ = true;
    return true;
}

void OBX8Plugin::stop_processing() {
    is_processing_ = false;
}

void OBX8Plugin::reset() {
    // Reset plugin state
}

clap_process_status OBX8Plugin::process(const clap_process_t *process) {
    // Process incoming MIDI events
    processIncomingMidi(process->in_events);
    
    // Process outgoing MIDI events
    processOutgoingMidi(process->out_events);
    
    // This is a hardware editor, so we don't process audio
    // Just copy input to output if audio ports are connected
    if (process->audio_inputs_count > 0 && process->audio_outputs_count > 0) {
        for (uint32_t ch = 0; ch < process->audio_inputs[0].channel_count; ++ch) {
            if (ch < process->audio_outputs[0].channel_count) {
                memcpy(process->audio_outputs[0].data32[ch], 
                       process->audio_inputs[0].data32[ch], 
                       process->frames_count * sizeof(float));
            }
        }
    }
    
    return CLAP_PROCESS_CONTINUE;
}

const void* OBX8Plugin::get_extension(const char *id) {
    if (strcmp(id, CLAP_EXT_PARAMS) == 0) {
        static const clap_plugin_params_t params_ext = {
            .count = [](const clap_plugin_t *plugin) -> uint32_t {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->params_count();
            },
            .get_info = [](const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info) -> bool {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->params_get_info(param_index, param_info);
            },
            .get_value = [](const clap_plugin_t *plugin, clap_id param_id, double *value) -> bool {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->params_get_value(param_id, value);
            },
            .value_to_text = [](const clap_plugin_t *plugin, clap_id param_id, double value, char *display, uint32_t size) -> bool {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->params_value_to_text(param_id, value, display, size);
            },
            .text_to_value = [](const clap_plugin_t *plugin, clap_id param_id, const char *display, double *value) -> bool {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->params_text_to_value(param_id, display, value);
            },
            .flush = [](const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out) {
                static_cast<OBX8Plugin*>(plugin->plugin_data)->params_flush(in, out);
            }
        };
        return &params_ext;
    }
    
    if (strcmp(id, CLAP_EXT_NOTE_PORTS) == 0) {
        static const clap_plugin_note_ports_t note_ports_ext = {
            .count = [](const clap_plugin_t *plugin, bool is_input) -> uint32_t {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->note_ports_count(is_input);
            },
            .get = [](const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_note_port_info_t *info) -> bool {
                return static_cast<const OBX8Plugin*>(plugin->plugin_data)->note_ports_get(index, is_input, info);
            }
        };
        return &note_ports_ext;
    }
    
    return nullptr;
}

void OBX8Plugin::on_main_thread() {
    // Handle main thread callbacks
}

void OBX8Plugin::initializeParameters() {
    param_values_.resize(param_manager_->getParameterCount());
    
    for (uint32_t i = 0; i < param_manager_->getParameterCount(); ++i) {
        const auto& params = param_manager_->getParameters();
        if (i < params.size()) {
            param_values_[i] = normalizeParameterValue(&params[i], params[i].default_value);
        }
    }
}

uint32_t OBX8Plugin::params_count() const {
    uint32_t count = param_manager_->getParameterCount();
    std::ofstream debug_file("/tmp/spobx8_debug.log", std::ios::app);
    debug_file << "*** params_count() called, returning: " << count << std::endl;
    debug_file.close();
    return count;
}

bool OBX8Plugin::params_get_info(uint32_t param_index, clap_param_info_t *param_info) const {
    const auto& params = param_manager_->getParameters();
    if (param_index >= params.size()) {
        return false;
    }
    
    const auto& param = params[param_index];
    
    param_info->id = param.id;
    strncpy(param_info->name, param.display_name.c_str(), sizeof(param_info->name) - 1);
    param_info->name[sizeof(param_info->name) - 1] = '\0';
    strncpy(param_info->module, "OBX8", sizeof(param_info->module) - 1);
    param_info->module[sizeof(param_info->module) - 1] = '\0';
    
    param_info->min_value = 0.0;
    param_info->max_value = 1.0;
    param_info->default_value = normalizeParameterValue(&param, param.default_value);
    
    param_info->flags = CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_IS_MODULATABLE;
    if (param.is_stepped) {
        param_info->flags |= CLAP_PARAM_IS_STEPPED;
    }
    
    return true;
}

bool OBX8Plugin::params_get_value(clap_id param_id, double *value) const {
    if (param_id >= param_values_.size()) {
        return false;
    }
    
    *value = param_values_[param_id];
    return true;
}

bool OBX8Plugin::params_value_to_text(clap_id param_id, double value, char *display, uint32_t size) const {
    const OBX8Parameter* param = param_manager_->getParameterById(param_id);
    if (!param) {
        return false;
    }
    
    double actual_value = denormalizeParameterValue(param, value);
    
    if (param->is_stepped && !param->step_names.empty()) {
        int step = static_cast<int>(actual_value);
        if (step >= 0 && step < static_cast<int>(param->step_names.size())) {
            strncpy(display, param->step_names[step].c_str(), size - 1);
            display[size - 1] = '\0';
            return true;
        }
    }
    
    snprintf(display, size, "%.2f%s", actual_value, param->unit.c_str());
    return true;
}

bool OBX8Plugin::params_text_to_value(clap_id param_id, const char *display, double *value) const {
    const OBX8Parameter* param = param_manager_->getParameterById(param_id);
    if (!param) {
        return false;
    }
    
    // Try to parse as number first
    double parsed_value = atof(display);
    *value = normalizeParameterValue(param, parsed_value);
    return true;
}

void OBX8Plugin::params_flush(const clap_input_events_t *in, const clap_output_events_t *out) {
    std::ofstream debug_file("/tmp/spobx8_debug.log", std::ios::app);
    debug_file << "=== *** PARAMS_FLUSH *** called ===" << std::endl;
    
    uint32_t event_count = in->size(in);
    debug_file << "Event count: " << event_count << std::endl;
    
    for (uint32_t i = 0; i < event_count; ++i) {
        const clap_event_header_t *header = in->get(in, i);
        debug_file << "Event " << i << " type: " << header->type << std::endl;
        
        if (header->type == CLAP_EVENT_PARAM_VALUE) {
            const clap_event_param_value_t *param_event = 
                reinterpret_cast<const clap_event_param_value_t*>(header);
            
            debug_file << "Parameter event - ID: " << param_event->param_id 
                      << ", value: " << param_event->value << std::endl;
            
            handleParameterChange(param_event->param_id, param_event->value);
        }
    }
    
    debug_file << "=== params_flush end ===" << std::endl;
    debug_file.close();
}

uint32_t OBX8Plugin::note_ports_count(bool is_input) const {
    return 1; // One MIDI port for both input and output
}

bool OBX8Plugin::note_ports_get(uint32_t index, bool is_input, clap_note_port_info_t *info) const {
    if (index != 0) {
        return false;
    }
    
    info->id = 0;
    strncpy(info->name, is_input ? "MIDI In" : "MIDI Out", sizeof(info->name) - 1);
    info->name[sizeof(info->name) - 1] = '\0';
    info->supported_dialects = CLAP_NOTE_DIALECT_MIDI;
    info->preferred_dialect = CLAP_NOTE_DIALECT_MIDI;
    
    return true;
}

void OBX8Plugin::handleParameterChange(clap_id param_id, double value) {
    // Write to debug file
    std::ofstream debug_file("/tmp/spobx8_debug.log", std::ios::app);
    debug_file << "=== handleParameterChange called ===" << std::endl;
    debug_file << "param_id: " << param_id << ", value: " << value << std::endl;
    
    if (param_id < param_values_.size()) {
        param_values_[param_id] = value;
        
        if (param_id == MIDI_DEVICE_SELECTION) {
            debug_file << "Calling onMidiDeviceSelected" << std::endl;
            onMidiDeviceSelected(param_id, value);
        } else {
            debug_file << "Calling sendParameterToHardware" << std::endl;
            sendParameterToHardware(param_id, value);
        }
    } else {
        debug_file << "param_id out of range: " << param_id << " >= " << param_values_.size() << std::endl;
    }
    
    debug_file << "=== handleParameterChange end ===" << std::endl;
    debug_file.close();
}

void OBX8Plugin::sendParameterToHardware(clap_id param_id, double value) {
    // Write to debug file
    std::ofstream debug_file("/tmp/spobx8_debug.log", std::ios::app);
    debug_file << "=== sendParameterToHardware called ===" << std::endl;
    debug_file << "param_id: " << param_id << ", value: " << value << std::endl;
    
    const OBX8Parameter* param = param_manager_->getParameterById(param_id);
    if (!param || !midi_device_manager_->isConnected()) {
        debug_file << "Not sending - param: " << (param ? "ok" : "null") 
                  << ", connected: " << (midi_device_manager_->isConnected() ? "yes" : "no") << std::endl;
        debug_file.close();
        return;
    }
    
    // Convert normalized value to NRPN value
    uint16_t nrpn_value = parameterToNRPNValue(param, value);
    uint16_t nrpn_param = (param->nrpn_msb << 7) | param->nrpn_lsb;
    
    debug_file << "Sending NRPN - param: " << param->display_name 
              << ", NRPN: " << nrpn_param << ", value: " << nrpn_value << std::endl;
    
    // Send NRPN to hardware via MIDI device manager
    midi_handler_->sendNRPN(nrpn_param, nrpn_value);
    
    // Get outgoing MIDI messages and send them through device manager
    std::vector<MidiMessage> messages;
    midi_handler_->getOutgoingMessages(messages);
    
    debug_file << "Got " << messages.size() << " MIDI messages" << std::endl;
    
    for (const auto& msg : messages) {
        uint8_t midi_data[3] = {msg.status, msg.data1, msg.data2};
        debug_file << "MIDI out: " << std::hex << (int)msg.status 
                  << " " << (int)msg.data1 << " " << (int)msg.data2 << std::dec << std::endl;
        bool sent = midi_device_manager_->sendMidiData(midi_data, 3);
        debug_file << "Send result: " << (sent ? "success" : "failed") << std::endl;
    }
    
    debug_file << "=== sendParameterToHardware end ===" << std::endl;
    debug_file.close();
}

void OBX8Plugin::processIncomingMidi(const clap_input_events_t *in_events) {
    std::ofstream debug_file("/tmp/spobx8_debug.log", std::ios::app);
    debug_file << "=== processIncomingMidi called ===" << std::endl;
    
    uint32_t event_count = in_events->size(in_events);
    debug_file << "Event count: " << event_count << std::endl;
    
    for (uint32_t i = 0; i < event_count; ++i) {
        const clap_event_header_t *header = in_events->get(in_events, i);
        debug_file << "Event " << i << " type: " << header->type << std::endl;
        
        if (header->type == CLAP_EVENT_MIDI) {
            const clap_event_midi_t *midi_event = 
                reinterpret_cast<const clap_event_midi_t*>(header);
            
            MidiMessage msg;
            msg.status = midi_event->data[0];
            msg.data1 = midi_event->data[1];
            msg.data2 = midi_event->data[2];
            msg.timestamp = header->time;
            
            midi_handler_->processMidiMessage(msg);
            debug_file << "Processed MIDI event" << std::endl;
        } else if (header->type == CLAP_EVENT_PARAM_VALUE) {
            const clap_event_param_value_t *param_event = 
                reinterpret_cast<const clap_event_param_value_t*>(header);
            
            debug_file << "Parameter event - ID: " << param_event->param_id 
                      << ", value: " << param_event->value << std::endl;
            
            handleParameterChange(param_event->param_id, param_event->value);
        }
    }
    
    debug_file << "=== processIncomingMidi end ===" << std::endl;
    debug_file.close();
}

void OBX8Plugin::processOutgoingMidi(const clap_output_events_t *out_events) {
    std::vector<MidiMessage> messages;
    midi_handler_->getOutgoingMessages(messages);
    
    for (const auto& msg : messages) {
        clap_event_midi_t midi_event;
        midi_event.header.size = sizeof(clap_event_midi_t);
        midi_event.header.time = msg.timestamp;
        midi_event.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        midi_event.header.type = CLAP_EVENT_MIDI;
        midi_event.header.flags = 0;
        midi_event.port_index = 0;
        midi_event.data[0] = msg.status;
        midi_event.data[1] = msg.data1;
        midi_event.data[2] = msg.data2;
        
        out_events->try_push(out_events, &midi_event.header);
    }
}

void OBX8Plugin::onNRPNReceived(uint16_t parameter, uint16_t value) {
    // Find parameter by NRPN
    uint16_t nrpn_msb = (parameter >> 7) & 0x7F;
    uint16_t nrpn_lsb = parameter & 0x7F;
    
    const OBX8Parameter* param = param_manager_->getParameterByNRPN(nrpn_msb, nrpn_lsb);
    if (param) {
        double normalized_value = nrpnToParameterValue(param, value);
        param_values_[param->id] = normalized_value;
        
        // Notify host of parameter change
        if (host_ && host_->request_callback) {
            host_->request_callback(host_);
        }
    }
}

void OBX8Plugin::onCCReceived(uint8_t cc, uint8_t value) {
    const OBX8Parameter* param = param_manager_->getParameterByCC(cc);
    if (param) {
        double normalized_value = static_cast<double>(value) / 127.0;
        param_values_[param->id] = normalized_value;
        
        // Notify host of parameter change
        if (host_ && host_->request_callback) {
            host_->request_callback(host_);
        }
    }
}

double OBX8Plugin::normalizeParameterValue(const OBX8Parameter* param, double value) const {
    return (value - param->min_value) / (param->max_value - param->min_value);
}

double OBX8Plugin::denormalizeParameterValue(const OBX8Parameter* param, double normalized) const {
    return param->min_value + normalized * (param->max_value - param->min_value);
}

uint16_t OBX8Plugin::parameterToNRPNValue(const OBX8Parameter* param, double value) {
    double actual_value = denormalizeParameterValue(param, value);
    
    // Use the actual parameter range from the OBX8 manual, not 14-bit range
    // The manual specifies exact ranges for each parameter
    return static_cast<uint16_t>(std::round(actual_value));
}

double OBX8Plugin::nrpnToParameterValue(const OBX8Parameter* param, uint16_t nrpn_value) {
    // Convert from hardware value directly to parameter value
    double actual_value = static_cast<double>(nrpn_value);
    
    // Clamp to parameter range
    actual_value = std::max(param->min_value, std::min(param->max_value, actual_value));
    
    return normalizeParameterValue(param, actual_value);
}

void OBX8Plugin::onMidiDeviceSelected(clap_id param_id, double value) {
    if (param_id != MIDI_DEVICE_SELECTION) {
        return;
    }
    
    auto device_names = midi_device_manager_->getDeviceNames();
    int device_index = static_cast<int>(denormalizeParameterValue(
        param_manager_->getParameterById(param_id), value));
    
    if (device_index >= 0 && device_index < static_cast<int>(device_names.size())) {
        std::string selected_device = device_names[device_index];
        
        if (selected_device == "Auto-detect OBX8") {
            // Find first OBX8 device
            midi_device_manager_->refreshDeviceList();
            const auto& devices = midi_device_manager_->getDevices();
            
            for (const auto& device : devices) {
                std::string lower_name = device.name;
                std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                if (lower_name.find("obx") != std::string::npos || 
                    lower_name.find("oberheim") != std::string::npos) {
                    midi_device_manager_->selectDevice(device.name);
                    return;
                }
            }
            
            // If no OBX8 found, select none
            midi_device_manager_->selectDevice("None");
        } else {
            midi_device_manager_->selectDevice(selected_device);
        }
    }
}

void OBX8Plugin::updateMidiDeviceList() {
    midi_device_manager_->refreshDeviceList();
    
    // Update the parameter's step names to reflect current devices
    std::vector<std::string> device_names = midi_device_manager_->getDeviceNames();
    param_manager_->updateParameterStepNames(MIDI_DEVICE_SELECTION, device_names);
}