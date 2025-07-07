#include "midi_handler.h"

MidiHandler::MidiHandler() : nrpn_state_(WAITING_FOR_NRPN_MSB) {
    resetNRPNState();
}

MidiHandler::~MidiHandler() {
}

void MidiHandler::processMidiMessage(const MidiMessage& message) {
    if ((message.status & 0xF0) == 0xB0) { // Control Change
        uint8_t cc = message.data1;
        uint8_t value = message.data2;
        
        if (cc == CC_NRPN_MSB || cc == CC_NRPN_LSB || cc == CC_DATA_MSB || cc == CC_DATA_LSB) {
            processNRPNCC(cc, value);
        } else {
            processCC(cc, value);
        }
    }
}

void MidiHandler::processCC(uint8_t cc, uint8_t value) {
    if (cc_callback_) {
        cc_callback_(cc, value);
    }
}

void MidiHandler::processNRPNCC(uint8_t cc, uint8_t value) {
    switch (cc) {
        case CC_NRPN_MSB:
            nrpn_msb_ = value;
            nrpn_state_ = WAITING_FOR_NRPN_LSB;
            break;
            
        case CC_NRPN_LSB:
            if (nrpn_state_ == WAITING_FOR_NRPN_LSB) {
                nrpn_lsb_ = value;
                nrpn_state_ = WAITING_FOR_DATA_MSB;
            }
            break;
            
        case CC_DATA_MSB:
            if (nrpn_state_ == WAITING_FOR_DATA_MSB) {
                data_msb_ = value;
                nrpn_state_ = WAITING_FOR_DATA_LSB;
            }
            break;
            
        case CC_DATA_LSB:
            if (nrpn_state_ == WAITING_FOR_DATA_LSB) {
                data_lsb_ = value;
                nrpn_state_ = NRPN_COMPLETE;
                
                // Complete NRPN message received
                NRPNMessage nrpn;
                nrpn.parameter = (nrpn_msb_ << 7) | nrpn_lsb_;
                nrpn.value = (data_msb_ << 7) | data_lsb_;
                nrpn.timestamp = 0; // TODO: Add proper timestamp
                nrpn.is_complete = true;
                
                incoming_nrpn_queue_.push(nrpn);
                
                if (nrpn_callback_) {
                    nrpn_callback_(nrpn.parameter, nrpn.value);
                }
                
                resetNRPNState();
            }
            break;
    }
}

void MidiHandler::processNRPNMessage(const NRPNMessage& nrpn) {
    if (nrpn_callback_) {
        nrpn_callback_(nrpn.parameter, nrpn.value);
    }
}

void MidiHandler::sendNRPN(uint16_t parameter, uint16_t value) {
    uint8_t nrpn_msb = (parameter >> 7) & 0x7F;
    uint8_t nrpn_lsb = parameter & 0x7F;
    uint8_t data_msb = (value >> 7) & 0x7F;
    uint8_t data_lsb = value & 0x7F;
    
    // Send NRPN parameter MSB
    MidiMessage msg1 = {0xB0, CC_NRPN_MSB, nrpn_msb, 0};
    outgoing_midi_queue_.push(msg1);
    
    // Send NRPN parameter LSB
    MidiMessage msg2 = {0xB0, CC_NRPN_LSB, nrpn_lsb, 0};
    outgoing_midi_queue_.push(msg2);
    
    // Send data MSB
    MidiMessage msg3 = {0xB0, CC_DATA_MSB, data_msb, 0};
    outgoing_midi_queue_.push(msg3);
    
    // Send data LSB
    MidiMessage msg4 = {0xB0, CC_DATA_LSB, data_lsb, 0};
    outgoing_midi_queue_.push(msg4);
}

void MidiHandler::sendCC(uint8_t cc, uint8_t value) {
    MidiMessage msg = {0xB0, cc, value, 0};
    outgoing_midi_queue_.push(msg);
}

bool MidiHandler::hasNRPNMessage() const {
    return !incoming_nrpn_queue_.empty();
}

NRPNMessage MidiHandler::popNRPNMessage() {
    if (!incoming_nrpn_queue_.empty()) {
        NRPNMessage nrpn = incoming_nrpn_queue_.front();
        incoming_nrpn_queue_.pop();
        return nrpn;
    }
    return NRPNMessage{0, 0, 0, false};
}

void MidiHandler::setNRPNCallback(std::function<void(uint16_t, uint16_t)> callback) {
    nrpn_callback_ = callback;
}

void MidiHandler::setCCCallback(std::function<void(uint8_t, uint8_t)> callback) {
    cc_callback_ = callback;
}

void MidiHandler::getOutgoingMessages(std::vector<MidiMessage>& messages) {
    messages.clear();
    while (!outgoing_midi_queue_.empty()) {
        messages.push_back(outgoing_midi_queue_.front());
        outgoing_midi_queue_.pop();
    }
}

void MidiHandler::clearOutgoingMessages() {
    while (!outgoing_midi_queue_.empty()) {
        outgoing_midi_queue_.pop();
    }
}

void MidiHandler::resetNRPNState() {
    nrpn_state_ = WAITING_FOR_NRPN_MSB;
    nrpn_msb_ = 0;
    nrpn_lsb_ = 0;
    data_msb_ = 0;
    data_lsb_ = 0;
}

bool MidiHandler::isNRPNComplete() {
    return nrpn_state_ == NRPN_COMPLETE;
}