namespace base {
    std::string GetConfigValue(std::string key);
    bool HasKey(std::string key);
    std::vector<std::map<std::string, std::string>> GetList(std::string key);
    std::map<std::string, std::string> GetDict(std::string key);
}
namespace switches{
    const char kExtraConfig[] = "extra-config";
}