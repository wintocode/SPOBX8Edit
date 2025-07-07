#include "obx8_plugin.h"
#include <clap/clap.h>

// Forward declarations for plugin interface
static bool obx8_plugin_init(const clap_plugin_t *plugin);
static void obx8_plugin_destroy(const clap_plugin_t *plugin);
static bool obx8_plugin_activate(const clap_plugin_t *plugin, double sample_rate, uint32_t min_frames, uint32_t max_frames);
static void obx8_plugin_deactivate(const clap_plugin_t *plugin);
static bool obx8_plugin_start_processing(const clap_plugin_t *plugin);
static void obx8_plugin_stop_processing(const clap_plugin_t *plugin);
static void obx8_plugin_reset(const clap_plugin_t *plugin);
static clap_process_status obx8_plugin_process(const clap_plugin_t *plugin, const clap_process_t *process);
static const void* obx8_plugin_get_extension(const clap_plugin_t *plugin, const char *id);
static void obx8_plugin_on_main_thread(const clap_plugin_t *plugin);

// Plugin interface implementation
static bool obx8_plugin_init(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    return p->init();
}

static void obx8_plugin_destroy(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    p->destroy();
    delete p;
}

static bool obx8_plugin_activate(const clap_plugin_t *plugin, double sample_rate, uint32_t min_frames, uint32_t max_frames) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    return p->activate(sample_rate, min_frames, max_frames);
}

static void obx8_plugin_deactivate(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    p->deactivate();
}

static bool obx8_plugin_start_processing(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    return p->start_processing();
}

static void obx8_plugin_stop_processing(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    p->stop_processing();
}

static void obx8_plugin_reset(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    p->reset();
}

static clap_process_status obx8_plugin_process(const clap_plugin_t *plugin, const clap_process_t *process) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    return p->process(process);
}

static const void* obx8_plugin_get_extension(const clap_plugin_t *plugin, const char *id) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    return p->get_extension(id);
}

static void obx8_plugin_on_main_thread(const clap_plugin_t *plugin) {
    OBX8Plugin *p = static_cast<OBX8Plugin*>(plugin->plugin_data);
    p->on_main_thread();
}

// Plugin factory implementation
static const clap_plugin_t *obx8_create_plugin(const clap_plugin_factory_t *factory, const clap_host_t *host, const char *plugin_id) {
    if (strcmp(plugin_id, obx8_plugin_descriptor.id) != 0) {
        return nullptr;
    }
    
    OBX8Plugin *plugin_impl = new OBX8Plugin(host);
    
    clap_plugin_t *plugin = new clap_plugin_t;
    plugin->desc = &obx8_plugin_descriptor;
    plugin->plugin_data = plugin_impl;
    plugin->init = obx8_plugin_init;
    plugin->destroy = obx8_plugin_destroy;
    plugin->activate = obx8_plugin_activate;
    plugin->deactivate = obx8_plugin_deactivate;
    plugin->start_processing = obx8_plugin_start_processing;
    plugin->stop_processing = obx8_plugin_stop_processing;
    plugin->reset = obx8_plugin_reset;
    plugin->process = obx8_plugin_process;
    plugin->get_extension = obx8_plugin_get_extension;
    plugin->on_main_thread = obx8_plugin_on_main_thread;
    
    return plugin;
}

static uint32_t obx8_get_plugin_count(const clap_plugin_factory_t *factory) {
    return 1;
}

static const clap_plugin_descriptor_t *obx8_get_plugin_descriptor(const clap_plugin_factory_t *factory, uint32_t index) {
    if (index == 0) {
        return &obx8_plugin_descriptor;
    }
    return nullptr;
}

static const clap_plugin_factory_t obx8_plugin_factory = {
    .get_plugin_count = obx8_get_plugin_count,
    .get_plugin_descriptor = obx8_get_plugin_descriptor,
    .create_plugin = obx8_create_plugin
};

// Plugin entry point implementation
static bool obx8_init(const char *plugin_path) {
    return true;
}

static void obx8_deinit() {
}

static const void* obx8_get_factory(const char *factory_id) {
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) {
        return &obx8_plugin_factory;
    }
    return nullptr;
}

// Export the plugin entry point
extern "C" {
    const clap_plugin_entry_t clap_entry = {
        .clap_version = CLAP_VERSION_INIT,
        .init = obx8_init,
        .deinit = obx8_deinit,
        .get_factory = obx8_get_factory
    };
}