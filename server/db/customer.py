class Customer():
    def __init__(self, id, dl_id, email, address, name, credits, phone_number):
        self.id = id
        self.dl_id = dl_id
        self.email = email
        self.address = address
        self.name = name
        self.credits = credits
        self.phone_number = phone_number


def insertCustomer(db_conn, ID, EMAIL, ADDRESS, PHONENUM, NAME, CREDITS):
    query = """INSERT INTO CUSTOMERS
                VALUES (%(ID)s, %(EMAIL)s, %(ADDRESS)s, %(NAME)s, %(CREDITS)s, %(PHONENUM)s)"""
    curs = db_conn.cursor()
    curs.execute(query, locals())


def getCustomerByBarcode(db_conn, barcode_id):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER
                   FROM CUSTOMERS WHERE DL_ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [barcode_id])
    for row in curs:
        return Customer(*row)

def getCredits(db_conn, id):
    query = """SELECT CREDITS
                   FROM CUSTOMERS WHERE ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [id])
    for row in curs:
        return row[0]
def updateCredits(db_conn, id, newValue):
    print(newValue)
    query = """UPDATE CUSTOMERS SET CREDITS = %s WHERE ID = %s"""
    curs = db_conn.cursor()
    curs.execute(query, [newValue, id])
    db_conn.commit()

