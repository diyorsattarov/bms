# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04 as builder

# Set the working directory inside the container
WORKDIR /usr/src/app

# Update and install necessary packages without prompts
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -yq build-essential cmake wget libssl-dev

# Download and extract Boost 1.84
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/boost_1_84_0.tar.gz && \
    tar -zxvf boost_1_84_0.tar.gz

# Build and install Boost 1.84
WORKDIR /usr/src/app/boost_1_84_0
RUN ./bootstrap.sh --with-libraries=all && ./b2 install

# Download httplib from GitHub
RUN wget https://github.com/yhirose/cpp-httplib/archive/master.tar.gz && \
    tar -zxvf master.tar.gz && \
    mv cpp-httplib-master /usr/src/app/httplib

# Copy the source code into the container
WORKDIR /usr/src/app
COPY . .

# Build the executable
RUN cmake -Bbuild -H. && cmake --build build -- -j$(nproc)

# Use a minimal image for runtime
FROM ubuntu:20.04

# Set the working directory inside the container
WORKDIR /usr/src/app

# Copy only the necessary files from the builder image
COPY --from=builder /usr/src/app/build/http_server .
COPY --from=builder /usr/src/app/httplib /usr/src/app/httplib

# Expose the port your application listens on
EXPOSE 8080

# Specify the command to run on container start
CMD ["./http_server", "0.0.0.0", "8080", "1"]
