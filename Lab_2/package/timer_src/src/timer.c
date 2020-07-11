#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/gpio.h>
#include <gpiod.h>

struct gpiod_line_event gpiod_line_event_without_bounce(struct gpiod_line *line, const struct timespec *timeout, struct gpiod_line_event oldevent){
	const struct timespec ts = { 0, 200000000 };
	struct gpiod_line_event event;
	int ev_val, new_val, read_val;
	ev_val = gpiod_line_event_wait(line, timeout);
	new_val = 0;

	event = oldevent;
	if(ev_val==1){
		read_val = gpiod_line_event_read(line, &event);
		event = gpiod_line_event_without_bounce(line, &ts, event);
	}

	return event;
}

struct gpiod_line_event bulk_gpiod_line_event_without_bounce(struct gpiod_line_bulk bulk, const struct timespec *timeout, struct gpiod_line_event oldevent)
{
	const struct timespec ts = { 0, 200000000 };
	struct gpiod_line_event event;
	int ev_val, new_val, read_val;
	struct gpiod_line_bulk result;
	struct gpiod_line *line;
	ev_val = gpiod_line_event_wait_bulk(&bulk, timeout, &result);
	line = result.lines[0];
	new_val = 0;
	
	event = oldevent;
	if(ev_val == 1)
	{
		read_val = gpiod_line_event_read(line, &event);
		event = gpiod_line_event_without_bounce(line, &ts, event);

		if(line == bulk.lines[0])
			event.event_type = 1;
		else if(line == bulk.lines[1])
		{
			event.event_type = 2;
		}
		else if(line == bulk.lines[2])
		{
			event.event_type = 3;
		}
	}

	return event;
}

int main(int argc, char** argv) {
	printf("LINES LAB2 by Walczak Patryk\n");
	struct timespec ts={ 2,0 };
	struct gpiod_line_event event, start, lab;
	struct gpiod_chip *chip;
	struct gpiod_line *line1;
	struct gpiod_line *line2;	
	struct gpiod_line_bulk buttons;
	bool timer = false, isLab = false;
	int seconds, nanoseconds;
	int rv;

	unsigned int buttons_num = 3;
	unsigned int buttons_offset[] = {12,13,14};
	const int buttons_initial[] = {0,0,0};


	chip = gpiod_chip_open("/dev/gpiochip1");
	if (!chip)
		return EXIT_FAILURE;


	if(gpiod_chip_get_lines(chip, buttons_offset, buttons_num, &buttons)){
		printf("Error: get_lines -> buttons\n");  
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}

	if(gpiod_line_request_bulk_falling_edge_events(&buttons, "its_me")){
		printf("Error: line_request_bulk_falling_edge_events -> buttons\n");  
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}

	line1 = gpiod_chip_get_line(chip, 24);
	if (!line1) 
	{    
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}

	line2 = gpiod_chip_get_line(chip, 25);
	if (!line2) 
	{    
		gpiod_chip_close(chip);
		return EXIT_FAILURE;
	}

	gpiod_line_request_output(line1, "its_me", 0);
	gpiod_line_request_output(line2, "its_me", 0);

	while(1){
		event.event_type = 0;
		event = bulk_gpiod_line_event_without_bounce(buttons, &ts, event);
		// printf("event: %i\n",  event.event_type);
		if(event.event_type == 1)
		{
			if(!timer){
				printf("Timer started!\n");
				timer = true;
				start = event;
				lab = event;
				gpiod_line_set_value(line1, 1);
			}
			else{
				timer = false;
				seconds = event.ts.tv_sec - start.ts.tv_sec;
				nanoseconds = event.ts.tv_nsec - start.ts.tv_nsec;
				if(nanoseconds < 0){
					nanoseconds += 1000000000;
					seconds -= 1;
				}
				printf("Time: [%8ld.%09ld]\n", seconds, nanoseconds);
				gpiod_line_set_value(line1, 0);
			}
		}
		else if(event.event_type == 2)
		{
			if(timer){
				seconds = event.ts.tv_sec - lab.ts.tv_sec;
				nanoseconds = event.ts.tv_nsec - lab.ts.tv_nsec;
				if(nanoseconds < 0){
					nanoseconds += 1000000000;
					seconds -= 1;
				}
				printf("Lap: [%8ld.%09ld]\n", seconds, nanoseconds);
				lab = event;
				isLab = true;
				gpiod_line_set_value(line2, 1);
			}
		}
		if(event.event_type == 0 && isLab){
			isLab = false;
			gpiod_line_set_value(line2, 0);
		}
		if(event.event_type == 3){
			printf("See you\n");
			gpiod_line_set_value(line2, 0);
			gpiod_line_set_value(line1, 0);
			break;
		}
	}
	gpiod_chip_close(chip);
	return EXIT_SUCCESS;
}