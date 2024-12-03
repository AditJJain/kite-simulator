CXX = g++
CXXFLAGS = -Wall -std=c++23
SRCS = main.cpp Core/UserLogin/login.cpp Core/Menu/mainmenu.cpp Core/APIs/apicall_KiteConnect_OHLC.cpp Core/APIs/apicall_KiteConnect_LTP.cpp Core/setNonBlockingMode.cpp Core/Orders/buy-sell.cpp Core/Portfolio/portfolio.cpp Core/Positions/intraday.cpp Core/Watchlist/watchlist.cpp Core/Funds/funds.cpp
OBJS = $(SRCS:%.cpp=%.o)
EXEC = kite-simulator

# Create the target executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp files into .o files in their respective directories
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I . -I Core/Menu -I Core/UserLogin -I Core/APIs -I Core/APIs -I Core -I Core/Orders -I Core/Portfolio -I Core/Positions -I Core/Watchlist -I Core/Funds

clean:
	rm -f $(OBJS) $(EXEC)