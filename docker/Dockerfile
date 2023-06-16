FROM debian:latest

# Install basics
RUN apt-get update && apt-get -y install build-essential cmake git openssl libssl-dev supervisor libarchive-dev
RUN apt-get -y install libiodbc2 libiodbc2-dev libpq-dev libpqxx-dev zlib1g-dev libcurl4 \
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
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
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

# Install aws-mock
RUN cd /usr/src && \
    git clone https://github.com/jensvogt/aws-mock.git && \
    cd /usr/src/aws-mock && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr . && \
    cmake --build . --config Release && \
    cmake --install . --prefix /usr

# Supervisord
RUN apt-get update && \
    apt-get install -y supervisor
RUN mkdir -p /var/log/supervisor
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

RUN mkdir -p /tmp/awsmock/data/db && \
    mkdir -p /tmp/awsmock/data/s3 && \
    mkdir -p /tmp/awsmock/data/sqs && \
    mkdir -p /tmp/awsmock/data/lambda && \
    mkdir -p /tmp/awsmock/data/tmp

RUN cp /usr/src/aws-mock/etc/aws-mock.properties /etc

EXPOSE 4567 4567

CMD ["/usr/bin/supervisord"]