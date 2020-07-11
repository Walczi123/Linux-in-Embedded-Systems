#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <gpiod.h>

struct gpiod_line_event gpiod_line_event_without_bounce(struct gpiod_line *line, const struct timespec *timeout, struct gpiod_line_event olevent){
	const struct timespec ts = { 0, 200000000 };
	struct gpiod_line_event event;
	int ev_val, new_val, read_val;
	ev_val = gpiod_line_event_wait(line, timeout);
	new_val = 0;

	event = olevent;
	if(ev_val==1){
		read_val = gpiod_line_event_read(line, &event);
		event = gpiod_line_event_without_bounce(line, &ts, event);
	}

	return event;
}

int main(int argc, char** argv) {
	struct timespec ts={ 2,0 };
	struct gpiod_line_event event, start, lab;
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	struct gpiod_line *line_31;
	bool timer = false;
	int seconds, nanoseconds;
	int rv;

	chip = gpiod_chip_open("/dev/gpiochip1");
	if (!chip)
		return EXIT_FAILURE;
	line = gpiod_chip_get_line(chip, 13);
	if (!line) 
	{   
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}
	line_31 = gpiod_chip_get_line(chip, 31);
	if (!line_31) 
	{   
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}

	gpiod_line_request_both_edges_events(line,"foobar");
	gpiod_line_request_output(line_31,"foobar",0);
	while(1){
        event.event_type=0;   
        event = gpiod_line_event_without_bounce(line, &ts, event);
        printf("event: %d\n",  event.event_type);
        if(event.event_type==1){
            if(!timer){
                gpiod_line_set_value(line_31,1);
                timer=true;
                start = event;
            }else{
                gpiod_line_set_value(line_31,0);
                timer=false;
                seconds = event.ts.tv_sec - start.ts.tv_sec;
                nanoseconds = event.ts.tv_nsec - start.ts.tv_nsec;
                if( nanoseconds<0){
                    nanoseconds += 1000000000;
                    seconds -= 1;
                }
                printf("Time: [%8ld.%09ld]\n",  seconds, nanoseconds);
            }
        }
	}
	gpiod_chip_close(chip);
	return EXIT_SUCCESS;
}