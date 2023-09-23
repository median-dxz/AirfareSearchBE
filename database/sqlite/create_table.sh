if [ -e "./as.db" ]; then
    unlink "./as.db"
fi

sqlite3 as.db < ./schema.sql