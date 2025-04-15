
FROM --platform=linux/amd64 ubuntu:22.04

# Install necessary dependencies
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
    gcc-multilib \
    build-essential \
    libc6-dev-i386 \
    wget \
    curl \
    unzip \
    apt-transport-https \
    software-properties-common \
    ca-certificates \
		file \
		gcc 

# Install PowerShell
RUN curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > microsoft.gpg && \
    install -o root -g root -m 644 microsoft.gpg /etc/apt/trusted.gpg.d/ && \
    sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/microsoft-ubuntu-jammy-prod jammy main" > /etc/apt/sources.list.d/microsoft.list' && \
    apt-get update && \
    apt-get install -y powershell && \
    rm microsoft.gpg

# Set working directory
WORKDIR /code

# Default shell
CMD ["pwsh"]
