def insertCustomer(db_conn, ID, EMAIL, ADDRESS, PHONENUM, NAME, CREDITS):
    query = """INSERT INTO CUSTOMERS
                VALUES (%(ID)s, %(EMAIL)s, %(ADDRESS)s, %(NAME)s, %(CREDITS)s, %(PHONENUM)s)"""
    curs = db_conn.cursor()
    curs.execute(query, locals())
