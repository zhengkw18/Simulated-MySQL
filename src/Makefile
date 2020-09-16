CROSS =   
CXX = $(CROSS)g++  
CFLAGS = -Wall -c -std=c++11

RM = -rm

SRCS = $(wildcard *.cpp)  

OBJS = $(patsubst %.cpp, %.o, $(SRCS))  

VERSION = 1.0.0.0  
TARGET = main1 main2
  
all:$(TARGET)  
	
$(TARGET) : $(OBJS)  
	$(CXX) $^ -o $@
  
$(OBJS):%.o : %.cpp  
	$(CXX) $(CFLAGS) $< -o $@
.PHONY : clean
clean :  
	$(RM) $(TARGET) $(OBJS)  