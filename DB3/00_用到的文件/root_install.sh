#! /bin/bash -e
## author:ywh
## install openGauss

## preparation
function set_password(){
	echo "input your user password:"
	read user_password
	echo $user_password | passwd --stdin omm
}

function download_source(){
	mkdir -p /opt/software/openGauss/data
	cd /opt/software
	if [ -f "openGauss-third_party_binarylibs.tar.gz" ]; then
		echo "openGauss already download"
	else
		wget https://opengauss.obs.cn-south-1.myhuaweicloud.com/2.1.0/openGauss-third_party_binarylibs.tar.gz
		tar -zxvf openGauss-third_party_binarylibs.tar.gz
		mv openGauss-third_party_binarylibs binarylibs
	fi
	yum install git -y --nogpgcheck
	if [ -d "openGauss-server" ]; then
		echo "openGauss-server already download"
	else
		git clone https://gitee.com/xzp-blog/openGauss-server.git
	fi
}
 
function handle_cmake(){
	cd /usr/local/src/
	CMAKE_FILE="cmake-3.20.0-linux-aarch64"
	
	if [ -f "$CMAKE_FILE.tar.gz" ]; then
		echo "cmake file find"
	else
		echo "!!!!error:cmake file not find"
		exit 1
	fi
	
	if [ -d "$CMAKE_FILE" ]; then
		echo "cmake file finishes decompressing"
	else	
		tar -zxvf cmake-3.20.0-linux-aarch64.tar.gz
	fi
	chmod 755 /usr/local/src/cmake-3.20.0-linux-aarch64/
}

function change_python(){
	cd /usr/bin
	mv python python.bak
	ln -s python3 /usr/bin/python
	chown omm:dbgrp -R /opt/software
	chmod 755 -R /opt/software
}

function database_install(){
	chmod 777 /bin/bash
	cp /root/omm_install.sh /tmp/omm_install.sh
	su - omm -s /bin/bash /tmp/omm_install.sh
	chmod 755 /bin/bash
}




groupadd -g 1000 dbgrp
useradd -g dbgrp -u 1000 -d /home/omm omm

set_password
download_source
## uploading cmake before this step
handle_cmake

yum install libaio-devel ncurses-devel pam-devel libffi-devel libtool libtool-devel libtool-ltdl openssl-devel bison-3.5 golang flex-2.6.1 dkms-2.6.1-5.oe1.noarch python3-devel patch -y --nogpgcheck
yum install "gcc-c++.aarch64" -y

change_python

database_install



