//
// Created by vogje01 on 29/05/2023.
//

#include "awsmock/core/HttpUtils.h"

namespace AwsMock::Core {

    std::string HttpUtils::GetBasePath(const std::string &uri) {
        std::string basePath = StringUtils::SubStringUntil(uri, "?");
        if (basePath[0] == '/') {
            basePath = basePath.substr(1);
        }
        return basePath;
    }

    std::string HttpUtils::GetPathParameter(const std::string &uri, int index) {

        std::string basePath = GetBasePath(uri);

        std::vector<std::string> parameters = StringUtils::Split(basePath, '/');
        if (index >= (int) parameters.size()) {
            return {};
        }
        if (IsUrlEncoded(parameters[index])) {
            return Core::StringUtils::UrlDecode(parameters[index]);
        }
        return parameters[index];
    }

    std::string HttpUtils::GetPathParametersFromIndex(const std::string &uri, int index) {

        std::string basePath = GetBasePath(uri);
        std::vector<std::string> parameters = StringUtils::Split(basePath, '/');
        return Core::StringUtils::UrlDecode(Core::StringUtils::Join(parameters, '/', index));
    }

    std::vector<std::string> HttpUtils::GetPathParameters(const std::string &uri) {

        std::string basePath = GetBasePath(uri);
        return StringUtils::Split(basePath, '/');
    }

    bool HttpUtils::HasPathParameters(const std::string &uri, int index) {

        std::string basePath = GetBasePath(uri);
        std::vector<std::string> pathVector = StringUtils::Split(basePath, '/');
        return index < pathVector.size();
    }

    std::string HttpUtils::GetQueryString(const std::string &uri) {
        if (StringUtils::Contains(uri, "?")) {
            return StringUtils::SubStringAfter(uri, "?");
        }
        return uri;
    }

    int HttpUtils::CountQueryParameters(const std::string &uri) {
        std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');
        return (int) parameters.size();
    }

    int HttpUtils::CountQueryParametersByPrefix(const std::string &uri, const std::string &prefix) {

        std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        int count = 0;
        for (auto &it: parameters) {
            if (it.starts_with(prefix)) {
                count++;
            }
        }
        return count;
    }

    std::string HttpUtils::GetQueryParameterName(const std::string &parameter) {
        return StringUtils::Split(parameter, '=')[0];
    }

    std::string HttpUtils::GetQueryParameterValue(const std::string &parameter) {
        std::vector<std::string> parts = StringUtils::Split(parameter, '=');
        if (parts.size() > 1) {
            std::string value = parts[1];
            if (IsUrlEncoded(value)) {
                return StringUtils::UrlDecode(value);
            }
            return value;
        }
        return {};
    }

    std::vector<std::string> HttpUtils::GetQueryParametersByPrefix(const std::string &uri, const std::string &prefix) {

        std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        std::vector<std::string> namedParameters;
        for (const auto &it: parameters) {
            if (it.starts_with(prefix)) {
                namedParameters.emplace_back(GetQueryParameterValue(it));
            }
        }
        return namedParameters;
    }

    std::string HttpUtils::GetQueryParameterValueByName(const std::string &uri, const std::string &name) {

        std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');

        for (const auto &it: parameters) {
            if (GetQueryParameterName(it) == name) {
                return GetQueryParameterValue(it);
            }
        }
        return {};
    }

    std::string HttpUtils::GetQueryParameterByPrefix(const std::string &uri, const std::string &prefix, int index) {

        std::vector<std::string> parameters = GetQueryParametersByPrefix(uri, prefix);

        if (index > parameters.size()) {
            throw Core::ServiceException("Invalid path parameter index");
        }
        log_trace << "Query parameter found, prefix: " << prefix << " index: " << index;
        return parameters[index - 1];
    }

    int HttpUtils::GetIntParameter(const std::string &body, const std::string &name, int min, int max, int def) {
        int value = def;
        std::string parameterValue = GetQueryParameterValueByName(body, name);
        if (!parameterValue.empty()) {
            value = std::stoi(parameterValue);
            value = value > min && value < max ? value : def;
        }
        log_warning << "Query parameter found, name: " << name << " value: " << value;
        return value;
    }

    bool HttpUtils::HasQueryParameter(const std::string &uri, const std::string &name) {
        std::string queryString = GetQueryString(uri);
        std::vector<std::string> parameters = StringUtils::Split(queryString, '&');
        for (const auto &it: parameters) {
            if (GetQueryParameterName(it) == name) {
                return true;
            }
        }
        log_warning << "Query parameter not found, name: " << name;
        return false;
    }

    [[maybe_unused]] std::string HttpUtils::GetHeaderValue(const Poco::Net::HTTPRequest &request, const std::string &name) {
        std::string headerValue = request.get(name);
        if (headerValue.empty()) {
            log_warning << "Header value not found, key: " << name;
        }
        return headerValue;
    }

    bool HttpUtils::IsUrlEncoded(const std::string &value) {
        return !StringUtils::Equals(StringUtils::UrlDecode(value), value);
    }

    void HttpUtils::GetVersionAction(const std::string &uri, std::string &version, std::string &action) {
        version = GetPathParameters(uri)[0];
        action = GetPathParameters(uri)[1];
    }

    std::string HttpUtils::GetBodyAsString(Poco::Net::HTTPServerRequest &request) {
        std::string body;
        Poco::StreamCopier::copyToString(request.stream(), body);
        return body;
    }
}// namespace AwsMock::Core