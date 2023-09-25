if [ -e "./build/database/as.db" ]; then
    unlink "./build/database/as.db"
fi

sql_file=$(realpath ./schema.sql)

cd ../build/database/ && \
sqlite3 as.db < $sql_file