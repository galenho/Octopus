################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../lua_lpeglabel/lplcap.c \
../lua_lpeglabel/lplcode.c \
../lua_lpeglabel/lplprint.c \
../lua_lpeglabel/lpltree.c \
../lua_lpeglabel/lplvm.c 

C_DEPS += \
./lua_lpeglabel/lplcap.d \
./lua_lpeglabel/lplcode.d \
./lua_lpeglabel/lplprint.d \
./lua_lpeglabel/lpltree.d \
./lua_lpeglabel/lplvm.d 

OBJS += \
./lua_lpeglabel/lplcap.o \
./lua_lpeglabel/lplcode.o \
./lua_lpeglabel/lplprint.o \
./lua_lpeglabel/lpltree.o \
./lua_lpeglabel/lplvm.o 

# Each subdirectory must supply rules for building sources it contributes
lua_lpeglabel/%.o: ../lua_lpeglabel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''
