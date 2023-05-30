//
// Created by vogje01 on 30/05/2023.
//

#include "awsmock/dto/s3/CreateBucketRequest.h"

namespace AwsMock::Dto::S3 {

    CreateBucketRequest::CreateBucketRequest(const std::string &xmlString) {
        FromXml(xmlString);
    }

    void CreateBucketRequest::FromXml(const std::string &xmlString) {

        Poco::XML::DOMParser parser;
        Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(xmlString);

        Poco::XML::Node *node = pDoc->getNodeByPath("/CreateBucketConfiguration/LocationConstraint");
        _locationConstraint = node->innerText();
    }

    std::string CreateBucketRequest::ToString() const {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    std::ostream &operator<<(std::ostream &os, const CreateBucketRequest &r) {
        os << "CreateBucketRequest={locationConstraint='" + r._locationConstraint + "'}";
        return os;
    }
}