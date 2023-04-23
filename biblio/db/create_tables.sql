create table videos
(
    id serial,
    title varchar(128) not null,
    video_type varchar(32) not null,
    country varchar(64) not null,
    year smallint not null check (year > 1900),
    genre varchar(64) not null,
    rating varchar(4),
    short_desc varchar(128)
);

alter table videos add constraint pk_id primary key (id);

create table video_details
(
    id serial,
    alt_title varchar(128),
    orig_lang varchar(32) not null,
    trans_lang varchar(32),
    files_num smallint not null check (files_num > 0),
    total_files_size varchar(8) not null check (total_files_size > 0),
    length smallint not null check (length > 0),
    rate smallint not null check (rate > 0),
    imdb_link varchar(128),
    long_desc varchar(1024)
);

alter table video_details add constraint fk_video_details foreign key (id) references videos (id);
