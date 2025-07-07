#pragma once
#include <string>
#include <vector>
#include <map>

struct OBX8Parameter {
    uint32_t id;
    std::string name;
    std::string display_name;
    uint16_t nrpn_msb;
    uint16_t nrpn_lsb;
    uint8_t midi_cc;
    double min_value;
    double max_value;
    double default_value;
    std::string unit;
    bool is_stepped;
    std::vector<std::string> step_names;
};

class OBX8ParameterManager {
public:
    OBX8ParameterManager();
    
    const std::vector<OBX8Parameter>& getParameters() const { return parameters_; }
    const OBX8Parameter* getParameterById(uint32_t id) const;
    const OBX8Parameter* getParameterByNRPN(uint16_t msb, uint16_t lsb) const;
    const OBX8Parameter* getParameterByCC(uint8_t cc) const;
    
    uint32_t getParameterCount() const { return parameters_.size(); }
    
private:
    std::vector<OBX8Parameter> parameters_;
    std::map<uint32_t, const OBX8Parameter*> id_map_;
    std::map<uint32_t, const OBX8Parameter*> nrpn_map_;
    std::map<uint8_t, const OBX8Parameter*> cc_map_;
    
    void addParameter(uint32_t id, const std::string& name, const std::string& display_name,
                     uint16_t nrpn_msb, uint16_t nrpn_lsb, uint8_t midi_cc,
                     double min_val, double max_val, double default_val,
                     const std::string& unit = "", bool stepped = false,
                     const std::vector<std::string>& step_names = {});
    
    void buildMaps();
};

// Common OBX8 parameter IDs
enum OBX8ParamID {
    // Oscillator 1
    OSC1_FREQUENCY = 0,
    OSC1_WAVEFORM,
    OSC1_PULSE_WIDTH,
    OSC1_SYNC,
    OSC1_LEVEL,
    
    // Oscillator 2
    OSC2_FREQUENCY,
    OSC2_WAVEFORM,
    OSC2_PULSE_WIDTH,
    OSC2_SYNC,
    OSC2_LEVEL,
    
    // Filter
    FILTER_FREQUENCY,
    FILTER_RESONANCE,
    FILTER_TRACKING,
    FILTER_POLE,
    
    // Envelopes
    ENV1_ATTACK,
    ENV1_DECAY,
    ENV1_SUSTAIN,
    ENV1_RELEASE,
    
    ENV2_ATTACK,
    ENV2_DECAY,
    ENV2_SUSTAIN,
    ENV2_RELEASE,
    
    // LFOs
    LFO1_RATE,
    LFO1_SHAPE,
    LFO1_AMOUNT,
    
    LFO2_RATE,
    LFO2_SHAPE,
    LFO2_AMOUNT,
    
    // Master
    MASTER_VOLUME,
    MASTER_TUNE,
    
    // MIDI Device Selection
    MIDI_DEVICE_SELECTION,
    
    PARAM_COUNT
};