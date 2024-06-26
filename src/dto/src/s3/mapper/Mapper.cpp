//
// Created by vogje01 on 5/10/24.
//

#include <awsmock/dto/s3/mapper/Mapper.h>

namespace AwsMock::Dto::S3 {

    Dto::S3::ListObjectVersionsResponse Mapper::map(const ListObjectVersionsRequest &request, const std::vector<Database::Entity::S3::Object> &objectList) {

        Dto::S3::ListObjectVersionsResponse response;
        response.region = request.region;
        response.name = request.bucket;
        response.maxKeys = request.maxKeys;
        for (const auto &object: objectList) {
            ObjectVersion version = {
                    .key = object.key,
                    .eTag = object.md5sum,
                    .versionId = object.versionId,
                    .storageClass = "STANDARD",
                    .isLatest = false,
                    .size = object.size,
                    .lastModified = object.modified,
            };
            version.owner = {.id = object.owner};

            response.versions.emplace_back(version);
        }
        return response;
    }

}// namespace AwsMock::Dto::S3