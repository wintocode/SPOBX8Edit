#include "midi_device_manager.h"
#include <algorithm>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

static std::string CFStringToStdString(CFStringRef cf_string) {
    if (!cf_string) return "";
    
    CFIndex length = CFStringGetLength(cf_string);
    CFIndex max_size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char* buffer = new char[max_size];
    
    if (CFStringGetCString(cf_string, buffer, max_size, kCFStringEncodingUTF8)) {
        std::string result(buffer);
        delete[] buffer;
        return result;
    }
    
    delete[] buffer;
    return "";
}
#endif

MidiDeviceManager::MidiDeviceManager() 
    : is_connected_(false)
#ifdef __APPLE__
    , midi_client_(0)
    , input_port_(0)
    , output_port_(0)
    , selected_input_endpoint_(0)
    , selected_output_endpoint_(0)
#endif
{
#ifdef __APPLE__
    initializeCoreAudio();
#endif
    refreshDeviceList();
}

MidiDeviceManager::~MidiDeviceManager() {
#ifdef __APPLE__
    cleanupCoreAudio();
#endif
}

#ifdef __APPLE__
void MidiDeviceManager::initializeCoreAudio() {
    OSStatus status = MIDIClientCreate(CFSTR("SPOBX8Edit"), nullptr, nullptr, &midi_client_);
    if (status != noErr) {
        std::cerr << "Failed to create MIDI client: " << status << std::endl;
        return;
    }
    
    status = MIDIInputPortCreate(midi_client_, CFSTR("SPOBX8Edit Input"), midiReadProc, this, &input_port_);
    if (status != noErr) {
        std::cerr << "Failed to create MIDI input port: " << status << std::endl;
    }
    
    status = MIDIOutputPortCreate(midi_client_, CFSTR("SPOBX8Edit Output"), &output_port_);
    if (status != noErr) {
        std::cerr << "Failed to create MIDI output port: " << status << std::endl;
    }
}

void MidiDeviceManager::cleanupCoreAudio() {
    if (input_port_) {
        MIDIPortDispose(input_port_);
        input_port_ = 0;
    }
    
    if (output_port_) {
        MIDIPortDispose(output_port_);
        output_port_ = 0;
    }
    
    if (midi_client_) {
        MIDIClientDispose(midi_client_);
        midi_client_ = 0;
    }
}

void MidiDeviceManager::midiReadProc(const MIDIPacketList* packet_list, void* read_proc_ref_con, void* src_conn_ref_con) {
    MidiDeviceManager* manager = static_cast<MidiDeviceManager*>(read_proc_ref_con);
    
    if (!manager->receive_callback_) {
        return;
    }
    
    const MIDIPacket* packet = &packet_list->packet[0];
    for (UInt32 i = 0; i < packet_list->numPackets; ++i) {
        manager->receive_callback_(packet->data, packet->length);
        packet = MIDIPacketNext(packet);
    }
}
#endif

