#include "Index2.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <loader/LoaderImpl.hpp>

using namespace geode::prelude;

// yes this is a macro.. so we can take advantage of string literal concat
#define GEODE_INDEX_URL "https://api.geode-sdk.org/v1"

std::string urlEncode(std::string const& input) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase;
    for (char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            ss << c;
        } else {
            ss << '%' << static_cast<int>(c);
        }
    }
    return ss.str();
}

std::string buildQueryString(std::initializer_list<std::pair<std::string, std::string>> const& values) {
    std::string query;
    for (auto const& [key, value] : values) {
        query += key + "=" + urlEncode(value) + "&";
    }
    return query;
}

void Index::getPageItems(int page, IndexQuery2 const& query, MiniFunction<void(std::vector<IndexItem2> const&)> callback, MiniFunction<void(std::string const&)> error) {
    web::AsyncWebRequest()
        .userAgent("Geode Loader")
        .get(GEODE_INDEX_URL "/mods?" + buildQueryString({
            { "gd", LoaderImpl::get()->getGameVersion() },
            { "page", std::to_string(page + 1) },
            { "per_page", std::to_string(m_pageLimit) },
            { "search", query.m_search },
        }))
        .json()
        .then([=](matjson::Value const& json) {
            std::vector<IndexItem2> items;
            for (auto const& entry : json["payload"]["data"].as_array()) {
                IndexItem2 item;
                auto const& latestVer = entry["versions"][0];
                item.m_modId = entry["id"].as_string();
                item.m_version = VersionInfo::parse(entry["latest_version"].as_string()).unwrap();
                item.m_downloadUrl = latestVer["download_link"].as_string();
                item.m_name = latestVer["name"].as_string();
                item.m_description = latestVer["description"].as_string();
                item.m_developer = "Lol api doesnt have this";
                item.m_isAPI = latestVer["api"].as_bool();
                items.push_back(item);
            }
            callback(std::move(items));
        })
        .expect([=](std::string const& msg) {
            try {
                log::error("Index error {}", matjson::parse(msg).dump());
            } catch (...) {
                log::error("Index error {}", msg);
            }
            error(msg);
        });
}

void Index::getDetailedInfo(std::string const& modId, MiniFunction<void(DetailedIndexItem2 const&)> callback, MiniFunction<void(std::string const&)> error) {
    web::AsyncWebRequest()
        .userAgent("Geode Loader")
        .get(GEODE_INDEX_URL "/mods/" + modId)
        .json()
        .then([=](matjson::Value const& json) {
            DetailedIndexItem2 item;
            auto const data = json["payload"];
            auto const& latestVer = data["versions"][0];
            item.m_modId = data["id"].as_string();
            item.m_version = VersionInfo::parse(latestVer["version"].as_string()).unwrap();
            item.m_downloadUrl = latestVer["download_link"].as_string();
            item.m_name = latestVer["name"].as_string();
            item.m_description = latestVer["description"].as_string();
            item.m_developer = "Lol api doesnt have this";
            item.m_isAPI = latestVer["api"].as_bool();
            item.m_about = data["about"].is_string() ? std::make_optional(data["about"].as_string()) : std::nullopt;
            item.m_changelog = data["changelog"].is_string() ? std::make_optional(data["changelog"].as_string()) : std::nullopt;
            callback(std::move(item));
        })
        .expect([=](std::string const& msg) {
            try {
                log::error("Index error {}", matjson::parse(msg).dump());
            } catch (...) {
                log::error("Index error {}", msg);
            }
            error(msg);
        });
}

ModInstallEvent::ModInstallEvent(
    std::string const& id, const UpdateStatus status
) : modID(id), status(status) {}

ListenerResult ModInstallFilter::handle(utils::MiniFunction<Callback> fn, ModInstallEvent* event) {
    if (m_id == event->modID) {
        fn(event);
    }
    return ListenerResult::Propagate;
}

ModInstallFilter::ModInstallFilter(std::string const& id) : m_id(id) {}

std::string IndexItem2::getDownloadURL() const {
    return {};
}
std::string IndexItem2::getPackageHash() const {
    return {};
}
std::unordered_set<PlatformID> IndexItem2::getAvailablePlatforms() const {
    return {};
}
bool IndexItem2::isFeatured() const {
    return {};
}
std::unordered_set<std::string> IndexItem2::getTags() const {
    return {};
}
bool IndexItem2::isInstalled() const {
    return {};
}