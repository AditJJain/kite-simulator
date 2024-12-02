CXX = g++
CXXFLAGS = -Wall -std=c++23
SRCS = main.cpp Core/UserLogin/login.cpp Core/Menu/mainmenu.cpp Core/Portfolio/portfolio.cpp
OBJS = $(SRCS:%.cpp=%.o)
EXEC = kite-simulator

# Create the target executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp files into .o files in their respective directories
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I . -I Core/Menu -I Core/UserLogin -I Core/Portfolio

clean:
	rm -f $(OBJS) $(EXEC)