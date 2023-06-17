# aws-mock

aws-mock is a high performance, persistent AWS simulation. Currently the following services are supported: 

- S3: Amazon Simple Storage Service
- SQS: Amazon Simple Queue Service
- SNS: Amazon Simple Notification Service
- Lambda: Amazon Lambda Service

aws-mock is written in C++ with a MongoDB persistence layer. All objects are stored in the database. The simulation 
reacts to all AWS commands send to the default port 4567. For a lists of supported command see below tables for the 
different AWS services.

## S3

Supported commands are:

| REST Api               | Description                                   | Example                                                                                                     |
|------------------------|-----------------------------------------------|-------------------------------------------------------------------------------------------------------------|
| CreateBucket           | Create a new bucket                           | aws s3 create-bucket <bucket> --endpoint http://localhost:4567                                             |
| PutObject              | Upload an object to an S3 bucket              | aws s3 cp <file> s3://<bucket> --endpoint http://localhost:4567                                            |
| GetObject              | Download a object from a S3 bucket            | aws s3 cp s3://bucket <file> --endpoint http://localhost:4567                                              |
| ListAllBucket          | List all buckets                              | aws s3 ls --endpoint http://localhost:4567                                                                 |
| ListBucket             | List the content of a bucket                  | aws s3 ls s3://<bucket> --recursive --endpoint http://localhost:4567                                        |
| PutBucketNotification  | Adds a notification configuration to a bucket | aws put-bucket-notification-configuration --bucket <bucket> --notification-configuration file://<hook-file> |

## SQS

Supported commands are:

| REST Api       | Description                   | Example                                                                   |
|----------------|-------------------------------|---------------------------------------------------------------------------|
| CreateQueue    | Create a new queue            | aws sqs create-queue --queue-name <name> --endpoint http://localhost:4567 |
| SendMessage    | Send a message to a queue     | aws sqs send-message --queue-url <url> --endpoint http://localhost:4567   |
| ReceiveMessage | Receive messages from a queue | aws sqs receive-message --queue-url <url> --endpoint http://localhost:4567                      |
