create table women
(
    key varchar(6) not null,
    name varchar(32) not null,
    date_of_birth varchar(64) not null,
    age integer not null,
    height integer not null,
    height_im varchar(10) not null,
    weight integer not null,
    weight_im integer not null,
    eye_color varchar(10) not null,
    hair_color varchar(10) not null,
    build_body varchar(32) not null,
    education varchar(32) not null,
    profession varchar(32) not null,
    occupation varchar(32) not null,
    smokes char(3) not null,
    zodiac varchar(16),
    marital varchar(16) not null,
    children integer,
    own_words varchar(1024),
    partner varchar(1024),
    partner_age varchar(32),
    location varchar(64),
    credits integer
);

alter table women add constraint pk_women primary key (key);


create table children
(
    parent_key varchar(6) not null,
    name varchar(32) not null,
    age integer not null,
    sex char(1) not null
);

alter table children add constraint fk_children foreign key (parent_key) references women (key);


create table users
(
    login varchar(32) not null,
    password varchar(128) not null
);

alter table users add constraint pk_users primary key (login);


create table ranks
(
    key varchar(6) not null,
    login varchar(32) not null,
    rank_face integer,
    rank_body integer,
    rank_legs integer,
    rank_breasts integer,
    rank_overall integer,
    comment varchar(1024)
);

alter table ranks add constraint fk_ranks_fk foreign key (key) references women (key);
alter table ranks add constraint fk_ranks_lg foreign key (login) references users (login);


create table images
(
    id integer not null,
    key varchar(6) not null,
    path varchar(1024) not null,
    type integer
);

alter table images add constraint pk_images primary key (id);
alter table images add constraint fk_images_fk foreign key (key) references women (key);


create table meta_info
(
    id integer not null,
    key varchar(6) not null,
    pregnancy integer
);

alter table meta_info add constraint pk_meta_info primary key (id);
alter table meta_info add constraint fk_meta_info_fk foreign key (key) references women (key);
