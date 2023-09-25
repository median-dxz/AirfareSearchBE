if ! [ -d ~/as_data ]; then
    mkdir -p ~/as_data
fi

if [ -e ~/as_data/as.db ]; then
    unlink ~/as_data/as.db
fi

script_dir="$(dirname "$(readlink -f "$0")")"
cd $script_dir

create_table_sql=$(realpath ./schema.sql)
import_data_sql=$(realpath ../build/import_data.sql)

echo "start create tabel"
cd ~/as_data/ && \
sqlite3 as.db < $create_table_sql

echo "start import data"
cd ~/as_data/ && \
sqlite3 as.db < $import_data_sql