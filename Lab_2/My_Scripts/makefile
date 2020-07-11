CC=$(CROSS_COMPILE)gcc
OBJS:=script.o
script: $(OBJS)
	$(CC) -o script $(CFLAGS) $(LDFLAGS) $(OBJS) -l gpiod
$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@