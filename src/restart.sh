echo "running restart file..."
pg_ctl -D $PGDATA -l logfile stop
make | tee gmake.out
make install | tee gmake_install.out
pg_ctl -D $PGDATA -l logfile start
psql test
#select * from hj;
#insert into hj values(3);