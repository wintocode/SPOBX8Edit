#include "obx8_parameters.h"

OBX8ParameterManager::OBX8ParameterManager() {
    // Oscillator 1 parameters - Using correct OB-X8 v2 manual NRPN numbers
    addParameter(OSC1_FREQUENCY, "osc1_frequency", "Osc 1 Frequency", 0, 1, 16, 0.0, 63.0, 32.0, "");
    addParameter(OSC1_WAVEFORM, "osc1_waveform", "Osc 1 Waveform", 0, 5, 17, 0.0, 3.0, 0.0, "", true,
                {"Sawtooth", "Triangle", "Pulse", "Sine"});
    addParameter(OSC1_PULSE_WIDTH, "osc1_pulse_width", "Osc 1 Pulse Width", 0, 7, 18, 0.0, 127.0, 64.0, "%");
    addParameter(OSC1_LEVEL, "osc1_level", "Osc 1 Level", 0, 19, 19, 0.0, 1.0, 1.0, "", true, {"Off", "On"});
    
    // Oscillator 2 parameters
    addParameter(OSC2_FREQUENCY, "osc2_frequency", "Osc 2 Frequency", 0, 2, 20, 0.0, 63.0, 32.0, "");
    addParameter(OSC2_WAVEFORM, "osc2_waveform", "Osc 2 Waveform", 0, 6, 21, 0.0, 3.0, 0.0, "", true,
                {"Sawtooth", "Triangle", "Pulse", "Sine"});
    addParameter(OSC2_PULSE_WIDTH, "osc2_pulse_width", "Osc 2 Pulse Width", 0, 8, 22, 0.0, 127.0, 64.0, "%");
    // OSC 2 DETUNE parameter (NRPN 3)
    addParameter(OSC2_DETUNE, "osc2_detune", "Osc 2 Detune", 0, 3, 23, 0.0, 63.0, 32.0, "");
    
    // OSC SYNC is a single parameter (NRPN 13) that affects both oscillators
    addParameter(OSC_SYNC, "osc_sync", "Osc Sync", 0, 13, 24, 0.0, 1.0, 0.0, "", true, {"Off", "On"});
    addParameter(OSC2_LEVEL, "osc2_level", "Osc 2 Level", 0, 20, 25, 0.0, 1.0, 1.0, "", true, {"Off", "On"});
    
    // Noise Level parameter (NRPN 21)
    addParameter(NOISE_LEVEL, "noise_level", "Noise Level", 0, 21, 26, 0.0, 1.0, 0.0, "", true, {"Off", "On"});
    
    // Filter parameters
    addParameter(FILTER_FREQUENCY, "filter_frequency", "Filter Frequency", 0, 22, 26, 0.0, 175.0, 88.0, "");
    addParameter(FILTER_RESONANCE, "filter_resonance", "Filter Resonance", 0, 23, 27, 0.0, 127.0, 0.0, "");
    addParameter(FILTER_TRACKING, "filter_tracking", "Filter Tracking", 0, 25, 28, 0.0, 1.0, 0.0, "", true, {"Off", "On"});
    addParameter(FILTER_POLE, "filter_pole", "Filter Type", 0, 24, 29, 0.0, 5.0, 0.0, "", true, {"12dB LP", "24dB LP", "6dB LP", "HP", "BP", "Notch"});
    
    // VINTAGE parameter (NRPN 26)
    addParameter(VINTAGE, "vintage", "Vintage", 0, 26, 30, 0.0, 127.0, 64.0, "");
    
    // Envelope 1 (Filter) parameters
    addParameter(ENV1_ATTACK, "env1_attack", "Filter Env Attack", 0, 60, 30, 0.0, 255.0, 0.0, "");
    addParameter(ENV1_DECAY, "env1_decay", "Filter Env Decay", 0, 62, 31, 0.0, 255.0, 64.0, "");
    addParameter(ENV1_SUSTAIN, "env1_sustain", "Filter Env Sustain", 0, 64, 32, 0.0, 127.0, 100.0, "");
    addParameter(ENV1_RELEASE, "env1_release", "Filter Env Release", 0, 66, 33, 0.0, 255.0, 64.0, "");
    
    // Envelope 2 (Volume) parameters
    addParameter(ENV2_ATTACK, "env2_attack", "Volume Env Attack", 0, 61, 34, 0.0, 255.0, 0.0, "");
    addParameter(ENV2_DECAY, "env2_decay", "Volume Env Decay", 0, 63, 35, 0.0, 255.0, 64.0, "");
    addParameter(ENV2_SUSTAIN, "env2_sustain", "Volume Env Sustain", 0, 65, 36, 0.0, 127.0, 100.0, "");
    addParameter(ENV2_RELEASE, "env2_release", "Volume Env Release", 0, 67, 37, 0.0, 255.0, 64.0, "");
    
    // LFO 1 parameters
    addParameter(LFO1_RATE, "lfo1_rate", "LFO 1 Rate", 0, 29, 38, 0.0, 127.0, 32.0, "");
    addParameter(LFO1_SHAPE, "lfo1_shape", "LFO 1 Shape", 0, 30, 39, 0.0, 4.0, 0.0, "", true,
                {"Triangle", "Sawtooth", "Pulse", "Random", "Sine"});
    addParameter(LFO1_AMOUNT, "lfo1_amount", "LFO 1 Depth 1", 0, 31, 40, 0.0, 127.0, 0.0, "");
    
    // LFO 2 parameters
    addParameter(LFO2_RATE, "lfo2_rate", "LFO 2 Rate", 0, 54, 41, 0.0, 127.0, 32.0, "");
    addParameter(LFO2_SHAPE, "lfo2_shape", "LFO 2 Shape", 0, 55, 42, 0.0, 5.0, 0.0, "", true,
                {"Triangle", "Sawtooth", "Pulse", "Random", "Sine", "Sample & Hold"});
    addParameter(LFO2_AMOUNT, "lfo2_amount", "LFO 2 Depth", 0, 58, 43, 0.0, 127.0, 0.0, "");
    
    // Filter Modulation parameter (NRPN 59)
    addParameter(FILTER_MODULATION, "filter_modulation", "Filter Modulation", 0, 59, 44, 0.0, 127.0, 0.0, "");
    
    // Master parameters
    addParameter(MASTER_VOLUME, "master_volume", "Program Volume", 0, 73, 7, 0.0, 127.0, 100.0, "");
    addParameter(MASTER_TUNE, "master_tune", "Master Tune", 1, 1, 44, -50.0, 50.0, 0.0, "cents");
    
    // MIDI Device Selection parameter (will be populated dynamically)
    addParameter(MIDI_DEVICE_SELECTION, "midi_device", "MIDI Device", 0, 30, 0, 0.0, 1.0, 0.0, "", true,
                {"None"});
    
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

void OBX8ParameterManager::updateParameterStepNames(uint32_t id, const std::vector<std::string>& step_names) {
    for (auto& param : parameters_) {
        if (param.id == id) {
            param.step_names = step_names;
            // Ensure max_value is at least 1 to avoid division by zero
            param.max_value = static_cast<double>(std::max(1, static_cast<int>(step_names.size()) - 1));
            break;
        }
    }
}