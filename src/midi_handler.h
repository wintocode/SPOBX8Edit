#pragma once
#include <clap/clap.h>
#include <vector>
#include <functional>
#include <queue>

struct MidiMessage {
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
    uint32_t timestamp;
};

struct NRPNMessage {
    uint16_t parameter;
    uint16_t value;
    uint32_t timestamp;
    bool is_complete;
};

class MidiHandler {
public:
    MidiHandler();
    ~MidiHandler();
    
    // MIDI message processing
    void processMidiMessage(const MidiMessage& message);
    void processNRPNMessage(const NRPNMessage& nrpn);
    
    // NRPN handling
    void sendNRPN(uint16_t parameter, uint16_t value);
    bool hasNRPNMessage() const;
    NRPNMessage popNRPNMessage();
    
    // CC handling
    void sendCC(uint8_t cc, uint8_t value);
    
    // Callbacks
    void setNRPNCallback(std::function<void(uint16_t, uint16_t)> callback);
    void setCCCallback(std::function<void(uint8_t, uint8_t)> callback);
    
    // Queue management
    void getOutgoingMessages(std::vector<MidiMessage>& messages);
    void clearOutgoingMessages();
    
private:
    // NRPN state machine
    enum NRPNState {
        WAITING_FOR_NRPN_MSB,
        WAITING_FOR_NRPN_LSB,
        WAITING_FOR_DATA_MSB,
        WAITING_FOR_DATA_LSB,
        NRPN_COMPLETE
    };
    
    NRPNState nrpn_state_;
    uint16_t nrpn_msb_;
    uint16_t nrpn_lsb_;
    uint16_t data_msb_;
    uint16_t data_lsb_;
    
    // Message queues
    std::queue<NRPNMessage> incoming_nrpn_queue_;
    std::queue<MidiMessage> outgoing_midi_queue_;
    
    // Callbacks
    std::function<void(uint16_t, uint16_t)> nrpn_callback_;
    std::function<void(uint8_t, uint8_t)> cc_callback_;
    
    // Helper methods
    void processCC(uint8_t cc, uint8_t value);
    void processNRPNCC(uint8_t cc, uint8_t value);
    void resetNRPNState();
    bool isNRPNComplete();
    
    // MIDI CC constants
    static const uint8_t CC_NRPN_MSB = 99;
    static const uint8_t CC_NRPN_LSB = 98;
    static const uint8_t CC_DATA_MSB = 6;
    static const uint8_t CC_DATA_LSB = 38;
};