################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/checking.cpp \
../src/foward_test.cpp \
../src/get_random_array.cpp \
../src/main_Room.cpp \
../src/main_server_frame.cpp \
../src/mysql_check.cpp \
../src/order.cpp \
../src/sub_Room.cpp 

OBJS += \
./src/checking.o \
./src/foward_test.o \
./src/get_random_array.o \
./src/main_Room.o \
./src/main_server_frame.o \
./src/mysql_check.o \
./src/order.o \
./src/sub_Room.o 

CPP_DEPS += \
./src/checking.d \
./src/foward_test.d \
./src/get_random_array.d \
./src/main_Room.d \
./src/main_server_frame.d \
./src/mysql_check.d \
./src/order.d \
./src/sub_Room.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


