################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../lua_aoi.cpp \
../lua_mongo.cpp \
../lua_mysql.cpp \
../lua_network.cpp \
../lua_octopus.cpp \
../main.cpp 

CPP_DEPS += \
./lua_aoi.d \
./lua_mongo.d \
./lua_mysql.d \
./lua_network.d \
./lua_octopus.d \
./main.d 

C_SRCS += \
../lua_filesystem.c \
../lua_pb.c \
../lua_seri.c 

C_DEPS += \
./lua_filesystem.d \
./lua_pb.d \
./lua_seri.d 

OBJS += \
./lua_aoi.o \
./lua_filesystem.o \
./lua_mongo.o \
./lua_mysql.o \
./lua_network.o \
./lua_octopus.o \
./lua_pb.o \
./lua_seri.o \
./main.o 

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''
