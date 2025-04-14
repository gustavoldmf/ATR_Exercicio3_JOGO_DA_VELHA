# Use the official Ubuntu image as the base
FROM ubuntu:22.04

# Install necessary dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    cmake \
    build-essential \
    gdb \
    git \
    sudo \
    curl \
    wget \
    clang \
    lldb \
    python3 \
    python3-pip \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Add a non-root user
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Install additional tools (as vscode-cpptools would need)
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    gcc-multilib \
    g++-multilib \
    && rm -rf /var/lib/apt/lists/*

# Optional: Install CMake if a newer version is needed than what's in Ubuntu repos
# RUN wget -qO- https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.tar.gz | \
#     tar xz --strip-components=1 -C /usr/local

# Switch to the non-root user
USER $USERNAME

# Set up the working directory
WORKDIR /workspace

# Optional: Configure GDB to run as root for debugging (matches your seccomp=unconfined)
RUN echo "set substitute-path /workspace /workspace" > /home/$USERNAME/.gdbinit
