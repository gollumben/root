FROM ubuntu

#this is the preamble, which must be after the first "FROM" statement
MAINTAINER benbruers
ARG version="v6-16-00"
LABEL description="CERN ROOT for Ubuntu"
LABEL version="${version}"

#install required packages
COPY packages packages

RUN apt-get update -qq \
 && ln -sf /usr/share/zoneinfo/UTC /etc/localtime \
 && apt-get -y install $(cat packages) \
 && localedef -i en_US -f UTF-8 en_US.UTF-8 \
 && rm -rf /packages /var/lib/apt/lists/*

#build ROOT
RUN cd /tmp \
 && git clone https://github.com/root-project/root /usr/src/root \
 && cd /usr/src/root \
 && git checkout tags/${version} \
 && cd - \
 && cmake /usr/src/root \
	-Dall=ON \
	-Dcxx14=ON \
	-Dfail-on-missing=ON \
	-Dgnuinstall=ON \
	-Drpath=ON \
	-Dbuiltin_afterimage=OFF \
	-Dbuiltin_ftgl=OFF \
	-Dbuiltin_gl2ps=OFF \
	-Dbuiltin_glew=OFF \
	-Dbuiltin_unuran=ON \
	-Dbuiltin_vc=ON \
	-Dbuiltin_vdt=ON \
	-Dbuiltin_veccore=ON \
	-Dbuiltin_xrootd=ON \
	-Darrow=OFF \
	-Dcastor=OFF \
	-Dchirp=OFF \
	-Dgeocad=OFF \
	-Dglite=OFF \
	-Dhdfs=OFF \
	-Dmonalisa=OFF \
	-Doracle=OFF \
	-Dpythia6=OFF \
	-Drfio=OFF \
	-Dsapdb=OFF \
	-Dsrp=OFF \
	-Dbuiltin_tbb=ON \
 && cmake --build . -- -j$(nproc --ignore=4) \
 && cmake --build . --target install \
 && rm -rf /tmp/* /usr/src/root

#setup the Python path
ENV PYTHONPATH /usr/local/lib/root
#CMD root -b
