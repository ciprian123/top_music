--drop table users;
create table USERS (
	user_id integer primary key autoincrement default 0,
	username text(64) not null,
	password text(64) not null,
	admin_status integer not null default 0,
	comment_status integer not null default 1,
	created_at date not null
);


--drop table SONGS;
create table SONGS (
    song_id integer primary key autoincrement default 0,
	user_id integer not null,
	title text(64) not null,
	author text(128) not null,
	release_year integer not null,
	genre text(128) not null,
	description text(512) not null,
	url text(128) not null,
	no_of_votes integer not null default 0,
	created_at date not null
);


--drop table comments;
create table comments (
	comment_id integer primary key autoincrement default 0,
	user_id integer not null,
	song_id integer not null,
	content text(2048) not null,
	created_at date not null
);


--drop table votes;
create table votes (
	vode_id integer primary key autoincrement default 0,
	user_id integer not null,
	song_id integer not null,
	created_at date not null
);



-- populating user table
INSERT INTO USERS (username, password, admin_status, comment_status, created_at) VALUES ('ciprian', 'password', 1, 1, date('now'));
INSERT INTO USERS (username, password, admin_status, comment_status, created_at) VALUES ('admin', 'admin', 1, 1, date('now'));
INSERT INTO USERS (username, password, admin_status, comment_status, created_at) VALUES ('bob', 'bob123', 0, 1, date('now'));
INSERT INTO users (username, password, admin_status, comment_status, created_at) VALUES ('student', 'student', 0, 0, date('now'));
INSERT INTO users (username, password, admin_status, comment_status, created_at) VALUES ('andrei', 'florinsalam123', 0, 1, date('now'));


