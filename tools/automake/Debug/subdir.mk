################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../dir_analysis.cpp \
../main.cpp \
../utility.cpp 

CPP_DEPS += \
./dir_analysis.d \
./main.d \
./utility.d 

OBJS += \
./dir_analysis.o \
./main.o \
./utility.o 

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++   -I../../.. -I../../../include -I../../../common -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''
