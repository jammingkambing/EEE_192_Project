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
OUTPUT_SUFFIX=null
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=null
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
SOURCEFILES_QUOTED_IF_SPACED=main.c irq_handlers.c platform.c usart.c locomotion.c wireless.c wall_following.c line_following.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/irq_handlers.o ${OBJECTDIR}/platform.o ${OBJECTDIR}/usart.o ${OBJECTDIR}/locomotion.o ${OBJECTDIR}/wireless.o ${OBJECTDIR}/wall_following.o ${OBJECTDIR}/line_following.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/irq_handlers.o.d ${OBJECTDIR}/platform.o.d ${OBJECTDIR}/usart.o.d ${OBJECTDIR}/locomotion.o.d ${OBJECTDIR}/wireless.o.d ${OBJECTDIR}/wall_following.o.d ${OBJECTDIR}/line_following.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/irq_handlers.o ${OBJECTDIR}/platform.o ${OBJECTDIR}/usart.o ${OBJECTDIR}/locomotion.o ${OBJECTDIR}/wireless.o ${OBJECTDIR}/wall_following.o ${OBJECTDIR}/line_following.o

# Source Files
SOURCEFILES=main.c irq_handlers.c platform.c usart.c locomotion.c wireless.c wall_following.c line_following.c

# Pack Options 
PACK_COMMON_OPTIONS=-I "${CMSIS_DIR}/CMSIS/Core/Include"



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

MP_PROCESSOR_OPTION=
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
${OBJECTDIR}/main.o: main.c  .generated_files/flags/default/231741b85bc248ced1a7260c8ebe93060085fbc2 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/irq_handlers.o: irq_handlers.c  .generated_files/flags/default/b227642ab84944d950855ea6b9d9afb7fd85a8d0 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/irq_handlers.o.d 
	@${RM} ${OBJECTDIR}/irq_handlers.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/irq_handlers.o.d" -o ${OBJECTDIR}/irq_handlers.o irq_handlers.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/platform.o: platform.c  .generated_files/flags/default/4f6453290123f95b08bbb7ff1b9d7208aa8afa82 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/platform.o.d 
	@${RM} ${OBJECTDIR}/platform.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/platform.o.d" -o ${OBJECTDIR}/platform.o platform.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/usart.o: usart.c  .generated_files/flags/default/f7324bcfb304330d0641bbd58e60a122a8e5fba .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/usart.o.d 
	@${RM} ${OBJECTDIR}/usart.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/usart.o.d" -o ${OBJECTDIR}/usart.o usart.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/locomotion.o: locomotion.c  .generated_files/flags/default/6ca0fb66d45a37cf46b3304ad1183a54e7e3e0df .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/locomotion.o.d 
	@${RM} ${OBJECTDIR}/locomotion.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/locomotion.o.d" -o ${OBJECTDIR}/locomotion.o locomotion.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/wireless.o: wireless.c  .generated_files/flags/default/b9bf6523f483c876e97ff5417a1f1afaf7c14c3c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wireless.o.d 
	@${RM} ${OBJECTDIR}/wireless.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/wireless.o.d" -o ${OBJECTDIR}/wireless.o wireless.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/wall_following.o: wall_following.c  .generated_files/flags/default/5ab6fe9daecb5151b2069360fb851bcfda51e513 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wall_following.o.d 
	@${RM} ${OBJECTDIR}/wall_following.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/wall_following.o.d" -o ${OBJECTDIR}/wall_following.o wall_following.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/line_following.o: line_following.c  .generated_files/flags/default/5f40c6e0e5184223e8753a57c03c5ccbf8b10dde .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/line_following.o.d 
	@${RM} ${OBJECTDIR}/line_following.o 
	 $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/line_following.o.d" -o ${OBJECTDIR}/line_following.o line_following.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
else
${OBJECTDIR}/main.o: main.c  .generated_files/flags/default/8bf41620567d512a2db4da23a084dfb7618b27c .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/irq_handlers.o: irq_handlers.c  .generated_files/flags/default/34376b255bef881a190c1a67c2f1ac08bb6802a .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/irq_handlers.o.d 
	@${RM} ${OBJECTDIR}/irq_handlers.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/irq_handlers.o.d" -o ${OBJECTDIR}/irq_handlers.o irq_handlers.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/platform.o: platform.c  .generated_files/flags/default/2970c3e110c3e31018ff0437faab6458278601a8 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/platform.o.d 
	@${RM} ${OBJECTDIR}/platform.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/platform.o.d" -o ${OBJECTDIR}/platform.o platform.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/usart.o: usart.c  .generated_files/flags/default/766bbb0b0de66573cd0a0d0098caaea63be134e1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/usart.o.d 
	@${RM} ${OBJECTDIR}/usart.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/usart.o.d" -o ${OBJECTDIR}/usart.o usart.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/locomotion.o: locomotion.c  .generated_files/flags/default/4581900fa544163443be4df33a75d4560d07b4f1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/locomotion.o.d 
	@${RM} ${OBJECTDIR}/locomotion.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/locomotion.o.d" -o ${OBJECTDIR}/locomotion.o locomotion.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/wireless.o: wireless.c  .generated_files/flags/default/3d4c7a482c40b9097f5f64dee2262dedfa2eb4d1 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wireless.o.d 
	@${RM} ${OBJECTDIR}/wireless.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/wireless.o.d" -o ${OBJECTDIR}/wireless.o wireless.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/wall_following.o: wall_following.c  .generated_files/flags/default/db9b3c0268da0ce2e7c7d8df763016900e433d38 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/wall_following.o.d 
	@${RM} ${OBJECTDIR}/wall_following.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/wall_following.o.d" -o ${OBJECTDIR}/wall_following.o wall_following.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
${OBJECTDIR}/line_following.o: line_following.c  .generated_files/flags/default/9f05eddfead8272608193f009d0c5eb47ec73156 .generated_files/flags/default/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/line_following.o.d 
	@${RM} ${OBJECTDIR}/line_following.o 
	 $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -O0 -fno-common -MMD -MF "${OBJECTDIR}/line_following.o.d" -o ${OBJECTDIR}/line_following.o line_following.c    -D=$(CND_CONF)  $(COMPARISON_BUILD)    
	
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
	$(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -D=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" 
	
else
${DISTDIR}/Code.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	$(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Code.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -D=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" 
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
