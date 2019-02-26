class Customer():
    def __init__(self, id, dl_id, email, address, name, credits, phone_number, password, cardnumber):
        self.id = id
        self.dl_id = dl_id
        self.email = email
        self.address = address
        self.name = name
        self.credits = credits
        self.phone_number = phone_number
        self.password = password
        self.cardnumber = cardnumber


def insertCustomer(db_conn, ID, EMAIL, ADDRESS, PHONENUM, NAME, CREDITS, PASSWORD, CARDNUMBER):
    sql = "INSERT INTO CUSTOMERS (DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER) VALUES(%s, %s, %s, %s, %s, %s, %s, %s)"
    val = (ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUM, PASSWORD, CARDNUMBER)
    curs = db_conn.cursor()
    curs.execute(sql, val)
    db_conn.commit()
    print(curs.rowcount, "record inserted.")


def getCustomerByBarcode(db_conn, barcode_id):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
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

def getCustomerByEmail(db_conn, email):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
                   FROM CUSTOMERS WHERE EMAIL=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [email])
    for row in curs:
        return Customer(*row)

def getCustomerByID(db_conn, id):
    query = """SELECT ID, DL_ID, EMAIL, ADDRESS, NAME, CREDITS, PHONENUMBER, USERPASS, CARDNUMBER
                   FROM CUSTOMERS WHERE ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [id])
    for row in curs:
         return Customer(*row)
        
def updateCredits(db_conn, id, newValue):
    print(newValue)
    query = """UPDATE CUSTOMERS SET CREDITS = %s WHERE ID = %s"""
    curs = db_conn.cursor()
    curs.execute(query, [newValue, id])
    db_conn.commit()

