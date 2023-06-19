FROM debian:bookworm-slim AS build

# Install basics
RUN apt-get update && apt-get -y install build-essential cmake git openssl libssl-dev supervisor libarchive-dev \
    zlib1g-dev wget
RUN mkdir -p /usr/src

# Install google test
RUN mkdir -p /usr/src && \
    cd /usr/src && \
    git clone https://github.com/google/googletest.git && \
    cd /usr/src/googletest && \
    cmake -DBUILD_SHARED_LIBS=ON . &&\
    make && \
    make install

# Install Poco libraries
RUN cd /usr/src && \
    git clone -b master https://github.com/pocoproject/poco.git && \
    cd /usr/src/poco && \
    cmake -DENABLE_PAGECOMPILER=OFF -DENABLE_DATA_SQLITE=OFF -DENABLE_REDIS=OFF -DENABLE_DATA_POSTGRESQL=OFF -DENABLE_DATA_MYSQL=OFF \
    -DENABLE_NETSSL=ON -DENABLE_MONGODB=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
    make && \
    make install

# Install mongo-c-driver
RUN cd /usr/src && \
    git clone -b master https://github.com/mongodb/mongo-c-driver.git && \
    cd /usr/src/mongo-c-driver && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
    make && \
    make install

# Install mongo-c-driver
RUN cd /usr/src && \
    git clone -b master https://github.com/mongodb/mongo-cxx-driver.git && \
    cd /usr/src/mongo-cxx-driver && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
    make && \
    make install

# Install sv4git
RUN cd /usr/src && \
    wget https://github.com/bvieira/sv4git/releases/download/v2.9.0/git-sv_2.9.0_linux_amd64.tar.gz && \
    tar -xzf git-sv_2.9.0_linux_amd64.tar.gz && \
    cp git-sv /usr/bin && chmod 755 /usr/bin/git-sv

# Install aws-mock
RUN cd /usr/src && \
    git clone https://github.com/jensvogt/aws-mock.git && \
    cd /usr/src/aws-mock && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
    cmake --build . --config Release && \
    cmake --install . --prefix /usr

FROM debian:bookworm-slim

RUN apt-get update && apt-get -y install openssl supervisor libarchive13 zlib1g

COPY --from=build /usr/src/aws-mock/etc/aws-mock.properties /etc
COPY --from=build /usr/bin/aws-mock /usr/bin
COPY --from=build /usr/lib/libPocoFoundation* /usr/lib
COPY --from=build /usr/lib/libPocoUtil* /usr/lib
COPY --from=build /usr/lib/libPocoNet* /usr/lib
COPY --from=build /usr/lib/libPocoXML* /usr/lib
COPY --from=build /usr/lib/libPocoJSON* /usr/lib
COPY --from=build /usr/lib/libPocoZip* /usr/lib
COPY --from=build /usr/lib/libPocoCrypto* /usr/lib
COPY --from=build /usr/lib/libPocoPrometheus* /usr/lib
COPY --from=build /usr/lib/x86_64-linux-gnu/libmongoc-1.0* /usr/lib
COPY --from=build /usr/lib/x86_64-linux-gnu/libmongocxx* /usr/lib
COPY --from=build /usr/lib/x86_64-linux-gnu/libbson* /usr/lib
COPY --from=build /usr/lib/libawsmock* /usr/lib

# Supervisord
RUN apt-get update && \
    apt-get install -y supervisor
RUN mkdir -p /var/log/supervisor
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Make dirctories
RUN mkdir -p /tmp/awsmock/data/s3 && \
    mkdir -p /tmp/awsmock/data/sqs && \
    mkdir -p /tmp/awsmock/data/lambda && \
    mkdir -p /tmp/awsmock/init

# Add properties

# Expose ports
EXPOSE 4567 4567

# Cleanup
RUN rm -rf /usr/src

CMD ["/usr/bin/supervisord"]