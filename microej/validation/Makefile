ifeq ($(OS),Windows_NT)
PID_S2S=$(shell cmd /c netstat -ano | findstr /RC:"[0-9]:5555 .*LISTENING")
KILL=taskkill /F /pid $(lastword $(PID_S2S))
else
PID_S2S=$(shell lsof -i :5555 -t)
KILL=kill -9 $(PID_S2S)
endif
TTY?=/dev/ttyACM0

VALIDATIONS ?= CORE_VAL UI_VAL
USAGE ?= eval


$(VALIDATIONS) : clean
	# create build dir
	mkdir build
	# configure cmake
	cd build && cmake .. -DCUR_VAL=$@ -DTTY=$(TTY) -DJUSAGE=$(USAGE) -DVERB=1 -G "Unix Makefiles"
	# start S2S
	cd build && make S2S &
	# start tests
	cd build && make

all: $(VALIDATIONS)

kill_s2s:
ifeq ($(strip $(PID_S2S)),)
	echo "nothing to do"
else
	# kill old S2S if alive
	$(KILL)
endif

clean: kill_s2s
	rm -rf build
