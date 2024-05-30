//
// Created by vogje01 on 04/01/2023.
//

#ifndef AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
#define AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H

// C++ include
#include <utility>

// AwsMock includes
#include <awsmock/core/HttpSocket.h>
#include <awsmock/core/HttpSocketResponse.h>
#include <awsmock/core/LogStream.h>
#include <awsmock/core/Task.h>
#include <awsmock/core/monitoring/MetricDefinition.h>
#include <awsmock/core/monitoring/MetricService.h>
#include <awsmock/core/monitoring/MetricServiceTimer.h>

namespace AwsMock::Service {

    namespace http = boost::beast::http;

    /**
     * @brief Lambda executor.
     *
     * The executor is launched asynchronously. As the dockerized lambda runtime using AWS RIE only allows the execution of a lambda function at a time, the lambda function invocation will be
     * queued up in a Poco notification queue and executed one by one. Each invocation will wait for the finishing of the last invocation request. The lambda image can run on a remote docker
     * instance. In this case the hostname on the invocation request has to be filled in. Default is 'localhost'.
     *
     * @author jens.vogt\@opitz-consulting.com
     */
    class LambdaExecutor : public Core::Task {

      public:

        /**
         * Constructor
         *
         * @param url lambda docker URL
         * @param payload event payload
         */
        explicit LambdaExecutor(std::string host, int port, std::string payload) : Core::Task("lambda-executor"), _host(std::move(host)), _port(port), _payload(std::move(payload)){};

        /**
         * Send the invocation request to the corresponding port
         */
        void Run() override;

      private:

        /**
         * Metric module
         */
        Core::MetricService &_metricService = Core::MetricService::instance();

        /**
         * Lambda host
         */
        std::string _host;

        /**
         * Lambda port
         */
        int _port;

        /**
         * Lambda payload
         */
        std::string _payload;
    };

}// namespace AwsMock::Service

#endif// AWSMOCK_SERVICE_LAMBDA_EXECUTOR_H
