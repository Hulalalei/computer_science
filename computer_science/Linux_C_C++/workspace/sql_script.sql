# show databases;
# use web_user;
# show tables;

# create table fruits (
#     f_id        char(10)        not null,
#     s_id        int             not null,
#     f_name      char(255)       not null,
#     f_price     decimal(8, 2)   not null,
#     PRIMARY KEY(f_id)
# );

# INSERT INTO fruits (
#     f_id, s_id, f_name, f_price
# ) VALUES ( 'a1', 101, 'apple', 5.2 ),
#          ( 'b1', 101, 'blaceberry', 10.2 ),
#          ( 'bs1', 102, 'orange', 11.2 ),
#          ( 'bs2', 105, 'melon', 8.2 ),
#          ( 't1', 102, 'banana', 10.3 ),
#          ( 't2', 102, 'grape', 5.3 ),
#          ( 'o2', 103, 'coconut', 9.2 ),
#          ( 'c0', 101, 'cherry', 3.2 ),
#          ( 'a2', 103, 'apricot', 2.2 ),
#          ( 'l2', 104, 'lemon', 6.4 ),
#          ( 'b2', 104, 'berry', 7.6 ),
#          ( 'm1', 106, 'mango', 15.7 ),
#          ( 'm2', 105, 'xbabay', 2.6 ),
#          ( 't4', 107, 'xbababa', 3.6 ),
#          ( 'm3', 105, 'xxtt', 11.6 ),
#          ( 'b5', 107, 'xxxx', 3.6 );


SELECT f_id, s_id FROM fruits ORDER BY s_id LIMIT 4;