-- populating SONGS table
INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Kazi Ploae si Specii - Imperiul Lianelor', 'Kazi Ploae, Specii', 2012, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=JPqOr7aOOHg&list=RDJPqOr7aOOHg&start_radio=1', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Serafim feat. Chimie & Praetor - Fratii Prastie 2', 'Serafim', 2018, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=8-Y7RU69ccA', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Chimie - Povesti despre noroc si ghinion (prod. gAZAh)', 'Chimie', 2013, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=WZs68EYMqz8&list=TLPQMTgwODIwMjBXBhiyALltXw&index=8', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Specii - Diagnoza', 'Specii', 2013, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=lijZg_vIpzs&list=TLPQMTgwODIwMjBXBhiyALltXw&index=15', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Ombladon - Ultimul tren', 'Ombladon', 2016, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=WBXA19ZNsvY', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'CTC - Interzis Nefumatorilor', 'CTC', 2014, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=4d_ox_7P-dY&list=TLPQMTgwODIwMjBXBhiyALltXw&index=21', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'B.U.G. Mafia - Ulei Si Apa (feat. Lalla & So) (Prod. Tata Vlad)', 'B.U.G. Mafia', 2016, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=DbJcD4fbcq8', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Cedry2k - Extreme', 'Cedry2k', 2014, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=syGLnZw5xKI', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Cedry2k - Apocalipsa verbala', 'Cedry2k', 2014, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=J5luey2Xz_Q', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (1, 'Parazitii - Lacrimi de ceara', 'Parazitii', 2019, 'RAP, HIP HOP', '-', 'https://www.youtube.com/watch?v=N9V11n-O9n0', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Thirty Seconds To Mars - Up In The Air', 'Thirty Seconds To Mars', 2013, 'ROCK', '-', 'https://www.youtube.com/watch?v=y9uSyICrtow', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'LOST IN THE ECHO - Linkin Park', 'Linkin Park', 2012, 'ROCK', '-', 'https://www.youtube.com/watch?v=co4YpHTqmfQ', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Marilyn Manson - Sweet Dreams (Are Made Of This) (Alt. Version)', 'Marilyn Manson', 1998, 'ROCK', '-', 'https://www.youtube.com/watch?v=QUvVdTlA23w', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Numb - Linkin Park', 'Linkin Park', 2007, 'ROCK', '-', 'https://www.youtube.com/watch?v=kXYiU_JCYtU', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Evanescence - Bring Me To Life', 'Evanescence', 2007, 'ROCK', '-', 'https://www.youtube.com/watch?v=3YxaaGgTQYM', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Rammstein - Deutschland', 'Rammstein', 2019, 'ROCK', '-', 'https://www.youtube.com/watch?v=NeQM1c-XCDc&has_verified=1', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Rammstein - Ich Will', 'Rammstein', 2015, 'ROCK', '-', 'https://www.youtube.com/watch?v=EOnSh3QlpbQ', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Marilyn Manson - This Is The New Shit', 'Marilyn Manson', 2003, 'ROCK', '-', 'https://www.youtube.com/watch?v=4kQMDSw3Aqo', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Five Finger Death Punch - Wrong Side Of Heaven', 'Five Finger Death Punch', 2014, 'ROCK', '-', 'https://www.youtube.com/watch?v=o_l4Ab5FRwM', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (2, 'Five Finger Death Punch - Gone Away', 'Five Finger Death Punch', 2017, 'ROCK', '-', 'https://www.youtube.com/watch?v=BIQK4-9YFW0', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Peggy Gou - Starry Night', 'Peggy Gou', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=r_wwmmo6UGY', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Busting Out - GRiZ (ft. Bootsy Collins)', 'GRiZ', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=uYCd3_1ZFBM', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Channel Tres - Sexy Black Timberlake', 'Channel Tres', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=KrGmm2jRCPs', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'The Chemical Brothers - Got To Keep On', 'The Chemical Brothers', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=rSYwtllbweY', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Burna Boy - Anybody ', 'Burna Boy', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=so4dgTRaWFk', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Joel Corry x MNEK - Head & Heart', 'Joel Corry, MNEK', 2020, 'DANCE', '-', 'https://www.youtube.com/watch?v=CRuOOxF-ENQk', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'SECRETS - REGARD & RAYE', 'REGARD, RAYE', 2020, 'DANCE', '-', 'https://www.youtube.com/watch?v=6soSrZBI78c', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Jubel - Dancing In The Moonlight (feat. NEIMY)', 'Jubel', 2018, 'DANCE', '-', 'https://www.youtube.com/watch?v=tYwBRlgeK9Q', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Joel Corry - Lonely', 'Joel Corry', 2020, 'DANCE', '-', 'https://www.youtube.com/watch?v=IIG4GgGKdKo', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (3, 'Joel Corry - Sorry', 'Joel Corry', 2019, 'DANCE', '-', 'https://www.youtube.com/watch?v=q_MVDY7nDQE', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Celine Dion - My Heart Will Go On', 'Celine Dion', 1997, 'POP', '-', 'https://www.youtube.com/watch?v=kVLzWqITfMs', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Christina Aguilera - Beautiful', 'Christina Aguilera', 2002, 'POP', '-', 'https://www.youtube.com/watch?v=eAfyFTzZDMM', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Lady Gaga - Bad Romance', 'Lady Gaga', 2009, 'POP', '-', 'https://www.youtube.com/watch?v=qrO4YZeyl0I', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Lady Gaga - Poker Face', 'Lady Gaga', 2010, 'POP', '-', 'https://www.youtube.com/watch?v=bESGLojNYSo', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Maroon 5 - Memoriese', 'Maroon 5', 2019, 'POP', '-', 'https://www.youtube.com/watch?v=SlPhMPnQ58k&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Dua Lipa - Dont Start Now', 'Dua Lipa', 2020, 'POP', '-', 'https://www.youtube.com/watch?v=oygrmJFKYZY&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=2', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'The Weeknd - Blinding Lights', 'The Weeknd', 2020, 'POP', '-', 'https://www.youtube.com/watch?v=4NRXx6U8ABQ&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=3', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Miley Cyrus - Midnight Sky', 'Miley Cyrus', 2020, 'POP', '-', 'youtube.com/watch?v=aS1no1myeTM&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=5', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Lil Mosey - Blueberry Faygo (Dir. by @_ColeBennett_)', 'Lil Mosey', 2020, 'POP', '-', 'https://www.youtube.com/watch?v=V_jHc_n0p9c&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=7', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Harry Styles - Watermelon Sugar', 'Harry Styles', 2020, 'POP', '-', 'https://www.youtube.com/watch?v=E07s5ZYygMg&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=8', 0, DATETIME('now','localtime'));

INSERT INTO SONGS (user_id, title, author, release_year, genre, description, url, no_of_votes, created_at) 
VALUES (4, 'Doja Cat - Say So', 'Doja Cat', 2020, 'POP', '-', 'https://www.youtube.com/watch?v=pok8H_KF1FA&list=PL4o29bINVT4EG_y-k5jGoOu3-Am8Nvi10&index=9', 0, DATETIME('now','localtime'));



-- populating comments table
-- select * from comments c ;
-- SELECT * from SONGS s ;

INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 1, 'I like this SONGS very much. Is one of my favorite!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 2, 'This SONGS makes me feel a little bit gangsta :)!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 3, 'Omg, this is very deep', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 11, 'I dont like this at all!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 12, 'Like if you listen this in 2020!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (2, 1, 'This SONGS is one of the greatest I have ever listened to!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (3, 1, 'This is a piece of art!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 13, 'Very nice flow indeed!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (4, 1, 'Old but gold, keep it up!', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 39, 'You hear this the first time and you know that will be a classic', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (2, 39, 'That SONGS remind me He-Man ....if you remember that old school cartoon give a like', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (3, 39, 'I want the next Cyberpunk 2077 trailer to have this SONGS', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (4, 39, 'I legit thought this was an actual 80s SONGS before eventually realizing it was from this year. And so I got blown off. This is what we need back in the third millenium.', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 19, 'Never compare Marilyn Manson to billie eilish. Dont disrespect this guy.', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (2, 19, 'Marilyn Manson should have been the joker instead of Jared Leto', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (3, 19, 'This SONGS is like if you opened up a shy person and discovered what was really inside........', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (4, 19, 'My father tested me for drugs when he saw I was listnenig to this.', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (1, 32, 'The saddest part is the two old couple lying on the bed when the water is flowing in their room ', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (2, 32, 'It is hard to believe that this movie is actually 23 years old.', DATETIME('now', 'localtime'));
INSERT INTO comments (user_id, song_id, content, created_at) VALUES (3, 32, 'The greatest movie of all time!!! Very nice song also..', DATETIME('now', 'localtime'));

