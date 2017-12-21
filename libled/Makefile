all: libled.a

libled.a: led.c
	@printf " BUILDING THE LED LIBRARY\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o led.o -c led.c
	$(Q)$(AR) rcs libled.a led.o


BUILD_HOME=../..

include $(BUILD_HOME)/amelin_workshop/Makefile.include