void MidiDeviceManager::refreshDeviceList() {
    devices_.clear();
    
#ifdef __APPLE__
    // Get number of MIDI devices
    ItemCount num_devices = MIDIGetNumberOfDevices();
    
    for (ItemCount i = 0; i < num_devices; ++i) {
        MIDIDeviceRef device = MIDIGetDevice(i);
        if (!device) continue;
        
        // Check if device is online/connected
        SInt32 is_offline = 0;
        OSStatus offline_status = MIDIObjectGetIntegerProperty(device, kMIDIPropertyOffline, &is_offline);
        if (offline_status == noErr && is_offline != 0) {
            continue; // Skip offline devices
        }
        
        // Get device name
        CFStringRef name_ref = nullptr;
        OSStatus status = MIDIObjectGetStringProperty(device, kMIDIPropertyName, &name_ref);
        if (status != noErr || !name_ref) continue;
        
        std::string device_name = CFStringToStdString(name_ref);
        CFRelease(name_ref);
        
        // Debug: Log all MIDI devices found
        std::ofstream debug_file("/tmp/midi_debug.log", std::ios::app);
        debug_file << "MIDI Device " << i << ": " << device_name << std::endl;
        debug_file.close();
        
        // Check for OBX8 in the name (case insensitive)
        std::string lower_name = device_name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        bool is_obx8 = lower_name.find("obx") != std::string::npos || 
                       lower_name.find("oberheim") != std::string::npos;
        
        // Get entities for this device
        ItemCount num_entities = MIDIDeviceGetNumberOfEntities(device);
        for (ItemCount j = 0; j < num_entities; ++j) {
            MIDIEntityRef entity = MIDIDeviceGetEntity(device, j);
            if (!entity) continue;
            
            // Check sources (inputs to our app)
            ItemCount num_sources = MIDIEntityGetNumberOfSources(entity);
            if (num_sources > 0) {
                MidiDeviceInfo info;
                info.name = device_name + " (Input)";
                info.id = std::to_string(i) + "_" + std::to_string(j) + "_in";
                info.is_input = true;
                info.is_output = false;
                info.is_available = true;
                devices_.push_back(info);
            }
            
            // Check destinations (outputs from our app)
            ItemCount num_destinations = MIDIEntityGetNumberOfDestinations(entity);
            if (num_destinations > 0) {
                MidiDeviceInfo info;
                info.name = device_name + " (Output)";
                info.id = std::to_string(i) + "_" + std::to_string(j) + "_out";
                info.is_input = false;
                info.is_output = true;
                info.is_available = true;
                devices_.push_back(info);
            }
        }
    }
    
    // Also check external endpoints
    ItemCount num_sources = MIDIGetNumberOfSources();
    for (ItemCount i = 0; i < num_sources; ++i) {
        MIDIEndpointRef source = MIDIGetSource(i);
        if (!source) continue;
        
        // Check if endpoint is online/connected
        SInt32 is_offline = 0;
        OSStatus offline_status = MIDIObjectGetIntegerProperty(source, kMIDIPropertyOffline, &is_offline);
        if (offline_status == noErr && is_offline != 0) {
            continue; // Skip offline endpoints
        }
        
        CFStringRef name_ref = nullptr;
        OSStatus status = MIDIObjectGetStringProperty(source, kMIDIPropertyName, &name_ref);
        if (status != noErr || !name_ref) continue;
        
        std::string endpoint_name = CFStringToStdString(name_ref);
        CFRelease(name_ref);
        
        // Debug: Log all MIDI source endpoints
        std::ofstream debug_file("/tmp/midi_debug.log", std::ios::app);
        debug_file << "MIDI Source " << i << ": " << endpoint_name << std::endl;
        debug_file.close();
        
        MidiDeviceInfo info;
        info.name = endpoint_name;
        info.id = "src_" + std::to_string(i);
        info.is_input = true;
        info.is_output = false;
        info.is_available = true;
        devices_.push_back(info);
    }
    
    ItemCount num_destinations = MIDIGetNumberOfDestinations();
    for (ItemCount i = 0; i < num_destinations; ++i) {
        MIDIEndpointRef destination = MIDIGetDestination(i);
        if (!destination) continue;
        
        // Check if endpoint is online/connected
        SInt32 is_offline = 0;
        OSStatus offline_status = MIDIObjectGetIntegerProperty(destination, kMIDIPropertyOffline, &is_offline);
        if (offline_status == noErr && is_offline != 0) {
            continue; // Skip offline endpoints
        }
        
        CFStringRef name_ref = nullptr;
        OSStatus status = MIDIObjectGetStringProperty(destination, kMIDIPropertyName, &name_ref);
        if (status != noErr || !name_ref) continue;
        
        std::string endpoint_name = CFStringToStdString(name_ref);
        CFRelease(name_ref);
        
        // Debug: Log all MIDI destination endpoints
        std::ofstream debug_file("/tmp/midi_debug.log", std::ios::app);
        debug_file << "MIDI Destination " << i << ": " << endpoint_name << std::endl;
        debug_file.close();
        
        // Skip individual ports entirely - this includes Port 1, Port 2, etc.
        std::string lower_endpoint_name = endpoint_name;
        std::transform(lower_endpoint_name.begin(), lower_endpoint_name.end(), lower_endpoint_name.begin(), ::tolower);
        
        if (lower_endpoint_name.find("port") != std::string::npos) {
            continue; // Skip ALL individual ports
        }
        
        MidiDeviceInfo info;
        info.name = endpoint_name;
        info.id = "dst_" + std::to_string(i);
        info.is_input = false;
        info.is_output = true;
        info.is_available = true;
        devices_.push_back(info);
    }
#endif
}

