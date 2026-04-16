#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=main.c irq_handlers.c platform.c usart.c locomotion.c wireless.c check_sensors.c wall_following.c line_following.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/irq_handlers.o ${OBJECTDIR}/platform.o ${OBJECTDIR}/usart.o ${OBJECTDIR}/locomotion.o ${OBJECTDIR}/wireless.o ${OBJECTDIR}/check_sensors.o ${OBJECTDIR}/wall_following.o ${OBJECTDIR}/line_following.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/irq_handlers.o.d ${OBJECTDIR}/platform.o.d ${OBJECTDIR}/usart.o.d ${OBJECTDIR}/locomotion.o.d ${OBJECTDIR}/wireless.o.d ${OBJECTDIR}/check_sensors.o.d ${OBJECTDIR}/wall_following.o.d ${OBJECTDIR}/line_following.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/irq_handlers.o ${OBJECTDIR}/platform.o ${OBJECTDIR}/usart.o ${OBJECTDIR}/locomotion.o ${OBJECTDIR}/wireless.o ${OBJECTDIR}/check_sensors.o ${OBJECTDIR}/wall_following.o ${OBJECTDIR}/line_following.o

# Source Files
SOURCEFILES=main.c irq_handlers.c platform.c usart.c locomotion.c wireless.c check_sensors.c wall_following.c line_following.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32CM5164LS00048
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  .generated_files/flags/default/93239b3692655fd2c5b6cfe31f38758a84abd71b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/irq_handlers.o: irq_handlers.c  .generated_files/flags/default/4ee19a62eeb224779da13048ee0415bad4d0765b .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/irq_handlers.o.d 
	@${RM} ${OBJECTDIR}/irq_handlers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/irq_handlers.o.d" -o ${OBJECTDIR}/irq_handlers.o irq_handlers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/platform.o: platform.c  .generated_files/flags/default/ecf3c1cac5bf7aaf23f765c4271e57e28e6649ec .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/platform.o.d 
	@${RM} ${OBJECTDIR}/platform.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/platform.o.d" -o ${OBJECTDIR}/platform.o platform.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/usart.o: usart.c  .generated_files/flags/default/6cce2880249fc2fd29e05cc3522b47b89e9f3440 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/usart.o.d 
	@${RM} ${OBJECTDIR}/usart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/usart.o.d" -o ${OBJECTDIR}/usart.o usart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/locomotion.o: locomotion.c  .generated_files/flags/default/e0e4385859e111925b5d90d1421424b35e763f48 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/locomotion.o.d 
	@${RM} ${OBJECTDIR}/locomotion.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/locomotion.o.d" -o ${OBJECTDIR}/locomotion.o locomotion.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/wireless.o: wireless.c  .generated_files/flags/default/f16ad3d068b5d940f58c75f8311fe03d8d8af958 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wireless.o.d 
	@${RM} ${OBJECTDIR}/wireless.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/wireless.o.d" -o ${OBJECTDIR}/wireless.o wireless.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/check_sensors.o: check_sensors.c  .generated_files/flags/default/2771455d0496879aa270ffb6ac420d7c542ae45c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/check_sensors.o.d 
	@${RM} ${OBJECTDIR}/check_sensors.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/check_sensors.o.d" -o ${OBJECTDIR}/check_sensors.o check_sensors.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/wall_following.o: wall_following.c  .generated_files/flags/default/4b5fba842fab37bc1729cba0510d94cab2c2f645 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wall_following.o.d 
	@${RM} ${OBJECTDIR}/wall_following.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/wall_following.o.d" -o ${OBJECTDIR}/wall_following.o wall_following.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/line_following.o: line_following.c  .generated_files/flags/default/7ed6a6297656a1e597589f7d4ea6fc142017ac05 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/line_following.o.d 
	@${RM} ${OBJECTDIR}/line_following.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/line_following.o.d" -o ${OBJECTDIR}/line_following.o line_following.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
else
${OBJECTDIR}/main.o: main.c  .generated_files/flags/default/c74772256a28840e3024f11362ea59cf5f9dbab9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/irq_handlers.o: irq_handlers.c  .generated_files/flags/default/9dfe86771269ed830313a0ea7f4276031e8b465c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/irq_handlers.o.d 
	@${RM} ${OBJECTDIR}/irq_handlers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/irq_handlers.o.d" -o ${OBJECTDIR}/irq_handlers.o irq_handlers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/platform.o: platform.c  .generated_files/flags/default/f8e0ee097187433da06034a46f2a646387e36366 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/platform.o.d 
	@${RM} ${OBJECTDIR}/platform.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/platform.o.d" -o ${OBJECTDIR}/platform.o platform.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/usart.o: usart.c  .generated_files/flags/default/f2afbe2c98a8fc55c9ad5129df2cdd01e37e21ba .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/usart.o.d 
	@${RM} ${OBJECTDIR}/usart.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/usart.o.d" -o ${OBJECTDIR}/usart.o usart.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/locomotion.o: locomotion.c  .generated_files/flags/default/c1c0c2baf51eab498d7bd273ac9ab65412119540 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/locomotion.o.d 
	@${RM} ${OBJECTDIR}/locomotion.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/locomotion.o.d" -o ${OBJECTDIR}/locomotion.o locomotion.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/wireless.o: wireless.c  .generated_files/flags/default/ecf016ea0fede22cda6952c32c1b94b391c3f7b9 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wireless.o.d 
	@${RM} ${OBJECTDIR}/wireless.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/wireless.o.d" -o ${OBJECTDIR}/wireless.o wireless.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/check_sensors.o: check_sensors.c  .generated_files/flags/default/12841202b745e7ca79842886d52f5ba8680f47fa .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/check_sensors.o.d 
	@${RM} ${OBJECTDIR}/check_sensors.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/check_sensors.o.d" -o ${OBJECTDIR}/check_sensors.o check_sensors.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/wall_following.o: wall_following.c  .generated_files/flags/default/b19487fb545318785b7132abe15e984e58725e4d .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wall_following.o.d 
	@${RM} ${OBJECTDIR}/wall_following.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/wall_following.o.d" -o ${OBJECTDIR}/wall_following.o wall_following.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/line_following.o: line_following.c  .generated_files/flags/default/e7b876c81faa0b35ce4a51125fd6ddcbefb23397 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/line_following.o.d 
	@${RM} ${OBJECTDIR}/line_following.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MP -MMD -MF "${OBJECTDIR}/line_following.o.d" -o ${OBJECTDIR}/line_following.o line_following.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}/PIC32CM-LS00" ${PACK_COMMON_OPTIONS} 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/PIC32CM-LS00"
	
else
${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Code.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/PIC32CM-LS00"
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/Code.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
