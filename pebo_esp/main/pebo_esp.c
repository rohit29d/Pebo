#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "nvs_flash.h"
#include "nvs.h"

#define settings_v 1
#define primary_button 1  // gpio 1
#define secondary_button 2  //gpio2


typedef struct{
    uint8_t version;
    bool bluetooth; 
    int volume; 
    int sleeptime;
    bool exit; 
    bool nomovemode;
    bool mute;

}pebo_settings;


pebo_settings settings;

void settings_defaults(pebo_settings *s){
    s-> version = settings_v;
    s-> bluetooth = false;
    s-> volume = 5;
    s-> sleeptime = 5; // minutes
    s-> exit = false;
    s-> nomovemode = false;
    s-> mute = false;
    

}

void settings_load(){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &handle);

    if(err != ESP_OK){
        settings_defaults(&settings);
        return;
    }

    size_t required_size =sizeof(settings);

    err = nvs_get_blob(handle,"pebo", &settings, &required_size);

    if(err != ESP_OK){
        settings_defaults(&settings);
    }

    nvs_close(handle);

}

void settings_save(){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("config",NVS_READWRITE,&handle);

    if (err != ESP_OK) return;

    nvs_set_blob(handle,"pebo",&settings,sizeof(settings));
    nvs_commit(handle);
    nvs_close(handle);

}
void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NEW_VERSION_FOUND){
        nvs_flash_erase();
        nvs_flash_init();
    }

    if (primary_button == high && secondary_button == high){
    settings_load();
    }

    if (settings.version != settings_v){
        settings_defaults(&settings);
    }
}
