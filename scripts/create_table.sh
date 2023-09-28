data_dir=$(realpath /var/as_data)

if ! [ -d $data_dir ]; then
    mkdir -p $data_dir
fi

if [ -e $data_dir/as.db ]; then
    unlink $data_dir/as.db
fi

script_dir="$(dirname "$(readlink -f "$0")")"
cd $script_dir

create_table_sql=$(realpath ./schema.sql)
import_data_sql=$(realpath ../build/import_data.sql)

echo "start create tabel"
cd $data_dir/ && \
sqlite3 as.db < $create_table_sql

echo "start import data"

start_time=$(date +%s)

cd $data_dir/ && \
sqlite3 as.db < $import_data_sql

end_time=$(date +%s)
elapsed_time=$((end_time - start_time))
echo "Elapsed time: $elapsed_time seconds"