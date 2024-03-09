make:
	g++ -I /usr/local/include -std=c++20 -o main main.cpp -L /home/lucio02/my-muparsex/muparserx/build -lmuparserx
clean:
	rm main
