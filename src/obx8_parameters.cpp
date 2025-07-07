#include "obx8_parameters.h"

OBX8ParameterManager::OBX8ParameterManager() {
    // Oscillator 1 parameters
    addParameter(OSC1_FREQUENCY, "osc1_frequency", "Osc 1 Frequency", 0, 0, 16, -50.0, 50.0, 0.0, "cents");
    addParameter(OSC1_WAVEFORM, "osc1_waveform", "Osc 1 Waveform", 0, 1, 17, 0.0, 4.0, 0.0, "", true,
                {"Sawtooth", "Triangle", "Pulse", "Sine", "Noise"});
    addParameter(OSC1_PULSE_WIDTH, "osc1_pulse_width", "Osc 1 Pulse Width", 0, 2, 18, 0.0, 127.0, 64.0, "%");
    addParameter(OSC1_SYNC, "osc1_sync", "Osc 1 Sync", 0, 3, 19, 0.0, 1.0, 0.0, "", true, {"Off", "On"});
    addParameter(OSC1_LEVEL, "osc1_level", "Osc 1 Level", 0, 4, 20, 0.0, 127.0, 100.0, "%");
    
    // Oscillator 2 parameters
    addParameter(OSC2_FREQUENCY, "osc2_frequency", "Osc 2 Frequency", 0, 5, 21, -50.0, 50.0, 0.0, "cents");
    addParameter(OSC2_WAVEFORM, "osc2_waveform", "Osc 2 Waveform", 0, 6, 22, 0.0, 4.0, 0.0, "", true,
                {"Sawtooth", "Triangle", "Pulse", "Sine", "Noise"});
    addParameter(OSC2_PULSE_WIDTH, "osc2_pulse_width", "Osc 2 Pulse Width", 0, 7, 23, 0.0, 127.0, 64.0, "%");
    addParameter(OSC2_SYNC, "osc2_sync", "Osc 2 Sync", 0, 8, 24, 0.0, 1.0, 0.0, "", true, {"Off", "On"});
    addParameter(OSC2_LEVEL, "osc2_level", "Osc 2 Level", 0, 9, 25, 0.0, 127.0, 100.0, "%");
    
    // Filter parameters
    addParameter(FILTER_FREQUENCY, "filter_frequency", "Filter Frequency", 0, 10, 26, 0.0, 127.0, 64.0, "");
    addParameter(FILTER_RESONANCE, "filter_resonance", "Filter Resonance", 0, 11, 27, 0.0, 127.0, 0.0, "");
    addParameter(FILTER_TRACKING, "filter_tracking", "Filter Tracking", 0, 12, 28, 0.0, 127.0, 64.0, "");
    addParameter(FILTER_POLE, "filter_pole", "Filter Pole", 0, 13, 29, 0.0, 1.0, 0.0, "", true, {"2-Pole", "4-Pole"});
    
    // Envelope 1 parameters
    addParameter(ENV1_ATTACK, "env1_attack", "Env 1 Attack", 0, 14, 30, 0.0, 127.0, 0.0, "");
    addParameter(ENV1_DECAY, "env1_decay", "Env 1 Decay", 0, 15, 31, 0.0, 127.0, 64.0, "");
    addParameter(ENV1_SUSTAIN, "env1_sustain", "Env 1 Sustain", 0, 16, 32, 0.0, 127.0, 100.0, "");
    addParameter(ENV1_RELEASE, "env1_release", "Env 1 Release", 0, 17, 33, 0.0, 127.0, 64.0, "");
    
    // Envelope 2 parameters
    addParameter(ENV2_ATTACK, "env2_attack", "Env 2 Attack", 0, 18, 34, 0.0, 127.0, 0.0, "");
    addParameter(ENV2_DECAY, "env2_decay", "Env 2 Decay", 0, 19, 35, 0.0, 127.0, 64.0, "");
    addParameter(ENV2_SUSTAIN, "env2_sustain", "Env 2 Sustain", 0, 20, 36, 0.0, 127.0, 100.0, "");
    addParameter(ENV2_RELEASE, "env2_release", "Env 2 Release", 0, 21, 37, 0.0, 127.0, 64.0, "");
    
    // LFO 1 parameters
    addParameter(LFO1_RATE, "lfo1_rate", "LFO 1 Rate", 0, 22, 38, 0.0, 127.0, 32.0, "");
    addParameter(LFO1_SHAPE, "lfo1_shape", "LFO 1 Shape", 0, 23, 39, 0.0, 5.0, 0.0, "", true,
                {"Triangle", "Sawtooth", "Pulse", "Random", "Sine", "Sample & Hold"});
    addParameter(LFO1_AMOUNT, "lfo1_amount", "LFO 1 Amount", 0, 24, 40, 0.0, 127.0, 0.0, "");
    
    // LFO 2 parameters
    addParameter(LFO2_RATE, "lfo2_rate", "LFO 2 Rate", 0, 25, 41, 0.0, 127.0, 32.0, "");
    addParameter(LFO2_SHAPE, "lfo2_shape", "LFO 2 Shape", 0, 26, 42, 0.0, 5.0, 0.0, "", true,
                {"Triangle", "Sawtooth", "Pulse", "Random", "Sine", "Sample & Hold"});
    addParameter(LFO2_AMOUNT, "lfo2_amount", "LFO 2 Amount", 0, 27, 43, 0.0, 127.0, 0.0, "");
    
    // Master parameters
    addParameter(MASTER_VOLUME, "master_volume", "Master Volume", 0, 28, 7, 0.0, 127.0, 100.0, "");
    addParameter(MASTER_TUNE, "master_tune", "Master Tune", 0, 29, 44, -50.0, 50.0, 0.0, "cents");
    
    // MIDI Device Selection parameter
    addParameter(MIDI_DEVICE_SELECTION, "midi_device", "MIDI Device", 0, 30, 0, 0.0, 10.0, 0.0, "", true,
                {"None", "Auto-detect OBX8", "Device 1", "Device 2", "Device 3", "Device 4", "Device 5", "Device 6", "Device 7", "Device 8", "Device 9"});
    
    buildMaps();
}

