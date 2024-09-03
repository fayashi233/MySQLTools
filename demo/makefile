all:demo1 demo2 demo3 demo4 demo5 demo6 demo7

flags=-lmysqlclient -I/usr/liclude/mysql -L/usr/lib64/mysql -I.

demo1:demo1.cpp
	g++ -o demo1 demo1.cpp $(flags)

demo2:demo2.cpp
	g++ -o demo2 demo2.cpp $(flags)

demo4:demo4.cpp
	g++ -o demo4 demo4.cpp $(flags)

demo3:demo3.cpp
	g++ -o demo3 demo3.cpp $(flags)

demo5:demo5.cpp ../MySQLTools.cpp
	g++ -o demo5 demo5.cpp ../MySQLTools.cpp $(flags) -g

demo6:demo6.cpp ../MySQLTools.cpp
	g++ -o demo6 demo6.cpp ../MySQLTools.cpp $(flags) -g

demo7:demo7.cpp ../MySQLTools.cpp
	g++ -o demo7 demo7.cpp ../MySQLTools.cpp $(flags) -g

clean:
	rm demo1 demo2 demo3 demo4 demo5 demo6 demo7
