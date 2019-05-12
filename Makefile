CFLAGS = -g
LNFLAGS = -g
LIBS = -lgcc -lm -lwiringPi
OBJS = Adafruit_Crickit.o  Adafruit_seesaw.o  StepperMotor.o  stepper_test.o
LINK_TARGET = stepper_test
REBUILDABLES = $(OBJS) $(LINK_TARGET)

all: $(LINK_TARGET)

$(LINK_TARGET): $(OBJS)
	 g++ $(LNFLAGS) $(LIBS) -o $@ $^

clean:
	 rm -f $(REBUILDABLES)

%.o : %.cpp
	g++ $(CFLAGS) -o $@ -c $<

stepper_test.o : stepper_test.cpp StepperMotor.h
StepperMotor.o : StepperMotor.cpp StepperMotor.h Adafruit_Crickit.h
Adafruit_Crickit.o : Adafruit_Crickit.h Adafruit_seesaw.h
Adafruit_seesaw.o : Adafruit_seesaw.h