void OBX8ParameterManager::addParameter(uint32_t id, const std::string& name, const std::string& display_name,
                                       uint16_t nrpn_msb, uint16_t nrpn_lsb, uint8_t midi_cc,
                                       double min_val, double max_val, double default_val,
                                       const std::string& unit, bool stepped,
                                       const std::vector<std::string>& step_names) {
    OBX8Parameter param;
    param.id = id;
    param.name = name;
    param.display_name = display_name;
    param.nrpn_msb = nrpn_msb;
    param.nrpn_lsb = nrpn_lsb;
    param.midi_cc = midi_cc;
    param.min_value = min_val;
    param.max_value = max_val;
    param.default_value = default_val;
    param.unit = unit;
    param.is_stepped = stepped;
    param.step_names = step_names;
    
    parameters_.push_back(param);
}

void OBX8ParameterManager::buildMaps() {
    for (const auto& param : parameters_) {
        id_map_[param.id] = &param;
        
        uint32_t nrpn_key = (param.nrpn_msb << 16) | param.nrpn_lsb;
        nrpn_map_[nrpn_key] = &param;
        
        if (param.midi_cc != 0) {
            cc_map_[param.midi_cc] = &param;
        }
    }
}

const OBX8Parameter* OBX8ParameterManager::getParameterById(uint32_t id) const {
    auto it = id_map_.find(id);
    return (it != id_map_.end()) ? it->second : nullptr;
}

const OBX8Parameter* OBX8ParameterManager::getParameterByNRPN(uint16_t msb, uint16_t lsb) const {
    uint32_t nrpn_key = (msb << 16) | lsb;
    auto it = nrpn_map_.find(nrpn_key);
    return (it != nrpn_map_.end()) ? it->second : nullptr;
}

const OBX8Parameter* OBX8ParameterManager::getParameterByCC(uint8_t cc) const {
    auto it = cc_map_.find(cc);
    return (it != cc_map_.end()) ? it->second : nullptr;
}