################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/helperFunctions.c \
../src/linkedList.c \
../src/terminote2.c \
../src/terminoteFunctions.c 

OBJS += \
./src/helperFunctions.o \
./src/linkedList.o \
./src/terminote2.o \
./src/terminoteFunctions.o 

C_DEPS += \
./src/helperFunctions.d \
./src/linkedList.d \
./src/terminote2.d \
./src/terminoteFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=c99 -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