std::vector<std::string> MidiDeviceManager::getDeviceNames() const {
    std::vector<std::string> names;
    names.push_back("None");
    
    // Find ONLY the main OB-X8 Module device (not individual ports)
    bool found_obx8_main = false;
    for (const auto& device : devices_) {
        // Only show output devices since we need to send MIDI TO the hardware
        if (!device.is_output) {
            continue;
        }
        
        std::string lower_name = device.name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        
        // Look for the main OB-X8 Module device (not individual ports)
        if ((lower_name.find("obx") != std::string::npos || 
             lower_name.find("oberheim") != std::string::npos ||
             lower_name.find("ob-x8") != std::string::npos) &&
            lower_name.find("port") == std::string::npos &&
            !found_obx8_main) {
            
            names.push_back("🎹 Oberheim OB-X8");
            found_obx8_main = true;
            break; // Only add one OBX8 device
        }
    }
    
    return names;
}

bool MidiDeviceManager::selectDevice(const std::string& device_name) {
    if (device_name == "None") {
        selected_device_name_ = "";
        is_connected_ = false;
        return true;
    }
    
    // Find the device - need to handle friendly names and connect to both input and output
    selected_device_name_ = device_name;
    
#ifdef __APPLE__
    // Reset endpoints
    selected_input_endpoint_ = 0;
    selected_output_endpoint_ = 0;
    
    if (device_name.find("Oberheim OB-X8") != std::string::npos) {
        // Find the OBX8 device by name match
        for (const auto& device : devices_) {
            std::string lower_name = device.name;
            std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
            
            bool is_obx8 = lower_name.find("obx") != std::string::npos || 
                          lower_name.find("oberheim") != std::string::npos ||
                          lower_name.find("ob-x8") != std::string::npos;
            
            if (is_obx8 && device.is_output && device.id.substr(0, 4) == "dst_") {
                int index = std::stoi(device.id.substr(4));
                selected_output_endpoint_ = MIDIGetDestination(index);
                break; // Use the first matching OBX8 output device
            }
        }
    } else {
        // Fallback to exact name match
        auto it = std::find_if(devices_.begin(), devices_.end(),
            [&device_name](const MidiDeviceInfo& device) {
                return device.name == device_name;
            });
        
        if (it != devices_.end()) {
            if (it->is_output && it->id.substr(0, 4) == "dst_") {
                int index = std::stoi(it->id.substr(4));
                selected_output_endpoint_ = MIDIGetDestination(index);
            }
            
            if (it->is_input && it->id.substr(0, 4) == "src_") {
                int index = std::stoi(it->id.substr(4));
                selected_input_endpoint_ = MIDIGetSource(index);
                
                if (input_port_ && selected_input_endpoint_) {
                    MIDIPortConnectSource(input_port_, selected_input_endpoint_, nullptr);
                }
            }
        }
    }
#endif
    
    updateConnectionStatus();
    return is_connected_;
}

bool MidiDeviceManager::sendMidiData(const uint8_t* data, size_t length) {
    if (!is_connected_ || length == 0) {
        return false;
    }
    
#ifdef __APPLE__
    if (!output_port_ || !selected_output_endpoint_) {
        return false;
    }
    
    Byte packet_buffer[1024];
    MIDIPacketList* packet_list = (MIDIPacketList*)packet_buffer;
    MIDIPacket* packet = MIDIPacketListInit(packet_list);
    
    packet = MIDIPacketListAdd(packet_list, sizeof(packet_buffer), packet, 0, length, data);
    if (!packet) {
        return false;
    }
    
    OSStatus status = MIDISend(output_port_, selected_output_endpoint_, packet_list);
    return (status == noErr);
#endif
    
    return false;
}

void MidiDeviceManager::setMidiReceiveCallback(std::function<void(const uint8_t*, size_t)> callback) {
    receive_callback_ = callback;
}

void MidiDeviceManager::updateConnectionStatus() {
    // Check if we have output connected (needed for sending data to hardware)
    is_connected_ = !selected_device_name_.empty() && selected_output_endpoint_ != 0;
}