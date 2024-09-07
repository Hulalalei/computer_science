debugf :
	@g++ test.cpp ConnectionPool.cpp MysqlConn.cpp -std=c++11 -l pthread -l jsoncpp -l mysqlclient -l dl -o test

debugs :
	@g++ main.cpp -std=c++11 -l mysqlclient -l pthread -l dl -o main
.PHONY : debugf debugs