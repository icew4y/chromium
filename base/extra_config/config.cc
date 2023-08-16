
#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include "base/base_switches.h"
#include "base/command_line.h"
#include "cc/base/switches.h"

#include "base/values.h"
#include "base/json/json_reader.h"

#include "base/base64.h"
#include "base/logging.h"
#include "base/lazy_instance.h"
#include "config.h"

base::LazyInstance<absl::optional<base::Value>>::Leaky g_extra_config = LAZY_INSTANCE_INITIALIZER;
// To see logs on Windows: C:\Users\$UserName\AppData\Local\Chromium\User Data\chrome_debug.log

void init(){
    //LOG(ERROR) << "try to init g_extra_config";
    if (!g_extra_config.Get().has_value()){
        LOG(ERROR) << "g_extra_config not init, try to init";
        base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
        if (command_line->HasSwitch("extra-config")){
            std::string extra_config = command_line->GetSwitchValueASCII("extra-config");
            std::string decoded_config;
            if (base::Base64Decode(extra_config, &decoded_config)){
                LOG(ERROR) << "decoded_config: " << decoded_config;
                g_extra_config.Get() = base::JSONReader::Read(decoded_config);
            }else{
                LOG(ERROR) << "decoded_config failed: " << extra_config;
            }
        }
    }

}

bool base::HasKey(std::string key){
    init();
    if (g_extra_config.Get().has_value() && g_extra_config.Get()->is_dict()){
        base::Value::Dict *dict = g_extra_config.Get()->GetIfDict();
        return dict->Find(key);
    }
    
    return false;
}

std::string base::GetConfigValue(std::string key){
    init();
    if (g_extra_config.Get().has_value() && g_extra_config.Get()->is_dict()){
        base::Value::Dict *dict = g_extra_config.Get()->GetIfDict();
        std::string config_value = dict->Find(key)->GetString();
        LOG(ERROR) << "GetConfigValue: " << ", key;" << key << ", value: " << config_value;
        return config_value;
    }
    return "";
}



std::vector<std::map<std::string, std::string>> base::GetList(std::string key) {
    init();
    std::vector<std::map<std::string, std::string>> devices;
    if (g_extra_config.Get().has_value() && g_extra_config.Get()->is_dict()){
        base::Value::Dict *dict = g_extra_config.Get()->GetIfDict();
        base::Value::List *list = dict->FindList(key);
        if (list){
            LOG(ERROR) << "GetList: " << key << ", size: " << list->size();
            for(size_t i = 0; i < list->size(); i ++){
                base::Value v_dict = std::move((*list)[i]);
                if (v_dict.is_dict()){
                    base::Value::Dict& d = v_dict.GetDict();
                    std::map<std::string, std::string> device;
                    for (const auto [k, v] : d) {
                        
                        std::string v_ = "";
                        if (v.is_string()){
                            v_ = v.GetString();
                        }
                        else if (v.is_bool()){
                            const bool vb = v.GetBool();
                            v_ = vb ? "true" : "false";
                        }
                        else if (v.is_int()){
                            const int vi = v.GetInt();
                            v_ = std::to_string(vi);
                        }
                        else if (v.is_double()){
                            const double vd = v.GetDouble();
                            v_ = std::to_string(vd);
                        }else{
                            LOG(ERROR) << "GetList: " << key << ", k: " << k << ", v GetTypeName: " << v.GetTypeName(v.type());
                        }

                        LOG(ERROR) << "GetList: " << key << ", k: " << k << ", v: " << v_;
                        device[k] = v_;
                    }
                    devices.push_back(device);
                }
                
            }
        }
    }
    return devices;
}

std::map<std::string, std::string> base::GetDict(std::string key) {
    init();
    std::map<std::string, std::string> dct;
    if (g_extra_config.Get().has_value() && g_extra_config.Get()->is_dict()){
        base::Value::Dict *dict = g_extra_config.Get()->GetIfDict();
        if (!dict){
            LOG(ERROR) << "GetDict: " << " is not a dict object";
                return dct;
        }
        base::Value::Dict *find_dct = dict->FindDict(key);
        if (!find_dct){
            LOG(ERROR) << "GetDict: " << key << ", not found";
            return dct;
        }
        for (const auto [k, v] : *find_dct) {
            std::string v_ = "";
            if (v.is_string()){
                v_ = v.GetString();
            }
            else if (v.is_bool()){
                const bool vb = v.GetBool();
                v_ = vb ? "true" : "false";
            }
            else if (v.is_int()){
                const int vi = v.GetInt();
                v_ = std::to_string(vi);
            }
            else if (v.is_double()){
                const double vd = v.GetDouble();
                v_ = std::to_string(vd);
            }else{
                LOG(ERROR) << "GetDict: " << key << ", k: " << k << ", v GetTypeName: " << v.GetTypeName(v.type());
            }
            LOG(ERROR) << "GetDict: " << key << ", k: " << k << ", v: " << v_;
            dct[k] = v_;
        }
    }
    return dct;
}