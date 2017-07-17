#--------------------------------------------------------------------------------
# File Name : Makefile
# Author    : Intercel
# Version   : 0.0.0.001
# Date      : 2013/12/23
# Time      : 15:07
#	............................................................................
# ..............................................................................
# ..............................................................................
# ............................................................................
#--------------------------------------------------------------------------------
#################################################################################
DEST_BIN    :=Ip_Camera
PRJ_DIR     :=../..
OUT_DIR     :=$(PRJ_DIR)/linux/out
CROSS       :=arm-none-linux-gnueabi-

#################################################################################
SRCs :=$(PRJ_DIR)/beeSAM2/ip_camera/ip_camera.c
INCs :=-I$(PRJ_DIR)/beeSAM2/ip_camera
LIBs :=-lpthread -lm -lrt -lcrypto

# For Wsclient
SRCs += $(PRJ_DIR)/beeSAM2/wsclient/base64.c
SRCs += $(PRJ_DIR)/beeSAM2/wsclient/sha1.c
SRCs += $(PRJ_DIR)/beeSAM2/wsclient/wsclient.c
INCs += -I$(PRJ_DIR)/beeSAM2/wsclient
LIBs += 

#--------------------------------------------------------------------------------
# For Sub-Module
SRCs +=
INCs +=
LIBs +=





#################################################################################
ARFLAGS   :=
ASFLAGS   :=
#CFLAGS   :=-g -p -O0
CFLAGS    :=-Os -Wall -Wextra
#CFLAGS    :=-Os -Wall -Wextra -std=c99
#CXXFLAGS :=-g -p -O0
CXXFLAGS  :=-Os -Wall -Wextra
COFLAGS   :=
CPPFLAGS  :=
FFLAGS    :=
GFLAGS    :=
CLDFLAGS  :=
CXXLDFLAGS:=
LFLAGS    :=
PFLAGS    :=
RFLAGS    :=
YFLAGS    :=

#################################################################################
include rules.mk
