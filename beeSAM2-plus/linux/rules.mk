#################################################################################
PWD     :=pwd
MKDIR   :=mkdir -p
RM      :=rm -f
MV      :=mv

#################################################################################
FindSameNameModuleDir =$(filter $(basename $(notdir $(1))),$(2))

#Probe Directory
#If directory exist then return directory else return empty.
FunPD =$(if $(shell test -d $(firstword $(1)) && echo $$?),$(firstword $(1)),)

#Probe Directory and Mkdir Directory
#If directory does not exist then creat it.
FunPDMD =$(if $(call FunPD,$(1)),,$(shell $(MKDIR) $(firstword $(1))))

#################################################################################
DEST_BIN :=$(OUT_DIR)/$(DEST_BIN)

SRCs     :=$(sort $(SRCs))
#DEPENDs  :=$(subst $(PRJ_DIR)/,$(OUT_DIR)/,$(patsubst %.s,%.d,$(patsubst %.d,%.o,$(patsubst %.cpp,%.d,$(SRCs)))))
OBJs     :=$(subst $(PRJ_DIR)/,$(OUT_DIR)/,$(patsubst %.s,%.o,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCs)))))

#################################################################################
# Output Directory
ACCESS_DIRs =$(sort $(dir $(DEPENDs)) $(dir $(OBJs)))

ifneq ($(ACCESS_DIRs),)
    _TmpStr :=$(foreach dir,$(ACCESS_DIRs),$(call FunPDMD,$(dir)))
else
    $(error ACCESS_DIRs is empty!)
endif

#################################################################################
AR       :=ar
AS       :=as
CC       :=gcc
CXX      :=g++
CO       :=
CPP      :=$(CXX)
FC       :=
GET      :=
LD       :=ld
LEX      :=
PC       :=
YACC     :=
YACCR    :=
MAKEINFO :=
TEX      :=
TEXI2DVI :=
WEAVE    :=
CWEAVE   :=
TANGLE   :=
CTANGLE  :=
#--------------------------------------------------------------------------------
AR       :=$(CROSS)$(AR)
AS       :=$(CROSS)$(AS)
CC       :=$(CROSS)$(CC)
CXX      :=$(CROSS)$(CXX)
CO       :=$(CROSS)$(CO)
CPP      :=$(CROSS)$(CPP)
FC       :=$(CROSS)$(FC)
GET      :=$(CROSS)$(GET)
LD       :=$(CROSS)$(LD)
LEX      :=$(CROSS)$(LEX)
PC       :=$(CROSS)$(PC)
YACC     :=$(CROSS)$(YACC)
YACCR    :=$(CROSS)$(YACCR)
MAKEINFO :=$(CROSS)$(MAKEINFO)
TEX      :=$(CROSS)$(TEX)
TEXI2DVI :=$(CROSS)$(TEXI2DVI)
WEAVE    :=$(CROSS)$(WEAVE)
CWEAVE   :=$(CROSS)$(CWEAVE)
TANGLE   :=$(CROSS)$(TANGLE)
CTANGLE  :=$(CROSS)$(CTANGLE)

#################################################################################
ARFLAGS   :=$(ARFLAGS)
ASFLAGS   :=$(ASFLAGS)
CFLAGS    :=$(CFLAGS) $(INCs)
CXXFLAGS  :=$(CXXFLAGS) $(INCs)
COFLAGS   :=$(COFLAGS)
CPPFLAGS  :=$(CPPFLAGS) $(INCs)
FFLAGS    :=$(FFLAGS)
GFLAGS    :=$(GFLAGS)
CLDFLAGS  :=$(CLDFLAGS) $(LIBs)
CXXLDFLAGS:=$(CXXLDFLAGS) $(LIBs)
LFLAGS    :=$(LFLAGS)
PFLAGS    :=$(PFLAGS)
RFLAGS    :=$(RFLAGS)
YFLAGS    :=$(YFLAGS)

################################################################################
.PHONY: all clean show
all: $(DEST_BIN)

clean:
	$(RM) $(OBJs) $(DEST_BIN)

$(DEST_BIN): $(DEPENDs) $(OBJs)
ifeq ($(strip $(filter-out %.c,$(SRCs))),)
	${CC} $(CLDFLAGS) $(OBJs) -o $@
else
	$(CXX) $(CXXLDFLAGS) $(OBJs) -o $@
endif

$(OUT_DIR)/%.o: $(PRJ_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o: $(PRJ_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

show:
	@echo "PRJ_DIR  =$(PRJ_DIR)"
	@echo "OUT_DIR  =$(OUT_DIR)"
	@echo "CROSS    =$(CROSS)"

	@echo "SRCs     =$(SRCs)"
	@echo "INCs     =$(INCs)"
	@echo "LIBs     =$(LIBs)"

	@echo "OBJs     =$(OBJs)"
	@echo "DEST_BIN =$(DEST_BIN)"

################################################################################
