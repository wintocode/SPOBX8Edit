#pragma once
#include <string>
#include <vector>
#include <functional>

#ifdef __APPLE__
#include <CoreMIDI/CoreMIDI.h>
#endif

struct MidiDeviceInfo {
    std::string name;
    std::string id;
    bool is_input;
    bool is_output;
    bool is_available;
};

class MidiDeviceManager {
public:
    MidiDeviceManager();
    ~MidiDeviceManager();
    
    // Device enumeration
    void refreshDeviceList();
    const std::vector<MidiDeviceInfo>& getDevices() const { return devices_; }
    std::vector<std::string> getDeviceNames() const;
    
    // Device selection
    bool selectDevice(const std::string& device_name);
    std::string getSelectedDeviceName() const { return selected_device_name_; }
    
    // MIDI I/O
    bool sendMidiData(const uint8_t* data, size_t length);
    void setMidiReceiveCallback(std::function<void(const uint8_t*, size_t)> callback);
    
    // Connection status
    bool isConnected() const { return is_connected_; }
    
private:
    std::vector<MidiDeviceInfo> devices_;
    std::string selected_device_name_;
    bool is_connected_;
    std::function<void(const uint8_t*, size_t)> receive_callback_;
    
#ifdef __APPLE__
    MIDIClientRef midi_client_;
    MIDIPortRef input_port_;
    MIDIPortRef output_port_;
    MIDIEndpointRef selected_input_endpoint_;
    MIDIEndpointRef selected_output_endpoint_;
    
    void initializeCoreAudio();
    void cleanupCoreAudio();
    static void midiReadProc(const MIDIPacketList* packet_list, void* read_proc_ref_con, void* src_conn_ref_con);
#endif
    
    void updateConnectionStatus();
};