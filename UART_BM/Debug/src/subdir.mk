################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/CRC.c \
../src/Client.c \
../src/Hello_world.c \
../src/UART_EIVE_Protocol_Flags.c \
../src/UART_EIVE_Protocol_Recv.c \
../src/UART_EIVE_Protocol_Send.c \
../src/UART_io.c 

OBJS += \
./src/CRC.o \
./src/Client.o \
./src/Hello_world.o \
./src/UART_EIVE_Protocol_Flags.o \
./src/UART_EIVE_Protocol_Recv.o \
./src/UART_EIVE_Protocol_Send.o \
./src/UART_io.o 

C_DEPS += \
./src/CRC.d \
./src/Client.d \
./src/Hello_world.d \
./src/UART_EIVE_Protocol_Flags.d \
./src/UART_EIVE_Protocol_Recv.d \
./src/UART_EIVE_Protocol_Send.d \
./src/UART_io.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../fsbl_bsp/psu_cortexa53_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


