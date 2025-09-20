################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../lua_cjson/dtoa.c \
../lua_cjson/g_fmt.c \
../lua_cjson/lua_cjson.c \
../lua_cjson/strbuf.c 

C_DEPS += \
./lua_cjson/dtoa.d \
./lua_cjson/g_fmt.d \
./lua_cjson/lua_cjson.d \
./lua_cjson/strbuf.d 

OBJS += \
./lua_cjson/dtoa.o \
./lua_cjson/g_fmt.o \
./lua_cjson/lua_cjson.o \
./lua_cjson/strbuf.o 

# Each subdirectory must supply rules for building sources it contributes
lua_cjson/%.o: ../lua_cjson/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -I../.. -I../../include -I../../include/lua -I../../include/libmongoc-1.0 -I../../include/libbson-1.0 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ''
