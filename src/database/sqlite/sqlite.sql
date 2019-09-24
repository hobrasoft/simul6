begin;

create table version (
    version int
);

insert into version values (0);

create table mix (
    id      int     primary key,
    json    text
);

create table controls (
    json    text
);

create table graf (
    time    numeric not null,
    id      int     not null references mix(id) on update cascade on delete cascade,
    values_array text
);

commit;
