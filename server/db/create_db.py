import utils

db_conn = utils.get_connection()
db_conn.autocommit = True
print("starting db script")
curs = db_conn.cursor()
query = open('create_db.sql').read()
curs.execute(query)


