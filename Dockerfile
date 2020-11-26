FROM dbisilm/llvm_pmdk:latest
MAINTAINER Kai-Uwe Sattler <kus@tu-ilmenau.de>


# Set default user
USER $USER
WORKDIR /home/$USER


# Download and prepare project
RUN cd /home/$USER \
 && git clone https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core.git \
 && mkdir poseidon_core/build \
 && cd poseidon_core/build \
 && cmake -DUSE_PMDK=ON -DLLVM_DIR=/usr/lib64/llvm10/lib/cmake/llvm \
 && make
