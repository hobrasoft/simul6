begin;

create table version (
    version int
);

insert into version values (0);

create table json (
    json    text
);

create table stepdata (
    time         numeric not null,
    internal_id  int     not null,
    values_array text,
    primary key (time, internal_id)
);

commit;
